// MobileGlues - gl/transfer.h
// Copyright (c) 2025-2026 MobileGL-Dev
// Licensed under the GNU Lesser General Public License v2.1:
//   https://www.gnu.org/licenses/old-licenses/lgpl-2.1.txt
// SPDX-License-Identifier: LGPL-2.1-only
// End of Source File Header

#ifndef MOBILEGLUES_TRANSFER_H
#define MOBILEGLUES_TRANSFER_H

#include "../includes.h"
#include <GL/gl.h>

// ---------------------------------------------------------------------------
// Pixel-transfer formats GLES does not have
//
// Desktop GL accepts GL_BGRA / GL_BGR as *client* formats: the driver reorders
// the data during the transfer and the texture stores ordinary RGBA. GLES has
// no reversed client formats and none of the packed GL_UNSIGNED_INT_8_8_8_8*
// types -- the pair Minecraft uploads every texture with.
//
// GL_EXT_texture_format_BGRA8888 is not a substitute. It makes BGRA an
// *internalformat*, which forbids exactly the combination desktop applications
// use (BGRA client data into a GL_RGBA8 texture), still has no packed types,
// and what it may render to or mipmap differs per driver.
//
// So the reordering desktop GL does in the driver happens here on the CPU: the
// client data is converted at the transfer boundary, honouring the full
// unpack state including a bound unpack PBO, and GLES only ever sees formats
// it has. This replaces the previous approach -- renaming the enum and
// patching the damage with a texture swizzle -- which mends sampling only, and
// only until the application mixes upload formats on one texture, renders
// into it, or sets a swizzle of its own.
// ---------------------------------------------------------------------------

#ifdef __cplusplus

// Rewrites one upload's (format, type, pixels) into a form GLES accepts.
//
//   mg_upload_fix_t fix(width, height, depth, format, type, pixels);
//   GLES.glTexSubImage2D(target, level, x, y, width, height,
//                        fix.format, fix.type, fix.pixels);
//
// For combinations GLES understands it does nothing at all. When it converts,
// the emitted stream is tightly packed, so the unpack parameters already
// applied on the way in are zeroed for the driver call; the destructor
// restores every piece of state it touched, including the unpack PBO binding.
struct mg_upload_fix_t {
    GLenum format;
    GLenum type;
    const void* pixels;

    mg_upload_fix_t(GLsizei width, GLsizei height, GLsizei depth, GLenum format_in, GLenum type_in,
                    const void* pixels_in);
    ~mg_upload_fix_t();

    mg_upload_fix_t(const mg_upload_fix_t&) = delete;
    mg_upload_fix_t& operator=(const mg_upload_fix_t&) = delete;

  private:
    bool converted_ = false;
    bool pbo_unbound_ = false;
    GLint prev_pbo_ = 0;
    GLint prev_align_ = 4, prev_row_len_ = 0, prev_skip_rows_ = 0, prev_skip_px_ = 0;
    GLint prev_img_h_ = 0, prev_skip_img_ = 0;
};

#endif // __cplusplus

#ifdef __cplusplus
extern "C"
{
#endif

    // Performs a glReadPixels whose (format, type) pair GLES lacks, encoding the
    // driver's RGBA readback into the layout the application asked for --
    // honouring the pack state, including a bound pack PBO. Returns false when
    // the pair is one GLES accepts, in which case the caller does its normal
    // read.
    bool mg_transfer_readback(GLint x, GLint y, GLsizei width, GLsizei height, GLenum format, GLenum type,
                              void* pixels);

#ifdef __cplusplus
}
#endif

#endif // MOBILEGLUES_TRANSFER_H
