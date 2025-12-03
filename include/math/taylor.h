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

MathErr_t MathGetTaylorSeries(MathCtx_t* math_ctx, MathCtx_t* taylor_ctx,
                              const char* str_var, int last_diff_degree,
                              double point);

//——————————————————————————————————————————————————————————————————————————————————————————

#endif /* MATH_TAYLOR_H */
