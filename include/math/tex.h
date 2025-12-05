#ifndef MATH_TEX_H
#define MATH_TEX_H

//——————————————————————————————————————————————————————————————————————————————————————————

#include "math_types.h"
#include "tree_types.h"
#include "tree_commands.h"
#include "funcs.h"
#include "plot.h"
#include <stdarg.h>
#include <math.h>

//——————————————————————————————————————————————————————————————————————————————————————————

MathErr_t MathOpenTexFile           ();
MathErr_t MathCloseTexFile          ();
void      MathTexSection            (const char* fmt, ...);
void      MathTexChapter            (const char* fmt, ...);
void      MathTexMessage            (const char* fmt, ...);
void      MathTexEval               (MathCtx_t* math_ctx, double result);
MathErr_t MathCtxTexDump            (MathCtx_t* math_ctx, const char* fmt, ...);
MathErr_t vMathCtxTexDump           (MathCtx_t* math_ctx, const char* fmt, va_list args);
void      MathTexDumpSubtree        (TreeNode_t* node, MathCtx_t* math_ctx);
void      MathTexDumpFuncSubtree    (TreeNode_t* node, MathCtx_t* math_ctx);
void      MathTexDumpDiffSubtree    (TreeNode_t* node, TreeNode_t* diff_node, MathCtx_t* math_ctx);
void      MathTexDumpTaylor         (MathCtx_t* math_ctx, TreeNode_t* node, int diff_degree, double point);
MathErr_t MathTexGraphic            (MathCtx_t* math_ctx,        MathCtx_t* diff_math_ctx,
                                     MathCtx_t* taylor_math_ctx, MathCtx_t* tangent_ctx,
                                     FuncParams_t* params);

//——————————————————————————————————————————————————————————————————————————————————————————

const char TEX_FILE_NAME[] = "math_log.tex";
const int  MAX_MESSAGE_LEN = 200;

//——————————————————————————————————————————————————————————————————————————————————————————

#endif /* MATH_TEX_H */
