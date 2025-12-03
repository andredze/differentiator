#include "eval.h"

//——————————————————————————————————————————————————————————————————————————————————————————

static MathErr_t MathGetVarValues          (MathCtx_t* math_ctx);
static MathErr_t MathEvalNode              (MathCtx_t* math_ctx, TreeNode_t* node, double* result);
static MathErr_t MathEvalNodeOpCase        (MathCtx_t* math_ctx, TreeNode_t* node, double* result);
static MathErr_t MathEvalNodeUnaryOpCase   (MathCtx_t* math_ctx, TreeNode_t* node, double* result);
static MathErr_t MathEvalNodeBinaryOpCase  (MathCtx_t* math_ctx, TreeNode_t* node, double* result);
static MathErr_t MathExecuteUnaryOperation (MathOp_t operation,  double argument,  double* result);

//——————————————————————————————————————————————————————————————————————————————————————————

MathErr_t MathEvaluate(MathCtx_t* math_ctx, double* presult)
{
    assert(math_ctx != NULL);
    assert(presult  != NULL);

    MathTexSection("Вычисление значений");
    MathTexMessage("Заметим, что");

    double result = 0.0;

    MathErr_t error = MATH_SUCCESS;

    if ((error = MathGetVarValues(math_ctx)))
        return error;

    if ((error = MathEvalNode(math_ctx, math_ctx->tree.dummy->right, &result)))
        return error;

    *presult = result;

    MathTexEval(math_ctx, *presult);

    return MATH_SUCCESS;
}

//------------------------------------------------------------------------------------------

MathErr_t MathEvaluateWSetValues(MathCtx_t* math_ctx, double* presult)
{
    assert(math_ctx != NULL);
    assert(presult  != NULL);

    double result = 0.0;

    MathErr_t error = MATH_SUCCESS;

    if ((error = MathEvalNode(math_ctx, math_ctx->tree.dummy->right, &result)))
        return error;

    *presult = result;

    MathTexEval(math_ctx, *presult);

    return MATH_SUCCESS;
}

//------------------------------------------------------------------------------------------

static MathErr_t MathGetVarValues(MathCtx_t* math_ctx)
{
    assert(math_ctx != NULL);

    for (size_t i = 0; i < math_ctx->vars.size; i++)
    {
        printf("Введите значение переменной %s: ", math_ctx->vars.data[i].str);

        double variable = 0.0;

        if (scanf("%lg", &variable) != 1)
        {
            PRINTERR("Invalid input for variable");
            return MATH_INVALID_INPUT;
        }

        getchar();

        math_ctx->vars.data[i].value = variable;
    }

    return MATH_SUCCESS;
}

//------------------------------------------------------------------------------------------

static MathErr_t MathEvalNode(MathCtx_t* math_ctx, TreeNode_t* node, double* result)
{
    assert(math_ctx != NULL);
    assert(result   != NULL);
    assert(node     != NULL);

    switch (node->data.type)
    {
        case TYPE_NUM:
            *result = node->data.value.num;
            return MATH_SUCCESS;

        case TYPE_VAR:
            *result = math_ctx->vars.data[node->data.value.var].value;
            return MATH_SUCCESS;

        case TYPE_OP:
            return MathEvalNodeOpCase(math_ctx, node, result);

        default:
            return MATH_UNKNOWN_OP;
    }

    return MATH_SUCCESS;
}

//------------------------------------------------------------------------------------------

static MathErr_t MathEvalNodeOpCase(MathCtx_t* math_ctx, TreeNode_t* node, double* result)
{
    assert(math_ctx != NULL);
    assert(result   != NULL);
    assert(node     != NULL);

    int args_count = OP_CASES_TABLE[node->data.value.op].args_count;

    switch (args_count)
    {
        case 1:
            return MathEvalNodeUnaryOpCase(math_ctx, node, result);

        case 2:
            return MathEvalNodeBinaryOpCase(math_ctx, node, result);

        default:
            PRINTERR("Operation has invalid number of arguments: %d", args_count);
            return MATH_UNKNOWN_OP;
    }

    return MATH_SUCCESS;
}

//------------------------------------------------------------------------------------------

static MathErr_t MathEvalNodeUnaryOpCase(MathCtx_t* math_ctx, TreeNode_t* node, double* result)
{
    assert(math_ctx != NULL);
    assert(result   != NULL);
    assert(node     != NULL);

    double right_result = 0.0;

    MathErr_t error = MATH_SUCCESS;

    if (node->right != NULL)
    {
        if ((error = MathEvalNode(math_ctx, node->right, &right_result)))
            return error;
    }

    return MathExecuteUnaryOperation(node->data.value.op, right_result, result);
}

//------------------------------------------------------------------------------------------

static MathErr_t MathEvalNodeBinaryOpCase(MathCtx_t* math_ctx, TreeNode_t* node, double* result)
{
    assert(math_ctx != NULL);
    assert(result   != NULL);
    assert(node     != NULL);

    double left_result  = 0.0;
    double right_result = 0.0;

    MathErr_t error = MATH_SUCCESS;

    if (node->left != NULL)
    {
        if ((error = MathEvalNode(math_ctx, node->left, &left_result)))
            return error;
    }
    if (node->right != NULL)
    {
        if ((error = MathEvalNode(math_ctx, node->right, &right_result)))
            return error;
    }

    // TODO: tex dump с подстановкой значений
    // MathCtxTexDump(math_ctx, "evaluated binary op at %p", node);

    return MathExecuteBinaryOperation(node->data.value.op, left_result, right_result, result);
}

//------------------------------------------------------------------------------------------

MathErr_t MathExecuteBinaryOperation(MathOp_t operation,
                                     double  left_result,
                                     double  right_result,
                                     double* result)
{
    assert(result != NULL);

    switch (operation)
    {
        case OP_ADD:
            *result = left_result + right_result;
            break;

        case OP_SUB:
            *result = left_result - right_result;
            break;

        case OP_MUL:
            *result = left_result * right_result;
            break;

        case OP_DIV:
            *result = left_result / right_result;
            break;

        case OP_DEG:
            *result = pow(left_result, right_result);
            break;

        case OP_SIN:
        case OP_COS:
        case OP_TG:
        case OP_CTG:
        case OP_LN:
            PRINTERR("Given operation is unary");
            return MATH_UNKNOWN_OP;

        case OP_UNKNOWN:
        default:
            PRINTERR("Unknown binary op");
            return MATH_UNKNOWN_OP;
    }

    return MATH_SUCCESS;
}

//------------------------------------------------------------------------------------------

static MathErr_t MathExecuteUnaryOperation(MathOp_t operation,
                                           double  argument,
                                           double* result)
{
    assert(result != NULL);

    switch (operation)
    {
        case OP_SIN:
            *result = sin(argument);
            break;

        case OP_COS:
            *result = cos(argument);
            break;

        case OP_TG:
            *result = tan(argument);
            break;

        case OP_CTG:
            *result = 1 / tan(argument);
            break;

        case OP_LN:
            *result = log(argument);
            break;

        case OP_ADD:
        case OP_SUB:
        case OP_MUL:
        case OP_DIV:
        case OP_DEG:
            PRINTERR("Given operation is binary");
            return MATH_UNKNOWN_OP;

        case OP_UNKNOWN:
        default:
            PRINTERR("Unknown unary op");
            return MATH_UNKNOWN_OP;
    }

    return MATH_SUCCESS;
}

//------------------------------------------------------------------------------------------
