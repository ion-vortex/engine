# Legal Documentation

This directory contains all legal documents related to Ion Vortex and contributions to it.

## Overview

Ion Vortex is an open-source project licensed under the **GNU Affero General Public License v3.0 (AGPL-3.0)**. This means:

- ✅ You can use, modify, and distribute the source code
- ✅ You can use Ion Vortex for commercial projects
- ⚠️ Any modifications must be released under the same AGPL-3.0 license
- ⚠️ If you run a modified version as a network service, you must provide the source code

## Documents in This Directory

### 📄 [Contributor License Agreement (CLA)](CLA.md)

Before contributing to Ion Vortex, all contributors must sign the Contributor License Agreement. This ensures:

- You have the right to contribute your code
- Oat Interactive can use and distribute your contributions
- The project remains legally clear for all users

The CLA is available in multiple formats:
- **[CLA.md](CLA.md)** - Markdown version (recommended for reading)
- **[CLA.pdf](CLA.pdf)** - PDF version for printing and signing
- **[CLA.docx](CLA.docx)** - Word version for digital signing

### How to Sign the CLA

1. Read the [CLA.md](CLA.md) carefully
2. Choose your contributor type:
   - **Individual Contributor**: You own the IP rights to your contributions
   - **Employer-Sponsored Contributor**: Your employer owns the IP rights
3. Fill out the appropriate section at the bottom of the document
4. Submit the signed CLA via one of these methods:
   - Email to: dev@oat.im
   - Include with your first pull request
   - Submit through the contributor portal (if available)

## Licensing Summary

### Source Code (AGPL-3.0)

All C++ source code in this repository is licensed under AGPL-3.0. This includes:
- All files in `src/`, `include/`, `apps/`, `libs/`
- Build scripts and CMake files
- Test code
- Documentation

**Key Requirements:**
- Include AGPL-3.0 notice in distributed code
- Provide source code for any modifications
- Network use counts as distribution

### Assets

Assets have different licenses:

#### Public Assets (CC0)
- Location: `assets/public/`
- License: Creative Commons Zero (Public Domain)
- Usage: No restrictions, use freely

#### Private Assets (Commercial)
- Location: `assets/private/` (not in repository)
- License: Commercial, proprietary
- Usage: Oat Interactive employees only
- Includes: Internal art

**Important**: Never commit private assets to the public repository!

## Compliance Guidelines

### For Contributors

1. **Only contribute code you have rights to**
2. **Sign the CLA before your first contribution**
3. **Never include proprietary assets in commits**
4. **Respect third-party licenses in dependencies**

### For Users

1. **Follow AGPL-3.0 requirements for source code**
2. **Check asset licenses before use**
3. **Maintain copyright notices**
4. **Provide source access for network services**

### For Commercial Users

If you want to use Ion Vortex in a commercial product:

1. **Source Code**: You must comply with AGPL-3.0 (share modifications)
2. **Public Assets**: Free to use commercially (CC0)
3. **Private Assets**: Require separate commercial license

For commercial licensing inquiries: dev@oat.im

## Third-Party Licenses

Ion Vortex uses various third-party libraries via vcpkg. Each has its own license:

- **SDL3**: Zlib
- **Dear ImGui**: MIT
- **GLM**: MIT
- **GLFW**: Zlib
- **libuv**: MIT
- **nlohmann-json**: MIT
- **tomlplusplus**: MIT
- **curl**: curl license
- **libsodium**: ISC
- **stb**: MIT/Public Domain

Full license texts are included with each library via vcpkg.

## Frequently Asked Questions

### Q: Can I use Ion Vortex in my commercial game?
A: This is legally complex. AGPL has no linking exception, so any software that links against Ion Vortex libraries may be considered a derivative work that must also be AGPL. Consult legal counsel before using Ion Vortex commercially.

### Q: Do I need to open-source my entire game if I use Ion Vortex?
A: **Probably yes.** AGPL is designed to prevent proprietary use. Unlike LGPL, it has no linking exception. Most legal interpretations suggest that linking against AGPL libraries makes your entire application subject to AGPL. This is why AGPL is rarely used for libraries intended for proprietary software.

### Q: Why use AGPL instead of LGPL or MIT?
A: AGPL ensures that:
- No one can create a closed-source fork of Ion Vortex
- Any improvements to the engine must be shared back
- Commercial competitors cannot use our work without contributing back

If you want to use Ion Vortex in proprietary software, you'll need a commercial license.

### Q: Can I use the public assets in my commercial project?
A: Yes, public assets are CC0 (public domain) and free for any use, regardless of how you use Ion Vortex itself.

### Q: What if I want to keep my modifications or game private?
A: You have two options:
1. **Commercial License**: Contact us at dev@oat.im for proprietary licensing
2. **Don't use Ion Vortex**: If you need a permissively licensed engine, Ion Vortex isn't for you

### Q: Can I run a game server using Ion Vortex without releasing my game's source?
A: No. AGPL's "network use is distribution" clause means running a server counts as distribution. You must provide source code access to all users.

### Q: What about dynamic linking?
A: AGPL makes no distinction between static and dynamic linking. Both are likely to create derivative works that must be AGPL.

### Q: Can I contribute if my employer owns my code?
A: Yes, use the "Employer-Sponsored Contributor" section of the CLA.

### Q: What about AI-generated code?
A: See [Contributing Guidelines](../contributing/ai-policy.md) for our AI policy.

## Legal Contact

For legal questions not covered here:
- Email: dev@ionvortex.com
- Include "Ion Vortex Legal Question" in subject line

## Disclaimer

This README provides a summary for convenience but is not legal advice. The interpretation of AGPL's linking and derivative work provisions is complex and contentious. **Always consult with legal counsel** before using AGPL software in any commercial or proprietary context.

The AGPL-3.0 license text is authoritative and supersedes any summaries or interpretations in this document.

**The AGPL was specifically chosen to prevent proprietary/commercial use without a separate commercial license agreement.**