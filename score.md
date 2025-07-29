# 给分设置 125pts

# Subtask1(init,add,commit,rm) 10pts

### `1-init` `test01-init.in` 2pts
*   **功能**: 测试 `init` 命令的基本功能，确保它能成功创建一个新的 Gitlet 仓库。
*   **使用命令**:
    *   `init`

---

### `1-add-01` `test18-nop-add.in` 1pts
*   **功能**: 测试 `add` 一个内容没有改变的已跟踪文件，这应该是一个无操作（no-op）。
*   **使用命令**:
    *   `add`

---

### `1-add-02` `test19-add-missing-err.in` 1pts
*   **功能**: 测试 `add` 一个不存在的文件时是否会报错。
*   **使用命令**:
    *   `add`

---

### `1-commit-01` `test16-empty-commit-err.in` 1pts
*   **功能**: 测试在没有文件被暂存时执行 `commit` 是否会报错。
*   **使用命令**:
    *   `commit`

---

### `1-commit-02` `test17-empty-commit-message-err.in` 1pts
*   **功能**: 测试 `commit` 命令使用空消息时是否会报错。
*   **使用命令**:
    *   `add`
    *   `commit`

---

### `1-rm` `test21-nop-remove-err.in` 2pts
*   **功能**: 测试 `rm` 一个未被跟踪也未被暂存的文件时是否会报错。
*   **使用命令**:
    *   `rm`

---

### `1-robust` `test41-no-command-err.in` 2pts
*   **功能**: 测试不带任何命令直接运行 `java gitlet.Main` 时是否会报错。
*   **使用命令**:
    *   (无)

---

# Subtask2(log,find,checkout) 10pts

### `2-log` `test03-basic-log.in` 2pts
*   **功能**: 测试 `log` 命令的基本功能，确保它能按正确格式和顺序显示提交历史。
*   **使用命令**:
    *   `init`
    *   `add`
    *   `commit`
    *   `log`

---

### `2-global-log-01` `test23-global-log.in` 1pts
*   **功能**: 测试 `global-log` 命令，确保它能显示所有分支的所有提交记录。
*   **使用命令**:
    *   `add`
    *   `commit`
    *   `log`
    *   `global-log`

---

### `2-find-01` `test25-successful-find.in` 2pts
*   **功能**: 测试 `find` 命令，确保它能根据提交信息找到并列出所有匹配的提交ID。
*   **使用命令**:
    *   `rm`
    *   `commit`
    *   `add`
    *   `log`
    *   `find`

---

### `2-find-02` `test27-unsuccessful-find-err.in` 1pts
*   **功能**: 测试 `find` 命令在找不到任何匹配的提交时是否会报错。
*   **使用命令**:
    *   `rm`
    *   `commit`
    *   `find`

---

### `2-checkout-01` `test02-basic-checkout.in` 2pts
*   **功能**: 测试 `checkout -- [file name]` 的基本功能，即用暂存区的文件版本覆盖工作目录中的文件。
*   **使用命令**:
    *   `init`
    *   `add`
    *   `commit`
    *   `checkout`

---

### `2-checkout-02` `test04-prev-checkout.in` 2pts
*   **功能**: 测试 `checkout [commit id] -- [file name]` 的功能，即用指定提交中的文件版本覆盖工作目录中的文件。
*   **使用命令**:
    *   `init`
    *   `add`
    *   `commit`
    *   `log`
    *   `checkout`

---


# Subtask3(status,checkout) 20pts

### `3-status` `test11-basic-status.in` 2pts
*   **功能**: 测试 `status` 命令在初始提交后的基本输出，包括分支、暂存文件和已删除文件等。
*   **使用命令**:
    *   `status`

---

### `3-status-01` `test20-status-after-commit.in` 2pts
*   **功能**: 测试在一次 `commit` 之后，工作目录应该是干净的，`status` 命令应该反映这一点。
*   **使用命令**:
    *   `rm`
    *   `commit`
    *   `status`

---

### `3-status-02` `test12-add-status.in` 2pts
*   **功能**: 测试文件被 `add` 后 `status` 命令的显示是否正确。
*   **使用命令**:
    *   `add`
    *   `status`

---

### `3-status-03` `test13-remove-status.in` 2pts
*   **功能**: 测试文件被 `rm` 后 `status` 命令的显示是否正确。
*   **使用命令**:
    *   `rm`
    *   `status`

---

### `3-status-04` `test14-add-remove-status.in` 2pts
*   **功能**: 测试对同一个文件先 `add` 再 `rm` 后 `status` 的显示。
*   **使用命令**:
    *   `add`
    *   `rm`
    *   `status`

---

### `3-status-05` `test15-remove-add-status.in` 2pts
*   **功能**: 测试对同一个文件先 `rm` 再 `add` 后 `status` 的显示。
*   **使用命令**:
    *   `rm`
    *   `add`
    *   `status`

---

### `3-status-06` `test22-remove-deleted-file.in` 2pts
*   **功能**: 测试 `rm` 一个在工作目录中已经被删除的文件，这应该能成功地将其从暂存区移除。
*   **使用命令**:
    *   `rm`
    *   `status`

---

### `3-status-07` `test42-other-err.in` 1pts
*   **功能**: 测试无效命令和 `status` 在未初始化仓库时的行为。
*   **使用命令**:
    *   (空命令)
    *   `status`
    *   `init`
    *   `glorp` (无效命令)

---

### `3-checkout-03` `test28-checkout-detail.in` 2pts
*   **功能**: 详细测试 `checkout [commit id] -- [file name]`，确保它能正确恢复文件到指定版本，并且 `status` 正确显示工作区是干净的。
*   **使用命令**:
    *   `add`
    *   `commit`
    *   `log`
    *   `checkout`
    *   `status`

---

### `3-checkout-04` `test29-bad-checkouts-err.in` 2pts
*   **功能**: 测试各种错误的 `checkout` 用法，例如文件不存在、提交ID不存在、检出不存在的分支等。
*   **使用命令**:
    *   `add`
    *   `commit`
    *   `log`
    *   `checkout`

---

### `3-checkout-05` `test39-short-uid.in` 1pts
*   **功能**: 测试是否支持使用缩短的、唯一的提交ID前缀来执行 `checkout` 等命令。
*   **使用命令**:
    *   `init`
    *   `add`
    *   `commit`
    *   `log`
    *   `checkout`

---

# Subtask4(branch,rm-branch,reset) 20pts

### `4-branch-01` `test30-branches.in` 3pts
*   **功能**: 测试分支的基本操作，包括 `branch` 创建新分支和 `checkout` 切换分支。
*   **使用命令**:
    *   `branch`
    *   `add`
    *   `commit`
    *   `checkout`
    *   `status`

---

### `4-branch-02` `test31-duplicate-branch-err.in` 2pts
*   **功能**: 测试当尝试创建一个已存在的分支时，`branch` 命令是否会报错。
*   **使用命令**:
    *   `branch`
    *   `add`
    *   `commit`

---

### `4-branch-03` `test32-file-overwrite-err.in` 2pts
*   **功能**: 测试在切换分支或 `reset` 时，如果工作目录中存在未跟踪的文件，并且该操作会覆盖这个文件，是否会报错。
*   **使用命令**:
    *   `branch`
    *   `add`
    *   `commit`
    *   `checkout`

---

### `4-rm-branch-01` `test30-rm-branch.in` 3pts
*   **功能**: 测试 `rm-branch` 命令，确保它可以删除一个分支，并且之后无法再检出该分支。
*   **使用命令**:
    *   `branch`
    *   `add`
    *   `commit`
    *   `checkout`
    *   `rm-branch`

---

### `4-rm-branch-02` `test31-rm-branch-err.in` 2pts
*   **功能**: 测试 `rm-branch` 的错误情况，例如删除不存在的分支或删除当前所在的分支。
*   **使用命令**:
    *   `branch`
    *   `rm-branch`

---

### `4-reset-01` `test37-reset1.in` 3pts
*   **功能**: 测试 `reset` 命令，将当前分支的 HEAD 回退到指定的提交，并更新工作目录。
*   **使用命令**:
    *   `branch`
    *   `add`
    *   `rm`
    *   `commit`
    *   `checkout`
    *   `log`
    *   `reset`
    *   `status`

---

### `4-reset-02` `test38-bad-resets-err.in` 3pts
*   **功能**: 测试 `reset` 的错误情况，例如重置到不存在的提交ID，或当工作目录有未跟踪文件可能被覆盖时。
*   **使用命令**:
    *   `branch`
    *   `add`
    *   `rm`
    *   `commit`
    *   `log`
    *   `checkout`
    *   `reset`

---

### `4-global-log-02` `test24-global-log-prev.in` 1pts
*   **功能**: 测试 `reset` 到一个旧的提交后，`global-log` 仍然能显示被“丢弃”的提交记录。
*   **使用命令**:
    *   `add`
    *   `commit`
    *   `log`
    *   `reset`
    *   `global-log`

---



### `4-find-03` `test26-successful-find-orphan.in` 1pts
*   **功能**: 测试 `find` 命令是否能找到因为 `reset` 而产生的孤立提交。
*   **使用命令**:
    *   `rm`
    *   `commit`
    *   `log`
    *   `reset`
    *   `find`

---

# Subtask5(merge) 30pts

### `5-merge-01` `test33-merge-no-conflicts.in` 3pts
*   **功能**: 测试没有冲突的 `merge` 场景。一个分支修改了文件，另一个分支添加了新文件，合并应该能自动完成。
*   **使用命令**:
    *   `branch`
    *   `add`
    *   `commit`
    *   `checkout`
    *   `merge`
    *   `log`

---

### `5-merge-02` `test34-merge-conflicts.in` 3pts
*   **功能**: 测试有冲突的 `merge` 场景。两个分支修改了同一个文件的同一部分，合并时应该产生冲突，并生成带有冲突标记的文件。
*   **使用命令**:
    *   `branch`
    *   `add`
    *   `rm`
    *   `commit`
    *   `checkout`
    *   `merge`
    *   `log`
    *   `status`

---

### `5-merge-03` `test35-merge-rm-conflicts.in` 3pts
*   **功能**: 测试合并时一个分支修改文件而另一个分支删除同一个文件所产生的冲突。
*   **使用命令**:
    *   `branch`
    *   `add`
    *   `rm`
    *   `commit`
    *   `checkout`
    *   `merge`
    *   `log`
    *   `status`

---

### `5-merge-04` `test36-merge-err.in` 5pts
*   **功能**: 测试 `merge` 的各种错误情况，如合并到当前分支、合并不存在的分支、在有未提交更改时进行合并等。
*   **使用命令**:
    *   `branch`
    *   `add`
    *   `rm`
    *   `commit`
    *   `checkout`
    *   `merge`

---

### `5-merge-05` `test36-merge-parent2.in` 3pts
*   **功能**: 测试一个更复杂的合并场景，涉及到共同祖先的判断，确保合并是基于正确的分叉点。
*   **使用命令**:
    *   `branch`
    *   `checkout`
    *   `add`
    *   `commit`
    *   `merge`

---



### `5-merge-06` `test40-special-merge-cases.in` 4pts
*   **功能**: 测试特殊的合并场景，例如一个分支是另一个分支的祖先（应为快进合并），或者当前分支已经是最新状态。
*   **使用命令**:
    *   `branch`
    *   `add`
    *   `commit`
    *   `checkout`
    *   `merge`

---



### `5-merge-07` `test43-criss-cross-merge.in` 4pts
*   **功能**: 测试一个复杂的“交叉合并”场景，两个分支互相合并对方的旧状态，然后再进行合并，考验寻找正确分裂点的能力。
*   **使用命令**:
    *   `init`
    *   `branch`
    *   `add`
    *   `commit`
    *   `checkout`
    *   `merge`
    *   `rm`

---

### `5-merge-08` `test44-bai-merge.in` 5pts
*   **功能**: 这是一个非常复杂的合并场景，有多个分支和多次合并，用于测试合并逻辑的鲁棒性。
*   **使用命令**:
    *   `branch`
    *   `checkout`
    *   `add`
    *   `commit`
    *   `merge`
    *   `log`


---

# Subtask 6 Bonus 20

### `6-status-05` `ec-test01-untracked.in` 5pts
*   **功能**: 测试 `status` 命令在存在未跟踪文件时的行为。它会检查在 `add` 和 `commit` 之后，对已跟踪文件进行修改以及存在未跟踪文件时的状态显示是否正确。
*   **使用命令**:
    *   `status`
    *   `add`
    *   `commit`

---

### `6-remote-01` `ec-test10-remote-fetch-push.in` 5pts
*   **功能**: 测试远程仓库操作，特别是 `fetch` 和 `push`。它设置了两个仓库，在一个仓库中进行提交，然后在另一个仓库中 `fetch` 变更，并 `push` 本地的新提交。
*   **使用命令**:
    *   `log`
    *   `init`
    *   `add`
    *   `commit`
    *   `add-remote`
    *   `fetch`
    *   `checkout`
    *   `reset`
    *   `push`

---



### `6-remote-02` `ec-test11-remote-fetch-pull.in` 5pts
*   **功能**: 测试 `pull` 命令。它设置了两个仓库，在一个仓库中进行提交，然后在另一个仓库中 `fetch` 和 `reset`，之后在本地创建新的提交，并最终使用 `pull` 来合并远程分支的更新。
*   **使用命令**:
    *   `log`
    *   `init`
    *   `add-remote`
    *   `fetch`
    *   `reset`
    *   `add`
    *   `commit`
    *   `pull`

---

### `6-remote-03` `ec-test12-bad-remotes-err.in` 5pts
*   **功能**: 测试远程命令的各种错误情况，例如添加已存在的远程仓库、操作不存在的远程仓库或分支、删除不存在的远程仓库等。
*   **使用命令**:
    *   `log`
    *   `init`
    *   `add`
    *   `commit`
    *   `add-remote`
    *   `fetch`
    *   `push`
    *   `rm-remote`

---

# CodeStyle : 5 pts

# Code Review : 10 pts