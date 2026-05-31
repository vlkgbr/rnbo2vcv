# rnbo2vcv

**Turn a Max/MSP RNBO patch into a working VCV Rack 2 plugin.**

**Known Stable Environment**
> This generator was built and verified working with the following stack:
> - **OS:** Debian GNU/Linux 13 (Trixie) x86_64
> - **OS:** Windows 11 (MSYS2 MINGW64)
> - **Max 8 / Max 9** (with RNBO)
> - **VCV Rack:** v2.6.6 (SDK)

`build.py` reads your exported RNBO C++ source code, parses your parameter declarations, generates C++ wrapper code, builds `plugin.so`, and installs it into VCV Rack automatically.

---

## Table of Contents

- [Quickstart](#quickstart)
- [Usage Instructions](#usage-instructions)
  - [Step 1 — Export your patch to C++](#step-1--export-your-patch-to-c)
  - [Step 2 — Run the Generator](#step-2--run-the-generator)
    - [Interactive Mode (Default)](#interactive-mode-default)
    - [Scripting / CLI Mode](#scripting--cli-mode)
  - [Step 3 — Compile and Install](#step-3--compile-and-install)
- [How to structure your project](#how-to-structure-your-project)
- [Prerequisites & Platform Compatibility](#prerequisites--platform-compatibility)
- [RNBO Naming Conventions](#rnbo-naming-conventions)
  - [Smart naming convention](#smart-naming-convention-recommended)
  - [Jack labeling with @comment](#jack-labeling-with-comment)
  - [Stereo I/O normalization](#stereo-io-normalization)
- [Custom Component Placement & UI](#custom-component-placement--ui)
- [Custom SVG Widgets](#custom-svg-widgets)
- [Polyphony](#polyphony)
- [Audio and CV signal levels](#audio-and-cv-signal-levels)
- [Troubleshooting](#troubleshooting)
- [Note on AI and Development](#note-on-ai-and-development)
- [Credits](#credits)

---

## Quickstart

If you want to see exactly how your files should be organized, check out the **`FMExample`** and **`PolySawExample`** folders! They are ready-to-build templates that demonstrate the correct folder structure.

> [!IMPORTANT]
> **Check your Rack-SDK!** The examples in this repository include the **Linux** VCV Rack SDK. If you are building on Windows or macOS, you **must** delete the `Rack-SDK` folder and replace it with the correct SDK for your operating system downloaded from [vcvrack.com/downloads](https://vcvrack.com/downloads). A Linux SDK will fail to compile on Windows/macOS.

To build an example, live-navigate to one of the folders, open a terminal there, and run the generator:
```bash
cd FMExample/
python3 build.py
```
You will be greeted by an interactive command prompt. Simply press **Enter** to accept the default settings, or type a new value to change them.
*(Note: If you run PolySawExample, make sure to type `yes` when asked about Polyphony, as it defaults to `no`!)*

Once the script finishes, restart VCV Rack to see your module!

---

## Usage Instructions

### Step 1 — Export your patch to C++

1. Open your patch inside a `rnbo~` object in Max.
2. Open the **Export Sidebar** on the right side of the RNBO window.
3. Select **C++ Source Code Export**.
4. Set the **Output Directory** to a folder inside your project directory (e.g., `rnbo_export/`).
5. Click **Export**.

### Step 2 — Run the Generator

Simply run the master build script from your project directory. 

#### Interactive Mode (Default)
This is the recommended way to use the generator. It features an interactive command prompt that guides you through the configuration step-by-step:

```bash
python3 build.py
```
*The script actively scans your project directory for the RNBO export files (`description.json` and `rnbo_source.cpp`) and will automatically detect the folder for you, regardless of what you named it.*

The interactive prompt handles configuration options without requiring CLI flags. It will ask if you want to enable **Custom Placement** (to override component positions), enable **Polyphony**, or customize port types.

#### Scripting / CLI Mode
If you prefer scripting or want to bypass the interactive prompts, you can pass arguments directly via CLI. 

Available flags:
- `--rnbo-dir <path>`: Folder containing the RNBO generated output (`description.json` and `rnbo_source.cpp`)
- `--module-name <name>`: CamelCase name of your module (e.g., `MySynth`)
- `--plugin-slug <slug>`: Unique alphanumeric ID for the plugin
- `--manufacturer <name>`: Your brand name, shows in the VCV Rack browser
- `--author <name>`: Your name, added to plugin.json
- `--version <version>`: Plugin version. Must start with '2.'
- `--license <license>`: Software license (e.g., `GPL-3.0`)
- `--block-size <int>`: DSP processing block size (default `64`)
- `--polyphony <yes|no>`: Enable 16-voice polyphony
- `--custom-layout <yes|no>`: Enable interactive component placement
- `--custom-ports <yes|no>`: Customize jack types via prompt
- `--non-interactive`: Skips the interactive prompt entirely. Any arguments you don't specify will fall back to their defaults.

Example:
```bash
python3 build.py --module-name MyPatch --custom-ports yes --polyphony yes --non-interactive
```
*(Note: The generator automatically scans for your RNBO C++ export folder by looking for `description.json` and `.cpp` files, so you usually don't need to specify `--rnbo-dir`!)*

### Step 3 — Compile and Install

The `build.py` script copies your C++ sources, compiles the VCV Rack plugin using `make`, and installs it directly into your VCV Rack `plugins` directory.

Restart VCV Rack 2 to load the module.

---

## How to structure your project

To use `build.py`, your project folder should look like this:

```
my_project/
├── build.py              ← Master build script
├── rnbo2vcv/             ← Internal Python modules
├── res/                  ← Custom UI SVGs (optional)
├── Rack-SDK/             ← Unzipped VCV Rack 2 Plugin SDK
│
└── rnbo_export/          ← Your RNBO C++ Export Directory
    ├── description.json
    └── rnbo_source.cpp
```

---

## Prerequisites & Platform Compatibility

| Tool | Why | How to get it |
|---|---|---|
| **Python 3.8+** | Runs the generator | Linux: `apt install python3` / Mac: `brew install python` / Windows: Python installer |
| **Max 8 / Max 9 (RNBO)** | Exports RNBO to C++ | cycling74.com. *Note: Max has no native Linux version. Use Wine (e.g., via Bottles) to run it on Linux.* |
| **VCV Rack 2 SDK** | Headers + build system | [vcvrack.com/downloads](https://vcvrack.com/downloads) → "Plugin SDK" |
| **gcc / g++ / make** | C++ compilation | Linux: `apt install build-essential` / Mac: Xcode / Win: MSYS2 |

### OS & SDK Compatibility

VCV Rack plugins must be compiled using the GNU toolchain (`gcc`, `g++`, and `make`).
- **Linux & macOS:** Works natively. Open your terminal, ensure you have build tools (like `make` and `gcc`) or Xcode installed, and you are ready.
- **Windows:** VCV Rack requires the MinGW64 toolchain. Since this tool is designed for patchers and musicians, here is the exact step-by-step to get your Windows PC ready:
  1. Download and install **MSYS2** from [msys2.org](https://www.msys2.org/).
  2. Open your Windows Start Menu and search for **"MSYS2 MinGW x64"** (Look for the **blue** icon! Do not use the purple MSYS icon, and do not use standard Windows PowerShell/CMD).
  3. In the terminal that opens, run this command to update the system (press `Y` when prompted. If the terminal closes, open it again and re-run):
     ```bash
     pacman -Syu
     ```
  4. Finally, install the complete VCV Rack toolchain by copying and pasting this exact command (press `Y` when prompted):
     ```bash
     pacman -Su git wget make tar unzip zip mingw-w64-x86_64-gcc mingw-w64-x86_64-gdb mingw-w64-x86_64-cmake autoconf automake libtool mingw-w64-x86_64-jq python3
     ```
  Once this finishes, you can use this blue terminal to run `python3 build.py` on any of your projects!

**CRITICAL:** You must download the SDK that matches your build platform from the VCV Rack website, unzip it, and place it as a folder named `Rack-SDK/` in your project root. A Linux SDK will not build Windows plugins.

---

## RNBO Naming Conventions

Every knob you want in VCV Rack needs a **`param`** object in your RNBO patch:

```
[param paramName @value <default> @min <min> @max <max>]
```

Example:
```
[param cutoff @value 1000 @min 20 @max 20000]
```

This creates a knob with range 20–20,000, defaulting to 1000.

---

### Smart naming convention (recommended)

Encode the **control type** and **column grouping** in the parameter name:

```
[ui_prefix]_[core_name]
```

| Prefix | Widget in VCV Rack | Rack type | Use for |
|---|---|---|---|
| `base_` | Large knob | `RoundHugeBlackKnob` | Primary value control (unipolar) |
| `attenv_` | Small knob | `RoundSmallBlackKnob` | Bipolar attenuverter (−1 → +1) |
| `atten_` | Tiny trim pot | `Trimpot` | Unipolar attenuator (0 → 1) |
| `button_` | Momentary button | `VCVButton` | Gate behavior (1.0 while held, 0.0 on release) |
| `trigger_` | Momentary button | `VCVButton` | Bang/trigger behavior (Sends 1.0 on press, ignores release) |
| `switch_` | Toggle switch | `CKSS` | Binary mode selection (0.0 / 1.0) |

> **`attenv_` vs `atten_`:** Both produce small controls, but they use different Rack widgets. `attenv_` uses `RoundSmallBlackKnob` (a knob you turn); `atten_` uses `Trimpot` (a smaller screwdriver-style trim pot). The parameter range is set automatically from your RNBO `@min`/`@max` values.

> **`button_` vs `trigger_`:** Both render as the same physical button widget, but behave differently under the hood. 
> - Use `button_` if you want a **Gate** (e.g. holding it down keeps an envelope open). It sends 1.0 when pressed and 0.0 when released.
> - Use `trigger_` if you want a **Bang**. It sends a single 1.0 value on the exact moment you press it and ignores the release.

Parameters sharing the same `[core_name]` are grouped in one **vertical column** on the panel. Column order (top to bottom):
1. `base_` knob
2. `attenv_` / `atten_` controls
3. `button_` / `switch_` controls
4. Mapped `in~` jack (via `@comment core_name`)
5. Mapped `out~` jack (via `@comment core_name`)

---

### Jack labeling with `@comment`

You can map your audio inputs (`in~`) and outputs (`out~`) to specific parameter columns and assign them custom port designs by adding a `@comment` attribute in RNBO.

The comment structure is:
`<core_name>_<port_type>` (Underscore is optional, e.g. `cutoff_cvi` or `cutoffcvi`)

#### Available Port Types
- `cvi`: Custom CV Input port (White ring)
- `audioi`: Custom Audio Input port (Red ring)
- `cvo`: Custom CV Output port (White ring)
- `audioo`: Custom Audio Output port (Red ring)
- `inl` / `inr`: Stereo Audio Input (Left/Right)
- `outl` / `outr`: Stereo Audio Output (Left/Right)

**Example — VCO with CV mapping and audio output:**
```
[param base_freq @min 20 @max 20000 @value 440]
[in~ 1 @comment freq_cvi]
[out~ 1 @comment audioo]
```

Generates:
```
  [Large knob]     ← base_freq        (top)
  [□ white jack]   ← in~ 1 (FREQ IN)  (row 3)

  [■ red jack]     ← out~ 1 (OUT)     (bottom unmapped row)
```

Any `in~` inputs or `out~` outputs **not** mapped to a column appear in the bottom row. 

---

### Stereo I/O normalization

If your patch uses **`in~ 1` and `in~ 2` and neither is mapped to a column**, the generator automatically promotes them to a stereo pair:
- `IN_1` → **`IN_L`**
- `IN_2` → **`IN_R`**

You can also explicitly force inputs or outputs into stereo pairs (and automatically update their UI text) by tagging them with `@comment inl`, `inr`, `outl`, or `outr`.

Standard VCV Rack mono-normalization is applied in the generated C++:
> When only `IN_L` is patched, `IN_R` automatically carries the same signal (mono mode).
> When both are patched, each routes independently to `in~ 1` and `in~ 2` in your patch (stereo mode).

Same rule applies to outputs: if your patch produces **`out~ 1` and `out~ 2`** and neither is claimed by a column mapping, they are promoted to **`OUT_L` / `OUT_R`**.

---

## Custom Component Placement & UI

If you want precise control over where every knob, jack, and button appears on your module panel, simply answer `yes` when the interactive prompt asks about **Custom Layout**.

### How it works

1. The auto-layout engine runs first and calculates default positions for all components based on your column core names.
2. You see each component with its auto-position and can override it in two steps.
3. Your overrides are saved to `.rnbo2vcv_layout.json` so you never have to re-enter them on subsequent builds.

### Customizing Jack Types Only

If you prefer the automatic layout but still want to assign jack types (audio/cv/stereo) interactively instead of using `@comment`, answer `no` to Custom Layout but `yes` to Custom Ports in the interactive prompt.

---

## Custom SVG Widgets

The generator relies on a `res/` folder next to `build.py` which is **always active**. You don't need any CLI flags to enable custom SVGs—simply place them in the `res/` folder and they will be used.

The system supports the following 16 SVG files, all of which are **optional**. Missing files automatically fall back to Rack's built-in widgets.

### Expected folder structure

```
res/
├── panel.svg           ← Light theme panel background
├── panel-dark.svg      ← Dark theme panel background (optional, light is reused if missing)
├── knob_large.svg      ← Replaces base_ knob
├── knob_small.svg      ← Replaces attenv_ knob
├── knob_trim.svg       ← Replaces atten_ trim pot
├── knob_default.svg    ← Replaces plain param knob
├── button.svg          ← Gate button unpressed state (button_ suffix)
├── button_pressed.svg  ← Gate button pressed state
├── trigger.svg         ← Momentary trigger unpressed state (trigger_ suffix)
├── trigger_pressed.svg ← Momentary trigger pressed state
├── switch_off.svg      ← Toggle switch off position
├── switch_on.svg       ← Toggle switch on position
├── port_cv_in.svg      ← CV input (light blue ring)
├── port_cv_out.svg     ← CV output (dark blue ring)
├── port_audio_in.svg   ← Audio input (silver ring)
├── port_audio_out.svg  ← Audio output (black ring)
├── port_in.svg         ← Generic input fallback
└── port_out.svg        ← Generic output fallback
```

### Panel SVG

The auto-generated panel is completely blank (only a solid rectangle with top/bottom borders) to avoid drawing component markers under your custom layout. 

If you provide a custom `panel.svg`, the generator:
- **Skips** auto-generating the blank panel
- **Auto-detects** the panel width from the SVG's `width` attribute (in mm)
- Uses your panel for both light and dark themes unless `panel-dark.svg` is also provided

---

## Polyphony

During the `python3 build.py` interactive prompts, you have the option to enable **16-voice polyphony**.

If you select `yes`, `rnbo2vcv` modifies the generated C++ wrapper to spin up 16 parallel RNBO processing contexts. This completely changes how inputs and outputs behave, aligning them with the VCV Rack polyphony standard:

### 1. Polyphonic Broadcasting
You do not need to specify "which" input is polyphonic. If polyphony is enabled, **ALL** inputs and outputs become polyphonic. The plugin will check the number of channels on every connected cable (1 to 16 channels) and automatically activate the highest number of voices.

- **Mono Cables (1 channel):** A mono cable plugged into a polyphonic module (e.g. a single LFO into a filter cutoff) will be broadcast to **all active voices**. This allows global modulation.
- **Poly Cables (2-16 channels):** A polyphonic cable (e.g. 4 channels of V/Oct pitch) will be routed individually. Voice 1 gets Pitch 1, Voice 2 gets Pitch 2, etc.

### 2. Stereo I/O Constraints
When you enable polyphony, the standard VCV Rack "Stereo Normalization" rule (Left/Right outputs) goes out the window, because polyphonic outputs are multiplexed over a single mono-per-voice cable.

**Do not design stereo patches for polyphony!** 
If you want to build a polyphonic synth, your patch should output a **single `out~ 1`**. The wrapper will automatically take the 16 independent `out~ 1` outputs from your 16 voices and pack them into a 16-channel polyphonic cable. If you absolutely need stereo polyphony, output `out~ 1` for Left and `out~ 2` for Right, and your module will produce two separate 16-channel polyphonic cables.

---

## Audio and CV signal levels

Voltages are passed exactly 1:1 between VCV Rack and RNBO. There is NO hidden scaling applied by the C++ wrapper. 

| VCV Rack | RNBO (inside patch) |
|---|---|
| +10.0 V | +10.0 |
| +5.0 V | +5.0 |
| +1.0 V | +1.0 |
| −5.0 V | −5.0 |

**What this means for your RNBO patch:**
*   **Audio & LFOs:** VCV Rack audio standard is ±5V. Expect your `in~` objects to receive signals between -5.0 and +5.0. 
*   **Envelopes:** VCV Rack envelopes are 0 to 10V. Expect your `in~` to receive 0.0 to 10.0.
*   **V/Oct Pitch:** 1V per octave = 1.0 per octave in RNBO.

If your RNBO patch strictly expects normalized signals (-1.0 to 1.0 or 0 to 1.0), you must manually scale them down inside the patch (e.g., attach a `*~ 0.2` right after your `in~` to convert ±5V down to ±1.0).

*Safety note: The C++ wrapper hard-clamps all `in~` inputs and `out~` outputs at **±12.0V** (Eurorack physical standard) to prevent runaway math from crashing other modules.*

---

## Note on AI and Development

I am a musician and patcher, not a C++ or Python programmer. The concept, the smart-naming conventions, the UI layout engine rules, and the signal routing logic are entirely my own design.
However, to bring this idea to life, I relied heavily on AI coding assistants to write the actual Python and C++ syntax. I am being completely transparent about this because I know the open-source community's feelings on AI can be mixed. My goal was simply to bridge a gap between graphical patching and VCV Rack that I desperately wanted for my own music, and I used the tools available to me to build it.
Because I am not a native C++ developer, the codebase might have structural quirks. Human pull requests, code reviews, and refactoring are incredibly welcome.

---

## Credits

- **RNBO and Max/MSP** by [Cycling '74](https://cycling74.com/)
- **VCV Rack SDK** © VCV LLC
- **rnbo2vcv.py** built upon the foundations of [pd2vcv](https://github.com/yourusername/pd2vcv)
