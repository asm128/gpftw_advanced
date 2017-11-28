// Tip: Hold Left ALT + SHIFT while tapping or holding the arrow keys in order to select multiple columns and write on them at once. 
//		Also useful for copy & paste operations in which you need to copy a bunch of variable or function names and you can't afford the time of copying them one by one.
//
#include "application.h"	// for
#include "ftw_ascii_color.h"

#include <stdio.h>		// for printf()
#include <windows.h>	// for interacting with Windows

static constexpr const uint32_t												SCREEN_WIDTH													= 128;
static constexpr const uint32_t												SCREEN_HEIGHT													= 64;

static ::ftwl::SParticle2<float>											particleDefinitions	[::ftwapp::PARTICLE_TYPE_COUNT]				= {};

void																		setupParticles													()														{
	particleDefinitions	[::ftwapp::PARTICLE_TYPE_SNOW].Position					= 
	particleDefinitions	[::ftwapp::PARTICLE_TYPE_FIRE].Position					= 
	particleDefinitions	[::ftwapp::PARTICLE_TYPE_RAIN].Position					= 
	particleDefinitions	[::ftwapp::PARTICLE_TYPE_LAVA].Position					= {SCREEN_WIDTH/2, SCREEN_HEIGHT/2};

	particleDefinitions	[::ftwapp::PARTICLE_TYPE_SNOW].SetMass					( 2);
	particleDefinitions	[::ftwapp::PARTICLE_TYPE_FIRE].SetMass					(-1);
	particleDefinitions	[::ftwapp::PARTICLE_TYPE_RAIN].SetMass					( 1);
	particleDefinitions	[::ftwapp::PARTICLE_TYPE_LAVA].SetMass					( 1);

	particleDefinitions	[::ftwapp::PARTICLE_TYPE_SNOW].Damping					= 0.80f;
	particleDefinitions	[::ftwapp::PARTICLE_TYPE_FIRE].Damping					= 0.80f;
	particleDefinitions	[::ftwapp::PARTICLE_TYPE_RAIN].Damping					= 1.0f;
	particleDefinitions	[::ftwapp::PARTICLE_TYPE_LAVA].Damping					= 0.99f;

	particleDefinitions	[::ftwapp::PARTICLE_TYPE_SNOW].Forces.Velocity			= {};
	particleDefinitions	[::ftwapp::PARTICLE_TYPE_FIRE].Forces.Velocity			= {(float)0,  (float)(rand() % 40)};
	particleDefinitions	[::ftwapp::PARTICLE_TYPE_RAIN].Forces.Velocity			= {};
	particleDefinitions	[::ftwapp::PARTICLE_TYPE_LAVA].Forces.Velocity			= {(float)0, -(float)(rand() % 20)-20};
}

// Cleanup application resources.
::ftwl::error_t																ftwapp::cleanup													(::ftwapp::SApplication& applicationInstance)			{ 
	::ftwl::asciiDisplayDestroy	();								
	::ftwl::asciiTargetDestroy	(applicationInstance.ASCIIRenderTarget);								
	return 0;
}

// Use this function to setup our game data
::ftwl::error_t																ftwapp::setup													(::ftwapp::SApplication& applicationInstance)			{ // Accepts an address pointing to an SGame instance
	::ftwl::asciiTargetCreate(applicationInstance.ASCIIRenderTarget, ::SCREEN_WIDTH, ::SCREEN_HEIGHT);
	::ftwl::asciiDisplayCreate(applicationInstance.ASCIIRenderTarget.Width(), applicationInstance.ASCIIRenderTarget.Height());
	::setupParticles();
	srand(0);
	return 0;
}

void																		addParticle														
	(	::ftwapp::PARTICLE_TYPE						particleType
	,	::std::vector<::ftwapp::SParticleInstance>	& particleInstances
	,	::ftwl::SParticle2Engine<float>			& particleEngine
	)														
{
	::ftwapp::SParticleInstance														newInstance														= {}; 
	newInstance.Type															= particleType; 
	newInstance.PhysicsId														= particleEngine.AddParticle(particleDefinitions[newInstance.Type]); 
	particleEngine.Particle[newInstance.PhysicsId].Position						= {(float)(rand() % SCREEN_WIDTH), (float)(rand() % SCREEN_HEIGHT)};
	switch(particleType) {
	case ::ftwapp::PARTICLE_TYPE_FIRE:	particleEngine.Particle[newInstance.PhysicsId].Position		= {SCREEN_WIDTH / 2 + (rand() % 3 - 1.0f) * (SCREEN_WIDTH / 5), SCREEN_HEIGHT / 2 + SCREEN_HEIGHT / 4}; break;
	case ::ftwapp::PARTICLE_TYPE_LAVA:	particleEngine.Particle[newInstance.PhysicsId].Position.y	= SCREEN_HEIGHT - 1; break;
	case ::ftwapp::PARTICLE_TYPE_RAIN:
	case ::ftwapp::PARTICLE_TYPE_SNOW:	particleEngine.Particle[newInstance.PhysicsId].Position.y	= 0; particleEngine.Particle[newInstance.PhysicsId].Forces.Velocity.y	= .001f; break;
	}
	particleInstances.push_back(newInstance);
}

// Use this function to update our game data
::ftwl::error_t															ftwapp::update													(::ftwapp::SApplication& applicationInstance)			{ // Accepts an address of an SGame instance
	::ftwl::asciiDisplayPresent(applicationInstance.ASCIIRenderTarget);

	::ftwl::STimer																& timerInstance													= applicationInstance.Timer;																	
	::ftwl::SFrameInfo															& frameInfo														= applicationInstance.FrameInfo;																	
	frameInfo.Frame(timerInstance.LastTimeMicroseconds);
	const float																		lastFrameSeconds												= (float)frameInfo.Seconds.LastFrame;

	::std::vector<SParticleInstance>												& particleInstances												= applicationInstance.ParticleInstances;
	::ftwl::SParticle2Engine<float>												& particleEngine												= applicationInstance.ParticleEngine;
	static float																	windDirection													= 0.1f;
	if(GetAsyncKeyState('1')) for(uint32_t i = 0; i < 3; ++i) addParticle(PARTICLE_TYPE_SNOW, particleInstances, particleEngine);
	if(GetAsyncKeyState('2')) for(uint32_t i = 0; i < 3; ++i) addParticle(PARTICLE_TYPE_FIRE, particleInstances, particleEngine);
	if(GetAsyncKeyState('3')) for(uint32_t i = 0; i < 3; ++i) addParticle(PARTICLE_TYPE_RAIN, particleInstances, particleEngine);
	if(GetAsyncKeyState('4')) for(uint32_t i = 0; i < 3; ++i) addParticle(PARTICLE_TYPE_LAVA, particleInstances, particleEngine);

	particleEngine.Integrate(lastFrameSeconds, frameInfo.Seconds.LastFrameHalfSquared);

	windDirection																= (float)sin(frameInfo.Seconds.Total/5.0f) * .25f;

	for(uint32_t iParticle = 0; iParticle < particleInstances.size(); ++iParticle) {
		SParticleInstance																& particleInstance												= particleInstances[iParticle];
		int32_t																			physicsId														= particleInstance.PhysicsId;
		::ftwl::SParticle2<float>														& particleNext													= particleEngine.ParticleNext[physicsId];
		if( particleNext.Position.x < 0 || particleNext.Position.x >= applicationInstance.ASCIIRenderTarget.Width	()
		 || particleNext.Position.y < 0 || particleNext.Position.y >= applicationInstance.ASCIIRenderTarget.Height	()
		 ) { // Remove the particle instance and related information.
			particleEngine.ParticleState[physicsId].Unused								= true;
			particleInstances.erase(particleInstances.begin()+iParticle);
			--iParticle;
		}
		else { // Apply forces from wind and gravity.
			static constexpr	const double												gravity															= 9.8;
			::ftwl::SParticle2<float>														& particleCurrent												= particleEngine.Particle[physicsId];
			particleCurrent																= particleEngine.ParticleNext[physicsId];
			particleCurrent.Forces.AccumulatedForce										= {0, float(gravity * lastFrameSeconds)};
			particleCurrent.Forces.AccumulatedForce.x									+= float(windDirection * abs(particleCurrent.Forces.Velocity.y) * .5) + float((rand() % 100) / 100.0 - .5);
		}
	}

	timerInstance.Frame();
	return 0;
}

::ftwl::error_t															ftwapp::render													(::ftwapp::SApplication& applicationInstance)			{
	::ftwl::SASCIITarget														& screenAscii													= applicationInstance.ASCIIRenderTarget;
	::ftwl::asciiTargetClear(screenAscii);
	::std::vector<SParticleInstance>											& particleInstances												= applicationInstance.ParticleInstances;
	for(uint32_t iParticle = 0, particleCount = (uint32_t)particleInstances.size(); iParticle < particleCount; ++iParticle) {
		SParticleInstance															& particleInstance												= particleInstances[iParticle];
		const int32_t																physicsId														= particleInstance.PhysicsId;
		const ::ftwl::SCoord2<float>												particlePosition												= applicationInstance.ParticleEngine.Particle[physicsId].Position;
		screenAscii.Characters	[(uint32_t)particlePosition.y][(uint32_t)particlePosition.x]	= 1 + particleInstance.Type;
		screenAscii.Colors		[(uint32_t)particlePosition.y][(uint32_t)particlePosition.x]	
			= (particleInstance.Type == PARTICLE_TYPE_FIRE) ? ::ftwl::ASCII_COLOR_RED
			: (particleInstance.Type == PARTICLE_TYPE_SNOW) ? ::ftwl::ASCII_COLOR_CYAN
			: (particleInstance.Type == PARTICLE_TYPE_LAVA) ? ::ftwl::ASCII_COLOR_RED
			: ::ftwl::ASCII_COLOR_BLUE
			;
	}
	return 0;
}

int																			main															()														{
	::ftwapp::SApplication															* applicationInstance											= new ::ftwapp::SApplication();	// Create a new instance of our application.
	if( 0 == applicationInstance )
		return -1;	// return error because we couldn't allocate the main instance of our application.

	::ftwapp::setup		(*applicationInstance); 

	while( true ) { /// Execute code between braces while the condition inside () evaluates to true.
		::ftwapp::update	(*applicationInstance);		/// Update frame.
		::ftwapp::render	(*applicationInstance);		/// Render frame.
		if(::GetAsyncKeyState(VK_ESCAPE))		/// Check for escape key pressed.
			break;	/// Exit while() loop.
	}

	::ftwapp::cleanup	(*applicationInstance);

	delete( applicationInstance );	// Destroy the applcation instance and release its memory.
	return 0; /// Exit from the function returning an (int)eger.
}

int	WINAPI														WinMain								
	(	_In_		::HINSTANCE		// hInstance
	,	_In_opt_	::HINSTANCE		// hPrevInstance
	,	_In_		::LPSTR			// lpCmdLine
	,	_In_		::INT				// nShowCmd
	)
{
	_CrtSetDbgFlag(_CRTDBG_LEAK_CHECK_DF | _CRTDBG_ALLOC_MEM_DF);
	return ::ftwl::failed(::main()) ? EXIT_FAILURE : EXIT_SUCCESS;	// just redirect to our generic main() function.
}
