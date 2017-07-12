#include <cstdint>

#ifndef FRAMEINFO_H
#define FRAMEINFO_H

namespace game 
{
	struct SFrameSeconds {
				double															Total									;
				double															LastFrame								;
				double															LastFrameHalfSquared					;
		// Helper
		inline	void															UpdateFromTime							(double secondsToAdd)						noexcept	{ 
			Total																	+= LastFrame								= secondsToAdd; 
			LastFrameHalfSquared													=  secondsToAdd * secondsToAdd * 0.5;
		}
	};

	struct SFrameMicroseconds {
				uint64_t														Total									;
				uint64_t														LastFrame								;
		// Helper
		inline	void															UpdateFromTime							(uint64_t microsecondsToAdd)				noexcept	{ Total += LastFrame = microsecondsToAdd; }
	};

	struct SFrameInfo {
				uint64_t														FrameNumber								= 0;
				uint64_t														FramesPerSecond							= 0;
				SFrameMicroseconds												Microseconds							= {};
				SFrameSeconds													Seconds									= {};
		
				void															Frame									(uint64_t timeElapsedMicroseconds)						{	// Set last frame time and number.
					++FrameNumber;
					Microseconds	.UpdateFromTime(timeElapsedMicroseconds);
					Seconds			.UpdateFromTime(timeElapsedMicroseconds / 1000000.0);
				}
	};
}

#endif // FRAMEINFO_H