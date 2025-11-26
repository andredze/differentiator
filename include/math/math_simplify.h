#ifndef MATH_SIMPLIFY_H
#define MATH_SIMPLIFY_H

//——————————————————————————————————————————————————————————————————————————————————————————

#include "math_types.h"
#include "math_eval.h"
#include "tree_types.h"
#include "tree_commands.h"

//——————————————————————————————————————————————————————————————————————————————————————————

MathErr_t MathSimplify          (MathCtx_t* math_ctx);
MathErr_t MathDeleteNeutral     (MathCtx_t* math_ctx);
MathErr_t MathConvoluteConsts   (MathCtx_t* math_ctx);

//——————————————————————————————————————————————————————————————————————————————————————————

#endif /* MATH_SIMPLIFY_H */
