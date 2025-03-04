#pragma once

#if !defined(FORCE_INLINE)
#if defined(_DEBUG) || !defined(NDEBUG)  // Debug build
	#define FORCE_INLINE
#else  // Release build
	#if defined(__clang__)
		#define FORCE_INLINE [[gnu::always_inline]] inline
	#elif defined(__GNUC__)
		#define FORCE_INLINE [[gnu::always_inline]] inline
	#elif defined(_MSC_VER)
		#pragma warning(error: 4714)
		#define FORCE_INLINE __forceinline
	#else
		#error Unsupported compiler
	#endif
#endif
#endif

