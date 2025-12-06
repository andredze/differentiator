#ifndef MATH_DIFF_H
#define MATH_DIFF_H

//——————————————————————————————————————————————————————————————————————————————————————————

#include "math_types.h"
#include "funcs.h"
#include "tex.h"
#include <math.h>
#include <stdlib.h>

//——————————————————————————————————————————————————————————————————————————————————————————

MathErr_t MathDifferentiate (MathCtx_t* src_math_ctx, MathCtx_t* dest_math_ctx, const char* str_var);

//——————————————————————————————————————————————————————————————————————————————————————————

#endif /* MATH_DIFF_H */
