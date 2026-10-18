# VREServer — Vulkan Render Engine server monitor

## Overview

**VREServer** is an independently developed **read-only public VRE dedicated-server monitor** for the original ROCCAT Power-Grid desktop host and mobile client.

It connects to a VRE server's **public TCP query port** and exposes server information as Power-Grid controls, including the advertised server name, availability, player population, build version, status message and query latency. It does **not** use the administrative console or require server credentials.

## Origins

After reconstructing Power-Grid's native plugin interface and demonstrating the repaired Sample2 TCP monitor, we created **VREServer.dll** as our **first entirely new, non-Sample2 native plugin**.

*VRE is a multi-player survival game being written in C# from scratch, using Vulkan as the renderer*

The implementation was developed against the VRE **1.38.40** dedicated-server source and its public query protocol. During testing, the public playtest server was running dedicated build **1.37.55**; the plugin was adapted for the older and newer public-query response versions.

The live test in Power-Grid successfully displayed the advertised name for the server instance, online state, **0 / 40** players, server build **1.37.55**, status message and numeric query latency. These are historical test readings, **not live values reported by this README**. Wide **2×1** and **3×1** controls were also tested with Power-Grid's existing resizable artwork.

## Package and metadata

| Item | Value |
| --- | --- |
| Repository file | [VREServer_Plugin_v0.1.6.rpgp](VREServer_Plugin_v0.1.6.rpgp) |
| Package name (`info.json`) | **VRE Server Monitor** |
| Package author (`info.json`) | **VulkanGame / Power-Grid Rescue** |
| Package description (`info.json`) | Read-only public status monitoring for VRE dedicated servers. |
| Native plugin ID (`plugin.json`) | `VREServer` |
| Native DLL | `vreserver.dll` |
| Script | `VREServer.qs` |
| Control set | `VREServer.ctrlset.json` |
| Editor control group | **[Plugin] VRE Server Monitor** |
| Installer package version | **v0.1.6** |
| Tested native monitor implementation | **v0.1.3** |
| Target build environment | **Qt 4.8.5 / MSVC 2010 SP1, x86 Release** |
| Plugin flag | `"Plugin":"1"` |

The packaged `info.json` is intentionally preserved as distributed; plugin version numbers and archive package version numbers describe different milestones.

## Available controls

| Control | What it displays |
| --- | --- |
| **Server Name** | Advertised name returned by the VRE server (2×1 control) |
| **Online Status** | Whether the public query is responding |
| **Players** | Connected / maximum players |
| **Server Build** | Reported dedicated-server build |
| **Query Latency** | Numeric public-query response time |
| **Status Message** | Public server status or query error information (3×1 control) |
| **Password Required** | Whether the server reports password protection; text and a conditional padlock layer |
| **Multiworld** | Whether multiworld support is advertised |
| **Player Count** | Connected-player count as a number |
| **Maximum Players** | Maximum-player capacity as a number |

The plugin does not provide a protocol-version display control. The padlock artwork reference in this edition uses the Power-Grid resource `Tab_IconLED3.png`; the text status remains a readable fallback.

## Connection configuration

Each control has editable settings in the Power-Grid editor:

| Setting | Shipped default |
| --- | --- |
| **Server host/IP** (`HostName`) | `vulkangame.online` |
| **Query port** (`QueryPort`) | `5128` |

The shipped **plugin.json** provides these defaults. Existing grid instances can retain their own saved configuration, so change the host and port in the relevant controls if monitoring a different server.

The native DLL shares a cached query state for controls targeting the same endpoint rather than creating an independent connection for every tile. Queries are **read-only TCP** against the dedicated-server's public query protocol, with protocol-version fallback and connection/error handling.

**Query latency** is the time measured for the VRE public query; it is not an ICMP echo ping.

## Installation

1. Import **[VREServer_Plugin_v0.1.6.rpgp](VREServer_Plugin_v0.1.6.rpgp)** through the Power-Grid desktop application, and approve plugin installation/loading if prompted.
2. Restart Power-Grid if necessary for the plugin controls to appear.
3. In the editor, open **[Plugin] VRE Server Monitor** and add the desired controls.
4. Configure the server host and query port on each control as needed, then send the grid to your Power-Grid mobile client.

### Companion grid

A separate, ready-made VRE monitoring **grid** is published in the repository's **VulkanRE grids** collection. **Install the plugin first, then import the grid.**

This separation is important: a plugin `.rpgp` installs a native plugin and its controls, while a grid `.rpgp` carries a layout referencing those installed controls. Combining both formats into a single root-level archive was tested and did **not** import the grid successfully.

## Implementation and security

- **Native:** C++ / Qt4 plugin implementing the recovered Power-Grid `IFuncProvider` interface.
- **Client scripting:** `VREServer.qs` uses the normal Power-Grid control lifecycle and `CallFunc` bridge.
- **Wire format:** VRE's public **binary TCP** query protocol, not JSON; VRE versioned replies use length-prefixed UTF-8 strings and little-endian numeric fields.
- **Scope:** Public monitoring only. No admin credentials, console calls, gameplay modifications or remote-control commands.
- **Compatibility:** Demonstrated against the stock Power-Grid host and VRE's public dedicated-server query; not guaranteed for every future server protocol revision.

## Project significance

VREServer demonstrates that the **reconstructed Power-Grid SDK supports genuinely new native plugins**, not merely replacements for old ROCCAT DLLs. It reuses VRE's already-exposed server information and Power-Grid's existing mobile display pipeline to provide live monitoring without modifying either application.
