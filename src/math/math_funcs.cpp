#include "math_funcs.h"

//------------------------------------------------------------------------------------------

MathErr_t MathCtxCtor(MathCtx_t* math_ctx, size_t vars_capacity)
{
    if (TreeCtor(&math_ctx->tree))
        return MATH_TREE_ERROR;

    if (vars_capacity < VARS_MIN_COUNT)
        vars_capacity = VARS_MIN_COUNT;

    math_ctx->vars_table = (VarCase_t*) calloc(VARS_MIN_COUNT, sizeof(VarCase_t));

    if (math_ctx->vars_table == NULL)
        return MATH_ALLOC_ERROR;

    math_ctx->size = 0;
    math_ctx->capacity = vars_capacity;

    return MATH_SUCCESS;
}

//------------------------------------------------------------------------------------------

MathErr_t MathVarsTableRealloc(MathCtx_t* math_ctx)
{
    size_t new_cap = math_ctx->capacity * 2 + 1;

    VarCase_t* vars_table = (VarCase_t*) calloc(new_cap, sizeof(VarCase_t));

    if (vars_table == NULL)
    {
        PRINTERR("Memory allocation failed");
        return MATH_ALLOC_ERROR;
    }

    math_ctx->vars_table = vars_table;
    math_ctx->capacity   = new_cap;

    return MATH_SUCCESS;
}

//------------------------------------------------------------------------------------------

size_t GetHash(const char* str)
{
    assert(str != NULL);

    char c = '\0';
    size_t hash = 5381;

    while ((c = *str++) != '\0')
    {
        hash += (hash << 5) + hash + (size_t) c;
    }

    return hash;
}

//------------------------------------------------------------------------------------------

MathErr_t MathCtxDtor(MathCtx_t* math_ctx)
{
    if (math_ctx == NULL)
    {
        PRINTERR("MathCtx is a nullptr");
        return MATH_NULL;
    }

    VarCase_t* table = math_ctx->vars_table;

    for (size_t i = 0; i < math_ctx->capacity; i++)
    {
        free(table[i].str);
        table[i].str  = NULL;
        table[i].hash = 0;
    }

    free(math_ctx->vars_table);

    math_ctx->vars_table = NULL;
    math_ctx->capacity   = 0;
    math_ctx->size       = 0;

    TreeDtor(&math_ctx->tree);

    math_ctx->tree = {};

    return MATH_SUCCESS;
}

//------------------------------------------------------------------------------------------

int VarCaseCompare(const void* par1, const void* par2)
{
    assert(par1 != NULL);
    assert(par2 != NULL);

    size_t hash1 = ((const VarCase_t*) par1)->hash;
    size_t hash2 = ((const VarCase_t*) par2)->hash;

    if (hash1 < hash2)
        return -1;

    if (hash1 > hash2)
        return 1;

    return 0;
}

//------------------------------------------------------------------------------------------
