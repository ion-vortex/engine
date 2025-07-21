# Code Review Process

This document outlines the code review process for Ion Vortex contributions. Our goal is to maintain high code quality while being respectful of contributors' time and effort.

## Overview

All code changes must go through peer review via GitHub Pull Requests. This ensures:
- Code quality and consistency
- Knowledge sharing across the team
- Bug prevention through multiple perspectives
- Adherence to project standards

## Pull Request Lifecycle

### 1. Pre-Submission Checklist

Before opening a PR, ensure:
- [ ] Code follows the [C++ Manual](../development/cpp-standards.md)
- [ ] All tests pass locally
- [ ] New code has appropriate tests
- [ ] Documentation is updated
- [ ] Commit messages follow conventions
- [ ] Branch is up-to-date with `main`
- [ ] CLA is signed (first-time contributors)

### 2. Opening a Pull Request

#### PR Title Format
```
type(scope): brief description
```

Examples:
- `feat(render): add sprite batching support`
- `fix(network): resolve packet fragmentation issue`
- `docs(build): clarify vcpkg setup steps`

#### PR Template
```markdown
## Description
Brief description of what this PR does.

## Motivation
Why is this change needed? What problem does it solve?

## Changes
- List of key changes
- Implementation approach
- Any design decisions

## Testing
How has this been tested?
- [ ] Unit tests added/updated
- [ ] Integration tests added/updated
- [ ] Manual testing performed

## Checklist
- [ ] Code follows C++ Manual
- [ ] Tests pass
- [ ] Documentation updated
- [ ] No compiler warnings
- [ ] Performance impact considered

Fixes #issue_number
```

### 3. Automated Checks

GitHub Actions automatically runs:
- **Build Matrix**: Linux, Windows, macOS
- **Compiler Matrix**: GCC, Clang, MSVC
- **Test Suite**: All unit and integration tests
- **Static Analysis**: clang-tidy, cppcheck
- **Format Check**: clang-format compliance
- **Documentation Build**: Doxygen generation

All checks must pass before review.

### 4. Review Assignment

- **Small PRs** (<100 lines): 1 reviewer required
- **Medium PRs** (100-500 lines): 2 reviewers required
- **Large PRs** (>500 lines): 2 reviewers + architecture review
- **Breaking Changes**: Require maintainer approval

Reviewers are auto-assigned based on:
- Code ownership (CODEOWNERS file)
- Expertise areas
- Availability

## Review Guidelines

### For Authors

#### Keep PRs Small and Focused
- One feature/fix per PR
- Split large changes into logical chunks
- Refactoring separate from functional changes

#### Respond to Feedback
- Address all comments before re-requesting review
- Use "Resolved" threads for addressed feedback
- Ask for clarification if feedback is unclear
- Be open to suggestions

#### Update Based on Feedback
```bash
# Add changes based on review
git add -p  # Stage selectively
git commit --fixup=HEAD  # Create fixup commit

# Before merging, clean up history
git rebase -i --autosquash main
```

### For Reviewers

#### Review Priorities
1. **Correctness**: Does it work as intended?
2. **Design**: Does it fit the architecture?
3. **Standards**: Does it follow our guidelines?
4. **Performance**: Are there efficiency concerns?
5. **Security**: Are there vulnerabilities?
6. **Maintainability**: Is it easy to understand?

#### Providing Feedback

Use GitHub's review comment prefixes:
- **`[MUST]`**: Required change for approval
- **`[SHOULD]`**: Strongly recommended
- **`[CONSIDER]`**: Suggestion to think about
- **`[NIT]`**: Minor style/preference
- **`[QUESTION]`**: Seeking clarification
- **`[PRAISE]`**: Highlighting good work

Example:
```cpp
// [MUST] This violates L.4 from the C++ Manual - use std::expected
// [MUST] This violates CSH.1 from the C++ Manual - use `snake_case`
Widget* CreateWidget() {
    return new Widget();  
}

// [SHOULD] Consider using a more descriptive name
int calc(int x) {
    return x * 2;
}

// [NIT] Prefer pre-increment for iterators
for (auto it = vec.begin(); it != vec.end(); it++) {
    // ...
}
```

#### Review Etiquette
- Be constructive and respectful
- Explain the "why" behind suggestions
- Acknowledge good solutions
- Focus on the code, not the person
- Respond within 2 business days

## Common Review Points

### C++ Specific
- [ ] No naked `new`/`delete` - use `std::unique_ptr`
- [ ] Factory functions return `std::expected<std::unique_ptr<T>, std::error_code>`
- [ ] No exceptions escaping module boundaries
- [ ] Proper `[[nodiscard, gnu::warn_unused_result]]
` usage
- [ ] Interface segregation (pure virtual interfaces)
- [ ] No `std::shared_ptr` without justification

### Architecture
- [ ] Follows layer boundaries (see AGENT.md)
- [ ] No circular dependencies
- [ ] Proper dependency injection
- [ ] Clear ownership semantics

### Performance
- [ ] No allocations in hot paths
- [ ] Appropriate data structures
- [ ] Cache-friendly layouts
- [ ] SIMD opportunities identified

### Testing
- [ ] Edge cases covered
- [ ] Error paths tested
- [ ] No flaky tests
- [ ] Meaningful test names

## Special Cases

### Breaking Changes
Require:
- Maintainer approval
- Migration guide
- Version bump
- CHANGELOG entry
- Announcement in PR description

### Performance-Critical Code
Additional requirements:
- Benchmark results
- Profiling data
- Performance regression tests
- Architecture team review

### Security-Sensitive Code
Must have:
- Security team review
- Threat model consideration
- Additional security tests
- No credentials in code

### AI-Generated Code
See [AI Policy](ai-policy.md). Must include:
- `[AI-GENERATED]` label
- Full review regardless of size
- Extra scrutiny for correctness

## Merge Requirements

### Approval Criteria
- Required number of approvals met
- All CI checks passing
- All review comments addressed
- No unresolved conversations
- Up-to-date with target branch

### Merge Strategy
We use **squash and merge** for:
- Clean, linear history
- Easy reverts if needed
- Clear commit messages

Final commit message format:
```
feat(render): add sprite batching support (#123)

* Implement batching for static sprites
* Add configuration for batch size
* Update documentation

Performance improvement: 40% reduction in draw calls
Fixes #122

Co-authored-by: Reviewer Name <email>
```

## Post-Merge

### Author Responsibilities
- Monitor CI on main branch
- Be available for quick fixes
- Update related documentation
- Close related issues

### If Problems Arise
1. **Revert first** if breaking main
2. **Fix forward** in new PR
3. **Communicate** in team channels
4. **Post-mortem** for significant issues

## Review Metrics

We track:
- Time to first review
- Time to merge
- Number of review rounds
- Post-merge defect rate

Goals:
- First review within 24 hours
- Merge within 3 days for small PRs
- Less than 3 review rounds
- Zero post-merge reverts

## FAQ

### Q: What if reviewers disagree?
A: Discussion should reach consensus. If not, escalate to maintainers.

### Q: Can I self-merge urgent fixes?
A: Only for critical security fixes, and only with maintainer status.

### Q: How do I become a reviewer?
A: Contribute quality code, show good judgment in discussions, get nominated.

### Q: What about documentation-only PRs?
A: Still need review, but relaxed requirements (1 reviewer, format checks optional).

### Q: Can I review my own PR?
A: You can respond to comments, but cannot approve your own PR.

## Tools and Resources

### Recommended Tools
- **GitHub CLI**: `gh pr create`, `gh pr review`
- **VS Code GitHub PR Extension**: Review in editor
- **Refined GitHub**: Browser extension for better PR experience

### Review Commands
```bash
# Check out PR locally
gh pr checkout 123

# Run tests for PR
cmake --build --preset debug --target test

# View PR diff
gh pr diff 123

# Approve PR from CLI
gh pr review 123 --approve
```

## Summary

Good code review is about:
- **Quality**: Maintaining high standards
- **Learning**: Sharing knowledge
- **Respect**: Being constructive
- **Efficiency**: Not blocking progress

Remember: Code review is a conversation, not a judgment. We're all working toward the same goal of making Ion Vortex better.