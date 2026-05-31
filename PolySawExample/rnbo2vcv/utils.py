import re
from typing import List
from rnbo2vcv.models import RnboParam, ComponentPos, _CPP_RESERVED

def sanitize_identifier(name: str) -> str:
    s = re.sub(r"[^A-Za-z0-9_]", "_", name)
    s = re.sub(r"_+", "_", s).strip("_") or "param"
    if s[0].isdigit():
        s = "p_" + s
    if s in _CPP_RESERVED:
        s = s + "_"
    return s

def deduplicate_safe_names(params: List[RnboParam]) -> None:
    seen: dict[str, int] = {}
    for p in params:
        base = p.safe_name
        if base in seen:
            seen[base] += 1
            p.safe_name = f"{base}_{seen[base]}"
        else:
            seen[base] = 0

def _param_label(p: RnboParam) -> str:
    return p.enum_label if p.enum_label else p.safe_name.upper()

def _enum_body(items: List[ComponentPos], terminal: str) -> str:
    lines = [f"        {c.label}_ID," for c in items]
    lines.append(f"        {terminal}")
    return "\n".join(lines)
