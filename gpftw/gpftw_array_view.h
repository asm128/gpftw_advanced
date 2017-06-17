#include <cstdint>
#include <exception>

#ifndef GPFTW_ARRAY_VIEW_H
#define GPFTW_ARRAY_VIEW_H

namespace gpftw 
{
	template <typename _tElement>
	class array_view {
		// Properties / Member Variables
		_tElement							* Data						= 0;
		uint32_t							Size						= 0;

	public:
		// Constructors
											array_view					()															= default;
											array_view					(_tElement* data, uint32_t size)							: Data(data), Size(size) {
			if(0 == data && 0 != size)	// Crash if we received invalid parameters in order to prevent further malfunctioning.
				throw(::std::exception("Invalid parameters."));
		}

		// Operators
		_tElement&							operator[]					(uint32_t index)											{ if(index >= Size) throw(::std::exception("Invalid index.")); return Data[index]; }
		const _tElement&					operator[]					(uint32_t index)									const	{ if(index >= Size) throw(::std::exception("Invalid index.")); return Data[index]; }

		// Methods
		inline _tElement*					begin						()															{ return Data;			}
		inline constexpr const _tElement*	begin						()													const	{ return Data;			}

		inline _tElement*					end							()															{ return Data + Size;	}
		inline constexpr const _tElement*	end							()													const	{ return Data + Size;	}

		inline constexpr uint32_t			size						()													const	{ return Size;			}
	};
}

#endif GPFTW_ARRAY_VIEW_H