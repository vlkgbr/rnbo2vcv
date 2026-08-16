# rnbo2vcv

Convert Max/MSP RNBO patches to VCV Rack 2 plugins.

```
.rnbopat → RNBO C++ Export → build.py [parser → layout → codegen] → make → plugin.so
```

For ready-to-build examples and step-by-step setup instructions, see [rnbo2vcv-examples](https://gitlab.com/vlkgbr/rnbo2vcv-examples).

---

## Requirements

| Tool | Version | Purpose |
|---|---|---|
| Python | 3.8+ | Runs the generator |
| Max 8 / Max 9 | with RNBO | Exports patch to C++ (no native Linux; use Wine/Bottles) |
| VCV Rack SDK | 2.x | Headers + build system ([download](https://vcvrack.com/downloads)) |
| gcc / g++ / make | — | C++ compilation |

Place the unzipped SDK as `Rack-SDK/` in your project root.

---

## Quick Start

```bash
python3 build.py
```

The interactive prompt walks you through everything. The script auto-detects your RNBO export folder by scanning for `description.json`. For scripted builds, see [CLI Reference](#cli-reference) below.

---

## Smart Naming Convention

Every parameter needs a `param` object in RNBO:

```
[param paramName @value <default> @min <min> @max <max>]
```

### Prefix table

Encode the **control type** and **column grouping** in the parameter name: `[prefix]_[core_name]`

| Prefix | Widget | Rack Type | Purpose |
|---|---|---|---|
| `base_` | Large knob | `RoundHugeBlackKnob` | Primary value (unipolar) |
| `attenv_` | Small knob | `RoundSmallBlackKnob` | Bipolar attenuverter (−1 → +1) |
| `atten_` | Trim pot | `Trimpot` | Unipolar attenuator (0 → 1) |
| `button_` | Button | `VCVButton` | Gate: 1.0 while held, 0.0 on release |
| `trigger_` | Button | `VCVButton` | Bang: sends 1.0 on press only, ignores release |
| `switch_` | Toggle | `CKSS` | Binary 0.0 / 1.0 |
| `menu_` | Dropdown | `CustomMenuWidget` | Integer selection. Prompts for entry labels during build. |
| `stepN_` | Stepped knob | `StepKnob` | Clicks into discrete values. N = step size. |

> **`attenv_` vs `atten_`:** Same parameter range logic, different physical widget. `attenv_` gives you a knob; `atten_` gives you a screwdriver-style trim pot.

> **`button_` vs `trigger_`:** `button_` sends 1.0 on press AND 0.0 on release (gate). `trigger_` sends 1.0 on press only (bang).

> **`menu_`:** Item count is derived from `@min`/`@max`. You assign labels interactively or via `--menu MODE:Sine,Tri,Saw`.

> **`stepN_`:** `step1_octave @min 1 @max 5` → outputs exactly 1, 2, 3, 4, 5. The knob clicks securely into each position.

### Column grouping

Parameters sharing the same `[core_name]` stack vertically:

1. `base_` knob (top)
2. `attenv_` / `atten_` controls
3. `button_` / `switch_` / `menu_` / `stepN_` controls
4. Mapped `in~` jack (via `@comment core_name`)
5. Mapped `out~` jack (via `@comment core_name`)

**Example — filter with CV:**
```
[param base_cutoff @min 20 @max 20000 @value 2000]
[param attenv_cutoff @min -1 @max 1 @value 0]
[in~ 1 @comment cutoff_cvi]
```

### Jack labeling with `@comment`

Map audio I/O to columns and assign port types via `@comment`:

| Comment suffix | Port type |
|---|---|
| `cvi` | CV Input |
| `cvo` | CV Output |
| `audioi` | Audio Input |
| `audioo` | Audio Output |
| `inl` / `inr` | Stereo Input Left/Right |
| `outl` / `outr` | Stereo Output Left/Right |

Format: `@comment <core_name>_<port_type>` (underscore optional).

### Stereo I/O

If `in~ 1` and `in~ 2` are both unmapped, they auto-promote to `IN_L` / `IN_R` with mono normalization. Same for `out~ 1` / `out~ 2` → `OUT_L` / `OUT_R`.

---

## Polyphony & Signal Levels

**Polyphony:** Answer `yes` during build to enable 16-voice polyphony. All I/O becomes polyphonic — mono cables broadcast to all voices, poly cables route per-voice. Don't mix stereo I/O with polyphony.

**Signal levels:** Voltages pass 1:1 between VCV Rack and RNBO. No hidden scaling. Audio ±5V, envelopes 0–10V, V/Oct = 1.0/octave. Hard-clamped at ±12V.

---

<details>
<summary><strong>CLI Reference</strong></summary>

```
python3 build.py [OPTIONS]
```

| Flag | Default | Description |
|---|---|---|
| `--rnbo-dir <path>` | auto-detect | Folder containing RNBO export (`description.json` + `.cpp`) |
| `--module-name <name>` | — | CamelCase module name (e.g. `MySynth`) |
| `--plugin-slug <slug>` | — | Unique alphanumeric plugin ID |
| `--manufacturer <name>` | — | Brand name for VCV browser |
| `--author <name>` | — | Author name for plugin.json |
| `--version <ver>` | `2.0.0` | Must start with `2.` |
| `--license <lic>` | `GPL-3.0` | Software license |
| `--block-size <int>` | `64` | DSP block size (1 = minimal latency) |
| `--ui-text <yes\|no>` | `yes` | Generate C++ text labels on panel |
| `--polyphony <yes\|no>` | `no` | Enable 16-voice polyphony |
| `--custom-layout <yes\|no>` | `no` | Interactive component placement |
| `--custom-ports <yes\|no>` | `yes` | Customize jack types |
| `--menu <string>` | — | Pre-fill menu entries (e.g. `MODE:Sine,Tri;LFO:A,B`) |
| `--non-interactive` | — | Skip all prompts, use defaults |

**Example:**
```bash
python3 build.py --module-name MyPatch --polyphony yes --non-interactive
```

</details>

<details>
<summary><strong>Custom SVG Widgets</strong></summary>

Place SVGs in `res/` next to `build.py`. No CLI flags needed — detected automatically. Missing files fall back to Rack built-ins.

```
res/
├── panel.svg           ← Light theme panel (auto-detects width from SVG)
├── panel-dark.svg      ← Dark theme (optional, light reused if missing)
├── knob_large.svg      ← base_ knob
├── knob_small.svg      ← attenv_ knob
├── knob_trim.svg       ← atten_ trim pot
├── knob_default.svg    ← plain param knob
├── step_knob.svg       ← stepN_ knob
├── button.svg          ← button_ unpressed
├── button_pressed.svg  ← button_ pressed
├── trigger.svg         ← trigger_ unpressed
├── trigger_pressed.svg ← trigger_ pressed
├── switch_off.svg      ← switch_ off
├── switch_on.svg       ← switch_ on
├── port_cv_in.svg      ← CV input
├── port_cv_out.svg     ← CV output
├── port_audio_in.svg   ← Audio input
├── port_audio_out.svg  ← Audio output
├── port_in.svg         ← Generic input fallback
└── port_out.svg        ← Generic output fallback
```

**Knob SVG tip:** VCV Rack's `SvgKnob` rotates around center. Design with indicator at 12 o'clock.

</details>

<details>
<summary><strong>How It Works</strong></summary>

```
RNBO C++ export → build.py → parser.py → layout.py → codegen.py → make → plugin.so
```

| Stage | File | What it does |
|---|---|---|
| Parse | `rnbo2vcv/parser.py` | Reads `description.json`, extracts parameters, smart-name prefixes, `@comment` mappings |
| Model | `rnbo2vcv/models.py` | Data classes for parameters and patch info |
| Layout | `rnbo2vcv/layout.py` | Calculates panel width (HP), column positions, component coordinates |
| Codegen | `rnbo2vcv/codegen.py` | Generates `plugin.hpp`, `plugin.cpp`, `Module.cpp`, `panel.svg`, `Makefile`, `plugin.json` |
| Writer | `rnbo2vcv/writer.py` | Orchestrates the pipeline, handles CLI args, interactive layout, file output |
| Build | `build.py` | Interactive prompts, invokes writer, copies sources, runs `make`, installs plugin |

</details>

---

## Contributing

Pull requests, code reviews, and refactoring are welcome. The smart-naming convention and layout engine are stable; the codegen layer is where most improvements can happen.

---

## License

- **This tool** (rnbo2vcv): [0BSD](LICENSE.txt) — do whatever you want.
- **Generated plugins** link against the VCV Rack SDK — see [VCV's licensing terms](https://vcvrack.com/manual/PluginDevelopmentTutorial).
- **RNBO runtime** is © Cycling '74 — see their license terms for redistribution.

---

## Note on AI and Development

The concept, naming conventions, layout engine rules, and signal routing logic are my own design. The Python and C++ implementation was written with heavy use of AI coding assistants. I'm a musician and Max patcher, not a programmer — PRs and code reviews are welcome.

---

## Credits

- **RNBO and Max/MSP** by [Cycling '74](https://cycling74.com/)
- **VCV Rack SDK** © VCV LLC
- Built upon the foundations of [pd2vcv](https://gitlab.com/vlkgbr/pd2vcv)
