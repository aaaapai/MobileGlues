//
// Created by hanji on 2025/2/3.A
//

#ifndef MOBILEGLUES_GL_MG_H
#define MOBILEGLUES_GL_MG_H

#include "gl.h"

#ifdef __cplusplus
extern "C" {
#endif

GLAPI GLAPIENTRY void glClearDepth( GLclampd depth );
GLAPI GLAPIENTRY void glHint( GLenum target, GLenum mode );

#ifdef __cplusplus
}
#endif


#endif //MOBILEGLUES_GL_MG_H
