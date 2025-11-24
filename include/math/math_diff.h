#ifndef MATH_DIFF_H
#define MATH_DIFF_H

//——————————————————————————————————————————————————————————————————————————————————————————

#include "math_types.h"
#include "math_funcs.h"
#include "math_tex.h"
#include <math.h>
#include <stdlib.h>

//——————————————————————————————————————————————————————————————————————————————————————————

MathErr_t MathDifferentiate(MathCtx_t* src_math_ctx, MathCtx_t* dest_math_ctx, const char* str_var);

//——————————————————————————————————————————————————————————————————————————————————————————

const double EULER_NUMBER = 2.718281828904590;

//——————————————————————————————————————————————————————————————————————————————————————————

#endif /* MATH_DIFF_H */
