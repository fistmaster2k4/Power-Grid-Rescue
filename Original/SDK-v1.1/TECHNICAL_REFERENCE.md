# Official SDK v1.1 — technical reference index

This index is a navigation aid written for Power-Grid Rescue. Consult the unmodified PDFs and Doxygen documentation for authoritative API details.

## Architecture

The 2015 `documentation.pdf` describes a Windows host (*launcher*) and a thin mobile client exchanging data via UDP/TCP. The host performs the logic and composes control images, while the client displays layouts and forwards interaction.

Plugins extend Power-Grid using declarative JSON, JavaScript (`.qs`) and optional native DLLs. The documented path is **control/layout → JavaScript → native DLL**: a DLL cannot directly modify controls without the JavaScript layer.

## Plugin files and configuration

The 2013 Plugin Building Guide specifies a `plugin.json` in each plugin's root folder, with a required `Name` namespace and optional `Includes`, `Controls`, `CtrlSets`, `Skins`, `Gfxs`, `Scripts`, and `DynamicLibraries` sections.

The guides describe:
- Controls and control sets: user-visible, configurable functions and groups.
- Skins and layout elements: dimensions, positions, graphics, text and input elements.
- Graphics: resource files, layering and tiling.
- Scripts: JavaScript functions that bridge UI controls to application or DLL logic.
- Native dynamic libraries: extension points for application-specific integration.
- Configuration options: text, password, checkbox, radio, file/folder, shortcut, dropdown, spinner, colour picker and other input controls.

The 2013 guide identifies the historical plugin location as `%USERPROFILE%\\Documents\\ROCCAT\\Power-Grid\\Plugins`; the 2015 guide describes the user-data plugins directory more generally.

## C++ Doxygen reference

Inside the SDK ZIP, open `docs/dox_cpp/html/index.html`. The generated class index documents:
- `IPluginBase`
- `IFuncProvider`
- `IIncomingDataSource`
- `IWindowMessageProxy`
- `PluginLogger`
- `PluginTools`

Also inspect `include/RoccatPowerGrid.h` and `lib/RoccatPowerGrid.lib`. Backups of both are included in the original distribution. The header and import library are especially relevant when examining the historical DLL ABI.

## JavaScript Doxygen reference

Open `docs/dox_qs/html/index.html`. Its generated class index covers:
- `CtrlView`
- `DCConfigOption`
- `DCControl`
- `DCGrid`
- `DCLayout`
- `DCObj`
- `DCPal`
- `DCSkin`

The generated class pages contain method/property lists; the SDK's HTML, CSS, images and scripts are preserved together in the ZIP.

## Supplied examples

| Original SDK path | Purpose |
| --- | --- |
| `examples/simple_plugin/` and `simple_plugin.rpgp` | Basic control/plugin package |
| `examples/advanced_plugin/` and `advanced_plugin.rpgp` | Richer graphics, script and configuration example |
| `examples/custom_dll/` and `custom_dll.rpgp` | Native DLL bridge, including `src/sample2.cpp`, `src/sample2.h`, Qt project file, `Sample2.qs`, prebuilt DLLs and build artefacts |

The `examples/DEPLOYMENT.txt`, individual example READMEs, `FAQ.txt`, `CHANGES.txt` and `LICENSE.txt` are also in the archive.

## Source documents

1. `Power-Grid Plugin Building Guide.pdf` — Version B, January 2013 (22 pages).
2. `documentation.pdf` — Plugin Building Guide v0.1, August 14, 2015 (8 pages).
3. `docs/dox_cpp/html/` and `docs/dox_qs/html/` — original generated API references shipped inside the SDK ZIP.

All code examples and legacy build outputs are supplied as historical reference, not as tested binaries or current build instructions.
