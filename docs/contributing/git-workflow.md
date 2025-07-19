# Git Workflow

This document describes the Git workflow and branching strategy for Ion Vortex.

## Branch Structure

### Protected Branches

#### `main`
- **Purpose**: Stable, production-ready code
- **Protection**: Requires PR, 1 approval, passing CI
- **Direct commits**: Forbidden
- **Force push**: Forbidden

### Feature Branches

#### Naming Convention
```
type/description-with-dashes
```

Types:
- `feat/` - New features
- `fix/` - Bug fixes
- `docs/` - Documentation only
- `style/` - Code style changes (formatting)
- `refactor/` - Code refactoring
- `perf/` - Performance improvements
- `test/` - Test additions/fixes
- `build/` - Build system changes
- `ci/` - CI configuration changes

Examples:
- `feat/sprite-batching`
- `fix/network-packet-corruption`
- `docs/build-instructions-windows`
- `refactor/extract-render-interface`

### Release Branches
```
release/v1.2.3
```
- Created from `main` for release preparation
- Only bug fixes and release notes updates
- Merged back to `main` and tagged

### Hotfix Branches
```
hotfix/critical-security-issue
```
- Created from `main` for urgent fixes
- Minimal changes only
- Merged back to `main` immediately

## Commit Guidelines

### Commit Message Format

Follow [Conventional Commits](https://www.conventionalcommits.org/):

```
type(scope): subject

body

footer
```

#### Structure
- **type**: feat, fix, docs, style, refactor, perf, test, build, ci
- **scope**: affected module (e.g., render, network, core)
- **subject**: imperative mood, no period, <50 chars
- **body**: detailed explanation, wrap at 72 chars
- **footer**: breaking changes, issue references

#### Examples
```
feat(render): add sprite batching support

Implements automatic batching for static sprites sharing the same
texture and shader. This reduces draw calls by up to 80% in 
sprite-heavy scenes.

- Sprites sorted by texture and shader
- Batch size configurable via RenderConfig
- Automatic flush on state changes

Fixes #122
```

```
fix(network): prevent packet fragmentation on Windows

Windows was fragmenting packets larger than 1400 bytes due to MTU
settings. Now we properly detect MTU and limit packet size.

BREAKING CHANGE: MaxPacketSize reduced from 1500 to 1400
```

### Commit Best Practices

#### Keep Commits Atomic
Each commit should:
- Have a single, clear purpose
- Compile and pass tests
- Be revertable without breaking other features

#### Write Good Messages
- Explain **what** and **why**, not how
- Reference issues and PRs
- Include breaking changes
- Be specific, not generic

Bad: `fix bug`
Good: `fix(network): resolve race condition in packet queue`

#### Sign Your Commits (Optional)
```bash
git config --global user.signingkey YOUR_KEY_ID
git config --global commit.gpgsign true
```

## Common Workflows

### Starting New Work

```bash
# Update your local main
git checkout main
git pull upstream main

# Create feature branch
git checkout -b feat/your-feature

# Make changes
git add -p  # Stage selectively
git commit -m "feat(module): add awesome feature"
```

### Keeping Branch Updated

```bash
# While on your feature branch
git fetch upstream
git rebase upstream/main

# If conflicts occur
git status  # See conflicted files
# Fix conflicts
git add .
git rebase --continue
```

### Cleaning Up History

Before opening PR:
```bash
# Interactive rebase to clean up commits
git rebase -i upstream/main

# Common operations:
# - squash: combine commits
# - fixup: combine without keeping message
# - reword: change commit message
# - drop: remove commit
```

### Submitting Pull Request

```bash
# Push to your fork
git push origin feat/your-feature

# Open PR via CLI
gh pr create --title "feat(module): add awesome feature" \
             --body "Description of changes"

# Or via web
# Go to GitHub and click "Create Pull Request"
```

### Updating Pull Request

```bash
# Make requested changes
git add -p
git commit -m "address review feedback"

# Update PR
git push origin feat/your-feature

# Or amend if single commit
git add .
git commit --amend
git push --force-with-lease origin feat/your-feature
```

### After PR Merge

```bash
# Update local main
git checkout main
git pull upstream main

# Delete local branch
git branch -d feat/your-feature

# Delete remote branch
git push origin --delete feat/your-feature

# Prune remote tracking
git remote prune origin
```

## Advanced Techniques

### Stashing Work

```bash
# Save current work
git stash push -m "work in progress on feature X"

# List stashes
git stash list

# Apply stash
git stash pop  # Apply and remove
git stash apply stash@{0}  # Apply without removing
```

### Cherry-Picking

```bash
# Apply specific commit to current branch
git cherry-pick abc123

# Cherry-pick without committing
git cherry-pick -n abc123
```

### Bisecting

Find when a bug was introduced:
```bash
git bisect start
git bisect bad  # Current version is bad
git bisect good v1.0.0  # v1.0.0 was good

# Git checks out commits for testing
# Run tests, then:
git bisect good  # or
git bisect bad

# When done
git bisect reset
```

### Recovering Lost Work

```bash
# Find lost commits
git reflog

# Recover commit
git checkout -b recovery abc123

# Or restore file from commit
git checkout abc123 -- path/to/file
```

## Git Configuration

### Recommended Settings

```bash
# User identity
git config --global user.name "Your Name"
git config --global user.email "you@example.com"

# Helpful aliases
git config --global alias.co checkout
git config --global alias.br branch
git config --global alias.ci commit
git config --global alias.st status
git config --global alias.unstage 'reset HEAD --'
git config --global alias.last 'log -1 HEAD'
git config --global alias.visual '!gitk'

# Better diffs
git config --global diff.algorithm histogram
git config --global diff.colorMoved zebra

# Rebase by default for pulls
git config --global pull.rebase true

# Push only current branch
git config --global push.default current

# Prune on fetch
git config --global fetch.prune true
```

### Useful Git Hooks

`.git/hooks/pre-commit`:
```bash
#!/bin/bash
# Run format check
if ! make format-check; then
    echo "Code formatting issues detected. Run 'make format'"
    exit 1
fi
```

`.git/hooks/commit-msg`:
```bash
#!/bin/bash
# Validate commit message format
if ! grep -qE "^(feat|fix|docs|style|refactor|perf|test|build|ci)(\(.+\))?: .{1,50}" "$1"; then
    echo "Commit message must follow Conventional Commits format"
    exit 1
fi
```

## Troubleshooting

### Common Issues

#### Merge Conflicts
```bash
# See conflict markers in files
git status

# Use merge tool
git mergetool

# Or manually edit and
git add resolved_file
git commit
```

#### Accidental Commit to Main
```bash
# Create branch from current state
git branch fix/accidental-commit

# Reset main to upstream
git checkout main
git reset --hard upstream/main

# Cherry-pick your commit to new branch
git checkout fix/accidental-commit
```

#### Large File Accidentally Committed
```bash
# Remove from history (destructive!)
git filter-branch --tree-filter 'rm -f path/to/large/file' HEAD

# Or use git-filter-repo (recommended)
git filter-repo --path path/to/large/file --invert-paths
```

### Getting Help

```bash
# Built-in help
git help <command>
git <command> --help

# Quick help
git <command> -h

# Visual history
git log --graph --oneline --all

# See what changed
git diff  # Unstaged changes
git diff --staged  # Staged changes
git diff main...HEAD  # All branch changes
```

## Team Guidelines

### Do's
- ✅ Commit early and often locally
- ✅ Write clear commit messages
- ✅ Keep branches up to date
- ✅ Review your own PR before requesting review
- ✅ Test before pushing

### Don'ts
- ❌ Force push to shared branches
- ❌ Commit sensitive data
- ❌ Leave commented code
- ❌ Mix refactoring with features
- ❌ Ignore CI failures

### Security
- Never commit credentials
- Use `.gitignore` for sensitive files
- Review diff before committing
- Use `git add -p` for selective staging

## Summary

Our Git workflow emphasizes:
- **Clean history** through atomic commits
- **Clear communication** via conventional commits
- **Safe collaboration** with protected branches
- **Quality assurance** through PR reviews

Remember: Git history is documentation. Make it useful for future developers (including yourself)!