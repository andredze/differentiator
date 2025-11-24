#ifndef MATH_TEX_H
#define MATH_TEX_H

//——————————————————————————————————————————————————————————————————————————————————————————

#include "math_types.h"
#include "tree_types.h"
#include "tree_commands.h"
#include "math_funcs.h"
#include <stdarg.h>

//——————————————————————————————————————————————————————————————————————————————————————————

MathErr_t MathOpenTexFile   ();
MathErr_t MathCloseTexFile  ();
MathErr_t MathCtxTexDump    (MathCtx_t* math_ctx, const char* fmt, ...);
MathErr_t vMathCtxTexDump   (MathCtx_t* math_ctx, const char* fmt, va_list args);
void      MathTexDumpSubtree    (TreeNode_t* node, MathCtx_t* math_ctx);
void      MathTexDumpDiffSubtree(TreeNode_t* node, TreeNode_t* diff_node, MathCtx_t* math_ctx);

//——————————————————————————————————————————————————————————————————————————————————————————

const char TEX_FILE_NAME[] = "math_log.tex";

//——————————————————————————————————————————————————————————————————————————————————————————

#endif /* MATH_TEX_H */
