//
// Created by Administrator on 2025/1/27.
//

#ifndef MOBILEGLUES_LOOKUP_H
#define MOBILEGLUES_LOOKUP_H
#include <GL/gl.h>

#include <GL/gl.h>
#ifdef __cplusplus
extern "C" {
#endif
typedef void (*__GLXextFuncPtr)(void);

GLAPI GLAPIENTRY __GLXextFuncPtr glXGetProcAddress(const GLubyte * name);
GLAPI GLAPIENTRY __GLXextFuncPtr glXGetProcAddressARB(const GLubyte * name);

#ifdef __cplusplus
}
#endif

#endif //MOBILEGLUES_LOOKUP_H
