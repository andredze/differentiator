#include "taylor.h"

//——————————————————————————————————————————————————————————————————————————————————————————

#define NUM_(number)       MathNodeCtor(taylor_ctx, {TYPE_NUM, { .num = (number) }}, NULL, NULL)

#define VAR_(var_index)    MathNodeCtor(taylor_ctx, {TYPE_VAR, { .var = var_index }}, NULL, NULL)

#define ADD_(lnode, rnode) MathNodeCtor(taylor_ctx, {TYPE_OP,  { .op  = OP_ADD }}, (lnode), (rnode))
#define MUL_(lnode, rnode) MathNodeCtor(taylor_ctx, {TYPE_OP,  { .op  = OP_MUL }}, (lnode), (rnode))
#define DIV_(lnode, rnode) MathNodeCtor(taylor_ctx, {TYPE_OP,  { .op  = OP_DIV }}, (lnode), (rnode))
#define DEG_(lnode, rnode) MathNodeCtor(taylor_ctx, {TYPE_OP,  { .op  = OP_DEG }}, (lnode), (rnode))

//——————————————————————————————————————————————————————————————————————————————————————————

static int         GetFactorial               (int value);
static MathErr_t   MathTaylorGetFirstElem     (SeriesCtx_t* series_ctx);
static MathErr_t   MathTaylorGetOneElem       (SeriesCtx_t* series_ctx);
static TreeNode_t* MathTaylorAddElemToTree    (SeriesCtx_t* series_ctx);

//------------------------------------------------------------------------------------------

MathErr_t MathGetTaylorSeries(MathCtx_t*    math_ctx, MathCtx_t* taylor_ctx,
                              FuncParams_t* params)
{
    assert(math_ctx != NULL);
    assert(params   != NULL);

    MathCtx_t prev_deriv_ctx = {};
    MathCtx_t curr_deriv_ctx = {};

    SeriesCtx_t series_ctx = { .original_ctx   = math_ctx,
                               .prev_deriv_ctx = &prev_deriv_ctx,
                               .curr_deriv_ctx = &curr_deriv_ctx,
                               .taylor_ctx     = taylor_ctx,
                               .params         = params};

    DPRINTF("Taylor series\n");

    MathErr_t error = MATH_SUCCESS;

    if ((error = MathTaylorGetFirstElem(&series_ctx)))
        return error;

    int last_diff_degree = params->taylor_degree;

    for (series_ctx.diff_degree = 1; series_ctx.diff_degree <= last_diff_degree; series_ctx.diff_degree++)
    {
        if ((error = MathTaylorGetOneElem(&series_ctx)))
            return error;
    }

    taylor_ctx->tree.dummy->right = series_ctx.node;
    series_ctx.node->parent = taylor_ctx->tree.dummy;

    if (((error = MathSimplify(taylor_ctx))))
        return error;

    TREE_CALL_DUMP(taylor_ctx, "TAYLOR SERIES TREE");

    MathTexMessage("Итоговое разложение в ряд Тейлора");
    MathTexDumpTaylor(taylor_ctx, taylor_ctx->tree.dummy->right, last_diff_degree);

    MathCtxDtor(series_ctx.curr_deriv_ctx);

    return MATH_SUCCESS;
}

//------------------------------------------------------------------------------------------

static MathErr_t MathTaylorGetFirstElem(SeriesCtx_t* series_ctx)
{
    assert(series_ctx != NULL);

    MathCtx_t*    original_ctx   = series_ctx->original_ctx;
    MathCtx_t*    prev_deriv_ctx = series_ctx->prev_deriv_ctx;
    MathCtx_t*    taylor_ctx     = series_ctx->taylor_ctx;
    FuncParams_t* params         = series_ctx->params;
    MathErr_t     error          = MATH_SUCCESS;

    if ((error = MathAddVarToTable(taylor_ctx, params->diff_var)))
        return error;

    prev_deriv_ctx->tree.dummy = MathCopySubtree(prev_deriv_ctx, original_ctx->tree.dummy, original_ctx);

    if ((error = MathAddVarToTable(prev_deriv_ctx, params->diff_var)))
        return error;

    if ((error = MathVarSetValue(prev_deriv_ctx, params->taylor_point, 0)))
        return error;

    TREE_CALL_DUMP(prev_deriv_ctx, "TAYLOR SERIES: ZERO DERIV TREE");

    double value_in_point = 0.0;

    if ((error = MathEvaluateWSetValues(prev_deriv_ctx, &value_in_point, 1)))
        return error;

    series_ctx->node = NUM_(value_in_point);

    return MATH_SUCCESS;
}

//------------------------------------------------------------------------------------------

static MathErr_t MathTaylorGetOneElem(SeriesCtx_t* series_ctx)
{
    assert(series_ctx != NULL);

    MathErr_t  error          = MATH_SUCCESS;
    MathCtx_t* curr_deriv_ctx = series_ctx->curr_deriv_ctx;
    MathCtx_t* prev_deriv_ctx = series_ctx->prev_deriv_ctx;
    MathCtx_t* taylor_ctx     = series_ctx->taylor_ctx;
    int        diff_degree    = series_ctx->diff_degree;

    if ((error = MathCtxCtor(curr_deriv_ctx, 1)))
        return error;

    TREE_CALL_DUMP(prev_deriv_ctx, "TAYLOR SERIES: %d PREV DERIV TREE", diff_degree);
    MATH_VARS_DUMP(prev_deriv_ctx, "TAYLOR SERIES: %d PREV DERIV TREE", diff_degree);
    TREE_CALL_DUMP(curr_deriv_ctx, "TAYLOR SERIES: %d CURR DERIV TREE", diff_degree);
    MATH_VARS_DUMP(curr_deriv_ctx, "TAYLOR SERIES: %d CURR DERIV TREE", diff_degree);

    if ((error = MathDifferentiate(prev_deriv_ctx, curr_deriv_ctx, series_ctx->params->diff_var)))
        return error;

    if ((error = MathSimplify(curr_deriv_ctx)))
        return error;

    MathCtxDtor(prev_deriv_ctx);

    series_ctx->node = ADD_(series_ctx->node, MathTaylorAddElemToTree(series_ctx));

    MathTexDumpSubtree(series_ctx->node, taylor_ctx);

    MathTexDumpTaylor(taylor_ctx, series_ctx->node, diff_degree);

    *series_ctx->prev_deriv_ctx = *curr_deriv_ctx;

    return MATH_SUCCESS;
}

//------------------------------------------------------------------------------------------

static int GetFactorial(int value)
{
    if (value > 256 || value < 0)
        return -1;

    if (value == 0)
        return 1;

    int result = 1;

    for (int mult = 2; mult <= value; mult++)
        result *= mult;

    return result;
}

//------------------------------------------------------------------------------------------

static TreeNode_t* MathTaylorAddElemToTree(SeriesCtx_t* series_ctx)
{
    assert(series_ctx != NULL);

    double value_in_point = 0.0;

    if (MathVarSetValue(series_ctx->curr_deriv_ctx, series_ctx->params->taylor_point, 0))
        return NULL;

    if (MathEvaluateWSetValues(series_ctx->curr_deriv_ctx, &value_in_point, 1))
        return NULL;

    int fact_res = GetFactorial(series_ctx->diff_degree);

    if (fact_res == -1)
        return NULL;

    double coeff = value_in_point / fact_res;

    MathCtx_t* taylor_ctx = series_ctx->taylor_ctx;

    TreeNode_t* node = MUL_(NUM_(coeff),
                            DEG_(VAR_(0), NUM_(series_ctx->diff_degree)));

    MathTexDumpSubtree(node, series_ctx->taylor_ctx);

    return node;
}

//------------------------------------------------------------------------------------------

//——————————————————————————————————————————————————————————————————————————————————————————

#undef NUM_

#undef VAR_

#undef ADD_
#undef MUL_
#undef DEG_

//——————————————————————————————————————————————————————————————————————————————————————————
