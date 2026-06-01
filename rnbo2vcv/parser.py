import re
import sys
import json
from pathlib import Path
from typing import List, Tuple, Optional

from rnbo2vcv.models import RnboParam, PatchInfo, _UI_PREFIXES
from rnbo2vcv.utils import sanitize_identifier, deduplicate_safe_names

# ─────────────────────────────────────────────────────────────────────────────
# Regex Constants
# ─────────────────────────────────────────────────────────────────────────────

# Suffix pattern: _in<N>  or  _out<N>
_IN_SUFFIX_RE = re.compile(r"_in(\d+)$", re.IGNORECASE)
_OUT_SUFFIX_RE = re.compile(r"_out(\d+)$", re.IGNORECASE)

# ─────────────────────────────────────────────────────────────────────────────
# Smart Naming Parsing
# ─────────────────────────────────────────────────────────────────────────────

def parse_smart_name(param: RnboParam) -> None:
    name = param.name
    matched_prefix = ""
    for pfx in _UI_PREFIXES:
        if name.startswith(pfx + "_"):
            matched_prefix = pfx
            break
    if not matched_prefix:
        return

    remainder = name[len(matched_prefix) + 1:]
    
    # Check for _inN suffix
    in_match = _IN_SUFFIX_RE.search(remainder)
    adc_num = 0
    if in_match:
        adc_num = int(in_match.group(1))
        remainder = remainder[:in_match.start()]

    # Check for _outN suffix
    out_match = _OUT_SUFFIX_RE.search(remainder)
    if out_match:
        remainder = remainder[:out_match.start()]

    core = remainder
    if not core:
        print(f"[smart] WARNING: '{param.name}' has empty core name after prefix strip — treating as plain param")
        return

    param.ui_type   = matched_prefix
    param.core_name = core
    param.adc_map   = adc_num
    param.enum_label = sanitize_identifier(core).upper()
    
    print(f"[smart] {param.name} -> {param.ui_type} '{param.core_name}'\n"
          f"  adc_map={adc_num} enum={param.enum_label}")


def apply_smart_names(params: List[RnboParam]) -> bool:
    for p in params:
        parse_smart_name(p)
    return any(p.ui_type for p in params)


# ─────────────────────────────────────────────────────────────────────────────
# RNBO Parsing
# ─────────────────────────────────────────────────────────────────────────────

def find_rnbo_json(rnbo_dir: Path) -> Optional[Path]:
    for name in ("description.json", "rnbo_source.json"):
        p = rnbo_dir / name
        if p.exists():
            return p
    for p in rnbo_dir.rglob("*.json"):
        try:
            data = json.loads(p.read_text())
            if "rnboobjname" in data.get("meta", {}):
                return p
        except (json.JSONDecodeError, OSError):
            pass
    return None

def parse_from_json(json_path: Path) -> Tuple[List[RnboParam], int, int]:
    data = json.loads(json_path.read_text(encoding="utf-8"))
    params: List[RnboParam] = []
    
    for p in data.get("parameters", []):
        params.append(RnboParam(
            name=p["name"],
            safe_name=sanitize_identifier(p["name"]),
            index=p["index"],
            minimum=float(p.get("minimum", 0.0)),
            maximum=float(p.get("maximum", 1.0)),
            default=float(p.get("initialValue", 0.5)),
        ))
        
    n_in = int(data.get("numInputChannels", 0))
    n_out = int(data.get("numOutputChannels", 0))
    inlets = data.get("inlets", [])
    outlets = data.get("outlets", [])
    
    return params, n_in, n_out, inlets, outlets

def gather_patch_info(rnbo_dir: Path, module_name: str) -> PatchInfo:
    json_path = find_rnbo_json(rnbo_dir)
    if not json_path:
        sys.exit(
            f"ERROR: No description.json found under {rnbo_dir}\n"
            f"       Export your RNBO patch with C++ Library checked."
        )

    print(f"[parse] JSON metadata  : {json_path.relative_to(rnbo_dir)}")
    params, n_in, n_out, inlets, outlets = parse_from_json(json_path)

    deduplicate_safe_names(params)

    print(f"[parse] Patch name  : {module_name}")
    print(f"[parse] Audio I/O   : {n_in} in / {n_out} out")
    print(f"[parse] Parameters  : {len(params)}")

    return PatchInfo(
        name=module_name,
        num_input_channels=n_in,
        num_output_channels=n_out,
        params=params,
        inlets=inlets,
        outlets=outlets,
        stereo_in=False,
        stereo_out=False,
    )
