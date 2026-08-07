#!/bin/bash
set -e

# 配置 Git 用户
git config --global user.name "GitHub Actions"
git config --global user.email "actions@github.com"

# 可选：限定扫描目录，留空代表整个仓库
BASE_DIR=""

# ---- 1. 准确获取当前注册的所有子模块路径 ----
registered_paths=$(git config --file .gitmodules --get-regexp 'submodule\..*\.path' | awk '{print $2}' | sort -u)

if [ -z "$registered_paths" ]; then
    echo "⚠️  .gitmodules 中没有注册任何子模块，将清理所有残留的子模块目录。"
else
    echo "✅ 当前注册的子模块路径："
    echo "$registered_paths"
fi

# ---- 2. 清理孤儿子模块（未注册的残留目录） ----
remove_orphan_submodules() {
    local scan_dir="${BASE_DIR:-.}"
    echo "🔍 扫描目录：$scan_dir 中可能残留的子模块..."
    find "$scan_dir" -type d -name ".git" | while read -r git_dir; do
        submodule_path=$(dirname "$git_dir")
        submodule_path=${submodule_path#./}   # 去掉开头的 ./
        # 跳过根目录本身（路径为 "." 或空）
        if [ "$submodule_path" = "." ] || [ -z "$submodule_path" ]; then
            continue
        fi
        # 如果该路径不在注册列表中，则移除
        if ! echo "$registered_paths" | grep -qxF "$submodule_path"; then
            echo "⚠️  发现未注册的子模块目录：$submodule_path （即将移除）"
            set +e
            git submodule deinit -f "$submodule_path" 2>/dev/null
            git rm --cached "$submodule_path" 2>/dev/null
            set -e
            rm -rf "$submodule_path"
            echo "🗑️  已移除残留子模块：$submodule_path"
        fi
    done
}

remove_orphan_submodules

# ---- 3. 初始化并更新所有已注册的子模块 ----
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

# ---- 4. 提交变更（如果有） ----
if git diff-index --quiet HEAD --; then
    echo "✅ 没有变更需要提交。"
else
    git commit -m "Automated submodule sync & cleanup $(date '+%Y-%m-%d %H:%M:%S')"
    git push
    echo "✅ 已提交并推送更新。"
fi
