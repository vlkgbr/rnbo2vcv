/*******************************************************************************************************************
Copyright (c) 2023 Cycling '74

The code that Max generates automatically and that end users are capable of
exporting and using, and any associated documentation files (the “Software”)
is a work of authorship for which Cycling '74 is the author and owner for
copyright purposes.

This Software is dual-licensed either under the terms of the Cycling '74
License for Max-Generated Code for Export, or alternatively under the terms
of the General Public License (GPL) Version 3. You may use the Software
according to either of these licenses as it is most appropriate for your
project on a case-by-case basis (proprietary or not).

A) Cycling '74 License for Max-Generated Code for Export

A license is hereby granted, free of charge, to any person obtaining a copy
of the Software (“Licensee”) to use, copy, modify, merge, publish, and
distribute copies of the Software, and to permit persons to whom the Software
is furnished to do so, subject to the following conditions:

The Software is licensed to Licensee for all uses that do not include the sale,
sublicensing, or commercial distribution of software that incorporates this
source code. This means that the Licensee is free to use this software for
educational, research, and prototyping purposes, to create musical or other
creative works with software that incorporates this source code, or any other
use that does not constitute selling software that makes use of this source
code. Commercial distribution also includes the packaging of free software with
other paid software, hardware, or software-provided commercial services.

For entities with UNDER $200k in annual revenue or funding, a license is hereby
granted, free of charge, for the sale, sublicensing, or commercial distribution
of software that incorporates this source code, for as long as the entity's
annual revenue remains below $200k annual revenue or funding.

For entities with OVER $200k in annual revenue or funding interested in the
sale, sublicensing, or commercial distribution of software that incorporates
this source code, please send inquiries to licensing@cycling74.com.

The above copyright notice and this license shall be included in all copies or
substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NON-INFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.

Please see
https://support.cycling74.com/hc/en-us/articles/10730637742483-RNBO-Export-Licensing-FAQ
for additional information

B) General Public License Version 3 (GPLv3)
Details of the GPLv3 license can be found at: https://www.gnu.org/licenses/gpl-3.0.html
*******************************************************************************************************************/

#ifdef RNBO_LIB_PREFIX
#define STR_IMPL(A) #A
#define STR(A) STR_IMPL(A)
#define RNBO_LIB_INCLUDE(X) STR(RNBO_LIB_PREFIX/X)
#else
#define RNBO_LIB_INCLUDE(X) #X
#endif // RNBO_LIB_PREFIX
#ifdef RNBO_INJECTPLATFORM
#define RNBO_USECUSTOMPLATFORM
#include RNBO_INJECTPLATFORM
#endif // RNBO_INJECTPLATFORM

#include RNBO_LIB_INCLUDE(RNBO_Common.h)
#include RNBO_LIB_INCLUDE(RNBO_AudioSignal.h)

namespace RNBO {


#define trunc(x) ((Int)(x))
#define autoref auto&

#if defined(__GNUC__) || defined(__clang__)
    #define RNBO_RESTRICT __restrict__
#elif defined(_MSC_VER)
    #define RNBO_RESTRICT __restrict
#endif

#define FIXEDSIZEARRAYINIT(...) { }

template <class ENGINE = INTERNALENGINE> class rnbomatic : public PatcherInterfaceImpl {

friend class EngineCore;
friend class Engine;
friend class MinimalEngine<>;
public:

rnbomatic()
: _internalEngine(this)
{
}

~rnbomatic()
{
    deallocateSignals();
}

Index getNumMidiInputPorts() const {
    return 0;
}

void processMidiEvent(MillisecondTime , int , ConstByteArray , Index ) {}

Index getNumMidiOutputPorts() const {
    return 0;
}

void process(
    const SampleValue * const* inputs,
    Index numInputs,
    SampleValue * const* outputs,
    Index numOutputs,
    Index n
) {
    this->vs = n;
    this->updateTime(this->getEngine()->getCurrentTime(), (ENGINE*)nullptr, true);
    SampleValue * out1 = (numOutputs >= 1 && outputs[0] ? outputs[0] : this->dummyBuffer);
    const SampleValue * in1 = (numInputs >= 1 && inputs[0] ? inputs[0] : this->zeroBuffer);
    const SampleValue * in2 = (numInputs >= 2 && inputs[1] ? inputs[1] : this->zeroBuffer);
    const SampleValue * in3 = (numInputs >= 3 && inputs[2] ? inputs[2] : this->zeroBuffer);
    this->ip_01_perform(this->signals[0], n);

    this->svf_tilde_01_perform(
        this->signals[0],
        this->svf_tilde_01_freq,
        this->svf_tilde_01_q,
        this->signals[1],
        this->dummyBuffer,
        this->dummyBuffer,
        this->dummyBuffer,
        n
    );

    this->dspexpr_02_perform(in1, this->dspexpr_02_in2, this->signals[0], n);
    this->dspexpr_01_perform(this->signals[1], this->signals[0], this->signals[2], n);
    this->mtof_tilde_01_perform(this->signals[2], this->signals[0], n);

    this->saw_tilde_01_perform(
        this->signals[0],
        this->saw_tilde_01_syncPhase,
        this->signals[2],
        this->dummyBuffer,
        n
    );

    this->ip_02_perform(this->signals[0], n);

    this->svf_tilde_03_perform(
        this->signals[0],
        this->svf_tilde_03_freq,
        this->svf_tilde_03_q,
        this->signals[1],
        this->dummyBuffer,
        this->dummyBuffer,
        this->dummyBuffer,
        n
    );

    this->scale_tilde_02_perform(
        in2,
        this->scale_tilde_02_lowin,
        this->scale_tilde_02_hiin,
        this->scale_tilde_02_lowout,
        this->scale_tilde_02_highout,
        this->scale_tilde_02_pow,
        this->signals[0],
        n
    );

    this->ip_03_perform(this->signals[3], n);

    this->svf_tilde_04_perform(
        this->signals[3],
        this->svf_tilde_04_freq,
        this->svf_tilde_04_q,
        this->signals[4],
        this->dummyBuffer,
        this->dummyBuffer,
        this->dummyBuffer,
        n
    );

    this->dspexpr_04_perform(this->signals[0], this->signals[4], this->signals[3], n);
    this->dspexpr_03_perform(this->signals[1], this->signals[3], this->signals[4], n);
    this->ip_04_perform(this->signals[3], n);

    this->svf_tilde_05_perform(
        this->signals[3],
        this->svf_tilde_05_freq,
        this->svf_tilde_05_q,
        this->signals[1],
        this->dummyBuffer,
        this->dummyBuffer,
        this->dummyBuffer,
        n
    );

    this->scale_tilde_03_perform(
        in3,
        this->scale_tilde_03_lowin,
        this->scale_tilde_03_hiin,
        this->scale_tilde_03_lowout,
        this->scale_tilde_03_highout,
        this->scale_tilde_03_pow,
        this->signals[3],
        n
    );

    this->ip_05_perform(this->signals[0], n);

    this->svf_tilde_06_perform(
        this->signals[0],
        this->svf_tilde_06_freq,
        this->svf_tilde_06_q,
        this->signals[5],
        this->dummyBuffer,
        this->dummyBuffer,
        this->dummyBuffer,
        n
    );

    this->dspexpr_07_perform(this->signals[3], this->signals[5], this->signals[0], n);
    this->dspexpr_06_perform(this->signals[1], this->signals[0], this->signals[5], n);

    this->dspexpr_05_perform(
        this->signals[5],
        this->dspexpr_05_in2,
        this->dspexpr_05_in3,
        this->signals[0],
        n
    );

    this->svf_tilde_02_perform(
        this->signals[2],
        this->signals[4],
        this->signals[0],
        this->signals[5],
        this->dummyBuffer,
        this->dummyBuffer,
        this->dummyBuffer,
        n
    );

    this->scale_tilde_01_perform(
        this->signals[5],
        this->scale_tilde_01_lowin,
        this->scale_tilde_01_hiin,
        this->scale_tilde_01_lowout,
        this->scale_tilde_01_highout,
        this->scale_tilde_01_pow,
        out1,
        n
    );

    this->stackprotect_perform(n);
    this->globaltransport_advance();
    this->advanceTime((ENGINE*)nullptr);
    this->audioProcessSampleCount += this->vs;
}

void prepareToProcess(number sampleRate, Index maxBlockSize, bool force) {
    RNBO_ASSERT(this->_isInitialized);

    if (this->maxvs < maxBlockSize || !this->didAllocateSignals) {
        Index i;

        for (i = 0; i < 6; i++) {
            this->signals[i] = resizeSignal(this->signals[i], this->maxvs, maxBlockSize);
        }

        this->ip_01_sigbuf = resizeSignal(this->ip_01_sigbuf, this->maxvs, maxBlockSize);
        this->ip_02_sigbuf = resizeSignal(this->ip_02_sigbuf, this->maxvs, maxBlockSize);
        this->ip_03_sigbuf = resizeSignal(this->ip_03_sigbuf, this->maxvs, maxBlockSize);
        this->ip_04_sigbuf = resizeSignal(this->ip_04_sigbuf, this->maxvs, maxBlockSize);
        this->ip_05_sigbuf = resizeSignal(this->ip_05_sigbuf, this->maxvs, maxBlockSize);
        this->globaltransport_tempo = resizeSignal(this->globaltransport_tempo, this->maxvs, maxBlockSize);
        this->globaltransport_state = resizeSignal(this->globaltransport_state, this->maxvs, maxBlockSize);
        this->zeroBuffer = resizeSignal(this->zeroBuffer, this->maxvs, maxBlockSize);
        this->dummyBuffer = resizeSignal(this->dummyBuffer, this->maxvs, maxBlockSize);
        this->didAllocateSignals = true;
    }

    const bool sampleRateChanged = sampleRate != this->sr;
    const bool maxvsChanged = maxBlockSize != this->maxvs;
    const bool forceDSPSetup = sampleRateChanged || maxvsChanged || force;

    if (sampleRateChanged || maxvsChanged) {
        this->vs = maxBlockSize;
        this->maxvs = maxBlockSize;
        this->sr = sampleRate;
        this->invsr = 1 / sampleRate;
    }

    this->ip_01_dspsetup(forceDSPSetup);
    this->svf_tilde_01_dspsetup(forceDSPSetup);
    this->saw_tilde_01_dspsetup(forceDSPSetup);
    this->ip_02_dspsetup(forceDSPSetup);
    this->svf_tilde_03_dspsetup(forceDSPSetup);
    this->ip_03_dspsetup(forceDSPSetup);
    this->svf_tilde_04_dspsetup(forceDSPSetup);
    this->ip_04_dspsetup(forceDSPSetup);
    this->svf_tilde_05_dspsetup(forceDSPSetup);
    this->ip_05_dspsetup(forceDSPSetup);
    this->svf_tilde_06_dspsetup(forceDSPSetup);
    this->svf_tilde_02_dspsetup(forceDSPSetup);
    this->globaltransport_dspsetup(forceDSPSetup);

    if (sampleRateChanged)
        this->onSampleRateChanged(sampleRate);
}

number msToSamps(MillisecondTime ms, number sampleRate) {
    return ms * sampleRate * 0.001;
}

MillisecondTime sampsToMs(SampleIndex samps) {
    return samps * (this->invsr * 1000);
}

Index getNumInputChannels() const {
    return 3;
}

Index getNumOutputChannels() const {
    return 1;
}

DataRef* getDataRef(DataRefIndex index)  {
    switch (index) {
    case 0:
        {
        return addressOf(this->RNBODefaultMtofLookupTable256);
        break;
        }
    default:
        {
        return nullptr;
        }
    }
}

DataRefIndex getNumDataRefs() const {
    return 1;
}

void processDataViewUpdate(DataRefIndex index, MillisecondTime time) {
    this->updateTime(time, (ENGINE*)nullptr);

    if (index == 0) {
        this->mtof_tilde_01_innerMtoF_buffer = reInitDataView(this->mtof_tilde_01_innerMtoF_buffer, this->RNBODefaultMtofLookupTable256);
    }
}

void initialize() {
    RNBO_ASSERT(!this->_isInitialized);

    this->RNBODefaultMtofLookupTable256 = initDataRef(
        this->RNBODefaultMtofLookupTable256,
        this->dataRefStrings->name0,
        true,
        this->dataRefStrings->file0,
        this->dataRefStrings->tag0
    );

    this->assign_defaults();
    this->applyState();
    this->RNBODefaultMtofLookupTable256->setIndex(0);
    this->mtof_tilde_01_innerMtoF_buffer = new SampleBuffer(this->RNBODefaultMtofLookupTable256);
    this->initializeObjects();
    this->allocateDataRefs();
    this->startup();
    this->_isInitialized = true;
}

void getPreset(PatcherStateInterface& preset) {
    this->updateTime(this->getEngine()->getCurrentTime(), (ENGINE*)nullptr);
    preset["__presetid"] = "rnbo";
    this->param_01_getPresetValue(getSubState(preset, "base_pitch"));
    this->param_02_getPresetValue(getSubState(preset, "base_cutoff"));
    this->param_03_getPresetValue(getSubState(preset, "attenv_cutoff"));
    this->param_04_getPresetValue(getSubState(preset, "base_res"));
    this->param_05_getPresetValue(getSubState(preset, "atten_res"));
}

void setPreset(MillisecondTime time, PatcherStateInterface& preset) {
    this->updateTime(time, (ENGINE*)nullptr);
    this->param_01_setPresetValue(getSubState(preset, "base_pitch"));
    this->param_02_setPresetValue(getSubState(preset, "base_cutoff"));
    this->param_03_setPresetValue(getSubState(preset, "attenv_cutoff"));
    this->param_04_setPresetValue(getSubState(preset, "base_res"));
    this->param_05_setPresetValue(getSubState(preset, "atten_res"));
}

void setParameterValue(ParameterIndex index, ParameterValue v, MillisecondTime time) {
    this->updateTime(time, (ENGINE*)nullptr);

    switch (index) {
    case 0:
        {
        this->param_01_value_set(v);
        break;
        }
    case 1:
        {
        this->param_02_value_set(v);
        break;
        }
    case 2:
        {
        this->param_03_value_set(v);
        break;
        }
    case 3:
        {
        this->param_04_value_set(v);
        break;
        }
    case 4:
        {
        this->param_05_value_set(v);
        break;
        }
    }
}

void processParameterEvent(ParameterIndex index, ParameterValue value, MillisecondTime time) {
    this->setParameterValue(index, value, time);
}

void processParameterBangEvent(ParameterIndex index, MillisecondTime time) {
    this->setParameterValue(index, this->getParameterValue(index), time);
}

void processNormalizedParameterEvent(ParameterIndex index, ParameterValue value, MillisecondTime time) {
    this->setParameterValueNormalized(index, value, time);
}

ParameterValue getParameterValue(ParameterIndex index)  {
    switch (index) {
    case 0:
        {
        return this->param_01_value;
        }
    case 1:
        {
        return this->param_02_value;
        }
    case 2:
        {
        return this->param_03_value;
        }
    case 3:
        {
        return this->param_04_value;
        }
    case 4:
        {
        return this->param_05_value;
        }
    default:
        {
        return 0;
        }
    }
}

ParameterIndex getNumSignalInParameters() const {
    return 0;
}

ParameterIndex getNumSignalOutParameters() const {
    return 0;
}

ParameterIndex getNumParameters() const {
    return 5;
}

ConstCharPointer getParameterName(ParameterIndex index) const {
    switch (index) {
    case 0:
        {
        return "base_pitch";
        }
    case 1:
        {
        return "base_cutoff";
        }
    case 2:
        {
        return "attenv_cutoff";
        }
    case 3:
        {
        return "base_res";
        }
    case 4:
        {
        return "atten_res";
        }
    default:
        {
        return "bogus";
        }
    }
}

ConstCharPointer getParameterId(ParameterIndex index) const {
    switch (index) {
    case 0:
        {
        return "base_pitch";
        }
    case 1:
        {
        return "base_cutoff";
        }
    case 2:
        {
        return "attenv_cutoff";
        }
    case 3:
        {
        return "base_res";
        }
    case 4:
        {
        return "atten_res";
        }
    default:
        {
        return "bogus";
        }
    }
}

void getParameterInfo(ParameterIndex index, ParameterInfo * info) const {
    {
        switch (index) {
        case 0:
            {
            info->type = ParameterTypeNumber;
            info->initialValue = 60;
            info->min = 0;
            info->max = 127;
            info->exponent = 1;
            info->steps = 0;
            info->debug = false;
            info->saveable = true;
            info->transmittable = true;
            info->initialized = true;
            info->visible = true;
            info->displayName = "";
            info->unit = "";
            info->ioType = IOTypeUndefined;
            info->signalIndex = INVALID_INDEX;
            break;
            }
        case 1:
            {
            info->type = ParameterTypeNumber;
            info->initialValue = 400;
            info->min = 20;
            info->max = 20000;
            info->exponent = 1;
            info->steps = 0;
            info->debug = false;
            info->saveable = true;
            info->transmittable = true;
            info->initialized = true;
            info->visible = true;
            info->displayName = "";
            info->unit = "";
            info->ioType = IOTypeUndefined;
            info->signalIndex = INVALID_INDEX;
            break;
            }
        case 2:
            {
            info->type = ParameterTypeNumber;
            info->initialValue = 0;
            info->min = -1;
            info->max = 1;
            info->exponent = 1;
            info->steps = 0;
            info->debug = false;
            info->saveable = true;
            info->transmittable = true;
            info->initialized = true;
            info->visible = true;
            info->displayName = "";
            info->unit = "";
            info->ioType = IOTypeUndefined;
            info->signalIndex = INVALID_INDEX;
            break;
            }
        case 3:
            {
            info->type = ParameterTypeNumber;
            info->initialValue = 0.5;
            info->min = 0;
            info->max = 1;
            info->exponent = 1;
            info->steps = 0;
            info->debug = false;
            info->saveable = true;
            info->transmittable = true;
            info->initialized = true;
            info->visible = true;
            info->displayName = "";
            info->unit = "";
            info->ioType = IOTypeUndefined;
            info->signalIndex = INVALID_INDEX;
            break;
            }
        case 4:
            {
            info->type = ParameterTypeNumber;
            info->initialValue = 0;
            info->min = 0;
            info->max = 1;
            info->exponent = 1;
            info->steps = 0;
            info->debug = false;
            info->saveable = true;
            info->transmittable = true;
            info->initialized = true;
            info->visible = true;
            info->displayName = "";
            info->unit = "";
            info->ioType = IOTypeUndefined;
            info->signalIndex = INVALID_INDEX;
            break;
            }
        }
    }
}

ParameterValue applyStepsToNormalizedParameterValue(ParameterValue normalizedValue, int steps) const {
    if (steps == 1) {
        if (normalizedValue > 0) {
            normalizedValue = 1.;
        }
    } else {
        ParameterValue oneStep = (number)1. / (steps - 1);
        ParameterValue numberOfSteps = rnbo_fround(normalizedValue / oneStep * 1 / (number)1) * (number)1;
        normalizedValue = numberOfSteps * oneStep;
    }

    return normalizedValue;
}

ParameterValue convertToNormalizedParameterValue(ParameterIndex index, ParameterValue value) const {
    switch (index) {
    case 3:
    case 4:
        {
        {
            value = (value < 0 ? 0 : (value > 1 ? 1 : value));
            ParameterValue normalizedValue = (value - 0) / (1 - 0);
            return normalizedValue;
        }
        }
    case 0:
        {
        {
            value = (value < 0 ? 0 : (value > 127 ? 127 : value));
            ParameterValue normalizedValue = (value - 0) / (127 - 0);
            return normalizedValue;
        }
        }
    case 1:
        {
        {
            value = (value < 20 ? 20 : (value > 20000 ? 20000 : value));
            ParameterValue normalizedValue = (value - 20) / (20000 - 20);
            return normalizedValue;
        }
        }
    case 2:
        {
        {
            value = (value < -1 ? -1 : (value > 1 ? 1 : value));
            ParameterValue normalizedValue = (value - -1) / (1 - -1);
            return normalizedValue;
        }
        }
    default:
        {
        return value;
        }
    }
}

ParameterValue convertFromNormalizedParameterValue(ParameterIndex index, ParameterValue value) const {
    value = (value < 0 ? 0 : (value > 1 ? 1 : value));

    switch (index) {
    case 3:
    case 4:
        {
        {
            {
                return 0 + value * (1 - 0);
            }
        }
        }
    case 0:
        {
        {
            {
                return 0 + value * (127 - 0);
            }
        }
        }
    case 1:
        {
        {
            {
                return 20 + value * (20000 - 20);
            }
        }
        }
    case 2:
        {
        {
            {
                return -1 + value * (1 - -1);
            }
        }
        }
    default:
        {
        return value;
        }
    }
}

ParameterValue constrainParameterValue(ParameterIndex index, ParameterValue value) const {
    switch (index) {
    case 0:
        {
        return this->param_01_value_constrain(value);
        }
    case 1:
        {
        return this->param_02_value_constrain(value);
        }
    case 2:
        {
        return this->param_03_value_constrain(value);
        }
    case 3:
        {
        return this->param_04_value_constrain(value);
        }
    case 4:
        {
        return this->param_05_value_constrain(value);
        }
    default:
        {
        return value;
        }
    }
}

void processNumMessage(MessageTag , MessageTag , MillisecondTime , number ) {}

void processListMessage(MessageTag , MessageTag , MillisecondTime , const list& ) {}

void processBangMessage(MessageTag , MessageTag , MillisecondTime ) {}

MessageTagInfo resolveTag(MessageTag tag) const {
    switch (tag) {

    }

    return "";
}

MessageIndex getNumMessages() const {
    return 0;
}

const MessageInfo& getMessageInfo(MessageIndex index) const {
    switch (index) {

    }

    return NullMessageInfo;
}

protected:

		
void advanceTime(EXTERNALENGINE*) {}
void advanceTime(INTERNALENGINE*) {
	_internalEngine.advanceTime(sampstoms(this->vs));
}

void processInternalEvents(MillisecondTime time) {
	_internalEngine.processEventsUntil(time);
}

void updateTime(MillisecondTime time, INTERNALENGINE*, bool inProcess = false) {
	if (time == TimeNow) time = getPatcherTime();
	processInternalEvents(inProcess ? time + sampsToMs(this->vs) : time);
	updateTime(time, (EXTERNALENGINE*)nullptr);
}

rnbomatic* operator->() {
    return this;
}
const rnbomatic* operator->() const {
    return this;
}
rnbomatic* getTopLevelPatcher() {
    return this;
}

void cancelClockEvents()
{
}

template<typename LISTTYPE = list> void listquicksort(LISTTYPE& arr, LISTTYPE& sortindices, Int l, Int h, bool ascending) {
    if (l < h) {
        Int p = (Int)(this->listpartition(arr, sortindices, l, h, ascending));
        this->listquicksort(arr, sortindices, l, p - 1, ascending);
        this->listquicksort(arr, sortindices, p + 1, h, ascending);
    }
}

template<typename LISTTYPE = list> Int listpartition(LISTTYPE& arr, LISTTYPE& sortindices, Int l, Int h, bool ascending) {
    number x = arr[(Index)h];
    Int i = (Int)(l - 1);

    for (Int j = (Int)(l); j <= h - 1; j++) {
        bool asc = (bool)((bool)(ascending) && arr[(Index)j] <= x);
        bool desc = (bool)((bool)(!(bool)(ascending)) && arr[(Index)j] >= x);

        if ((bool)(asc) || (bool)(desc)) {
            i++;
            this->listswapelements(arr, i, j);
            this->listswapelements(sortindices, i, j);
        }
    }

    i++;
    this->listswapelements(arr, i, h);
    this->listswapelements(sortindices, i, h);
    return i;
}

template<typename LISTTYPE = list> void listswapelements(LISTTYPE& arr, Int a, Int b) {
    auto tmp = arr[(Index)a];
    arr[(Index)a] = arr[(Index)b];
    arr[(Index)b] = tmp;
}

inline number safediv(number num, number denom) {
    return (denom == 0.0 ? 0.0 : num / denom);
}

number safepow(number base, number exponent) {
    return fixnan(rnbo_pow(base, exponent));
}

inline number linearinterp(number frac, number x, number y) {
    return x + (y - x) * frac;
}

number mstosamps(MillisecondTime ms) {
    return ms * this->sr * 0.001;
}

number maximum(number x, number y) {
    return (x < y ? y : x);
}

MillisecondTime sampstoms(number samps) {
    return samps * 1000 / this->sr;
}

void param_01_value_set(number v) {
    v = this->param_01_value_constrain(v);
    this->param_01_value = v;
    this->sendParameter(0, false);

    if (this->param_01_value != this->param_01_lastValue) {
        {
            this->getEngine()->presetTouched();
        }

        this->param_01_lastValue = this->param_01_value;
    }

    this->ip_01_value_set(v);
}

void param_02_value_set(number v) {
    v = this->param_02_value_constrain(v);
    this->param_02_value = v;
    this->sendParameter(1, false);

    if (this->param_02_value != this->param_02_lastValue) {
        {
            this->getEngine()->presetTouched();
        }

        this->param_02_lastValue = this->param_02_value;
    }

    this->ip_02_value_set(v);
}

void param_03_value_set(number v) {
    v = this->param_03_value_constrain(v);
    this->param_03_value = v;
    this->sendParameter(2, false);

    if (this->param_03_value != this->param_03_lastValue) {
        {
            this->getEngine()->presetTouched();
        }

        this->param_03_lastValue = this->param_03_value;
    }

    this->ip_03_value_set(v);
}

void param_04_value_set(number v) {
    v = this->param_04_value_constrain(v);
    this->param_04_value = v;
    this->sendParameter(3, false);

    if (this->param_04_value != this->param_04_lastValue) {
        {
            this->getEngine()->presetTouched();
        }

        this->param_04_lastValue = this->param_04_value;
    }

    this->ip_04_value_set(v);
}

void param_05_value_set(number v) {
    v = this->param_05_value_constrain(v);
    this->param_05_value = v;
    this->sendParameter(4, false);

    if (this->param_05_value != this->param_05_lastValue) {
        {
            this->getEngine()->presetTouched();
        }

        this->param_05_lastValue = this->param_05_value;
    }

    this->ip_05_value_set(v);
}

MillisecondTime getPatcherTime() const {
    return this->_currentTime;
}

void deallocateSignals() {
    Index i;

    for (i = 0; i < 6; i++) {
        this->signals[i] = freeSignal(this->signals[i]);
    }

    this->ip_01_sigbuf = freeSignal(this->ip_01_sigbuf);
    this->ip_02_sigbuf = freeSignal(this->ip_02_sigbuf);
    this->ip_03_sigbuf = freeSignal(this->ip_03_sigbuf);
    this->ip_04_sigbuf = freeSignal(this->ip_04_sigbuf);
    this->ip_05_sigbuf = freeSignal(this->ip_05_sigbuf);
    this->globaltransport_tempo = freeSignal(this->globaltransport_tempo);
    this->globaltransport_state = freeSignal(this->globaltransport_state);
    this->zeroBuffer = freeSignal(this->zeroBuffer);
    this->dummyBuffer = freeSignal(this->dummyBuffer);
}

Index getMaxBlockSize() const {
    return this->maxvs;
}

number getSampleRate() const {
    return this->sr;
}

bool hasFixedVectorSize() const {
    return false;
}

void setProbingTarget(MessageTag ) {}

void fillRNBODefaultMtofLookupTable256(DataRef& ref) {
    SampleBuffer buffer(ref);
    number bufsize = buffer->getSize();

    for (Index i = 0; i < bufsize; i++) {
        number midivalue = -256. + (number)512. / (bufsize - 1) * i;
        buffer[i] = rnbo_exp(.057762265 * (midivalue - 69.0));
    }
}

void fillDataRef(DataRefIndex index, DataRef& ref) {
    switch (index) {
    case 0:
        {
        this->fillRNBODefaultMtofLookupTable256(ref);
        break;
        }
    }
}

void allocateDataRefs() {
    this->mtof_tilde_01_innerMtoF_buffer->requestSize(65536, 1);
    this->mtof_tilde_01_innerMtoF_buffer->setSampleRate(this->sr);
    this->mtof_tilde_01_innerMtoF_buffer = this->mtof_tilde_01_innerMtoF_buffer->allocateIfNeeded();

    if (this->RNBODefaultMtofLookupTable256->hasRequestedSize()) {
        if (this->RNBODefaultMtofLookupTable256->wantsFill())
            this->fillRNBODefaultMtofLookupTable256(this->RNBODefaultMtofLookupTable256);

        this->getEngine()->sendDataRefUpdated(0);
    }
}

void initializeObjects() {
    this->ip_01_init();
    this->mtof_tilde_01_innerScala_init();
    this->mtof_tilde_01_init();
    this->ip_02_init();
    this->ip_03_init();
    this->ip_04_init();
    this->ip_05_init();
}

Index getIsMuted()  {
    return this->isMuted;
}

void setIsMuted(Index v)  {
    this->isMuted = v;
}

void onSampleRateChanged(double ) {}

void extractState(PatcherStateInterface& ) {}

void applyState() {}

void processClockEvent(MillisecondTime , ClockId , bool , ParameterValue ) {}

void processOutletAtCurrentTime(EngineLink* , OutletIndex , ParameterValue ) {}

void processOutletEvent(
    EngineLink* sender,
    OutletIndex index,
    ParameterValue value,
    MillisecondTime time
) {
    this->updateTime(time, (ENGINE*)nullptr);
    this->processOutletAtCurrentTime(sender, index, value);
}

void sendOutlet(OutletIndex index, ParameterValue value) {
    this->getEngine()->sendOutlet(this, index, value);
}

void startup() {
    this->updateTime(this->getEngine()->getCurrentTime(), (ENGINE*)nullptr);

    {
        this->scheduleParamInit(0, 0);
    }

    {
        this->scheduleParamInit(1, 0);
    }

    {
        this->scheduleParamInit(2, 0);
    }

    {
        this->scheduleParamInit(3, 0);
    }

    {
        this->scheduleParamInit(4, 0);
    }

    this->processParamInitEvents();
}

number param_01_value_constrain(number v) const {
    v = (v > 127 ? 127 : (v < 0 ? 0 : v));
    return v;
}

void ip_01_value_set(number v) {
    this->ip_01_value = v;
    this->ip_01_fillSigBuf();
    this->ip_01_lastValue = v;
}

number param_02_value_constrain(number v) const {
    v = (v > 20000 ? 20000 : (v < 20 ? 20 : v));
    return v;
}

void ip_02_value_set(number v) {
    this->ip_02_value = v;
    this->ip_02_fillSigBuf();
    this->ip_02_lastValue = v;
}

number param_03_value_constrain(number v) const {
    v = (v > 1 ? 1 : (v < -1 ? -1 : v));
    return v;
}

void ip_03_value_set(number v) {
    this->ip_03_value = v;
    this->ip_03_fillSigBuf();
    this->ip_03_lastValue = v;
}

number param_04_value_constrain(number v) const {
    v = (v > 1 ? 1 : (v < 0 ? 0 : v));
    return v;
}

void ip_04_value_set(number v) {
    this->ip_04_value = v;
    this->ip_04_fillSigBuf();
    this->ip_04_lastValue = v;
}

number param_05_value_constrain(number v) const {
    v = (v > 1 ? 1 : (v < 0 ? 0 : v));
    return v;
}

void ip_05_value_set(number v) {
    this->ip_05_value = v;
    this->ip_05_fillSigBuf();
    this->ip_05_lastValue = v;
}

void ip_01_perform(SampleValue * out, Index n) {
    auto __ip_01_lastValue = this->ip_01_lastValue;
    auto __ip_01_lastIndex = this->ip_01_lastIndex;

    for (Index i = 0; i < n; i++) {
        out[(Index)i] = ((SampleIndex)(i) >= __ip_01_lastIndex ? __ip_01_lastValue : this->ip_01_sigbuf[(Index)i]);
    }

    __ip_01_lastIndex = 0;
    this->ip_01_lastIndex = __ip_01_lastIndex;
}

void svf_tilde_01_perform(
    const Sample * x,
    number freq,
    number q,
    SampleValue * out1,
    SampleValue * out2,
    SampleValue * out3,
    SampleValue * out4,
    Index n
) {
    RNBO_UNUSED(q);
    RNBO_UNUSED(freq);
    auto __svf_tilde_01_high = this->svf_tilde_01_high;
    auto __svf_tilde_01_notch = this->svf_tilde_01_notch;
    auto __svf_tilde_01_band = this->svf_tilde_01_band;
    auto __svf_tilde_01_low = this->svf_tilde_01_low;
    auto __svf_tilde_01__q = this->svf_tilde_01__q;
    auto __svf_tilde_01__freq = this->svf_tilde_01__freq;
    Index i;

    for (i = 0; i < (Index)n; i++) {
        if (this->svf_tilde_01_dFreq_next(20) != 0) {
            __svf_tilde_01__freq = (20 > this->sr * 0.25 ? this->sr * 0.25 : 20);
            __svf_tilde_01__freq = rnbo_sin(6.28318530717958647692 * __svf_tilde_01__freq / this->sr);
            __svf_tilde_01__freq = (__svf_tilde_01__freq > 0.99 ? 0.99 : (__svf_tilde_01__freq < 0.001 ? 0.001 : __svf_tilde_01__freq));
        }

        if (this->svf_tilde_01_dQ_next(0) != 0) {
            __svf_tilde_01__q = (1. - 0) * 1.2;
            __svf_tilde_01__q = __svf_tilde_01__q * __svf_tilde_01__q;
            __svf_tilde_01__q = (__svf_tilde_01__q > 0.99 ? 0.99 : (__svf_tilde_01__q < 0.001 ? 0.001 : __svf_tilde_01__q));
        }

        __svf_tilde_01_low += __svf_tilde_01__freq * __svf_tilde_01_band;
        __svf_tilde_01_notch = x[(Index)i] - __svf_tilde_01__q * __svf_tilde_01_band;
        __svf_tilde_01_high = __svf_tilde_01_notch - __svf_tilde_01_low;
        __svf_tilde_01_band += __svf_tilde_01__freq * __svf_tilde_01_high - __svf_tilde_01_band * __svf_tilde_01_band * __svf_tilde_01_band * 0.0001;
        array<number, 4> tmp = {0, 0, 0, 0};
        tmp[0] = __svf_tilde_01_low;
        tmp[1] = __svf_tilde_01_high;
        tmp[2] = __svf_tilde_01_band;
        tmp[3] = __svf_tilde_01_notch;
        out1[(Index)i] = tmp[0];
        out2[(Index)i] = tmp[1];
        out3[(Index)i] = tmp[2];
        out4[(Index)i] = tmp[3];
    }

    this->svf_tilde_01__freq = __svf_tilde_01__freq;
    this->svf_tilde_01__q = __svf_tilde_01__q;
    this->svf_tilde_01_low = __svf_tilde_01_low;
    this->svf_tilde_01_band = __svf_tilde_01_band;
    this->svf_tilde_01_notch = __svf_tilde_01_notch;
    this->svf_tilde_01_high = __svf_tilde_01_high;
}

void dspexpr_02_perform(const Sample * in1, number in2, SampleValue * out1, Index n) {
    RNBO_UNUSED(in2);
    Index i;

    for (i = 0; i < (Index)n; i++) {
        out1[(Index)i] = in1[(Index)i] * 12;//#map:_###_obj_###_:1
    }
}

void dspexpr_01_perform(const Sample * in1, const Sample * in2, SampleValue * out1, Index n) {
    Index i;

    for (i = 0; i < (Index)n; i++) {
        out1[(Index)i] = in1[(Index)i] + in2[(Index)i];//#map:_###_obj_###_:1
    }
}

void mtof_tilde_01_perform(const Sample * midivalue, SampleValue * out, Index n) {
    auto __mtof_tilde_01_base = this->mtof_tilde_01_base;

    for (Index i = 0; i < n; i++) {
        out[(Index)i] = this->mtof_tilde_01_innerMtoF_next(midivalue[(Index)i], __mtof_tilde_01_base);
    }
}

void saw_tilde_01_perform(
    const Sample * frequency,
    number syncPhase,
    SampleValue * out1,
    SampleValue * out2,
    Index n
) {
    RNBO_UNUSED(syncPhase);
    auto __saw_tilde_01_didSync = this->saw_tilde_01_didSync;
    auto __saw_tilde_01_lastSyncDiff = this->saw_tilde_01_lastSyncDiff;
    auto __saw_tilde_01_lastSyncPhase = this->saw_tilde_01_lastSyncPhase;
    auto __saw_tilde_01_t = this->saw_tilde_01_t;
    Index i;

    for (i = 0; i < (Index)n; i++) {
        number dt = frequency[(Index)i] / this->sr;
        number t1 = __saw_tilde_01_t + 0.5;
        t1 -= trunc(t1);
        number y = 2 * t1 - 1;

        if (dt != 0.0) {
            number syncDiff = 0 - __saw_tilde_01_lastSyncPhase;
            __saw_tilde_01_lastSyncPhase = 0;
            __saw_tilde_01_lastSyncDiff = syncDiff;
            number lookahead = 0 + syncDiff;

            if (t1 < dt) {
                number d = t1 / dt;
                y -= d + d - d * d - 1;
            } else if (t1 + dt > 1) {
                number d = (t1 - 1) / dt;
                y -= d + d + d * d + 1;
            } else if ((bool)(__saw_tilde_01_didSync)) {
                y = 0;
                __saw_tilde_01_didSync = false;
            } else if (lookahead > 1) {
                y *= 0.5;
                __saw_tilde_01_t = 0;
                __saw_tilde_01_didSync = true;
            }

            __saw_tilde_01_t += dt;

            if (dt > 0) {
                while (__saw_tilde_01_t >= 1) {
                    __saw_tilde_01_t -= 1;
                }
            } else {
                while (__saw_tilde_01_t <= 0) {
                    __saw_tilde_01_t += 1;
                }
            }
        }

        y = this->saw_tilde_01_dcblocker_next(y, 0.9997);
        out1[(Index)i] = 0.5 * y;
        out2[(Index)i] = __saw_tilde_01_t;
    }

    this->saw_tilde_01_t = __saw_tilde_01_t;
    this->saw_tilde_01_lastSyncPhase = __saw_tilde_01_lastSyncPhase;
    this->saw_tilde_01_lastSyncDiff = __saw_tilde_01_lastSyncDiff;
    this->saw_tilde_01_didSync = __saw_tilde_01_didSync;
}

void ip_02_perform(SampleValue * out, Index n) {
    auto __ip_02_lastValue = this->ip_02_lastValue;
    auto __ip_02_lastIndex = this->ip_02_lastIndex;

    for (Index i = 0; i < n; i++) {
        out[(Index)i] = ((SampleIndex)(i) >= __ip_02_lastIndex ? __ip_02_lastValue : this->ip_02_sigbuf[(Index)i]);
    }

    __ip_02_lastIndex = 0;
    this->ip_02_lastIndex = __ip_02_lastIndex;
}

void svf_tilde_03_perform(
    const Sample * x,
    number freq,
    number q,
    SampleValue * out1,
    SampleValue * out2,
    SampleValue * out3,
    SampleValue * out4,
    Index n
) {
    RNBO_UNUSED(q);
    RNBO_UNUSED(freq);
    auto __svf_tilde_03_high = this->svf_tilde_03_high;
    auto __svf_tilde_03_notch = this->svf_tilde_03_notch;
    auto __svf_tilde_03_band = this->svf_tilde_03_band;
    auto __svf_tilde_03_low = this->svf_tilde_03_low;
    auto __svf_tilde_03__q = this->svf_tilde_03__q;
    auto __svf_tilde_03__freq = this->svf_tilde_03__freq;
    Index i;

    for (i = 0; i < (Index)n; i++) {
        if (this->svf_tilde_03_dFreq_next(20) != 0) {
            __svf_tilde_03__freq = (20 > this->sr * 0.25 ? this->sr * 0.25 : 20);
            __svf_tilde_03__freq = rnbo_sin(6.28318530717958647692 * __svf_tilde_03__freq / this->sr);
            __svf_tilde_03__freq = (__svf_tilde_03__freq > 0.99 ? 0.99 : (__svf_tilde_03__freq < 0.001 ? 0.001 : __svf_tilde_03__freq));
        }

        if (this->svf_tilde_03_dQ_next(0) != 0) {
            __svf_tilde_03__q = (1. - 0) * 1.2;
            __svf_tilde_03__q = __svf_tilde_03__q * __svf_tilde_03__q;
            __svf_tilde_03__q = (__svf_tilde_03__q > 0.99 ? 0.99 : (__svf_tilde_03__q < 0.001 ? 0.001 : __svf_tilde_03__q));
        }

        __svf_tilde_03_low += __svf_tilde_03__freq * __svf_tilde_03_band;
        __svf_tilde_03_notch = x[(Index)i] - __svf_tilde_03__q * __svf_tilde_03_band;
        __svf_tilde_03_high = __svf_tilde_03_notch - __svf_tilde_03_low;
        __svf_tilde_03_band += __svf_tilde_03__freq * __svf_tilde_03_high - __svf_tilde_03_band * __svf_tilde_03_band * __svf_tilde_03_band * 0.0001;
        array<number, 4> tmp = {0, 0, 0, 0};
        tmp[0] = __svf_tilde_03_low;
        tmp[1] = __svf_tilde_03_high;
        tmp[2] = __svf_tilde_03_band;
        tmp[3] = __svf_tilde_03_notch;
        out1[(Index)i] = tmp[0];
        out2[(Index)i] = tmp[1];
        out3[(Index)i] = tmp[2];
        out4[(Index)i] = tmp[3];
    }

    this->svf_tilde_03__freq = __svf_tilde_03__freq;
    this->svf_tilde_03__q = __svf_tilde_03__q;
    this->svf_tilde_03_low = __svf_tilde_03_low;
    this->svf_tilde_03_band = __svf_tilde_03_band;
    this->svf_tilde_03_notch = __svf_tilde_03_notch;
    this->svf_tilde_03_high = __svf_tilde_03_high;
}

void scale_tilde_02_perform(
    const Sample * x,
    number lowin,
    number hiin,
    number lowout,
    number highout,
    number pow,
    SampleValue * out1,
    Index n
) {
    RNBO_UNUSED(pow);
    RNBO_UNUSED(highout);
    RNBO_UNUSED(lowout);
    RNBO_UNUSED(hiin);
    RNBO_UNUSED(lowin);
    auto inscale = this->safediv(1., 5 - -5);
    number outdiff = 10000 - -10000;
    Index i;

    for (i = 0; i < (Index)n; i++) {
        number value = (x[(Index)i] - -5) * inscale;
        value = value * outdiff + -10000;
        out1[(Index)i] = value;
    }
}

void ip_03_perform(SampleValue * out, Index n) {
    auto __ip_03_lastValue = this->ip_03_lastValue;
    auto __ip_03_lastIndex = this->ip_03_lastIndex;

    for (Index i = 0; i < n; i++) {
        out[(Index)i] = ((SampleIndex)(i) >= __ip_03_lastIndex ? __ip_03_lastValue : this->ip_03_sigbuf[(Index)i]);
    }

    __ip_03_lastIndex = 0;
    this->ip_03_lastIndex = __ip_03_lastIndex;
}

void svf_tilde_04_perform(
    const Sample * x,
    number freq,
    number q,
    SampleValue * out1,
    SampleValue * out2,
    SampleValue * out3,
    SampleValue * out4,
    Index n
) {
    RNBO_UNUSED(q);
    RNBO_UNUSED(freq);
    auto __svf_tilde_04_high = this->svf_tilde_04_high;
    auto __svf_tilde_04_notch = this->svf_tilde_04_notch;
    auto __svf_tilde_04_band = this->svf_tilde_04_band;
    auto __svf_tilde_04_low = this->svf_tilde_04_low;
    auto __svf_tilde_04__q = this->svf_tilde_04__q;
    auto __svf_tilde_04__freq = this->svf_tilde_04__freq;
    Index i;

    for (i = 0; i < (Index)n; i++) {
        if (this->svf_tilde_04_dFreq_next(20) != 0) {
            __svf_tilde_04__freq = (20 > this->sr * 0.25 ? this->sr * 0.25 : 20);
            __svf_tilde_04__freq = rnbo_sin(6.28318530717958647692 * __svf_tilde_04__freq / this->sr);
            __svf_tilde_04__freq = (__svf_tilde_04__freq > 0.99 ? 0.99 : (__svf_tilde_04__freq < 0.001 ? 0.001 : __svf_tilde_04__freq));
        }

        if (this->svf_tilde_04_dQ_next(0) != 0) {
            __svf_tilde_04__q = (1. - 0) * 1.2;
            __svf_tilde_04__q = __svf_tilde_04__q * __svf_tilde_04__q;
            __svf_tilde_04__q = (__svf_tilde_04__q > 0.99 ? 0.99 : (__svf_tilde_04__q < 0.001 ? 0.001 : __svf_tilde_04__q));
        }

        __svf_tilde_04_low += __svf_tilde_04__freq * __svf_tilde_04_band;
        __svf_tilde_04_notch = x[(Index)i] - __svf_tilde_04__q * __svf_tilde_04_band;
        __svf_tilde_04_high = __svf_tilde_04_notch - __svf_tilde_04_low;
        __svf_tilde_04_band += __svf_tilde_04__freq * __svf_tilde_04_high - __svf_tilde_04_band * __svf_tilde_04_band * __svf_tilde_04_band * 0.0001;
        array<number, 4> tmp = {0, 0, 0, 0};
        tmp[0] = __svf_tilde_04_low;
        tmp[1] = __svf_tilde_04_high;
        tmp[2] = __svf_tilde_04_band;
        tmp[3] = __svf_tilde_04_notch;
        out1[(Index)i] = tmp[0];
        out2[(Index)i] = tmp[1];
        out3[(Index)i] = tmp[2];
        out4[(Index)i] = tmp[3];
    }

    this->svf_tilde_04__freq = __svf_tilde_04__freq;
    this->svf_tilde_04__q = __svf_tilde_04__q;
    this->svf_tilde_04_low = __svf_tilde_04_low;
    this->svf_tilde_04_band = __svf_tilde_04_band;
    this->svf_tilde_04_notch = __svf_tilde_04_notch;
    this->svf_tilde_04_high = __svf_tilde_04_high;
}

void dspexpr_04_perform(const Sample * in1, const Sample * in2, SampleValue * out1, Index n) {
    Index i;

    for (i = 0; i < (Index)n; i++) {
        out1[(Index)i] = in1[(Index)i] * in2[(Index)i];//#map:_###_obj_###_:1
    }
}

void dspexpr_03_perform(const Sample * in1, const Sample * in2, SampleValue * out1, Index n) {
    Index i;

    for (i = 0; i < (Index)n; i++) {
        out1[(Index)i] = in1[(Index)i] + in2[(Index)i];//#map:_###_obj_###_:1
    }
}

void ip_04_perform(SampleValue * out, Index n) {
    auto __ip_04_lastValue = this->ip_04_lastValue;
    auto __ip_04_lastIndex = this->ip_04_lastIndex;

    for (Index i = 0; i < n; i++) {
        out[(Index)i] = ((SampleIndex)(i) >= __ip_04_lastIndex ? __ip_04_lastValue : this->ip_04_sigbuf[(Index)i]);
    }

    __ip_04_lastIndex = 0;
    this->ip_04_lastIndex = __ip_04_lastIndex;
}

void svf_tilde_05_perform(
    const Sample * x,
    number freq,
    number q,
    SampleValue * out1,
    SampleValue * out2,
    SampleValue * out3,
    SampleValue * out4,
    Index n
) {
    RNBO_UNUSED(q);
    RNBO_UNUSED(freq);
    auto __svf_tilde_05_high = this->svf_tilde_05_high;
    auto __svf_tilde_05_notch = this->svf_tilde_05_notch;
    auto __svf_tilde_05_band = this->svf_tilde_05_band;
    auto __svf_tilde_05_low = this->svf_tilde_05_low;
    auto __svf_tilde_05__q = this->svf_tilde_05__q;
    auto __svf_tilde_05__freq = this->svf_tilde_05__freq;
    Index i;

    for (i = 0; i < (Index)n; i++) {
        if (this->svf_tilde_05_dFreq_next(20) != 0) {
            __svf_tilde_05__freq = (20 > this->sr * 0.25 ? this->sr * 0.25 : 20);
            __svf_tilde_05__freq = rnbo_sin(6.28318530717958647692 * __svf_tilde_05__freq / this->sr);
            __svf_tilde_05__freq = (__svf_tilde_05__freq > 0.99 ? 0.99 : (__svf_tilde_05__freq < 0.001 ? 0.001 : __svf_tilde_05__freq));
        }

        if (this->svf_tilde_05_dQ_next(0) != 0) {
            __svf_tilde_05__q = (1. - 0) * 1.2;
            __svf_tilde_05__q = __svf_tilde_05__q * __svf_tilde_05__q;
            __svf_tilde_05__q = (__svf_tilde_05__q > 0.99 ? 0.99 : (__svf_tilde_05__q < 0.001 ? 0.001 : __svf_tilde_05__q));
        }

        __svf_tilde_05_low += __svf_tilde_05__freq * __svf_tilde_05_band;
        __svf_tilde_05_notch = x[(Index)i] - __svf_tilde_05__q * __svf_tilde_05_band;
        __svf_tilde_05_high = __svf_tilde_05_notch - __svf_tilde_05_low;
        __svf_tilde_05_band += __svf_tilde_05__freq * __svf_tilde_05_high - __svf_tilde_05_band * __svf_tilde_05_band * __svf_tilde_05_band * 0.0001;
        array<number, 4> tmp = {0, 0, 0, 0};
        tmp[0] = __svf_tilde_05_low;
        tmp[1] = __svf_tilde_05_high;
        tmp[2] = __svf_tilde_05_band;
        tmp[3] = __svf_tilde_05_notch;
        out1[(Index)i] = tmp[0];
        out2[(Index)i] = tmp[1];
        out3[(Index)i] = tmp[2];
        out4[(Index)i] = tmp[3];
    }

    this->svf_tilde_05__freq = __svf_tilde_05__freq;
    this->svf_tilde_05__q = __svf_tilde_05__q;
    this->svf_tilde_05_low = __svf_tilde_05_low;
    this->svf_tilde_05_band = __svf_tilde_05_band;
    this->svf_tilde_05_notch = __svf_tilde_05_notch;
    this->svf_tilde_05_high = __svf_tilde_05_high;
}

void scale_tilde_03_perform(
    const Sample * x,
    number lowin,
    number hiin,
    number lowout,
    number highout,
    number pow,
    SampleValue * out1,
    Index n
) {
    RNBO_UNUSED(pow);
    RNBO_UNUSED(highout);
    RNBO_UNUSED(lowout);
    RNBO_UNUSED(hiin);
    RNBO_UNUSED(lowin);
    auto inscale = this->safediv(1., 5 - -5);
    number outdiff = 1 - 0;
    Index i;

    for (i = 0; i < (Index)n; i++) {
        number value = (x[(Index)i] - -5) * inscale;
        value = value * outdiff + 0;
        out1[(Index)i] = value;
    }
}

void ip_05_perform(SampleValue * out, Index n) {
    auto __ip_05_lastValue = this->ip_05_lastValue;
    auto __ip_05_lastIndex = this->ip_05_lastIndex;

    for (Index i = 0; i < n; i++) {
        out[(Index)i] = ((SampleIndex)(i) >= __ip_05_lastIndex ? __ip_05_lastValue : this->ip_05_sigbuf[(Index)i]);
    }

    __ip_05_lastIndex = 0;
    this->ip_05_lastIndex = __ip_05_lastIndex;
}

void svf_tilde_06_perform(
    const Sample * x,
    number freq,
    number q,
    SampleValue * out1,
    SampleValue * out2,
    SampleValue * out3,
    SampleValue * out4,
    Index n
) {
    RNBO_UNUSED(q);
    RNBO_UNUSED(freq);
    auto __svf_tilde_06_high = this->svf_tilde_06_high;
    auto __svf_tilde_06_notch = this->svf_tilde_06_notch;
    auto __svf_tilde_06_band = this->svf_tilde_06_band;
    auto __svf_tilde_06_low = this->svf_tilde_06_low;
    auto __svf_tilde_06__q = this->svf_tilde_06__q;
    auto __svf_tilde_06__freq = this->svf_tilde_06__freq;
    Index i;

    for (i = 0; i < (Index)n; i++) {
        if (this->svf_tilde_06_dFreq_next(20) != 0) {
            __svf_tilde_06__freq = (20 > this->sr * 0.25 ? this->sr * 0.25 : 20);
            __svf_tilde_06__freq = rnbo_sin(6.28318530717958647692 * __svf_tilde_06__freq / this->sr);
            __svf_tilde_06__freq = (__svf_tilde_06__freq > 0.99 ? 0.99 : (__svf_tilde_06__freq < 0.001 ? 0.001 : __svf_tilde_06__freq));
        }

        if (this->svf_tilde_06_dQ_next(0) != 0) {
            __svf_tilde_06__q = (1. - 0) * 1.2;
            __svf_tilde_06__q = __svf_tilde_06__q * __svf_tilde_06__q;
            __svf_tilde_06__q = (__svf_tilde_06__q > 0.99 ? 0.99 : (__svf_tilde_06__q < 0.001 ? 0.001 : __svf_tilde_06__q));
        }

        __svf_tilde_06_low += __svf_tilde_06__freq * __svf_tilde_06_band;
        __svf_tilde_06_notch = x[(Index)i] - __svf_tilde_06__q * __svf_tilde_06_band;
        __svf_tilde_06_high = __svf_tilde_06_notch - __svf_tilde_06_low;
        __svf_tilde_06_band += __svf_tilde_06__freq * __svf_tilde_06_high - __svf_tilde_06_band * __svf_tilde_06_band * __svf_tilde_06_band * 0.0001;
        array<number, 4> tmp = {0, 0, 0, 0};
        tmp[0] = __svf_tilde_06_low;
        tmp[1] = __svf_tilde_06_high;
        tmp[2] = __svf_tilde_06_band;
        tmp[3] = __svf_tilde_06_notch;
        out1[(Index)i] = tmp[0];
        out2[(Index)i] = tmp[1];
        out3[(Index)i] = tmp[2];
        out4[(Index)i] = tmp[3];
    }

    this->svf_tilde_06__freq = __svf_tilde_06__freq;
    this->svf_tilde_06__q = __svf_tilde_06__q;
    this->svf_tilde_06_low = __svf_tilde_06_low;
    this->svf_tilde_06_band = __svf_tilde_06_band;
    this->svf_tilde_06_notch = __svf_tilde_06_notch;
    this->svf_tilde_06_high = __svf_tilde_06_high;
}

void dspexpr_07_perform(const Sample * in1, const Sample * in2, SampleValue * out1, Index n) {
    Index i;

    for (i = 0; i < (Index)n; i++) {
        out1[(Index)i] = in1[(Index)i] * in2[(Index)i];//#map:_###_obj_###_:1
    }
}

void dspexpr_06_perform(const Sample * in1, const Sample * in2, SampleValue * out1, Index n) {
    Index i;

    for (i = 0; i < (Index)n; i++) {
        out1[(Index)i] = in1[(Index)i] + in2[(Index)i];//#map:_###_obj_###_:1
    }
}

void dspexpr_05_perform(const Sample * in1, number in2, number in3, SampleValue * out1, Index n) {
    RNBO_UNUSED(in3);
    RNBO_UNUSED(in2);
    Index i;

    for (i = 0; i < (Index)n; i++) {
        out1[(Index)i] = (in1[(Index)i] > 1 ? 1 : (in1[(Index)i] < 0 ? 0 : in1[(Index)i]));//#map:_###_obj_###_:1
    }
}

void svf_tilde_02_perform(
    const Sample * x,
    const Sample * freq,
    const Sample * q,
    SampleValue * out1,
    SampleValue * out2,
    SampleValue * out3,
    SampleValue * out4,
    Index n
) {
    auto __svf_tilde_02_high = this->svf_tilde_02_high;
    auto __svf_tilde_02_notch = this->svf_tilde_02_notch;
    auto __svf_tilde_02_band = this->svf_tilde_02_band;
    auto __svf_tilde_02_low = this->svf_tilde_02_low;
    auto __svf_tilde_02__q = this->svf_tilde_02__q;
    auto __svf_tilde_02__freq = this->svf_tilde_02__freq;
    Index i;

    for (i = 0; i < (Index)n; i++) {
        if (this->svf_tilde_02_dFreq_next(freq[(Index)i]) != 0) {
            __svf_tilde_02__freq = (freq[(Index)i] > this->sr * 0.25 ? this->sr * 0.25 : (freq[(Index)i] < 0.0 ? 0.0 : freq[(Index)i]));
            __svf_tilde_02__freq = rnbo_sin(6.28318530717958647692 * __svf_tilde_02__freq / this->sr);
            __svf_tilde_02__freq = (__svf_tilde_02__freq > 0.99 ? 0.99 : (__svf_tilde_02__freq < 0.001 ? 0.001 : __svf_tilde_02__freq));
        }

        if (this->svf_tilde_02_dQ_next(q[(Index)i]) != 0) {
            __svf_tilde_02__q = (1. - q[(Index)i]) * 1.2;
            __svf_tilde_02__q = __svf_tilde_02__q * __svf_tilde_02__q;
            __svf_tilde_02__q = (__svf_tilde_02__q > 0.99 ? 0.99 : (__svf_tilde_02__q < 0.001 ? 0.001 : __svf_tilde_02__q));
        }

        __svf_tilde_02_low += __svf_tilde_02__freq * __svf_tilde_02_band;
        __svf_tilde_02_notch = x[(Index)i] - __svf_tilde_02__q * __svf_tilde_02_band;
        __svf_tilde_02_high = __svf_tilde_02_notch - __svf_tilde_02_low;
        __svf_tilde_02_band += __svf_tilde_02__freq * __svf_tilde_02_high - __svf_tilde_02_band * __svf_tilde_02_band * __svf_tilde_02_band * 0.0001;
        array<number, 4> tmp = {0, 0, 0, 0};
        tmp[0] = __svf_tilde_02_low;
        tmp[1] = __svf_tilde_02_high;
        tmp[2] = __svf_tilde_02_band;
        tmp[3] = __svf_tilde_02_notch;
        out1[(Index)i] = tmp[0];
        out2[(Index)i] = tmp[1];
        out3[(Index)i] = tmp[2];
        out4[(Index)i] = tmp[3];
    }

    this->svf_tilde_02__freq = __svf_tilde_02__freq;
    this->svf_tilde_02__q = __svf_tilde_02__q;
    this->svf_tilde_02_low = __svf_tilde_02_low;
    this->svf_tilde_02_band = __svf_tilde_02_band;
    this->svf_tilde_02_notch = __svf_tilde_02_notch;
    this->svf_tilde_02_high = __svf_tilde_02_high;
}

void scale_tilde_01_perform(
    const Sample * x,
    number lowin,
    number hiin,
    number lowout,
    number highout,
    number pow,
    SampleValue * out1,
    Index n
) {
    RNBO_UNUSED(pow);
    RNBO_UNUSED(highout);
    RNBO_UNUSED(lowout);
    RNBO_UNUSED(hiin);
    RNBO_UNUSED(lowin);
    auto inscale = this->safediv(1., 1 - -1);
    number outdiff = 5 - -5;
    Index i;

    for (i = 0; i < (Index)n; i++) {
        number value = (x[(Index)i] - -1) * inscale;
        value = value * outdiff + -5;
        out1[(Index)i] = value;
    }
}

void stackprotect_perform(Index n) {
    RNBO_UNUSED(n);
    auto __stackprotect_count = this->stackprotect_count;
    __stackprotect_count = 0;
    this->stackprotect_count = __stackprotect_count;
}

number svf_tilde_01_dFreq_next(number x) {
    number temp = (number)(x - this->svf_tilde_01_dFreq_prev);
    this->svf_tilde_01_dFreq_prev = x;
    return temp;
}

void svf_tilde_01_dFreq_dspsetup() {
    this->svf_tilde_01_dFreq_reset();
}

void svf_tilde_01_dFreq_reset() {
    this->svf_tilde_01_dFreq_prev = 0;
}

number svf_tilde_01_dQ_next(number x) {
    number temp = (number)(x - this->svf_tilde_01_dQ_prev);
    this->svf_tilde_01_dQ_prev = x;
    return temp;
}

void svf_tilde_01_dQ_dspsetup() {
    this->svf_tilde_01_dQ_reset();
}

void svf_tilde_01_dQ_reset() {
    this->svf_tilde_01_dQ_prev = 0;
}

void svf_tilde_01_reset() {
    this->svf_tilde_01_low = 0;
    this->svf_tilde_01_high = 0;
    this->svf_tilde_01_band = 0;
    this->svf_tilde_01_notch = 0;
    this->svf_tilde_01_dFreq_reset();
    this->svf_tilde_01_dQ_reset();
}

void svf_tilde_01_dspsetup(bool force) {
    if ((bool)(this->svf_tilde_01_setupDone) && (bool)(!(bool)(force)))
        return;

    this->svf_tilde_01_reset();
    this->svf_tilde_01_setupDone = true;
    this->svf_tilde_01_dFreq_dspsetup();
    this->svf_tilde_01_dQ_dspsetup();
}

void ip_01_init() {
    this->ip_01_lastValue = this->ip_01_value;
}

void ip_01_fillSigBuf() {
    if ((bool)(this->ip_01_sigbuf)) {
        SampleIndex k = (SampleIndex)(this->sampleOffsetIntoNextAudioBuffer);

        if (k >= (SampleIndex)(this->vs))
            k = (SampleIndex)(this->vs) - 1;

        for (SampleIndex i = (SampleIndex)(this->ip_01_lastIndex); i < k; i++) {
            if (this->ip_01_resetCount > 0) {
                this->ip_01_sigbuf[(Index)i] = 1;
                this->ip_01_resetCount--;
            } else {
                this->ip_01_sigbuf[(Index)i] = this->ip_01_lastValue;
            }
        }

        this->ip_01_lastIndex = k;
    }
}

void ip_01_dspsetup(bool force) {
    if ((bool)(this->ip_01_setupDone) && (bool)(!(bool)(force)))
        return;

    this->ip_01_lastIndex = 0;
    this->ip_01_setupDone = true;
}

number mtof_tilde_01_innerMtoF_next(number midivalue, number tuning) {
    if (midivalue == this->mtof_tilde_01_innerMtoF_lastInValue && tuning == this->mtof_tilde_01_innerMtoF_lastTuning)
        return this->mtof_tilde_01_innerMtoF_lastOutValue;

    this->mtof_tilde_01_innerMtoF_lastInValue = midivalue;
    this->mtof_tilde_01_innerMtoF_lastTuning = tuning;
    number result = 0;

    {
        result = rnbo_exp(.057762265 * (midivalue - 69.0));
    }

    this->mtof_tilde_01_innerMtoF_lastOutValue = tuning * result;
    return this->mtof_tilde_01_innerMtoF_lastOutValue;
}

void mtof_tilde_01_innerMtoF_reset() {
    this->mtof_tilde_01_innerMtoF_lastInValue = 0;
    this->mtof_tilde_01_innerMtoF_lastOutValue = 0;
    this->mtof_tilde_01_innerMtoF_lastTuning = 0;
}

void mtof_tilde_01_innerScala_mid(Int v) {
    this->mtof_tilde_01_innerScala_kbmMid = v;
    this->mtof_tilde_01_innerScala_updateRefFreq();
}

void mtof_tilde_01_innerScala_ref(Int v) {
    this->mtof_tilde_01_innerScala_kbmRefNum = v;
    this->mtof_tilde_01_innerScala_updateRefFreq();
}

void mtof_tilde_01_innerScala_base(number v) {
    this->mtof_tilde_01_innerScala_kbmRefFreq = v;
    this->mtof_tilde_01_innerScala_updateRefFreq();
}

void mtof_tilde_01_innerScala_init() {
    list sclValid = {
        12,
        100,
        0,
        200,
        0,
        300,
        0,
        400,
        0,
        500,
        0,
        600,
        0,
        700,
        0,
        800,
        0,
        900,
        0,
        1000,
        0,
        1100,
        0,
        2,
        1
    };

    this->mtof_tilde_01_innerScala_updateScale(sclValid);
}

template<typename LISTTYPE = list> void mtof_tilde_01_innerScala_update(const LISTTYPE& scale, const LISTTYPE& map) {
    if (scale->length > 0) {
        this->mtof_tilde_01_innerScala_updateScale(scale);
    }

    if (map->length > 0) {
        this->mtof_tilde_01_innerScala_updateMap(map);
    }
}

number mtof_tilde_01_innerScala_mtof(number note) {
    if ((bool)(this->mtof_tilde_01_innerScala_lastValid) && this->mtof_tilde_01_innerScala_lastNote == note) {
        return this->mtof_tilde_01_innerScala_lastFreq;
    }

    array<Int, 2> degoct = this->mtof_tilde_01_innerScala_applyKBM(note);
    number out = 0;

    if (degoct[1] > 0) {
        out = this->mtof_tilde_01_innerScala_applySCL(degoct[0], fract(note), this->mtof_tilde_01_innerScala_refFreq);
    }

    this->mtof_tilde_01_innerScala_updateLast(note, out);
    return out;
}

number mtof_tilde_01_innerScala_ftom(number hz) {
    if (hz <= 0.0) {
        return 0.0;
    }

    if ((bool)(this->mtof_tilde_01_innerScala_lastValid) && this->mtof_tilde_01_innerScala_lastFreq == hz) {
        return this->mtof_tilde_01_innerScala_lastNote;
    }

    array<number, 2> df = this->mtof_tilde_01_innerScala_hztodeg(hz);
    Int degree = (Int)(df[0]);
    number frac = df[1];
    number out = 0;

    if (this->mtof_tilde_01_innerScala_kbmSize == 0) {
        out = this->mtof_tilde_01_innerScala_kbmMid + degree;
    } else {
        array<Int, 2> octdeg = this->mtof_tilde_01_innerScala_octdegree(degree, this->mtof_tilde_01_innerScala_kbmOctaveDegree);
        number oct = (number)(octdeg[0]);
        Int index = (Int)(octdeg[1]);
        Index entry = 0;

        for (Index i = 0; i < this->mtof_tilde_01_innerScala_kbmMapSize; i++) {
            if (index == this->mtof_tilde_01_innerScala_kbmValid[(Index)(i + this->mtof_tilde_01_innerScala_KBM_MAP_OFFSET)]) {
                entry = i;
                break;
            }
        }

        out = oct * this->mtof_tilde_01_innerScala_kbmSize + entry + this->mtof_tilde_01_innerScala_kbmMid;
    }

    out = out + frac;
    this->mtof_tilde_01_innerScala_updateLast(out, hz);
    return this->mtof_tilde_01_innerScala_lastNote;
}

template<typename LISTTYPE = list> Int mtof_tilde_01_innerScala_updateScale(const LISTTYPE& scl) {
    if (scl->length < 1) {
        return 0;
    }

    number sclDataEntries = scl[0] * 2 + 1;

    if (sclDataEntries <= scl->length) {
        this->mtof_tilde_01_innerScala_lastValid = false;
        this->mtof_tilde_01_innerScala_sclExpMul = {};
        number last = 1;

        for (Index i = 1; i < sclDataEntries; i += 2) {
            const number c = (const number)(scl[(Index)(i + 0)]);
            const number d = (const number)(scl[(Index)(i + 1)]);

            if (d <= 0) {
                last = c / (number)1200;
            } else {
                last = rnbo_log2(c / d);
            }

            this->mtof_tilde_01_innerScala_sclExpMul->push(last);
        }

        this->mtof_tilde_01_innerScala_sclOctaveMul = last;
        this->mtof_tilde_01_innerScala_sclEntryCount = (Int)(this->mtof_tilde_01_innerScala_sclExpMul->length);

        if (scl->length >= sclDataEntries + 3) {
            this->mtof_tilde_01_innerScala_kbmMid = (Int)(scl[(Index)(sclDataEntries + 2)]);
            this->mtof_tilde_01_innerScala_kbmRefNum = (Int)(scl[(Index)(sclDataEntries + 1)]);
            this->mtof_tilde_01_innerScala_kbmRefFreq = scl[(Index)(sclDataEntries + 0)];
            this->mtof_tilde_01_innerScala_kbmSize = (Int)(0);
        }

        this->mtof_tilde_01_innerScala_updateRefFreq();
        return 1;
    }

    return 0;
}

template<typename LISTTYPE = list> Int mtof_tilde_01_innerScala_updateMap(const LISTTYPE& kbm) {
    list _kbm = kbm;

    if (_kbm->length == 1 && _kbm[0] == 0.0) {
        _kbm = {0.0, 0.0, 0.0, 60.0, 69.0, 440.0};
    }

    if (_kbm->length >= 6 && _kbm[0] >= 0.0) {
        this->mtof_tilde_01_innerScala_lastValid = false;
        Index size = (Index)(_kbm[0]);
        Int octave = 12;

        if (_kbm->length > 6) {
            octave = (Int)(_kbm[6]);
        }

        if (size > 0 && _kbm->length < this->mtof_tilde_01_innerScala_KBM_MAP_OFFSET) {
            return 0;
        }

        this->mtof_tilde_01_innerScala_kbmSize = (Int)(size);
        this->mtof_tilde_01_innerScala_kbmMin = (Int)(_kbm[1]);
        this->mtof_tilde_01_innerScala_kbmMax = (Int)(_kbm[2]);
        this->mtof_tilde_01_innerScala_kbmMid = (Int)(_kbm[3]);
        this->mtof_tilde_01_innerScala_kbmRefNum = (Int)(_kbm[4]);
        this->mtof_tilde_01_innerScala_kbmRefFreq = _kbm[5];
        this->mtof_tilde_01_innerScala_kbmOctaveDegree = octave;
        this->mtof_tilde_01_innerScala_kbmValid = _kbm;
        this->mtof_tilde_01_innerScala_kbmMapSize = (_kbm->length - this->mtof_tilde_01_innerScala_KBM_MAP_OFFSET > _kbm->length ? _kbm->length : (_kbm->length - this->mtof_tilde_01_innerScala_KBM_MAP_OFFSET < 0 ? 0 : _kbm->length - this->mtof_tilde_01_innerScala_KBM_MAP_OFFSET));
        this->mtof_tilde_01_innerScala_updateRefFreq();
        return 1;
    }

    return 0;
}

void mtof_tilde_01_innerScala_updateLast(number note, number freq) {
    this->mtof_tilde_01_innerScala_lastValid = true;
    this->mtof_tilde_01_innerScala_lastNote = note;
    this->mtof_tilde_01_innerScala_lastFreq = freq;
}

array<number, 2> mtof_tilde_01_innerScala_hztodeg(number hz) {
    number hza = rnbo_abs(hz);

    number octave = rnbo_floor(
        rnbo_log2(hza / this->mtof_tilde_01_innerScala_refFreq) / this->mtof_tilde_01_innerScala_sclOctaveMul
    );

    Int i = 0;
    number frac = 0;
    number n = 0;

    for (; i < this->mtof_tilde_01_innerScala_sclEntryCount; i++) {
        number c = this->mtof_tilde_01_innerScala_applySCLOctIndex(octave, i + 0, 0.0, this->mtof_tilde_01_innerScala_refFreq);
        n = this->mtof_tilde_01_innerScala_applySCLOctIndex(octave, i + 1, 0.0, this->mtof_tilde_01_innerScala_refFreq);

        if (c <= hza && hza < n) {
            if (c != hza) {
                frac = rnbo_log2(hza / c) / rnbo_log2(n / c);
            }

            break;
        }
    }

    if (i == this->mtof_tilde_01_innerScala_sclEntryCount && n != hza) {
        number c = n;
        n = this->mtof_tilde_01_innerScala_applySCLOctIndex(octave + 1, 0, 0.0, this->mtof_tilde_01_innerScala_refFreq);
        frac = rnbo_log2(hza / c) / rnbo_log2(n / c);
    }

    number deg = i + octave * this->mtof_tilde_01_innerScala_sclEntryCount;

    {
        deg = rnbo_fround((deg + frac) * 1 / (number)1) * 1;
        frac = 0.0;
    }

    return {deg, frac};
}

array<Int, 2> mtof_tilde_01_innerScala_octdegree(Int degree, Int count) {
    Int octave = 0;
    Int index = 0;

    if (degree < 0) {
        octave = -(1 + (-1 - degree) / count);
        index = -degree % count;

        if (index > 0) {
            index = count - index;
        }
    } else {
        octave = degree / count;
        index = degree % count;
    }

    return {octave, index};
}

array<Int, 2> mtof_tilde_01_innerScala_applyKBM(number note) {
    if ((this->mtof_tilde_01_innerScala_kbmMin == this->mtof_tilde_01_innerScala_kbmMax && this->mtof_tilde_01_innerScala_kbmMax == 0) || (note >= this->mtof_tilde_01_innerScala_kbmMin && note <= this->mtof_tilde_01_innerScala_kbmMax)) {
        Int degree = (Int)(rnbo_floor(note - this->mtof_tilde_01_innerScala_kbmMid));

        if (this->mtof_tilde_01_innerScala_kbmSize == 0) {
            return {degree, 1};
        }

        array<Int, 2> octdeg = this->mtof_tilde_01_innerScala_octdegree(degree, this->mtof_tilde_01_innerScala_kbmSize);
        Int octave = (Int)(octdeg[0]);
        Index index = (Index)(octdeg[1]);

        if (this->mtof_tilde_01_innerScala_kbmMapSize > index) {
            degree = (Int)(this->mtof_tilde_01_innerScala_kbmValid[(Index)(this->mtof_tilde_01_innerScala_KBM_MAP_OFFSET + index)]);

            if (degree >= 0) {
                return {degree + octave * this->mtof_tilde_01_innerScala_kbmOctaveDegree, 1};
            }
        }
    }

    return {-1, 0};
}

number mtof_tilde_01_innerScala_applySCL(Int degree, number frac, number refFreq) {
    array<Int, 2> octdeg = this->mtof_tilde_01_innerScala_octdegree(degree, this->mtof_tilde_01_innerScala_sclEntryCount);
    return this->mtof_tilde_01_innerScala_applySCLOctIndex(octdeg[0], octdeg[1], frac, refFreq);
}

number mtof_tilde_01_innerScala_applySCLOctIndex(number octave, Int index, number frac, number refFreq) {
    number p = 0;

    if (index > 0) {
        p = this->mtof_tilde_01_innerScala_sclExpMul[(Index)(index - 1)];
    }

    if (frac > 0) {
        p = this->linearinterp(frac, p, this->mtof_tilde_01_innerScala_sclExpMul[(Index)index]);
    } else if (frac < 0) {
        p = this->linearinterp(-frac, this->mtof_tilde_01_innerScala_sclExpMul[(Index)index], p);
    }

    return refFreq * rnbo_pow(2, p + octave * this->mtof_tilde_01_innerScala_sclOctaveMul);
}

void mtof_tilde_01_innerScala_updateRefFreq() {
    this->mtof_tilde_01_innerScala_lastValid = false;
    Int refOffset = (Int)(this->mtof_tilde_01_innerScala_kbmRefNum - this->mtof_tilde_01_innerScala_kbmMid);

    if (refOffset == 0) {
        this->mtof_tilde_01_innerScala_refFreq = this->mtof_tilde_01_innerScala_kbmRefFreq;
    } else {
        Int base = (Int)(this->mtof_tilde_01_innerScala_kbmSize);

        if (base < 1) {
            base = this->mtof_tilde_01_innerScala_sclEntryCount;
        }

        array<Int, 2> octdeg = this->mtof_tilde_01_innerScala_octdegree(refOffset, base);
        number oct = (number)(octdeg[0]);
        Int index = (Int)(octdeg[1]);

        if (base > 0) {
            oct = oct + rnbo_floor(index / base);
            index = index % base;
        }

        if (index >= 0 && index < this->mtof_tilde_01_innerScala_kbmSize) {
            if (index < (Int)(this->mtof_tilde_01_innerScala_kbmMapSize)) {
                index = (Int)(this->mtof_tilde_01_innerScala_kbmValid[(Index)((Index)(index) + this->mtof_tilde_01_innerScala_KBM_MAP_OFFSET)]);
            } else {
                index = -1;
            }
        }

        if (index < 0 || index > (Int)(this->mtof_tilde_01_innerScala_sclExpMul->length))
            {} else {
            number p = 0;

            if (index > 0) {
                p = this->mtof_tilde_01_innerScala_sclExpMul[(Index)(index - 1)];
            }

            this->mtof_tilde_01_innerScala_refFreq = this->mtof_tilde_01_innerScala_kbmRefFreq / rnbo_pow(2, p + oct * this->mtof_tilde_01_innerScala_sclOctaveMul);
        }
    }
}

void mtof_tilde_01_innerScala_reset() {
    this->mtof_tilde_01_innerScala_lastValid = false;
    this->mtof_tilde_01_innerScala_lastNote = 0;
    this->mtof_tilde_01_innerScala_lastFreq = 0;
    this->mtof_tilde_01_innerScala_sclEntryCount = 0;
    this->mtof_tilde_01_innerScala_sclOctaveMul = 1;
    this->mtof_tilde_01_innerScala_sclExpMul = {};
    this->mtof_tilde_01_innerScala_kbmValid = {0, 0, 0, 60, 69, 440};
    this->mtof_tilde_01_innerScala_kbmMid = 60;
    this->mtof_tilde_01_innerScala_kbmRefNum = 69;
    this->mtof_tilde_01_innerScala_kbmRefFreq = 440;
    this->mtof_tilde_01_innerScala_kbmSize = 0;
    this->mtof_tilde_01_innerScala_kbmMin = 0;
    this->mtof_tilde_01_innerScala_kbmMax = 0;
    this->mtof_tilde_01_innerScala_kbmOctaveDegree = 12;
    this->mtof_tilde_01_innerScala_kbmMapSize = 0;
    this->mtof_tilde_01_innerScala_refFreq = 261.63;
}

void mtof_tilde_01_init() {
    this->mtof_tilde_01_innerScala_update(this->mtof_tilde_01_scale, this->mtof_tilde_01_map);
}

void param_01_getPresetValue(PatcherStateInterface& preset) {
    preset["value"] = this->param_01_value;
}

void param_01_setPresetValue(PatcherStateInterface& preset) {
    if ((bool)(stateIsEmpty(preset)))
        return;

    this->param_01_value_set(preset["value"]);
}

number svf_tilde_02_dFreq_next(number x) {
    number temp = (number)(x - this->svf_tilde_02_dFreq_prev);
    this->svf_tilde_02_dFreq_prev = x;
    return temp;
}

void svf_tilde_02_dFreq_dspsetup() {
    this->svf_tilde_02_dFreq_reset();
}

void svf_tilde_02_dFreq_reset() {
    this->svf_tilde_02_dFreq_prev = 0;
}

number svf_tilde_02_dQ_next(number x) {
    number temp = (number)(x - this->svf_tilde_02_dQ_prev);
    this->svf_tilde_02_dQ_prev = x;
    return temp;
}

void svf_tilde_02_dQ_dspsetup() {
    this->svf_tilde_02_dQ_reset();
}

void svf_tilde_02_dQ_reset() {
    this->svf_tilde_02_dQ_prev = 0;
}

void svf_tilde_02_reset() {
    this->svf_tilde_02_low = 0;
    this->svf_tilde_02_high = 0;
    this->svf_tilde_02_band = 0;
    this->svf_tilde_02_notch = 0;
    this->svf_tilde_02_dFreq_reset();
    this->svf_tilde_02_dQ_reset();
}

void svf_tilde_02_dspsetup(bool force) {
    if ((bool)(this->svf_tilde_02_setupDone) && (bool)(!(bool)(force)))
        return;

    this->svf_tilde_02_reset();
    this->svf_tilde_02_setupDone = true;
    this->svf_tilde_02_dFreq_dspsetup();
    this->svf_tilde_02_dQ_dspsetup();
}

number saw_tilde_01_dcblocker_next(number x, number gain) {
    number y = x - this->saw_tilde_01_dcblocker_xm1 + this->saw_tilde_01_dcblocker_ym1 * gain;
    this->saw_tilde_01_dcblocker_xm1 = x;
    this->saw_tilde_01_dcblocker_ym1 = y;
    return y;
}

void saw_tilde_01_dcblocker_reset() {
    this->saw_tilde_01_dcblocker_xm1 = 0;
    this->saw_tilde_01_dcblocker_ym1 = 0;
}

void saw_tilde_01_dcblocker_dspsetup() {
    this->saw_tilde_01_dcblocker_reset();
}

void saw_tilde_01_dspsetup(bool force) {
    if ((bool)(this->saw_tilde_01_setupDone) && (bool)(!(bool)(force)))
        return;

    this->saw_tilde_01_setupDone = true;
    this->saw_tilde_01_dcblocker_dspsetup();
}

number svf_tilde_03_dFreq_next(number x) {
    number temp = (number)(x - this->svf_tilde_03_dFreq_prev);
    this->svf_tilde_03_dFreq_prev = x;
    return temp;
}

void svf_tilde_03_dFreq_dspsetup() {
    this->svf_tilde_03_dFreq_reset();
}

void svf_tilde_03_dFreq_reset() {
    this->svf_tilde_03_dFreq_prev = 0;
}

number svf_tilde_03_dQ_next(number x) {
    number temp = (number)(x - this->svf_tilde_03_dQ_prev);
    this->svf_tilde_03_dQ_prev = x;
    return temp;
}

void svf_tilde_03_dQ_dspsetup() {
    this->svf_tilde_03_dQ_reset();
}

void svf_tilde_03_dQ_reset() {
    this->svf_tilde_03_dQ_prev = 0;
}

void svf_tilde_03_reset() {
    this->svf_tilde_03_low = 0;
    this->svf_tilde_03_high = 0;
    this->svf_tilde_03_band = 0;
    this->svf_tilde_03_notch = 0;
    this->svf_tilde_03_dFreq_reset();
    this->svf_tilde_03_dQ_reset();
}

void svf_tilde_03_dspsetup(bool force) {
    if ((bool)(this->svf_tilde_03_setupDone) && (bool)(!(bool)(force)))
        return;

    this->svf_tilde_03_reset();
    this->svf_tilde_03_setupDone = true;
    this->svf_tilde_03_dFreq_dspsetup();
    this->svf_tilde_03_dQ_dspsetup();
}

void ip_02_init() {
    this->ip_02_lastValue = this->ip_02_value;
}

void ip_02_fillSigBuf() {
    if ((bool)(this->ip_02_sigbuf)) {
        SampleIndex k = (SampleIndex)(this->sampleOffsetIntoNextAudioBuffer);

        if (k >= (SampleIndex)(this->vs))
            k = (SampleIndex)(this->vs) - 1;

        for (SampleIndex i = (SampleIndex)(this->ip_02_lastIndex); i < k; i++) {
            if (this->ip_02_resetCount > 0) {
                this->ip_02_sigbuf[(Index)i] = 1;
                this->ip_02_resetCount--;
            } else {
                this->ip_02_sigbuf[(Index)i] = this->ip_02_lastValue;
            }
        }

        this->ip_02_lastIndex = k;
    }
}

void ip_02_dspsetup(bool force) {
    if ((bool)(this->ip_02_setupDone) && (bool)(!(bool)(force)))
        return;

    this->ip_02_lastIndex = 0;
    this->ip_02_setupDone = true;
}

void param_02_getPresetValue(PatcherStateInterface& preset) {
    preset["value"] = this->param_02_value;
}

void param_02_setPresetValue(PatcherStateInterface& preset) {
    if ((bool)(stateIsEmpty(preset)))
        return;

    this->param_02_value_set(preset["value"]);
}

number svf_tilde_04_dFreq_next(number x) {
    number temp = (number)(x - this->svf_tilde_04_dFreq_prev);
    this->svf_tilde_04_dFreq_prev = x;
    return temp;
}

void svf_tilde_04_dFreq_dspsetup() {
    this->svf_tilde_04_dFreq_reset();
}

void svf_tilde_04_dFreq_reset() {
    this->svf_tilde_04_dFreq_prev = 0;
}

number svf_tilde_04_dQ_next(number x) {
    number temp = (number)(x - this->svf_tilde_04_dQ_prev);
    this->svf_tilde_04_dQ_prev = x;
    return temp;
}

void svf_tilde_04_dQ_dspsetup() {
    this->svf_tilde_04_dQ_reset();
}

void svf_tilde_04_dQ_reset() {
    this->svf_tilde_04_dQ_prev = 0;
}

void svf_tilde_04_reset() {
    this->svf_tilde_04_low = 0;
    this->svf_tilde_04_high = 0;
    this->svf_tilde_04_band = 0;
    this->svf_tilde_04_notch = 0;
    this->svf_tilde_04_dFreq_reset();
    this->svf_tilde_04_dQ_reset();
}

void svf_tilde_04_dspsetup(bool force) {
    if ((bool)(this->svf_tilde_04_setupDone) && (bool)(!(bool)(force)))
        return;

    this->svf_tilde_04_reset();
    this->svf_tilde_04_setupDone = true;
    this->svf_tilde_04_dFreq_dspsetup();
    this->svf_tilde_04_dQ_dspsetup();
}

void ip_03_init() {
    this->ip_03_lastValue = this->ip_03_value;
}

void ip_03_fillSigBuf() {
    if ((bool)(this->ip_03_sigbuf)) {
        SampleIndex k = (SampleIndex)(this->sampleOffsetIntoNextAudioBuffer);

        if (k >= (SampleIndex)(this->vs))
            k = (SampleIndex)(this->vs) - 1;

        for (SampleIndex i = (SampleIndex)(this->ip_03_lastIndex); i < k; i++) {
            if (this->ip_03_resetCount > 0) {
                this->ip_03_sigbuf[(Index)i] = 1;
                this->ip_03_resetCount--;
            } else {
                this->ip_03_sigbuf[(Index)i] = this->ip_03_lastValue;
            }
        }

        this->ip_03_lastIndex = k;
    }
}

void ip_03_dspsetup(bool force) {
    if ((bool)(this->ip_03_setupDone) && (bool)(!(bool)(force)))
        return;

    this->ip_03_lastIndex = 0;
    this->ip_03_setupDone = true;
}

void param_03_getPresetValue(PatcherStateInterface& preset) {
    preset["value"] = this->param_03_value;
}

void param_03_setPresetValue(PatcherStateInterface& preset) {
    if ((bool)(stateIsEmpty(preset)))
        return;

    this->param_03_value_set(preset["value"]);
}

number svf_tilde_05_dFreq_next(number x) {
    number temp = (number)(x - this->svf_tilde_05_dFreq_prev);
    this->svf_tilde_05_dFreq_prev = x;
    return temp;
}

void svf_tilde_05_dFreq_dspsetup() {
    this->svf_tilde_05_dFreq_reset();
}

void svf_tilde_05_dFreq_reset() {
    this->svf_tilde_05_dFreq_prev = 0;
}

number svf_tilde_05_dQ_next(number x) {
    number temp = (number)(x - this->svf_tilde_05_dQ_prev);
    this->svf_tilde_05_dQ_prev = x;
    return temp;
}

void svf_tilde_05_dQ_dspsetup() {
    this->svf_tilde_05_dQ_reset();
}

void svf_tilde_05_dQ_reset() {
    this->svf_tilde_05_dQ_prev = 0;
}

void svf_tilde_05_reset() {
    this->svf_tilde_05_low = 0;
    this->svf_tilde_05_high = 0;
    this->svf_tilde_05_band = 0;
    this->svf_tilde_05_notch = 0;
    this->svf_tilde_05_dFreq_reset();
    this->svf_tilde_05_dQ_reset();
}

void svf_tilde_05_dspsetup(bool force) {
    if ((bool)(this->svf_tilde_05_setupDone) && (bool)(!(bool)(force)))
        return;

    this->svf_tilde_05_reset();
    this->svf_tilde_05_setupDone = true;
    this->svf_tilde_05_dFreq_dspsetup();
    this->svf_tilde_05_dQ_dspsetup();
}

void ip_04_init() {
    this->ip_04_lastValue = this->ip_04_value;
}

void ip_04_fillSigBuf() {
    if ((bool)(this->ip_04_sigbuf)) {
        SampleIndex k = (SampleIndex)(this->sampleOffsetIntoNextAudioBuffer);

        if (k >= (SampleIndex)(this->vs))
            k = (SampleIndex)(this->vs) - 1;

        for (SampleIndex i = (SampleIndex)(this->ip_04_lastIndex); i < k; i++) {
            if (this->ip_04_resetCount > 0) {
                this->ip_04_sigbuf[(Index)i] = 1;
                this->ip_04_resetCount--;
            } else {
                this->ip_04_sigbuf[(Index)i] = this->ip_04_lastValue;
            }
        }

        this->ip_04_lastIndex = k;
    }
}

void ip_04_dspsetup(bool force) {
    if ((bool)(this->ip_04_setupDone) && (bool)(!(bool)(force)))
        return;

    this->ip_04_lastIndex = 0;
    this->ip_04_setupDone = true;
}

void param_04_getPresetValue(PatcherStateInterface& preset) {
    preset["value"] = this->param_04_value;
}

void param_04_setPresetValue(PatcherStateInterface& preset) {
    if ((bool)(stateIsEmpty(preset)))
        return;

    this->param_04_value_set(preset["value"]);
}

number svf_tilde_06_dFreq_next(number x) {
    number temp = (number)(x - this->svf_tilde_06_dFreq_prev);
    this->svf_tilde_06_dFreq_prev = x;
    return temp;
}

void svf_tilde_06_dFreq_dspsetup() {
    this->svf_tilde_06_dFreq_reset();
}

void svf_tilde_06_dFreq_reset() {
    this->svf_tilde_06_dFreq_prev = 0;
}

number svf_tilde_06_dQ_next(number x) {
    number temp = (number)(x - this->svf_tilde_06_dQ_prev);
    this->svf_tilde_06_dQ_prev = x;
    return temp;
}

void svf_tilde_06_dQ_dspsetup() {
    this->svf_tilde_06_dQ_reset();
}

void svf_tilde_06_dQ_reset() {
    this->svf_tilde_06_dQ_prev = 0;
}

void svf_tilde_06_reset() {
    this->svf_tilde_06_low = 0;
    this->svf_tilde_06_high = 0;
    this->svf_tilde_06_band = 0;
    this->svf_tilde_06_notch = 0;
    this->svf_tilde_06_dFreq_reset();
    this->svf_tilde_06_dQ_reset();
}

void svf_tilde_06_dspsetup(bool force) {
    if ((bool)(this->svf_tilde_06_setupDone) && (bool)(!(bool)(force)))
        return;

    this->svf_tilde_06_reset();
    this->svf_tilde_06_setupDone = true;
    this->svf_tilde_06_dFreq_dspsetup();
    this->svf_tilde_06_dQ_dspsetup();
}

void ip_05_init() {
    this->ip_05_lastValue = this->ip_05_value;
}

void ip_05_fillSigBuf() {
    if ((bool)(this->ip_05_sigbuf)) {
        SampleIndex k = (SampleIndex)(this->sampleOffsetIntoNextAudioBuffer);

        if (k >= (SampleIndex)(this->vs))
            k = (SampleIndex)(this->vs) - 1;

        for (SampleIndex i = (SampleIndex)(this->ip_05_lastIndex); i < k; i++) {
            if (this->ip_05_resetCount > 0) {
                this->ip_05_sigbuf[(Index)i] = 1;
                this->ip_05_resetCount--;
            } else {
                this->ip_05_sigbuf[(Index)i] = this->ip_05_lastValue;
            }
        }

        this->ip_05_lastIndex = k;
    }
}

void ip_05_dspsetup(bool force) {
    if ((bool)(this->ip_05_setupDone) && (bool)(!(bool)(force)))
        return;

    this->ip_05_lastIndex = 0;
    this->ip_05_setupDone = true;
}

void param_05_getPresetValue(PatcherStateInterface& preset) {
    preset["value"] = this->param_05_value;
}

void param_05_setPresetValue(PatcherStateInterface& preset) {
    if ((bool)(stateIsEmpty(preset)))
        return;

    this->param_05_value_set(preset["value"]);
}

void globaltransport_advance() {}

void globaltransport_dspsetup(bool ) {}

bool stackprotect_check() {
    this->stackprotect_count++;

    if (this->stackprotect_count > 128) {
        console->log("STACK OVERFLOW DETECTED - stopped processing branch !");
        return true;
    }

    return false;
}

Index getPatcherSerial() const {
    return 0;
}

void sendParameter(ParameterIndex index, bool ignoreValue) {
    this->getEngine()->notifyParameterValueChanged(index, (ignoreValue ? 0 : this->getParameterValue(index)), ignoreValue);
}

void scheduleParamInit(ParameterIndex index, Index order) {
    this->paramInitIndices->push(index);
    this->paramInitOrder->push(order);
}

void processParamInitEvents() {
    this->listquicksort(
        this->paramInitOrder,
        this->paramInitIndices,
        0,
        (int)(this->paramInitOrder->length - 1),
        true
    );

    for (Index i = 0; i < this->paramInitOrder->length; i++) {
        this->getEngine()->scheduleParameterBang(this->paramInitIndices[i], 0);
    }
}

void updateTime(MillisecondTime time, EXTERNALENGINE* engine, bool inProcess = false) {
    RNBO_UNUSED(inProcess);
    RNBO_UNUSED(engine);
    this->_currentTime = time;
    auto offset = rnbo_fround(this->msToSamps(time - this->getEngine()->getCurrentTime(), this->sr));

    if (offset >= (SampleIndex)(this->vs))
        offset = (SampleIndex)(this->vs) - 1;

    if (offset < 0)
        offset = 0;

    this->sampleOffsetIntoNextAudioBuffer = (Index)(offset);
}

void assign_defaults()
{
    scale_tilde_01_x = 0;
    scale_tilde_01_lowin = -1;
    scale_tilde_01_hiin = 1;
    scale_tilde_01_lowout = -5;
    scale_tilde_01_highout = 5;
    scale_tilde_01_pow = 1;
    dspexpr_01_in1 = 0;
    dspexpr_01_in2 = 0;
    svf_tilde_01_x = 0;
    svf_tilde_01_freq = 20;
    svf_tilde_01_q = 0;
    ip_01_value = 0;
    ip_01_impulse = 0;
    mtof_tilde_01_midivalue = 0;
    mtof_tilde_01_base = 440;
    param_01_value = 60;
    svf_tilde_02_x = 0;
    svf_tilde_02_freq = 0;
    svf_tilde_02_q = 0;
    saw_tilde_01_frequency = 0;
    saw_tilde_01_syncPhase = 0;
    dspexpr_02_in1 = 0;
    dspexpr_02_in2 = 12;
    dspexpr_03_in1 = 0;
    dspexpr_03_in2 = 0;
    svf_tilde_03_x = 0;
    svf_tilde_03_freq = 20;
    svf_tilde_03_q = 0;
    ip_02_value = 0;
    ip_02_impulse = 0;
    param_02_value = 400;
    dspexpr_04_in1 = 0;
    dspexpr_04_in2 = 0;
    scale_tilde_02_x = 0;
    scale_tilde_02_lowin = -5;
    scale_tilde_02_hiin = 5;
    scale_tilde_02_lowout = -10000;
    scale_tilde_02_highout = 10000;
    scale_tilde_02_pow = 1;
    svf_tilde_04_x = 0;
    svf_tilde_04_freq = 20;
    svf_tilde_04_q = 0;
    ip_03_value = 0;
    ip_03_impulse = 0;
    param_03_value = 0;
    dspexpr_05_in1 = 0;
    dspexpr_05_in2 = 0;
    dspexpr_05_in3 = 1;
    dspexpr_06_in1 = 0;
    dspexpr_06_in2 = 0;
    svf_tilde_05_x = 0;
    svf_tilde_05_freq = 20;
    svf_tilde_05_q = 0;
    ip_04_value = 0;
    ip_04_impulse = 0;
    param_04_value = 0.5;
    dspexpr_07_in1 = 0;
    dspexpr_07_in2 = 0;
    scale_tilde_03_x = 0;
    scale_tilde_03_lowin = -5;
    scale_tilde_03_hiin = 5;
    scale_tilde_03_lowout = 0;
    scale_tilde_03_highout = 1;
    scale_tilde_03_pow = 1;
    svf_tilde_06_x = 0;
    svf_tilde_06_freq = 20;
    svf_tilde_06_q = 0;
    ip_05_value = 0;
    ip_05_impulse = 0;
    param_05_value = 0;
    _currentTime = 0;
    audioProcessSampleCount = 0;
    sampleOffsetIntoNextAudioBuffer = 0;
    zeroBuffer = nullptr;
    dummyBuffer = nullptr;
    signals[0] = nullptr;
    signals[1] = nullptr;
    signals[2] = nullptr;
    signals[3] = nullptr;
    signals[4] = nullptr;
    signals[5] = nullptr;
    didAllocateSignals = 0;
    vs = 0;
    maxvs = 0;
    sr = 44100;
    invsr = 0.000022675736961451248;
    svf_tilde_01_low = 0;
    svf_tilde_01_high = 0;
    svf_tilde_01_band = 0;
    svf_tilde_01_notch = 0;
    svf_tilde_01__freq = 0.001;
    svf_tilde_01__q = 0.001;
    svf_tilde_01_dFreq_prev = 0;
    svf_tilde_01_dQ_prev = 0;
    svf_tilde_01_setupDone = false;
    ip_01_lastIndex = 0;
    ip_01_lastValue = 0;
    ip_01_resetCount = 0;
    ip_01_sigbuf = nullptr;
    ip_01_setupDone = false;
    mtof_tilde_01_innerMtoF_lastInValue = 0;
    mtof_tilde_01_innerMtoF_lastOutValue = 0;
    mtof_tilde_01_innerMtoF_lastTuning = 0;
    mtof_tilde_01_innerScala_lastValid = false;
    mtof_tilde_01_innerScala_lastNote = 0;
    mtof_tilde_01_innerScala_lastFreq = 0;
    mtof_tilde_01_innerScala_sclEntryCount = 0;
    mtof_tilde_01_innerScala_sclOctaveMul = 1;
    mtof_tilde_01_innerScala_kbmValid = { 0, 0, 0, 60, 69, 440 };
    mtof_tilde_01_innerScala_kbmMid = 60;
    mtof_tilde_01_innerScala_kbmRefNum = 69;
    mtof_tilde_01_innerScala_kbmRefFreq = 440;
    mtof_tilde_01_innerScala_kbmSize = 0;
    mtof_tilde_01_innerScala_kbmMin = 0;
    mtof_tilde_01_innerScala_kbmMax = 0;
    mtof_tilde_01_innerScala_kbmOctaveDegree = 12;
    mtof_tilde_01_innerScala_kbmMapSize = 0;
    mtof_tilde_01_innerScala_refFreq = 261.63;
    param_01_lastValue = 0;
    svf_tilde_02_low = 0;
    svf_tilde_02_high = 0;
    svf_tilde_02_band = 0;
    svf_tilde_02_notch = 0;
    svf_tilde_02__freq = 0.001;
    svf_tilde_02__q = 0.001;
    svf_tilde_02_dFreq_prev = 0;
    svf_tilde_02_dQ_prev = 0;
    svf_tilde_02_setupDone = false;
    saw_tilde_01_t = 0;
    saw_tilde_01_lastSyncPhase = 0;
    saw_tilde_01_lastSyncDiff = 0;
    saw_tilde_01_didSync = false;
    saw_tilde_01_dcblocker_xm1 = 0;
    saw_tilde_01_dcblocker_ym1 = 0;
    saw_tilde_01_setupDone = false;
    svf_tilde_03_low = 0;
    svf_tilde_03_high = 0;
    svf_tilde_03_band = 0;
    svf_tilde_03_notch = 0;
    svf_tilde_03__freq = 0.001;
    svf_tilde_03__q = 0.001;
    svf_tilde_03_dFreq_prev = 0;
    svf_tilde_03_dQ_prev = 0;
    svf_tilde_03_setupDone = false;
    ip_02_lastIndex = 0;
    ip_02_lastValue = 0;
    ip_02_resetCount = 0;
    ip_02_sigbuf = nullptr;
    ip_02_setupDone = false;
    param_02_lastValue = 0;
    svf_tilde_04_low = 0;
    svf_tilde_04_high = 0;
    svf_tilde_04_band = 0;
    svf_tilde_04_notch = 0;
    svf_tilde_04__freq = 0.001;
    svf_tilde_04__q = 0.001;
    svf_tilde_04_dFreq_prev = 0;
    svf_tilde_04_dQ_prev = 0;
    svf_tilde_04_setupDone = false;
    ip_03_lastIndex = 0;
    ip_03_lastValue = 0;
    ip_03_resetCount = 0;
    ip_03_sigbuf = nullptr;
    ip_03_setupDone = false;
    param_03_lastValue = 0;
    svf_tilde_05_low = 0;
    svf_tilde_05_high = 0;
    svf_tilde_05_band = 0;
    svf_tilde_05_notch = 0;
    svf_tilde_05__freq = 0.001;
    svf_tilde_05__q = 0.001;
    svf_tilde_05_dFreq_prev = 0;
    svf_tilde_05_dQ_prev = 0;
    svf_tilde_05_setupDone = false;
    ip_04_lastIndex = 0;
    ip_04_lastValue = 0;
    ip_04_resetCount = 0;
    ip_04_sigbuf = nullptr;
    ip_04_setupDone = false;
    param_04_lastValue = 0;
    svf_tilde_06_low = 0;
    svf_tilde_06_high = 0;
    svf_tilde_06_band = 0;
    svf_tilde_06_notch = 0;
    svf_tilde_06__freq = 0.001;
    svf_tilde_06__q = 0.001;
    svf_tilde_06_dFreq_prev = 0;
    svf_tilde_06_dQ_prev = 0;
    svf_tilde_06_setupDone = false;
    ip_05_lastIndex = 0;
    ip_05_lastValue = 0;
    ip_05_resetCount = 0;
    ip_05_sigbuf = nullptr;
    ip_05_setupDone = false;
    param_05_lastValue = 0;
    globaltransport_tempo = nullptr;
    globaltransport_state = nullptr;
    stackprotect_count = 0;
    _voiceIndex = 0;
    _noteNumber = 0;
    isMuted = 1;
}

    // data ref strings
    struct DataRefStrings {
    	static constexpr auto& name0 = "RNBODefaultMtofLookupTable256";
    	static constexpr auto& file0 = "";
    	static constexpr auto& tag0 = "buffer~";
    	DataRefStrings* operator->() { return this; }
    	const DataRefStrings* operator->() const { return this; }
    };

    DataRefStrings dataRefStrings;

// member variables

    number scale_tilde_01_x;
    number scale_tilde_01_lowin;
    number scale_tilde_01_hiin;
    number scale_tilde_01_lowout;
    number scale_tilde_01_highout;
    number scale_tilde_01_pow;
    number dspexpr_01_in1;
    number dspexpr_01_in2;
    number svf_tilde_01_x;
    number svf_tilde_01_freq;
    number svf_tilde_01_q;
    number ip_01_value;
    number ip_01_impulse;
    number mtof_tilde_01_midivalue;
    list mtof_tilde_01_scale;
    list mtof_tilde_01_map;
    number mtof_tilde_01_base;
    number param_01_value;
    number svf_tilde_02_x;
    number svf_tilde_02_freq;
    number svf_tilde_02_q;
    number saw_tilde_01_frequency;
    number saw_tilde_01_syncPhase;
    number dspexpr_02_in1;
    number dspexpr_02_in2;
    number dspexpr_03_in1;
    number dspexpr_03_in2;
    number svf_tilde_03_x;
    number svf_tilde_03_freq;
    number svf_tilde_03_q;
    number ip_02_value;
    number ip_02_impulse;
    number param_02_value;
    number dspexpr_04_in1;
    number dspexpr_04_in2;
    number scale_tilde_02_x;
    number scale_tilde_02_lowin;
    number scale_tilde_02_hiin;
    number scale_tilde_02_lowout;
    number scale_tilde_02_highout;
    number scale_tilde_02_pow;
    number svf_tilde_04_x;
    number svf_tilde_04_freq;
    number svf_tilde_04_q;
    number ip_03_value;
    number ip_03_impulse;
    number param_03_value;
    number dspexpr_05_in1;
    number dspexpr_05_in2;
    number dspexpr_05_in3;
    number dspexpr_06_in1;
    number dspexpr_06_in2;
    number svf_tilde_05_x;
    number svf_tilde_05_freq;
    number svf_tilde_05_q;
    number ip_04_value;
    number ip_04_impulse;
    number param_04_value;
    number dspexpr_07_in1;
    number dspexpr_07_in2;
    number scale_tilde_03_x;
    number scale_tilde_03_lowin;
    number scale_tilde_03_hiin;
    number scale_tilde_03_lowout;
    number scale_tilde_03_highout;
    number scale_tilde_03_pow;
    number svf_tilde_06_x;
    number svf_tilde_06_freq;
    number svf_tilde_06_q;
    number ip_05_value;
    number ip_05_impulse;
    number param_05_value;
    MillisecondTime _currentTime;
    ENGINE _internalEngine;
    UInt64 audioProcessSampleCount;
    Index sampleOffsetIntoNextAudioBuffer;
    signal zeroBuffer;
    signal dummyBuffer;
    SampleValue * signals[6];
    bool didAllocateSignals;
    Index vs;
    Index maxvs;
    number sr;
    number invsr;
    number svf_tilde_01_low;
    number svf_tilde_01_high;
    number svf_tilde_01_band;
    number svf_tilde_01_notch;
    number svf_tilde_01__freq;
    number svf_tilde_01__q;
    number svf_tilde_01_dFreq_prev;
    number svf_tilde_01_dQ_prev;
    bool svf_tilde_01_setupDone;
    SampleIndex ip_01_lastIndex;
    number ip_01_lastValue;
    SampleIndex ip_01_resetCount;
    signal ip_01_sigbuf;
    bool ip_01_setupDone;
    number mtof_tilde_01_innerMtoF_lastInValue;
    number mtof_tilde_01_innerMtoF_lastOutValue;
    number mtof_tilde_01_innerMtoF_lastTuning;
    SampleBufferRef mtof_tilde_01_innerMtoF_buffer;
    const Index mtof_tilde_01_innerScala_KBM_MAP_OFFSET = 7;
    bool mtof_tilde_01_innerScala_lastValid;
    number mtof_tilde_01_innerScala_lastNote;
    number mtof_tilde_01_innerScala_lastFreq;
    Int mtof_tilde_01_innerScala_sclEntryCount;
    number mtof_tilde_01_innerScala_sclOctaveMul;
    list mtof_tilde_01_innerScala_sclExpMul;
    list mtof_tilde_01_innerScala_kbmValid;
    Int mtof_tilde_01_innerScala_kbmMid;
    Int mtof_tilde_01_innerScala_kbmRefNum;
    number mtof_tilde_01_innerScala_kbmRefFreq;
    Int mtof_tilde_01_innerScala_kbmSize;
    Int mtof_tilde_01_innerScala_kbmMin;
    Int mtof_tilde_01_innerScala_kbmMax;
    Int mtof_tilde_01_innerScala_kbmOctaveDegree;
    Index mtof_tilde_01_innerScala_kbmMapSize;
    number mtof_tilde_01_innerScala_refFreq;
    number param_01_lastValue;
    number svf_tilde_02_low;
    number svf_tilde_02_high;
    number svf_tilde_02_band;
    number svf_tilde_02_notch;
    number svf_tilde_02__freq;
    number svf_tilde_02__q;
    number svf_tilde_02_dFreq_prev;
    number svf_tilde_02_dQ_prev;
    bool svf_tilde_02_setupDone;
    number saw_tilde_01_t;
    number saw_tilde_01_lastSyncPhase;
    number saw_tilde_01_lastSyncDiff;
    bool saw_tilde_01_didSync;
    number saw_tilde_01_dcblocker_xm1;
    number saw_tilde_01_dcblocker_ym1;
    bool saw_tilde_01_setupDone;
    number svf_tilde_03_low;
    number svf_tilde_03_high;
    number svf_tilde_03_band;
    number svf_tilde_03_notch;
    number svf_tilde_03__freq;
    number svf_tilde_03__q;
    number svf_tilde_03_dFreq_prev;
    number svf_tilde_03_dQ_prev;
    bool svf_tilde_03_setupDone;
    SampleIndex ip_02_lastIndex;
    number ip_02_lastValue;
    SampleIndex ip_02_resetCount;
    signal ip_02_sigbuf;
    bool ip_02_setupDone;
    number param_02_lastValue;
    number svf_tilde_04_low;
    number svf_tilde_04_high;
    number svf_tilde_04_band;
    number svf_tilde_04_notch;
    number svf_tilde_04__freq;
    number svf_tilde_04__q;
    number svf_tilde_04_dFreq_prev;
    number svf_tilde_04_dQ_prev;
    bool svf_tilde_04_setupDone;
    SampleIndex ip_03_lastIndex;
    number ip_03_lastValue;
    SampleIndex ip_03_resetCount;
    signal ip_03_sigbuf;
    bool ip_03_setupDone;
    number param_03_lastValue;
    number svf_tilde_05_low;
    number svf_tilde_05_high;
    number svf_tilde_05_band;
    number svf_tilde_05_notch;
    number svf_tilde_05__freq;
    number svf_tilde_05__q;
    number svf_tilde_05_dFreq_prev;
    number svf_tilde_05_dQ_prev;
    bool svf_tilde_05_setupDone;
    SampleIndex ip_04_lastIndex;
    number ip_04_lastValue;
    SampleIndex ip_04_resetCount;
    signal ip_04_sigbuf;
    bool ip_04_setupDone;
    number param_04_lastValue;
    number svf_tilde_06_low;
    number svf_tilde_06_high;
    number svf_tilde_06_band;
    number svf_tilde_06_notch;
    number svf_tilde_06__freq;
    number svf_tilde_06__q;
    number svf_tilde_06_dFreq_prev;
    number svf_tilde_06_dQ_prev;
    bool svf_tilde_06_setupDone;
    SampleIndex ip_05_lastIndex;
    number ip_05_lastValue;
    SampleIndex ip_05_resetCount;
    signal ip_05_sigbuf;
    bool ip_05_setupDone;
    number param_05_lastValue;
    signal globaltransport_tempo;
    signal globaltransport_state;
    number stackprotect_count;
    DataRef RNBODefaultMtofLookupTable256;
    Index _voiceIndex;
    Int _noteNumber;
    Index isMuted;
    indexlist paramInitIndices;
    indexlist paramInitOrder;
    bool _isInitialized = false;
};

static PatcherInterface* creaternbomatic()
{
    return new rnbomatic<EXTERNALENGINE>();
}

#ifndef RNBO_NO_PATCHERFACTORY
extern "C" PatcherFactoryFunctionPtr GetPatcherFactoryFunction()
#else
extern "C" PatcherFactoryFunctionPtr rnbomaticFactoryFunction()
#endif
{
    return creaternbomatic;
}

#ifndef RNBO_NO_PATCHERFACTORY
extern "C" void SetLogger(Logger* logger)
#else
void rnbomaticSetLogger(Logger* logger)
#endif
{
    console = logger;
}

} // end RNBO namespace

