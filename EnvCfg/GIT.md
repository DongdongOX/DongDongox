### **一、基础配置命令**

用于首次使用 Git 时配置用户信息、编辑器等。

1. **`git config --global user.name "你的名字"`**
   - 配置全局用户名（提交代码时显示的作者）。
   - 不加 `--global` 则仅对当前仓库生效。
2. **`git config --global user.email "你的邮箱"`**
   - 配置全局用户邮箱（与代码托管平台账号关联）。
3. **`git config --global core.editor "编辑器路径"`**
   - 配置默认编辑器（如 `code --wait` 对应 VS Code）。
4. **`git config --list`**
   - 查看当前所有 Git 配置。

### **二、仓库初始化与克隆**

创建新仓库或获取远程仓库到本地。

1. **`git init`**
   - 在当前目录初始化一个新的 Git 仓库（生成 `.git` 隐藏文件夹，存储版本信息）。
2. **`git clone <远程仓库地址>`**
   - 克隆远程仓库到本地（如 `git clone git@github.com:xxx/xxx.git`）。
   - 会自动创建与远程仓库的关联，默认远程名为 `origin`。

### **三、工作区与暂存区操作**

管理文件的修改、暂存和提交。

1. **`git status`**
   - 查看工作区状态（哪些文件被修改、新增、删除，是否已暂存）。
2. **`git add <文件名>`**
   - 将指定文件的修改添加到暂存区（准备提交）。
   - 常用：`git add .`（添加当前目录所有修改）、`git add -u`（添加已跟踪文件的修改 / 删除）。
3. **`git reset <文件名>`**
   - 将暂存区的文件移除（放回工作区，不影响修改内容）。
4. **`git commit -m "提交信息"`**
   - 将暂存区的内容提交到本地仓库，生成一个版本记录。
   - 常用：`git commit -am "提交信息"`（直接提交已跟踪文件的修改，跳过 `git add`）。
5. **`git rm <文件名>`**
   - 删除工作区文件，并将删除操作添加到暂存区（需配合 `git commit` 提交）。
   - 若仅删除工作区文件，用 `rm <文件名>`，再 `git add` 标记删除。
6. **`git mv <原文件名> <新文件名>`**
   - 重命名文件，并将操作添加到暂存区（等价于 `mv` + `git add`）。

### **四、版本历史查看**

查看提交记录、对比版本差异。

1. **`git log`**
   - 查看提交历史（按时间倒序，显示作者、时间、提交信息等）。
   - 常用参数：
     - `git log --oneline`：简洁显示（仅 commit ID 和提交信息）。
     - `git log -n <数字>`：显示最近 n 次提交。
     - `git log --graph`：以图形化显示分支合并历史。
2. **`git log <分支名>`**
   - 查看指定分支的提交历史（如 `git log dev`）。
3. **`git diff`**
   - 查看工作区与暂存区的差异（未 `git add` 的修改）。
4. **`git diff --cached` 或 `git diff --staged`**
   - 查看暂存区与本地仓库的差异（已 `git add` 但未 `git commit` 的修改）。
5. **`git diff < commit ID1 > < commit ID2 >`**
   - 对比两个版本之间的差异。

### **五、分支操作**

Git 的核心功能，用于并行开发、隔离功能。

1. **`git branch`**
   - 查看本地所有分支（当前分支前带 `*`）。
2. **`git branch <分支名>`**
   - 创建新分支（基于当前分支的最新提交）。
3. **`git checkout <分支名>`**
   - 切换到指定分支（工作区文件会同步为该分支的内容）。
   - 新版 Git 可用 `git switch <分支名>` 替代，更直观。
4. **`git checkout -b <分支名>`**
   - 创建并立即切换到新分支（等价于 `git branch <分支名>` + `git checkout <分支名>`）。
   - 新版可用 `git switch -c <分支名>`。
5. **`git merge <分支名>`**
   - 将指定分支的代码合并到当前分支（如在 `main` 分支合并 `dev` 分支：`git merge dev`）。
   - 若有冲突，需手动解决后 `git add` + `git commit` 完成合并。
6. **`git branch -d <分支名>`**
   - 删除本地分支（需先切换到其他分支，且分支已合并到主分支）。
   - 强制删除未合并的分支：`git branch -D <分支名>`。
7. **`git branch -m <旧分支名> <新分支名>`**
   - 重命名本地分支。

### **六、远程仓库操作**

与远程仓库（如 GitHub、GitLab）同步代码。

1. **`git remote`**
   - 查看已关联的远程仓库别名（默认 `origin`）。
2. **`git remote -v`**
   - 查看远程仓库的详细地址（fetch 拉取地址、push 推送地址）。
3. **`git remote add <别名> <远程地址>`**
   - 关联远程仓库（如 `git remote add origin git@github.com:xxx/xxx.git`）。
4. **`git remote remove <别名>`**
   - 移除已关联的远程仓库。
5. **`git pull <远程别名> <远程分支名>`**
   - 拉取远程分支的最新代码并合并到本地当前分支（如 `git pull origin main`）。
   - 等价于 `git fetch` + `git merge`。
6. **`git push <远程别名> <本地分支名>`**
   - 推送本地分支到远程仓库（如 `git push origin main`）。
   - 首次推送时需关联：`git push -u origin main`（后续可直接 `git push`）。
7. **`git push <远程别名> --delete <远程分支名>`**
   - 删除远程仓库的指定分支（如 `git push origin --delete dev`）。
8. **`git fetch <远程别名>`**
   - 拉取远程仓库的所有分支更新到本地（但不合并，需手动 `git merge`）。

### **七、版本回退与撤销**

修正错误提交或恢复历史版本。

1. **`git reset --hard <commit ID>`**
   - 回退到指定版本（工作区、暂存区、本地仓库均同步到该版本，**谨慎使用，会丢失后续修改**）。
   - 常用：`git reset --hard HEAD^`（回退到上一个版本，`HEAD^^` 是上上个，`HEAD~n` 是上 n 个）。
2. **`git reset --soft <commit ID>`**
   - 回退到指定版本，但保留工作区和暂存区的修改（可重新提交）。
3. **`git revert <commit ID>`**
   - 创建一个新的提交，抵消指定版本的修改（不删除历史，适合已推送到远程的版本）。
4. **`git checkout -- <文件名>`**
   - 丢弃工作区对指定文件的修改（恢复到最近一次 `git add` 或 `git commit` 的状态）。

### **八、标签操作**

标记重要版本（如发布版本 v1.0）。

1. **`git tag`**
   - 查看所有标签。
2. **`git tag <标签名> <commit ID>`**
   - 给指定提交打标签（如 `git tag v1.0 abc123`，默认标签打在最新提交）。
3. **`git tag -a <标签名> -m "标签说明" <commit ID>`**
   - 创建带说明的标签（ annotated tag，推荐，包含更多信息）。
4. **`git push <远程别名> <标签名>`**
   - 推送标签到远程仓库（如 `git push origin v1.0`）。
   - 推送所有标签：`git push origin --tags`。
5. **`git tag -d <标签名>`**
   - 删除本地标签。
6. **`git push <远程别名> --delete <标签名>`**
   - 删除远程标签。

### **九、其他实用命令**

1. **`git stash`**
   - 暂存工作区的修改（用于切换分支前保存未提交的内容）。
   - 恢复暂存：`git stash pop`（恢复后删除 stash 记录）或 `git stash apply`（保留 stash 记录）。
   - 查看暂存列表：`git stash list`。
2. **`git cherry-pick <commit ID>`**
   - 将指定提交复制到当前分支（适用于跨分支复用单个提交）。
3. **`git rebase <分支名>`**
   - 变基操作：将当前分支的提交 “嫁接” 到目标分支的最新提交上（使历史更整洁，替代 `merge`）。
   - 注意：已推送到远程的分支慎用 rebase，可能导致冲突。
4. **`git clean -n` / `git clean -f`**
   - 清理工作区的未跟踪文件（`-n` 预览，`-f` 强制删除）。



重命名本地分支：git branch -m master main

推送并关联远程main分支：git push -u Switch main

查看本地分支：git branch

查看远程分支：git branch -r



配置ssh：ssh-keygen -t rsa -C "your_email@example.com"    cat ~/.ssh/id_rsa.pub