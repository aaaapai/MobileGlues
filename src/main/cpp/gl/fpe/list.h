//
// Created by hanji on 2025/3/28.
//

#ifndef MOBILEGLUES_PLUGIN_LIST_H
#define MOBILEGLUES_PLUGIN_LIST_H

#include "../gl.h"
#include "../log.h"

#include <vector>
#include <memory>
#include <tuple>
#include <unordered_map>
#include <GLES2/gl2.h>

#define DEBUG 0

// Todo: record more functions

class GLCmd {
public:
    virtual ~GLCmd() = default;
    GLCmd() = default;
    GLCmd(GLCmd&&) = default;
    GLCmd& operator=(GLCmd&&) = default;
    virtual void execute() const = 0;

    GLCmd(const GLCmd&) = delete;
    GLCmd& operator=(const GLCmd&) = delete;
};
using DisplayList = std::vector<std::unique_ptr<GLCmd>>;

template<auto FuncPtr, typename... Args>
class GLFuncCmd : public GLCmd {
    using StoredArgs = std::tuple<std::decay_t<Args>...>;
    StoredArgs args;

    static_assert(std::is_invocable_v<decltype(FuncPtr), Args...>,
                  "Function arguments mismatch");

public:
    explicit GLFuncCmd(Args&&... args) noexcept
            : args(std::forward<Args>(args)...) {}

    GLFuncCmd(GLFuncCmd&&) = default;
    GLFuncCmd& operator=(GLFuncCmd&&) = default;

    void execute() const override {
        std::apply([](auto&&... args) {
            FuncPtr(std::forward<decltype(args)>(args)...);
        }, args);
    }

private:
    GLFuncCmd(const GLFuncCmd&) = delete;
    GLFuncCmd& operator=(const GLFuncCmd&) = delete;
};

class DisplayListManager {
    inline static GLuint nextListId = 1;
    inline static GLenum listMode = GL_COMPILE;
    inline static GLboolean calling = GL_FALSE;

    inline static std::unordered_map<GLuint, DisplayList> lists;
    inline static GLuint currentListID = 0;

    template<auto Func, typename... Args>
    void recordImpl(Args&&... args) {
        if (currentListID != 0) {
            lists[currentListID].emplace_back(
                    std::make_unique<GLFuncCmd<Func, Args...>>(
                            std::forward<Args>(args)...
                    )
            );
        }
    }

public:
    static GLuint genDisplayList(GLsizei range) {
        GLuint first = nextListId;
        nextListId += range;
        for (GLuint i = first; i < first + range; ++i) {
            lists[i] = std::vector<std::unique_ptr<GLCmd>>{};
        }
        return first;
    }

    static void deleteDisplayList(GLuint list, GLsizei range) {
        for (GLuint i = 0; i < range; ++i) {
            lists.erase(list + i);
        }
    }

    static GLboolean isDisplayList(GLuint list) {
        return lists.find(list) != lists.end() ? GL_TRUE : GL_FALSE;
    }

    static void startRecord(GLuint listID, GLenum mode) {
        LOG_D("[DisplayList]: Start recording GL functions. List id = %d", listID)
        currentListID = listID;
        listMode = mode;
        lists.try_emplace(listID).first->second.clear();
    }

    static void endRecord() {
        LOG_D("[DisplayList]: End recording GL functions. List id = %d", currentListID)
        currentListID = 0;
        listMode = GL_COMPILE;
    }

    static int isRecording() {
        return currentListID != 0 ? 1 : 0;
    }

    static int shouldFinish() {
        return (currentListID != 0 && listMode == GL_COMPILE) ? 1 : 0;
    }

    template<auto Func, typename... Args>
    void record(Args&&... args) {
        if (!calling)
            recordImpl<Func>(std::forward<Args>(args)...);
    }

    static void callList(GLuint listID) {
        if (auto it = lists.find(listID); it != lists.end()) {
            LOG_D("Start executing list, list id = %d", listID)
            calling = GL_TRUE;
            for (auto& cmd : it->second) {
                cmd->execute();
            }
            calling = GL_FALSE;
            LOG_D("End executing list, list id = %d", listID)
        }
    }
};

inline DisplayListManager displayListManager;

inline GLboolean disableRecording = GL_FALSE;

#define SELF_CALL(func, ...)                      \
{                                                 \
    GLboolean alreadyDisabled = disableRecording; \
    disableRecording = GL_TRUE;                   \
    func(__VA_ARGS__);                            \
    if (!alreadyDisabled)                         \
        disableRecording = GL_FALSE;              \
}

GLAPI GLAPIENTRY GLuint glGenLists(GLsizei range);
GLAPI GLAPIENTRY void glDeleteLists(GLuint list, GLsizei range);
GLAPI GLAPIENTRY GLboolean glIsList(GLuint list);
GLAPI GLAPIENTRY void glNewList(GLuint list, GLenum mode);
GLAPI GLAPIENTRY void glEndList();
GLAPI GLAPIENTRY void glCallList(GLuint list);
GLAPI GLAPIENTRY void glCallLists(GLsizei n, GLenum type, const GLvoid* lists);
GLAPI GLAPIENTRY void glListBase(GLuint base);

#endif //MOBILEGLUES_PLUGIN_LIST_H
