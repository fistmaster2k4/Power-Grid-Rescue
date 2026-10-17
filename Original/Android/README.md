# Original Android Client — ROCCAT Power-Grid

This folder preserves the original Android application used by ROCCAT Power-Grid.

Included file:

`org-roccat-powergrid.apk`

# Original APK manifest details

The preserved APK identifies itself as:

- Package: `org.roccat.powergrid`
- Version name: `0.461`
- Version code: `28310`
- Minimum Android version: **Android 2.2 (Froyo) / API 8**
- Target Android version: **Android 4.2 / API 17**

The manifest requests the expected networking permissions used by the Power-Grid client, including:

- `INTERNET`
- `ACCESS_NETWORK_STATE`
- `ACCESS_WIFI_STATE`

It also includes legacy Google Play billing/licensing permissions from the original application era.

Other manifest characteristics include hardware acceleration being enabled and support declarations for small, normal and large screens. The original manifest does not explicitly declare xlarge-screen support.

These values are recorded here as part of the preservation baseline for the untouched original APK.

# What the Android app did

The Android client turned a phone or tablet into the visible and interactive half of the Power-Grid system.

Once paired with the Windows Power-Grid application over the local network, the device could receive a custom **Grid** from the PC and present it as a touchscreen control surface.

Depending on the Grid, the Android device could be used as:

- a macro pad
- a game control panel
- a simulator button box
- an OBS / streaming controller
- a media remote
- an application launcher
- a custom dashboard or status panel

This made spare Android hardware surprisingly useful: an old phone or tablet could become a dedicated PC control surface instead of sitting in a drawer.

# Relationship to the Windows application

The Android app was not the main automation engine.

The Windows Power-Grid application remained responsible for Grid editing, macro definitions, command execution and most of the system logic.

The Android client primarily:

- discovered or connected to the Power-Grid host on the local network
- received Grid/layout data from the Windows application
- downloaded and cached the resources needed to draw that Grid
- rendered buttons, images and other controls
- returned touch/control events to the PC

The Windows application then interpreted those events and executed the configured action.

In simplified form:

```text
Windows Power-Grid
    ↓ layout/resources
Android client
    ↓ touch/control events
Windows Power-Grid
    ↓
PC macro / action
```

# Why the original APK is preserved

The original APK is retained separately from the rebuilt rescue APK so there is a clear historical baseline.

That allows the project to preserve:

- the original application package
- the original protocol behaviour for comparison
- original resources and presentation
- a reference point for future compatibility work

The rescued/rebuilt Android client is kept separately under:

`Patches/Android/`

# Modern Android considerations

The original client dates from the Android 2.x / 4.x era: its manifest permits installation from Android 2.2 onward and targets API 17.

Behaviour on modern Android versions can vary because Android's application, permission, networking and package-signing requirements have changed substantially since the app was released.

The Power-Grid Rescue client has been successfully tested on **Android 11 / API 30**, while the untouched original APK is retained here as the historical baseline.

The rescue project therefore keeps the untouched APK for preservation while providing a rebuilt/patched APK for practical use where required.

# Preservation note

The APK in this folder is preserved as original ROCCAT material. It is not claimed as original work of the Power-Grid Rescue project.

ROCCAT / Turtle Beach software, trademarks and assets remain the property of their respective rights holders.

The purpose of this copy is preservation, compatibility research and maintaining a reproducible baseline for the restoration project.
