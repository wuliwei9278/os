/// taken from circle project, made some modification
/// license is the same as one in circle
/// Deuan Zhang
#ifndef _rpvm_assert_h
#define _rpvm_assert_h

#ifdef __cplusplus
extern "C" {
#endif

#ifdef DEBUG
	void assertion_failed (const char *pExpr, const char *pFile, unsigned nLine);

	#define assert(expr)	((expr)	? ((void) 0) : assertion_failed (#expr, __FILE__, __LINE__))

#else

	#define assert(expr)	((void) 0)

#endif

#ifdef __cplusplus
}
#endif

#endif