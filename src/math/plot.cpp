#include "plot.h"

//------------------------------------------------------------------------------------------

static int graphs_count = 0;

//------------------------------------------------------------------------------------------

static MathErr_t MathPlotMakeScript(FuncParams_t* params);
static void      MathPlotRunScript ();

//------------------------------------------------------------------------------------------

MathErr_t MathPlotDumpPoints(MathCtx_t* math_ctx, FuncParams_t* params)
{
    assert(math_ctx != NULL);
    assert(params   != NULL);

    DPRINTF("DUMPING POINTS FOR PLOT\n");

    FILE* fp = fopen(GRAPHIC_FILE_NAME, graphs_count == 0 ? "w" : "a");

    if (fp == NULL)
    {
        PRINTERR("Opening file %s failed", GRAPHIC_FILE_NAME);
        return MATH_FILE_ERROR;
    }

    MathErr_t error = MATH_SUCCESS;

    size_t var_ind = 0;

    if ((error = MathGetVarIndex(math_ctx, params->diff_var, &var_ind)))
        return error;

    for (double x = params->x_left; x <= params->x_right; x += PLOT_X_INCREMENT)
    {
        double y = 0.0;

        if ((error = MathVarSetValue(math_ctx, x, var_ind)))
            return error;

        if ((error = MathEvaluateWSetValues(math_ctx, &y, 0)))
            return error;

        DPRINTF("x = %lg; y = %lg\n", x, y);

        fprintf(fp, "%lg %lg\n", x, y);
    }

    fprintf(fp, "\n\n\n");

    fclose(fp);

    graphs_count++;

    DPRINTF("DUMPED POINTS FOR PLOT\n");

    return MATH_SUCCESS;
}

//------------------------------------------------------------------------------------------

MathErr_t MathPlotConvertGraphic(FuncParams_t* params)
{
    MathErr_t error = MATH_SUCCESS;

    if ((error = MathPlotMakeScript(params)))
        return error;

    MathPlotRunScript();

    return MATH_SUCCESS;
}

//------------------------------------------------------------------------------------------

static MathErr_t MathPlotMakeScript(FuncParams_t* params)
{
    assert(params != NULL);

    DPRINTF("MAKING SCRIPT FOR PLOT\n");

    FILE* fp = fopen(PLOT_SCRIPT_FILE_NAME, "w");

    if (fp == NULL)
    {
        PRINTERR("Opening file %s failed", PLOT_SCRIPT_FILE_NAME);
        return MATH_FILE_ERROR;
    }

    fprintf(fp,
R"(set terminal pdf
set output "graphics.pdf"
set title "График анализа функции"
set xlabel "X"
set ylabel "Y"
set grid
set xrange [%lg:%lg]
set yrange [%lg:%lg]
set key opaque
set key box

plot "graphic.txt" index 0 with linespoints lt rgb "red" pt 0 ps 0.5 title "Функция", \
"graphic.txt" index 1 with linespoints lt rgb "green" pt 0 ps 0.5 title "Производная", \
"graphic.txt" index 2 with linespoints lt rgb "blue" pt 0 ps 0.5 title "Ряд Тейлора")",
    params->x_left, params->x_right,
    params->y_left, params->y_right);

    fclose(fp);

    DPRINTF("MADE SCRIPT FOR PLOT\n");

    return MATH_SUCCESS;
}

//------------------------------------------------------------------------------------------

static void MathPlotRunScript()
{
    char command[2048] = {};

    snprintf(command, sizeof(command), "gnuplot %s", PLOT_SCRIPT_FILE_NAME);

    DPRINTF("command = %s\n", command);

    system(command);
}

//------------------------------------------------------------------------------------------
