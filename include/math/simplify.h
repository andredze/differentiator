#ifndef MATH_SIMPLIFY_H
#define MATH_SIMPLIFY_H

//——————————————————————————————————————————————————————————————————————————————————————————

#include "math_types.h"
#include "eval.h"
#include "tree_types.h"
#include "tree_commands.h"
#include <stdlib.h>

//——————————————————————————————————————————————————————————————————————————————————————————

MathErr_t MathSimplify          (MathCtx_t* math_ctx, int tex_dump);
MathErr_t MathDeleteNeutral     (MathCtx_t* math_ctx);
MathErr_t MathConvoluteConsts   (MathCtx_t* math_ctx);

//——————————————————————————————————————————————————————————————————————————————————————————

const int PROB_COEFF = 5;

//——————————————————————————————————————————————————————————————————————————————————————————

#endif /* MATH_SIMPLIFY_H */
