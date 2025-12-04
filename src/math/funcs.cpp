#include "funcs.h"

//------------------------------------------------------------------------------------------

static MathErr_t MathCopyNodeAddVarToTable(MathCtx_t* math_ctx, TreeNode_t* node,
                                           MathCtx_t* src_math_ctx);

static MathErr_t MathPutDataInVarTable  (MathCtx_t* math_ctx, const char* var_str);

//------------------------------------------------------------------------------------------

MathErr_t MathCtxCtor(MathCtx_t* math_ctx, size_t vars_capacity)
{
    if (TreeCtor(&math_ctx->tree))
        return MATH_TREE_ERROR;

    if (vars_capacity < VARS_MIN_COUNT)
        vars_capacity = VARS_MIN_COUNT;

    math_ctx->vars.data = (VarCase_t*) calloc(VARS_MIN_COUNT, sizeof(VarCase_t));

    if (math_ctx->vars.data == NULL)
    {
        PRINTERR("Memory allocation failed");
        return MATH_ALLOC_ERROR;
    }

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

TreeNode_t* MathUnaryOpNodeCtor(MathCtx_t* math_ctx, MathOp_t op, TreeNode_t* right)
{
    assert(math_ctx != NULL);

    return MathNodeCtor(math_ctx, {TYPE_OP,  { .op  = op  }}, NULL, right);
}

//------------------------------------------------------------------------------------------

TreeNode_t* MathBinaryOpNodeCtor(MathCtx_t*  math_ctx, MathOp_t    op,
                                 TreeNode_t* left,     TreeNode_t* right)
{
    assert(math_ctx != NULL);

    return MathNodeCtor(math_ctx, {TYPE_OP,  { .op  = op  }}, left, right);
}

//------------------------------------------------------------------------------------------

TreeNode_t* MathCopySubtree(MathCtx_t* math_ctx, TreeNode_t* node, MathCtx_t* src_math_ctx)
{
    assert(math_ctx != NULL);

    if (node == NULL)
        return NULL;

    TreeNode_t* left  = MathCopySubtree(math_ctx, node->left,  src_math_ctx);
    TreeNode_t* right = MathCopySubtree(math_ctx, node->right, src_math_ctx);

    if (right == NULL && left != NULL)
    {
        TreeNodeDtor(left, &math_ctx->tree);
        return NULL;
    }

    TreeNode_t* new_node = MathNodeCtor(math_ctx, node->data, left, right);

    if (new_node == NULL)
    {
        TreeNodeDtor(left,  &math_ctx->tree);
        TreeNodeDtor(right, &math_ctx->tree);
        return NULL;
    }

    if (new_node->left != NULL)
        new_node->left->parent = new_node;
    if (new_node->right != NULL)
        new_node->right->parent = new_node;

    if (new_node->data.type == TYPE_VAR)
        MathCopyNodeAddVarToTable(math_ctx, new_node, src_math_ctx);

    return new_node;
}

//------------------------------------------------------------------------------------------

static MathErr_t MathCopyNodeAddVarToTable(MathCtx_t* math_ctx, TreeNode_t* node,
                                           MathCtx_t* src_math_ctx)
{
    assert(node->data.type == TYPE_VAR);

    char* var_str = src_math_ctx->vars.data[node->data.value.var].str;

    MathErr_t error = MATH_SUCCESS;

    if ((error = MathAddVarToTable(math_ctx, var_str)))
        return error;

    node->data.value.var = math_ctx->vars.size - 1; // -1 because of size++

    return MATH_SUCCESS;
}

//------------------------------------------------------------------------------------------

MathErr_t MathAddVarToTable(MathCtx_t* math_ctx, const char* var_str)
{
    assert(math_ctx != NULL);
    assert(var_str  != NULL);

    if (MathVarInTable(math_ctx, var_str))
        return MATH_SUCCESS;

    MathErr_t error = MATH_SUCCESS;

    if (math_ctx->vars.size + 1 > math_ctx->vars.capacity)
    {
        if ((error = MathVarsTableRealloc(math_ctx)))
            return error;
    }

    if ((error = MathPutDataInVarTable(math_ctx, var_str)))
        return error;

    return MATH_SUCCESS;
}

//------------------------------------------------------------------------------------------

int MathVarInTable(MathCtx_t* math_ctx, const char* var_str)
{
    assert(math_ctx != NULL);
    assert(var_str  != NULL);

    for (size_t i = 0; i < math_ctx->vars.size; i++)
    {
        if (strcmp(math_ctx->vars.data[i].str, var_str) == 0)
            return 1; /* variable is already in table */
    }

    return 0;
}

//------------------------------------------------------------------------------------------

static MathErr_t MathPutDataInVarTable(MathCtx_t* math_ctx, const char* var_str)
{
    assert(math_ctx != NULL);
    assert(var_str  != NULL);

    char* copy_str_var = strdup(var_str);

    if (copy_str_var == NULL)
    {
        PRINTERR("Memory allocation failed");
        return MATH_ALLOC_ERROR;
    }

    VarCase_t var_case = { .str = copy_str_var };

    math_ctx->vars.data[math_ctx->vars.size] = var_case;

    math_ctx->vars.size++;

    return MATH_SUCCESS;
}

//------------------------------------------------------------------------------------------

MathErr_t MathGetVarIndex(MathCtx_t* math_ctx, const char* str_var, size_t* var_ind)
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
