#!/bin/bash
set -e  # 遇到错误立即退出，保证流程安全

# ----- 配置 Git 用户（用于自动提交）-----
git config --global user.name "GitHub Actions"
git config --global user.email "actions@github.com"

# ----- 可选：限定只处理某个目录下的子模块，留空则扫描整个仓库 -----
# 例如：BASE_DIR="MobileGlues-cpp/3rdparty"
BASE_DIR=""   # 空代表整个仓库根目录

# ----- 1. 获取当前 .gitmodules 中注册的所有子模块路径 -----
registered_paths=$(git config --file .gitmodules --name-only --get-regexp 'submodule\..*\.path' | sed 's/^submodule\.[^.]*\.path //' | sort -u)

if [ -z "$registered_paths" ]; then
    echo "⚠️  .gitmodules 中没有注册任何子模块，将清理所有残留的子模块目录。"
else
    echo "✅ 当前注册的子模块路径："
    echo "$registered_paths"
fi

# ----- 2. 定义一个函数，递归扫描并移除“僵尸子模块” -----
remove_orphan_submodules() {
    local scan_dir="$1"
    # 如果指定了 BASE_DIR，则只扫描该目录；否则扫描整个仓库
    if [ -n "$BASE_DIR" ]; then
        scan_dir="$BASE_DIR"
    else
        scan_dir="."
    fi

    echo "🔍 扫描目录：$scan_dir 中可能残留的子模块..."

    # 使用 find 查找所有包含 .git 子目录或 .git 文件的目录（可能是子模块）
    find "$scan_dir" -type d -name ".git" | while read -r git_dir; do
        # 获取该 .git 所在父目录的路径（即子模块根目录）
        submodule_path=$(dirname "$git_dir")
        # 去掉开头的 ./ 以便比较
        submodule_path=${submodule_path#./}

        # 如果该路径不在 registered_paths 中，则视为孤儿子模块
        if ! echo "$registered_paths" | grep -qxF "$submodule_path"; then
            echo "⚠️  发现未注册的子模块目录：$submodule_path （即将移除）"
            # 确保它不是当前仓库主目录的 .git
            if [ "$submodule_path" != "." ] && [ "$submodule_path" != "" ]; then
                # 执行标准移除流程
                set +e  # 允许某些命令失败（比如未初始化）
                git submodule deinit -f "$submodule_path" 2>/dev/null
                git rm --cached "$submodule_path" 2>/dev/null
                set -e
                rm -rf "$submodule_path"
                echo "🗑️  已移除残留子模块：$submodule_path"
            fi
        fi
    done
}

# ----- 3. 执行清理（先清理，再更新，避免影响） -----
remove_orphan_submodules

# ----- 4. 初始化并更新所有已注册的子模块 -----
if [ -n "$registered_paths" ]; then
    echo "🔄 开始更新所有注册的子模块..."
    for path in $registered_paths; do
        echo "  处理子模块：$path"
        git submodule update --init --remote "$path"
        git add "$path"   # 将新的 commit 引用加入暂存区
    done
else
    echo "ℹ️  没有子模块需要更新。"
fi

# ----- 5. 提交变更（如果有） -----
if git diff-index --quiet HEAD --; then
    echo "✅ 没有变更需要提交。"
else
    git commit -m "Automated submodule sync & cleanup $(date '+%Y-%m-%d %H:%M:%S')"
    git push
    echo "✅ 已提交并推送更新。"
fi
