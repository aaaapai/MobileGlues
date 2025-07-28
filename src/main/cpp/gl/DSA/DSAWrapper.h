#define GL_GLEXT_PROTOTYPES
#include <GL/gl.h>
#include <regex.h>
#include "../log.h"
#include "../shader.h"
#include "../program.h"
#include "../buffer.h"
#include <regex>
#include <cstring>
#include <iostream>
#include "../../config/settings.h"
#include <ankerl/unordered_dense.h>
#include "../drawing.h"

extern "C" {

       struct attachment_t {
              GLenum textarget;
              GLuint texture;
              GLint level;
       };

       struct framebuffer_t {
              GLenum current_target;
              struct attachment_t* draw_attachment;
              struct attachment_t* read_attachment;
       };
       extern struct framebuffer_t* bound_framebuffer;


	/* Transform Feedback object functions */
	GLAPI GLAPIENTRY void glCreateTransformFeedbacks(GLsizei n, GLuint* ids);
	GLAPI GLAPIENTRY void glTransformFeedbackBufferBase(GLuint xfb, GLuint index, GLuint buffer);
	GLAPI GLAPIENTRY void glTransformFeedbackBufferRange(GLuint xfb, GLuint index, GLuint buffer, GLintptr offset, GLsizeiptr size);
	GLAPI GLAPIENTRY void glGetTransformFeedbackiv(GLuint xfb, GLenum pname, GLint* param);
	GLAPI GLAPIENTRY void glGetTransformFeedbacki_v(GLuint xfb, GLenum pname, GLuint index, GLint* param);
	GLAPI GLAPIENTRY void glGetTransformFeedbacki64_v(GLuint xfb, GLenum pname, GLuint index, GLint64* param);

	/* Buffer object functions */
	GLAPI GLAPIENTRY void glCreateBuffers(GLsizei n, GLuint* buffers);
	GLAPI GLAPIENTRY void glNamedBufferStorage(GLuint buffer, GLsizeiptr size, const void* data, GLbitfield flags);
	GLAPI GLAPIENTRY void glNamedBufferData(GLuint buffer, GLsizeiptr size, const void* data, GLenum usage);
	GLAPI GLAPIENTRY void glNamedBufferSubData(GLuint buffer, GLintptr offset, GLsizeiptr size, const void* data);
	GLAPI GLAPIENTRY void glCopyNamedBufferSubData(GLuint readBuffer, GLuint writeBuffer, GLintptr readOffset, GLintptr writeOffset, GLsizeiptr size);
        GLAPI GLAPIENTRY void glClearBufferData (GLenum target, GLenum internalformat, GLenum format, GLenum type, const void *data);
	GLAPI GLAPIENTRY void glClearNamedBufferData(GLuint buffer, GLenum internalformat, GLenum format, GLenum type, const void* data);
        GLAPI GLAPIENTRY void glClearBufferSubData (GLenum target, GLenum internalformat, GLintptr offset, GLsizeiptr size, GLenum format, GLenum type, const void *data);
	GLAPI GLAPIENTRY void glClearNamedBufferSubData(GLuint buffer, GLenum internalformat, GLintptr offset, GLsizeiptr size, GLenum format, GLenum type, const void* data);
	GLAPI GLAPIENTRY void* glMapNamedBuffer(GLuint buffer, GLenum access);
	GLAPI GLAPIENTRY void* glMapNamedBufferRange(GLuint buffer, GLintptr offset, GLsizeiptr length, GLbitfield access);
	GLAPI GLAPIENTRY GLboolean glUnmapNamedBuffer(GLuint buffer);
	GLAPI GLAPIENTRY void glFlushMappedNamedBufferRange(GLuint buffer, GLintptr offset, GLsizeiptr length);
	GLAPI GLAPIENTRY void glGetNamedBufferParameteriv(GLuint buffer, GLenum pname, GLint* params);
	GLAPI GLAPIENTRY void glGetNamedBufferParameteri64v(GLuint buffer, GLenum pname, GLint64* params);
	GLAPI GLAPIENTRY void glGetNamedBufferPointerv(GLuint buffer, GLenum pname, void** params);
	GLAPI GLAPIENTRY void glGetNamedBufferSubData(GLuint buffer, GLintptr offset, GLsizeiptr size, void* data);

	/* Framebuffer object functions */
	GLAPI GLAPIENTRY void glCreateFramebuffers(GLsizei n, GLuint* framebuffers);
	GLAPI GLAPIENTRY void glNamedFramebufferRenderbuffer(GLuint framebuffer, GLenum attachment, GLenum renderbuffertarget, GLuint renderbuffer);
	GLAPI GLAPIENTRY void glNamedFramebufferParameteri(GLuint framebuffer, GLenum pname, GLint param);
        GLAPI GLAPIENTRY void glFramebufferTexture(GLenum target, GLenum attachment, GLuint texture, GLint level);
	GLAPI GLAPIENTRY void glNamedFramebufferTexture(GLuint framebuffer, GLenum attachment, GLuint texture, GLint level);
	GLAPI GLAPIENTRY void glNamedFramebufferTextureLayer(GLuint framebuffer, GLenum attachment, GLuint texture, GLint level, GLint layer);
	GLAPI GLAPIENTRY void glNamedFramebufferDrawBuffer(GLuint framebuffer, GLenum mode);
	GLAPI GLAPIENTRY void glNamedFramebufferDrawBuffers(GLuint framebuffer, GLsizei n, const GLenum* bufs);
	GLAPI GLAPIENTRY void glNamedFramebufferReadBuffer(GLuint framebuffer, GLenum mode);
	GLAPI GLAPIENTRY void glInvalidateNamedFramebufferData(GLuint framebuffer, GLsizei numAttachments, const GLenum* attachments);
	GLAPI GLAPIENTRY void glInvalidateNamedFramebufferSubData(GLuint framebuffer, GLsizei numAttachments, const GLenum* attachments, GLint x, GLint y, GLsizei width, GLsizei height);
	GLAPI GLAPIENTRY void glClearNamedFramebufferiv(GLuint framebuffer, GLenum buffer, GLint drawbuffer, const GLint* value);
	GLAPI GLAPIENTRY void glClearNamedFramebufferuiv(GLuint framebuffer, GLenum buffer, GLint drawbuffer, const GLuint* value);
	GLAPI GLAPIENTRY void glClearNamedFramebufferfv(GLuint framebuffer, GLenum buffer, GLint drawbuffer, const GLfloat* value);
	GLAPI GLAPIENTRY void glClearNamedFramebufferfi(GLuint framebuffer, GLenum buffer, GLint drawbuffer, GLfloat depth, GLint stencil);
	GLAPI GLAPIENTRY void glBlitNamedFramebuffer(GLuint readFramebuffer, GLuint drawFramebuffer, GLint srcX0, GLint srcY0, GLint srcX1, GLint srcY1, GLint dstX0, GLint dstY0, GLint dstX1, GLint dstY1, GLbitfield mask, GLenum filter);
	GLAPI GLAPIENTRY GLenum glCheckNamedFramebufferStatus(GLuint framebuffer, GLenum target);
	GLAPI GLAPIENTRY void glGetNamedFramebufferParameteriv(GLuint framebuffer, GLenum pname, GLint* param);
	GLAPI GLAPIENTRY void glGetNamedFramebufferAttachmentParameteriv(GLuint framebuffer, GLenum attachment, GLenum pname, GLint* params);

	/* Renderbuffer object functions */
	GLAPI GLAPIENTRY void glCreateRenderbuffers(GLsizei n, GLuint* renderbuffers);
	GLAPI GLAPIENTRY void glNamedRenderbufferStorage(GLuint renderbuffer, GLenum internalformat, GLsizei width, GLsizei height);
	GLAPI GLAPIENTRY void glNamedRenderbufferStorageMultisample(GLuint renderbuffer, GLsizei samples, GLenum internalformat, GLsizei width, GLsizei height);
	GLAPI GLAPIENTRY void glGetNamedRenderbufferParameteriv(GLuint renderbuffer, GLenum pname, GLint* params);

	/* Texture object functions */
	GLAPI GLAPIENTRY void glCreateTextures(GLenum target, GLsizei n, GLuint* textures);
	GLAPI GLAPIENTRY void glTextureBuffer(GLuint texture, GLenum internalformat, GLuint buffer);
	GLAPI GLAPIENTRY void glTextureBufferRange(GLuint texture, GLenum internalformat, GLuint buffer, GLintptr offset, GLsizeiptr size);
	GLAPI GLAPIENTRY void glTextureStorage1D(GLuint texture, GLsizei levels, GLenum internalformat, GLsizei width);
	GLAPI GLAPIENTRY void glTextureStorage2D(GLuint texture, GLsizei levels, GLenum internalformat, GLsizei width, GLsizei height);
	GLAPI GLAPIENTRY void glTextureStorage3D(GLuint texture, GLsizei levels, GLenum internalformat, GLsizei width, GLsizei height, GLsizei depth);
	GLAPI GLAPIENTRY void glTextureStorage2DMultisample(GLuint texture, GLsizei samples, GLenum internalformat, GLsizei width, GLsizei height, GLboolean fixedsamplelocations);
	GLAPI GLAPIENTRY void glTextureStorage3DMultisample(GLuint texture, GLsizei samples, GLenum internalformat, GLsizei width, GLsizei height, GLsizei depth, GLboolean fixedsamplelocations);
	GLAPI GLAPIENTRY void glTextureSubImage1D(GLuint texture, GLint level, GLint xoffset, GLsizei width, GLenum format, GLenum type, const void* pixels);
	GLAPI GLAPIENTRY void glTextureSubImage2D(GLuint texture, GLint level, GLint xoffset, GLint yoffset, GLsizei width, GLsizei height, GLenum format, GLenum type, const void* pixels);
	GLAPI GLAPIENTRY void glTextureSubImage3D(GLuint texture, GLint level, GLint xoffset, GLint yoffset, GLint zoffset, GLsizei width, GLsizei height, GLsizei depth, GLenum format, GLenum type, const void* pixels);
	GLAPI GLAPIENTRY void glCompressedTextureSubImage1D(GLuint texture, GLint level, GLint xoffset, GLsizei width, GLenum format, GLsizei imageSize, const void* data);
	GLAPI GLAPIENTRY void glCompressedTextureSubImage2D(GLuint texture, GLint level, GLint xoffset, GLint yoffset, GLsizei width, GLsizei height, GLenum format, GLsizei imageSize, const void* data);
	GLAPI GLAPIENTRY void glCompressedTextureSubImage3D(GLuint texture, GLint level, GLint xoffset, GLint yoffset, GLint zoffset, GLsizei width, GLsizei height, GLsizei depth, GLenum format, GLsizei imageSize, const void* data);
	GLAPI GLAPIENTRY void glCopyTextureSubImage1D(GLuint texture, GLint level, GLint xoffset, GLint x, GLint y, GLsizei width);
	GLAPI GLAPIENTRY void glCopyTextureSubImage2D(GLuint texture, GLint level, GLint xoffset, GLint yoffset, GLint x, GLint y, GLsizei width, GLsizei height);
	GLAPI GLAPIENTRY void glCopyTextureSubImage3D(GLuint texture, GLint level, GLint xoffset, GLint yoffset, GLint zoffset, GLint x, GLint y, GLsizei width, GLsizei height);
	GLAPI GLAPIENTRY void glTextureParameterf(GLuint texture, GLenum pname, GLfloat param);
	GLAPI GLAPIENTRY void glTextureParameterfv(GLuint texture, GLenum pname, const GLfloat* param);
	GLAPI GLAPIENTRY void glTextureParameteri(GLuint texture, GLenum pname, GLint param);
	GLAPI GLAPIENTRY void glTextureParameterIiv(GLuint texture, GLenum pname, const GLint* params);
	GLAPI GLAPIENTRY void glTextureParameterIuiv(GLuint texture, GLenum pname, const GLuint* params);
	GLAPI GLAPIENTRY void glTextureParameteriv(GLuint texture, GLenum pname, const GLint* param);
	GLAPI GLAPIENTRY void glGenerateTextureMipmap(GLuint texture);
	GLAPI GLAPIENTRY void glBindTextureUnit(GLuint unit, GLuint texture);
	GLAPI GLAPIENTRY void glGetTextureImage(GLuint texture, GLint level, GLenum format, GLenum type, GLsizei bufSize, void* pixels);
	GLAPI GLAPIENTRY void glGetCompressedTextureImage(GLuint texture, GLint level, GLsizei bufSize, void* pixels);
	GLAPI GLAPIENTRY void glGetTextureLevelParameterfv(GLuint texture, GLint level, GLenum pname, GLfloat* params);
	GLAPI GLAPIENTRY void glGetTextureLevelParameteriv(GLuint texture, GLint level, GLenum pname, GLint* params);
	GLAPI GLAPIENTRY void glGetTextureParameterfv(GLuint texture, GLenum pname, GLfloat* params);
	GLAPI GLAPIENTRY void glGetTextureParameterIiv(GLuint texture, GLenum pname, GLint* params);
	GLAPI GLAPIENTRY void glGetTextureParameterIuiv(GLuint texture, GLenum pname, GLuint* params);
	GLAPI GLAPIENTRY void glGetTextureParameteriv(GLuint texture, GLenum pname, GLint* params);

	/* Vertex Array object functions */
	GLAPI GLAPIENTRY void glCreateVertexArrays(GLsizei n, GLuint* arrays);
	GLAPI GLAPIENTRY void glDisableVertexArrayAttrib(GLuint vaobj, GLuint index);
	GLAPI GLAPIENTRY void glEnableVertexArrayAttrib(GLuint vaobj, GLuint index);
	GLAPI GLAPIENTRY void glVertexArrayElementBuffer(GLuint vaobj, GLuint buffer);
	GLAPI GLAPIENTRY void glVertexArrayVertexBuffer(GLuint vaobj, GLuint bindingindex, GLuint buffer, GLintptr offset, GLsizei stride);
	GLAPI GLAPIENTRY void glVertexArrayVertexBuffers(GLuint vaobj, GLuint first, GLsizei count, const GLuint* buffers, const GLintptr* offsets, const GLsizei* strides);
	GLAPI GLAPIENTRY void glVertexArrayAttribFormat(GLuint vaobj, GLuint attribindex, GLint size, GLenum type, GLboolean normalized, GLuint relativeoffset);
	GLAPI GLAPIENTRY void glVertexArrayAttribIFormat(GLuint vaobj, GLuint attribindex, GLint size, GLenum type, GLuint relativeoffset);
	GLAPI GLAPIENTRY void glVertexArrayAttribLFormat(GLuint vaobj, GLuint attribindex, GLint size, GLenum type, GLuint relativeoffset);
	GLAPI GLAPIENTRY void glVertexArrayAttribBinding(GLuint vaobj, GLuint attribindex, GLuint bindingindex);
	GLAPI GLAPIENTRY void glVertexArrayBindingDivisor(GLuint vaobj, GLuint bindingindex, GLuint divisor);
	GLAPI GLAPIENTRY void glGetVertexArrayiv(GLuint vaobj, GLenum pname, GLint* param);
	GLAPI GLAPIENTRY void glGetVertexArrayIndexediv(GLuint vaobj, GLuint index, GLenum pname, GLint* param);
	GLAPI GLAPIENTRY void glGetVertexArrayIndexed64iv(GLuint vaobj, GLuint index, GLenum pname, GLint64* param);

	/* Sampler object functions */
	GLAPI GLAPIENTRY void glCreateSamplers(GLsizei n, GLuint* samplers);

	/* Program Pipeline object functions */
	GLAPI GLAPIENTRY void glCreateProgramPipelines(GLsizei n, GLuint* pipelines);

	/* Query object functions */

	GLAPI GLAPIENTRY void glCreateQueries(GLenum target, GLsizei n, GLuint* ids);
	GLAPI GLAPIENTRY void glGetQueryBufferObjectiv(GLuint id, GLuint buffer, GLenum pname, GLintptr offset);
	GLAPI GLAPIENTRY void glGetQueryBufferObjectuiv(GLuint id, GLuint buffer, GLenum pname, GLintptr offset);
	GLAPI GLAPIENTRY void glGetQueryBufferObjecti64v(GLuint id, GLuint buffer, GLenum pname, GLintptr offset);
	GLAPI GLAPIENTRY void glGetQueryBufferObjectui64v(GLuint id, GLuint buffer, GLenum pname, GLintptr offset);
}
