#include "plot.h"

//------------------------------------------------------------------------------------------

static int graphs_count = 0;

//------------------------------------------------------------------------------------------

static MathErr_t MathOpenPlotFile(MathCtx_t* math_ctx)
{
    snprintf(math_ctx->plot_file_name, MAX_FILE_NAME_LEN, "graph_%03d.txt", graphs_count);

    math_ctx->plot_fp = fopen(math_ctx->plot_file_name, "w");

    if (math_ctx->plot_fp == NULL)
    {
        PRINTERR("Opening file %s failed", math_ctx->plot_file_name);
        return MATH_FILE_ERROR;
    }

    PlotPrintTitle(math_ctx);

    return MATH_SUCCESS;
}

//------------------------------------------------------------------------------------------

static MathErr_t MathOpenPlotFile(MathCtx_t* math_ctx)
{
    PlotPrintEnding();

    fclose(fp);

    PlotConvertToPng();

    return MATH_SUCCESS;
}

//------------------------------------------------------------------------------------------
