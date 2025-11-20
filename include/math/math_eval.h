#ifndef MATH_EVAL_H
#define MATH_EVAL_H

//——————————————————————————————————————————————————————————————————————————————————————————

#include "math_types.h"
#include "math_funcs.h"
#include "math_tex.h"
#include <math.h>

//——————————————————————————————————————————————————————————————————————————————————————————

MathErr_t MathEvaluate              (MathCtx_t* math_ctx, double* presult);
MathErr_t MathGetVariables          (MathCtx_t* math_ctx);
MathErr_t MathEvalNode              (MathCtx_t* math_ctx, TreeNode_t* node, double* result);
MathErr_t MathEvalNodeOpCase        (MathCtx_t* math_ctx, TreeNode_t* node, double* result);
MathErr_t MathEvalNodeUnaryOpCase   (MathCtx_t* math_ctx, TreeNode_t* node, double* result);
MathErr_t MathEvalNodeBinaryOpCase  (MathCtx_t* math_ctx, TreeNode_t* node, double* result);

MathErr_t MathExecuteBinaryOperation(MathOp_t operation, double left_result,
                                    double right_result, double* result);

MathErr_t MathExecuteUnaryOperation(MathOp_t operation, double argument, double* result);

//——————————————————————————————————————————————————————————————————————————————————————————

#endif /* MATH_EVAL_H */
