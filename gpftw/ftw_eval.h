#ifndef FTW_EVAL_H_97840928734902837409
#define FTW_EVAL_H_97840928734902837409

namespace ftwlib 
{
	template<typename _tValue>	static inline constexpr	const _tValue&					min				(const _tValue& a, const _tValue& b)			noexcept	{ return a < b ? a : b; }
	template<typename _tValue>	static inline constexpr	const _tValue&					max				(const _tValue& a, const _tValue& b)			noexcept	{ return a > b ? a : b; }
}

#endif // FTW_EVAL_H_97840928734902837409