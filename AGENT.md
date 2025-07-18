# AGENT.md

*Rules and check-list for every human **or** automated agent that touches the **Ion Vortex** code-base.*
*(This file is surfaced to both ChatGPT / Claude agents via project settings — keep it tight, explicit, and up-to-date.)*

---

## 1   Prime directive

> **Stay within project scope.**
> Every change **must** push toward the milestones in `README.md`.
> No new external libs, no speculative subsystems, no pattern creep.
> Obey `docs/CPP_MANUAL.md` *and* the constraints below.

---

## 2   Library boundaries

| Layer           | May call                           | **Must NOT** call                |
| --------------- | ---------------------------------- | -------------------------------- |
| `libs/core`     | – (bottom layer)                   | anything else                    |
| `libs/crypto`   | `core`                             | asset, physics, render, ui       |
| `libs/asset`    | `core`, `crypto`                   | physics, render, ui              |
| `libs/physics`  | `core`                             | asset, render, ui, protocol      |
| `libs/protocol` | `core`, `crypto`                   | render, ui, physics              |
| `libs/audio`    | `core`                             | render, ui                       |
| `libs/render`   | `core`, `physics` *(read-only)*    | protocol, ui (except imgui draw) |
| `libs/ui`       | `core`, `render`                   | protocol, physics, crypto        |
| `apps/*`        | all libs **via public interfaces** | –                                |

### Sanity macro

Every `*.cpp` **must** start with:

```cpp
#include "ion/layer_assert.h"   // static_asserts the table above
```

---

## 3   Coding contract (superset of `docs/CPP_MANUAL.md`)

* **No exceptions** across public boundaries (`std::expected` only).
* **No RTTI** (`dynamic_cast`, `typeid`) in libs.
* **No singletons.**  Inject via factory or pass a reference.
* **No `std::shared_ptr`** in engine libs.  Use `std::unique_ptr`; UI (ImGui) is immediate-mode and does not require shared ownership.
* Public headers: `#pragma once`, no `using namespace std`.
* Must compile with `-Wall -Wextra -Werror -Wshadow -Wconversion` (or MSVC equivalents).
* Use CMake ≥ **3.28**.  Unity builds (`CMAKE_UNITY_BUILD`) and PCH are on by default via `CMakePresets.json`.

---

## 4   AI-generated content policy  *(mirrors CONTRIBUTING.md)*

1. **AI-generated art assets are strictly forbidden.**
2. **AI-generated code is allowed** *only* when **all** of the following hold:

   * PR description clearly states *“contains AI-generated code”*.
   * Author fully understands the code — no “vibe-coded” blobs.
   * A complete PR review is provided, regardless of change size; assume AI will be wrong.

PRs that violate any of the above are closed without review.

---

## 5   Security checklist

* All outbound HTTP(S) uses `ion::protocol::HttpsClient` (libcurl wrapper) with CA-pinning.
* Crypto work goes through `libs/crypto` only — never inline libsodium calls.
* Never log secrets or private keys.  Mask before calling `ILog`.

---

## 6   Large file / history hygiene

* **No file > 10 MiB** in Git history.
* If you slip one in:

  ```bash
  git filter-repo --path path/to/big.file --invert-paths
  git push --force-with-lease
  ```

  Then add it to `.gitignore` and/or Git-LFS.

---

## 7   Documentation + version bump

If you touch **any** of:

* public headers in `libs/**/include/`
* runtime behaviour of `apps/client`, `apps/zoned`, `apps/unid`
* the build workflow, presets, or triplets

then:

1. Update `README.md` **and** relevant docs under `docs/`.
2. Bump the **minor** version in `vcpkg.json`.

---

## 8   Build quick-check

```bash
# configure + build debug preset
cmake --preset debug
cmake --build --preset debug --target client

# run the sandbox
./out/build/debug/apps/client --dev
```

The CI pipeline runs the same commands; if they fail locally they will fail in CI.