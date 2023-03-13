# Advanvced Git

*Saez Computational Club (6 March 2023)*

---

## Mind your commit messages

The most typical commit message in our repos is `Updated boxplots.R`. Which
part of this message is informative? Once you make a commit, of course you
updated something. Any git interface readily shows which files you have
updated in the commit. Sadly, this message delivers zero useful information.
What to write then into a commit message? Focus on the purpose of your
work. E.g. `boxplots: darker whiskers` is a perfectly informative, concise
commit message. At large projects, it is also a good practice to use labels,
to define the broader scope or part of the software affected. In our previous
example we've just done it: the *boxplots:* can be such a label. But you
can formalise it further, defining a set of labels that you and your
colleagues always use. It is common to put these in square brackets.
E.g. `[PLOTS] viridis colour scale for gene by sample heatmap`.

You can also trigger github functionalities by commit messages. For example,
if your work in any way related to an issue, include the issue number prefixed
with a hash symbol. For example, `protect against zero division error in score
calculation (#17)`. This way the commit will be referenced in the issue
timeline. If you also add some keywords like *fix...*, github will not only
reference the commit, but automatically close the issue: `protect against
zero division error in score calculation; fixes #17` - and as soon as you
push this commit to github, issue 17 will be closed.

Sometimes we commit really uninteresting, trivial, boring changes. That's
completely fine: make it clear in the message, I often write simply `nothing`,
so everyone knows nothing interesting is there, or `syntax error` (fixing
syntax errors is trivial and boring; better option is `commit --amend`).

## Don't you know what you're about to commit?

If you've just forgotten what you've been working on recently, check it by
`git diff HEAD`. This is a diff of the index (in git's mind) and the actual
state of the files on the disk. You can use `git diff` between any pair
of commits, tags, branches.

## Not all recent work must go into one commit

In general, it's good practice to commit often, but in many situations more
convenient to finish a larger unit of work and commit at the end. Okay, but
maybe you wrote a little helper function, a plotting function, and adjusted
something in the workflow management. These belong naturally to 3 commits.
An easy way to add the changes by files:

```
git add R/misc.R
git commit -m 'generic function to add version info to plots'
git add R/boxplots.R
git commit -m 'moved boxplots to facet grid'
git add R/workflow.R
git commit -m 'workflow: boxplots also for activities'
```

Because the changes of different purpose were in different files, it was
really easy to do as above. Sometimes you find that changes within one
file have nothing to do with each other and should go into separate commits.
In this case, the `--patch` switch of `git` comes handy:
`git add -p R/plots.R`. Then an interactive prompt will ask you which updated
blocks you want to include in the commit. You can also select line by line
within the same block.

## Stash your changes until you work on something else

You are working on something when you suddenly have to do something else
urgently; for example, you want to check out another branch, or merge
something, thus you can't afford to have uncommitted changes. The `stash`
command of git is there exactly for this purpose:

```
git diff HEAD
git stash
git checkout dev
# ... do something on dev
git stash pop
```

You can also give names to your stashes: `git stash save 'volcano plot'`.
Then use `git stash list` to see the list of stashed changes, and apply
them by their numeric index, e.g. to apply the first stash in the list:
`git stash apply 0`.

## Know the very basics of how git works

It's much easier to understand all the git commands (reset, rebase, fetch,
branch, etc) if you have a basic understanding of the principles. Git
maintains a graph, each commit is a node, it points to its ancestor and a
particular state of the index (the tracked files). Anything else, such as
branches, tags, HEAD, FETCH_HEAD, etc. are only human readable pointers to
the existing nodes. In many operations we only move around these labels.
For a visual understanding of this design, go through Mary Rose Cook's
*Git from inside out* material, which is available in assay and video format.

## Cherry picking

Sometimes you want to add a change to multiple divergent branches. E.g.
you've just fixed a bug in your Bioconductor package, and you want to backport
it to the release branch. With cherry picking it couldn't be easier:

```
git branch --show
# master
git add R/models.R
git commit -m 'fixed model calculations broken due to library update'
[master 53eeec6] fixed model calculations broken due to library update'
 1 file changed, 7 insertion(+), 5 deletion(-)
git checkout RELEASE_3_16
git cherry-pick 53eeec6
git push bioc RELEASE_3_16
git push saez RELEASE_3_16
git push bioc master
git push saez master
git checkout master
```

You can also cherry-pick commits of others and even modify them, in this case
the commit hash will change and both of you will be authors of the new commit.
In case of merge conflict you address the conflict and continue the cherry
picking:

```
git add -u
git cherry-pick --continue
```

Or you might realise it was not a good idea, too many conflicts, then you
are free to abort the cherry pick:

```
git cherry-pick --abort
```

## Modify commits

If you haven't pushed yet a commit, you can easily modify it. You can do it
even after push, but in this case you have to force at push, and your
colleagues too when they pull. (For push use the `--force-with-lease` flag
instead of `--force`, to avoid overwriting someone else's work.) To modify
the last commit, add your new  changes, and use the `-a` (`--amend`) switch:

```
git add -u
git commit --amend -m 'better proportions for plots'
```

To modify an older commit, use interactive `rebase`:

```
git add -u
git rebase -i 53eeec6^
# then you do your new changes
git add -u
git commit --amend -m 'better proportions for plots'
```

If you've already done the changes before rebase, commit them, and use
the interactive interface to move the commit that you want to modify to
the second place in the list, and apply `squash` on it instead of `pick`.

## Merge multiple commits into one

Why would I want to do that? For example, if you push to a remote that
applies hooks on every commit, and some of them fail, hence your push is
rejected. Or, you had many meaningless commits during a recent development
period, e.g. you spend one afternoon with a bug fix, and you have 3 commits
that were redundant attempts, and 4 others that are almost the same. The
larger the project, the more annoying meaningless commits might be. It
works exactly the same way as the previous example:

```
git rebase -i a788fe9  # refer to the oldest commit in the sequence
```

Typically when you work on an issue or a pull request, you already know
that you'll want to squash the commits. Git can help you to do it easier
in this case:

```
git add -u
git commit --squash -m 'created data frame output'
git add -u
git commit --squash -m 'updated data frame column layout'
git add -u
git commit --squash -m 'fixed data frame dtypes'
git rebase -u --autosquash
```

## Branches

Since git is available, developer teams came up with various branching
strategies, typically using feature branches. Branches serve the purpose
of grouping commits. Keeping any group of commits isolated from the main
branch results extra work when you want to merge them: due to outdated
implementations and conflicting changes. Unless there is a very strong
reason, I recommend to do all development on one branch. If more than one
developers work on it, merge (push+pull) it very often, at least once per
day. Sometimes branches are useful for distribution or deployment, and this
justify their use, but that has little to do with the development. For example,
you might prefer to have the default branch as a release branch, or have
branches for each release, just like Bioconductor, then you can cherry pick
and maintain the release on those branches, while you develop on one
dedicated development branch.

Switch branches easily:

```
git branch -
```

Insights about branches:

```
git branch -vv
```

## Merge old stuff

For this you can use rebase: you check out an old state, merge the branch
or commit or patch, and then rebase the branch onto the new merge commit.

## Revert undesired changes or commits

With git you always have the choice whether you apply an operation on
the index or also on the actual files. This is the main difference
between the modes of `reset`: `--soft` only moves the `HEAD` pointer;
`--mixed` resets the staging area too; and `--hard` also resets the contents
of the actual files.

```
echo 'something' > foobar
git add foobar
git commit -m 'test commit, to be removed'
git log
git reset --soft @~1
git status
git reset --hard @
git status
ls foobar  # it's gone!
```

## Apropos: what are those magic characters?

`@` simply means `HEAD`; about the tilde and caret:

- `HEAD~2`: 2 steps back from `HEAD`
- `HEAD^2`: the second parent of `HEAD`, if `HEAD` is a merge
- `HEAD^2~3`: 3 steps back from the 2nd parent (also if `HEAD` is a merge)
- `HEAD@{2}`: 3 steps back in `reflog`
- `HEAD~~` or `HEAD^^`: 2 commits older than `HEAD`

Check out `git help revisions`, it has a cool figure about these.

## Fast clone: ignore the history

When you contribute to someone else's project, and many other cases,
the full history is redundant, and omiting it reduces the cloning time
and the directory size:

```
git clone --depth 1 https://github.com/saezlab/OmnipathR
cd OmnipathR
git log  # see the history: it's only of length 1
```

## Check out a pull request locally

This is a very common workflow, when you receive a PR from someone else,
and you want to work on it yourself before merging it. Below, we check
out PR `#17` to a local branch `pr17`:

```
git fetch origin pull/17/head:pr17
git checkout pr17
# do your work
# then merge and push:
git checkout master
git merge pr17
git push origin master
# or push it to the PR, without merge:
git remote add colleague https://github.com/colleague/repo.git
# note: local branch "pr17" is pushed to "master" of the "colleague" remote
git push pr17 colleague:master
```

The first line seems difficult to remember, let's create an alias for it:

```
git config --global alias.pr '!sh -c '"'"'git fetch $1 pull/$2/head:pr$2 && git checkout pr$2'"'"' -'
```

## Hooks

Every git repo has the `hooks` directory. You can put custom scripts here
that will be executed in certain operations, e.g. before each commit, or
before each push. As an example, create a pre-commit hook:

```
echo "cowsay 'hello, thanks for the commit!'" >> .git/hooks/pre-commit
chmod +x .git/hooks/pre-commit
echo 'foo' > bar
git add bar
git commit -m 'test commit, to be removed'
git reset HEAD~1
```

## Pretty logs

Try these fancy log formats! :)

```
git log --oneline
git log --graph --oneline
git log --graph --pretty=format:'%Cred%h%Creset -%C(yellow)%d%Creset %s %Cgreen(%cr) %C(bold blue)<%an>%Creset' --abbrev-commit --date=relative
```

```
git log --graph --abbrev-commit --decorate --all \
    --format=format:"%C(bold blue)%h%C(reset) - %C(bold cyan)%aD%C(dim white) \
    - %an%C(reset) %C(bold green)(%ar)%C(reset)%C(bold yellow)%d%C(reset)%n %C(white)%s%C(reset)"
```

...and create aliases for your favourite ones:

```
git config --global alias.lol "log --graph --pretty=format:'%Cred%h%Creset -%C(yellow)%d%Creset %s %Cgreen(%cr) %C(bold blue)<%an>%Creset' --abbrev-commit --date=relative"
```

A different log: `reflog` keeps track of everything, even of things that
you undo or delete from the plain log:

```
git reflog
```

## Other info about the repo

Some more useful insights: first, total number of commits:

```
git rev-list --all --count
git rev-list --count master  # only on master
```

Number of lines changed since a certain point:

```
git diff --stat 7a4f9dd HEAD  # since that commit
git diff --shortstat @~3 @  # last 3 commits
git diff --cached --shortstat @~3 @  # include uncommitted changes
git diff --shortstat '@{10 days ago}'  # last 10 days
```

Commits from a particular author:

```
git log --author=deeenes
```

Lines changed by a particular author (yes, git can't add those numbers):

```
git log --since=2023-01-01 --until='{@ today}' --author=deeenes --format= --numstat | awk '{s+=$1; s+=$2} END {print s}'
```

## Who wrote this line?

I found `git blame` super useful in many situations, especially when I
am not sure about the origin of a weird piece of code, I don't know if it
remained there from old times, or someone recently added it:

```
git blame path/to/file
```

## Info about the remotes

Without opening GitHub in a browser, how do you tell which remotes do
you have configured, what branches and references do those have, and
what is the situation there, has someone committed since your last
pull? Do you have changes to push?

```
git status
git remote show origin
```

## Undo changes but keep them in history

There might be many reasons why you want to keep undone commits in the log.
For example, you have already pushed them to a remote, and someone else
might be working on them. In this situation, `git revert` is your friend:
it creates a new commit that is exactly the inverse of the changes that
you want to undo.

```
git revert bfb994a  # refer to the commit that you want to undo
```

## Use `.gitignore`

And don't commit `.DS_Store` directories. Also don't do `git add -A`.
And always include API tokens, password files and similar stuff, as soon
as you copy them into the repo directory!

No need to add the `.gitignore` file to the repo, but if you do it, include
stuff that commonly present under the project directory, e.g. build
artefacts, such as `dist` directory, R package tarballs, docs, etc.
There are many ready to use `.gitignore` templates with the most common
patterns. It's a good practice to maintain a user level `.gitignore`
with patterns like `.DS_Store`, it should be in your home: `~/.gitignore`.

## Remove a file from git without deleting it locally

```
git rm --cached large.pdf
```

## Modify the history

Not an everyday use case, but it happens time to time that you have to
remove something from the history: you added a password file; or you
added a huge file and the repo wastes space and is slow to clone. Do
this only if absolutely necessary: all commit hashes will change in
the subsequent history, that means push an pull with `--force` must
follow.

```
git filter-branch --index-filter 'git rm --cached --ignore-unmatch file/to/be/removed' --prune-empty --tag-name-filter cat -- --all
```

## Reduce repo size

We have seen already how to remove a large file from the history. But
where are those large (deleted) files, how to list them?

```
git rev-list --all | xargs -rL1 git ls-tree -r --long | sort -uk3 | sort -rnk4
```

Or with some more insight:

```
du -sh .git/objects/pack/* | sort -hr
git ls-tree -r --long 992d7fe134e5405e7e55c8730c0c709c776d28a5 | sort -rnk4
```

Another option to reduce the repo size is to remove stale branches.

```
git remote prune origin --dry-run
git remote prune origin
```

Set config to automatically do this:

```
git config --global fetch.prune true
```

Git also has a garbage collector procedure, but likely you don't have to
manually call it as it is called automatically if git thinks the repo is
unnecessarily large.

```
git gc --aggressive --prune=now
```

## Archiving and offline sharing

Git makes it easy to package your whole repo into a tarball. Why this
should be done by git? To include exactly the tracked files, including
hidden ones, and excluding other stray files within the repo directory,
and to reflect exactly the state you want to capture (e.g. a past commit,
or even if it's `HEAD`, no uncommitted changes).

```
git archive --format=tar.gz master > master.tar.gz
```

While `archive` creates a snapshot, to have a full backup or update
the repo on another computer offline, use `bundle`. Bundles behave
in many sense like they were remotes, they are just not on another
computer, but in a file.

```
git bundle crate backup-20230313.bundle master
# restore the repo (to the "newrepo" directory):
git bundle unbundle backup-20230313.bundle master newrepo
```

## Collect and remove untracked files

Maybe one day you realise it's not the best idea to keep dozens of project
datasets, paper figures, and your family photo album inside your git repo
directory. Even if those are not in the index, not tracked. But it would be
tedious to manually collect those files, so let git do it for us:

```
git ls-files --others --exclude-standard -z | xargs -0 tar -rzvf repo-junk.tar.gz
git clean -fdn
git clean -fd
```

Be careful with `git clean`!

## Git is keen to help you!

You must have noticed, it has well designed messages, typically it
tells you the reason and solution of errors right away, but not only that.
Git has exceptionally great docs, I recommend to look into it often.
Each git command has its own page with really well written text and
richly explained examples. For example:

```
git reset --help
```

And even beyond that, it has a whole manual book full of useful topics:

```
git help -g
git help -a
git help tutorial
```
