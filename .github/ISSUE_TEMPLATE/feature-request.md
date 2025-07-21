---
name: Feature Request
about: Suggest a new feature for Ion Vortex
title: "[FEATURE] Brief description of the feature"
labels: enhancement
assignees: raygoe

---

## Description

**Summary:**
A clear and concise description of the feature you'd like to see.

**Problem Statement:**
What problem does this feature solve? What use case does it address?

**Proposed Solution:**
Describe your preferred solution or approach.

**Alternative Solutions:**
Describe any alternative solutions you've considered.

## Milestone Alignment

**Does this feature align with current project priorities?**
- [ ] Directly supports Tier 0 milestone (ship spawning, steering, networking, ImGui)
- [ ] Supports future milestone (R1: client prediction, R2: multi-zone, R3: public alpha)
- [ ] General improvement/quality of life
- [ ] New capability beyond current roadmap

**If this is beyond current milestones, why should it be prioritized?**
Explain the strategic value or critical need.

## Technical Scope

**Which Ion Vortex modules would be affected?** (check all that apply)
- [ ] `ion::core` (logging, error handling, lifecycle)
- [ ] `ion::render` (SDL3, rendering backend)
- [ ] `ion::ui` (ImGui integration)
- [ ] `ion::physics` (SAT collision detection)
- [ ] `ion::protocol` (networking protocol)
- [ ] `ion::asset` (asset loading)
- [ ] `ion::audio` (SDL3 audio)
- [ ] `ion::crypto` (cryptographic utilities)
- [ ] `apps/client` (SDL3 + ImGui client)
- [ ] `apps/zoned` (headless zone server)
- [ ] `apps/unid` (zone router/login)
- [ ] Build system (CMake, vcpkg)
- [ ] Documentation
- [ ] New module needed

**Would this require new external dependencies?**
- [ ] No new dependencies
- [ ] New vcpkg dependencies (list below)
- [ ] System dependencies (list below)

**List any new dependencies:**
```
Dependency name and justification
```

## Implementation Considerations

**Estimated Implementation Complexity:**
- [ ] Small (< 1 day, single module)
- [ ] Medium (1-3 days, multiple modules)
- [ ] Large (> 3 days, significant architecture changes)
- [ ] Unknown/Needs investigation

**Breaking Changes:**
- [ ] No breaking changes expected
- [ ] Minor breaking changes (interface additions)
- [ ] Major breaking changes (interface modifications)
- [ ] Architecture changes required

**Performance Impact:**
- [ ] No performance impact expected
- [ ] Minimal performance impact
- [ ] Significant performance implications (explain below)

**Performance considerations:**
```
Describe any performance implications
```

## Design Approach

**API Design Sketch:** (if applicable)
```cpp
// Rough API design or interface outline
namespace ion::module {
    class new_feature_base {
    public:
        virtual ~new_feature_base() = default;
        virtual std::expected<Result, std::error_code> do_something() = 0;
    };
    
    std::expected<std::unique_ptr<new_feature_base>, std::error_code> make_new_feature();
}
```

**Primary Use Case:**
Describe the main scenario where this feature would be used.

**Secondary Use Cases:**
List additional scenarios that would benefit from this feature.

**Example Usage:**
```cpp
// Show how the feature would be used in practice
auto feature = ion::module::make_new_feature();
if (!feature) {
    return feature.error();
}
auto result = feature->do_something();
```
