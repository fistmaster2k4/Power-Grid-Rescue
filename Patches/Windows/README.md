# Windows Rescue Patcher

`PowerGridRescuePatcher.exe` restores ROCCAT Power-Grid build **0461** for offline/local use without requiring the original online service.

# Supported build

The current patch targets the Windows executable supplied by the preserved ROCCAT Power-Grid build 0461 installer.

Original executable:

- File: `ROCCATPowerGrid.exe`
- SHA-1: `4dff6c77b7b0f20c61dc1d30611b9638dd279b1c`
- SHA-256: `f519f59ee0c91547716d71ad51a542d7bbd003c45e9be6cf809f30a62f7e9831`
- Size: `5,149,256 bytes`

Patched executable:

- File: `ROCCATPowerGrid.exe`
- SHA-1: `4f6303e4cb9be0ff63ae613c7d5e9fcaac8478f7`

# What the patcher does

The patcher modifies only `ROCCATPowerGrid.exe`. No Qt4 or other bundled DLLs are modified.

The current rescue patch performs the following changes:

- bypasses the original online activation/authentication path that depended on ROCCAT's unavailable service
- redirects the dead backend path to the local rescue path used by the restored application
- removes the long connection timeout caused by the unavailable original backend
- unlocks the additional Power-Grid slots already present in the shipped application

The patch is intentionally small and surgical. The patched executable is the same size as the original and differs only at a small number of known byte ranges.

# Backup and restore

Before patching, the original executable is preserved as:

`ROCCATPowerGrid.exe.original`

The patcher will not intentionally destroy this backup. This provides two ways back to the original application:

1. use **RESTORE ORIGINAL** in the patcher
2. manually rename `ROCCATPowerGrid.exe.original` back to `ROCCATPowerGrid.exe`

The patcher verifies the build before applying the patch and verifies the resulting executable after patching.

# Installation

1. Install the original ROCCAT Power-Grid build 0461 package.
2. Run `PowerGridRescuePatcher.exe`.
3. The patcher will attempt to locate Power-Grid automatically.
4. If detection fails, use **Browse...** and select the folder containing `ROCCATPowerGrid.exe`.
5. Confirm the patcher reports **READY TO PATCH** and build 0461 is verified.
6. Click **PATCH**.
7. Launch Power-Grid normally.

# Login screen

The original login screen remains in place for now.

The rescue patch removes the dead online dependency, but the local login form still requires non-empty values. The entered email/password are not validated against the former ROCCAT service. Any non-empty values are sufficient with the current rescue build.

This behaviour is being left intact for the first restoration releases. A future rescue feature may repurpose the screen as a local user profile for the revived grid library.

# Additional slot unlock

Build 0461 already contains the UI and application support for additional grid slots, but access to them is gated by the original application logic.

The rescue patch modifies the relevant runtime gate so those existing slots remain available without the original entitlement/backend path.

No new slot system is added by the rescue patch; it exposes functionality already present in the shipped application.

# Binary patch details

The patch is applied only after the original build 0461 executable is identified by hash and the expected original bytes are verified at each patch location.

For each patch range, the patcher knows:

- file offset
- expected original bytes
- replacement bytes

If the executable does not match the supported source build, the patcher refuses to blindly modify it.

This is deliberate: the current offsets are documented and validated for build 0461 only.

# Current scope

The first rescue patcher is Windows-only.

Linux support is not being added to this patcher at this stage because the longer-term intention is to investigate a native Linux port of the Power-Grid desktop application rather than treating Wine as the final Linux implementation.
