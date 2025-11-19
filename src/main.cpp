#include "tree_commands.h"
#include "data_read.h"
#include "math_funcs.h"

//------------------------------------------------------------------------------------------

int main()
{
    MathCtx_t math_ctx = {};

    if (MathCtxCtor(&math_ctx, 0))
        return EXIT_FAILURE;

    do {
        if (TreeReadData(&math_ctx, "data/data.txt"))
            break;
    } while (0);

    if (MathCtxDtor(&math_ctx))
        return EXIT_FAILURE;

    return EXIT_SUCCESS;
}

//------------------------------------------------------------------------------------------
