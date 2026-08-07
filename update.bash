#!/bin/bash
set -e

# ================== 配置 ==================
# 如果要合并的 PR 分支名（留空则跳过合并）
PR_BRANCH=""

# ================== 1. 解决合并冲突 ==================
echo "🔍 检查合并冲突..."
if git status --porcelain | grep -E '^(UU|AA|DD|AU|UA|DU|UD)' > /dev/null; then
    echo "⚠️  检测到冲突，自动采用 PR 分支（theirs）版本..."
    for file in $(git diff --name-only --diff-filter=U); do
        git checkout --theirs "$file"
        git add "$file"
    done
    # 额外清理可能残留的 glm 子模块目录
    if [ -d "MobileGlues-cpp/3rdparty/glm" ]; then
        git rm --cached -f MobileGlues-cpp/3rdparty/glm 2>/dev/null || true
        rm -rf MobileGlues-cpp/3rdparty/glm
    fi
    git commit --no-edit
    echo "✅ 冲突已解决并提交。"
fi

if [ -n "$PR_BRANCH" ] && ! git status --porcelain | grep -E '^(UU|AA|DD|AU|UA|DU|UD)' > /dev/null; then
    echo "🔄 合并 PR 分支：$PR_BRANCH"
    git merge --no-edit "$PR_BRANCH" || {
        echo "⚠️  合并冲突，自动解决..."
        for file in $(git diff --name-only --diff-filter=U); do
            git checkout --theirs "$file"
            git add "$file"
        done
        git commit --no-edit
    }
fi

# ================== 2. 配置 Git 用户 ==================
git config --global user.name "GitHub Actions"
git config --global user.email "actions@github.com"

# ================== 3. 彻底清理所有未注册的子模块 ==================
echo "🔍 清理所有未注册的子模块（包括残留配置）..."

# 获取当前 .gitmodules 中注册的路径列表（如果有）
registered_paths=$(git config --file .gitmodules --get-regexp 'submodule\..*\.path' 2>/dev/null | awk '{print $2}' || true)

# 强制清理函数：移除子模块的所有痕迹
force_remove_submodule() {
    local path="$1"
    echo "  强制移除子模块：$path"
    # 从 .gitmodules 删除节（如果存在）
    git config -f .gitmodules --remove-section "submodule.$path" 2>/dev/null || true
    # 从 .git/config 删除节
    git config -f .git/config --remove-section "submodule.$path" 2>/dev/null || true
    # 从索引中强制删除
    git rm --cached -f "$path" 2>/dev/null || true
    # 删除物理目录
    rm -rf "$path"
    # 删除 .git/modules/ 下的缓存
    rm -rf ".git/modules/$path"
    echo "  ✅ 已彻底移除：$path"
}

# 扫描所有包含 .git 的目录，但跳过根目录
find . -type d -name ".git" ! -path "." | while read -r git_dir; do
    sub_path=$(dirname "$git_dir" | sed 's|^\./||')
    if [ -z "$sub_path" ] || [ "$sub_path" = "." ]; then
        continue
    fi
    # 检查该路径是否在注册列表中
    if ! echo "$registered_paths" | grep -qxF "$sub_path"; then
        force_remove_submodule "$sub_path"
    fi
done

# 额外检查：如果 .gitmodules 中还有残留的 glm 条目，也一并移除
if git config -f .gitmodules --get-regexp 'submodule\..*\.path' | grep -q "MobileGlues-cpp/3rdparty/glm"; then
    echo "⚠️  .gitmodules 中仍存在 glm 条目，正在移除..."
    git config -f .gitmodules --remove-section "submodule.MobileGlues-cpp/3rdparty/glm" 2>/dev/null || true
    git add .gitmodules
fi

# 如果 .gitmodules 被修改，需要提交
if ! git diff --quiet .gitmodules; then
    git add .gitmodules
    git commit -m "Remove glm from .gitmodules" || true
fi

# ================== 4. 更新所有注册的子模块 ==================
if [ -n "$registered_paths" ]; then
    echo "🔄 开始更新所有注册的子模块..."
    for path in $registered_paths; do
        echo "  处理子模块：$path"
        git submodule update --init --remote "$path"
        git add "$path"
    done
else
    echo "ℹ️  没有子模块需要更新。"
fi

# ================== 5. 提交所有变更 ==================
if git diff-index --quiet HEAD --; then
    echo "✅ 没有子模块变更需要提交。"
else
    git commit -m "Automated submodule sync & cleanup $(date '+%Y-%m-%d %H:%M:%S')"
    git push
    echo "✅ 已提交并推送更新。"
fi

echo "🎉 所有操作完成！"
