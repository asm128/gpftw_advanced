// A particle is the simplest object that can be simulated in the physics system.
// It has position data (no orientation data), along with velocity. It can be integrated forward through time, and have linear forces, and impulses applied to it. 
// This system allows defining the floating-point precision of the elements. It obviously won't work for integer types so don't use it in that way.
#include "ftw_coord.h"	// for ::ftwlib::SCoord2<>
#include "ftw_error.h"	// for ::ftwlib::error_t

#include <vector>		// for ::std::vector

#ifndef PHYSICS_PARTICLE_H
#define PHYSICS_PARTICLE_H

namespace game 
{
#pragma pack(push, 1)
	template<typename _tElement>
	struct SParticle2Forces {
		typedef				::ftwlib::SCoord2<_tElement>						TCoord;

							TCoord												AccumulatedForce						= {};
							TCoord												Acceleration							= {};	// A vector representing the speed in a given direction 
							TCoord												Velocity								= {};	// A vector representing the speed in a given direction 

		// This basically does Acceleration += (Force * Mass) and Velocity += (Acceleration * Time).
						void													IntegrateAccumulatedForce				(const _tElement inverseMass, const _tElement damping, const double timeElapsed) {
			Acceleration																+= AccumulatedForce * inverseMass;			// Calculate linear acceleration from force inputs.
			AccumulatedForce															= {};										// Clear this out now that we've used it already.

			Velocity																	+= Acceleration * timeElapsed;	// Update linear velocity from both acceleration and impulse.
			Velocity																	*= ::pow(damping, timeElapsed);	// Impose drag.
		}
	};

	template<typename _tElement>
	struct SParticle2 {
		// The member variables are organized such that matches the order in which they are used.
							::ftwlib::SCoord2<_tElement>						Position								= {};	// Coordinates in tile map
							SParticle2Forces<_tElement>							Forces									= {};
							_tElement											InverseMass								= 0;
							_tElement											Damping									= .99f;	// A vector representing the speed in a given direction 

		inline				void												SetMass									(const double mass)												{ InverseMass = 1.0 / mass;									}
		inline				double												GetMass									()														const	{ return (InverseMass == 0) ? DBL_MAX : 1.0 / InverseMass;	}
		inline				bool												HasFiniteMass							()														const	{ return InverseMass >= 0.0f;								}
	};

	// This compact structure allows to define all the boolean states of the particle packed in a single byte.
	struct SParticle2State {
							bool												Unused									: 1;
							bool												Active									: 1;

		inline constexpr	bool												RequiresProcessing						()														const	{ return (false == Unused) && Active; }
	};
#pragma pack(pop)
	// This basically does Position = Velocity * Time.
	template<typename _tElement>
	static inline		void												particleIntegratePosition
		(	const ::ftwlib::SCoord2<_tElement>		& velocity
		,	const double							timeElapsed
		,	const double							timeElapsedHalfSquared
		,	::ftwlib::SCoord2<_tElement>			& position
		)
	{
		position																+= velocity * timeElapsed;
		position																+= velocity * timeElapsedHalfSquared;
	}

	template<typename _tElement>
	struct SParticle2Engine {
		typedef	::game::SParticle2<_tElement>									TParticle;
		typedef	::ftwlib::SCoord2<_tElement>									TCoord;

				::std::vector<TParticle>										Particle								= {};
				::std::vector<TParticle>										ParticleNext							= {};
				::std::vector<::game::SParticle2State>							ParticleState							= {};
		// --------------------------------------------------------------------
		inline	::ftwlib::error_t												Integrate								(double timeElapsed, double timeElapsedHalfSquared)				{
			for(uint32_t iParticle = 0, particleCount = (uint32_t)ParticleState.size(); iParticle < particleCount; ++iParticle)	
				if(ParticleState[iParticle].RequiresProcessing()) {
					TParticle																	& particle = ParticleNext[iParticle]	= Particle[iParticle];	// Copy the current particle state to the next
					::game::particleIntegratePosition			(particle.Forces.Velocity, timeElapsed, timeElapsedHalfSquared, particle.Position);
					particle.Forces.IntegrateAccumulatedForce	(particle.InverseMass, particle.Damping, timeElapsed);
				}
			return 0;
		}
				::ftwlib::error_t												AddParticle								(const TParticle& particleData)									{
			const uint32_t																particleCount							= (uint32_t)ParticleState.size();
			static constexpr	const ::game::SParticle2State							initialBodyState						= {false, true};

			for(uint32_t iBody = 0; iBody < particleCount; ++iBody) {
				if(false == ParticleState[iBody].Unused)
					continue;
				ParticleState		[iBody]												= initialBodyState;
				Particle			[iBody]												= 
				ParticleNext		[iBody]												= particleData;
				return iBody;
			}
			try {	// Later on we're going to add a way to avoid using ::std::vector which require these ugly try/catch blocks.
				ParticleState		.push_back(initialBodyState);
				Particle			.push_back(particleData);
				ParticleNext		.push_back(particleData);
			}
			catch(...) { // Restore the previous size to all the containers and return an error.
				ParticleState		.resize(particleCount);
				Particle			.resize(particleCount);
				ParticleNext		.resize(particleCount);
				return -1;
			}
			return particleCount;
		}
	};
}

#endif // PHYSICS_PARTICLE_H