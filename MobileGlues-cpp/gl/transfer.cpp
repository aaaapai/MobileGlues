// MobileGlues - gl/transfer.cpp
// Copyright (c) 2025-2026 MobileGL-Dev
// Licensed under the GNU Lesser General Public License v2.1:
//   https://www.gnu.org/licenses/old-licenses/lgpl-2.1.txt
// SPDX-License-Identifier: LGPL-2.1-only
// End of Source File Header

#include "transfer.h"
#include "log.h"
#include "mg.h"
#include "pixel.h"
#include "../gles/loader.h"
#include <cstring>
#include <vector>

#define DEBUG 0

// GL 3.0 enums that live in glext.h, which not every include path pulls in.
#ifndef GL_BGR_INTEGER
#define GL_BGR_INTEGER 0x8D9A
#endif
#ifndef GL_BGRA_INTEGER
#define GL_BGRA_INTEGER 0x8D9B
#endif

#define TR_WARN_ONCE(...)                                                                                              \
    do {                                                                                                               \
        static bool mg_tr_warned = false;                                                                              \
        if (!mg_tr_warned) {                                                                                           \
            mg_tr_warned = true;                                                                                       \
            LOG_W_FORCE(__VA_ARGS__)                                                                                   \
        }                                                                                                              \
    } while (0)

namespace {

// One scratch per thread: uploads can happen on any thread that holds a
// context, and nothing here takes a lock.
std::vector<uint8_t>& scratch() {
    static thread_local std::vector<uint8_t> buf;
    return buf;
}

// ---------------------------------------------------------------------------
// Decoders. Each reads one source pixel and writes RGBA (or RGB) bytes in
// memory order. The packed GL_UNSIGNED_INT_* types are defined as components
// of a host-endian integer -- non-REV packs the format's first component into
// the most significant bits, REV into the least -- so decoding through an
// integer load is endian-correct by construction. Only GL_UNSIGNED_BYTE
// addresses memory directly, and its component order *is* memory order.
// ---------------------------------------------------------------------------

void dec_bgra_u8(const uint8_t* s, uint8_t* d) {
    d[0] = s[2];
    d[1] = s[1];
    d[2] = s[0];
    d[3] = s[3];
}

void dec_bgr_u8(const uint8_t* s, uint8_t* d) {
    d[0] = s[2];
    d[1] = s[1];
    d[2] = s[0];
}

void dec_bgra_8888(const uint8_t* s, uint8_t* d) { // B=31..24 G=23..16 R=15..8 A=7..0
    uint32_t v;
    memcpy(&v, s, 4);
    d[0] = (v >> 8) & 0xff;
    d[1] = (v >> 16) & 0xff;
    d[2] = (v >> 24) & 0xff;
    d[3] = v & 0xff;
}

void dec_bgra_8888_rev(const uint8_t* s, uint8_t* d) { // B=7..0 G=15..8 R=23..16 A=31..24
    uint32_t v;
    memcpy(&v, s, 4);
    d[0] = (v >> 16) & 0xff;
    d[1] = (v >> 8) & 0xff;
    d[2] = v & 0xff;
    d[3] = (v >> 24) & 0xff;
}

void dec_rgba_8888(const uint8_t* s, uint8_t* d) { // R=31..24 G=23..16 B=15..8 A=7..0
    uint32_t v;
    memcpy(&v, s, 4);
    d[0] = (v >> 24) & 0xff;
    d[1] = (v >> 16) & 0xff;
    d[2] = (v >> 8) & 0xff;
    d[3] = v & 0xff;
}

void dec_rgba_8888_rev(const uint8_t* s, uint8_t* d) { // R=7..0 ... A=31..24
    uint32_t v;
    memcpy(&v, s, 4);
    d[0] = v & 0xff;
    d[1] = (v >> 8) & 0xff;
    d[2] = (v >> 16) & 0xff;
    d[3] = (v >> 24) & 0xff;
}

void dec_bgra_1555_rev(const uint8_t* s, uint8_t* d) { // B=4..0 G=9..5 R=14..10 A=15
    uint16_t v;
    memcpy(&v, s, 2);
    const uint8_t b = v & 0x1f, g = (v >> 5) & 0x1f, r = (v >> 10) & 0x1f;
    d[0] = (r << 3) | (r >> 2);
    d[1] = (g << 3) | (g >> 2);
    d[2] = (b << 3) | (b >> 2);
    d[3] = (v & 0x8000) ? 255 : 0;
}

void dec_bgra_4444_rev(const uint8_t* s, uint8_t* d) { // B=3..0 G=7..4 R=11..8 A=15..12
    uint16_t v;
    memcpy(&v, s, 2);
    d[0] = ((v >> 8) & 0x0f) * 17;
    d[1] = ((v >> 4) & 0x0f) * 17;
    d[2] = (v & 0x0f) * 17;
    d[3] = ((v >> 12) & 0x0f) * 17;
}

struct upload_rule_t {
    GLenum format, type;
    void (*dec)(const uint8_t*, uint8_t*);
    int src_size;
    GLenum out_format;
    int out_size;
};

const upload_rule_t k_upload_rules[] = {
    {GL_BGRA, GL_UNSIGNED_BYTE, dec_bgra_u8, 4, GL_RGBA, 4},
    {GL_BGRA, GL_UNSIGNED_INT_8_8_8_8_REV, dec_bgra_8888_rev, 4, GL_RGBA, 4},
    {GL_BGRA, GL_UNSIGNED_INT_8_8_8_8, dec_bgra_8888, 4, GL_RGBA, 4},
    {GL_BGRA, GL_UNSIGNED_SHORT_1_5_5_5_REV, dec_bgra_1555_rev, 2, GL_RGBA, 4},
    {GL_BGRA, GL_UNSIGNED_SHORT_4_4_4_4_REV, dec_bgra_4444_rev, 2, GL_RGBA, 4},
    {GL_BGR, GL_UNSIGNED_BYTE, dec_bgr_u8, 3, GL_RGB, 3},
    // Not reversed, but the packed 8888 types themselves do not exist in GLES.
    {GL_RGBA, GL_UNSIGNED_INT_8_8_8_8_REV, dec_rgba_8888_rev, 4, GL_RGBA, 4},
    {GL_RGBA, GL_UNSIGNED_INT_8_8_8_8, dec_rgba_8888, 4, GL_RGBA, 4},
};

const upload_rule_t* find_upload_rule(GLenum format, GLenum type) {
    for (const auto& r : k_upload_rules) {
        if (r.format == format && r.type == type) return &r;
    }
    return nullptr;
}

bool is_reversed_family(GLenum format, GLenum type) {
    return format == GL_BGRA || format == GL_BGR || format == GL_BGRA_INTEGER || format == GL_BGR_INTEGER ||
           type == GL_UNSIGNED_INT_8_8_8_8 || type == GL_UNSIGNED_INT_8_8_8_8_REV;
}

} // namespace

mg_upload_fix_t::mg_upload_fix_t(GLsizei width, GLsizei height, GLsizei depth, GLenum format_in, GLenum type_in,
                                 const void* pixels_in)
    : format(format_in), type(type_in), pixels(pixels_in) {
    const upload_rule_t* rule = find_upload_rule(format_in, type_in);
    if (!rule) {
        if (is_reversed_family(format_in, type_in)) {
            TR_WARN_ONCE("pixel transfer: unhandled combination %s + %s, the driver will reject this upload",
                         glEnumToString(format_in), glEnumToString(type_in));
        }
        return;
    }

    format = rule->out_format;
    type = GL_UNSIGNED_BYTE;

    // With an unpack PBO bound, pixels is an offset -- and offset 0 is real
    // data, so nullptr only means "no data" when no PBO is bound.
    GLES.glGetIntegerv(GL_PIXEL_UNPACK_BUFFER_BINDING, &prev_pbo_);
    if (pixels_in == nullptr && prev_pbo_ == 0) return; // allocation only: the enums were all that needed fixing
    if (width <= 0 || height <= 0 || depth <= 0) return;

    GLES.glGetIntegerv(GL_UNPACK_ALIGNMENT, &prev_align_);
    GLES.glGetIntegerv(GL_UNPACK_ROW_LENGTH, &prev_row_len_);
    GLES.glGetIntegerv(GL_UNPACK_SKIP_ROWS, &prev_skip_rows_);
    GLES.glGetIntegerv(GL_UNPACK_SKIP_PIXELS, &prev_skip_px_);
    GLES.glGetIntegerv(GL_UNPACK_IMAGE_HEIGHT, &prev_img_h_);
    GLES.glGetIntegerv(GL_UNPACK_SKIP_IMAGES, &prev_skip_img_);

    const size_t ss = static_cast<size_t>(rule->src_size);
    const size_t row_px = prev_row_len_ > 0 ? static_cast<size_t>(prev_row_len_) : static_cast<size_t>(width);
    const size_t row_stride = widthalign(row_px * ss, static_cast<size_t>(prev_align_));
    const size_t img_rows = prev_img_h_ > 0 ? static_cast<size_t>(prev_img_h_) : static_cast<size_t>(height);
    const size_t img_stride = row_stride * img_rows;
    const size_t start = static_cast<size_t>(prev_skip_img_) * img_stride +
                         static_cast<size_t>(prev_skip_rows_) * row_stride + static_cast<size_t>(prev_skip_px_) * ss;
    const size_t span = start + static_cast<size_t>(depth - 1) * img_stride +
                        static_cast<size_t>(height - 1) * row_stride + static_cast<size_t>(width) * ss;

    const uint8_t* src = nullptr;
    void* mapped = nullptr;
    if (prev_pbo_ != 0) {
        mapped = GLES.glMapBufferRange(GL_PIXEL_UNPACK_BUFFER, static_cast<GLintptr>(reinterpret_cast<uintptr_t>(pixels_in)),
                                       static_cast<GLsizeiptr>(span), GL_MAP_READ_BIT);
        if (!mapped) {
            // Wrong colours are worse than a missing upload: dropping keeps the
            // failure visible instead of shipping swapped channels.
            TR_WARN_ONCE("pixel transfer: unpack buffer %d is not mappable for reading, upload dropped", prev_pbo_);
            GLES.glBindBuffer(GL_PIXEL_UNPACK_BUFFER, 0);
            pbo_unbound_ = true;
            pixels = nullptr;
            return;
        }
        src = static_cast<const uint8_t*>(mapped);
    } else {
        src = static_cast<const uint8_t*>(pixels_in);
    }

    scratch().resize(static_cast<size_t>(width) * static_cast<size_t>(height) * static_cast<size_t>(depth) *
                     static_cast<size_t>(rule->out_size));
    uint8_t* out = scratch().data();
    for (GLsizei z = 0; z < depth; ++z) {
        for (GLsizei row = 0; row < height; ++row) {
            const uint8_t* s = src + start + static_cast<size_t>(z) * img_stride + static_cast<size_t>(row) * row_stride;
            for (GLsizei col = 0; col < width; ++col) {
                rule->dec(s, out);
                s += ss;
                out += rule->out_size;
            }
        }
    }

    if (mapped) {
        GLES.glUnmapBuffer(GL_PIXEL_UNPACK_BUFFER);
        GLES.glBindBuffer(GL_PIXEL_UNPACK_BUFFER, 0);
        pbo_unbound_ = true;
    }

    // The converted stream is tight and the skips are already applied.
    GLES.glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
    GLES.glPixelStorei(GL_UNPACK_ROW_LENGTH, 0);
    GLES.glPixelStorei(GL_UNPACK_SKIP_ROWS, 0);
    GLES.glPixelStorei(GL_UNPACK_SKIP_PIXELS, 0);
    GLES.glPixelStorei(GL_UNPACK_IMAGE_HEIGHT, 0);
    GLES.glPixelStorei(GL_UNPACK_SKIP_IMAGES, 0);
    converted_ = true;
    pixels = scratch().data();
}

mg_upload_fix_t::~mg_upload_fix_t() {
    if (converted_) {
        GLES.glPixelStorei(GL_UNPACK_ALIGNMENT, prev_align_);
        GLES.glPixelStorei(GL_UNPACK_ROW_LENGTH, prev_row_len_);
        GLES.glPixelStorei(GL_UNPACK_SKIP_ROWS, prev_skip_rows_);
        GLES.glPixelStorei(GL_UNPACK_SKIP_PIXELS, prev_skip_px_);
        GLES.glPixelStorei(GL_UNPACK_IMAGE_HEIGHT, prev_img_h_);
        GLES.glPixelStorei(GL_UNPACK_SKIP_IMAGES, prev_skip_img_);
    }
    if (pbo_unbound_) {
        GLES.glBindBuffer(GL_PIXEL_UNPACK_BUFFER, static_cast<GLuint>(prev_pbo_));
    }
}

// ---------------------------------------------------------------------------
// Readback
// ---------------------------------------------------------------------------

namespace {

// Encoders: RGBA memory-order bytes in, one destination pixel out.
void enc_bgra_u8(const uint8_t* s, uint8_t* d) {
    d[0] = s[2];
    d[1] = s[1];
    d[2] = s[0];
    d[3] = s[3];
}

void enc_bgr_u8(const uint8_t* s, uint8_t* d) {
    d[0] = s[2];
    d[1] = s[1];
    d[2] = s[0];
}

void enc_bgra_8888(const uint8_t* s, uint8_t* d) {
    const uint32_t v = (static_cast<uint32_t>(s[2]) << 24) | (static_cast<uint32_t>(s[1]) << 16) |
                       (static_cast<uint32_t>(s[0]) << 8) | s[3];
    memcpy(d, &v, 4);
}

void enc_bgra_8888_rev(const uint8_t* s, uint8_t* d) {
    const uint32_t v = s[2] | (static_cast<uint32_t>(s[1]) << 8) | (static_cast<uint32_t>(s[0]) << 16) |
                       (static_cast<uint32_t>(s[3]) << 24);
    memcpy(d, &v, 4);
}

void enc_rgba_8888(const uint8_t* s, uint8_t* d) {
    const uint32_t v = (static_cast<uint32_t>(s[0]) << 24) | (static_cast<uint32_t>(s[1]) << 16) |
                       (static_cast<uint32_t>(s[2]) << 8) | s[3];
    memcpy(d, &v, 4);
}

void enc_rgba_8888_rev(const uint8_t* s, uint8_t* d) {
    const uint32_t v = s[0] | (static_cast<uint32_t>(s[1]) << 8) | (static_cast<uint32_t>(s[2]) << 16) |
                       (static_cast<uint32_t>(s[3]) << 24);
    memcpy(d, &v, 4);
}

struct readback_rule_t {
    GLenum format, type;
    void (*enc)(const uint8_t*, uint8_t*);
    int dst_size;
};

const readback_rule_t k_readback_rules[] = {
    {GL_BGRA, GL_UNSIGNED_BYTE, enc_bgra_u8, 4},
    {GL_BGRA, GL_UNSIGNED_INT_8_8_8_8_REV, enc_bgra_8888_rev, 4},
    {GL_BGRA, GL_UNSIGNED_INT_8_8_8_8, enc_bgra_8888, 4},
    {GL_BGR, GL_UNSIGNED_BYTE, enc_bgr_u8, 3},
    {GL_RGBA, GL_UNSIGNED_INT_8_8_8_8_REV, enc_rgba_8888_rev, 4},
    {GL_RGBA, GL_UNSIGNED_INT_8_8_8_8, enc_rgba_8888, 4},
};

} // namespace

bool mg_transfer_readback(GLint x, GLint y, GLsizei width, GLsizei height, GLenum format, GLenum type, void* pixels) {
    const readback_rule_t* rule = nullptr;
    for (const auto& r : k_readback_rules) {
        if (r.format == format && r.type == type) {
            rule = &r;
            break;
        }
    }
    if (!rule) return false;
    if (width <= 0 || height <= 0) return true; // ours, and nothing to do

    GLint pbo = 0, align = 4, row_len = 0, skip_rows = 0, skip_px = 0;
    GLES.glGetIntegerv(GL_PIXEL_PACK_BUFFER_BINDING, &pbo);
    GLES.glGetIntegerv(GL_PACK_ALIGNMENT, &align);
    GLES.glGetIntegerv(GL_PACK_ROW_LENGTH, &row_len);
    GLES.glGetIntegerv(GL_PACK_SKIP_ROWS, &skip_rows);
    GLES.glGetIntegerv(GL_PACK_SKIP_PIXELS, &skip_px);

    // Read tight RGBA into scratch, with the pack state and PBO out of the way.
    scratch().resize(static_cast<size_t>(width) * static_cast<size_t>(height) * 4);
    if (pbo != 0) GLES.glBindBuffer(GL_PIXEL_PACK_BUFFER, 0);
    GLES.glPixelStorei(GL_PACK_ALIGNMENT, 1);
    GLES.glPixelStorei(GL_PACK_ROW_LENGTH, 0);
    GLES.glPixelStorei(GL_PACK_SKIP_ROWS, 0);
    GLES.glPixelStorei(GL_PACK_SKIP_PIXELS, 0);
    GLES.glReadPixels(x, y, width, height, GL_RGBA, GL_UNSIGNED_BYTE, scratch().data());
    GLES.glPixelStorei(GL_PACK_ALIGNMENT, align);
    GLES.glPixelStorei(GL_PACK_ROW_LENGTH, row_len);
    GLES.glPixelStorei(GL_PACK_SKIP_ROWS, skip_rows);
    GLES.glPixelStorei(GL_PACK_SKIP_PIXELS, skip_px);

    // Encode into the destination the application described with its pack state.
    const size_t ds = static_cast<size_t>(rule->dst_size);
    const size_t row_px = row_len > 0 ? static_cast<size_t>(row_len) : static_cast<size_t>(width);
    const size_t dst_stride = widthalign(row_px * ds, static_cast<size_t>(align));
    const size_t start = static_cast<size_t>(skip_rows) * dst_stride + static_cast<size_t>(skip_px) * ds;
    const size_t span = start + static_cast<size_t>(height - 1) * dst_stride + static_cast<size_t>(width) * ds;

    uint8_t* dst = nullptr;
    void* mapped = nullptr;
    if (pbo != 0) {
        GLES.glBindBuffer(GL_PIXEL_PACK_BUFFER, static_cast<GLuint>(pbo));
        mapped = GLES.glMapBufferRange(GL_PIXEL_PACK_BUFFER, static_cast<GLintptr>(reinterpret_cast<uintptr_t>(pixels)),
                                       static_cast<GLsizeiptr>(span), GL_MAP_WRITE_BIT);
        if (!mapped) {
            TR_WARN_ONCE("pixel transfer: pack buffer %d is not mappable for writing, readback dropped", pbo);
            return true;
        }
        dst = static_cast<uint8_t*>(mapped);
    } else {
        if (pixels == nullptr) return true;
        dst = static_cast<uint8_t*>(pixels);
    }

    const uint8_t* src = scratch().data();
    for (GLsizei row = 0; row < height; ++row) {
        uint8_t* d = dst + start + static_cast<size_t>(row) * dst_stride;
        for (GLsizei col = 0; col < width; ++col) {
            rule->enc(src, d);
            src += 4;
            d += ds;
        }
    }

    if (mapped) GLES.glUnmapBuffer(GL_PIXEL_PACK_BUFFER);
    return true;
}
