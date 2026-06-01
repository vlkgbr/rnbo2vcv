import argparse
import json
import shutil
import sys
from pathlib import Path
from typing import Optional

from rnbo2vcv.models import CustomWidgets
from rnbo2vcv.parser import gather_patch_info, apply_smart_names
from rnbo2vcv.layout import run_layout, detect_svg_panel_hp, apply_layout_overrides, HP_MM
from rnbo2vcv.codegen import generate_all

def main() -> None:
    ap = argparse.ArgumentParser(
        description="Generate VCV Rack 2 C++ wrapper from an RNBO exported patch",
        formatter_class=argparse.RawDescriptionHelpFormatter,
    )
    ap.add_argument("--rnbo-dir",    required=True,
                    help="Path to the RNBO output directory (contains description.json and rnbo/) ")
    ap.add_argument("--module-name", required=True,
                    help="CamelCase module name  e.g. MyPatch")
    ap.add_argument("--plugin-slug", default="MyPlugin",
                    help="Alphanumeric plugin slug (default: MyPlugin)")
    ap.add_argument("--plugin-name", default=None,
                    help="Human-readable plugin name (defaults to --plugin-slug)")
    ap.add_argument("--out-dir",     default=".",
                    help="Output directory (default: current dir)")
    ap.add_argument("--rnbo-src",    default="rnbo_source",
                    help="Relative path from project root to RNBO C sources (default: rnbo_source)")
    ap.add_argument("--block-size",  type=int, default=64,
                    help="RNBO processing block size in samples (default: 64)")
    ap.add_argument("--manufacturer", default="MyBrand",
                    help="Brand/manufacturer name written into plugin.json (default: MyBrand)")
    ap.add_argument("--version",      default="2.0.0",
                    help="Plugin version string in plugin.json (default: 2.0.0 — must start with 2. for Rack 2)")
    ap.add_argument("--author",       default=None,
                    help="Author name for plugin.json (defaults to --manufacturer)")
    ap.add_argument("--license",      default="GPL-3.0",
                    help="License identifier in plugin.json (default: GPL-3.0)")
    ap.add_argument("--polyphony", default="no", 
                    help="Enable VCV Rack polyphony loop (yes/no)")
    ap.add_argument("--ui-text",     choices=["yes", "no"], default="yes",
                    help="Generate UI text labels in C++ (default: yes)")
    ap.add_argument("--dump-layout-file", default=None,
                    help="Path to save auto-layout JSON. If provided, writes layout to this file and exits.")
    ap.add_argument("--layout-file", default=None,
                    help="Path to a .rnbo2vcv_layout.json file with saved position overrides.")
    ap.add_argument("--res-dir",     default="res",
                    help="Path to the SVG resource folder (default: res/ relative to script).")
    args = ap.parse_args()

    rnbo_dir    = Path(args.rnbo_dir).expanduser().resolve()
    out_dir     = Path(args.out_dir).expanduser().resolve()
    plugin_name = args.plugin_name or args.plugin_slug

    if not rnbo_dir.is_dir():
        sys.exit(f"ERROR: --rnbo-dir does not exist: {rnbo_dir}")

    src_dir = out_dir / "src"
    src_dir.mkdir(parents=True, exist_ok=True)
    (out_dir / "res").mkdir(exist_ok=True)

    info = gather_patch_info(rnbo_dir, args.module_name)

    has_smart = apply_smart_names(info.params)
    use_poly = args.polyphony.lower() in ("yes", "y", "true", "1")
    panel_hp, components = run_layout(info, {}, has_smart)

    print(f"[layout] {panel_hp} HP  ({panel_hp * HP_MM:.1f} mm)  — {len(components)} components")

    res_dir = Path(args.res_dir).expanduser().resolve()
    if not res_dir.is_absolute():
        res_dir = (Path(__file__).parent / args.res_dir).resolve()
    custom_widgets: Optional[CustomWidgets] = None
    if res_dir.is_dir():
        custom_widgets = CustomWidgets.from_dir(res_dir)
        custom_panel_path = res_dir / "panel.svg"
        if custom_widgets.panel and custom_panel_path.exists():
            panel_hp = detect_svg_panel_hp(custom_panel_path)
            print(f"[layout] Panel HP overridden by custom SVG: {panel_hp} HP")
    else:
        print(f"[res]   res/ folder not found at {res_dir} — using Rack built-in widgets")

    if args.layout_file:
        layout_path = Path(args.layout_file).expanduser().resolve()
        if layout_path.exists():
            try:
                layout_data = json.loads(layout_path.read_text())
                components = apply_layout_overrides(components, layout_data)
            except (json.JSONDecodeError, OSError) as e:
                print(f"[layout] WARNING: Could not load layout file: {e}")
        else:
            print(f"[layout] WARNING: --layout-file not found: {layout_path}")

    has_inl  = any(c.port_type == "inl" for c in components)
    has_inr  = any(c.port_type == "inr" for c in components)
    has_outl = any(c.port_type == "outl" for c in components)
    has_outr = any(c.port_type == "outr" for c in components)
    if has_inl or has_inr:
        info.stereo_in = True
        print("[layout] Stereo input detected from jack type overrides")
    if has_outl or has_outr:
        info.stereo_out = True
        print("[layout] Stereo output detected from jack type overrides")

    if args.dump_layout_file:
        layout_dump = {
            "version": 2,
            "panel_hp": panel_hp,
            "components": [
                {
                    "label": c.label,
                    "kind": c.kind,
                    "knob_type": c.knob_type,
                    "x": round(c.x, 2),
                    "y": round(c.y, 2),
                    "index": c.index,
                    "ui_label": c.ui_label,
                    "port_type": c.port_type,
                }
                for c in components
            ],
        }
        dump_path = Path(args.dump_layout_file).resolve()
        dump_path.write_text(json.dumps(layout_dump, indent=2) + "\n", encoding="utf-8")
        print(f"[layout] Dumped layout to {dump_path}")
        return

    if custom_widgets and custom_widgets.panel and res_dir:
        shutil.copy2(res_dir / "panel.svg", out_dir / "res" / f"{args.module_name}.svg")
        print(f"[res]   Copied panel.svg → res/{args.module_name}.svg")
        if custom_widgets.panel_dark:
            shutil.copy2(res_dir / "panel-dark.svg", out_dir / "res" / f"{args.module_name}-dark.svg")
            print(f"[res]   Copied panel-dark.svg → res/{args.module_name}-dark.svg")
        else:
            shutil.copy2(res_dir / "panel.svg", out_dir / "res" / f"{args.module_name}-dark.svg")
            print(f"[res]   No dark panel — using panel.svg for both themes")

    gen_files = generate_all(
        info=info,
        panel_hp=panel_hp,
        components=components,
        block_size=args.block_size,
        ui_text=args.ui_text,
        polyphony=use_poly,
        rnbo_src_rel=args.rnbo_src,
        rnbo_dir=rnbo_dir,
        custom_widgets=custom_widgets
    )
    
    files = {}
    for k, v in gen_files.items():
        files[out_dir / k] = v

    if custom_widgets and res_dir and res_dir.is_dir():
        widget_svgs = [
            "knob_large.svg", "knob_small.svg", "knob_trim.svg", "knob_default.svg",
            "button.svg", "button_pressed.svg",
            "trigger.svg", "trigger_pressed.svg",
            "switch_on.svg", "switch_off.svg",
            "port_cv_in.svg", "port_cv_out.svg",
            "port_audio_in.svg", "port_audio_out.svg",
            "port_in.svg", "port_out.svg",
        ]
        for svg_name in widget_svgs:
            svg_src = res_dir / svg_name
            if svg_src.exists():
                shutil.copy2(svg_src, out_dir / "res" / svg_name)
                print(f"[res]   Copied {svg_name} → res/{svg_name}")

    for path, content in files.items():
        path.write_text(content, encoding="utf-8")
        print(f"[wrote]  {path.relative_to(out_dir)}")

    plugin_json_data = {
        "slug":    args.plugin_slug,
        "name":    plugin_name,
        "version": args.version,
        "author":  args.author or args.manufacturer,
        "license": args.license,
        "brand":   args.manufacturer,
        "modules": [{"slug": args.module_name, "name": args.module_name}],
    }
    plugin_json_path = out_dir / "plugin.json"
    plugin_json_path.write_text(json.dumps(plugin_json_data, indent=2) + "\n", encoding="utf-8")
    print(f"[wrote]  plugin.json")

    print("\n" + "─" * 64)
    print("Next steps")
    print("─" * 64)
    print(f"  1. Copy RNBO C++ export (rnbo_source.cpp, rnbo/ dir)")
    print(f"       → ./{args.rnbo_src}/")
    print(f"  2. export RACK_DIR=/path/to/Rack-SDK && make")
    print("─" * 64)

if __name__ == "__main__":
    main()
