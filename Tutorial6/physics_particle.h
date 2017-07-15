// This work is loosely based on Ian Millington's "Game Physics Engine Development" 
//
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
		static constexpr	const _tElement										VelocityEpsilon								= 0.0001f;

							TCoord												AccumulatedForce							= {};
							TCoord												Acceleration								= {};	// A vector representing the speed in a given direction 
							TCoord												Velocity									= {};	// A vector representing the speed in a given direction 

		inline constexpr	bool												VelocityDepleted							()																					const	{ return (Velocity + Acceleration) < VelocityEpsilon; }
		// This basically does Acceleration += (Force * 1 / Mass) and Velocity += (Acceleration * Time).
							void												IntegrateAccumulatedForce					(const _tElement inverseMass, const _tElement damping, const double timeElapsed)			{
			Acceleration															+= AccumulatedForce * inverseMass;				// Calculate linear acceleration from force inputs.
			AccumulatedForce														= {};											// Clear this out now that we've used it already.

			Velocity																+= Acceleration * timeElapsed;					// Update linear velocity from both acceleration and impulse.
			Velocity																*= ::pow(damping, timeElapsed);					// Impose drag.
		}
	};

	template<typename _tElement>
	struct SParticle2 {
		// The member variables are organized such that matches the order in which they are used.
							::ftwlib::SCoord2<_tElement>						Position									= {};	
							SParticle2Forces<_tElement>							Forces										= {};
							_tElement											InverseMass									= 0;
							_tElement											Damping										= .99f;	// A vector representing the speed in a given direction 

		inline				void												SetMass										(const double mass)																			{ InverseMass = mass ? ((_tElement)(1.0 / mass)) : 0;					}
		inline constexpr	double												GetMass										()																					const	{ return (InverseMass == 0) ? DBL_MAX : 1.0 / InverseMass;	}
		inline constexpr	bool												HasFiniteMass								()																					const	{ return InverseMass >= 0.0f;								}
	};

	// This compact structure allows to define all the boolean states of the particle packed in a single byte.
	struct SParticle2State {
							bool												Unused										: 1;
							bool												Active										: 1;

		inline constexpr	bool												RequiresProcessing							()																					const	{ return (false == Unused) && Active; }
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
		typedef				::game::SParticle2	<_tElement>						TParticle;
		typedef				::ftwlib::SCoord2	<_tElement>						TCoord;

							::std::vector<TParticle>							Particle									= {};
							::std::vector<TParticle>							ParticleNext								= {};
							::std::vector<::game::SParticle2State>				ParticleState								= {};
		// --------------------------------------------------------------------
		inline				::ftwlib::error_t									Integrate									(double timeElapsed, double timeElapsedHalfSquared)											{
			for(uint32_t iParticle = 0, particleCount = (uint32_t)ParticleState.size(); iParticle < particleCount; ++iParticle)	
				if(ParticleState[iParticle].RequiresProcessing()) {
					TParticle																	& particle = ParticleNext[iParticle]		= Particle[iParticle];	// Copy the current particle state to the next
					::game::particleIntegratePosition			(particle.Forces.Velocity, timeElapsed, timeElapsedHalfSquared, particle.Position);
					particle.Forces.IntegrateAccumulatedForce	(particle.InverseMass, particle.Damping, timeElapsed);
				}
			return 0;
		}
							::ftwlib::error_t									AddParticle									(const TParticle& particleData)																{
								const uint32_t											particleCount								= (uint32_t)ParticleState.size();
			static constexpr	const ::game::SParticle2State							initialBodyState							= {false, true};

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
 //   // port of this JavaScript code with some changes:
 //   //   http://www.kevlindev.com/gui/math/intersection/Intersection.js
 //   // found here:
 //   //   http://stackoverflow.com/questions/563198/how-do-you-detect-where-two-line-segments-intersect/563240#563240

	//public class Intersector
	//{
	//	static double MyEpsilon = 0.00001;

	//	private static float[] OverlapIntervals(float ub1, float ub2)
	//	{
	//		float l = Math.Min(ub1, ub2);
	//		float r = Math.Max(ub1, ub2);
	//		float A = Math.Max(0, l);
	//		float B = Math.Min(1, r);
	//		if (A > B) // no intersection
	//			return new float[] { };
	//		else if (A == B)
	//			return new float[] { A };
	//		else // if (A < B)
	//			return new float[] { A, B };
	//	}

	//	// IMPORTANT: a1 and a2 cannot be the same, e.g. a1--a2 is a true segment, not a point
	//	// b1/b2 may be the same (b1--b2 is a point)
	//	private static PointF[] OneD_Intersection(PointF a1, PointF a2, PointF b1, PointF b2)
	//	{
	//		//float ua1 = 0.0f; // by definition
	//		//float ua2 = 1.0f; // by definition
	//		float ub1, ub2;

	//		float denomx = a2.X - a1.X;
	//		float denomy = a2.Y - a1.Y;

	//		if (Math.Abs(denomx) > Math.Abs(denomy))
	//		{
	//			ub1 = (b1.X - a1.X) / denomx;
	//			ub2 = (b2.X - a1.X) / denomx;
	//		}
	//		else
	//		{
	//			ub1 = (b1.Y - a1.Y) / denomy;
	//			ub2 = (b2.Y - a1.Y) / denomy;
	//		}

	//		List<PointF> ret = new List<PointF>();
	//		float[] interval = OverlapIntervals(ub1, ub2);
	//		foreach (float f in interval)
	//		{
	//			float x = a2.X * f + a1.X * (1.0f - f);
	//			float y = a2.Y * f + a1.Y * (1.0f - f);
	//			PointF p = new PointF(x, y);
	//			ret.Add(p);
	//		}
	//		return ret.ToArray();
	//	}

	//	private static bool PointOnLine(PointF p, PointF a1, PointF a2)
	//	{
	//		float dummyU = 0.0f;
	//		double d = DistFromSeg(p, a1, a2, MyEpsilon, ref dummyU);
	//		return d < MyEpsilon;
	//	}

	//	private static double DistFromSeg(PointF p, PointF q0, PointF q1, double radius, ref float u)
	//	{
	//		// formula here:
	//		//http://mathworld.wolfram.com/Point-LineDistance2-Dimensional.html
	//		// where x0,y0 = p
	//		//       x1,y1 = q0
	//		//       x2,y2 = q1
	//		double dx21 = q1.X - q0.X;
	//		double dy21 = q1.Y - q0.Y;
	//		double dx10 = q0.X - p.X;
	//		double dy10 = q0.Y - p.Y;
	//		double segLength = Math.Sqrt(dx21 * dx21 + dy21 * dy21);
	//		if (segLength < MyEpsilon)
	//			throw new Exception("Expected line segment, not point.");
	//		double num = Math.Abs(dx21 * dy10 - dx10 * dy21);
	//		double d = num / segLength;
	//		return d;
	//	}

	//	// this is the general case. Really really general
	//	public static PointF[] Intersection(PointF a1, PointF a2, PointF b1, PointF b2)
	//	{
	//		if (a1.Equals(a2) && b1.Equals(b2))
	//		{
	//			// both "segments" are points, return either point
	//			if (a1.Equals(b1))
	//				return new PointF[] { a1 };
	//			else // both "segments" are different points, return empty set
	//				return new PointF[] { };
	//		}
	//		else if (b1.Equals(b2)) // b is a point, a is a segment
	//		{
	//			if (PointOnLine(b1, a1, a2))
	//				return new PointF[] { b1 };
	//			else
	//				return new PointF[] { };
	//		}
	//		else if (a1.Equals(a2)) // a is a point, b is a segment
	//		{
	//			if (PointOnLine(a1, b1, b2))
	//				return new PointF[] { a1 };
	//			else
	//				return new PointF[] { };
	//		}

	//		// at this point we know both a and b are actual segments

	//		float ua_t = (b2.X - b1.X) * (a1.Y - b1.Y) - (b2.Y - b1.Y) * (a1.X - b1.X);
	//		float ub_t = (a2.X - a1.X) * (a1.Y - b1.Y) - (a2.Y - a1.Y) * (a1.X - b1.X);
	//		float u_b = (b2.Y - b1.Y) * (a2.X - a1.X) - (b2.X - b1.X) * (a2.Y - a1.Y);

	//		// Infinite lines intersect somewhere
	//		if (!(-MyEpsilon < u_b && u_b < MyEpsilon))   // e.g. u_b != 0.0
	//		{
	//			float ua = ua_t / u_b;
	//			float ub = ub_t / u_b;
	//			if (0.0f <= ua && ua <= 1.0f && 0.0f <= ub && ub <= 1.0f)
	//			{
	//				// Intersection
	//				return new PointF[] {
	//					new PointF(a1.X + ua * (a2.X - a1.X),
	//						a1.Y + ua * (a2.Y - a1.Y)) };
	//			}
	//			else
	//			{
	//				// No Intersection
	//				return new PointF[] { };
	//			}
	//		}
	//		else // lines (not just segments) are parallel or the same line
	//		{
	//			// Coincident
	//			// find the common overlapping section of the lines
	//			// first find the distance (squared) from one point (a1) to each point
	//			if ((-MyEpsilon < ua_t && ua_t < MyEpsilon)
	//			   || (-MyEpsilon < ub_t && ub_t < MyEpsilon))
	//			{
	//				if (a1.Equals(a2)) // danger!
	//					return OneD_Intersection(b1, b2, a1, a2);
	//				else // safe
	//					return OneD_Intersection(a1, a2, b1, b2);
	//			}
	//			else
	//			{
	//				// Parallel
	//				return new PointF[] { };
	//			}
	//		}
	//	}
	//}
//Here is the test code:
//
//    public class IntersectTest
//    {
//        public static void PrintPoints(PointF[] pf)
//        {
//            if (pf == null || pf.Length < 1)
//                System.Console.WriteLine("Doesn't intersect");
//            else if (pf.Length == 1)
//            {
//                System.Console.WriteLine(pf[0]);
//            }
//            else if (pf.Length == 2)
//            {
//                System.Console.WriteLine(pf[0] + " -- " + pf[1]);
//            }
//        }
//
//        public static void TestIntersect(PointF a1, PointF a2, PointF b1, PointF b2)
//        {
//            System.Console.WriteLine("----------------------------------------------------------");
//            System.Console.WriteLine("Does      " + a1 + " -- " + a2);
//            System.Console.WriteLine("intersect " + b1 + " -- " + b2 + " and if so, where?");
//            System.Console.WriteLine("");
//            PointF[] result = Intersect.Intersection(a1, a2, b1, b2);
//            PrintPoints(result);
//        }
//
//        public static void Main()
//        {
//            System.Console.WriteLine("----------------------------------------------------------");
//            System.Console.WriteLine("line segments intersect");
//            TestIntersect(new PointF(0, 0),
//                          new PointF(100, 100),
//                          new PointF(100, 0),
//                          new PointF(0, 100));
//            TestIntersect(new PointF(5, 17),
//                          new PointF(100, 100),
//                          new PointF(100, 29),
//                          new PointF(8, 100));
//            System.Console.WriteLine("----------------------------------------------------------");
//            System.Console.WriteLine("");
//
//            System.Console.WriteLine("----------------------------------------------------------");
//            System.Console.WriteLine("just touching points and lines cross");
//            TestIntersect(new PointF(0, 0),
//                          new PointF(25, 25),
//                          new PointF(25, 25),
//                          new PointF(100, 75));
//            System.Console.WriteLine("----------------------------------------------------------");
//            System.Console.WriteLine("");
//
//            System.Console.WriteLine("----------------------------------------------------------");
//            System.Console.WriteLine("parallel");
//            TestIntersect(new PointF(0, 0),
//                          new PointF(0, 100),
//                          new PointF(100, 0),
//                          new PointF(100, 100));
//            System.Console.WriteLine("----------------------------------------------------------");
//            System.Console.WriteLine("");
//
//            System.Console.WriteLine("----");
//            System.Console.WriteLine("lines cross but segments don't intersect");
//            TestIntersect(new PointF(50, 50),
//                          new PointF(100, 100),
//                          new PointF(0, 25),
//                          new PointF(25, 0));
//            System.Console.WriteLine("----------------------------------------------------------");
//            System.Console.WriteLine("");
//
//            System.Console.WriteLine("----------------------------------------------------------");
//            System.Console.WriteLine("coincident but do not overlap!");
//            TestIntersect(new PointF(0, 0),
//                          new PointF(25, 25),
//                          new PointF(75, 75),
//                          new PointF(100, 100));
//            System.Console.WriteLine("----------------------------------------------------------");
//            System.Console.WriteLine("");
//
//            System.Console.WriteLine("----------------------------------------------------------");
//            System.Console.WriteLine("touching points and coincident!");
//            TestIntersect(new PointF(0, 0),
//                          new PointF(25, 25),
//                          new PointF(25, 25),
//                          new PointF(100, 100));
//            System.Console.WriteLine("----------------------------------------------------------");
//            System.Console.WriteLine("");
//
//            System.Console.WriteLine("----------------------------------------------------------");
//            System.Console.WriteLine("overlap/coincident");
//            TestIntersect(new PointF(0, 0),
//                          new PointF(75, 75),
//                          new PointF(25, 25),
//                          new PointF(100, 100));
//            TestIntersect(new PointF(0, 0),
//                          new PointF(100, 100),
//                          new PointF(0, 0),
//                          new PointF(100, 100));
//            System.Console.WriteLine("----------------------------------------------------------");
//            System.Console.WriteLine("");
//
//            while (!System.Console.KeyAvailable) { }
//        }
//
//    }
////and here is the output:
////
////----------------------------------------------------------
////line segments intersect
////----------------------------------------------------------
////Does      {X=0, Y=0} -- {X=100, Y=100}
////intersect {X=100, Y=0} -- {X=0, Y=100} and if so, where?
////
////{X=50, Y=50}
////----------------------------------------------------------
////Does      {X=5, Y=17} -- {X=100, Y=100}
////intersect {X=100, Y=29} -- {X=8, Y=100} and if so, where?
////
////{X=56.85001, Y=62.30054}
////----------------------------------------------------------
////
////----------------------------------------------------------
////just touching points and lines cross
////----------------------------------------------------------
////Does      {X=0, Y=0} -- {X=25, Y=25}
////intersect {X=25, Y=25} -- {X=100, Y=75} and if so, where?
////
////{X=25, Y=25}
////----------------------------------------------------------
////
////----------------------------------------------------------
////parallel
////----------------------------------------------------------
////Does      {X=0, Y=0} -- {X=0, Y=100}
////intersect {X=100, Y=0} -- {X=100, Y=100} and if so, where?
////
////Doesn't intersect
////----------------------------------------------------------
////
////----
////lines cross but segments don't intersect
////----------------------------------------------------------
////Does      {X=50, Y=50} -- {X=100, Y=100}
////intersect {X=0, Y=25} -- {X=25, Y=0} and if so, where?
////
////Doesn't intersect
////----------------------------------------------------------
////
////----------------------------------------------------------
////coincident but do not overlap!
////----------------------------------------------------------
////Does      {X=0, Y=0} -- {X=25, Y=25}
////intersect {X=75, Y=75} -- {X=100, Y=100} and if so, where?
////
////Doesn't intersect
////----------------------------------------------------------
////
////----------------------------------------------------------
////touching points and coincident!
////----------------------------------------------------------
////Does      {X=0, Y=0} -- {X=25, Y=25}
////intersect {X=25, Y=25} -- {X=100, Y=100} and if so, where?
////
////{X=25, Y=25}
////----------------------------------------------------------
////
////----------------------------------------------------------
////overlap/coincident
////----------------------------------------------------------
////Does      {X=0, Y=0} -- {X=75, Y=75}
////intersect {X=25, Y=25} -- {X=100, Y=100} and if so, where?
////
////{X=25, Y=25} -- {X=75, Y=75}
////----------------------------------------------------------
////Does      {X=0, Y=0} -- {X=100, Y=100}
////intersect {X=0, Y=0} -- {X=100, Y=100} and if so, where?
////
////{X=0, Y=0} -- {X=100, Y=100}
////----------------------------------
// Solve for T2!

// Make sure that T1 > 0 and  0 < T2 < 1. 
// If they aren't, then the supposed intersection is not on the ray/segment, and there is no intersection after all. 
// But if they are, great! You've found an intersect. Now just loop through all other line segments with the same ray, in order to find the closest intersection. (It will be the one with the lowest T1 value) 
}





#endif // PHYSICS_PARTICLE_H