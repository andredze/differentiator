#include "tree_commands.h"
#include "data_read.h"
#include "math_funcs.h"
#include "math_eval.h"
#include "math_diff.h"

//------------------------------------------------------------------------------------------

int main()
{
    MathCtx_t math_ctx = {};
    MathCtx_t diff_math_ctx = {};

    do {
        if (MathCtxCtor(&math_ctx, 0))
            break;

        if (MathCtxCtor(&diff_math_ctx, 0))
            break;

        if (TreeReadData(&math_ctx, "data/sin.txt"))
            break;

        if (MathCtxTexDump(&math_ctx, "Main dump"))
            break;

        if (MathDifferentiate(&math_ctx, &diff_math_ctx, "+"))
            break;

//         double result = 0.0;
//
//         if (MathEvaluate(&math_ctx, &result))
//             break;
//
//         printf("result = %lg\n", result);

    } while (0);

    MathCtxDtor(&diff_math_ctx);
    MathCtxDtor(&math_ctx);

    return EXIT_SUCCESS;
}

//------------------------------------------------------------------------------------------
