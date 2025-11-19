#ifndef MATH_EXPR_H
#define MATH_EXPR_H

//——————————————————————————————————————————————————————————————————————————————————————————

#include "tree_commands.h"
#include "math_types.h"

//——————————————————————————————————————————————————————————————————————————————————————————

MathErr_t MathCtxCtor           (MathCtx_t* math_ctx, size_t vars_capacity);
MathErr_t MathVarsTableRealloc  (MathCtx_t* math_ctx);
MathErr_t MathCtxDtor           (MathCtx_t* math_ctx);

size_t GetHash(const char* str);

//——————————————————————————————————————————————————————————————————————————————————————————

const int VARS_MIN_COUNT = 64;

//——————————————————————————————————————————————————————————————————————————————————————————

#endif /* MATH_EXPR_H */
