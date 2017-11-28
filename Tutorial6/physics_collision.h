#include "ftw_coord.h"

#ifndef PHYSICS_COLLISION_H
#define PHYSICS_COLLISION_H

namespace game 
{
	// Represents a bounding sphere that can be tested for overlap.
	template<typename _tElement>
	struct SBoundingSphere : public ::ftwl::SSphere2D<_tElement> {
		typedef		::ftwl::SCoord2<_tElement>		TCoord;
		using		::ftwl::SSphere2D<_tElement>	::SSphere2D;

	// --- Creates a bounding sphere to enclose the two given bounding spheres.
													SBoundingSphere				(const TCoord &center, _tElement radius)										: Center(center), Radius(radius)	{}
													SBoundingSphere				(const SBoundingSphere &one, const SBoundingSphere &two)						{
		const TCoord										centerOffset				= two.Center - one.Center;
		const double										distanceSquared				= centerOffset.LengthSquared();
		const double										radiusDiff					= two.Radius - one.Radius;

		if (radiusDiff * radiusDiff >= distanceSquared) {	// Check if the larger sphere encloses the small one
			if (one.Radius > two.Radius) {
				Center											= one.Center;
				Radius											= one.Radius;
			}
			else {
				Center											= two.Center;
				Radius											= two.Radius;
			}
		}
		else {	// Otherwise we need to work with partially overlapping spheres
			const double distance							= ::ftwl::sqrt_safe(distanceSquared);
			Radius											= (distance + one.Radius + two.Radius) * 0.5;

			Center											= one.Center;	// The new centre is based on one's centre, moved towards two's centre by an ammount proportional to the spheres' radii.
			if (distance > 0)
				Center											+= centerOffset * ((Radius - one.Radius) / distance);
		}
	}
		// Reports how much this bouNding sphere would have to grow by to incorporate the given bounding sphere. Note that this calculation returns a value not in any particular units (i.e. its not a volume growth). 
		// In fact the best implemenTation takes into account the growth in surface area (after the * Goldsmith-Salmon algorithm for tree construction).
					double							GetGrowth					(const SBoundingSphere &other)									const			{
			const SBoundingSphere							newSphere					= {*this, other};
			return newSphere.Radius * newSphere.Radius - Radius * Radius;	// We return a value proportional to the change in surface area of the sphere.
		}
	};
}

#endif // PHYSICS_COLLISION_H
