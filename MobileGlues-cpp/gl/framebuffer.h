// MobileGlues - gl/framebuffer.h
// Copyright (c) 2025-2026 MobileGL-Dev
// Licensed under the GNU Lesser General Public License v2.1:
//   https://www.gnu.org/licenses/old-licenses/lgpl-2.1.txt
// SPDX-License-Identifier: LGPL-2.1-only
// End of Source File Header

#ifndef MOBILEGLUES_FRAMEBUFFER_H
#define MOBILEGLUES_FRAMEBUFFER_H

#include <vector>
#include <GL/gl.h>
#include <cstddef>

struct attachment_t {
    GLenum textarget;
    GLuint texture;
    GLint level;
};
struct framebuffer_t {
    bool initialized = false;
    bool color_attachments_all_none = false;
    // A vector rather than a raw new[]: these records live in a std::vector that
    // reallocates as framebuffer names grow, and the struct has no destructor or
    // copy control, so an owning raw pointer here both leaked every array it ever
    // allocated and made the type unsafe to copy. Indexing syntax is unchanged.
    std::vector<attachment_t> color_attachments;
    attachment_t depth_attachment = {0};
    attachment_t stencil_attachment = {0};
};

#ifdef __cplusplus
extern "C"
{
#endif

    GLint getMaxDrawBuffers();

    GLAPI GLAPIENTRY void glBindFramebuffer(GLenum target, GLuint framebuffer);
    GLAPI GLAPIENTRY void glFramebufferTexture2D(GLenum target, GLenum attachment, GLenum textarget, GLuint texture,
                                                 GLint level);
    GLAPI GLAPIENTRY void glFramebufferTexture(GLenum target, GLenum attachment, GLuint texture, GLint level);
    GLAPI GLAPIENTRY void glDrawBuffer(GLenum buf);
    GLAPI GLAPIENTRY void glDrawBuffers(GLsizei n, const GLenum* bufs);
    GLAPI GLAPIENTRY void glReadBuffer(GLenum src);
    GLAPI GLAPIENTRY GLenum glCheckFramebufferStatus(GLenum target);

#ifdef __cplusplus
}
#endif

void InitFramebufferMap(size_t expectedSize);

// The framebuffer table of the current context. It used to be a plain global
// that gl/gl.cpp reached by extern; it is per-context now, so it has to be
// fetched rather than named.
// True when the draw framebuffer currently bound in *this* context has had every
// colour attachment set to GL_NONE. Answers false for an id this context has
// never bound, which the caller could not check when it indexed the table
// directly from another translation unit.
bool mg_draw_framebuffer_all_none();

#endif // MOBILEGLUES_FRAMEBUFFER_H
