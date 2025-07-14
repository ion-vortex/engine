# AGENT.md  
*Rules and check-list for any human or automated agent that touches the **Oxide** code-base.*

---

## 1.  Prime directive
> **Do not widen project scope.**  
> Every change must push us toward the stated goals in `README.md` without introducing new external dependencies, subsystems, or pattern creep.
> Be sure to follow all the instructions in `docs/CPP_MANUAL.md`.

---

## 2.  Library boundaries
| Layer           | May call                | Must NOT call                   |
|-----------------|-------------------------|---------------------------------|
| `libs/core`     | – (bottom layer)        | anything else                   |
| `libs/crypto`   | `core`                  | net, render, ui, apps           |
| `libs/net`      | `core`                  | render, ui                      |
| `libs/asset`    | `core`, `crypto`, `net` | render, ui                      |
| `libs/physics`.     | `core`                  | net, ui, asset                  |
| `libs/render`   | `core`, `physics` (*)       | net, ui (except texture id)     |
| `libs/ui`       | `core`, `render`        | net, physics                    |
| `libs/platform` | `core`                  | anything but core               |
| `apps/*`        | all libs via interfaces | –                               |

(*) `render` only sees physics **read-only** for debug draw helpers.

### Sanity macro
Every `.cpp` must start with:

```cpp
#include "oxide/layer_assert.h"  // static_asserts that enforce the table above
````

---

## 3.  Coding contract (superset of `docs/CPP_MANUAL.md`)

* No exceptions across public boundaries (`std::expected` only).
* No RTTI (`dynamic_cast`, `typeid`) in libs.
* No singletons.  Inject via factory or pass reference.
* `std::shared_ptr` allowed only in UI code for Ultralight JS bindings – nowhere else.
* Public headers: `#pragma once`, no `using namespace std`.
* Build must pass `-Wall -Wextra -Werror -Wshadow -Wconversion`.

---

## 4.  Platform layer (`libs/platform`)

* GLFW is the only backend for now.  Hide it behind `IWindow` and `input()` as specified in README.
* Do **not** pull bgfx headers into platform.  Pass native window handle (`void*`) upward.
* Each poll cycle must fill `InputState` *once*; higher layers read but never mutate it.
* In fact, for each internal library, we should *never* expose third party headers. It should wrap any external library properly in an interface so we chan swap them out if necessary.

---

## 5.  Security checklist

* All external HTTP(S) fetches must use `rune::net::HttpsClient` (libcurl wrapper) with CA-pinning.
* New cryptographic code goes through `rune::crypto` only—never open-code libsodium calls.
* Never log secrets or private keys; redact before emitting to `ILog`.

---

## 6.  Large file / history hygiene

* No asset larger than 10 MiB in Git history.
* If you accidentally commit one:

  ```
  git filter-repo --path path/to/large.file --invert-paths
  git push --force-with-lease
  ```

  Then add it to `.gitignore` and `.gitattributes` (git lfs)

---

## 7.  Documentation update required

If you change:

* a public header in `libs/<lib>/include/`
* runtime behavior of any daemon
* the build workflow

then update the relevant section in `README.md` or `docs/`, and bump the **minor** version in `vcpkg.json`.