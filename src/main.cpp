#include "tree_commands.h"
#include "data_read.h"
#include "math_funcs.h"
#include "math_eval.h"

//------------------------------------------------------------------------------------------

int main()
{
    MathCtx_t math_ctx = {};

    if (MathCtxCtor(&math_ctx, 0))
        return EXIT_FAILURE;

    do {
        if (TreeReadData(&math_ctx, "data/data.txt"))
            break;

        double result = 0.0;

        if (MathEvaluate(&math_ctx, &result))
            break;

        printf("result = %lg\n", result);

    } while (0);

    if (MathCtxDtor(&math_ctx))
        return EXIT_FAILURE;

    return EXIT_SUCCESS;
}

//------------------------------------------------------------------------------------------
