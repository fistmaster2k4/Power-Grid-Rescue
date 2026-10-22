# Power-Grid Plugin Development Guide

**Choose a part:**

- **[Part 1 — Core Project Structure](#part-1--core-project-structure):** Understand the files, JSON identifiers and package layout.
- **[Part 2 — Your First Incoming Data Source](#part-2--your-first-incoming-data-source):** Make a message appear in Incoming Center.
- **[Part 3 — Your First Function Provider](#part-3--your-first-function-provider):** Connect JSON, QS and C++ to show a value on a grid.
- **[Part 4 — Writing Your Own C++ Plugin](#part-4--writing-your-own-c-plugin):** Follow the native source, write your own behaviour and build it.

---

## Part 1 — Core Project Structure

**Applies to:** Power-Grid Windows host 0.461 and the reconstructed Power-Grid PDK v0.3.31.  
**Audience:** Developers comfortable editing JSON and C++ who are new to Power-Grid plugins.  
**Scope:** The shared package structure, manifests, resources, and the relationship between the declarative files and code. The incoming-data implementation is covered in Part 2 below; function providers and `.qs` are covered next.

> **Use the two source-only examples supplied with the PDK as your working references:** `NativeBuild/Sample` (Incoming Center) and `NativeBuild/VREServer` (function provider). Examples below are taken from the preserved VREServer project or identified as illustrative excerpts. This chapter does not use or distribute DiscordNotifier source.

### 1.1 Understand the pieces before writing code

A Power-Grid plugin is not just a DLL. Power-Grid also needs manifests that describe the package, the plugin's capabilities and—in the case of grid controls—the scripts and visual layouts associated with those controls.

| Component | Responsibility | Common to both supplied native examples? |
| --- | --- | --- |
| `info.json` | Package-facing identity and description used for installing/identifying the plugin | Yes |
| `plugin.json` | Plugin declaration: native DLLs and, where applicable, data sources, scripts, skins, controls and settings | Yes |
| `icon.png` | Project/plugin icon; keep at the project root | Standard PDK project convention |
| C++ source (`.cpp`, `.h`) | Developer-written native implementation, compiled to a DLL | Yes, but **source is not placed inside the RPGP** |
| `.qs` | Script behaviour used by controls, where a plugin defines them | As needed; the VREServer example uses one |
| `rsc/` | Additional images referenced by the plugin | Optional |
| `*.ctrlset.json` | Control-set data referenced by `CtrlSets` | For plugins that supply such a control set |

**Important:** The names `info.json` and `plugin.json` are conventional package names. The project-specific DLL, script and control-set filenames are not required to match each other; instead, their references must be consistent. Not every plugin needs a script, control set or extra artwork.

#### A typical project

```text
MyPlugin/
├── info.json
├── plugin.json
├── icon.png
├── MyPlugin.qs            # Only if the plugin uses a script
├── MyPlugin.ctrlset.json  # Only if referenced by CtrlSets
├── rsc/                   # Optional extra artwork
│   └── status-icon.png
└── Source/                # Example organisation; actual source layout can vary
    ├── MyPlugin.cpp
    └── MyPlugin.h
```

The C++ compiler creates the plugin DLL. The exporter assembles the **runtime** files into the installable `.rpgp`; it does not need the C++ sources, object files, SDK headers or compiler logs in the resulting package.

### 1.2 `info.json` — identify the package

The VREServer sample provides this complete file:

```json
{
  "Name": "VRE Server Monitor",
  "Author": "VulkanGame / Power-Grid Rescue",
  "Desc": "Read-only public status monitoring for VRE dedicated servers.",
  "Plugin": "1"
}
```

| Property | Meaning in the supplied sample |
| --- | --- |
| `Name` | Human-readable package name |
| `Author` | Author or publisher identification |
| `Desc` | Short package description |
| `Plugin` | Marks this item as a plugin; the working sample uses the **string** `"1"` |

This is a known-working structure from a supplied example, not a claim that these are the only keys Power-Grid accepts. Preserve the spelling and types used by the sample while learning the format.

**Exercise:** Copy the example project and change only `Name`, `Author` and `Desc` to identify your test plugin. Do not rename its DLL or native data source yet; those are tied to other files.

### 1.3 `plugin.json` — describe what Power-Grid should load

Unlike `info.json`, the historical `plugin.json` format need not be strict JSON. The VREServer sample uses **relaxed JSON-style syntax**: unquoted property names, trailing commas and comments. The PDK can read both relaxed manifests and standard strict JSON.

A shortened excerpt from the VREServer manifest shows its top-level structure:

```javascript
{
  Name: "VREServer",
  DynamicLibraries: ["vreserver.dll"],
  CtrlSets: ["VREServer.ctrlset.json"],
  Scripts: [
    {
      Name: "VREServer",
      Functions: ["InitPlayers", "TickPlayers"],
      Rsc: "VREServer.qs"
    },
  ],
  Skins: [ /* the visual button layouts */ ],
  Controls: [ /* selectable grid controls */ ],
}
```

**This is a teaching excerpt, not a complete, usable replacement `plugin.json`.** In the actual sample the `Functions` array lists every exported script function, and `Skins`/`Controls` contain real definitions. Leave those intact until the later chapters.

| Property | What it connects | When you'll use it |
| --- | --- | --- |
| `Name` | Internal plugin identity | In the plugin declaration |
| `DynamicLibraries` | DLL filenames loaded by Power-Grid | A native-plugin build |
| `CtrlSets` | Referenced control-set JSON files | When providing associated control sets |
| `Scripts` | Script identifiers, `.qs` resource paths and exported function names | When defining script-driven controls |
| `Skins` | Appearance and event bindings for visual controls | When supplying your own grid-control appearance |
| `Controls` | User-selectable controls, their skins, sizes and configuration | When exposing controls in the grid editor |
| `DataSources` | Native data-source declarations and settings | Relevant to incoming-data-source plugins, covered later |
| `Gfxs` | Named additional graphic resources | When using images supplied in `rsc/` |

An important distinction: **the existence of a top-level property does not mean all plugins require it**. The VREServer manifest demonstrates script-driven grid controls; `Sample/PDKIncomingProbe` demonstrates a different entry point into the host.

#### File references must agree

For VREServer, these relationships are visible in the supplied files:

```text
plugin.json
 ├─ DynamicLibraries[0] ──────> vreserver.dll
 ├─ CtrlSets[0] ──────────────> VREServer.ctrlset.json
 └─ Scripts[0]
     ├─ Name ─────────────────> VREServer
     ├─ Rsc ──────────────────> VREServer.qs
     └─ Functions[] ──────────> names implemented in the QS file
```

Changing a script filename in one place but not the other breaks that relationship. Similarly, changing a DLL filename in the build without updating `DynamicLibraries` leaves Power-Grid looking for the old name.

### 1.4 Scripts and controls — how JSON relates to `.qs`

In the VREServer sample, a control named `Players` uses the skin named `Players`:

```javascript
// Excerpt from the existing plugin.json
{
  Name: "Players",
  Skins: ["Players"],
  Config: {
    Description: "Player population / maximum slots",
    Options: [
      { Type: "Text", Name: "HostName", Title: "Server host/IP:", Value: "vulkangame.online" },
      { Type: "Text", Name: "QueryPort", Title: "Query port:", Value: "5128" },
    ]
  },
  Width: 1,
  Height: 1
}
```

The related skin defines `OnInit:"InitPlayers"` and `OnTick:"TickPlayers"`, with a text layout named `Result`. Its script functions are listed under `Scripts[].Functions` and implemented in `VREServer.qs`.

That lets the script initialise a control, read the configured host and port, call the named native function provider, and place the returned value in the on-screen text layout. The precise `CallFunc()` implementation and the C++ function-provider entry point are explained in the VREServer tutorial, **not** required knowledge to edit `info.json`.

Think of the responsibilities this way:

- **Manifest JSON:** Declares controls, resources, names and event bindings.
- **QS:** Handles UI/control behaviour and requests named data.
- **C++ DLL:** Implements the native interface and supplies or receives data.

### 1.5 Images: project icon versus UI graphics

Place the plugin's **`icon.png` in the project root**. Place additional plugin artwork in **`rsc/`** and reference it through the manifest's graphics declarations.

A proven resource-entry pattern documented by the PDK is:

```javascript
Gfxs: [
  { Name: "StatusIcon", Rsc: "rsc/status-icon.png", Tiling: "Scale" },
],
```

The name `StatusIcon` is a resource **identifier**, while `rsc/status-icon.png` is its **file path**. A control or other manifest field that uses the graphic must use the identifier expected by that field. Do not assume a raw filename is interchangeable with a registered resource name.

The RPGP exporter should include referenced runtime images, preserving required paths. Unused source art and alternate icons are not needed in the release package.

### 1.6 What actually goes inside an RPGP?

An `.rpgp` is a ZIP-format archive. A tested VREServer package used this runtime layout:

```text
info.json
plugin.json
VREServer.ctrlset.json
vreserver.dll
VREServer.qs
```

Other plugins may include `icon.png` and an `rsc/` directory. The **source** `.cpp` and `.h` files stay in the developer's project. This separation matters: distributing a usable plugin does **not** require publishing its native source code.

**Build in the PDK:** Use **Project → Build Plugin**, then **Project → Export RPGP**. Open the resulting package with Power-Grid to test it. For full instructions see the [PDK README](README.md).

### 1.7 First safe exercise — inspect without breaking a working plugin

1. Open `NativeBuild/VREServer` in the PDK.
2. Locate `info.json` and identify its four fields.
3. Open `plugin.json` and find `Name`, `DynamicLibraries`, `Scripts`, `Skins` and `Controls`.
4. Find the `Players` control. Follow `Skins:["Players"]` to the matching skin and its `OnInit`/`OnTick` names.
5. Open `VREServer.qs` and locate `InitPlayers` and `TickPlayers`.
6. Confirm the DLL referenced by `DynamicLibraries` is the one the build produces.
7. Build and export the original sample before changing any native method or resource identifiers.

At the end of this exercise, you should be able to explain which files describe the plugin, which file implements behaviour, and why a DLL on its own is not a complete Power-Grid plugin.

## Continue learning

- **Part 2 — Incoming Data Source:** immediately below, using `NativeBuild/Sample`, `IIncomingDataSource`, `GetEntries()` and `IsActive()`.
- **Part 3 — Function Provider:** `NativeBuild/VREServer`, `IFuncProvider`, `GetList()`, `GetValue()`, `CallFunc()` and script-driven controls.
- **Later reference chapters:** settings, persistence, graphics and troubleshooting.



---

## Part 2 — Your First Incoming Data Source

**Starting project:** `NativeBuild/Sample` (the working Incoming Center example supplied with the PDK).  
**You'll learn:** how the manifest identifies an incoming data source, what its C++ methods do, and how to change a message and see it in Power-Grid.  
**Before starting:** read Part 1, build the supplied sample once, and work on a **copy** of the sample project.

> **A note about the names:** This tutorial uses `Sample` as the project folder. In the inspected example source, the native class is `TestIncomingSourceClang` and the plugin is identified as `ClangIncomingProbe`. You may also see the project referred to as *PDKIncomingProbe* in our general PDK documentation. Use the names in **your actual files**: identifiers in code and JSON must agree where they refer to the same thing.

### 2.1 What is an incoming data source?

An incoming data source supplies messages for **Power-Grid's Incoming Center**. Power-Grid periodically asks the plugin for available entries, then its own Incoming Center interface displays them. The plugin provides the data; the host supplies the presentation and manages the user interaction.

For our beginner example, we'll provide a single message. We won't need to connect to a website, listen on a network socket, or write a new grid layout.

```text
plugin.json: identifies the data source and its settings
                ↓
C++ plugin: supplies GetEntries() and IsActive()
                ↓
Power-Grid Incoming Center: displays the supplied message
```

### 2.2 Start by reading `plugin.json`

The supplied `Sample/plugin.json` declares a native DLL and one incoming data source. This is the full manifest from the inspected sample:

```json
{
  "Name": "ClangIncomingProbe",
  "DynamicLibraries": ["ClangIncomingProbe.dll"],
  "DataSources": [
    {
      "Name": "ClangIncomingProbe",
      "Title": "Clang incoming compatibility probe",
      "Icon": "incoming_icons_rss",
      "Config": {
        "Description": "Experimental clang-cl / MSVC2010 ABI test only",
        "Options": [
          {
            "Name": "Message",
            "Title": "Message:",
            "Type": "Text",
            "Value": "Hello from ClangIncomingProbe"
          }
        ]
      }
    }
  ]
}
```

| Field | What it means here | What you can experiment with |
| --- | --- | --- |
| `Name` at the top | The plugin's internal manifest name | Leave it alone during your first test |
| `DynamicLibraries` | The DLL Power-Grid should load | Must match the DLL built by the PDK |
| `DataSources` | Describes the native source shown by the host | Keep this entry while learning |
| Data source `Name` | Identifies this source to Power-Grid | Must agree with the identity returned by C++ `GetName()` in this example |
| `Title` | Human-readable name for the source | Safe to edit for your own example |
| `Icon` | Names a host-provided graphic for this source | Leave the working name until you understand graphics resources |
| `Config.Description` | Explanatory text in its settings | Safe to change |
| `Options[]` | The editable settings controls | One text field is supplied here |
| Option `Name` | Settings key passed to and from the plugin | Keep `Message`: the C++ code looks it up by that exact key |
| Option `Type` | The kind of settings control | `Text` means an editable text field |
| Option `Value` | Its initial/default value | Safe to change, noting an already saved setting may override the default |

**Why does `Message` matter?** The name is the bridge between the manifest's editable box and `m_config.value("Message")` in the C++ source. Change only one side and the connection breaks.

### 2.3 Meet the incoming-data interface

Open `Source/TestIncomingSourceClang.h`. The relevant portion of the working header reads:

```cpp
#include "RoccatPowerGrid.h"
#include <QtCore/QMutex>

class TestIncomingSourceClang : public IIncomingDataSource
{
    Q_OBJECT
    Q_INTERFACES(IIncomingDataSource)
public:
    TestIncomingSourceClang();
    virtual ~TestIncomingSourceClang();
    virtual QString GetName();
    virtual void SetPluginData(PGConfiguration value);
    virtual PGConfiguration GetPluginData();
    virtual PGIncomingEntries GetEntries();
    virtual bool IsActive();
private:
    QMutex m_mutex;
    PGConfiguration m_config;
    qint64 m_entryTime;
};
```

`#include "RoccatPowerGrid.h"` brings in the common interfaces from our PDK. The `IIncomingDataSource` base class tells the host what kind of plugin this is. Qt's `Q_OBJECT` and `Q_INTERFACES(...)` declarations let Qt recognise that interface.

The interesting methods are:

| Method | In ordinary language |
| --- | --- |
| `GetName()` | “Who am I?” Returns the source identifier |
| `SetPluginData(...)` | “Here are your settings.” Receives configuration from Power-Grid |
| `GetPluginData()` | “What settings do you currently have?” Returns them for the host to save/use |
| `GetEntries()` | “What messages do you have?” Returns incoming message data |
| `IsActive()` | “Are you currently available?” Lets the host check the source's active state |

The constructor prepares the plugin's starting values; the destructor handles its cleanup. You do **not** call these interface methods manually from the QS script in this example: Power-Grid calls them through the native interface.

### 2.4 How the C++ source provides a message

Open `Source/TestIncomingSourceClang.cpp`. The example's factory registration is:

```cpp
Q_EXPORT_PLUGIN2(ClangIncomingProbe, TestIncomingSourceClang)
```

The first identifier is the Qt plugin factory key and the second is the C++ class Qt creates. Copy this pattern from the working sample; don't casually rename either while learning.

The source prepares a default message and its unique timestamp in the constructor:

```cpp
TestIncomingSourceClang::TestIncomingSourceClang()
    : m_entryTime(QDateTime::currentMSecsSinceEpoch())
{
    m_config.insert("Message", QString("Hello from ClangIncomingProbe"));
}
```

Then `GetName()` identifies the data source:

```cpp
QString TestIncomingSourceClang::GetName()
{
    return QString("ClangIncomingProbe");
}
```

Notice the matching `ClangIncomingProbe` value in `plugin.json`. For now, keep it consistent.

The central method is `GetEntries()`. Here is the **complete method** from the inspected sample:

```cpp
PGIncomingEntries TestIncomingSourceClang::GetEntries()
{
    QMutexLocker lock(&m_mutex);
    PGConfiguration entry;
    entry.insert("time", QVariant(static_cast<qlonglong>(m_entryTime)));
    entry.insert("title", QString("ClangIncomingProbe"));
    QString text = m_config.value("Message").toString();
    if (text.isEmpty()) text = QString("(empty test message)");
    entry.insert("text", text);
    // Host adds "src". Existing IncomingCenter script uses descriptor Icon
    // when an entry has no custom icon. No network link/response is supplied.
    PGIncomingEntries result;
    result.insert(m_entryTime, entry);
    return result;
}
```

Read it from the inside out:

1. Create one `entry` map: a collection of named values belonging to one message.
2. Set `time`: a timestamp in **milliseconds since the Unix epoch**.
3. Set `title`: the message's heading.
4. Get `text` from the saved `Message` setting, or use a fallback if empty.
5. Put the message into `result` under its timestamp key.
6. Return that result to Power-Grid.

`PGConfiguration` is the map used for a message's fields and for plugin settings. `PGIncomingEntries` is the map containing one or more such messages, keyed by timestamp.

**An important detail:** The sample records `m_entryTime` **once when the object is constructed**. Calling `GetEntries()` again returns the same message with the same key. This is intentional: it doesn't generate a new notification every time Power-Grid checks. If you later want to add *new* messages, you'll need an entry lifecycle and a new key for each new message.

The remaining method is deliberately simple:

```cpp
bool TestIncomingSourceClang::IsActive()
{
    return true;
}
```

That means the source always reports itself active. A future plugin might have a meaningful connected/disconnected state, but this sample doesn't need one.

### 2.5 Where the editable setting goes

The sample starts with a default `Message`, but the host can supply a saved value when it calls `SetPluginData()`:

```cpp
void TestIncomingSourceClang::SetPluginData(PGConfiguration value)
{
    QMutexLocker lock(&m_mutex);
    if (!value.contains("Message"))
        value.insert("Message", QString("Hello from ClangIncomingProbe"));
    m_config = value;
    qDebug("ClangIncomingProbe: configuration applied");
}

PGConfiguration TestIncomingSourceClang::GetPluginData()
{
    QMutexLocker lock(&m_mutex);
    return m_config;
}
```

In plain terms, `SetPluginData()` receives the settings and remembers them; `GetPluginData()` hands them back. Both methods use the same `m_config` member. The mutex prevents another thread accessing that shared data while the method is using it.

This is why the **`Message` spelling in JSON matters**. It's not just a label on screen: it's a key that the C++ source reads.

### 2.6 Build it, install it and see the result

1. Make a copy of `NativeBuild/Sample` so the supplied reference remains untouched.
2. Open the copy in the PDK.
3. Build it with **Project → Build Plugin**. Confirm the build succeeds and produces the DLL referenced by `DynamicLibraries`.
4. Choose **Project → Export RPGP** and select the output package.
5. Double-click the `.rpgp` to import it into the restored Power-Grid Windows host.
6. Enable/open its incoming data source in Power-Grid, then view the Incoming Center.
7. Look for the sample entry with its heading and default message.

**Expected result:** An Incoming Center item whose text is `Hello from ClangIncomingProbe` unless a previously saved `Message` value overrides the default.

If the plugin imports but you don't see the item, first check that Power-Grid is running, that its Incoming Center is displaying the source, and that the expected DLL and `DataSources[].Name` are unchanged. See also the notes below on persistent settings and timestamps.

### 2.7 Your first experiment: change a message

This experiment deliberately changes **just one value**.

1. In your copied project, open `plugin.json`.
2. Find the `Options` entry where `Name` is `Message`.
3. Change its default `Value` to `My first Power-Grid message!`.
4. In the C++ constructor **and** in the `SetPluginData()` fallback, update the default string to the same text. This keeps both defaults understandable, but an existing saved value may still win.
5. Rebuild and export your copy, then reinstall it into Power-Grid.
6. If the old value still appears, open the plugin's settings and change the `Message` field there; Power-Grid can preserve previously saved values across installations.

**Expected result:** The Incoming Center message displays your new text when the plugin is using that setting. You haven't changed the method names, source identity, ABI, or any packaging structure.

**Optional second experiment:** Change the `title` string in `GetEntries()` and rebuild. Observe that the heading changes but the settings field and message body do not. That's a useful way to see the difference between a **field name** (`"title"`) and a **field value** (`"ClangIncomingProbe"`).

### 2.8 Common mistakes and what they mean

| Symptom | First thing to check |
| --- | --- |
| Power-Grid can't load the DLL | `DynamicLibraries` filename; build output; whether it is a 32-bit DLL |
| Plugin builds but source isn't available | `DataSources[].Name` and the return value of `GetName()` |
| Edited default doesn't appear | A previous `Message` setting may have been saved |
| Text is empty | Check that `Message` exists in the configuration map; use the example fallback |
| Repeated checks don't generate fresh messages | The sample's timestamp is set only once on construction |
| Edit to a JSON file seems ignored in PDK | Close and reopen a tab after external changes; project-tree refresh doesn't reload open editor contents |

### 2.9 What you've learned

You've now seen the whole chain for an incoming source:

```text
plugin.json describes the DLL and the Message setting
                 ↓
Power-Grid loads the C++ plugin and supplies settings
                 ↓
GetEntries() builds a timestamped title/text entry
                 ↓
Power-Grid's Incoming Center displays that entry
```

The next chapter takes the other route: using **VREServer as a function provider**, so controls and scripts can **ask C++ for live values** rather than receiving a message from it.

**Reference:** This chapter's code and manifest excerpts were checked against `NativeBuild/Sample` in the PDK's v0.3.30 release-ready source archive; the documented interface and build process carry forward into the v0.3.31 PDK. The sample package you have installed is the final authority on its filenames and display names.

---

## Part 3 — Your First Function Provider

**Starting project:** `NativeBuild/VREServer` (included with the PDK).  
**You'll learn:** how Power-Grid controls ask a native plugin for information, how `.qs` scripts pass settings to C++, and how to make a visible change.  
**Before starting:** read Parts 1 and 2, and experiment on a **copy** of VREServer.

### 3.1 What's a function provider?

The incoming-data sample **pushes entries into Incoming Center**. A function provider works the other way round: a Power-Grid control **asks the plugin for a named value**.

VREServer supplies information about a VRE dedicated server, including its player count, server name, build and query latency. It's read-only; it queries the server's public status port rather than sending game administration commands.

```text
A control's settings (HostName and QueryPort)
             ↓
VREServer.qs asks for a named value
             ↓
The native VREServer DLL receives GetValue(...)
             ↓
C++ returns a value (or a placeholder)
             ↓
The QS script updates a text field on the grid
```

We'll follow **Players**, which displays the current number of players against server capacity.

### 3.2 Locate the files and identifiers

Open `NativeBuild/VREServer`. The files you'll refer to are:

| File | Job |
| --- | --- |
| `info.json` | Describes the plugin to people and the package importer |
| `plugin.json` | Registers the DLL, control layouts, configuration options and script functions |
| `VREServer.ctrlset.json` | Places the plugin's controls in Power-Grid's control picker |
| `VREServer.qs` | Connects a visible control to values returned by C++ |
| `Source/vreserver.h` | Declares the `VREServer` C++ class and its methods |
| `Source/vreserver.cpp` | Implements the methods and the server query logic |

The repeated name **VREServer** matters: some names are used by the host to connect the manifest, control set, script and compiled plugin. Other names, such as **Players**, identify one particular function or control.

### 3.3 Begin with the manifest

In `plugin.json`, the important opening entries are:

```js
{
  Name:"VREServer",
  DynamicLibraries:["vreserver.dll"],
  CtrlSets:["VREServer.ctrlset.json"],
  Scripts:[
    {
      Name:"VREServer",
      Functions:["InitPlayers","TickPlayers" /* and other functions */],
      Rsc:"VREServer.qs"
    }
  ],
  // Skins and Controls follow ...
}
```

*Shortened excerpt from the sample's relaxed JSON manifest.* The actual manifest lists every script function; the comment is explanatory, not a replacement to paste into the project.

- `Name` identifies the plugin in this manifest.
- `DynamicLibraries` names the DLL to load.
- `CtrlSets` tells Power-Grid which control-set file to read.
- `Scripts[].Rsc` points to the QS script.
- `Scripts[].Functions` lists the callable QS function names.

The names `InitPlayers` and `TickPlayers` must match functions in `VREServer.qs`.

#### One control: Players

The manifest also defines a **skin** (the visible appearance) and a **control** (the configurable thing you add to a grid).

```js
// Shortened excerpts; consult the full plugin.json for the complete layouts.
{ Name:"Players", Width:"1", Height:"1",
  OnInit:"InitPlayers", OnTick:"TickPlayers",
  Layouts:[
    { Type:"Text", Text:"PLAYERS" },
    { Type:"Text", Name:"Result", Text:"--" }
  ]
}
```

The actual skin includes background graphics, font choices, positioning and colours. Notice the named `Result` text layout: the script looks that up and changes its displayed text.

The corresponding control declares its skin and offers editable connection settings:

```js
{ Name:"Players", Skins:["Players"],
  Config:{ Description:"Player population / maximum slots", Options:[
    { Type:"Text", Name:"HostName", Title:"Server host/IP:", Value:"play.vulkangame.online" },
    { Type:"Text", Name:"QueryPort", Title:"Query port:", Value:"5128" }
  ] }, Width:1, Height:1
}
```

`HostName` and `QueryPort` are **configuration keys**, not C++ method names. Changing their display titles is different from renaming those keys.

### 3.4 Make the control available in Power-Grid

Open `VREServer.ctrlset.json`:

```js
{
  Type:"ControlSet",
  Name:"[Plugin] VRE Server Monitor",
  ExpandCategories:["*"],
  Elements:[
    { Name:"Players", Control:"VREServer:Players", Category:"Information" }
    // Additional controls exist in the full example.
  ]
}
```

This is a shortened excerpt. `VREServer:Players` is the reference that tells the control picker which plugin control to use. A developer can edit the friendly display name or category without changing that identifier.

**Expected result:** When the plugin is installed, its *Players* control can be found in Power-Grid's control picker and added to a grid.

### 3.5 Read the QS script

The script is the bridge between a control on the screen and the native C++ plugin.

From `VREServer.qs`:

```js
function InitPlayers(p_xSkin)
{
    var cfg = p_xSkin.getControl().getConfigData();
    var host = cfg["HostName"];
    var port = cfg["QueryPort"];
    if (host == null || String(host).length == 0) host = "play.vulkangame.online";
    if (port == null || String(port).length == 0) port = "5128";
    p_xSkin.data = { host: String(host), port: String(port),
                     result: this.FindLayout("Result") };
}

function TickPlayers(p_xSkin)
{
    var d = p_xSkin.data;
    if (!d || !d.result) return;
    var value = this.CallFunc("VREServer:Players", [d.host, d.port]);
    if (value == null) return;
    d.result.setText(String(value));
}
```

**Here's what happens:**

1. `InitPlayers` runs for the skin's initialisation and reads the `HostName` and `QueryPort` settings.
2. `FindLayout("Result")` finds the **text element** named `Result` in the skin's JSON layout.
3. `p_xSkin.data` holds those references so the tick function can reuse them.
4. `TickPlayers` calls the native function by its qualified name, passing host and port.
5. `setText()` puts the returned value on the control.

The `OnInit` and `OnTick` names came from the skin declaration in `plugin.json`. The `VREServer:Players` name is the connection to the function provider. All three connections need to be correctly named.

**Important:** A QS script is not C++ and is not compiled into the DLL. It's a separate runtime script included in the RPGP alongside the DLL.

### 3.6 Meet the native function-provider interface

Open `Source/vreserver.h`:

```cpp
class VREServer : public IFuncProvider
{
    Q_OBJECT
    Q_INTERFACES(IFuncProvider)

public:
    VREServer();
    virtual ~VREServer();
    virtual QString GetName();
    virtual const QStringList &GetList();
    virtual QVariant GetValue(QString name, const QVariantList &params);

    // The sample also declares its network and caching helpers.
};
```

This is shortened to focus on the methods Power-Grid uses.

| Method | Plain-English meaning |
| --- | --- |
| `GetName()` | “What's this plugin's name?” |
| `GetList()` | “Which named values can I ask you for?” |
| `GetValue(name, params)` | “Give me the value named `name`, using these parameters.” |

In `Source/vreserver.cpp`, `GetName()` returns `"VREServer"`. The constructor fills a `QStringList` with names such as `"Players"`, `"ServerName"` and `"QueryLatency"`, and `GetList()` returns that list.

The sample registers the Qt plugin factory using:

```cpp
Q_EXPORT_PLUGIN2(vreserver, VREServer)
```

Leave the registration and interface declarations intact in your first experiments. The PDK already handles compiling and linking the DLL.

#### What `GetValue()` receives

For our control, the QS call is:

```js
this.CallFunc("VREServer:Players", [d.host, d.port])
```

The C++ method therefore receives the **value name** `"Players"` and a parameter list with the configured host and port.

The actual function first checks whether the requested name is in its supported list. It then finds or creates a cached connection for the requested endpoint, and returns a value based on the most recent response.

For the Players case, its return statement is:

```cpp
if (name == QLatin1String("Players"))
    return QString::number(s.players) + QLatin1String(" / ") +
           QString::number(s.maximum);
```

If the server reports 3 connected players and a capacity of 20, the display will show `3 / 20`. **Those numbers are illustrative**, not a claim about a live server.

The native method returns a `QVariant`, so it can supply strings or numbers as needed. The QS script converts this example's result to text before displaying it.

#### Why the network code is more complicated

VREServer uses Qt networking to query a server without stopping the UI for every request. It caches results and handles offline servers, invalid host names and timeouts.

**You don't have to understand its binary network protocol to learn the function-provider interface.** Read the three public methods and the QS call first; the rest is the application's particular job.

### 3.7 Try it yourself: change a label

This first exercise doesn't require an active VRE server.

1. Make a copy of the supplied `VREServer` project and open the copy in the PDK.
2. Open `plugin.json` and find the `Players` skin.
3. Change the visible text label from `PLAYERS` to `CREW ONLINE` (keep `Name:"Result"` unchanged).
4. Save, choose **Project → Build Plugin**, then **Project → Export RPGP**.
5. Install your copy into Power-Grid and add or refresh the Players control.

**Expected result:** The label says `CREW ONLINE`. The result still comes from the same C++ function. You've changed the presentation without changing the function's name or its result.

#### Second experiment: change a script's output

For a second safe exercise, change this line in `TickPlayers`:

```js
d.result.setText(String(value));
```

to:

```js
d.result.setText("Crew: " + String(value));
```

Save and re-export the RPGP. Reinstall or reload it and check the control.

**Expected result:** The numeric value is prefixed with `Crew:`. The C++ DLL still provides the same underlying value; only the QS presentation has changed.

If the control runs out of room for its text, that's a layout limitation, not a failed native call. Restore the original line after experimenting.

### 3.8 Common mistakes

| Symptom | Where to look |
| --- | --- |
| No VREServer controls appear | `CtrlSets` path and references in `VREServer.ctrlset.json` |
| Skin appears but stays at `--` | QS `OnInit` / `OnTick` function names, `FindLayout("Result")`, and whether the server can be queried |
| Script says it can't find a function | `VREServer:Players` spelling, `GetName()`, and the list returned by `GetList()` |
| Connection settings seem ignored | `HostName` / `QueryPort` option keys versus the strings read by QS |
| DLL doesn't load | `DynamicLibraries` filename, build errors and x86 target |
| Editing a file changes nothing | Save, export again, reinstall; reopen a PDK tab after an external file overwrite |
| Your text doesn't fit | Skin layout dimensions, font size and `Result` text area |

### 3.9 What you've learned

A function provider turns a native C++ result into something a grid control can display:

```text
VREServer.ctrlset.json → offers Players in the control picker
plugin.json           → defines Players' settings, skin and script hooks
VREServer.qs           → calls VREServer:Players with host and port
vreserver.cpp          → GetValue("Players", params) returns the value
VREServer.qs           → writes that value into the Result text element
```

You now know the two supplied plugin types:

- **Incoming data source:** C++ provides entries for Incoming Center.
- **Function provider:** QS/control asks C++ for named values.

Both use the core project files explained in Part 1, and both build into installable RPGP packages through the same PDK.

**Next steps:** Try changing friendly names and labels, then create a copy of a working sample for your own application. Keep interface method signatures and linked identifiers consistent as you modify it.

**Source note:** This chapter's excerpts were checked against `NativeBuild/VREServer` in the PDK v0.3.30 release-ready source archive, whose demonstrated interfaces carry forward into v0.3.31. The source supplied with your installed PDK is authoritative if details differ.

---

## Part 4 — Writing Your Own C++ Plugin

**Starting point:** A copy of `NativeBuild/Sample` or `NativeBuild/VREServer` from the PDK.  
**You'll learn:** how the native source files fit together, what the SDK methods do, and how to produce your own working variation.

**Start by copying a working sample.** That preserves its already-tested JSON, Qt registration and build setup while you learn to write the C++ behaviour. We will not use or distribute DiscordNotifier source.

### 4.1 What belongs in the C++ files?

| File | What it does |
| --- | --- |
| `plugin.json` | Declares the DLL, settings and available capabilities |
| `.h` | Declares your C++ class and the methods Power-Grid can call |
| `.cpp` | Implements your plugin's behaviour |
| `.qs` | Connects a native function provider to visible grid controls, when needed |

The incoming-data sample does not require a QS script. The VREServer function provider uses QS to update its grid controls.

### 4.2 Declare your class

Open `NativeBuild/Sample/Source/TestIncomingSourceClang.h`. The sample's working declaration includes:

```cpp
#include "RoccatPowerGrid.h"
#include <QtCore/QMutex>

class TestIncomingSourceClang : public IIncomingDataSource
{
    Q_OBJECT
    Q_INTERFACES(IIncomingDataSource)
public:
    TestIncomingSourceClang();
    virtual ~TestIncomingSourceClang();
    virtual QString GetName();
    virtual void SetPluginData(PGConfiguration value);
    virtual PGConfiguration GetPluginData();
    virtual PGIncomingEntries GetEntries();
    virtual bool IsActive();
private:
    QMutex m_mutex;
    PGConfiguration m_config;
    qint64 m_entryTime;
};
```

- **`RoccatPowerGrid.h`** provides the SDK interfaces.
- **`TestIncomingSourceClang`** is the developer's class name; its implementations use that name followed by `::`.
- **`IIncomingDataSource`** says this is a source of Incoming Center entries.
- **`Q_OBJECT`** and **`Q_INTERFACES`** register the class with Qt's plugin system.
- **The virtual methods** are called by Power-Grid. Keep their parameter and return types consistent with the interface.
- **`m_config`**, **`m_entryTime`** and **`m_mutex`** hold settings, one entry's timestamp and a lock for safe access.

For your first exercise, keep the class name and the interface declarations unchanged. Make your changes inside the methods.

### 4.3 Start with the constructor

The sample constructor prepares an initial message:

```cpp
TestIncomingSourceClang::TestIncomingSourceClang()
    : m_entryTime(QDateTime::currentMSecsSinceEpoch())
{
    m_config.insert("Message", QString("Hello from ClangIncomingProbe"));
}
```

The `Message` key matches an option `Name` in `plugin.json`; it isn't a display label. The text beside it is the starting value. A previously saved host setting may replace that default.

The timestamp is assigned **once**, so a repeated `GetEntries()` call doesn't manufacture a new message every time.

**Try it:** Change the default string, build and install a fresh copy. If the old wording remains, inspect the saved `Message` setting before assuming the DLL failed.

### 4.4 Identify the source and keep its settings

The working identity method is:

```cpp
QString TestIncomingSourceClang::GetName()
{
    return QString("ClangIncomingProbe");
}
```

That name matches the incoming data-source identifier in this sample's `plugin.json`. It's not the title displayed inside an individual message.

The pair of configuration methods exchange the settings map with Power-Grid:

```cpp
void TestIncomingSourceClang::SetPluginData(PGConfiguration value)
{
    QMutexLocker lock(&m_mutex);
    m_config = value;
}

PGConfiguration TestIncomingSourceClang::GetPluginData()
{
    QMutexLocker lock(&m_mutex);
    return m_config;
}
```

Think of them as **“restore my settings”** and **“save my settings.”** The key `"Message"` must remain consistent between the JSON option and the C++ lookup.

### 4.5 Write an Incoming Center entry

Here's the method from the supplied incoming-data sample, without changing its entry structure:

```cpp
PGIncomingEntries TestIncomingSourceClang::GetEntries()
{
    QMutexLocker lock(&m_mutex);
    PGConfiguration entry;
    entry.insert("time", QVariant(static_cast<qlonglong>(m_entryTime)));
    entry.insert("title", QString("ClangIncomingProbe"));
    QString text = m_config.value("Message").toString();
    if (text.isEmpty()) text = QString("(empty test message)");
    entry.insert("text", text);

    PGIncomingEntries result;
    result.insert(m_entryTime, entry);
    return result;
}
```

The keys `time`, `title` and `text` tell Incoming Center **what the values mean**. The words inside `title` and `text` are the values you can change.

`PGIncomingEntries` is a collection of timestamp-keyed entries. The sample returns one entry. The host can call the method repeatedly; this alone doesn't mean a new message should appear each time.

The companion method tells Power-Grid whether this source is available:

```cpp
bool TestIncomingSourceClang::IsActive()
{
    return true;
}
```

**Try it:** Change the message `title` and rebuild. Expect a different heading, with the body still taken from `Message`. For a previously displayed entry, Incoming Center may retain its cached copy; test in a fresh plugin session.

### 4.6 The Qt plugin registration

The sample exports:

```cpp
Q_EXPORT_PLUGIN2(ClangIncomingProbe, TestIncomingSourceClang)
```

The second name identifies the C++ class Qt creates. The first is the Qt factory identifier. Neither should be casually renamed during the introductory exercise: they are part of the working project's registration.

### 4.7 What changes for a Function Provider?

Open `NativeBuild/VREServer/Source/vreserver.h`. Here the class derives from `IFuncProvider`. In addition to its common methods, the interesting methods are:

```cpp
virtual const QStringList &GetList();
virtual QVariant GetValue(QString name, const QVariantList &params);
```

`GetList()` tells Power-Grid which named values the plugin offers; `GetValue()` returns one of them, optionally using parameters. The script makes a request such as:

```js
var value = this.CallFunc("VREServer:Players", [d.host, d.port]);
```

The provider sees the name `Players` and the host/port arguments. In the working C++ source, the Players branch formats the result:

```cpp
if (name == QLatin1String("Players"))
    return QString::number(s.players) + QLatin1String(" / ") +
           QString::number(s.maximum);
```

This is a **snippet** from the full method, not a standalone program. The values in `s` come from VREServer's query implementation. Part 3 shows how QS puts the returned text into a grid control.

For a first experiment, change the QS text formatting described in Part 3; there's no need to rewrite the server query engine.

### 4.8 Build, export, install and test

1. Copy one of the included projects into your own workspace.
2. Make **one small change** and save the edited file with **Ctrl+S**.
3. Choose **Project → Build Plugin** and check the compiler output.
4. Choose **Project → Export RPGP**.
5. Install the new package in Power-Grid and open its corresponding feature.
6. Compare the outcome to the **expected result** in the exercise, then try a second change.

The exported RPGP contains the DLL, manifests, scripts and necessary assets—not your developer C++ source or SDK headers.

### 4.9 When something doesn't work

| Symptom | What to check |
| --- | --- |
| Compiler reports a missing or mismatched method | Method name, parameter types and return type in the supplied SDK |
| Qt doesn't load the plugin | `Q_OBJECT`, `Q_INTERFACES`, `Q_EXPORT_PLUGIN2` and the build log |
| Power-Grid cannot find the DLL | `DynamicLibraries` in `plugin.json` versus the exported DLL filename |
| Incoming Center shows nothing | `GetName()`, `GetEntries()`, `IsActive()` and the manifest's data-source name |
| Edited text doesn't show | Saved settings, cached notifications, whether you installed the new RPGP |
| Grid control stays at its placeholder | QS `OnInit`/`OnTick`, `FindLayout("Result")`, provider and function names |
| PDK editor shows older file contents | Close and reopen the tab after an external overwrite; refreshing the tree doesn't reload it |

### 4.10 Try it yourself: your first variation

Start with a **copy** of the incoming-data `Sample` project. Keep the interface names intact initially.

1. Set the default `Message` to `My first Power-Grid plugin`.
2. Change the entry's `title` to `MyFirstNotice`.
3. Update `Config.Description` in `plugin.json` to describe your plugin.
4. Build and export a new RPGP.
5. Install it and confirm both its heading and message appear.
6. Edit `Message` through the native settings panel, then check that Incoming Center uses the saved value after a fresh session.

**Expected result:** You have customised the native behaviour and host settings of a working plugin without changing its tested interface.

When you're comfortable with that, use the name/reference explanations in Parts 1–3 to create a fully renamed project, updating all dependent references together.

**You've now moved from reading other people's plugin code to writing your own native Power-Grid behaviour.**
