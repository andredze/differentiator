#ifndef MATH_TEX_H
#define MATH_TEX_H

//——————————————————————————————————————————————————————————————————————————————————————————

#include "math_types.h"
#include "tree_types.h"
#include "math_funcs.h"
#include <stdarg.h>

//——————————————————————————————————————————————————————————————————————————————————————————

MathErr_t MathOpenTexFile   (MathCtx_t* math_ctx);
MathErr_t MathCloseTexFile  (MathCtx_t* math_ctx);
MathErr_t MathCtxTexDump    (MathCtx_t* math_ctx, const char* fmt, ...);
MathErr_t vMathCtxTexDump   (MathCtx_t* math_ctx, const char* fmt, va_list args);

//——————————————————————————————————————————————————————————————————————————————————————————

#endif /* MATH_TEX_H */
