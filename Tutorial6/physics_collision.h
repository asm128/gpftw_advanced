#include "ftw_coord.h"

#ifndef PHYSICS_COLLISION_H
#define PHYSICS_COLLISION_H

namespace game 
{
	// Represents a bounding sphere that can be tested for overlap.
	template<typename _tElement>
	struct SBoundingSphere {
		typedef	::ftwlib::SCoord2<_tElement>	TCoord;

				TCoord							Center;
				_tElement						Radius;
	public:
												SBoundingSphere				(const TCoord &center, _tElement radius)										: Center(center), Radius(radius)	{}

	// Creates a bounding sphere to enclose the two given bounding spheres.
												SBoundingSphere				(const SBoundingSphere &one, const SBoundingSphere &two)						{
		const TCoord									centerOffset				= two.Center - one.Center;
		const double									distanceSquared				= centerOffset.LengthSquared();
		const double									radiusDiff					= two.Radius - one.Radius;

		if (radiusDiff * radiusDiff >= distanceSquared) {	// Check if the larger sphere encloses the small one
			if (one.Radius > two.Radius) {
				Center										= one.Center;
				Radius										= one.Radius;
			}
			else {
				Center										= two.Center;
				Radius										= two.Radius;
			}
		}
		else {	// Otherwise we need to work with partially overlapping spheres
			const double distance						= ::ftwlib::sqrt_safe(distanceSquared);
			Radius										= (distance + one.Radius + two.Radius) * 0.5;

			Center										= one.Center;	// The new centre is based on one's centre, moved towards two's centre by an ammount proportional to the spheres' radii.
			if (distance > 0)
				Center										+= centerOffset * ((Radius - one.Radius) / distance);
		}
	}
		// Returns the volume of thiS bounding volume. This is used to calculate how to recurse into the bounding volume tree. For a bounding sphere it is a simple calculation.
				double							GetSize						()																const			{ return 1.333333 * R_PI * Radius * Radius * Radius; }

				bool							Overlaps					(const SBoundingSphere &other)									const			{
			const double								distanceSquared				= (Center - other.Center).squareMagnitude();
			const double								radiiSum					= (Radius + other.Radius);
			return distanceSquared < (radiiSum * radiiSum);	// check squared distance against squared radius
		}
		// Reports how much this bouNding sphere would have to grow by to incorporate the given bounding sphere. Note that this calculation returns a value not in any particular units (i.e. its not a volume growth). 
		// In fact the best implemenTation takes into account the growth in surface area (after the * Goldsmith-Salmon algorithm for tree construction).
				double							GetGrowth					(const SBoundingSphere &other)									const			{
			SBoundingSphere								newSphere					= {*this, other};
			return newSphere.Radius * newSphere.Radius - Radius * Radius;	// We return a value proportional to the change in surface area of the sphere.
		}
	};
}

#endif // PHYSICS_COLLISION_H
