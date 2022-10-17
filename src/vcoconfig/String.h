//
// Created by Paul Walker on 10/8/22.
//

#include "dsp/oscillators/StringOscillator.h"

#ifndef RACK_HACK_STRING_H
#define RACK_HACK_STRING_H

namespace sst::surgext_rack::vco
{
template <> VCOConfig<ot_string>::layout_t VCOConfig<ot_string>::getLayout()
{
    typedef VCO<ot_string> M;
    int cp = M::OSC_CTRL_PARAM_0;
    return {
        // clang-format off
        LayoutItem::createVCOKnob(M::PITCH_0, "PITCH", 0, 0),
        LayoutItem::createVCOKnob(cp+1, "EXCITER", 0, 1),
        LayoutItem::createVCOKnob(cp+2, "", 0, 2),
        LayoutItem::createVCOKnob(cp+3, "", 0, 3),
        LayoutItem::createVCOSpanLabel("S1 - DECAY - S2", 0, 2, 2),
        LayoutItem::createVCOPort(M::AUDIO_INPUT, "Audio", 1,0),
        LayoutItem::createVCOKnob(cp+4, "STIFF", 1, 1),
        LayoutItem::createVCOKnob(cp+5, "BALANCE", 1, 2),
        LayoutItem::createVCOKnob(cp+6, "DETUNE", 1, 3),
        // clang-format on
    };
}

template <> int VCOConfig<ot_string>::rightMenuParamId() { return 0; }
template <>
std::function<std::string(const std::string &)> VCOConfig<ot_string>::rightMenuTransformFunction()
{
    auto res = [](const std::string &s) {
        auto rv = s;
        auto pn = rv.find("PINK NOISE");
        if (pn != std::string::npos)
        {
            rv = rv.substr(0, pn) + "PINK NS";
        }
        pn = rv.find("CONSTANT ");
        if (pn != std::string::npos)
        {
            rv = std::string("CNS ") + rv.substr(pn + strlen("CONSTANT "));
        }
        return rv;
    };
    return res;
}
template <> constexpr bool VCOConfig<ot_string>::supportsAudioIn() { return true; }
template <> std::string VCOConfig<ot_string>::retriggerLabel() { return "TRIG"; }
template <> int VCOConfig<ot_string>::getMenuLightID() { return 0; }
template <> std::string VCOConfig<ot_string>::getMenuLightString() { return "2X"; }
template <> void VCOConfig<ot_string>::processLightParameters(VCO<ot_string> *m)
{
    auto l0 = (bool)(m->params[VCO<ot_string>::ARBITRARY_SWITCH_0 + 0].getValue() > 0.5);

    for (auto s : {m->oscstorage, m->oscstorage_display})
    {
        auto p = &(s->p[StringOscillator::str_exciter_level]);
        auto dt = p->deform_type & StringOscillator::os_all;
        if (l0 && dt != StringOscillator::os_twox)
        {
            p->deform_type =
                (p->deform_type & ~StringOscillator::os_all) | StringOscillator::os_twox;
            m->forceRespawnDueToExternality = true;
        }
        else if (!l0 && dt != StringOscillator::os_onex)
        {
            p->deform_type =
                (p->deform_type & ~StringOscillator::os_all) | StringOscillator::os_onex;
            m->forceRespawnDueToExternality = true;
        }
    }
}

template <> void VCOConfig<ot_string>::oscillatorSpecificSetup(VCO<ot_string> *m)
{
    for (auto s : {m->oscstorage, m->oscstorage_display})
    {
        s->p[StringOscillator::str_stiffness].deform_type = StringOscillator::filter_compensate;
    }
}
} // namespace sst::surgext_rack::vco
#endif // RACK_HACK_STRING_H