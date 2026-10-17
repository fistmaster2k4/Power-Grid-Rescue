# Original Windows Installer — ROCCAT Power-Grid build 0461

This folder preserves the original Windows installer used by ROCCAT Power-Grid.

Included file:

`roccat_power_grid_setup-0461.exe`

The rescue project currently uses **build 0461** as its known-good Windows baseline. The Windows rescue patcher verifies the installed `ROCCATPowerGrid.exe` from this build before making any changes.

# What Power-Grid was

ROCCAT Power-Grid was a Windows + Android control system that turned a phone or tablet into a programmable companion panel for a PC.

Instead of requiring dedicated macro hardware, Power-Grid let users build or download custom touchscreen layouts called **Grids** and send them to an Android device over the local network.

A Grid could contain buttons, status displays and other controls. When the user pressed a control on the Android device, the action was sent back to the Windows Power-Grid application, which performed the configured command on the PC.

Typical uses included:

- keyboard and macro shortcuts
- launching applications or commands
- media controls
- game-specific control panels
- simulator dashboards
- OBS / streaming controls
- custom button boxes for games such as Euro Truck Simulator 2 and American Truck Simulator
- general-purpose remote controls and information panels

For users with spare Android phones or tablets, Power-Grid effectively provided a software-defined macro pad years before dedicated touchscreen and button-deck products became common.

# How the original system worked

The Windows application was the main controller and editor.

It handled:

- creation and editing of Grids
- storage/import of `.rpgp` Grid packages
- macro and action execution
- communication with Android clients
- transfer of Grid layouts and resources to connected devices
- integration with parts of the wider ROCCAT ecosystem

The Android application acted primarily as the remote display and input surface. It rendered the Grid sent by the PC and returned button/touch events to the Windows application.

Communication between the two sides took place over the local network, while parts of the original application also depended on ROCCAT-hosted online services for account/login and library functionality.

When those services disappeared, the software itself still contained most of the useful local functionality, but the dead online dependency made normal use increasingly difficult or impossible.

# Why build 0461 matters to this project

Build 0461 is the preserved Windows release currently targeted by Power-Grid Rescue.

The rescue patch does not replace the whole application. It makes a small set of verified changes to the installed `ROCCATPowerGrid.exe` so that the useful local functionality can operate without waiting on the unavailable original backend.

The current rescue also exposes additional Grid slots already present in the shipped application.

For technical details, see the documentation under:

`Patches/Windows/`

# Preservation note

The installer in this folder is preserved as original material. It is not a rebuilt installer and is not claimed as original work of the Power-Grid Rescue project.

ROCCAT / Turtle Beach software, trademarks, assets and bundled third-party components remain the property of their respective rights holders.

The purpose of preserving this installer is to retain a known baseline from which the rescue patch can be reproduced, verified and used.
