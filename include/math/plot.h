#ifndef MATH_PLOT_H
#define MATH_PLOT_H

//——————————————————————————————————————————————————————————————————————————————————————————

#include "math_types.h"
#include "tree_types.h"
#include "tree_commands.h"
#include "funcs.h"
#include <stdarg.h>

//——————————————————————————————————————————————————————————————————————————————————————————

MathErr_t MathOpenPlotFile           ();
MathErr_t MathClosePlotFile          ();

//——————————————————————————————————————————————————————————————————————————————————————————

const char PLOT_FILE_NAME[] = "math_plot.txt";

//——————————————————————————————————————————————————————————————————————————————————————————

#endif /* MATH_PLOT_H */
