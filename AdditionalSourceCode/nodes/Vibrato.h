#pragma once

#include <JuceHeader.h>
#include "Nakof_Vibrato.h"
// These will improve the readability of the connection definition

#define getT(Idx) template get<Idx>()
#define connectT(Idx, target) template connect<Idx>(target)
#define getParameterT(Idx) template getParameter<Idx>()
#define setParameterT(Idx, value) template setParameter<Idx>(value)
#define setParameterWT(Idx, value) template setWrapParameter<Idx>(value)
using namespace scriptnode;
using namespace snex;
using namespace snex::Types;

namespace Vibrato_impl
{
// ===========================| Node & Parameter type declarations |===========================

namespace Vibrato_t_parameters
{
// Parameter list for Vibrato_impl::Vibrato_t -------------------------------------------------

DECLARE_PARAMETER_RANGE(RateTest_InputRange, 
                        0., 
                        3.7);
DECLARE_PARAMETER_RANGE_STEP(RateTest_0Range, 
                             0., 
                             3.7, 
                             0.1);

template <int NV>
using RateTest_0 = parameter::from0To1<project::Nakof_Vibrato<NV>, 
                                       0, 
                                       RateTest_0Range>;

template <int NV>
using RateTest = parameter::chain<RateTest_InputRange, RateTest_0<NV>>;

template <int NV>
using DepthTest = parameter::plain<project::Nakof_Vibrato<NV>, 
                                   1>;
template <int NV>
using Vibrato_t_plist = parameter::list<RateTest<NV>, DepthTest<NV>>;
}

template <int NV>
using Vibrato_t_ = container::chain<Vibrato_t_parameters::Vibrato_t_plist<NV>, 
                                    wrap::fix<2, project::Nakof_Vibrato<NV>>>;

// ===============================| Root node initialiser class |===============================

template <int NV> struct instance: public Vibrato_impl::Vibrato_t_<NV>
{
	
	struct metadata
	{
		static const int NumTables = 0;
		static const int NumSliderPacks = 0;
		static const int NumAudioFiles = 0;
		static const int NumFilters = 0;
		static const int NumDisplayBuffers = 0;
		
		SNEX_METADATA_ID(Vibrato);
		SNEX_METADATA_NUM_CHANNELS(2);
		SNEX_METADATA_ENCODED_PARAMETERS(36)
		{
			0x005B, 0x0000, 0x5200, 0x7461, 0x5465, 0x7365, 0x0074, 0x0000, 
            0x0000, 0xCCCD, 0x406C, 0x0000, 0x3F80, 0x0000, 0x3F80, 0x0000, 
            0x0000, 0x015B, 0x0000, 0x4400, 0x7065, 0x6874, 0x6554, 0x7473, 
            0x0000, 0x0000, 0x0000, 0x8000, 0x003F, 0x8000, 0x003F, 0x8000, 
            0x003F, 0x0000, 0x0000, 0x0000
		};
	};
	
	instance()
	{
		// Node References --------------------------------------------------------------------
		
		auto& Nakof_Vibrato = this->getT(0); // project::Nakof_Vibrato<NV>
		
		// Parameter Connections --------------------------------------------------------------
		
		this->getParameterT(0).connectT(0, Nakof_Vibrato); // RateTest -> Nakof_Vibrato::Rate
		
		this->getParameterT(1).connectT(0, Nakof_Vibrato); // DepthTest -> Nakof_Vibrato::Depth
		
		// Default Values ---------------------------------------------------------------------
		
		; // Nakof_Vibrato::Rate is automated
		; // Nakof_Vibrato::Depth is automated
		
		this->setParameterT(0, 1.);
		this->setParameterT(1, 1.);
	}
	
	static constexpr bool isPolyphonic() { return NV > 1; };
	
	static constexpr bool hasTail() { return true; };
};
}

#undef getT
#undef connectT
#undef setParameterT
#undef setParameterWT
#undef getParameterT
// ====================================| Public Definition |====================================

namespace project
{
// polyphonic template declaration

template <int NV>
using Vibrato = wrap::node<Vibrato_impl::instance<NV>>;
}


