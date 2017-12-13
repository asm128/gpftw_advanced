#include "ftw_array_view.h"		// for ftwl::array_view<>

#ifndef GPFTW_GRID_VIEW_H
#define GPFTW_GRID_VIEW_H

namespace ftwl 
{
#pragma pack(push, 1)
	template <typename _tElement>
	class grid_view {
		// Properties / Member Variables
							_tElement								* Data						= 0;
							uint32_t								Width						= 0;
							uint32_t								Height						= 0;
	public:
		// Constructors
		inline constexpr											grid_view					()																			= default;
		inline														grid_view					(_tElement* dataElements, uint32_t gridWidth, uint32_t gridHeight)			: Data(dataElements), Width(gridWidth), Height(gridHeight)	{
			if(0 == dataElements && 0 != Width && 0 != Height)	// Crash if we received invalid parameters in order to prevent further malfunctioning.
				throw(::std::exception("Invalid parameters."));
		}
		// Operators
							::ftwl::array_view<_tElement>			operator[]					(uint32_t row)																{ if(0 == Data) throw(::std::exception("Uninitialized array pointer.")); if(row >= Height) throw(::std::exception("Invalid row.")); return ::ftwl::array_view<_tElement			>(&Data[row*Width], Width); }
							::ftwl::array_view<const _tElement>		operator[]					(uint32_t row)														const	{ if(0 == Data) throw(::std::exception("Uninitialized array pointer.")); if(row >= Height) throw(::std::exception("Invalid row.")); return ::ftwl::array_view<const _tElement	>(&Data[row*Width], Width); }
		// Methods
		inline constexpr	const _tElement*						begin						()																	const	{ return Data;						}
		inline constexpr	const _tElement*						end							()																	const	{ return Data + size();				}

		inline				_tElement*								begin						()																			{ return Data;						}
		inline				_tElement*								end							()																			{ return Data + size();				}

		inline constexpr	uint32_t								size						()																	const	{ return Width * Height;			}
		inline constexpr	uint32_t								width						()																	const	{ return Width	;					}
		inline constexpr	uint32_t								height						()																	const	{ return Height	;					}
	};
#pragma pack(pop)
}

#endif // GPFTW_GRID_VIEW_H