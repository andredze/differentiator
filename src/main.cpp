#include "tree_commands.h"
#include "data_read.h"
#include "text_parse.h"
#include "funcs.h"
#include "eval.h"
#include "diff.h"
#include "simplify.h"
#include "taylor.h"
#include "tangent.h"
#include "plot.h"

//------------------------------------------------------------------------------------------

int main()
{
    MathCtx_t math_ctx        = { .is_dump_enabled = 1 };
    MathCtx_t diff_math_ctx   = { .is_dump_enabled = 1 };
    MathCtx_t taylor_math_ctx = { .is_dump_enabled = 1 };
    MathCtx_t tangent_ctx     = { .is_dump_enabled = 1 };

    TreeOpenLogFile();
    MathOpenTexFile();

    do {
        MathTexChapter("Обычная функция");

        if (MathCtxCtor(&math_ctx, 0))
            break;

        FuncParams_t params = {};

        if (TreeReadInputData(&math_ctx, &params))
            break;

        if (MathSimplify(&math_ctx))
            break;

        MathTexChapter("Первая производная");

        if (MathCtxCtor(&diff_math_ctx, 0))
            break;

        if (MathDifferentiate(&math_ctx, &diff_math_ctx, params.diff_var))
            break;

        if (MathSimplify(&diff_math_ctx))
            break;

        if (math_ctx.vars.size != 1)
            break;

        MathTexChapter("Разложение в ряд Тейлора");

        if (MathCtxCtor(&taylor_math_ctx, 0))
            break;

        if (MathGetTaylorSeries(&math_ctx, &taylor_math_ctx, &params))
            break;

        if (MathCtxCtor(&tangent_ctx, 0))
            break;

        if (MathGetTangent(&math_ctx, &diff_math_ctx, params.taylor_point, &tangent_ctx))
            break;

        if (MathTexGraphic(&math_ctx, &diff_math_ctx, &taylor_math_ctx, &tangent_ctx, &params))
            break;

    } while (0);

    MathCtxDtor(&taylor_math_ctx);
    MathCtxDtor(&diff_math_ctx);
    MathCtxDtor(&tangent_ctx);
    MathCtxDtor(&math_ctx);

    TreeCloseLogFile();

    MathCloseTexFile();

    return EXIT_SUCCESS;
}

//------------------------------------------------------------------------------------------
