#include "math_funcs.h"

//------------------------------------------------------------------------------------------

MathErr_t MathCtxCtor(MathCtx_t* math_ctx, size_t vars_capacity)
{
    if (TreeCtor(&math_ctx->tree))
        return MATH_TREE_ERROR;

    if (vars_capacity < VARS_MIN_COUNT)
        vars_capacity = VARS_MIN_COUNT;

    math_ctx->vars.data = (VarCase_t*) calloc(VARS_MIN_COUNT, sizeof(VarCase_t));

    if (math_ctx->vars.data == NULL)
        return MATH_ALLOC_ERROR;

    math_ctx->vars.size = 0;
    math_ctx->vars.capacity = vars_capacity;

    return MATH_SUCCESS;
}

//------------------------------------------------------------------------------------------

MathErr_t MathVarsTableRealloc(MathCtx_t* math_ctx)
{
    size_t new_cap = math_ctx->vars.capacity * 2 + 1;

    VarCase_t* vars_table = (VarCase_t*) calloc(new_cap, sizeof(VarCase_t));

    if (vars_table == NULL)
    {
        PRINTERR("Memory allocation failed");
        return MATH_ALLOC_ERROR;
    }

    math_ctx->vars.data     = vars_table;
    math_ctx->vars.capacity = new_cap;

    return MATH_SUCCESS;
}

//------------------------------------------------------------------------------------------

MathErr_t MathCtxDtor(MathCtx_t* math_ctx)
{
    DPRINTF("Destroying %p:\n", math_ctx);

    if (math_ctx == NULL)
    {
        PRINTERR("MathCtx is a nullptr");
        return MATH_NULL;
    }

    VarCase_t* table = math_ctx->vars.data;

    for (size_t i = 0; i < math_ctx->vars.capacity; i++)
    {
        free(table[i].str);
        table[i].str  = NULL;
    }

    free(math_ctx->vars.data);

    math_ctx->vars.data     = NULL;
    math_ctx->vars.capacity = 0;
    math_ctx->vars.size     = 0;

    TreeDtor(&math_ctx->tree);

    math_ctx->tree = {};

    return MATH_SUCCESS;
}

//------------------------------------------------------------------------------------------

TreeNode_t* MathNodeCtor(MathCtx_t* math_ctx, MathData_t data, TreeNode_t* left, TreeNode_t* right)
{
    assert(math_ctx != NULL);

    TreeNode_t* node = TreeNodeCtor(&math_ctx->tree, data, left, right, NULL);

    if (node == NULL)
    {
        TreeNodeDtor(left, &math_ctx->tree);
        TreeNodeDtor(right, &math_ctx->tree);
    }
    else
    {
        if (node->left != NULL)
            node->left->parent  = node;
        if (node->right != NULL)
            node->right->parent = node;
    }

    return node;
}

//------------------------------------------------------------------------------------------

TreeNode_t* MathCopySubtree(MathCtx_t* math_ctx, TreeNode_t* node)
{
    assert(math_ctx != NULL);

    if (node == NULL)
        return NULL;

    TreeNode_t* left  = MathCopySubtree(math_ctx, node->left);
    TreeNode_t* right = MathCopySubtree(math_ctx, node->right);

    if (right == NULL && left != NULL)
    {
        TreeNodeDtor(left, &math_ctx->tree);
        return NULL;
    }

    TreeNode_t* new_node  = MathNodeCtor(math_ctx, node->data, left, right);

    if (new_node == NULL)
    {
        TreeNodeDtor(left, &math_ctx->tree);
        TreeNodeDtor(right, &math_ctx->tree);
        return NULL;
    }
    else
    {
        if (new_node->left != NULL)
            new_node->left->parent = new_node;
        if (new_node->right != NULL)
            new_node->right->parent = new_node;
    }

    return new_node;
}

//------------------------------------------------------------------------------------------
