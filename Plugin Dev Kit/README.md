# Power-Grid Plugin Development Kit

The **Power-Grid Plugin Development Kit (PDK)** is a Windows development environment for creating native plugins for **ROCCAT Power-Grid**.

It brings plugin editing, C++ compilation and Power-Grid package creation together in one application. Developers can build a plugin from source and export an installable **`.rpgp`** package without assembling the DLL and its supporting files by hand.

This PDK is part of the [Power-Grid Rescue project](../README.md).

## Download

**Current version: v0.3.31**

The release archive is **`PluginDevelopmentKit_0.3.31.7z`**. Installation and first-build steps are in **Getting started** below.

The archive includes the PDK, its native build toolchain, the shared SDK interface and two source-only example projects. You do **not** need Visual Studio to use the packaged PDK. The original Power-Grid Windows application is needed to install and run the resulting plugins.

**Runtime requirement:** You may also need the **Microsoft Visual C++ 2010 SP1 Redistributable (x86)** if it is not already installed. Download it from the [official Microsoft Download Center](https://www.microsoft.com/en-gb/download/details.aspx?id=26999) and choose `vcredist_x86.exe` — **even on 64-bit Windows**, because Power-Grid's native plugins are 32-bit.

## What it does

- **Project workspace:** Create a new plugin project or open an existing Power-Grid plugin directory.
- **Organised editor:** Browse source files, manifests, scripts, artwork and documentation in a file-type-based project tree, regardless of their physical folder layout.
- **Manifest support:** Read standard JSON and the relaxed JSON-style manifests used by original Power-Grid plugins.
- **Native build:** Compile 32-bit C++ plugin DLLs from the Project menu using the included Clang/LLVM and Qt tools.
- **Package export:** Build an installable `.rpgp` containing the compiled DLL and required runtime resources.
- **Validation:** See manifest issues, compiler diagnostics and build output in the application.
- **Preferences:** Choose default project, output and log directories and the location of the PDK configuration file.
- **Built-in help:** Reference material for plugin projects, interfaces, manifests and packaging.

## Getting started

1. Download [`PluginDevelopmentKit_0.3.31.7z`](PluginDevelopmentKit_0.3.31.7z), extract it to a folder of your choice on Windows and launch `PowerGridPluginDevelopmentKit.exe`.
2. Choose **File → Preferences** to set your preferred project, output and log folders.
3. Open either included example under `NativeBuild`, or choose **File → New Project**.
4. Explore or edit the C++ source and plugin manifests.
5. Select **Project → Build Plugin** to compile the native DLL.
6. Select **Project → Export RPGP** to build and package the plugin. The completed archive is written to your configured output folder; use **Export RPGP As…** when you want a different location.
7. Double-click the exported `.rpgp` on a Windows installation with Power-Grid, then follow the Power-Grid import and plugin-permission prompts.

An unbuilt example may show a warning that its DLL is not present yet. This is expected: the PDK produces that DLL when you build the project.

## Editor keyboard shortcuts

Standard text-editing shortcuts are available in the code editor, including **Ctrl+S** to save the open file.

| Shortcut | Action |
| --- | --- |
| **Ctrl+S** | Save current file |
| **Ctrl+C** | Copy selection |
| **Ctrl+X** | Cut selection |
| **Ctrl+V** | Paste clipboard text |
| **Ctrl+A** | Select all editor text |
| **Ctrl+Z** | Undo |
| **Ctrl+Y** | Redo (when supported by the active editor) |

These refer to editor operations, not application-wide Build/Export commands.

## Plugin artwork and resources

Keep **`icon.png` in the plugin project root**. Additional artwork used by plugin controls belongs in **`rsc/`**, with project-relative references in `plugin.json` (for example, `rsc/Discord-Symbol-Blurple.png`). The RPGP exporter includes the required artwork inside the distributable package.

```text
MyPlugin/
  icon.png
  plugin.json
  Source/
    MyPlugin.cpp
  rsc/
    notification-icon.png
```

Remove unused graphics before export to avoid unnecessary package size.

## Included examples

| Project | What it demonstrates |
| --- | --- |
| **Sample / PDKIncomingProbe** | An incoming data source that adds a message to Power-Grid's Incoming Center. |
| **VREServer** | A function-provider plugin that queries and displays read-only status information from a VRE dedicated server. |

Both examples contain source and supporting plugin files, **not prebuilt DLLs**. They are intended to be built through the PDK.

Plugins include the shared interface with:

```cpp
#include "RoccatPowerGrid.h"
```

The PDK supplies the SDK include path automatically. The shared header is located at `SDK/include/RoccatPowerGrid.h` in the extracted distribution.

## Screenshots

Screenshots of the PDK and its development workflow:

| | |
| --- | --- |
| <img src="images/Screenshot%202026-10-21%20000025.png" alt="Power-Grid PDK screenshot 000025" width="420"> | <img src="images/Screenshot%202026-10-21%20000102.png" alt="Power-Grid PDK screenshot 000102" width="420"> |
| <img src="images/Screenshot%202026-10-21%20000114.png" alt="Power-Grid PDK screenshot 000114" width="420"> | <img src="images/Screenshot%202026-10-21%20001340.png" alt="Power-Grid PDK screenshot 001340" width="420"> |
| <img src="images/Screenshot%202026-10-21%20001417.png" alt="Power-Grid PDK screenshot 001417" width="420"> | <img src="images/Screenshot%202026-10-21%20001430.png" alt="Power-Grid PDK screenshot 001430" width="420"> |
| <img src="images/Screenshot%202026-10-21%20001444.png" alt="Power-Grid PDK screenshot 001444" width="420"> | <img src="images/Screenshot%202026-10-21%20001448.png" alt="Power-Grid PDK screenshot 001448" width="420"> |
| <img src="images/Screenshot%202026-10-21%20001524.png" alt="Power-Grid PDK screenshot 001524" width="420"> |  |

## Compatibility and status

The reconstructed PDK has also built the [DiscordNotifier](../Plugins/Plugins/DiscordNotifier/README.md) plugin, demonstrating Qt 4 desktop RPC notification handling, persistent plugin settings, Incoming Center links and Android-to-PC navigation. The PDK editor currently does not reload externally changed files merely when its explorer is refreshed; close and reopen the affected editor tab before editing after an external overwrite.



**v0.3.31 has been tested with Power-Grid Windows Launcher 0.461.** Both included plugin types have been compiled, exported as RPGP packages, imported into Power-Grid and exercised successfully: VRE Server displayed live status, and PDKIncomingProbe appeared in the Incoming Center.

These are working reference examples, not a guarantee that every older third-party plugin source will compile without changes.

## Developer tutorial and technical documentation

New to writing Power-Grid plugins? Start with the **[four-part Plugin Development Guide](PLUGIN_DEVELOPMENT.md)**. It introduces the core JSON files, walks through both bundled examples, and teaches you how to modify and build native C++ plugins.

For the underlying engineering, compiler pipeline and reconstructed native interfaces, see **[TECHNICAL.md](TECHNICAL.md)**.

For the wider restoration project, including the Windows and Android clients, return to [Power-Grid Rescue](../README.md).

## Notice

This is an independent preservation and development project, not an official ROCCAT or Turtle Beach SDK. ROCCAT, Power-Grid and other third-party names and assets remain the property of their respective owners.
