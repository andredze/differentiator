#include "tree_commands.h"
#include "data_read.h"
#include "text_parse.h"
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

        const char* buffer = "3+5$";
        const char* cur_p  = buffer;

        Expr_t expr = { .buffer = buffer, .cur_p = cur_p };

        if (MathParseText(&math_ctx, &expr))
            break;

        break;

        if (TreeReadInputData(&math_ctx))
            break;

        if (MathSimplify(&math_ctx))
            break;

        double result = 0;

        if (MathEvaluate(&math_ctx, &result))
            break;

        if (MathCtxCtor(&diff_math_ctx, 0))
            break;

        if (MathDifferentiate(&math_ctx, &diff_math_ctx, "x"))
            break;

        if (MathSimplify(&diff_math_ctx))
            break;

        if (MathVarsTableDump(&diff_math_ctx, "diff vars table"))
            break;
    } while (0);

    MathCloseTexFile();
    MathCtxDtor(&diff_math_ctx);
    MathCtxDtor(&math_ctx);

    TreeCloseLogFile();

    return EXIT_SUCCESS;
}

//------------------------------------------------------------------------------------------
