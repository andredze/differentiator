#include "math_diff.h"

/* ==================== Domain Specific Language for differentiation ==================== */

// lnode means left_node
// rnode means right_node

#define dL MathDiffNode(math_ctx, node->left,  diff_var_ind)
#define dR MathDiffNode(math_ctx, node->right, diff_var_ind)

#define cL TreeCopySubtree(&math_ctx->tree, node->left )
#define cR TreeCopySubtree(&math_ctx->tree, node->right)

#define NUM_(number)       TreeNodeCtor(&math_ctx->tree, {TYPE_NUM, { .num = number }}, NULL,  node   )
#define ADD_(lnode, rnode) TreeNodeCtor(&math_ctx->tree, {TYPE_OP,  { .op  = OP_ADD }}, lnode, rnode  )
#define SUB_(lnode, rnode) TreeNodeCtor(&math_ctx->tree, {TYPE_OP,  { .op  = OP_SUB }}, lnode, rnode  )
#define MUL_(lnode, rnode) TreeNodeCtor(&math_ctx->tree, {TYPE_OP,  { .op  = OP_MUL }}, lnode, rnode  )
#define DIV_(lnode, rnode) TreeNodeCtor(&math_ctx->tree, {TYPE_OP,  { .op  = OP_DIV }}, lnode, rnode  )
#define SIN_(node)         TreeNodeCtor(&math_ctx->tree, {TYPE_OP,  { .op  = OP_SIN }}, NULL,  node   )
#define COS_(node)         TreeNodeCtor(&math_ctx->tree, {TYPE_OP,  { .op  = OP_COS }}, NULL,  node   )
#define SQR_(node)         TreeNodeCtor(&math_ctx->tree, {TYPE_OP,  { .op  = OP_DEG }}, node,  NUM_(2))

/* ====================================================================================== */

static TreeNode_t* MathDiffNumber   (MathCtx_t* math_ctx);
static TreeNode_t* MathDiffVariable (MathCtx_t* math_ctx, size_t curr_var_ind, size_t diff_var_ind);
static TreeNode_t* MathDiffNode     (MathCtx_t* math_ctx, TreeNode_t* node,    size_t diff_var_ind);
static TreeNode_t* MathDiffOperation(MathCtx_t* math_ctx, TreeNode_t* node,    size_t diff_var_ind);
static TreeNode_t* MathDiffAdd      (MathCtx_t* math_ctx, TreeNode_t* node,    size_t diff_var_ind);
static TreeNode_t* MathDiffSub      (MathCtx_t* math_ctx, TreeNode_t* node,    size_t diff_var_ind);
static TreeNode_t* MathDiffMul      (MathCtx_t* math_ctx, TreeNode_t* node,    size_t diff_var_ind);
static TreeNode_t* MathDiffDiv      (MathCtx_t* math_ctx, TreeNode_t* node,    size_t diff_var_ind);
static TreeNode_t* MathDiffSin      (MathCtx_t* math_ctx, TreeNode_t* node,    size_t diff_var_ind);
static TreeNode_t* MathDiffCos      (MathCtx_t* math_ctx, TreeNode_t* node,    size_t diff_var_ind);
// static TreeNode_t* MathDiffTg       (MathCtx_t* math_ctx, TreeNode_t* node,    size_t diff_var_ind);
// static TreeNode_t* MathDiffCtg      (MathCtx_t* math_ctx, TreeNode_t* node,    size_t diff_var_ind);
// static TreeNode_t* MathDiffLn       (MathCtx_t* math_ctx, TreeNode_t* node,    size_t diff_var_ind);
// static TreeNode_t* MathDiffDeg      (MathCtx_t* math_ctx, TreeNode_t* node,    size_t diff_var_ind);

//——————————————————————————————————————————————————————————————————————————————————————————

TreeNode_t* (* const MATH_DIFF_OPER_TABLE[]) (MathCtx_t*, TreeNode_t*, size_t) =
{
    [OP_ADD] = MathDiffAdd,
    [OP_SUB] = MathDiffSub,
    [OP_MUL] = MathDiffMul,
    [OP_DIV] = MathDiffDiv,
    [OP_SIN] = MathDiffSin,
    [OP_COS] = MathDiffCos,
    // [OP_TG]  = MathDiffTg,
    // [OP_CTG] = MathDiffCtg,
    // [OP_LN]  = MathDiffLn,
    // [OP_DEG] = MathDiffDeg
};

//——————————————————————————————————————————————————————————————————————————————————————————

MathErr_t MathDifferentiate(MathCtx_t* src_math_ctx, MathCtx_t* dest_math_ctx, const char* str_var)
{
    assert(dest_math_ctx != NULL);
    assert(src_math_ctx  != NULL);

    // TODO: DEBUG_MATH_CTX_CHECK(src_math_ctx);

    size_t var_hash = GetHash(str_var);

    qsort(src_math_ctx->vars_table,
          src_math_ctx->size,
          sizeof(src_math_ctx->vars_table[0]),
          VarCaseCompare);

    char* copy_str_var = strdup(str_var);

    if (copy_str_var == NULL)
    {
        PRINTERR("Memory allocation failed");
        return MATH_ALLOC_ERROR;
    }

    VarCase_t var_case = { .str = copy_str_var, .hash = var_hash };

    VarCase_t* var_case_p = (VarCase_t*) bsearch(&var_case, src_math_ctx->vars_table,
                                                 src_math_ctx->size,
                                                 sizeof(src_math_ctx->vars_table[0]),
                                                 VarCaseCompare);

    size_t var_index = (size_t) (var_case_p - src_math_ctx->vars_table);

    dest_math_ctx->vars_table[0] = var_case;
    dest_math_ctx->size = 1;

    TreeNode_t* root = MathDiffNode(dest_math_ctx, src_math_ctx->tree.dummy->right, var_index);

    if (root == NULL)
    {
        PRINTERR("Differentiation failed");
        return MATH_NULL;
    }

    dest_math_ctx->tree.dummy->right = root;

    TreeDumpInfo_t dump_info = {TREE_SUCCESS, __func__, __FILE__, __LINE__};
    TreeDump(dest_math_ctx, &dump_info, "TREE AFTER DIFFERENTIATION");

    return MATH_SUCCESS;
}

//------------------------------------------------------------------------------------------

static TreeNode_t* MathDiffNode(MathCtx_t* math_ctx, TreeNode_t* node, size_t diff_var_ind)
{
    assert(math_ctx != NULL);
    assert(node     != NULL);

    switch (node->data.type)
    {
        case TYPE_NUM:
            return MathDiffNumber(math_ctx);

        case TYPE_VAR:
            return MathDiffVariable(math_ctx, node->data.value.var, diff_var_ind);

        case TYPE_OP:
            return MathDiffOperation(math_ctx, node, diff_var_ind);

        default:
            PRINTERR("Unknown math value type");
            return NULL;
    }

    return NULL;
}

//------------------------------------------------------------------------------------------

static TreeNode_t* MathDiffNumber(MathCtx_t* math_ctx)
{
    assert(math_ctx != NULL);

    return TreeNodeCtor(&math_ctx->tree, {TYPE_NUM, { .num = 0 }}, NULL, NULL);
}

//------------------------------------------------------------------------------------------

static TreeNode_t* MathDiffVariable(MathCtx_t* math_ctx, size_t curr_var_ind, size_t diff_var_ind)
{
    assert(math_ctx != NULL);

    if (diff_var_ind == curr_var_ind)
    {
        return TreeNodeCtor(&math_ctx->tree, {TYPE_NUM, { .num = 1 }}, NULL, NULL);
    }

    return TreeNodeCtor(&math_ctx->tree, {TYPE_NUM, { .num = 0 }}, NULL, NULL);
}

//------------------------------------------------------------------------------------------

static TreeNode_t* MathDiffOperation(MathCtx_t* math_ctx, TreeNode_t* node, size_t diff_var_ind)
{
    assert(math_ctx != NULL);
    assert(node     != NULL);

    return MATH_DIFF_OPER_TABLE[node->data.value.op](math_ctx, node, diff_var_ind);
}

//------------------------------------------------------------------------------------------

static TreeNode_t* MathDiffAdd(MathCtx_t* math_ctx, TreeNode_t* node, size_t diff_var_ind)
{
    assert(math_ctx != NULL);
    assert(node     != NULL);

    TreeNode_t* new_node = ADD_(dL, dR);

    MathCtxTexDump(math_ctx, "Dump after MathDiffAdd() at %p", new_node);

    return new_node;
}

//------------------------------------------------------------------------------------------

static TreeNode_t* MathDiffSub(MathCtx_t* math_ctx, TreeNode_t* node, size_t diff_var_ind)
{
    assert(math_ctx != NULL);
    assert(node     != NULL);

    return SUB_(dL, dR);
}

//------------------------------------------------------------------------------------------

static TreeNode_t* MathDiffMul(MathCtx_t* math_ctx, TreeNode_t* node, size_t diff_var_ind)
{
    assert(math_ctx != NULL);
    assert(node     != NULL);

    return ADD_(MUL_(dL, cR), MUL_(cL, dR));
}

//------------------------------------------------------------------------------------------

static TreeNode_t* MathDiffDiv(MathCtx_t* math_ctx, TreeNode_t* node, size_t diff_var_ind)
{
    assert(math_ctx != NULL);
    assert(node     != NULL);

    return DIV_(SUB_(MUL_(dL, cR), MUL_(cL, dR)), SQR_(cR));
}

//------------------------------------------------------------------------------------------

static TreeNode_t* MathDiffSin(MathCtx_t* math_ctx, TreeNode_t* node, size_t diff_var_ind)
{
    assert(math_ctx != NULL);
    assert(node     != NULL);

    return MUL_(COS_(cR), dR);
}

//------------------------------------------------------------------------------------------

static TreeNode_t* MathDiffCos(MathCtx_t* math_ctx, TreeNode_t* node, size_t diff_var_ind)
{
    assert(math_ctx != NULL);
    assert(node     != NULL);

    return MUL_(MUL_(SIN_(cR), NUM_(-1)), dR);
}

//==========================================================================================

#undef dL
#undef dR

#undef cL
#undef cR

#undef ADD_
#undef SUB_
#undef MUL_
#undef DIV_
#undef SQR_
#undef SIN_
#undef COS_
#undef NUM_

//==========================================================================================
