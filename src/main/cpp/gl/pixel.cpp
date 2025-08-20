//
// Created by Swung 0x48 on 2025/2/27.
//

#include "pixel.h"
#include "log.h"
#include "mg.h"

#define DEBUG 0

GLsizei gl_sizeof(GLenum type) {
  // types
  switch (type) {
  case GL_DOUBLE:
    return 8;
  case GL_FLOAT:
  case GL_INT:
  case GL_UNSIGNED_INT:
  case GL_UNSIGNED_INT_10_10_10_2:
  case GL_UNSIGNED_INT_2_10_10_10_REV:
  case GL_UNSIGNED_INT_8_8_8_8:
  case GL_UNSIGNED_INT_8_8_8_8_REV:
  case GL_UNSIGNED_INT_24_8:
  case GL_4_BYTES:
    return 4;
  case GL_3_BYTES:
    return 3;
  case GL_LUMINANCE_ALPHA:
  case GL_SHORT:
  case GL_HALF_FLOAT:
  case GL_UNSIGNED_SHORT:
  case GL_UNSIGNED_SHORT_1_5_5_5_REV:
  case GL_UNSIGNED_SHORT_4_4_4_4:
  case GL_UNSIGNED_SHORT_4_4_4_4_REV:
  case GL_UNSIGNED_SHORT_5_5_5_1:
  case GL_UNSIGNED_SHORT_5_6_5:
  case GL_UNSIGNED_SHORT_5_6_5_REV:
  case GL_2_BYTES:
    return 2;
  case GL_ALPHA:
  case GL_LUMINANCE:
  case GL_BYTE:
  case GL_UNSIGNED_BYTE:
  case GL_UNSIGNED_BYTE_2_3_3_REV:
  case GL_UNSIGNED_BYTE_3_3_2:
  case GL_DEPTH_COMPONENT:
  case GL_COLOR_INDEX:
    return 1;
  default:
    LOG_D("Unsupported pixel data type: %s\n", glEnumToString(type))
    return 0;
  }
}

GLboolean is_type_packed(GLenum type) {
  switch (type) {
  case GL_4_BYTES:
  case GL_3_BYTES:
  case GL_2_BYTES:
  case GL_UNSIGNED_BYTE_2_3_3_REV:
  case GL_UNSIGNED_BYTE_3_3_2:
  case GL_UNSIGNED_INT_10_10_10_2:
  case GL_UNSIGNED_INT_2_10_10_10_REV:
  case GL_UNSIGNED_INT_8_8_8_8:
  case GL_UNSIGNED_INT_8_8_8_8_REV:
  case GL_UNSIGNED_SHORT_1_5_5_5_REV:
  case GL_UNSIGNED_SHORT_4_4_4_4:
  case GL_UNSIGNED_SHORT_4_4_4_4_REV:
  case GL_UNSIGNED_SHORT_5_5_5_1:
  case GL_UNSIGNED_SHORT_5_6_5:
  case GL_UNSIGNED_SHORT_5_6_5_REV:
  case GL_DEPTH_STENCIL:
    return true;
  default:
    return false;
  }
}

GLsizei pixel_sizeof(GLenum format, GLenum type) {
  GLsizei width = 0;
  switch (format) {
  case GL_R:
  case GL_RED:
  case GL_ALPHA:
  case GL_LUMINANCE:
  case GL_DEPTH_COMPONENT:
  case GL_DEPTH_STENCIL:
  case GL_COLOR_INDEX:
    width = 1;
    break;
  case GL_RG:
  case GL_LUMINANCE_ALPHA:
    width = 2;
    break;
  case GL_RGB:
  case GL_BGR:
  case GL_RGB8:
    width = 3;
    break;
  case GL_RGBA:
  case GL_BGRA:
  case GL_RGBA8:
    // RGBA or BGRA with GL_UNSIGNED_INT_8_8_8_8_REV <-> GL_UNSIGNED_BYTE
    if((src_format==dst_format) && (src_format==GL_RGBA || src_format==GL_BGRA) && ((src_type==GL_UNSIGNED_INT_8_8_8_8_REV && dst_type==GL_UNSIGNED_BYTE) || (src_type==GL_UNSIGNED_BYTE && dst_type==GL_UNSIGNED_INT_8_8_8_8_REV))) {
        for (GLuint i = 0; i < height; i++) {
			for (GLuint j = 0; j < width; j++) {
				((char*)dst_pos)[0] = ((char*)src_pos)[3];
				((char*)dst_pos)[1] = ((char*)src_pos)[2];
				((char*)dst_pos)[2] = ((char*)src_pos)[1];
                ((char*)dst_pos)[3] = ((char*)src_pos)[0];
				src_pos += src_stride;
				dst_pos += dst_stride;
			}
			dst_pos += dst_width;
          
  // BGRA1555 -> RGBA5551
  if ((src_format == GL_BGRA) && (dst_format == GL_RGBA) &&
      (dst_type == GL_UNSIGNED_SHORT_5_5_5_1) &&
      (src_type == GL_UNSIGNED_SHORT_1_5_5_5_REV)) {
    GLushort tmp;
    for (int i = 0; i < height; i++) {
      for (int j = 0; j < width; j++) {
        // invert 1555/BGRA to 5551/RGBA (0x1f / 0x3e0 / 7c00)
        tmp = *(GLushort *)src_pos;
        *(GLushort *)dst_pos = ((tmp & 0x8000) >> 15) | ((tmp & 0x7fff) << 1);
        src_pos += src_stride;
        dst_pos += dst_stride;
      }
      dst_pos += dst_width;
      src_pos += src_widthadj;
    }
    return true;
  }
  // L -> RGBA
  if ((src_format == GL_LUMINANCE) && (dst_format == GL_RGBA) &&
      (dst_type == GL_UNSIGNED_BYTE) && ((src_type == GL_UNSIGNED_BYTE))) {
    for (int i = 0; i < height; i++) {
      for (int j = 0; j < width; j++) {
        // tmp = *(const GLuint*)src_pos;
       
    // BGRA1555 -> RGBA5551
    if ((src_format == GL_BGRA) && (dst_format == GL_RGBA) && (dst_type == GL_UNSIGNED_SHORT_5_5_5_1) && (src_type == GL_UNSIGNED_SHORT_1_5_5_5_REV)) {
        GLushort tmp;
        for (GLuint i = 0; i < height; i++) {
            for (GLuint j = 0; j < width; j++) {
                // invert 1555/BGRA to 5551/RGBA (0x1f / 0x3e0 / 7c00)
                tmp=*(GLushort*)src_pos;
                *(GLushort*)dst_pos = ((tmp&0x8000)>>15) | ((tmp&0x7fff)<<1);
                src_pos += src_stride;
                dst_pos += dst_stride;
            }
            dst_pos += dst_width;
            src_pos += src_widthadj;
        }
        return true;
    }
    // L -> RGBA
    if ((src_format == GL_LUMINANCE) && (dst_format == GL_RGBA) && (dst_type == GL_UNSIGNED_BYTE) && ((src_type == GL_UNSIGNED_BYTE))) {
        for (GLuint i = 0; i < height; i++) {
            for (GLuint j = 0; j < width; j++) {
     
#ifdef __BIG_ENDIAN__
        *(unsigned char *)dst_pos =
            (((int)byte_src[1]) * 77 + ((int)byte_src[2]) * 151 +
             ((int)byte_src[3]) * 28) >>
            8;
#else
        *(unsigned char *)dst_pos =
            (((int)byte_src[2]) * 77 + ((int)byte_src[1]) * 151 +
             ((int)byte_src[0]) * 28) >>
            8;
#endif
        src_pos += src_stride;
        dst_pos += dst_stride;
      }
      dst_pos += dst_width;
      src_pos += src_widthadj;
    }    // BGR(A) -> RGB
    if (((src_format == GL_BGR)||(src_format == GL_BGRA)) && (dst_format == GL_RGB) && (dst_type == GL_UNSIGNED_BYTE) && ((src_type == GL_UNSIGNED_BYTE))) {
        for (GLuint i = 0; i < height; i++) {
            for (GLuint j = 0; j < width; j++) {
                ((char*)dst_pos)[0] = ((char*)src_pos)[2];
                ((char*)dst_pos)[1] = ((char*)src_pos)[1];
                ((char*)dst_pos)[2] = ((char*)src_pos)[0];
                src_pos += src_stride;
                dst_pos += dst_stride;
            }
            dst_pos += dst_width;
            src_pos += src_widthadj;
        }
        return true;
    }
    // BGR -> RGBA
    if (((src_format == GL_BGR)) && (dst_format == GL_RGBA) && (dst_type == GL_UNSIGNED_BYTE) && ((src_type == GL_UNSIGNED_BYTE))) {
        for (GLuint i = 0; i < height; i++) {
            for (GLuint j = 0; j < width; j++) {
                ((unsigned char*)dst_pos)[0] = ((unsigned char*)src_pos)[2];
                ((unsigned char*)dst_pos)[1] = ((unsigned char*)src_pos)[1];
                ((unsigned char*)dst_pos)[2] = ((unsigned char*)src_pos)[0];
                ((unsigned char*)dst_pos)[3] = 255;
                src_pos += src_stride;
                dst_pos += dst_stride;
            }
            dst_pos += dst_width;
            src_pos += src_widthadj;
        }
        return true;
    }
    // RGBA -> RGB
    if ((src_format == GL_RGBA) && (dst_format == GL_RGB) && (dst_type == GL_UNSIGNED_BYTE) && ((src_type == GL_UNSIGNED_BYTE))) {
        for (GLuint i = 0; i < height; i++) {
            for (GLuint j = 0; j < width; j++) {
                ((char*)dst_pos)[0] = ((char*)src_pos)[0];
                ((char*)dst_pos)[1] = ((char*)src_pos)[1];
                ((char*)dst_pos)[2] = ((char*)src_pos)[2];
                src_pos += src_stride;
                dst_pos += dst_stride;
            }
            dst_pos += dst_width;
            src_pos += src_widthadj;
        }
        return true;
    }
    // RGB(A) -> RGB565
    if (((src_format == GL_RGB)||(src_format == GL_RGBA)) && (dst_format == GL_RGB) && (dst_type == GL_UNSIGNED_SHORT_5_6_5) && ((src_type == GL_UNSIGNED_BYTE))) {
        for (GLuint i = 0; i < height; i++) {
            for (GLuint j = 0; j < width; j++) {
                *(GLushort*)dst_pos = ((GLushort)(((char*)src_pos)[2]&0xf8)>>(3)) | ((GLushort)(((char*)src_pos)[1]&0xfc)<<(5-2)) | ((GLushort)(((char*)src_pos)[0]&0xf8)<<(11-3));
                src_pos += src_stride;
                dst_pos += dst_stride;
            }
            dst_pos += dst_width;
            src_pos += src_widthadj;
        }
        return true;
    }
    // BGR(A) -> RGB565
    if (((src_format == GL_BGR) || (src_format == GL_BGRA)) && (dst_format == GL_RGB) && (dst_type == GL_UNSIGNED_SHORT_5_6_5) && ((src_type == GL_UNSIGNED_BYTE))) {
        for (GLuint i = 0; i < height; i++) {
            for (GLuint j = 0; j < width; j++) {
                *(GLushort*)dst_pos = ((GLushort)(((char*)src_pos)[0]&0xf8)>>(3)) | ((GLushort)(((char*)src_pos)[1]&0xfc)<<(5-2)) | ((GLushort)(((char*)src_pos)[2]&0xf8)<<(11-3));
                src_pos += src_stride;
                dst_pos += dst_stride;
            }
            dst_pos += dst_width;
            src_pos += src_widthadj;
        }
        return true;
    }
    // RGBA -> RGBA5551
    if ((src_format == GL_RGBA) && (dst_format == GL_RGBA) && (dst_type == GL_UNSIGNED_SHORT_5_5_5_1) && ((src_type == GL_UNSIGNED_BYTE))) {
        for (GLuint i = 0; i < height; i++) {
            for (GLuint j = 0; j < width; j++) {
                *(GLushort*)dst_pos = ((GLushort)(((char*)src_pos)[2]&0xf8)>>(3-1)) | ((GLushort)(((char*)src_pos)[1]&0xf8)<<(5-2)) | ((GLushort)(((char*)src_pos)[0]&0xf8)<<(10-2)) | ((GLushort)(((char*)src_pos)[3])?1:0);
                src_pos += src_stride;
                dst_pos += dst_stride;
            }
            dst_pos += dst_width;
            src_pos += src_widthadj;
        }
        return true;
    }
    // BGRA -> RGBA5551
    if ((src_format == GL_BGRA) && (dst_format == GL_RGBA) && (dst_type == GL_UNSIGNED_SHORT_5_5_5_1) && ((src_type == GL_UNSIGNED_BYTE))) {
        for (GLuint i = 0; i < height; i++) {
            for (GLuint j = 0; j < width; j++) {
                *(GLushort*)dst_pos = ((GLushort)(((char*)src_pos)[0]&0xf8)>>(3-1)) | ((GLushort)(((char*)src_pos)[1]&0xf8)<<(5-2)) | ((GLushort)(((char*)src_pos)[2]&0xf8)<<(10-2)) | ((GLushort)(((char*)src_pos)[3])?1:0);
                src_pos += src_stride;
                dst_pos += dst_stride;
            }
            dst_pos += dst_width;
            src_pos += src_widthadj;
        }
        return true;
    }
    // RGBA -> RGBA4444
    if ((src_format == GL_RGBA) && (dst_format == GL_RGBA) && (dst_type == GL_UNSIGNED_SHORT_4_4_4_4) && ((src_type == GL_UNSIGNED_BYTE))) {
        for (GLuint i = 0; i < height; i++) {
            for (GLuint j = 0; j < width; j++) {
                *(GLushort*)dst_pos = ((GLushort)(((char*)src_pos)[3]&0xf0))>>(4) | ((GLushort)(((char*)src_pos)[2]&0xf0)) | ((GLushort)(((char*)src_pos)[1]&0xf0))<<(4) | ((GLushort)(((char*)src_pos)[0]&0xf0))<<(8);
                src_pos += src_stride;
                dst_pos += dst_stride;
            }
            dst_pos += dst_width;
            src_pos += src_widthadj;
        }
        return true;
    }
    // BGRA -> RGBA4444
    if ((src_format == GL_BGRA) && (dst_format == GL_RGBA) && (dst_type == GL_UNSIGNED_SHORT_4_4_4_4) && ((src_type == GL_UNSIGNED_BYTE))) {
        for (GLuint i = 0; i < height; i++) {
            for (GLuint j = 0; j < width; j++) {
                *(GLushort*)dst_pos = ((GLushort)(((char*)src_pos)[3]&0xf0)>>(4)) | ((GLushort)(((char*)src_pos)[0]&0xf0)) | ((GLushort)(((char*)src_pos)[1]&0xf0)<<(4)) | ((GLushort)(((char*)src_pos)[2]&0xf0)<<(8));
                src_pos += src_stride;
                dst_pos += dst_stride;
            }
            dst_pos += dst_width;
            src_pos += src_widthadj;
        }
        return true;
    }
    // BGRA4444 -> RGBA
    if ((src_format == GL_BGRA) && (dst_format == GL_RGBA) && (dst_type == GL_UNSIGNED_BYTE) && (src_type == GL_UNSIGNED_SHORT_4_4_4_4_REV)) {
        for (GLuint i = 0; i < height; i++) {
            for (GLuint j = 0; j < width; j++) {
                const GLushort pix = *(GLushort*)src_pos;
                ((char*)dst_pos)[3] = ((pix>>12)&0x0f)<<4;
                ((char*)dst_pos)[2] = ((pix>>8)&0x0f)<<4;
                ((char*)dst_pos)[1] = ((pix>>4)&0x0f)<<4;
                ((char*)dst_pos)[0] = ((pix)&0x0f)<<4;
                src_pos += src_stride;
                dst_pos += dst_stride;
            }
            dst_pos += dst_width;
            src_pos += src_widthadj;
        }
        return true;
    }
    // RGBA5551 -> RGBA
    if ((src_format == GL_RGBA) && (dst_format == GL_RGBA) && (dst_type == GL_UNSIGNED_BYTE) && (src_type == GL_UNSIGNED_SHORT_5_5_5_1)) {
        for (GLuint i = 0; i < height; i++) {
            for (GLuint j = 0; j < width; j++) {
                const GLushort pix = *(GLushort*)src_pos;
                ((unsigned char*)dst_pos)[0] = ((pix>>11)&0x1f)<<3;
                ((unsigned char*)dst_pos)[1] = ((pix>>6)&0x1f)<<3;
                ((unsigned char*)dst_pos)[2] = ((pix>>1)&0x1f)<<3;
                ((unsigned char*)dst_pos)[3] = ((pix)&0x01)?255:0;
                src_pos += src_stride;
                dst_pos += dst_stride;
            }
            dst_pos += dst_width;
            src_pos += src_widthadj;
        }
        return true;
    }
    return true;
  }
  return true;
}
