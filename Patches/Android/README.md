# Android Rescue Client

`PowerGrid.apk` is the rebuilt/modified Android client supplied by **Power-Grid Rescue** for use with the restored Windows Power-Grid application.

The original ROCCAT Android package is preserved separately under `Original/Android/`. This file is the rescue build and should be treated as a replacement client rather than an untouched archival copy.

# What it restores

The rescue client keeps the original Power-Grid model intact: the Windows application remains responsible for grids, actions and macro execution, while the Android device acts primarily as the remote display/input surface.

The restored client has been tested end-to-end with the rescued Windows application:

- the Windows application can discover/connect to the Android client on the local network
- grids can be transferred to the Android device
- grid controls render and remain interactive on the device
- button/control input from Android is returned to the Windows application
- the original Windows macro/action engine executes the requested action

The intention is preservation rather than redesign: existing Power-Grid grids and the original desktop/mobile workflow remain the basis of the system.

# Protocol compatibility

Power-Grid uses a local-network protocol between the Windows application and the Android client. The recovered implementation includes the original discovery/session concepts, including local discovery, resource transfer and a live WebSocket-style control session.

The rescue work deliberately preserves that relationship instead of replacing Power-Grid with an unrelated remote-control protocol.

# Installation

1. Install the preserved original ROCCAT Power-Grid desktop application on Windows.
2. Apply the Windows rescue patch from `Patches/Windows/`.
3. Copy `PowerGrid.apk` to the Android device.
4. Allow installation from the relevant sideload/source location if Android requests it.
5. Install the APK and launch Power-Grid.
6. Ensure the Android device and Windows PC are on the same local network and that local firewall rules are not blocking Power-Grid.

Because the rescue APK is rebuilt and cannot use ROCCAT's original private signing key, Android may refuse to install it directly over an existing official Power-Grid installation. If that happens, uninstall the original Power-Grid Android application first, then install the rescue APK.

# Android compatibility

The original ROCCAT Power-Grid client targeted **Android 2.2 (Froyo / API 8) and later**.

The current Power-Grid Rescue APK has been tested successfully on **Android 11**. Older and newer Android versions may also work, but Android 11 is the currently confirmed modern compatibility baseline for the rescue build.

# Original APK

The untouched original Android package is retained separately for preservation:

`Original/Android/org-roccat-powergrid.apk`

The rescue APK is intentionally kept in `Patches/Android/` so the original and modified packages cannot easily be confused.

# Modern Android considerations

This application comes from a much older Android generation, so behaviour can vary on recent devices because Android's application, permission, networking and package-signing requirements have changed substantially since Power-Grid was released.

The successful Android 11 test confirms that the rescue client can still operate on a significantly newer Android release than the original Android 2.2 minimum. Additional Android versions will be documented as they are tested.

# Current scope

The current Android work is focused on making the original Power-Grid client usable again with the rescued desktop application.

Future work may add features around the revived grid library, local user profiles and additional device support, but those are separate from the initial restoration goal.

# Preservation note

This APK contains original ROCCAT Power-Grid application material together with restoration changes. ROCCAT/Turtle Beach software, artwork, trademarks and other original assets remain the property of their respective rights holders. The rescue project does not claim ownership of those materials.
