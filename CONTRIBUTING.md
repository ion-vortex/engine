# Contributing to Project Oxide

We love clean patches and brutal honesty. Please read this page **before** filing an issue or opening a pull‑request.

---

## 1 – Ground Rules

| Rule                                                           | Why it exists                                        |
| -------------------------------------------------------------- | ---------------------------------------------------- |
| **Follow the Oat Interactive C++ Manual (CPP\_MANUAL.md).**    | Same patterns, same expectations, easy review.       |
| **One feature branch per PR.**  No drive‑by commits on `main`. | Keeps history atomic & revert‑able.                  |
| **Never commit generated/binary files.**                       | Art packs, build artefacts, IDE junk ⇒ `.gitignore`. |

---

## 2 – AI Policy (Read **carefully**)

### 2.1 Art assets

- **AI‑generated art is strictly forbidden.**  Yes, even "it's just a concept sketch."  The legal surface is not worth the risk.

### 2.2 Code / Documentation

AI‑assisted code and documentation is acceptable **only if all of the following are true**:

1. **Clearly labelled.**  The PR title or description must start with `AI:` or contain a line `Contains AI‑generated code`.
2. **Understand it, or bin it.**  The contributor must be able to explain every line.  "ChatGPT spat it out" is not an explanation.
3. **Full PR required.**  No one‑liner GitHub web edits.  Even tiny fixes need a proper branch + description so reviewers can sanity‑check the machine output.
4. **You own the license.**  Verify your AI tool's terms allow AGPL v3 contributions.
5. **You need an CLA.** Remember that your PR will not be merged without a signed CLA.

If a reviewer suspects "vibe‑coded" paste, they'll bounce the PR.

## 3 – Commit Message Style

```
[module] concise title (<72 chars)

Motivation: why this change matters.
What was the bug / missing feature?

* Bullet list of important details.
* Reference issues with `Fixes #123`.
```

---

## 4 – Pull‑Request Review Flow

1. CI must be green (Linux, macOS, Windows jobs).  Red CI == no review.
2. At least one core maintainer must approve.
3. Squash‑merge unless multiple commits are truly meaningful.

We're ruthless but fair.  Ship quality code, skip the drama.

---

Happy hacking!

