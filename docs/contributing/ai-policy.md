# AI-Generated Content Policy

This document outlines Ion Vortex's policy on AI-generated content in contributions.

## Overview

We recognize that AI tools can assist in software development, but we require transparency and human oversight for all AI-generated contributions to maintain code quality, security, and legal clarity.

## Policy Summary

### 🎨 AI-Generated Art Assets
**Status: STRICTLY FORBIDDEN**

AI-generated art assets (sprites, textures, models, etc.) are not accepted due to:
- Unresolved copyright concerns
- Potential training data disputes
- Quality and consistency requirements
- Legal liability risks

### 💻 AI-Generated Code
**Status: ALLOWED WITH CONDITIONS**

AI-generated code is permitted only when ALL of the following conditions are met:

1. **Clear Disclosure**: PR description must state "Contains AI-generated code"
2. **Human Understanding**: Contributor fully understands and can explain the code
3. **Complete Review**: Full code review regardless of PR size
4. **Human Verification**: Contributor has tested and verified the code

## Detailed Requirements

### For Contributors

#### 1. Disclosure Requirements

Your PR description MUST include:
```markdown
## AI Disclosure
This PR contains AI-generated code:
- **Tool Used**: [e.g., GitHub Copilot, ChatGPT, Claude]
- **Scope**: [Which parts were AI-generated]
- **Modifications**: [Human changes made to AI output]
```

Example:
```markdown
## AI Disclosure
This PR contains AI-generated code:
- **Tool Used**: GitHub Copilot
- **Scope**: Initial CMake configuration for the physics module
- **Modifications**: Fixed target names, added missing dependencies, 
  updated to follow project conventions
```

#### 2. Understanding Requirement

You must:
- Understand every line of code you're submitting
- Be able to explain design decisions
- Know why the code works the way it does
- Be prepared to debug and maintain it

**"The AI wrote it" is never an acceptable answer to a review question.**

#### 3. Quality Standards

AI-generated code must meet all the same standards as human-written code:
- Follow the [C++ Manual](../development/cpp-standards.md)
- Include appropriate tests
- Have proper documentation
- Build without warnings
- Pass all CI checks

### For Reviewers

#### Review Process for AI-Generated Code

1. **Verify Disclosure**: Ensure AI use is properly disclosed
2. **Extra Scrutiny**: Review more carefully than usual
3. **Test Understanding**: Ask questions about design decisions
4. **Check for Issues**: Watch for common AI problems (see below)
5. **Verify Testing**: Ensure thorough test coverage

#### Red Flags in AI-Generated Code

Watch for:
- **Inconsistent Style**: Mixing different coding conventions
- **Overcomplexity**: Unnecessarily complex solutions
- **Wrong Patterns**: Using patterns inappropriate for C++
- **Memory Issues**: Poor memory management
- **Missing Error Handling**: Incomplete error paths
- **Fictional APIs**: Calls to non-existent functions
- **License Pollution**: Code that might be from other projects

#### Example Review Comments

```cpp
// [QUESTION] This seems like an unusual approach for C++. 
// Why was std::variant chosen here instead of inheritance?

// [MUST] This looks like it might be from a different project
// (uses camelCase instead of our snake_case). Please verify origin.

// [MUST] AI hallucination detected: std::filesystem::recursive_rename
// doesn't exist. Use correct API.
```

## Common AI Tools

### Acceptable Uses

✅ **GitHub Copilot/Codeium**
- Autocompletion
- Boilerplate generation
- Test case generation
- Documentation comments

✅ **ChatGPT/Claude**
- Algorithm explanations
- Debugging assistance
- Documentation writing
- Code review preparation

✅ **Specialized Tools**
- CMake generators
- Protobuf generators
- Binding generators

### Unacceptable Uses

❌ **Copy-Paste Programming**
- Blindly copying AI output
- Not understanding the code
- Submitting without testing

❌ **Bulk Generation**
- Entire modules from AI
- Mass refactoring via AI
- Automated PR creation

❌ **Hiding AI Use**
- Not disclosing AI assistance
- Claiming AI work as your own
- Obfuscating AI origins

## Best Practices

### 1. Use AI as a Tool, Not a Replacement

```cpp
// ❌ Bad: Just copy AI output
auto result = some_complex_ai_generated_function_i_dont_understand();

// ✅ Good: Understand and improve AI suggestions
// AI suggested basic version, but I added error handling and
// adapted to our std::error_code/std::expected pattern
[[OAT_NODISCARD("Please remember to verify the std::expected result.")]]
std::expected<processed_data, std::error_code> process_data(buffer_view input) {
    if (input.empty()) {
        return std::unexpected(std::make_error_code(std::errc::bad_message));
    }
    
    // Core algorithm from AI, verified and optimized
    processed_data result;
    // ... implementation ...
    return result;
}
```

### 2. Review AI Output Critically

AI often:
- Uses outdated patterns
- Ignores project conventions
- Makes security mistakes
- Violates SOLID principles
- Generates inefficient code

Always review and improve.

### 3. Test Thoroughly

AI-generated code needs extra testing:
- Edge cases AI might miss
- Error conditions
- Performance characteristics
- Memory usage
- Thread safety

## Legal Considerations

### Copyright and Liability

By submitting AI-generated code, you certify that:
1. You have the right to submit the code
2. The code doesn't violate any licenses
3. You take responsibility for the code
4. You've verified it's not copied from other projects

### License Compatibility

Ensure AI-generated code:
- Doesn't include GPL/proprietary code
- Is compatible with AGPL-3.0
- Doesn't violate any patents
- Has clear provenance

## FAQ

### Q: Can I use AI to help write tests?
**A:** Yes, but you must understand the tests, verify they actually test the intended behavior, and ensure they're not flaky.

### Q: What if AI helps me understand an algorithm?
**A:** Learning from AI is fine. Using AI to explain concepts doesn't require disclosure. Only code that ends up in the PR needs disclosure.

### Q: Can AI review my code before I submit?
**A:** Yes, using AI for pre-review is encouraged. It can catch issues early. But it doesn't replace human review.

### Q: What about AI-generated documentation?
**A:** Allowed with disclosure. Must be technically accurate, follow project style, and be reviewed like code.

### Q: Can I use AI to refactor existing code?
**A:** Yes, with disclosure and extra care. Refactoring requires deep understanding of existing behavior to avoid regressions.

### Q: What if I'm unsure if something counts as "AI-generated"?
**A:** When in doubt, disclose. Better to over-communicate than under-communicate.

## Enforcement

### Violations

Failure to follow this policy may result in:
1. **First offense**: PR rejection, education on policy
2. **Repeated offenses**: Temporary contribution ban
3. **Intentional deception**: Permanent ban

### Appeals

If you believe a decision was made in error:
1. Discuss with the reviewer
2. Escalate to maintainers if needed
3. Final appeals to project leads

## Future Considerations

This policy will evolve as:
- AI tools improve
- Legal landscape clarifies
- Community standards develop
- New use cases emerge

We review this policy quarterly and welcome feedback.

## Summary

We embrace AI as a development tool while maintaining high standards for code quality, security, and legal compliance. The key principles are:

1. **Transparency**: Always disclose AI use
2. **Understanding**: Know your code inside and out
3. **Responsibility**: You own what you submit
4. **Quality**: AI doesn't lower the bar

Remember: AI is a powerful assistant, but you are the engineer. Use it wisely, and always prioritize understanding and quality over speed.