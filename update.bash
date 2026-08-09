#!/bin/bash
set -e

# 配置 Git 用户
git config --global user.name "GitHub Actions"
git config --global user.email "actions@github.com"

# ================== 1. 解决合并冲突（如果有） ==================
echo "🔍 检查合并冲突..."
if git status --porcelain | grep -E '^(UU|AA|DD|AU|UA|DU|UD)' > /dev/null; then
    echo "⚠️  检测到冲突，自动采用 PR 分支（theirs）版本..."
    for file in $(git diff --name-only --diff-filter=U); do
        git checkout --theirs "$file"
        git add "$file"
    done
    git commit --no-edit || true
    echo "✅ 冲突已解决并提交。"
fi

# ================== 2. 强制清理特定坏子模块（FastSTL） ==================
BAD_SUBMODULE="MobileGlues-cpp/include/FastSTL"
echo "🔍 强制移除已知的坏子模块：$BAD_SUBMODULE"

# 从 .gitmodules 删除节（如果存在）
git config -f .gitmodules --remove-section "submodule.$BAD_SUBMODULE" 2>/dev/null || true
# 从 .git/config 删除节
git config -f .git/config --remove-section "submodule.$BAD_SUBMODULE" 2>/dev/null || true
# 从索引中强制删除
git rm --cached -f "$BAD_SUBMODULE" 2>/dev/null || true
# 删除物理目录
rm -rf "$BAD_SUBMODULE"
# 删除 .git/modules/ 缓存
rm -rf ".git/modules/$BAD_SUBMODULE"
echo "✅ 已彻底移除：$BAD_SUBMODULE"

# ================== 3. 清理所有未注册的子模块（通用） ==================
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

# 扫描所有包含 .git 的目录（但排除根目录）
find . -type d -name ".git" ! -path "." | while read -r git_dir; do
    sub_path=$(dirname "$git_dir" | sed 's|^\./||')
    if [ -z "$sub_path" ] || [ "$sub_path" = "." ]; then
        continue
    fi
    if ! echo "$registered_paths" | grep -qxF "$sub_path"; then
        force_remove_submodule "$sub_path"
    fi
done

# 如果 .gitmodules 被修改，添加它
if ! git diff --quiet .gitmodules; then
    git add .gitmodules
fi

# ================== 4. 提交所有清理变更 ==================
if git diff-index --quiet HEAD --; then
    echo "✅ 没有清理变更需要提交。"
else
    git commit -m "Clean up orphan submodules $(date '+%Y-%m-%d %H:%M:%S')"
    git push
    echo "✅ 已提交并推送清理变更。"
fi

# ================== 5. 更新所有注册的子模块 ==================
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

# ================== 6. 提交子模块更新 ==================
if git diff-index --quiet HEAD --; then
    echo "✅ 没有子模块更新需要提交。"
else
    git commit -m "Automated submodule update $(date '+%Y-%m-%d %H:%M:%S')"
    git push
    echo "✅ 已提交并推送子模块更新。"
fi

# ================== 7. 额外同步子模块配置 ==================
echo "🔄 同步子模块配置..."
git submodule sync --recursive

echo "🎉 所有操作完成！"
