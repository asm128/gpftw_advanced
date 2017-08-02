#include "ftw_particle.h"		// for ::game::SGame
#include "ftw_ascii_screen.h"	// for ::ftwl::SScreenASCII
#include "ftw_timer.h"			// for ::ftwl::STimer
#include "ftw_frameinfo.h"

#ifndef APPLICATION_H
#define APPLICATION_H

namespace ftwapp {
	enum PARTICLE_TYPE : int8_t
		{	PARTICLE_TYPE_SNOW		= 0
		,	PARTICLE_TYPE_FIRE
		,	PARTICLE_TYPE_LAVA
		,	PARTICLE_TYPE_RAIN
		,	PARTICLE_TYPE_COUNT
		,	PARTICLE_TYPE_INVALID	= -1
		};

	struct SParticleInstance {
		PARTICLE_TYPE															Type								= PARTICLE_TYPE_INVALID;
		int32_t																	PhysicsId							= -1;
	};

	struct SApplication {
		::ftwl::SScreenASCII													ScreenASCII							= {};
		int64_t																	FrameCounter						= 0;	// Declare and initialize a variable of (int)eger type for keeping track of the number of frame since execution began.

		::ftwl::STimer														Timer								= {};
		::ftwl::SParticle2Engine<float>										ParticleEngine						= {};
		::ftwl::SFrameInfo													FrameInfo							= {};
		::std::vector<SParticleInstance>										ParticleInstances					= {};
	};

	::ftwl::error_t														setup								(::ftwapp::SApplication& applicationInstance);
	::ftwl::error_t														update								(::ftwapp::SApplication& applicationInstance);
	::ftwl::error_t														render								(::ftwapp::SApplication& applicationInstance);
	::ftwl::error_t														cleanup								(::ftwapp::SApplication& applicationInstance);
}


#endif // APPLICATION_H