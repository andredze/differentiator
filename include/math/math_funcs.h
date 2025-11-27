#ifndef MATH_EXPR_H
#define MATH_EXPR_H

//——————————————————————————————————————————————————————————————————————————————————————————

#include "tree_commands.h"
#include "math_types.h"
#include "math_tex.h"

//——————————————————————————————————————————————————————————————————————————————————————————

MathErr_t   MathCtxCtor           (MathCtx_t* math_ctx, size_t vars_capacity);
MathErr_t   MathVarsTableRealloc  (MathCtx_t* math_ctx);
MathErr_t   MathCtxDtor           (MathCtx_t* math_ctx);
TreeNode_t* MathNodeCtor          (MathCtx_t* math_ctx, MathData_t data, TreeNode_t* left, TreeNode_t* right);
TreeNode_t* MathCopySubtree       (MathCtx_t* math_ctx, TreeNode_t* node, MathCtx_t* src_math_ctx);
MathErr_t   MathAddVarToTable     (MathCtx_t* math_ctx, const char* var_str);
int         MathVarInTable        (MathCtx_t* math_ctx, const char* var_str);

//——————————————————————————————————————————————————————————————————————————————————————————

const int VARS_MIN_COUNT = 4;

//——————————————————————————————————————————————————————————————————————————————————————————

#endif /* MATH_EXPR_H */
