#ifndef MATH_EVAL_H
#define MATH_EVAL_H

//——————————————————————————————————————————————————————————————————————————————————————————

#include "math_types.h"
#include "math_funcs.h"
#include "math_tex.h"
#include <math.h>

//——————————————————————————————————————————————————————————————————————————————————————————

MathErr_t MathEvaluate (MathCtx_t* math_ctx, double* presult);

MathErr_t MathExecuteBinaryOperation(MathOp_t operation, double  left_result,
                                     double  right_result, double* result);

//——————————————————————————————————————————————————————————————————————————————————————————

#endif /* MATH_EVAL_H */
