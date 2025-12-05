#ifndef MATH_TANGENT_H
#define MATH_TANGENT_H

//——————————————————————————————————————————————————————————————————————————————————————————

#include "math_types.h"
#include "funcs.h"
#include "tex.h"

//——————————————————————————————————————————————————————————————————————————————————————————

MathErr_t MathGetTangent(MathCtx_t* math_ctx, MathCtx_t* diff_math_ctx,
                         double     point,    MathCtx_t* tangent_ctx, int is_dump_enabled);

//——————————————————————————————————————————————————————————————————————————————————————————

#endif /* MATH_TANGENT_H */
