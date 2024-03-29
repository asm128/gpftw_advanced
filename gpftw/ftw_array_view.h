#include <cstdint>		// for uint32_t, uint8_t, uint16_t
#include <exception>	// for ::std::exception

#include "ftw_eval.h"

#ifndef GPFTW_ARRAY_VIEW_H
#define GPFTW_ARRAY_VIEW_H

namespace ftwl 
{
	template <typename _tElement>
	class array_view {
		// Properties / Member Variables
							_tElement				* Data						= 0;
							uint32_t				Count						= 0;
	public:
		// Constructors
		inline constexpr							array_view					()																			noexcept	= default;
		inline										array_view					(_tElement* dataElements, uint32_t elementCount)										: Data(dataElements), Count(elementCount)										{
			if(0 == dataElements && 0 != elementCount)	// Crash if we received invalid parameters in order to prevent further malfunctioning.
				throw(::std::exception("Invalid parameters."));
		}
		template <size_t _elementCount>
		inline constexpr							array_view					(_tElement (&_dataElements)[_elementCount])									noexcept	: Data(_dataElements), Count(_elementCount)										{}
		template <size_t _elementCount>
		inline constexpr							array_view					(_tElement (&_dataElements)[_elementCount], uint32_t elementCount)			noexcept	: Data(_dataElements), Count(::ftwl::min((uint32_t)_elementCount, elementCount))			{}

		// Operators
							_tElement&				operator[]					(uint32_t index)																		{ if(0 == Data) throw(::std::exception("Uninitialized array pointer.")); if(index >= Count) throw(::std::exception("Invalid index.")); return Data[index]; }
							const _tElement&		operator[]					(uint32_t index)													const				{ if(0 == Data) throw(::std::exception("Uninitialized array pointer.")); if(index >= Count) throw(::std::exception("Invalid index.")); return Data[index]; }
		// Methods
		inline				_tElement*				begin						()																			noexcept	{ return Data;			}
		inline				_tElement*				end							()																			noexcept	{ return Data + Count;	}

		inline constexpr	const _tElement*		begin						()																	const	noexcept	{ return Data;			}
		inline constexpr	const _tElement*		end							()																	const	noexcept	{ return Data + Count;	}

		inline constexpr	uint32_t				size						()																	const	noexcept	{ return Count;			}
	};
}

#endif GPFTW_ARRAY_VIEW_H