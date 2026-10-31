# Original ROCCAT Power-Grid SDK v1.1

This directory preserves the **official, unmodified ROCCAT Power-Grid SDK v1.1 archive** and its original PDF guides. The SDK and documentation are historical reference material, distinct from the project's modern [Plugin Dev Kit](../../Plugin%20Dev%20Kit/) and restoration patches.

## Download and read

- [Power-Grid_SDK_v1.1.zip](Power-Grid_SDK_v1.1.zip) — original SDK archive, including C++ headers/import libraries, sample plugins, source, build artefacts and the original Doxygen HTML documentation.
- [Power-Grid Plugin Building Guide.pdf](Power-Grid%20Plugin%20Building%20Guide.pdf) — official 22-page guide, Version B (January 2013); plugin namespace, controls, control sets, skins, graphics, scripts, DLLs and configuration options.
- [documentation.pdf](documentation.pdf) — official eight-page *Power-Grid SDK Plugin Building Guide v0.1*, dated August 2015; host/client and plugin architecture, plugin.json, JavaScript and custom DLLs.
- [Technical reference index](TECHNICAL_REFERENCE.md) — an independently written navigation guide to the preserved documentation and SDK structure.

The two PDFs are exact byte-for-byte copies of the files at `docs/Power-Grid Plugin Building Guide.pdf` and `docs/documentation.pdf` in the ZIP, surfaced separately for convenient reading.

## Original Doxygen manuals

Extract the ZIP to browse these locally (they use accompanying CSS/JavaScript/image assets):

- **C++ / native DLL API:** `docs/dox_cpp/html/index.html`
- **JavaScript / .qs API:** `docs/dox_qs/html/index.html`

The SDK's `docs/Power-Grid Dynamic Library Reference Manual.html` and `docs/Power-Grid JavaScript Reference Manual.html` are original entry-point files for those two Doxygen references.

## Version and provenance notes

The recovered archive is distributed as **SDK v1.1**, but the embedded `LICENSE.txt` identifies the package as **v1.0, December 19, 2013**. Its contents also contain file timestamps from 2015. Those historical inconsistencies are preserved as found; no attempt has been made to silently amend original materials.

The original licence covers the source-code portions and `RoccatPowerGrid.lib`, grants redistribution subject to attribution and retaining the notice, and warns about statically linking Qt. Read `LICENSE.txt` inside the ZIP for its full terms. Original trademarks, documentation and third-party material remain attributed to their rights holders.

These are archival materials; they are **not** a claim of compatibility with current build tools or the restored host, and the bundled example DLLs have not been independently validated as safe or functional.
