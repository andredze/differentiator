#ifndef MATH_TAYLOR_H
#define MATH_TAYLOR_H

//——————————————————————————————————————————————————————————————————————————————————————————

#include "math_types.h"
#include "tree_types.h"
#include "tree_commands.h"
#include "funcs.h"
#include "eval.h"
#include "diff.h"
#include "simplify.h"
#include <stdarg.h>

//——————————————————————————————————————————————————————————————————————————————————————————

typedef struct SeriesCtx
{
    MathCtx_t*    original_ctx;
    MathCtx_t*    prev_deriv_ctx;
    MathCtx_t*    curr_deriv_ctx;
    MathCtx_t*    taylor_ctx;

    int           diff_degree;
    FuncParams_t* params;

    TreeNode_t*   node;

} SeriesCtx_t;

//——————————————————————————————————————————————————————————————————————————————————————————

MathErr_t MathGetTaylorSeries(MathCtx_t*    math_ctx, MathCtx_t* taylor_ctx,
                              FuncParams_t* params);

//——————————————————————————————————————————————————————————————————————————————————————————

#endif /* MATH_TAYLOR_H */
