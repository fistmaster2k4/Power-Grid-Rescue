# Discord Latest Notification Tiles for ROCCAT Power-Grid

Live, custom-sized Discord notification buttons for the restored ROCCAT Power-Grid Android controller. Each button shows the latest message and opens that specific message in the **Discord desktop application on your Windows PC** when tapped.

**Release:** v0.3.0  
**Download:** [`DiscordLatestTile_v0.3.0.rpgp`](DiscordLatestTile_v0.3.0.rpgp)

## Features

- Displays the newest notification already received by **DiscordNotifier**.
- Updates the sender and message quietly in place; no page refresh or flashing.
- Cyan sender name, white message, native multiline word wrapping.
- Five control sizes, listed as **rows × columns**: 1×3, 1×4, 2×2, 2×3 and 2×4.
- Tap a tile to open the corresponding Discord message using Power-Grid's original external-action mechanism (`DFP:DoExecute`). Power-Grid may show its external-content confirmation first.
- Uses the original Power-Grid rendering and data-source interfaces; no extra background service or image decoder.

## Requirements

1. [Power-Grid Rescue](https://github.com/fistmaster2k4/Power-Grid-Rescue) running on Windows, with a connected Android Power-Grid client.
2. The separate [DiscordNotifier plugin](../DiscordNotifier/) (tested with v0.1.18), installed and receiving notifications from Discord desktop on the Windows PC.

This package **does not contain DiscordNotifier.dll**, credentials, or a Discord login/OAuth flow. It consumes notifications from the existing plugin.

## Installation

1. Download [`DiscordLatestTile_v0.3.0.rpgp`](DiscordLatestTile_v0.3.0.rpgp).
2. Import the package into the restored Power-Grid launcher.
3. Open the **Editor** tab and choose **[Plugin] Discord Notification Tiles** from the controls dropdown.
4. Add any **Latest Discord Notification** control to a grid, then assign that grid to your Android device.
5. Receive a Discord notification. The button should update automatically. Tap it to navigate to that message in Discord on the PC.

The button remains in a waiting state until DiscordNotifier has supplied a notification. An informative message is shown if its data source is unavailable.

## Notes

- Only the **latest** notification is displayed on each tile. DiscordNotifier's normal Incoming Center history is unaffected.
- All five controls show the newest available notification, rather than providing independent sender/channel filters.
- The URL action is restricted to Discord message deep links.
- Standard text rendering is intentional for compatibility with older Android tablets. Inline custom emoji images and attachment thumbnails are future possibilities, **not features of this release**.
- The v0.2.7 behaviour was confirmed working on Android; v0.3.0 removes debug output and test branding without changing the working rendering or click mechanism.

## Technical details

The tile is a Power-Grid JavaScript control that reads `DiscordNotifier` entries using the SDK's `CtrlView.GetDataSourceEntries()` interface. Its full-area layout hit surface handles `OnMouseClick`, then calls Power-Grid's built-in `DFP:DoExecute` with the validated Discord message URL. This is the same native action mechanism traced from ROCCAT's original Website control.

Project: [Power-Grid Rescue](https://github.com/fistmaster2k4/Power-Grid-Rescue)
