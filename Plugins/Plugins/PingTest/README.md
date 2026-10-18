# PingTest — repaired ROCCAT Sample2 TCP monitor

## Overview

**PingTest** is the rescued ROCCAT **ExamplePlugin3 / Sample2** native Power-Grid example. Its **Check Host** control measures how long a TCP connection takes to establish and displays the result as a **number of milliseconds**, without an `ms` suffix.

This is a **TCP connection-time measurement**, not an ICMP echo ("ping"). The original example targets TCP port **80**; the repaired implementation also supports a `hostname:port` endpoint.

## Origins and restoration

This plugin began as ROCCAT's original **ExamplePlugin3** Power-Grid SDK sample. The copy recovered during the Power-Grid Rescue project had been modified with unrelated **AIDA64 shared-memory** logic and crashed when the original host-check control executed.

The original native SDK headers were unavailable. We reconstructed the Power-Grid **IFuncProvider** interface and Qt 4.8.5 plugin registration/ABI from the supplied application and sample binaries, then built a replacement **Sample2.dll** using **MSVC 2010 SP1 x86 / Qt 4.8.5**.

The replacement removes the unrelated AIDA64 dependency, restores a working asynchronous TCP connection test, and keeps the **original Sample2.qs script unchanged**. It was tested in Power-Grid 0.461: values updated in the desktop editor and live on an Android Power-Grid client.

## Package and metadata

| Item | Value |
| --- | --- |
| Repository file | [ExamplePlugin3_v0.1.0.rpgp](ExamplePlugin3_v0.1.0.rpgp) |
| Package identity (`info.json`) | `ExamplePlugin3` |
| Native plugin identity (`plugin.json`) | `Sample2` |
| Native DLL | `Sample2.dll` |
| Power-Grid script | `Sample2.qs` |
| Control set | `Sample2.ctrlset.json` |
| Editor category | `[Plugin] Sample2` → **Test** |
| Control | **Check Host** (`Sample2:HostCheck`) |
| Editor option | **Host** (`HostName`) |
| Packaging version | **v0.1.0** |
| Native TCP implementation baseline | **v0.1.3** of the reconstructed SDK work |
| Target | ROCCAT Power-Grid **0.461**, Windows **x86**, Qt **4.8.5** |

### Original attribution preserved

The archive's `info.json` retains the original sample metadata:

- **Name:** ExamplePlugin3
- **Author:** ROCCAT GmbH
- **Description:** ExamplePlugin3 — store release
- **Email:** support@roccat.org
- **Website:** http://www.roccat.org
- **Plugin flag:** `"Plugin":"1"`

The original metadata identifies the upstream sample; it does **not** mean ROCCAT authored or endorsed the repaired DLL.

## Installation and use

1. Import **[ExamplePlugin3_v0.1.0.rpgp](ExamplePlugin3_v0.1.0.rpgp)** through the Power-Grid desktop application as a **plugin**.
2. Accept the plugin installation/load prompts as appropriate.
3. Open the grid editor and locate **[Plugin] Sample2 → Test → Check Host**.
4. Add the control to a grid and configure its **Host** field.
5. The control displays numeric TCP connection timing and refreshes repeatedly. Failures may display negative result codes.

The package contains the plugin and control definition; it does **not** contain a separate pre-made demonstration grid.

## Technical notes

- The Qt-native plugin exposes the `Sample2` function provider to Power-Grid's JavaScript bridge.
- `Sample2.qs` calls `SendTcpRspTime` to start a measurement and `ResultTcpRspTime` to retrieve it.
- A result of `0` means the request is still pending; positive results represent elapsed milliseconds.
- The Windows x86 Release build is required for compatibility with the stock Power-Grid host.
- The `.rpgp` is ZIP-based internally, with `info.json`, `plugin.json`, script, DLL and control-set files at the archive root.

## Project significance

Sample2 was our **first proof that the missing native Power-Grid SDK interface could be reconstructed**, compiled independently, and used end-to-end with the unmodified desktop application and Android client. It is retained as a working reference for future native plugins.

This README describes the repository package and the behaviours verified during the rescue work; it is not a claim of compatibility with every Power-Grid release or network environment.
