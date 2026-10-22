# Plugin Development Kit — Technical Notes

This document records the engineering behind the Power-Grid PDK **v0.3.31**. For normal installation and plugin creation, start with the [README](README.md) or the PDK's built-in documentation.

## Background

ROCCAT Power-Grid's Windows application supports loadable native plugins. Its original development environment targeted 32-bit Windows and an older Visual C++/Qt toolchain. The preserved sample sources and plugin binaries, together with analysis of the Power-Grid 0.461 host, provided the basis for reconstructing the interface required to build working plugins today.

The PDK is a Windows desktop application built with **C# / .NET 8**, developed using **Visual Studio 2022**. Its native plugin build pipeline uses staged **Clang/LLVM 19**, **Qt 4.8.5 development components**, and support code and import libraries arranged for the original host's x86 ABI.

The purpose of the reconstruction is practical compatibility with the existing Power-Grid host, not a new plugin protocol or a replacement for Power-Grid itself.

## Architecture

```text
Power-Grid PDK (C# application)
  |-- Project browser, editor and validation
  |-- Manifest parser (strict and relaxed JSON syntax)
  |-- Native build orchestration
  |     |-- Qt moc
  |     |-- clang-cl (32-bit C++)
  |     |-- lld-link
  |     '-- llvm-readobj (binary inspection)
  |-- RPGP exporter
  '-- User preferences and build logs
```

The **running C# application** reads the manifest, resolves source files and build settings, launches the compiler-related executables, collects their diagnostics and produces the requested DLL. Plugin compilation is **not** orchestrated through a PowerShell build script or a second headless PDK process.

The build's native intermediates can include generated MOC sources, object files, an import library, linker map and inspection output. These are development artefacts; they do **not** belong inside an RPGP.

### Relevant distribution layout

```text
PowerGridPluginDevelopmentKit.exe
SDK/
  include/
    RoccatPowerGrid.h
Toolchain/
  llvm/
  qt/
  open/
  mingw/
  imports/
NativeBuild/
  Sample/
  VREServer/
```

The exact application output also contains the .NET runtime files and PDK documentation required by the distributable build.

## Native compatibility

The verified build path produces **32-bit PE/COFF DLLs** for Power-Grid's Windows host. The compiler is configured for the MSVC-compatible **x86 ABI** and the Qt interface expected by the original application.

Key compatibility considerations include:

- Matching the 32-bit MSVC class, calling and virtual-function layout used by the host.
- Compiling against compatible Qt 4 interfaces; Qt's Meta-Object Compiler generates metadata for QObject-derived plugin classes.
- Exporting the standard Qt 4 plugin factory and verification symbols, `qt_plugin_instance` and `qt_plugin_query_verification_data`.
- Linking the plugin to the Qt DLLs actually needed by the implementation (for example, `QtNetwork4.dll` for the VRE Server example).
- Supporting the original shared C runtime dependency, **MSVCR100.dll**. The corresponding Microsoft Visual C++ 2010 SP1 x86 runtime must be available on the target system.

The reconstructed compiler support includes an OpenCRT library and compatible headers/import libraries. It enables the accepted samples to compile without requiring the original Visual Studio 2010 development installation. This does **not** eliminate the plugin's legitimate runtime DLL dependencies.

The supported implementation is deliberately bounded by the interfaces and runtime behaviours tested so far; it is not a claim of complete compatibility with every legacy compiler feature or third-party plugin.

## Reconstructed SDK interfaces

Both supplied samples now use the **single shared header**:

```cpp
#include "RoccatPowerGrid.h"
```

The PDK configures its include paths to find `SDK/include/RoccatPowerGrid.h`. Sample projects should not bundle independent copies of the interface definitions.

### Common interface

`IPluginBase` derives from `QObject`. The reconstructed 32-bit virtual layout is:

| Vtable offset | Function | Purpose |
| --- | --- | --- |
| `+0x2C` | `GetName()` | Identifies the plugin/data source to the host. |
| `+0x30` | `SetPluginData(QVariantMap)` | Receives saved configuration/state. |
| `+0x34` | `GetPluginData()` | Returns configuration/state. |
| `+0x38` | `GetValue(QString, const QVariantList&)` | Supplies a named value when used by an appropriate plugin type. |

`SetPluginData` and `GetPluginData` are **reconstructed source-level names**; their original ROCCAT SDK names have not been established. Compatibility depends on the correct virtual slot order and ABI signatures, not the spelling of these two methods.

### Function provider

`IFuncProvider` extends `IPluginBase` with:

| Vtable offset | Function | Purpose |
| --- | --- | --- |
| `+0x3C` | `const QStringList& GetList()` | Exposes the names of values/functions to Power-Grid. |

Its Qt interface identifier is `org.roccat.powergrid.IFuncProvider`.

**VREServer** is the practical reference example: it exposes status values that Power-Grid controls can display. Its application-level TCP queries are read-only, not a mechanism for remote server administration.

### Incoming data source

`IIncomingDataSource` extends `IPluginBase` with:

| Vtable offset | Function | Purpose |
| --- | --- | --- |
| `+0x3C` | `PGIncomingEntries GetEntries()` | Returns entries for Power-Grid's Incoming Center. |
| `+0x40` | `bool IsActive()` | Indicates whether the source is active. |

Its Qt interface identifier is `org.roccat.powergrid.IIncomingDataSource`.

The reconstructed type aliases are:

```cpp
typedef QMap<QString, QVariant> PGConfiguration;
typedef QMap<qint64, PGConfiguration> PGIncomingEntries;
```

**PDKIncomingProbe** is the reference example. It registers the incoming-data interface with Qt, returns a timestamped entry and has been observed displaying its message in Power-Grid's Incoming Center.

These offsets reflect the reconstructed and experimentally validated x86 layout. Do not reorder virtual members, introduce virtual functions into `IPluginBase`, or assume an x64 build is ABI-compatible with Power-Grid 0.461.

## Plugin registration and manifests

Qt 4 plugins use `Q_OBJECT`, `Q_INTERFACES(...)` and `Q_EXPORT_PLUGIN2(...)` as appropriate to the concrete implementation. The linker must expose the Qt plugin entry points expected by the host.

`plugin.json` specifies the plugin's dynamic library filename via `DynamicLibraries`; that value is also used by the PDK to choose the compiled DLL name. The **DLL filename**, **Qt factory identifier** and **runtime `GetName()` value** serve different roles and do not inherently have to be identical. However, manifest data-source names and the implementation's name should agree where the host associates them.

The PDK supports both ordinary strict JSON and the relaxed manifest syntax found in historical Power-Grid projects (including unquoted keys and trailing commas). Reading or compiling an existing project does not require rewriting its manifest.

### Settings-panel capability and limitations (Power-Grid 0.461)

Recovered native settings code builds the UI from static `DataSources[].Config.Options` descriptors, **not** arbitrary keys returned by `GetPluginData()`. Its supported builder controls include `Label`, `Text`, `Password`, `Check`, `Drop`, `Group`, `TextList` and `Button`. Other native enum names do not demonstrate functional settings controls.

- Dynamic label text and option titles can use `#Method()`, resolved through plugin `GetValue()`. The resolver supplies an **empty** `QVariantList`; inline arguments must not be assumed.
- Button `Value:"#Method()"` executes a plugin command immediately; optional script `OnMouseClick` can override this route.
- Available layout directives include `<br>`, `<hr>`, `<stretch...>` and limited `<b>` handling; this is **not** general HTML.
- Runtime insertion of controls, dynamic checkbox captions, and changing `Drop` choices by returning arbitrary config fields are **not** exposed by the tested plugin interface.
- Fixed rows with dynamic labels and action buttons can implement paginated user lists. DiscordNotifier demonstrates five fixed rows, sender names, live Notify ON/OFF status, and direct Toggle actions.
- Settings scripts expose `GetSettingsValue`, `SrcGetValue` and `Revert()`. Revert **rebuilds the form from backend state, without saving**, so unsaved changes elsewhere (such as Diagnostics) can be discarded. Conventional paginated Check widgets need explicit capture/staging and Apply/Cancel validation.
- Button actions can immediately mutate plugin-owned state and aren't automatically rolled back by Cancel.
- Native settings use a scroll area, but arbitrary dynamic per-Group scrolling has not been established. `TextList` construction/add has a **128-row limit**; this is unrelated to arbitrary saved config-map key counts.
- The host saves editable control values into `PGConfiguration` via `SetPluginData`; unknown stored keys are retained. Check states may be **strings** `"0"`, `"1"`, `"2"`, rather than booleans. The observed settings file is `Documents\\ROCCAT\\Power-Grid\\userCfg.json` under `CfgSrc.<name>`. Use stable IDs rather than mutable display names for entity-specific preferences.

Relevant native addresses: option parser `0x00419eb0`, settings builder `0x00480ef0`, dynamic resolver `0x0047c560`, label refresh `0x0047dfd0`, button handler `0x0047e4e0`, collector `0x00480320`, saver `0x00482ab0`.

### Incoming Center rendering and action boundaries

In Power-Grid 0.461, the notification text control (`TextA`) draws with Qt `QPainter::drawText` into a fixed layout, with ordinary wrapping and clipping. It is **not** an HTML/hyperlink renderer, marquee, or pageable text viewer. Native `$()` text substitution exists for registered control mappings/formatters but doesn't establish plugin-provided JavaScript execution or dynamic title/body switching.

The script caches entries and skips rewriting text for existing rendered boxes. Replacing the `text` field for the same timestamp won't reliably refresh the existing banner; omitting a returned entry doesn't automatically delete the visible box. Toggling `IsActive()` can force source-level refresh in some cases, but is disruptive and timing-dependent rather than a supported per-entry update.

The `link` field invokes the original external-content confirmation and executes on the **Windows host**, including when the user taps a notification on the Android controller. `resp` triggers a confirmation and can reach the plugin's `GetValue("SendResponse", ...)`, but neither field provides a verified per-entry display toggle. DiscordNotifier uses `title` and `body` as a combined newline-delimited `text` preview and sets a native `discord://-/channels/...` deep link; tests confirmed message-level highlighting on desktop Discord.

## RPGP package generation

An `.rpgp` is a ZIP-format Power-Grid plugin package. The exporter assembles the compiled library and runtime files referenced by the project, with entries located at the package root when expected by Power-Grid.

For example, the tested VRE Server package contained:

```text
info.json
plugin.json
VREServer.ctrlset.json
vreserver.dll
VREServer.qs
```

Keep the project/package icon as **`icon.png` at the project root**. Additional runtime artwork belongs under **`rsc/`**; `plugin.json` references it through `Gfxs`, e.g. `{"Name":"DiscordSymbolBlurple","Rsc":"rsc/Discord-Symbol-Blurple.png","Tiling":"Scale"}`. The exporter includes required assets in the RPGP while preserving resource paths. In the tested DiscordNotifier, `Icon` uses the **unqualified** `Gfxs[].Name` value, rather than a data-source-prefixed identifier.

Other plugins may require images or other referenced assets. The exporter should validate those dependencies rather than blindly include every project file.

Source code, SDK headers, object files, import libraries, linker maps and build logs are **not** runtime package contents.

The PDK's Preferences allow separate directories for projects, finished RPGP files and logs, and support a configurable PDK settings-file location.

## Validation evidence

The following end-to-end behaviours were confirmed with **Power-Grid Windows Launcher 0.461**:

| Test | Observed result |
| --- | --- |
| Function-provider sample (VREServer) | Compiled DLL, exported RPGP, normal Power-Grid import, successful live dedicated-server status display. |
| Incoming-data-source sample (PDKIncomingProbe) | Compiled DLL, exported RPGP, normal Power-Grid import, successful incoming message display. |
| Manifest compatibility | Both strict-JSON and legacy relaxed-manifest projects were handled through PDK workflows. |

These tests establish a useful compatibility baseline for the two reconstructed interfaces. They do not constitute exhaustive verification of every SDK feature, plugin project or host version.

### Editor external-change limitation

Refreshing the project explorer does **not** reload open file tabs. The editor may show stale in-memory text while the RPGP packager uses newer files on disk. Close and reopen tabs following external file changes; do not save an outdated tab over newer disk data. Automatic change detection is not a confirmed feature of v0.3.31.

## Reconstruction and maintenance notes

The original function-provider interface and incoming-data-source interface were independently reconstructed from available binaries and source evidence, then merged into one common header after comparison of their shared base-class layout. Some source-level names are descriptive replacements because virtual dispatch did not reveal the original names.

ABI-critical edits should be reviewed against the accepted samples. A sensible regression sequence is to build each sample through the PDK, export a package, import it through genuine Power-Grid and exercise the relevant host feature.

Releases should be packaged from a clean build/distribution tree. Sample projects in the public PDK remain **source-only**, with no prebuilt example DLLs or build intermediates.

## Attribution and scope

Power-Grid Rescue is an independent preservation effort. This document describes the project's reconstructed interfaces and observed compatibility; it is not original ROCCAT SDK documentation. Original software, Qt components, runtime DLLs and trademarks remain subject to their respective ownership and licensing terms.
