#include "math_diff.h"

//FIXME - родительские узлы в дифф. дереве указывают на дерево обычной функции

/* ==================== Domain Specific Language for differentiation ==================== */

// lnode means left_node
// rnode means right_node

/* if operation has 1 argument, it should be placed in right node */

#define dL    MathDiffNode(math_ctx, node->left,  diff_var_ind, node)
#define dR    MathDiffNode(math_ctx, node->right, diff_var_ind, node)

#define cL    TreeCopySubtree(&math_ctx->tree, node->left , node)
#define cR    TreeCopySubtree(&math_ctx->tree, node->right, node)
#define cN    TreeCopySubtree(&math_ctx->tree, node       , parent) // NOTE: проверить parent

#define numL  node->left->data.value.num
#define numR  node->right->data.value.num

#define typeL node->left->data.type
#define typeR node->right->data.type

#define NUM_(number)       TreeNodeCtor(&math_ctx->tree, {TYPE_NUM, { .num = (number) }}, NULL, NULL, parent )

#define ADD_(lnode, rnode) TreeNodeCtor(&math_ctx->tree, {TYPE_OP,  { .op  = OP_ADD }}, (lnode), (rnode), parent )
#define SUB_(lnode, rnode) TreeNodeCtor(&math_ctx->tree, {TYPE_OP,  { .op  = OP_SUB }}, (lnode), (rnode), parent )
#define MUL_(lnode, rnode) TreeNodeCtor(&math_ctx->tree, {TYPE_OP,  { .op  = OP_MUL }}, (lnode), (rnode), parent )
#define DIV_(lnode, rnode) TreeNodeCtor(&math_ctx->tree, {TYPE_OP,  { .op  = OP_DIV }}, (lnode), (rnode), parent )
#define DEG_(lnode, rnode) TreeNodeCtor(&math_ctx->tree, {TYPE_OP,  { .op  = OP_DEG }}, (lnode), (rnode), parent )

#define SIN_(node)         TreeNodeCtor(&math_ctx->tree, {TYPE_OP,  { .op  = OP_SIN }}, NULL, (node), parent )
#define COS_(node)         TreeNodeCtor(&math_ctx->tree, {TYPE_OP,  { .op  = OP_COS }}, NULL, (node), parent )
#define LN_(node)          TreeNodeCtor(&math_ctx->tree, {TYPE_OP,  { .op  = OP_LN  }}, NULL, (node), parent )

#define SQRT_(node)        DEG_((node), NUM_(0.5))
#define SQR_(node)         DEG_((node), NUM_(2))
#define EXP_(node)         DEG_(NUM_(EULER_NUMBER), (node))

/* ====================================================================================== */

static TreeNode_t* MathDiffNumber   (MathCtx_t* math_ctx, TreeNode_t* parent);
static TreeNode_t* MathDiffVariable (MathCtx_t* math_ctx, size_t curr_var_ind, size_t diff_var_ind, TreeNode_t* parent);
static TreeNode_t* MathDiffNode     (MathCtx_t* math_ctx, TreeNode_t* node,    size_t diff_var_ind, TreeNode_t* parent);
static TreeNode_t* MathDiffOperation(MathCtx_t* math_ctx, TreeNode_t* node,    size_t diff_var_ind, TreeNode_t* parent);
static TreeNode_t* MathDiffDeg      (MathCtx_t* math_ctx, TreeNode_t* node,    size_t diff_var_ind, TreeNode_t* parent);

//——————————————————————————————————————————————————————————————————————————————————————————

#define DECLARE_MATH_DIFF_OPER(op_name, formula)    \
static TreeNode_t*                                  \
MathDiff##op_name ( MathCtx_t*   math_ctx,          \
                    TreeNode_t*  node,              \
                    size_t       diff_var_ind,      \
                    TreeNode_t*  parent )           \
{                                                   \
    assert(math_ctx != NULL);                       \
    assert(node     != NULL);                       \
                                                    \
    return formula;                                 \
}

//------------------------------------------------------------------------------------------

DECLARE_MATH_DIFF_OPER(Add, ( ADD_(dL, dR) ));

DECLARE_MATH_DIFF_OPER(Sub, ( SUB_(dL, dR) ));

DECLARE_MATH_DIFF_OPER(Mul, ( ADD_(MUL_(dL, cR), MUL_(cL, dR)) ));

DECLARE_MATH_DIFF_OPER(Div, ( DIV_(SUB_(MUL_(dL, cR), MUL_(cL, dR)), SQR_(cR)) ));

DECLARE_MATH_DIFF_OPER(Sin, ( MUL_(COS_(cR), dR) ));

DECLARE_MATH_DIFF_OPER(Cos, ( MUL_(MUL_(SIN_(cR), NUM_(-1)), dR) ));

DECLARE_MATH_DIFF_OPER(Tg,  ( MUL_(DIV_(NUM_(1), SQR_(COS_(cR))), dR) ));

DECLARE_MATH_DIFF_OPER(Ctg, ( MUL_(DIV_(NUM_(-1), SQR_(SIN_(cR))), dR) ));

DECLARE_MATH_DIFF_OPER(Ln,  ( MUL_(DIV_(NUM_(1), cR), dR) ));

//------------------------------------------------------------------------------------------

#undef DECLARE_MATH_DIFF_OPER

//——————————————————————————————————————————————————————————————————————————————————————————

TreeNode_t* (* const MATH_DIFF_OPER_TABLE[]) (MathCtx_t*, TreeNode_t*, size_t, TreeNode_t*) =
{
    [OP_ADD] = MathDiffAdd,
    [OP_SUB] = MathDiffSub,
    [OP_MUL] = MathDiffMul,
    [OP_DIV] = MathDiffDiv,
    [OP_SIN] = MathDiffSin,
    [OP_COS] = MathDiffCos,
    [OP_TG]  = MathDiffTg,
    [OP_CTG] = MathDiffCtg,
    [OP_LN]  = MathDiffLn,
    [OP_DEG] = MathDiffDeg
};

//——————————————————————————————————————————————————————————————————————————————————————————

// TODO: почистить эту ф-ю

MathErr_t MathDifferentiate(MathCtx_t* src_math_ctx, MathCtx_t* dest_math_ctx, const char* str_var)
{
    assert(dest_math_ctx != NULL);
    assert(src_math_ctx  != NULL);

    DEBUG_TREE_CHECK(src_math_ctx, "MATH_DIFF");

    char* copy_str_var = strdup(str_var);

    if (copy_str_var == NULL)
    {
        PRINTERR("Memory allocation failed");
        return MATH_ALLOC_ERROR;
    }

    VarCase_t var_case  = { .str = copy_str_var };
    size_t    var_index = 0;

    for (size_t i = 0; i < src_math_ctx->vars.size; i++)
    {
        if (strcmp(copy_str_var, src_math_ctx->vars.data[i].str) == 0)
            var_index = i;
    }

    dest_math_ctx->vars.data[0] = var_case;
    dest_math_ctx->vars.size = 1;

    TreeNode_t* root = MathDiffNode(dest_math_ctx, src_math_ctx->tree.dummy->right,
                                    var_index, dest_math_ctx->tree.dummy);

    if (root == NULL)
    {
        PRINTERR("Differentiation failed");
        return MATH_NULL;
    }

    dest_math_ctx->tree.dummy->right = root;

    TREE_CALL_DUMP(dest_math_ctx, "TREE AFTER DIFFERENTIATION");

    return MATH_SUCCESS;
}

//------------------------------------------------------------------------------------------

static TreeNode_t* MathDiffNode(MathCtx_t* math_ctx, TreeNode_t* node,
                                size_t diff_var_ind, TreeNode_t* parent)
{
    assert(math_ctx != NULL);
    assert(parent   != NULL);
    assert(node     != NULL);

    TreeNode_t* new_node = NULL;

    switch (node->data.type)
    {
        case TYPE_NUM:
            new_node = MathDiffNumber(math_ctx, parent);
            break;

        case TYPE_VAR:
            new_node = MathDiffVariable(math_ctx, node->data.value.var, diff_var_ind, parent);
            break;

        case TYPE_OP:
            new_node = MathDiffOperation(math_ctx, node, diff_var_ind, parent);
            break;

        default:
            PRINTERR("Unknown math value type");
            return NULL;
    }

    MathTexDumpDiffSubtree(node, new_node, math_ctx);

    return new_node;
}

//------------------------------------------------------------------------------------------

static TreeNode_t* MathDiffNumber(MathCtx_t* math_ctx, TreeNode_t* parent)
{
    assert(math_ctx != NULL);

    return NUM_(0);
}

//------------------------------------------------------------------------------------------

static TreeNode_t* MathDiffVariable(MathCtx_t* math_ctx, size_t curr_var_ind,
                                    size_t diff_var_ind, TreeNode_t* parent)
{
    assert(math_ctx != NULL);

    if (diff_var_ind == curr_var_ind)
    {
        return NUM_(1);
    }

    return NUM_(0);
}

//------------------------------------------------------------------------------------------

static TreeNode_t* MathDiffOperation(MathCtx_t* math_ctx, TreeNode_t* node,
                                     size_t diff_var_ind, TreeNode_t* parent)
{
    assert(math_ctx != NULL);
    assert(node     != NULL);

    return MATH_DIFF_OPER_TABLE[node->data.value.op](math_ctx, node, diff_var_ind, parent);
}

//------------------------------------------------------------------------------------------

static TreeNode_t* MathDiffDeg(MathCtx_t* math_ctx, TreeNode_t* node,
                               size_t diff_var_ind, TreeNode_t* parent)
{
    assert(math_ctx != NULL);
    assert(node     != NULL);

    if (typeL == TYPE_NUM) /* a ^ f(x) */
    {
        return MUL_(MUL_(cN, LN_(cL)), dR);
    }
    else if (typeR == TYPE_NUM) /* f(x) ^ a */
    {
        return MUL_(MUL_(cR, DEG_(cL, NUM_(numR - 1))), dL);
    }

    /* f(x) ^ g(x)
        f == cL
        g == cR
        d/dx(f) == dL
        d/dx(g) == dR

     d/dx ( e^(ln(f)*g) )
     = e^(ln(f)*g) * d/dx (ln(f)*g)
     = e^(ln(f)*g) * ( g * d/dx(f) * 1/f +  ln(f) * d/dx(g) ) */

    return MUL_(EXP_(MUL_(LN_(cL), cR)),
                ADD_(MUL_(MUL_(cR, dL),
                          DIV_(NUM_(1), cL)),
                     MUL_(LN_(cL), dR)));
}

//==========================================================================================

#undef dL
#undef dR

#undef cL
#undef cR
#undef cN

#undef numL
#undef numR

#undef typeL
#undef typeR

#undef NUM_

#undef ADD_
#undef SUB_
#undef MUL_
#undef DIV_

#undef SQR_
#undef SIN_
#undef COS_

//==========================================================================================
