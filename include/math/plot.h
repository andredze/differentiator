#ifndef MATH_PLOT_H
#define MATH_PLOT_H

//——————————————————————————————————————————————————————————————————————————————————————————

#include "math_types.h"
#include "tree_types.h"
#include "tree_commands.h"
#include "funcs.h"
#include "eval.h"
#include <stdarg.h>

//——————————————————————————————————————————————————————————————————————————————————————————

MathErr_t MathPlotDumpPoints        (MathCtx_t*    math_ctx, FuncParams_t* params);
MathErr_t MathPlotConvertGraphic    (FuncParams_t* params);

//——————————————————————————————————————————————————————————————————————————————————————————

const char* const PLOT_SCRIPT_FILE_NAME = "plot_script.txt";
const char* const GRAPHIC_PDF_FILE_NAME = "graphics.pdf";
const char* const GRAPHIC_FILE_NAME     = "graphic.txt";
const double      PLOT_X_INCREMENT      = 0.001;

//——————————————————————————————————————————————————————————————————————————————————————————

#endif /* MATH_PLOT_H */
