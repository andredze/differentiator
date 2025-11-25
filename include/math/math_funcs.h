#ifndef MATH_EXPR_H
#define MATH_EXPR_H

//——————————————————————————————————————————————————————————————————————————————————————————

#include "tree_commands.h"
#include "math_types.h"
#include "math_tex.h"

//——————————————————————————————————————————————————————————————————————————————————————————

MathErr_t MathCtxCtor           (MathCtx_t* math_ctx, size_t vars_capacity);
MathErr_t MathVarsTableRealloc  (MathCtx_t* math_ctx);
MathErr_t MathCtxDtor           (MathCtx_t* math_ctx);

size_t GetHash(const char* str);

int VarCaseCompare(const void* par1, const void* par2);

//——————————————————————————————————————————————————————————————————————————————————————————

const int VARS_MIN_COUNT = 4;

//——————————————————————————————————————————————————————————————————————————————————————————

#endif /* MATH_EXPR_H */
