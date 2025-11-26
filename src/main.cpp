#include "tree_commands.h"
#include "data_read.h"
#include "math_funcs.h"
#include "math_eval.h"
#include "math_diff.h"
#include "math_simplify.h"

//------------------------------------------------------------------------------------------

int main()
{
    MathCtx_t math_ctx = {};
    MathCtx_t diff_math_ctx = {};

    TreeOpenLogFile();

    do {
        if (MathOpenTexFile())
            break;

        if (MathCtxCtor(&math_ctx, 0))
            break;

        if (TreeReadInputData(&math_ctx))
            break;

        if (MathConvoluteConsts(&math_ctx))
            break;

        if (MathDeleteNeutral(&math_ctx))
            break;

        if (MathCtxCtor(&diff_math_ctx, 0))
            break;

        if (MathCtxTexDump(&math_ctx, "Main dump"))
            break;

        if (MathDifferentiate(&math_ctx, &diff_math_ctx, "x"))
            break;

        if (MathConvoluteConsts(&diff_math_ctx))
            break;

        if (MathVarsTableDump(&diff_math_ctx, "diff vars table"))
            break;

//         double result = 0.0;
//
//         if (MathEvaluate(&math_ctx, &result))
//             break;
//
//         printf("result = %lg\n", result);

    } while (0);

    MathCloseTexFile();
    MathCtxDtor(&diff_math_ctx);
    MathCtxDtor(&math_ctx);

    TreeCloseLogFile();

    return EXIT_SUCCESS;
}

//------------------------------------------------------------------------------------------
