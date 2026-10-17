# Windows Patch Details — ROCCAT Power-Grid build 0461

This document records the byte-level changes used by Power-Grid Rescue so the restoration remains auditable and reproducible.

The current rescue modifies **only `ROCCATPowerGrid.exe`**. No Qt4 or other bundled DLLs are altered.

# Supported source executable

`ROCCATPowerGrid.exe` from the preserved build 0461 installer:

- File size: `5,149,256 bytes`
- SHA-1: `4dff6c77b7b0f20c61dc1d30611b9638dd279b1c`
- SHA-256: `f519f59ee0c91547716d71ad51a542d7bbd003c45e9be6cf809f30a62f7e9831`

Known rescued result:

- File size: `5,149,256 bytes`
- SHA-1: `4f6303e4cb9be0ff63ae613c7d5e9fcaac8478f7`
- SHA-256: `b2f280375e3b6dcb7c8c8bbbbfdabeeac05c5262d7ee5eab844c148f241d57d8`

There are **72 bytes whose values differ**, spread across six contiguous diff ranges. The patcher applies these as five logical hunks; the backend URL is treated as one complete string-sized patch window.

# 1. Additional grid slots

Three compare immediates in the grid-slot construction/limit path are changed from decimal `2` to decimal `12` (`0x02 -> 0x0C`).

This is the rescue change that exposes the additional grid slots rather than retaining the original two-slot limit.

| File offset | Original | Patched | Effect |
|---|---|---|---|
| `0x00070F8A` | `02` | `0C` | slot-limit constant 1/3: 2 -> 12 |
| `0x00070FD0` | `02` | `0C` | slot-limit constant 2/3: 2 -> 12 |
| `0x0007114F` | `02` | `0C` | slot-limit constant 3/3: 2 -> 12 |

In the original x86 code these bytes are immediates used by `cmp ... , 2` instructions in the same routine. All three references are changed together so the routine consistently uses the new limit.

# 2. Account / activation failure branch bypass

At file offset `0x00117541`, the original six-byte conditional branch is replaced with NOPs.

Original:

```text
0F 85 81 00 00 00    jne +0x81
```

Patched:

```text
90 90 90 90 90 90    nop x6
```

The branch leads into the original account/error handling path near strings such as `Account not Activated`, `Invalid e-mail and/or password`, and `Too many login tries`. NOPing the branch keeps execution on the successful/local path instead of diverting into the now-unusable online account failure flow.

The original login form itself still performs local presence validation, so the current rescue build requires non-empty values in the email/password fields. Those values are not validated against the former ROCCAT service.

# 3. Dead backend redirect

The original embedded backend string starts at file offset `0x001B4860`:

```text
https://powergrid.interface.roccat.org/interface.en,5,powergrid.dll#
```

The rescue build replaces that fixed-width region with:

```text
http://127.0.0.1:9/
```

and zero-fills the remainder of the original string storage.

This prevents the abandoned ROCCAT backend hostname from being used and points the application at a deliberately local endpoint instead. In testing this removes the long wait previously caused by the dead online service; the application now moves from the login screen into the main interface within a few seconds after non-empty credentials are supplied.

The URL patch window is 68 bytes. Four leading bytes happen to retain the same values and one slash within the changed region also remains identical, which is why a raw binary diff reports two contiguous changed ranges for this one logical string replacement.

# Backup and restore behaviour

Before installing the rescue executable, the patcher preserves the verified source executable as:

```text
ROCCATPowerGrid.exe.original
```

The patcher will not overwrite an existing `.original` file unless it hashes to the known build 0461 original.

The restored executable is generated and hash-checked in a temporary file before replacement. The **RESTORE ORIGINAL** function copies the verified `.original` back to `ROCCATPowerGrid.exe` while leaving the backup in place.

This also leaves a simple manual recovery route: a user can remove the patched executable and rename/copy `ROCCATPowerGrid.exe.original` back to `ROCCATPowerGrid.exe`.

# Scope

These offsets and byte values are specific to **ROCCAT Power-Grid build 0461**. The patcher intentionally refuses an executable with an unknown SHA-1 rather than applying fixed offsets to an unverified build.

The current patcher is Windows-only. A native Linux port of the desktop application is a separate future goal rather than treating Wine support as the final Linux implementation.
