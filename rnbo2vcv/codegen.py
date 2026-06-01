from typing import List, Optional, Dict, Tuple
from pathlib import Path

from rnbo2vcv.models import ComponentPos, PatchInfo, RnboParam, CustomWidgets, _UI_PREFIXES
from rnbo2vcv.utils import _enum_body, _param_label

HP_MM = 5.08
PANEL_H_MM = 128.5

def _resolve_widget(c: ComponentPos, custom_widgets: Optional[CustomWidgets] = None) -> str:
    if custom_widgets is None:
        if c.kind == "param":
            kt = c.knob_type or "RoundBlackKnob"
            return f"rack::{'VCVButton' if kt == 'VCVTrigger' else kt}"
        return "rack::PJ301MPort"

    if c.kind == "param":
        kt = c.knob_type or "RoundBlackKnob"
        widget_map = {
            "RoundHugeBlackKnob":  ("knob_large",   "CustomLargeKnob"),
            "RoundSmallBlackKnob": ("knob_small",   "CustomSmallKnob"),
            "Trimpot":             ("knob_trim",     "CustomTrimKnob"),
            "RoundBlackKnob":      ("knob_default",  "CustomDefaultKnob"),
            "VCVButton":           ("button",        "CustomButton"),
            "VCVTrigger":          ("trigger",       "CustomTrigger"),
            "CKSS":                ("_switch",       "CustomSwitch"),
        }
        if kt in widget_map:
            attr, custom_cls = widget_map[kt]
            if attr == "_switch":
                if custom_widgets.switch_on and custom_widgets.switch_off:
                    return custom_cls
            elif getattr(custom_widgets, attr, False):
                return custom_cls
        rack_kt = "VCVButton" if kt == "VCVTrigger" else kt
        return f"rack::{rack_kt}"

    elif c.kind == "input":
        if c.port_type in ("cvi",):
            if custom_widgets.port_cv_in: return "CustomCvInputPort"
        elif c.port_type in ("audioi", "inl", "inr"):
            if custom_widgets.port_audio_in: return "CustomAudioInputPort"
        if custom_widgets.port_in: return "CustomInputPort"
        return "rack::PJ301MPort"

    elif c.kind == "output":
        if c.port_type in ("cvo",):
            if custom_widgets.port_cv_out: return "CustomCvOutputPort"
        elif c.port_type in ("audioo", "outl", "outr"):
            if custom_widgets.port_audio_out: return "CustomAudioOutputPort"
        if custom_widgets.port_out: return "CustomOutputPort"
        return "rack::PJ301MPort"

    return "rack::PJ301MPort"


def gen_custom_widget_structs(custom_widgets: Optional[CustomWidgets] = None) -> str:
    if custom_widgets is None:
        return ""
    structs: List[str] = []

    if custom_widgets.knob_large: structs.append('struct CustomLargeKnob : rack::app::SvgKnob { CustomLargeKnob() { minAngle = -0.83 * M_PI; maxAngle = 0.83 * M_PI; setSvg(rack::Svg::load(rack::asset::plugin(pluginInstance, "res/knob_large.svg"))); } };')
    if custom_widgets.knob_small: structs.append('struct CustomSmallKnob : rack::app::SvgKnob { CustomSmallKnob() { minAngle = -0.83 * M_PI; maxAngle = 0.83 * M_PI; setSvg(rack::Svg::load(rack::asset::plugin(pluginInstance, "res/knob_small.svg"))); } };')
    if custom_widgets.knob_trim: structs.append('struct CustomTrimKnob : rack::app::SvgKnob { CustomTrimKnob() { minAngle = -0.83 * M_PI; maxAngle = 0.83 * M_PI; setSvg(rack::Svg::load(rack::asset::plugin(pluginInstance, "res/knob_trim.svg"))); } };')
    if custom_widgets.knob_default: structs.append('struct CustomDefaultKnob : rack::app::SvgKnob { CustomDefaultKnob() { minAngle = -0.83 * M_PI; maxAngle = 0.83 * M_PI; setSvg(rack::Svg::load(rack::asset::plugin(pluginInstance, "res/knob_default.svg"))); } };')

    if custom_widgets.button: structs.append('struct CustomButton : rack::app::SvgSwitch { CustomButton() { momentary = true; addFrame(rack::Svg::load(rack::asset::plugin(pluginInstance, "res/button.svg"))); addFrame(rack::Svg::load(rack::asset::plugin(pluginInstance, "res/button_pressed.svg"))); } };')
    if custom_widgets.trigger: structs.append('struct CustomTrigger : rack::app::SvgSwitch { CustomTrigger() { momentary = true; addFrame(rack::Svg::load(rack::asset::plugin(pluginInstance, "res/trigger.svg"))); addFrame(rack::Svg::load(rack::asset::plugin(pluginInstance, "res/trigger_pressed.svg"))); } };')
    if custom_widgets.switch_on and custom_widgets.switch_off: structs.append('struct CustomSwitch : rack::app::SvgSwitch { CustomSwitch() { addFrame(rack::Svg::load(rack::asset::plugin(pluginInstance, "res/switch_off.svg"))); addFrame(rack::Svg::load(rack::asset::plugin(pluginInstance, "res/switch_on.svg"))); } };')

    if custom_widgets.port_cv_in: structs.append('struct CustomCvInputPort : rack::app::SvgPort { CustomCvInputPort() { setSvg(rack::Svg::load(rack::asset::plugin(pluginInstance, "res/port_cv_in.svg"))); } };')
    if custom_widgets.port_cv_out: structs.append('struct CustomCvOutputPort : rack::app::SvgPort { CustomCvOutputPort() { setSvg(rack::Svg::load(rack::asset::plugin(pluginInstance, "res/port_cv_out.svg"))); } };')
    if custom_widgets.port_audio_in: structs.append('struct CustomAudioInputPort : rack::app::SvgPort { CustomAudioInputPort() { setSvg(rack::Svg::load(rack::asset::plugin(pluginInstance, "res/port_audio_in.svg"))); } };')
    if custom_widgets.port_audio_out: structs.append('struct CustomAudioOutputPort : rack::app::SvgPort { CustomAudioOutputPort() { setSvg(rack::Svg::load(rack::asset::plugin(pluginInstance, "res/port_audio_out.svg"))); } };')
    if custom_widgets.port_in: structs.append('struct CustomInputPort : rack::app::SvgPort { CustomInputPort() { setSvg(rack::Svg::load(rack::asset::plugin(pluginInstance, "res/port_in.svg"))); } };')
    if custom_widgets.port_out: structs.append('struct CustomOutputPort : rack::app::SvgPort { CustomOutputPort() { setSvg(rack::Svg::load(rack::asset::plugin(pluginInstance, "res/port_out.svg"))); } };')

    if not structs: return ""
    return "\n// ── Custom SVG Widgets ────────────────────────────────────────────────────\n\n" + "\n\n".join(structs) + "\n"


def _addchild_lines(components: List[ComponentPos], mn: str, panel_hp: int, ui_text: str = "yes", custom_widgets: Optional[CustomWidgets] = None) -> str:
    lines = []
    for c in components:
        pos = f"rack::mm2px(rack::Vec({c.x:.2f}f, {c.y:.2f}f))"
        mod = f"{mn}Module"
        widget_cls = _resolve_widget(c, custom_widgets)
        if c.kind == "param": lines.append(f"        addParam(rack::createParamCentered<{widget_cls}>({pos}, module, {mod}::{c.label}_ID));")
        elif c.kind == "input": lines.append(f"        addInput(rack::createInputCentered<{widget_cls}>({pos}, module, {mod}::{c.label}_ID));")
        elif c.kind == "output": lines.append(f"        addOutput(rack::createOutputCentered<{widget_cls}>({pos}, module, {mod}::{c.label}_ID));")
            
        if ui_text.lower() in ("yes", "y", "true", "1"):
            kt = c.knob_type or ""
            if kt in ("VCVButton", "VCVTrigger"): label_y = c.y + 3.5 + 2.2
            elif kt == "CKSS": label_y = c.y + 4.0 + 2.2
            elif c.kind == "param": label_y = c.y + (11.0 if kt == _UI_PREFIXES["base"] else 3.0) + 2.0
            else: label_y = c.y + 4.5 + 2.2
            ui_lbl = c.ui_label or c.label.replace("_", " ")
            if ui_lbl.startswith("ATTENV "): ui_lbl = ui_lbl[7:]
            elif ui_lbl.startswith("ATTEN "): ui_lbl = ui_lbl[6:]
            elif ui_lbl.endswith(" ATTENV"): ui_lbl = ui_lbl[:-7]
            elif ui_lbl.endswith(" ATTEN"): ui_lbl = ui_lbl[:-6]
            font_sz = "2.4f" if c.kind == "param" else "2.0f"
            pos_lbl = f"rack::mm2px(rack::Vec({c.x:.2f}f, {label_y:.2f}f))"
            lines.append(f'        addChild(new TextLabel({pos_lbl}, "{ui_lbl}", {font_sz}));')

    if ui_text.lower() in ("yes", "y", "true", "1"):
        w = panel_hp * 5.08
        title_pos = f"rack::mm2px(rack::Vec({w/2:.2f}f, 4.8f))"
        lines.append(f'        addChild(new TextLabel({title_pos}, "{mn}", 3.2f));')

    return "\n".join(lines)


def gen_plugin_hpp(module_name: str) -> str:
    return (
        "#pragma once\n"
        "#include <rack.hpp>\n\n"
        "extern rack::Plugin *pluginInstance;\n\n"
        f"extern rack::Model *model{module_name};\n"
    )

def gen_plugin_cpp(plugin_slug: str, plugin_name: str, module_name: str) -> str:
    return (
        '#include "plugin.hpp"\n\n'
        "rack::Plugin *pluginInstance;\n\n"
        "void init(rack::Plugin *p) {\n"
        "    pluginInstance = p;\n"
        f'    p->addModel(model{module_name});\n'
        "}\n"
    )

def gen_module_cpp(info: PatchInfo, panel_hp: int,
                   components: List[ComponentPos],
                   block_size: int,
                   ui_text: str = "yes",
                   polyphony: bool = False,
                   custom_widgets: Optional[CustomWidgets] = None) -> str:
    mn = info.name
    n_ai = info.num_input_channels
    n_ao = info.num_output_channels
    in_p = info.params
    n_knobs = len(in_p)

    param_comps = [c for c in components if c.kind == "param"]
    audio_in_comps = sorted([c for c in components if c.kind == "input"], key=lambda c: c.index)
    output_comps = sorted([c for c in components if c.kind == "output"], key=lambda c: c.index)

    param_enum = _enum_body(param_comps, "NUM_PARAM_IDS")
    input_enum = _enum_body(audio_in_comps, "NUM_INPUT_IDS")
    output_enum = _enum_body(output_comps, "NUM_OUTPUT_IDS")

    param_indices = "\n".join(
        f"    static constexpr RNBO::ParameterIndex PARAM_{_param_label(p)} = {p.index};"
        for p in in_p
    ) or "    // (no parameters)"

    label_to_param = {_param_label(p): p for p in in_p}
    cfg_lines = []
    for c in param_comps:
        p = label_to_param.get(c.label)
        if p is None:
            p = next((x for x in in_p if x.safe_name.upper() == c.label), None)
        if p is None:
            cfg_lines.append(f'        configParam({c.label}_ID, 0.0f, 1.0f, 0.5f, "{c.label}");')
        else:
            cfg_lines.append(f'        configParam({c.label}_ID, {p.minimum}f, {p.maximum}f, {p.default}f, "{p.name}");')

    for c in audio_in_comps: cfg_lines.append(f'        configInput({c.label}_ID, "{c.label}");')
    for c in output_comps: cfg_lines.append(f'        configOutput({c.label}_ID, "{c.label}");')
    config_block = "\n".join(cfg_lines)

    if polyphony:
        rnbo_decl = "    RNBO::CoreObject _rnbo[16];"
        inbuf_decl = (f"    alignas(32) RNBO::SampleValue _inBuf[{max(n_ai, 1)}][16][BLOCK] = {{}};\n"
                      f"    RNBO::SampleValue* _inPtrs[{max(n_ai, 1)}];" if n_ai > 0 else "    RNBO::SampleValue** _inPtrs = nullptr;")
        outbuf_decl = (f"    alignas(32) RNBO::SampleValue _outBuf[{max(n_ao, 1)}][16][BLOCK] = {{}};\n"
                       f"    RNBO::SampleValue* _outPtrs[{max(n_ao, 1)}];" if n_ao > 0 else "    RNBO::SampleValue** _outPtrs = nullptr;")
        
        prepare_block = "        for (int v=0; v<16; v++) _rnbo[v].prepareToProcess(APP->engine->getSampleRate(), BLOCK);"
        sr_block = "        for (int v=0; v<16; v++) _rnbo[v].prepareToProcess(e.sampleRate, BLOCK);"

        in_fill = "        int activeVoices = 1;\n"
        in_fill += "        for (int i = 0; i < NUM_INPUT_IDS; i++) {\n"
        in_fill += "            int c = inputs[i].getChannels();\n"
        in_fill += "            if (c > activeVoices) activeVoices = c;\n"
        in_fill += "        }\n"
        in_fill += "        for (int v = 0; v < activeVoices; v++) {\n"
        in_fill_lines = []
        for i, c in enumerate(audio_in_comps[:n_ai]):
            in_fill_lines.append(f"            _inBuf[{i}][v][_blkPos] = (RNBO::SampleValue)rack::math::clamp(inputs[{c.label}_ID].getPolyVoltage(v), -12.0f, 12.0f);")
        in_fill += "\n".join(in_fill_lines)
        in_fill += "\n        }"

        out_read = "\n".join(f"        outputs[{c.label}_ID].setChannels(activeVoices);" for c in output_comps[:n_ao])
        out_read += "\n        for (int v = 0; v < activeVoices; v++) {\n"
        out_read += "\n".join(f"            outputs[{c.label}_ID].setVoltage(rack::math::clamp((float)_outBuf[{i}][v][_blkPos], -12.0f, 12.0f), v);" for i, c in enumerate(output_comps[:n_ao]))
        out_read += "\n        }"

        param_sends_lines = []
        for i, p in enumerate(in_p):
            lbl = _param_label(p)
            is_trigger = getattr(p, "ui_type", "") == "trigger"
            if is_trigger:
                param_sends_lines.append(
                    f"                float val_{i} = rack::math::clamp(params[{lbl}_ID].getValue(), {p.minimum}f, {p.maximum}f);\n"
                    f"                if (val_{i} > 0.0f && _prevParam[{i}] == 0.0f) {{\n"
                    f"                    _prevParam[{i}] = val_{i};\n"
                    f"                    for (int v = 0; v < activeVoices; v++) _rnbo[v].setParameterValue(PARAM_{lbl}, val_{i});\n"
                    f"                }}\n"
                    f"                else if (std::fabs(val_{i} - _prevParam[{i}]) > 1e-6f) {{\n"
                    f"                    _prevParam[{i}] = val_{i};\n"
                    f"                }}\n"
                    f"                else if (activeVoices > _prevActiveVoices) {{\n"
                    f"                    for (int v = _prevActiveVoices; v < activeVoices; v++) _rnbo[v].setParameterValue(PARAM_{lbl}, val_{i});\n"
                    f"                }}"
                )
            else:
                param_sends_lines.append(
                    f"                float val_{i} = rack::math::clamp(params[{lbl}_ID].getValue(), {p.minimum}f, {p.maximum}f);\n"
                    f"                if (std::fabs(val_{i} - _prevParam[{i}]) > 1e-6f) {{\n"
                    f"                    _prevParam[{i}] = val_{i};\n"
                    f"                    for (int v = 0; v < activeVoices; v++) _rnbo[v].setParameterValue(PARAM_{lbl}, val_{i});\n"
                    f"                }}\n"
                    f"                else if (activeVoices > _prevActiveVoices) {{\n"
                    f"                    for (int v = _prevActiveVoices; v < activeVoices; v++) _rnbo[v].setParameterValue(PARAM_{lbl}, val_{i});\n"
                    f"                }}"
                )
        param_sends = "\n".join(param_sends_lines)
        param_sends += "\n                _prevActiveVoices = activeVoices;"

        process_loop = "            for (int v = 0; v < activeVoices; v++) {\n"
        if n_ai > 0: process_loop += "\n".join(f"                _inPtrs[{i}] = _inBuf[{i}][v];" for i in range(n_ai)) + "\n"
        if n_ao > 0: process_loop += "\n".join(f"                _outPtrs[{i}] = _outBuf[{i}][v];" for i in range(n_ao)) + "\n"
        process_loop += f"                _rnbo[v].process(_inPtrs, N_AI, _outPtrs, N_AO, BLOCK);\n"
        process_loop += "            }"

        extra_vars = "    int _prevActiveVoices = 1;\n"

    else:
        rnbo_decl = "    RNBO::CoreObject _rnbo;"
        inbuf_decl = (f"    alignas(32) RNBO::SampleValue _inBuf[{max(n_ai, 1)}][BLOCK] = {{}};\n"
                      f"    RNBO::SampleValue* _inPtrs[{max(n_ai, 1)}];" if n_ai > 0 else "    RNBO::SampleValue** _inPtrs = nullptr;")
        outbuf_decl = (f"    alignas(32) RNBO::SampleValue _outBuf[{max(n_ao, 1)}][BLOCK] = {{}};\n"
                       f"    RNBO::SampleValue* _outPtrs[{max(n_ao, 1)}];" if n_ao > 0 else "    RNBO::SampleValue** _outPtrs = nullptr;")
        
        prepare_block = "        _rnbo.prepareToProcess(APP->engine->getSampleRate(), BLOCK);"
        sr_block = "        _rnbo.prepareToProcess(e.sampleRate, BLOCK);"

        in_fill_lines = []
        for i, c in enumerate(audio_in_comps[:n_ai]):
            if info.stereo_in and i == 1:
                in_fill_lines.append(f"        _inBuf[{i}][_blkPos] = (RNBO::SampleValue)rack::math::clamp(inputs[{c.label}_ID].getNormalVoltage(inputs[{audio_in_comps[0].label}_ID].getVoltage()), -12.0f, 12.0f);")
            else:
                in_fill_lines.append(f"        _inBuf[{i}][_blkPos] = (RNBO::SampleValue)rack::math::clamp(inputs[{c.label}_ID].getVoltage(), -12.0f, 12.0f);")
        in_fill = "\n".join(in_fill_lines)

        out_read = "\n".join(f"        outputs[{c.label}_ID].setVoltage(rack::math::clamp((float)_outBuf[{i}][_blkPos], -12.0f, 12.0f));" for i, c in enumerate(output_comps[:n_ao]))

        param_sends_lines = []
        for i, p in enumerate(in_p):
            lbl = _param_label(p)
            is_trigger = getattr(p, "ui_type", "") == "trigger"
            if is_trigger:
                param_sends_lines.append(
                    f"                float val_{i} = rack::math::clamp(params[{lbl}_ID].getValue(), {p.minimum}f, {p.maximum}f);\n"
                    f"                if (val_{i} > 0.0f && _prevParam[{i}] == 0.0f) {{\n"
                    f"                    _prevParam[{i}] = val_{i};\n"
                    f"                    _rnbo.setParameterValue(PARAM_{lbl}, val_{i});\n"
                    f"                }}\n"
                    f"                else if (std::fabs(val_{i} - _prevParam[{i}]) > 1e-6f) {{\n"
                    f"                    _prevParam[{i}] = val_{i};\n"
                    f"                }}"
                )
            else:
                param_sends_lines.append(
                    f"                float val_{i} = rack::math::clamp(params[{lbl}_ID].getValue(), {p.minimum}f, {p.maximum}f);\n"
                    f"                if (std::fabs(val_{i} - _prevParam[{i}]) > 1e-6f) {{\n"
                    f"                    _prevParam[{i}] = val_{i};\n"
                    f"                    _rnbo.setParameterValue(PARAM_{lbl}, val_{i});\n"
                    f"                }}"
                )
        param_sends = "\n".join(param_sends_lines)

        process_loop = ""
        if n_ai > 0: process_loop += "\n".join(f"            _inPtrs[{i}] = _inBuf[{i}];" for i in range(n_ai)) + "\n"
        if n_ao > 0: process_loop += "\n".join(f"            _outPtrs[{i}] = _outBuf[{i}];" for i in range(n_ao)) + "\n"
        process_loop += f"            _rnbo.process(_inPtrs, N_AI, _outPtrs, N_AO, BLOCK);"

        extra_vars = ""

    return f"""\
#include <cmath>
#include <string>
#include "plugin.hpp"
#include "RNBO.h"

struct {mn}Module : rack::Module {{
#ifndef BLOCK_SIZE
    static constexpr int BLOCK = {block_size};
#else
    static constexpr int BLOCK = BLOCK_SIZE;
#endif

    static constexpr int N_AI = {n_ai};
    static constexpr int N_AO = {n_ao};

{param_indices}

    enum ParamId {{ {param_enum} }};
    enum InputId {{ {input_enum} }};
    enum OutputId {{ {output_enum} }};
    enum LightId {{ NUM_LIGHT_IDS }};

{rnbo_decl}

{inbuf_decl}
{outbuf_decl}

    int _blkPos = 0;
{extra_vars}
    float _prevParam[{max(n_knobs, 1)}] = {{}};

    {mn}Module() {{
        config(NUM_PARAM_IDS, NUM_INPUT_IDS, NUM_OUTPUT_IDS, NUM_LIGHT_IDS);
{config_block}
{prepare_block}
    }}

    void onSampleRateChange(const SampleRateChangeEvent &e) override {{
{sr_block}
    }}

    void process(const ProcessArgs &args) override {{
        // ── 1. Fill input audio buffer ─────────────────────────────────────
{in_fill}

        // ── 2. Read output audio from previous-block buffer ────────────────
{out_read}

        // ── 3. Advance block position; process a full block when ready ──────
        if (++_blkPos >= BLOCK) {{
            _blkPos = 0;
{param_sends}
{process_loop}
        }}
    }}
}};

{gen_custom_widget_structs(custom_widgets)}
struct TextLabel : rack::widget::TransparentWidget {{
    std::string text;
    float fontSizeMm;
    TextLabel(rack::math::Vec pos, std::string text, float fontSizeMm = 2.4f) {{
        this->box.pos = pos;
        this->box.size = rack::mm2px(rack::math::Vec(20.f, 5.f));
        this->text = text;
        this->fontSizeMm = fontSizeMm;
    }}
    void draw(const rack::widget::Widget::DrawArgs& args) override {{
        std::shared_ptr<rack::window::Font> font = APP->window->uiFont;
        if (font) {{
            nvgFontSize(args.vg, rack::mm2px(fontSizeMm));
            nvgFontFaceId(args.vg, font->handle);
            nvgFillColor(args.vg, rack::settings::preferDarkPanels ? nvgRGBA(255, 255, 255, 255) : nvgRGBA(0, 0, 0, 255));
            nvgTextAlign(args.vg, NVG_ALIGN_CENTER | NVG_ALIGN_TOP);
            nvgText(args.vg, 0, 0, text.c_str(), nullptr);
        }}
    }}
}};

struct {mn}Widget : rack::ModuleWidget {{
    explicit {mn}Widget({mn}Module *module) {{
        setModule(module);
        box.size = rack::Vec({panel_hp} * rack::app::RACK_GRID_WIDTH, rack::app::RACK_GRID_HEIGHT);
        setPanel(rack::createPanel(
            rack::asset::plugin(pluginInstance, "res/{mn}.svg"),
            rack::asset::plugin(pluginInstance, "res/{mn}-dark.svg")
        ));
        addChild(rack::createWidget<rack::ScrewSilver>(rack::Vec(rack::app::RACK_GRID_WIDTH, 0)));
        addChild(rack::createWidget<rack::ScrewSilver>(rack::Vec(box.size.x - 2 * rack::app::RACK_GRID_WIDTH, 0)));
        addChild(rack::createWidget<rack::ScrewSilver>(rack::Vec(rack::app::RACK_GRID_WIDTH, rack::app::RACK_GRID_HEIGHT - rack::app::RACK_GRID_WIDTH)));
        addChild(rack::createWidget<rack::ScrewSilver>(rack::Vec(box.size.x - 2 * rack::app::RACK_GRID_WIDTH, rack::app::RACK_GRID_HEIGHT - rack::app::RACK_GRID_WIDTH)));

{_addchild_lines(components, mn, panel_hp, ui_text, custom_widgets)}
    }}
}};

rack::Model *model{mn} = rack::createModel<{mn}Module, {mn}Widget>("{mn}");
"""


def gen_panel_svg(module_name: str, panel_hp: int,
                  bg_color: str = "#E8E8E8", fg_color: str = "#000000") -> str:
    W = panel_hp * HP_MM
    H = PANEL_H_MM
    return (
        f'<?xml version="1.0" encoding="UTF-8"?>\n'
        f'<svg xmlns="http://www.w3.org/2000/svg"\n'
        f'     width="{W:.3f}mm" height="{H:.3f}mm"\n'
        f'     viewBox="0 0 {W:.3f} {H:.3f}">\n'
        f'  <rect width="{W:.3f}" height="{H:.3f}" fill="{bg_color}"/>\n'
        f'  <rect x="0" y="0" width="{W:.3f}" height="0.6" fill="{fg_color}"/>\n'
        f'  <rect x="0" y="{H-0.6:.3f}" width="{W:.3f}" height="0.6" fill="{fg_color}"/>\n'
        f'</svg>\n'
    )


def generate_all(info: PatchInfo, panel_hp: int, components: List[ComponentPos],
                 block_size: int, ui_text: str, polyphony: bool, rnbo_src_rel: str,
                 rnbo_dir: Path, custom_widgets: Optional[CustomWidgets] = None) -> Dict[str, str]:
    files = {}
    files["plugin.hpp"] = gen_plugin_hpp(info.name)
    files["plugin.cpp"] = gen_plugin_cpp("slug", "name", info.name)
    files[f"{info.name}.cpp"] = gen_module_cpp(info, panel_hp, components, block_size, ui_text, polyphony, custom_widgets)
    
    if not (custom_widgets and custom_widgets.panel):
        files[f"res/{info.name}.svg"] = gen_panel_svg(info.name, panel_hp, bg_color="#E8E8E8", fg_color="#000000")
        files[f"res/{info.name}-dark.svg"] = gen_panel_svg(info.name, panel_hp, bg_color="#1A1A1A", fg_color="#CCCCCC")

    
    makefile = (
        f"FLAGS += -I{rnbo_src_rel}/rnbo\n"
        f"FLAGS += -I{rnbo_src_rel}/rnbo/common\n"
        f"FLAGS += -DRNBO_USE_FLOAT32\n"
        f"SOURCES += plugin.cpp {info.name}.cpp\n"
        f"SOURCES += $(wildcard {rnbo_src_rel}/*.cpp)\n"
        f"SOURCES += $(wildcard {rnbo_src_rel}/rnbo/src/*.cpp)\n"
        f"SOURCES += $(wildcard {rnbo_src_rel}/rnbo/common/*.cpp)\n"
        f"DISTRIBUTABLES += $(wildcard LICENSE*)\n"
        f"include $(RACK_DIR)/plugin.mk\n"
    )
    files["Makefile"] = makefile
    return files
