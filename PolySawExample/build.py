#!/usr/bin/env python3
# ─────────────────────────────────────────────────────────────────────────────
# build.py  —  rnbo2vcv master build script
# ─────────────────────────────────────────────────────────────────────────────
import os
import sys
import json
import shutil
import platform
import argparse
import subprocess
import re
from pathlib import Path

# ── Config block ──────────────────────────────────────────────────────────────
MODULE_NAME  = "MyModule"
PLUGIN_SLUG  = "MyPlugin"
MANUFACTURER = "YourName"
AUTHOR       = "YourName"
VERSION      = "2.0.0"
LICENSE      = "GPL-3.0"
BLOCK_SIZE   = 64
UI_TEXT      = "yes"
CUSTOM_LAYOUT = "no"
CUSTOM_PORTS  = "yes"
# ─────────────────────────────────────────────────────────────────────────────

def prompt_with_default(prompt_text, default_value):
    val = input(f"{prompt_text} [{default_value}]: ").strip()
    val = re.sub(r'\s+', ' ', val)
    return val if val else default_value

def auto_detect_rnbo_dir(script_dir, default_dir):
    script_dir = Path(script_dir)
    def is_rnbo_export(d):
        return (d / "description.json").exists() and (d / "rnbo").is_dir() and any(f.suffix == '.cpp' for f in d.iterdir() if f.is_file())
        
    try:
        if is_rnbo_export(script_dir):
            return "."
    except (PermissionError, OSError):
        pass

    try:
        for d in script_dir.iterdir():
            try:
                if d.is_dir() and is_rnbo_export(d):
                    return d.name
            except (PermissionError, OSError):
                continue
    except (PermissionError, OSError):
        pass
        
    return default_dir

def _interactive_layout(base_cmd, cache_path, env, prompt_positions=True, prompt_ports=True, menu_arg=None):
    _VALID_PORT_TYPES = {"cvi", "cvo", "audioi", "audioo", "inl", "inr", "outl", "outr"}
    dump_file = Path(".rnbo2vcv_dump.json").resolve()
    dump_cmd = base_cmd + ["--dump-layout-file", str(dump_file)]
    print("\n━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━")
    print("  Generating auto-layout for interactive placement...")
    try: subprocess.run(dump_cmd, check=True, env=env)
    except subprocess.CalledProcessError as e: sys.exit(f"  ERROR: Layout dump failed: {e}")

    if not dump_file.exists(): sys.exit("  ERROR: Could not find layout JSON dump file.")

    try:
        layout_data = json.loads(dump_file.read_text(encoding="utf-8"))
    except json.JSONDecodeError as e:
        sys.exit(f"  ERROR: Invalid layout JSON: {e}")
    finally:
        if dump_file.exists(): dump_file.unlink()
    components = layout_data.get("components", [])
    panel_hp = layout_data.get("panel_hp", 10)
    controls = [c for c in components if c["kind"] == "param" and c.get("knob_type") != "CustomMenuWidget"]
    menus    = [c for c in components if c["kind"] == "param" and c.get("knob_type") == "CustomMenuWidget"]
    jacks    = [c for c in components if c["kind"] in ("input", "output")]

    print(f"\n  Panel: {panel_hp} HP  ({panel_hp * 5.08:.1f} mm)")
    print(f"  {len(controls)} control(s), {len(menus)} menu(s), {len(jacks)} jack(s) detected.")

    overrides = {}
    menu_entries = {}

    if menu_arg:
        for group in menu_arg.split(";"):
            if ":" in group:
                k, v = group.split(":", 1)
                k = k.strip().upper()
                matched = k
                for m in menus:
                    if m["label"] == k or m["label"].replace("_MENU", "") == k.replace("MENU_", ""):
                        matched = m["label"]
                        break
                menu_entries[matched] = [x.strip() for x in v.split(",")]
            else:
                if len(menus) == 1:
                    menu_entries[menus[0]["label"]] = [x.strip() for x in group.split(",")]
    if prompt_positions:
        print("\n━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━")
        print("  STEP 1: Control Placement (Knobs, Buttons, Switches)")
        print("  Press [Enter] to keep auto-position, or type: x, y")
        print("━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━")
        for c in controls:
            kt = c.get("knob_type", "").replace("Round", "").replace("Black", "").replace("VCV", "")
            prompt = f"  {c['label']:20s} ({kt:12s}) [{c['x']:.1f}, {c['y']:.1f}]: "
            while True:
                val = input(prompt).strip()
                if not val: break
                try:
                    parts = [float(v.strip()) for v in val.split(",")]
                    if len(parts) >= 2:
                        overrides[c["label"]] = {"x": parts[0], "y": parts[1]}
                        break
                    else:
                        print("    ⚠ Needs two numbers separated by comma (e.g. 20, 45). Try again or press Enter to skip.")
                except ValueError:
                    print(f"    ⚠ Invalid format '{val}'. Try again or press Enter to skip.")

    if prompt_ports or prompt_positions:
        print("\n━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━")
        if prompt_positions and prompt_ports:
            print("  STEP 2: Jack Placement & Type")
        elif prompt_positions and not prompt_ports:
            print("  STEP 2: Jack Placement")
        else:
            print("  Jack Type Configuration")
            
        if prompt_ports:
            print("  Types:  cvi    = CV Input       cvo    = CV Output")
            print("          audioi = Audio Input     audioo = Audio Output")
            print("          inl    = Audio In Left   inr    = Audio In Right")
            print("          outl   = Audio Out Left  outr   = Audio Out Right")
        
        print("  Press [Enter] to keep defaults.")
        if prompt_ports and prompt_positions:
            print("  Type only (keep position): cvi")
            print("  Position only:             20, 45")
            print("  Both:                      cvi 20, 45")
        elif prompt_ports:
            print("  Type only: cvi")
        elif prompt_positions:
            print("  Position only: 20, 45")
        print("━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━")

        for c in jacks:
            direction = "Input Jack" if c["kind"] == "input" else "Output Jack"
            ptype_hint = f" [{c.get('port_type')}]" if c.get('port_type') else " []"
            prompt = f"  {c['label']:20s} ({direction:12s}) [{c['x']:.1f}, {c['y']:.1f}]{ptype_hint}: "
            while True:
                val = input(prompt).strip()
                if not val: break

                tokens = val.split()
                first_token = tokens[0].lower()

                if first_token in _VALID_PORT_TYPES and prompt_ports:
                    rest = val[len(first_token):].strip()
                    if rest and prompt_positions:
                        try:
                            parts = [float(v.strip()) for v in rest.split(",")]
                            if len(parts) >= 2:
                                overrides[c["label"]] = {"x": parts[0], "y": parts[1], "port_type": first_token}
                                break
                            else:
                                print("    ⚠ Needs two numbers separated by comma (e.g. 20, 45). Try again or press Enter to skip.")
                        except ValueError:
                            print(f"    ⚠ Invalid coords '{rest}'. Try again or press Enter to skip.")
                    else:
                        overrides[c["label"]] = {"port_type": first_token}
                        if rest and not prompt_positions:
                            print(f"    ⚠ Positions ignored (layout disabled). Type set to {first_token}")
                        break
                elif prompt_positions:
                    try:
                        parts = [float(v.strip()) for v in val.split(",")]
                        if len(parts) >= 2:
                            overrides[c["label"]] = {"x": parts[0], "y": parts[1]}
                            break
                        else:
                            print("    ⚠ Needs two numbers separated by comma (e.g. 20, 45). Try again or press Enter to skip.")
                    except ValueError:
                        print(f"    ⚠ Invalid input '{val}'. Try again or press Enter to skip.")
                else:
                    if prompt_ports and not prompt_positions:
                        print(f"    ⚠ Invalid type '{val}'. Try again or press Enter to skip.")
                    else:
                        print(f"    ⚠ Invalid input '{val}'. Try again or press Enter to skip.")

    if menus:
        print("\n━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━")
        print("  Menu Content Configuration")
        print("  Provide a label for each menu item.")
        print("  Press [Enter] to use the default index number.")
        print("━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━")
        for m in menus:
            if m["label"] in menu_entries:
                continue
            print(f"\n  Configuring '{m['label']}':")
            max_allowed = int(m.get("max", 0.0) - m.get("min", 0.0) + 1) if m.get("max", 0) > 0 else 0
            if max_allowed <= 0:
                print(f"    ⚠ Cannot determine number of items for {m['label']}, skipping.")
                continue
            
            items = []
            for i in range(1, max_allowed + 1):
                val = input(f"    {i} - ").strip()
                if not val:
                    val = str(i)
                items.append(val)
            menu_entries[m["label"]] = items

    cache_data = {"version": 2, "panel_hp": panel_hp, "positions": overrides, "menus": menu_entries}
    try:
        cache_path.write_text(json.dumps(cache_data, indent=2) + "\n", encoding="utf-8")
        print(f"\n  ✓ Saved {len(overrides)} override(s) and {len(menu_entries)} menu config(s) to {cache_path.name}\n")
    except OSError as e:
        print(f"\n  ⚠ WARNING: Could not save layout cache to {cache_path.name}: {e}\n")


def main():
    parser = argparse.ArgumentParser()
    parser.add_argument("--rnbo-dir", default=None)
    parser.add_argument("--module-name", default=None)
    parser.add_argument("--plugin-slug", default=None)
    parser.add_argument("--manufacturer", default=None)
    parser.add_argument("--author", default=None)
    parser.add_argument("--version", default=None)
    parser.add_argument("--license", default=None)
    parser.add_argument("--block-size", type=int, default=BLOCK_SIZE)
    parser.add_argument("--ui-text", default=UI_TEXT, choices=["yes", "no", "y", "n"])
    parser.add_argument("--polyphony", default="no", choices=["yes", "no", "y", "n"])
    parser.add_argument("--custom-layout", default=CUSTOM_LAYOUT, choices=["yes", "no", "y", "n"])
    parser.add_argument("--custom-ports", default=CUSTOM_PORTS, choices=["yes", "no", "y", "n"])
    parser.add_argument("--menu", default=None)
    parser.add_argument("--non-interactive", action="store_true")
    args = parser.parse_args()

    rnbo_dir = args.rnbo_dir
    module_name = args.module_name
    plugin_slug = args.plugin_slug
    manufacturer = args.manufacturer
    author = args.author
    version = args.version
    license_str = args.license

    SCRIPT_DIR = Path(__file__).parent.resolve()

    if not args.non_interactive:
        print("─────────────────────────────────────────────────────────────────────────────")
        print("  rnbo2vcv Configuration")
        print("  Press Enter to accept the default values.")
        print("─────────────────────────────────────────────────────────────────────────────")
        if not rnbo_dir:
            auto_rnbo = auto_detect_rnbo_dir(SCRIPT_DIR, "export")
            print("\n  [RNBO_DIR]: The folder containing your RNBO C++ export (e.g. 'rnbo_export').")
            rnbo_dir = prompt_with_default("  RNBO Export Directory", auto_rnbo)
        if not module_name:
            print("\n  [MODULE_NAME]: CamelCase name of your module (e.g. MySynth). No spaces.")
            module_name = prompt_with_default("  Module Name", MODULE_NAME)
        if not plugin_slug:
            print("\n  [PLUGIN_SLUG]: Unique alphanumeric ID for the plugin (e.g. MySynthPlugin).")
            plugin_slug = prompt_with_default("  Plugin Slug", PLUGIN_SLUG)
        if not manufacturer:
            print("\n  [MANUFACTURER]: Your brand name, shows in the VCV Rack browser.")
            manufacturer = prompt_with_default("  Manufacturer", MANUFACTURER)
        if not author:
            print("\n  [AUTHOR]: Your name, added to plugin.json.")
            author = prompt_with_default("  Author", AUTHOR)
        if not version:
            print("\n  [VERSION]: Plugin version. Must start with '2.' for VCV Rack 2.")
            version = prompt_with_default("  Version", VERSION)
        if not license_str:
            print("\n  [LICENSE]: Software license (e.g. GPL-3.0, MIT, etc).")
            license_str = prompt_with_default("  License", LICENSE)
        if args.block_size == BLOCK_SIZE:
            print("\n  [BLOCK_SIZE]: DSP processing block size.")
            print("  - 64 is the recommended default for stable CPU.")
            print("  - 1 provides minimal latency (1 sample), great for feedback loops, but uses much more CPU.")
            while True:
                val = prompt_with_default("  Block Size", str(BLOCK_SIZE))
                try:
                    args.block_size = int(val)
                    break
                except ValueError:
                    print(f"    ⚠ Block size must be an integer. Got '{val}'. Try again.")
        if args.ui_text == UI_TEXT:
            print("\n  [UI_TEXT]: Generate C++ text labels for your panel? (yes / no)")
            print("  - 'yes' automatically draws the module name and port labels in C++.")
            print("  - 'no' keeps the panel blank so you can bake custom text into your SVG later.")
            args.ui_text = prompt_with_default("  UI Text", UI_TEXT)
        if args.polyphony == "no":
            print("\n  [POLYPHONY]: Enable 16-voice polyphony? (yes / no)")
            print("  - 'yes' supports up to 16 voices via polyphonic cables. Treats all I/O as polyphonic.")
            print("  - 'no' generates standard monophonic/stereo code (uses less CPU).")
            args.polyphony = prompt_with_default("  Polyphony", "no")
        if args.custom_layout == CUSTOM_LAYOUT:
            print("\n  [CUSTOM_LAYOUT]: Enable interactive component placement? (yes / no)")
            print("  - 'yes' lets you manually position each knob, jack, and button.")
            print("  - 'no' uses the automatic layout engine (recommended for most users).")
            args.custom_layout = prompt_with_default("  Custom Layout", CUSTOM_LAYOUT)
        if args.custom_ports == CUSTOM_PORTS:
            print("\n  [CUSTOM_PORTS]: Customize jack types (audio/cv/stereo)? (yes / no)")
            print("  - 'yes' lets you tag inputs/outputs for correct CV/Audio/Stereo normalization.")
            print("  - 'no' uses generic port types for everything.")
            args.custom_ports = prompt_with_default("  Custom Ports", CUSTOM_PORTS)
        print("─────────────────────────────────────────────────────────────────────────────\n")
    else:
        module_name = module_name or MODULE_NAME
        plugin_slug = plugin_slug or PLUGIN_SLUG
        manufacturer = manufacturer or MANUFACTURER
        author = author or AUTHOR
        version = version or VERSION
        license_str = license_str or LICENSE

    module_name = module_name.strip()
    if not re.fullmatch(r'[A-Za-z_][A-Za-z0-9_]{0,63}', module_name):
        sys.exit(f"ERROR: Module name '{module_name}' must be a valid C++ identifier.")
        
    plugin_slug = plugin_slug.strip()
    if not re.fullmatch(r'[A-Za-z0-9_\-]{1,64}', plugin_slug):
        sys.exit(f"ERROR: Plugin slug '{plugin_slug}' must be alphanumeric/underscore/hyphen, max 64 chars.")
        
    version = version.strip()
    if not version.startswith("2."):
        sys.exit(f"ERROR: Version '{version}' must start with '2.' for VCV Rack 2.")

    if not rnbo_dir:
        rnbo_dir = auto_detect_rnbo_dir(SCRIPT_DIR, "export")

    RNBO_DIR = Path(rnbo_dir).resolve()
    if not RNBO_DIR.exists():
        sys.exit(
            f"ERROR: RNBO export directory '{RNBO_DIR}' does not exist.\n"
            f"       Did you export from RNBO? Or pass --rnbo-dir <path> explicitly."
        )

    OUT_DIR = SCRIPT_DIR / "rack_plugin"
    
    # This path must match the C++ source directory expected by the generated Makefile template in rnbo2vcv/writer.py
    RNBO_SRC = "rnbo_source"

    RACK_DIR = SCRIPT_DIR / "Rack-SDK"
    os.environ["RACK_DIR"] = str(RACK_DIR)

    system = platform.system()
    machine = platform.machine().lower()

    if system == "Linux":
        rack_platform = "lin-arm64" if "aarch64" in machine or "arm" in machine else "lin-x64"
        install_base = Path.home() / ".local" / "share" / "Rack2" / f"plugins-{rack_platform}"
    elif system == "Darwin":
        rack_platform = "mac-arm64" if "arm64" in machine else "mac-x64"
        install_base = Path.home() / "Library" / "Application Support" / "Rack2" / f"plugins-{rack_platform}"
    elif system == "Windows" or os.environ.get("MSYSTEM", "").startswith("MINGW"):
        rack_platform = "win-x64"
        install_base = Path(os.environ.get("LOCALAPPDATA", Path.home() / "AppData" / "Local")) / "Rack2" / f"plugins-{rack_platform}"
    else:
        rack_platform = "lin-x64"
        install_base = Path.home() / ".local" / "share" / "Rack2" / f"plugins-{rack_platform}"

    install_dir = install_base / plugin_slug
    
    if install_dir.resolve() == install_base.resolve() or install_dir.resolve() == install_base.parent.resolve():
        sys.exit("ERROR: Install dir resolved to base directory")
        
    if OUT_DIR.resolve() == SCRIPT_DIR.resolve() or OUT_DIR.resolve() == Path("/").resolve():
        sys.exit("ERROR: OUT_DIR resolved dangerously")
        
    if not str(OUT_DIR.resolve()).startswith(str(SCRIPT_DIR.resolve())):
        sys.exit("ERROR: OUT_DIR is outside project directory")

    print("━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━")
    print("  rnbo2vcv  —  generating plugin files")
    print("━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━")

    env = os.environ.copy()
    env["PYTHONPATH"] = str(SCRIPT_DIR)
    env["PYTHONUTF8"] = "1"
    
    if OUT_DIR.exists(): shutil.rmtree(OUT_DIR)
    OUT_DIR.mkdir(parents=True, exist_ok=True)

    generator_cmd = [
        sys.executable, str(SCRIPT_DIR / "rnbo2vcv" / "writer.py"),
        "--rnbo-dir", str(RNBO_DIR),
        "--module-name", module_name,
        "--plugin-slug", plugin_slug,
        "--out-dir", str(OUT_DIR),
        "--rnbo-src", RNBO_SRC,
        "--manufacturer", manufacturer,
        "--author", author,
        "--version", version,
        "--license", license_str,
        "--ui-text", args.ui_text,
        "--polyphony", args.polyphony,
        "--block-size", str(args.block_size)
    ]
    
    res_dir = SCRIPT_DIR / "res"
    if res_dir.is_dir():
        generator_cmd.extend(["--res-dir", str(res_dir)])

    layout_cache_path = SCRIPT_DIR / ".rnbo2vcv_layout.json"
    use_custom_layout = args.custom_layout.lower() in ("yes", "y")
    use_custom_ports  = args.custom_ports.lower() in ("yes", "y")

    if use_custom_layout or use_custom_ports:
        cache_valid = False
        if layout_cache_path.exists():
            try:
                cached = json.loads(layout_cache_path.read_text(encoding="utf-8"))
                if cached.get("version", 1) >= 2:
                    cache_valid = True
                else:
                    print("  ⚠ Layout cache is outdated (v1). It will be regenerated.")
            except (json.JSONDecodeError, OSError):
                print("  ⚠ Layout cache is corrupted. It will be regenerated.")

        if cache_valid and not args.non_interactive:
            reuse = prompt_with_default("\n  Found saved layout. Use saved layout/types?", "yes")
            if reuse.lower() not in ("yes", "y"):
                _interactive_layout(generator_cmd, layout_cache_path, env, prompt_positions=use_custom_layout, prompt_ports=use_custom_ports, menu_arg=args.menu)
        elif not cache_valid and not args.non_interactive:
            _interactive_layout(generator_cmd, layout_cache_path, env, prompt_positions=use_custom_layout, prompt_ports=use_custom_ports, menu_arg=args.menu)
        elif not cache_valid and args.non_interactive:
            sys.exit("ERROR: Stale or invalid layout cache in non-interactive mode. Aborting.")
        
        if layout_cache_path.exists():
            generator_cmd.extend(["--layout-file", str(layout_cache_path)])

    try:
        subprocess.run(generator_cmd, check=True, env=env)
    except subprocess.CalledProcessError as e:
        sys.exit(f"ERROR: Generator failed (exit {e.returncode}). Check the output above.")
    except FileNotFoundError:
        sys.exit(f"ERROR: Could not find writer.py at {SCRIPT_DIR / 'rnbo2vcv' / 'writer.py'}")

    print(f"\n  Copying RNBO C++ sources → {OUT_DIR.name}/{RNBO_SRC}/")
    rnbo_dest = OUT_DIR / RNBO_SRC
    rnbo_dest.mkdir(parents=True, exist_ok=True)

    for item in RNBO_DIR.iterdir():
        if item.is_symlink(): continue
        if item.name == "rnbo" and item.is_dir():
            shutil.copytree(item, rnbo_dest / "rnbo", dirs_exist_ok=True)
        elif item.suffix == ".cpp" or item.suffix == ".h" or item.name == "description.json":
            shutil.copy2(item, rnbo_dest)

    print("\n━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━")
    print(f"  make  (RACK_DIR={RACK_DIR})")
    print("━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━")
    if not RACK_DIR.is_dir():
        sys.exit(f"ERROR: Rack-SDK not found at '{RACK_DIR}'.\nDownload the correct SDK for your OS from vcvrack.com/downloads and place it as 'Rack-SDK/'.")
        
    try:
        subprocess.run(["make", "-C", str(OUT_DIR), f"RACK_DIR={RACK_DIR}"], check=True)
    except subprocess.CalledProcessError as e:
        sys.exit(f"ERROR: Compilation failed (exit {e.returncode}). See make output above.")
    except FileNotFoundError:
        sys.exit("ERROR: 'make' not found. Install build tools (Linux: apt install build-essential / Windows: see MSYS2 setup in README).")

    print("\n━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━")
    print(f"  Installing → {install_dir}")
    print("━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━")
    
    if install_dir.exists(): shutil.rmtree(install_dir)
    (install_dir / "res").mkdir(parents=True, exist_ok=True)

    for pf in ["plugin.so", "plugin.dylib", "plugin.dll"]:
        src_file = OUT_DIR / pf
        if src_file.exists(): shutil.copy2(src_file, install_dir)
            
    json_src = OUT_DIR / "plugin.json"
    if json_src.exists(): shutil.copy2(json_src, install_dir)
    
    installed_bins = [pf for pf in ["plugin.so", "plugin.dylib", "plugin.dll"] if (install_dir / pf).exists()]
    if not installed_bins: sys.exit("ERROR: No plugin binary found after build.")
    if not (install_dir / "plugin.json").exists(): sys.exit("ERROR: plugin.json missing after build.")

    source_svgs = list((OUT_DIR / "res").glob("*.svg"))
    for res_file in source_svgs:
        shutil.copy2(res_file, install_dir / "res")

    installed_svgs = list((install_dir / "res").glob("*.svg"))
    if len(installed_svgs) != len(source_svgs):
        print(f"  ⚠ WARNING: SVG count mismatch. Source: {len(source_svgs)}, Installed: {len(installed_svgs)}")

    print("\n  ✓ Done. Restart VCV Rack 2 to load plugin.\n")

if __name__ == "__main__":
    main()
