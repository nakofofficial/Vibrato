#pragma once

#include <JuceHeader.h>
// These will improve the readability of the connection definition

#define getT(Idx) template get<Idx>()
#define connectT(Idx, target) template connect<Idx>(target)
#define getParameterT(Idx) template getParameter<Idx>()
#define setParameterT(Idx, value) template setParameter<Idx>(value)
#define setParameterWT(Idx, value) template setWrapParameter<Idx>(value)
using namespace scriptnode;
using namespace snex;
using namespace snex::Types;

namespace Nakof_Vibrato_impl
{
// ==============================| Node & Parameter type declarations |==============================

template <int NV>
using oscillator_t = wrap::no_data<core::oscillator<NV>>;

DECLARE_PARAMETER_RANGE(peak_modRange, 
                        0., 
                        10.);

using peak_mod = parameter::from0To1<jdsp::jdelay, 
                                     1, 
                                     peak_modRange>;

using peak_t = wrap::mod<peak_mod, 
                         wrap::no_data<core::peak>>;

template <int NV>
using modchain_t_ = container::chain<parameter::empty, 
                                     wrap::fix<1, oscillator_t<NV>>, 
                                     core::smoother<NV>,
                                     math::pow<NV>, 
                                     math::sig2mod<NV>, 
                                     peak_t>;

template <int NV>
using modchain_t = wrap::control_rate<modchain_t_<NV>>;

template <int NV>
using frame2_block_t_ = container::chain<parameter::empty, 
                                         wrap::fix<2, modchain_t<NV>>, 
                                         jdsp::jdelay>;

template <int NV>
using frame2_block_t = wrap::frame<2, frame2_block_t_<NV>>;

namespace Nakof_Vibrato_t_parameters
{
// Parameter list for Nakof_Vibrato_impl::Nakof_Vibrato_t ------------------------------------------

DECLARE_PARAMETER_RANGE_STEP(RateRange, 
                             0., 
                             3.7, 
                             0.1);

template <int NV>
using Rate = parameter::from0To1<Nakof_Vibrato_impl::oscillator_t<NV>, 
                                 1, 
                                 RateRange>;

template <int NV>
using Depth = parameter::plain<Nakof_Vibrato_impl::oscillator_t<NV>, 
                               5>;
template <int NV>
using Nakof_Vibrato_t_plist = parameter::list<Rate<NV>, Depth<NV>>;
}

template <int NV>
using Nakof_Vibrato_t_ = container::chain<Nakof_Vibrato_t_parameters::Nakof_Vibrato_t_plist<NV>, 
                                          wrap::fix<2, frame2_block_t<NV>>>;

// =================================| Root node initialiser class |=================================

template <int NV> struct instance: public Nakof_Vibrato_impl::Nakof_Vibrato_t_<NV>
{
	
	struct metadata
	{
		static const int NumTables = 0;
		static const int NumSliderPacks = 0;
		static const int NumAudioFiles = 0;
		static const int NumFilters = 0;
		static const int NumDisplayBuffers = 0;
		
		SNEX_METADATA_ID(Nakof_Vibrato);
		SNEX_METADATA_NUM_CHANNELS(2);
		SNEX_METADATA_ENCODED_PARAMETERS(32)
		{
			0x005B, 0x0000, 0x5200, 0x7461, 0x0065, 0x0000, 0x0000, 0xCCCD, 
            0x406C, 0x0000, 0x3F80, 0x0000, 0x3F80, 0xCCCD, 0x3DCC, 0x015B, 
            0x0000, 0x4400, 0x7065, 0x6874, 0x0000, 0x0000, 0x0000, 0x8000, 
            0x003F, 0x8000, 0x003F, 0x8000, 0x003F, 0x0000, 0x0000, 0x0000
		};
	};
	
	instance()
	{
		// Node References -------------------------------------------------------------------------
		
		auto& frame2_block = this->getT(0);               // Nakof_Vibrato_impl::frame2_block_t<NV>
		auto& modchain = this->getT(0).getT(0);           // Nakof_Vibrato_impl::modchain_t<NV>
		auto& oscillator = this->getT(0).getT(0).getT(0); // Nakof_Vibrato_impl::oscillator_t<NV>
		auto& smoother = this->getT(0).getT(0).getT(1);   // core::smoother
		auto& pow = this->getT(0).getT(0).getT(2);        // math::pow<NV>
		auto& sig2mod = this->getT(0).getT(0).getT(3);    // math::sig2mod<NV>
		auto& peak = this->getT(0).getT(0).getT(4);       // Nakof_Vibrato_impl::peak_t
		auto& jdelay = this->getT(0).getT(1);             // jdsp::jdelay
		
		// Parameter Connections -------------------------------------------------------------------
		
		this->getParameterT(0).connectT(0, oscillator); // Rate -> oscillator::Frequency
		
		this->getParameterT(1).connectT(0, oscillator); // Depth -> oscillator::Gain
		
		// Modulation Connections ------------------------------------------------------------------
		
		peak.getParameter().connectT(0, jdelay); // peak -> jdelay::DelayTime
		
		// Default Values --------------------------------------------------------------------------
		
		oscillator.setParameterT(0, 0.); // core::oscillator::Mode
		;                                // oscillator::Frequency is automated
		oscillator.setParameterT(2, 2.); // core::oscillator::FreqRatio
		oscillator.setParameterT(3, 1.); // core::oscillator::Gate
		oscillator.setParameterT(4, 0.); // core::oscillator::Phase
		;                                // oscillator::Gain is automated
		
		smoother.setParameterT(0, 100.); // core::smoother::SmoothingTime
		smoother.setParameterT(1, 0.);   // core::smoother::DefaultValue
		
		pow.setParameterT(0, 1.); // math::pow::Value
		
		sig2mod.setParameterT(0, 1.); // math::sig2mod::Value
		
		jdelay.setParameterT(0, 1000.); // jdsp::jdelay::Limit
		;                               // jdelay::DelayTime is automated
		
		this->setParameterT(0, 1.);
		this->setParameterT(1, 1.);
		this->setExternalData({}, -1);
	}
	
	static constexpr bool isPolyphonic() { return NV > 1; };
	
	static constexpr bool isProcessingHiseEvent() { return true; };
	
	static constexpr bool hasTail() { return true; };
	
	void setExternalData(const ExternalData& b, int index)
	{
		// External Data Connections ---------------------------------------------------------------
		
		this->getT(0).getT(0).getT(0).setExternalData(b, index); // Nakof_Vibrato_impl::oscillator_t<NV>
		this->getT(0).getT(0).getT(4).setExternalData(b, index); // Nakof_Vibrato_impl::peak_t
	}
};
}

#undef getT
#undef connectT
#undef setParameterT
#undef setParameterWT
#undef getParameterT
// ======================================| Public Definition |======================================

namespace project
{
// polyphonic template declaration

template <int NV>
using Nakof_Vibrato = wrap::node<Nakof_Vibrato_impl::instance<NV>>;
}


