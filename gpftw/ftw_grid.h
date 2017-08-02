#include "ftw_grid_view.h"

#ifndef GPFTW_GRID_H_92813740928374092387498237
#define GPFTW_GRID_H_92813740928374092387498237

namespace ftwl
{
#pragma pack(push, 1)
	template<typename _tCell, size_t _sizeWidth, size_t _sizeDepth>
	struct SGrid : public grid_view<_tCell> {
							typedef						_tCell							TCell;

		static constexpr	const uint32_t				Width							= (uint32_t)_sizeWidth;
		static constexpr	const uint32_t				Depth							= (uint32_t)_sizeDepth;

							TCell						Cells[_sizeDepth][_sizeWidth]	= {};

		inline constexpr								SGrid							()							: grid_view(&Cells[0][0], _sizeWidth, _sizeDepth)					{}
	};
#pragma pack(pop)
}

#endif // GPFTW_GRID_H_92813740928374092387498237