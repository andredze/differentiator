#include "taylor.h"

//——————————————————————————————————————————————————————————————————————————————————————————

#define NUM_(number)       MathNodeCtor(taylor_ctx, {TYPE_NUM, { .num = (number) }}, NULL, NULL)

#define VAR_(var_index)    MathNodeCtor(taylor_ctx, {TYPE_VAR, { .var = var_index }}, NULL, NULL)

#define ADD_(lnode, rnode) MathNodeCtor(taylor_ctx, {TYPE_OP,  { .op  = OP_ADD }}, (lnode), (rnode))
#define MUL_(lnode, rnode) MathNodeCtor(taylor_ctx, {TYPE_OP,  { .op  = OP_MUL }}, (lnode), (rnode))
#define DIV_(lnode, rnode) MathNodeCtor(taylor_ctx, {TYPE_OP,  { .op  = OP_DIV }}, (lnode), (rnode))
#define DEG_(lnode, rnode) MathNodeCtor(taylor_ctx, {TYPE_OP,  { .op  = OP_DEG }}, (lnode), (rnode))

//——————————————————————————————————————————————————————————————————————————————————————————

static int         GetFactorial             (int value);

static TreeNode_t* MathTaylorSeriesGetElem  (MathCtx_t* taylor_ctx, MathCtx_t* curr_deriv_ctx,
                                             int diff_degree, double point);

static MathErr_t   MathTaylorSetValue       (MathCtx_t* math_ctx, double point, size_t var_index);
static MathErr_t   MathGetVarIndex          (MathCtx_t* math_ctx, const char* str_var, size_t* var_ind);

//------------------------------------------------------------------------------------------

MathErr_t MathGetTaylorSeries(MathCtx_t* math_ctx, MathCtx_t* taylor_ctx,
                              const char* str_var, int last_diff_degree,
                              double point)
{
    assert(math_ctx != NULL);
    assert(str_var  != NULL);

    DPRINTF("Taylor series\n");

    MathErr_t error = MATH_SUCCESS;

    if ((error = MathAddVarToTable(taylor_ctx, str_var)))
        return error;

    DPRINTF("Added var to taylor_ctx\n");

    MathCtx_t prev_deriv_ctx = {};

    prev_deriv_ctx.tree.dummy = MathCopySubtree(&prev_deriv_ctx, math_ctx->tree.dummy, math_ctx);
    DPRINTF("Copied zero deriv\n");

    if ((error = MathAddVarToTable(&prev_deriv_ctx, str_var)))
        return error;
    DPRINTF("Added var to zero deriv\n");

    MATH_VARS_DUMP(&prev_deriv_ctx, "added diff var to zero deriv");

    if ((error = MathTaylorSetValue(&prev_deriv_ctx, point, 0)))
        return error;

    DPRINTF("Set var value to zero deriv\n");
    MATH_VARS_DUMP(&prev_deriv_ctx, "set var value to zero deriv");

    TREE_CALL_DUMP(&prev_deriv_ctx, "TAYLOR SERIES: ZERO DERIV TREE");

    double value_in_point = 0.0;

    if ((error = MathEvaluateWSetValues(&prev_deriv_ctx, &value_in_point)))
        return error;

    DPRINTF("Counted value of zero deriv in %lg = %lg\n", point, value_in_point);

    MathCtx_t curr_deriv_ctx = {};

    TreeNode_t* node = NUM_(value_in_point);

    DPRINTF("Created node of zero deriv in taylor\n");

    for (int diff_degree = 1; diff_degree <= last_diff_degree; diff_degree++)
    {
        if ((error = MathCtxCtor(&curr_deriv_ctx, 1)))
            return error;

        if ((error = MathAddVarToTable(&curr_deriv_ctx, str_var)))
            return error;

        if ((error = MathDifferentiate(&prev_deriv_ctx, &curr_deriv_ctx, str_var)))
            return error;

        if (MathSimplify(&curr_deriv_ctx))
            break;

        MathCtxDtor(&prev_deriv_ctx);

        node = ADD_(node, MathTaylorSeriesGetElem(taylor_ctx, &curr_deriv_ctx,
                                                  diff_degree, point));
        MathTexDumpSubtree(node, taylor_ctx);

        TREE_CALL_DUMP(&curr_deriv_ctx, "TAYLOR SERIES: %d DERIV TREE", diff_degree);

        prev_deriv_ctx = curr_deriv_ctx;
    }

    taylor_ctx->tree.dummy->right = node;
    node->parent = taylor_ctx->tree.dummy;

    if (((error = MathSimplify(taylor_ctx))))
        return error;

    MathTexDumpSubtree(node, taylor_ctx);
    TREE_CALL_DUMP(taylor_ctx, "TAYLOR SERIES TREE");

    MathCtxDtor(&curr_deriv_ctx);

    return MATH_SUCCESS;
}

//------------------------------------------------------------------------------------------

static MathErr_t MathTaylorSetValue(MathCtx_t* math_ctx, double point, size_t var_index)
{
    assert(math_ctx != NULL);

    DPRINTF("math_ctx->vars.size = %zu; var_index = %zu;\n",
             math_ctx->vars.size, var_index);

    if (math_ctx->vars.size <= var_index)
    {
        PRINTERR("Variable is not in table");
        return MATH_INVALID_INPUT;
    }

    math_ctx->vars.data[var_index].value = point;

    return MATH_SUCCESS;
}

//------------------------------------------------------------------------------------------

static MathErr_t MathGetVarIndex(MathCtx_t* math_ctx, const char* str_var, size_t* var_ind)
{
    assert(math_ctx != NULL);
    assert(str_var  != NULL);
    assert(var_ind  != NULL);

    for (size_t i = 0; i < math_ctx->vars.size; i++)
    {
        if (strcmp(str_var, math_ctx->vars.data[i].str) == 0)
        {
            *var_ind = i;
            return MATH_SUCCESS;
        }
    }

    PRINTERR("Variable \"%s\" is not in table", str_var);
    return MATH_INVALID_INPUT;
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

static TreeNode_t* MathTaylorSeriesGetElem(MathCtx_t* taylor_ctx, MathCtx_t* curr_deriv_ctx,
                                           int diff_degree, double point)
{
    assert(curr_deriv_ctx != NULL);
    assert(taylor_ctx     != NULL);

    double value_in_point = 0.0;

    if (MathTaylorSetValue(curr_deriv_ctx, point, 0))
        return NULL;

    if (MathEvaluateWSetValues(curr_deriv_ctx, &value_in_point))
        return NULL;

    int fact_res = GetFactorial(diff_degree);

    if (fact_res == -1)
        return NULL;

    double coeff = value_in_point / fact_res;

    TreeNode_t* node = MUL_(NUM_(coeff),
                            DEG_(VAR_(0), NUM_(diff_degree)));

    MathTexDumpSubtree(node, taylor_ctx);

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
