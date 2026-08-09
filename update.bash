#!/bin/bash
set -e

# ==================== 配置区 ====================
# 如果 ska 不在 .gitmodules 中，请在此设置默认 URL（或通过环境变量 SKA_REPO_URL 传入）
DEFAULT_SKA_URL="${SKA_REPO_URL:-https://github.com/username/ska.git}"

SKA_PATH="MobileGlues-cpp/include/ska"
BAD_SUBMODULE="MobileGlues-cpp/include/FastSTL"

# Git 用户配置（CI 环境）
git config --global user.name "GitHub Actions"
git config --global user.email "actions@github.com"

# ==================== 1. 强制删除 FastSTL ====================
echo "🔍 强制移除坏子模块：$BAD_SUBMODULE"
git config -f .gitmodules --remove-section "submodule.$BAD_SUBMODULE" 2>/dev/null || true
git config -f .git/config --remove-section "submodule.$BAD_SUBMODULE" 2>/dev/null || true
git rm --cached -f "$BAD_SUBMODULE" 2>/dev/null || true
rm -rf "$BAD_SUBMODULE"
rm -rf ".git/modules/$BAD_SUBMODULE"
echo "✅ 已彻底移除：$BAD_SUBMODULE"

if ! git diff --quiet .gitmodules 2>/dev/null; then
    git add .gitmodules
fi

# ==================== 2. 处理 ska 子模块 ====================
echo "🔍 检查 ska 子模块状态..."

# 检查 .gitmodules 是否已有 ska 注册
if git config -f .gitmodules --get "submodule.$SKA_PATH.path" >/dev/null 2>&1; then
    echo "ℹ️  ska 已在 .gitmodules 中注册，直接恢复/更新..."
    # 如果本地路径不存在，执行 init 克隆
    if [ ! -d "$SKA_PATH" ]; then
        echo "  本地路径不存在，执行 git submodule update --init $SKA_PATH"
        git submodule update --init "$SKA_PATH"
    else
        echo "  本地路径已存在，更新至最新..."
        git submodule update --remote "$SKA_PATH"
    fi
    git add "$SKA_PATH"
else
    echo "ℹ️  ska 未在 .gitmodules 中注册，将执行添加操作..."
    # 如果本地有残留目录，先删除
    if [ -d "$SKA_PATH" ]; then
        echo "  删除未注册的本地目录..."
        rm -rf "$SKA_PATH"
    fi
    # 使用默认 URL 添加
    git submodule add "$DEFAULT_SKA_URL" "$SKA_PATH"
    git add .gitmodules "$SKA_PATH"
    echo "✅ 已添加 ska 子模块。"
fi

# ==================== 3. 提交删除 FastSTL 及 ska 相关变更 ====================
if git diff-index --quiet HEAD --; then
    echo "✅ 没有变更需要提交。"
else
    git commit -m "Remove FastSTL and ensure ska submodule $(date '+%Y-%m-%d %H:%M:%S')"
    git push
    echo "✅ 已提交并推送变更。"
fi

# ==================== 4. 清理所有未注册的子模块 ====================
echo "🔍 清理所有未注册的子模块（包括残留配置）..."
registered_paths=$(git config --file .gitmodules --get-regexp 'submodule\..*\.path' 2>/dev/null | awk '{print $2}' || true)

force_remove_submodule() {
    local path="$1"
    echo "  强制移除子模块：$path"
    git config -f .gitmodules --remove-section "submodule.$path" 2>/dev/null || true
    git config -f .git/config --remove-section "submodule.$path" 2>/dev/null || true
    git rm --cached -f "$path" 2>/dev/null || true
    rm -rf "$path"
    rm -rf ".git/modules/$path"
    echo "  ✅ 已彻底移除：$path"
}

find . -type d -name ".git" ! -path "." | while read -r git_dir; do
    sub_path=$(dirname "$git_dir" | sed 's|^\./||')
    if [ -z "$sub_path" ] || [ "$sub_path" = "." ]; then
        continue
    fi
    if ! echo "$registered_paths" | grep -qxF "$sub_path"; then
        force_remove_submodule "$sub_path"
    fi
done

if ! git diff --quiet .gitmodules 2>/dev/null; then
    git add .gitmodules
fi

# ==================== 5. 提交清理变更 ====================
if git diff-index --quiet HEAD --; then
    echo "✅ 没有清理变更需要提交。"
else
    git commit -m "Clean up orphan submodules $(date '+%Y-%m-%d %H:%M:%S')"
    git push
    echo "✅ 已提交并推送清理变更。"
fi

# ==================== 6. 更新所有已注册的子模块 ====================
if [ -n "$registered_paths" ]; then
    echo "🔄 开始更新所有注册的子模块..."
    for path in $registered_paths; do
        echo "  处理子模块：$path"
        if [ ! -d "$path" ]; then
            echo "    路径不存在，先执行初始化克隆..."
            git submodule update --init "$path"
        fi
        git submodule update --remote "$path"
        git add "$path"
    done
else
    echo "ℹ️  没有子模块需要更新。"
fi

# ==================== 7. 提交子模块更新 ====================
if git diff-index --quiet HEAD --; then
    echo "✅ 没有子模块更新需要提交。"
else
    git commit -m "Automated submodule update $(date '+%Y-%m-%d %H:%M:%S')"
    git push
    echo "✅ 已提交并推送子模块更新。"
fi

# ==================== 8. 同步子模块配置 ====================
echo "🔄 同步子模块配置..."
git submodule sync --recursive

echo "🎉 所有操作完成！"
