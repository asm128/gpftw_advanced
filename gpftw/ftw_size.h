#include <cstdint>

#ifndef FTW_SIZE_H_29037409238749082374908327
#define FTW_SIZE_H_29037409238749082374908327

namespace ftwlib {
	template<typename _tElement, size_t _staticSize>	static inline constexpr	const uint32_t			size				(const _tElement (&)[_staticSize])					noexcept	{ return (uint32_t)_staticSize;	}
}

#endif 