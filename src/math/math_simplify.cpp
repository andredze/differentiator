#include "math_simplify.h"

/* ===================== Domain Specific Language for simplification ==================== */

#define ISVALUE_(side, number) (node->side->data.type == TYPE_NUM && \
                                CompareDoubles(node->side->data.value.num, (number)) == 0)

/* ====================================================================================== */

static int       MathNodeIsNum                  (TreeNode_t* node);
static MathErr_t MathConvoluteNodeToNumber      (TreeNode_t* node, MathCtx_t* math_ctx, double number);
static MathErr_t MathConvoluteLeftToCurrent     (TreeNode_t* node, MathCtx_t* math_ctx);
static MathErr_t MathConvoluteRightToCurrent    (TreeNode_t* node, MathCtx_t* math_ctx);
static MathErr_t MathDeleteNeutralNode          (TreeNode_t* node, MathCtx_t* math_ctx);
static MathErr_t MathProcessNeutrals            (TreeNode_t* node, MathCtx_t* math_ctx);
static MathErr_t MathConvoluteConnectParent     (TreeNode_t* node, TreeNode_t* new_node);
static MathErr_t MathProcessNeutralsAddCase     (TreeNode_t* node, MathCtx_t* math_ctx);
static MathErr_t MathProcessNeutralsSubCase     (TreeNode_t* node, MathCtx_t* math_ctx);
static MathErr_t MathProcessNeutralsAddSubCase  (TreeNode_t* node, MathCtx_t* math_ctx, MathOp_t op);
static MathErr_t MathProcessNeutralsMulCase     (TreeNode_t* node, MathCtx_t* math_ctx);
static MathErr_t MathProcessNeutralsDivCase     (TreeNode_t* node, MathCtx_t* math_ctx);
static MathErr_t MathProcessNeutralsDegCase     (TreeNode_t* node, MathCtx_t* math_ctx);
static MathErr_t MathConvoluteConstsNode        (TreeNode_t* node, MathCtx_t* math_ctx);
static MathErr_t MathConvoluteSingleNode        (TreeNode_t* node, MathCtx_t* math_ctx);

//------------------------------------------------------------------------------------------

MathErr_t MathSimplify(MathCtx_t* math_ctx)
{
    MathTexSection("Упрощение выражения");
    MathTexMessage("Путем несложных преобразований");

    MathCtxTexDump(math_ctx, NULL);

    size_t size_before = 0;

    MathErr_t error = MATH_SUCCESS;

    while (size_before != math_ctx->tree.size)
    {
        size_before = math_ctx->tree.size;

        if ((error = MathDeleteNeutral(math_ctx)))
            return error;
        if ((error = MathConvoluteConsts(math_ctx)))
            return error;
    }

    return MATH_SUCCESS;
}

//------------------------------------------------------------------------------------------

MathErr_t MathDeleteNeutral(MathCtx_t* math_ctx)
{
    assert(math_ctx != NULL);

    DPRINTF("> START of deleting neutrals for tree\n");

    MathDeleteNeutralNode(math_ctx->tree.dummy->right, math_ctx);

    DPRINTF("> END   of deleting neutrals for tree\n");

    TREE_CALL_DUMP(math_ctx, "DUMP AFTER DELETING NEUTRALS");

    return MATH_SUCCESS;
}

//------------------------------------------------------------------------------------------

static MathErr_t MathDeleteNeutralNode(TreeNode_t* node, MathCtx_t* math_ctx)
{
    assert(node     != NULL);
    assert(math_ctx != NULL);

    MathErr_t error = MATH_SUCCESS;

    if (node->left != NULL)
    {
        if ((error = MathDeleteNeutralNode(node->left, math_ctx)))
            return error;
    }
    if (node->right != NULL)
    {
        if ((error = MathDeleteNeutralNode(node->right, math_ctx)))
            return error;
    }

    if (node->left == NULL || node->right == NULL)
        return MATH_SUCCESS;

    if ((error = MathProcessNeutrals(node, math_ctx)))
        return error;

    return MATH_SUCCESS;
}

//------------------------------------------------------------------------------------------

static MathErr_t MathProcessNeutrals(TreeNode_t* node, MathCtx_t* math_ctx)
{
    assert(node     != NULL);
    assert(math_ctx != NULL);

    if (node->data.type != TYPE_OP)
    {
        PRINTERR("Node is not a binary operation");
        return MATH_UNKNOWN_OP;
    }

    size_t size_before = math_ctx->tree.size;

    switch (node->data.value.op)
    {
        case OP_ADD:
            MathProcessNeutralsAddCase(node, math_ctx);
            break;

        case OP_SUB:
            MathProcessNeutralsSubCase(node, math_ctx);
            break;

        case OP_MUL:
            MathProcessNeutralsMulCase(node, math_ctx);
            break;

        case OP_DIV:
            MathProcessNeutralsDivCase(node, math_ctx);
            break;

        case OP_DEG:
            MathProcessNeutralsDegCase(node, math_ctx);
            break;

        case OP_SIN:
        case OP_COS:
        case OP_TG:
        case OP_CTG:
        case OP_LN:
        default:
            PRINTERR("Given node is not a binary operation");
            return MATH_UNKNOWN_OP;
    }

    if (size_before != math_ctx->tree.size)
        MathCtxTexDump(math_ctx, NULL, node);

    return MATH_SUCCESS;
}

//------------------------------------------------------------------------------------------

static MathErr_t MathConvoluteNodeToNumber(TreeNode_t* node, MathCtx_t* math_ctx, double number)
{
    DPRINTF("\tHandling \"convolute node to number %lg\" case\n", number);

    node->data.type      = TYPE_NUM;
    node->data.value.num = number;

    if (TreeSubtreesDtor(node, &math_ctx->tree))
        return MATH_TREE_ERROR;

    TREE_CALL_DUMP(math_ctx, "DUMP AFTER \"convolute node to number\" at %p", node);

    return MATH_SUCCESS;
}

//------------------------------------------------------------------------------------------

static MathErr_t MathConvoluteLeftToCurrent(TreeNode_t* node, MathCtx_t* math_ctx)
{
    DPRINTF("\tHandling \"convolute left to current\" case\n");

    MathErr_t error = MATH_SUCCESS;

    if ((error = MathConvoluteConnectParent(node, node->left)))
        return error;

    TREE_CALL_DUMP(math_ctx, "DUMP AFTER \"convolute connect parent\" at %p", node);

    TreeSingleNodeDtor(node->right, &math_ctx->tree);
    TreeSingleNodeDtor(node, &math_ctx->tree);

    TREE_CALL_DUMP(math_ctx, "DUMP AFTER \"convolute left to current\" at %p", node);

    return MATH_SUCCESS;
}

//------------------------------------------------------------------------------------------

static MathErr_t MathConvoluteRightToCurrent(TreeNode_t* node, MathCtx_t* math_ctx)
{
    DPRINTF("\tHandling \"convolute right to current\" case\n");

    MathErr_t error = MATH_SUCCESS;

    if ((error = MathConvoluteConnectParent(node, node->right)))
        return error;

    TreeSingleNodeDtor(node->left, &math_ctx->tree);
    TreeSingleNodeDtor(node, &math_ctx->tree);

    TREE_CALL_DUMP(math_ctx, "DUMP AFTER \"convolute right to current\" at %p", node);

    return MATH_SUCCESS;
}

//------------------------------------------------------------------------------------------

static MathErr_t MathConvoluteConnectParent(TreeNode_t* node, TreeNode_t* new_node)
{
    assert(new_node != NULL);
    assert(node     != NULL);

    new_node->parent = node->parent;
    DPRINTF("\t\tnode->parent = %p\n", node->parent);

    if (node->parent->left == node)
    {
        node->parent->left = new_node;
        DPRINTF("\t\tnode->parent->left = %p\n", node->parent->left);
    }
    else
    {
        node->parent->right = new_node;
        DPRINTF("\t\tnode->parent->right = %p\n", node->parent->right);
    }

    DPRINTF("\t\tnew_node = %p\n", new_node);

    return MATH_SUCCESS;
}

//------------------------------------------------------------------------------------------

static MathErr_t MathProcessNeutralsAddCase(TreeNode_t* node, MathCtx_t* math_ctx)
{
    return MathProcessNeutralsAddSubCase(node, math_ctx, OP_ADD);
}

//------------------------------------------------------------------------------------------

static MathErr_t MathProcessNeutralsSubCase(TreeNode_t* node, MathCtx_t* math_ctx)
{
    return MathProcessNeutralsAddSubCase(node, math_ctx, OP_SUB);
}

//------------------------------------------------------------------------------------------

static MathErr_t MathProcessNeutralsAddSubCase(TreeNode_t* node, MathCtx_t* math_ctx, MathOp_t op)
{
    assert(node     != NULL);
    assert(math_ctx != NULL);

    if (op == OP_ADD && ISVALUE_(left, 0.0))
    {
        return MathConvoluteRightToCurrent(node, math_ctx);
    }
    else if (ISVALUE_(right, 0.0))
    {
        return MathConvoluteLeftToCurrent(node, math_ctx);
    }

    return MATH_SUCCESS;
}

//------------------------------------------------------------------------------------------

static MathErr_t MathProcessNeutralsMulCase(TreeNode_t* node, MathCtx_t* math_ctx)
{
    assert(node     != NULL);
    assert(math_ctx != NULL);

    if (ISVALUE_(left, 0.0) || ISVALUE_(right, 0.0))
        return MathConvoluteNodeToNumber(node, math_ctx, 0.0);

    if (ISVALUE_(left, 1.0))
        return MathConvoluteRightToCurrent(node, math_ctx);

    if (ISVALUE_(right, 1.0))
        return MathConvoluteLeftToCurrent(node, math_ctx);

    return MATH_SUCCESS;
}

//------------------------------------------------------------------------------------------

static MathErr_t MathProcessNeutralsDivCase(TreeNode_t* node, MathCtx_t* math_ctx)
{
    assert(node     != NULL);
    assert(math_ctx != NULL);

    if (ISVALUE_(right, 0.0))
    {
        PRINTERR("ERROR: division by zero");
        return MATH_INVALID_OP;
    }

    if (ISVALUE_(left, 0.0))
        return MathConvoluteNodeToNumber(node, math_ctx, 0.0);

    if (ISVALUE_(right, 1.0))
        return MathConvoluteLeftToCurrent(node, math_ctx);

    return MATH_SUCCESS;
}

//------------------------------------------------------------------------------------------

static MathErr_t MathProcessNeutralsDegCase(TreeNode_t* node, MathCtx_t* math_ctx)
{
    assert(node     != NULL);
    assert(math_ctx != NULL);

    if (ISVALUE_(left, 0.0))
        return MathConvoluteNodeToNumber(node, math_ctx, 0.0);

    if (ISVALUE_(left, 1.0) || ISVALUE_(right, 0.0))
        return MathConvoluteNodeToNumber(node, math_ctx, 1.0);

    if (ISVALUE_(right, 1.0))
        return MathConvoluteLeftToCurrent(node, math_ctx);

    return MATH_SUCCESS;
}

//------------------------------------------------------------------------------------------

MathErr_t MathConvoluteConsts(MathCtx_t* math_ctx)
{
    assert(math_ctx != NULL);

    DPRINTF("> START of convolution of consts for tree\n");

    MathErr_t error = MATH_SUCCESS;

    if ((error = MathConvoluteConstsNode(math_ctx->tree.dummy->right, math_ctx)))
    {
        return error;
    }

    DPRINTF("> END   of convolution of consts for tree\n");

    TREE_CALL_DUMP(math_ctx, "DUMP AFTER CONSTS CONVOLUTION");

    return MATH_SUCCESS;
}

//------------------------------------------------------------------------------------------

static MathErr_t MathConvoluteConstsNode(TreeNode_t* node, MathCtx_t* math_ctx)
{
    assert(node     != NULL);
    assert(math_ctx != NULL);

    MathErr_t error = MATH_SUCCESS;

    if (node->left != NULL)
    {
        if ((error = MathConvoluteConstsNode(node->left, math_ctx)))
            return error;
    }
    if (node->right != NULL)
    {
        if ((error = MathConvoluteConstsNode(node->right, math_ctx)))
            return error;
    }

    if (node->left == NULL || node->right == NULL)
        return MATH_SUCCESS;

    if (MathNodeIsNum(node->left) && MathNodeIsNum(node->right))
    {
        if ((error = MathConvoluteSingleNode(node, math_ctx)))
            return error;
    }

    return MATH_SUCCESS;
}

//------------------------------------------------------------------------------------------

static MathErr_t MathConvoluteSingleNode(TreeNode_t* node, MathCtx_t* math_ctx)
{
    assert(node     != NULL);
    assert(math_ctx != NULL);

    double result = 0.0;
    double lvalue = node->left->data.value.num;
    double rvalue = node->right->data.value.num;

    if (MathExecuteBinaryOperation(node->data.value.op, lvalue, rvalue, &result))
    {
        PRINTERR("Executing binary op failed");
        return MATH_UNKNOWN_OP;
    }
    if (TreeSubtreesDtor(node, &math_ctx->tree))
    {
        PRINTERR("Deleting subtrees failed");
        return MATH_TREE_ERROR;
    }

    node->data.type      = TYPE_NUM;
    node->data.value.num = result;

    MathCtxTexDump(math_ctx, NULL, node);

    // TODO: MathTexDumpSimplify(node, math_ctx);

    return MATH_SUCCESS;
}

//------------------------------------------------------------------------------------------

static int MathNodeIsNum(TreeNode_t* node)
{
    return node->data.type == TYPE_NUM;
}

/* ====================================================================================== */

#undef ISVALUE_

/* ====================================================================================== */
