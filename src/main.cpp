#include "tree_commands.h"
#include "data_read.h"
#include "text_parse.h"
#include "funcs.h"
#include "eval.h"
#include "diff.h"
#include "simplify.h"

//------------------------------------------------------------------------------------------

int main()
{
    MathCtx_t math_ctx = {};
    MathCtx_t diff_math_ctx = {};

    TreeOpenLogFile();
    MathOpenTexFile();

    do {
        MathTexChapter("Обычная функция");

        if (MathCtxCtor(&math_ctx, 0))
            break;

        if (TreeReadInputData(&math_ctx))
            break;

        if (MathSimplify(&math_ctx))
            break;

        double result = 0;

        if (MathEvaluate(&math_ctx, &result))
            break;

        MathTexChapter("Первая производная");

        if (MathCtxCtor(&diff_math_ctx, 0))
            break;

        if (MathDifferentiate(&math_ctx, &diff_math_ctx, "x"))
            break;

        if (MathSimplify(&diff_math_ctx))
            break;

        if (MathVarsTableDump(&diff_math_ctx, "diff vars table"))
            break;
    } while (0);

    MathCtxDtor(&diff_math_ctx);
    MathCtxDtor(&math_ctx);

    TreeCloseLogFile();

    MathCloseTexFile();

    return EXIT_SUCCESS;
}

//------------------------------------------------------------------------------------------
