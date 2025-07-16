# Contributing to Oxide

Thank you for your interest in contributing to Oxide! This guide will help you get started with contributing code, documentation, or other improvements to the project.

## Before You Start

### Legal Requirements

**All contributors must sign the [Contributor License Agreement (CLA)](../legal/CLA.md)** before their first contribution can be accepted. This ensures:
- You have the right to contribute your code
- Oat Interactive can use and distribute your contributions
- The project remains legally clear for all users

See the [Legal Documentation](../legal/) for details on signing the CLA.

### Code of Conduct

We expect all contributors to:
- Be respectful and professional
- Focus on technical merit
- Help maintain a welcoming environment
- Follow the project's engineering standards

## Ways to Contribute

### 🐛 Bug Reports
Found a bug? [Open an issue](https://github.com/oat-im/oxide/issues) with:
- Clear description of the problem
- Steps to reproduce
- Expected vs actual behavior
- System information (OS, compiler, etc.)

### 💡 Feature Requests
Have an idea? Open an issue describing:
- The problem you're trying to solve
- Your proposed solution
- Why this benefits the project

### 📝 Documentation
Documentation improvements are always welcome:
- Fix typos or clarify existing docs
- Add examples or tutorials
- Improve API documentation
- Translate documentation (coordinate first)

### 💻 Code Contributions
Ready to code? Check out:
- [Code Review Process](code-review.md) - How we review PRs
- [Git Workflow](git-workflow.md) - Branching and commit standards
- [C++ Standards](../development/cpp-manual.md) - Our coding standards
- [Build System](../development/build-system.md) - How to build and test

## Getting Started

### 1. Fork and Clone
```bash
# Fork on GitHub, then:
git clone https://github.com/YOUR_USERNAME/oxide.git
cd oxide
git remote add upstream https://github.com/oat-im/oxide.git
```

### 2. Create a Branch
```bash
git checkout -b feature/your-feature-name
# or
git checkout -b fix/issue-description
```

### 3. Make Your Changes
- Follow the [C++ Manual](../development/cpp-manual.md)
- Add tests for new functionality
- Update documentation as needed
- Keep commits focused and atomic

### 4. Test Your Changes
```bash
# Build and run tests
cmake --preset debug -DOXIDE_BUILD_TESTS=ON
cmake --build --preset debug
ctest --preset debug
```

### 5. Submit a Pull Request
- Push to your fork
- Open a PR against `main`
- Fill out the PR template completely
- Wait for CI to pass

## Contribution Standards

### Code Quality
- **Follow the [C++ Manual](../development/cpp-manual.md)** - No exceptions
- **Write tests** - New features need tests
- **Document APIs** - Public headers need documentation
- **No compiler warnings** - Code must build clean

### Commit Messages
Follow conventional commits:
```
type(scope): brief description

Longer explanation if needed. Wrap at 72 characters.

Fixes #123
```

Types: `feat`, `fix`, `docs`, `style`, `refactor`, `test`, `build`, `ci`, `perf`

### Pull Request Guidelines
- **One feature per PR** - Keep PRs focused
- **Link related issues** - Use "Fixes #123" in description
- **Update CHANGELOG** - Add entry for user-visible changes
- **Pass all CI checks** - No failing tests or builds

### AI-Generated Content
See our [AI Policy](ai-policy.md) for rules on AI-generated contributions.

## Review Process

### What to Expect
1. **Automated Checks** - CI runs immediately
2. **Initial Review** - Maintainer reviews within 2-3 days
3. **Feedback Round** - Address review comments
4. **Final Approval** - Two maintainer approvals required
5. **Merge** - Maintainer merges when ready

### Review Criteria
- **Correctness** - Does it work as intended?
- **Design** - Does it follow project architecture?
- **Standards** - Does it follow our coding standards?
- **Tests** - Are changes adequately tested?
- **Documentation** - Are changes documented?

## Development Environment

### Recommended Setup
- **IDE**: VS Code with C++ extensions or CLion
- **Compiler**: Latest stable (GCC 13+, Clang 16+, MSVC 2022)
- **Tools**: clang-format, clang-tidy, cmake-format
- **OS**: Linux (primary), Windows, macOS

### Helpful Commands
```bash
# Format code
clang-format -i src/**/*.cpp include/**/*.h

# Run linter
clang-tidy src/**/*.cpp

# Build specific target
cmake --build --preset debug --target oxide_core

# Run specific test
./build/debug/tests/core_tests --gtest_filter=ErrorTest.*
```

## Communication Channels

### Where to Ask Questions
- **GitHub Issues** - Bug reports and features
- **GitHub Discussions** - General questions
- **Discord** - Real-time chat (if available)
- **Email** - dev@oat.im for private matters

### Response Times
- **Issues**: 2-3 business days
- **PRs**: Initial review within 3 business days
- **Security issues**: Within 24 hours (see [Security Policy](../../SECURITY.md))

## Recognition

### Contributors
All contributors are recognized in:
- Git history (use your real name/email)
- CONTRIBUTORS.md file
- Release notes for significant contributions

### Becoming a Maintainer
Active contributors may be invited to become maintainers based on:
- Quality and quantity of contributions
- Understanding of project architecture
- Commitment to project values
- Community involvement

## Need Help?

- Read the [Getting Started Guide](../getting-started/)
- Check the [Development Documentation](../development/)
- Search existing [Issues](https://github.com/oat-im/oxide/issues)
- Ask in [Discussions](https://github.com/oat-im/oxide/discussions)

Remember: There are no stupid questions, but there are poorly researched ones. Do your homework, and we're happy to help!

## Quick Links

- [Code Review Process](code-review.md)
- [Git Workflow](git-workflow.md)
- [AI Policy](ai-policy.md)
- [C++ Standards](../development/cpp-manual.md)
- [Build System](../development/build-system.md)
- [Legal/CLA](../legal/)