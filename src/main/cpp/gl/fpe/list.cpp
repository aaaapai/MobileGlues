//
// Created by hanji on 2025/3/28.
//

#include "list.h"
#include "pointer_utils.h"
#include "../log.h"
#include "fpe.hpp"

#define DEBUG 0

GLuint currentListBase = 0;

GLuint glGenLists(GLsizei range) {
    LOG()
    LOG_D("glGenLists(%i)", range)
    GLuint first = DisplayListManager::genDisplayList(range);
    return first;
}

void glDeleteLists(GLuint list, GLsizei range) {
    LOG()
    LOG_D("glDeleteLists(%d, %i)", list, range)
    DisplayListManager::deleteDisplayList(list, range);
}

GLboolean glIsList(GLuint list) {
    LOG()
    LOG_D("glIsList(%d)", list)
    return DisplayListManager::isDisplayList(list);
}

void glNewList(GLuint list, GLenum mode) {
    LOG()
    LOG_D("glNewList(%d, %s)", list, glEnumToString(mode))
    DisplayListManager::startRecord(list, mode);
}

void glEndList() {
    LOG()
    LOG_D("glEndLists()")
    DisplayListManager::endRecord();
}

void glCallList(GLuint list) {
    LOG()
    LOG_D("glCallList(%d)", list)

    if (DisplayListManager::shouldRecord()) {
        displayListManager.record<glCallList>({}, list);
        if (DisplayListManager::isRecording())
            return;
    }
    GET_PREV_PROGRAM
    DisplayListManager::callList(list);
    SET_PREV_PROGRAM
}

void glCallLists(GLsizei n, GLenum type, const GLvoid* lists) {
    LOG()
    LOG_D("glCallLists(%i, %s, %p)", n, glEnumToString(type), lists)

    if (DisplayListManager::shouldRecord()) {
        displayListManager.record<glCallLists>({{2, n * PointerUtils::type_to_bytes(type)}}, n, type, lists);
        if (DisplayListManager::isRecording())
            return;
    }
    GET_PREV_PROGRAM
    const auto* ptr = static_cast<const uint8_t*>(lists);
    for (int i = 0; i < n; ++i) {
        GLuint offset = 0;
        switch(type) {
            case GL_BYTE:
                offset = static_cast<GLuint>(*reinterpret_cast<const GLbyte*>(ptr));
                ptr += 1;
                break;
            case GL_UNSIGNED_BYTE:
                offset = *reinterpret_cast<const GLubyte*>(ptr);
                ptr += 1;
                break;
            case GL_SHORT:
                offset = static_cast<GLuint>(*reinterpret_cast<const GLshort*>(ptr));
                ptr += 2;
                break;
            case GL_UNSIGNED_SHORT:
                offset = *reinterpret_cast<const GLushort*>(ptr);
                ptr += 2;
                break;
            case GL_INT:
                offset = static_cast<GLuint>(*reinterpret_cast<const GLint*>(ptr));
                ptr += 4;
                break;
            case GL_UNSIGNED_INT:
                offset = *reinterpret_cast<const GLuint*>(ptr);
                ptr += 4;
                break;
            case GL_FLOAT:
                offset = static_cast<GLuint>(*reinterpret_cast<const GLfloat*>(ptr));
                ptr += 4;
                break;
            case GL_2_BYTES: {
                const auto* bytes = reinterpret_cast<const GLubyte*>(ptr);
                offset = (static_cast<GLuint>(bytes[0]) << 8) | bytes[1];
                ptr += 2;
                break;
            }
            case GL_3_BYTES: {
                const auto* bytes = reinterpret_cast<const GLubyte*>(ptr);
                offset = (static_cast<GLuint>(bytes[0]) << 16) |
                         (static_cast<GLuint>(bytes[1]) << 8) |
                         bytes[2];
                ptr += 3;
                break;
            }
            case GL_4_BYTES: {
                const auto* bytes = reinterpret_cast<const GLubyte*>(ptr);
                offset = (static_cast<GLuint>(bytes[0]) << 24) |
                         (static_cast<GLuint>(bytes[1]) << 16) |
                         (static_cast<GLuint>(bytes[2]) << 8) |
                         bytes[3];
                ptr += 4;
                break;
            }
            default:
                LOG_W("ERROR: Failed to handle lists and type!")
                break;
        }
        DisplayListManager::callList(currentListBase + offset);
    }
    SET_PREV_PROGRAM
}

void glListBase(GLuint base) {
    LOG()
    LOG_D("glGenLists(%d)", base)

    if (DisplayListManager::shouldRecord()) {
        displayListManager.record<glListBase>({}, base);
        if (DisplayListManager::shouldFinish())
            return;
    }

    currentListBase = base;
}