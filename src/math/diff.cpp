#include "diff.h"

//------------------------------------------------------------------------------------------

static int gl_tex_dump = 0;

/* ==================== Domain Specific Language for differentiation ==================== */

// lnode means left_node
// rnode means right_node

/* if operation has 1 argument, it should be placed in right node */

#define dL    MathDiffNode(math_ctx, node->left,  diff_var_ind, src_math_ctx)
#define dR    MathDiffNode(math_ctx, node->right, diff_var_ind, src_math_ctx)

#define cL    MathCopySubtree(math_ctx, node->left , src_math_ctx)
#define cR    MathCopySubtree(math_ctx, node->right, src_math_ctx)
#define cN    MathCopySubtree(math_ctx, node       , src_math_ctx)

#define numL  node->left->data.value.num
#define numR  node->right->data.value.num

#define typeL node->left->data.type
#define typeR node->right->data.type

#define NUM_(number)       MathNodeCtor(math_ctx, {TYPE_NUM, { .num = (number) }}, NULL, NULL)

#define ADD_(lnode, rnode) MathBinaryOpNodeCtor(math_ctx, OP_ADD, (lnode), (rnode))
#define SUB_(lnode, rnode) MathBinaryOpNodeCtor(math_ctx, OP_SUB, (lnode), (rnode))
#define MUL_(lnode, rnode) MathBinaryOpNodeCtor(math_ctx, OP_MUL, (lnode), (rnode))
#define DIV_(lnode, rnode) MathBinaryOpNodeCtor(math_ctx, OP_DIV, (lnode), (rnode))
#define DEG_(lnode, rnode) MathBinaryOpNodeCtor(math_ctx, OP_DEG, (lnode), (rnode))

#define SIN_(node)         MathUnaryOpNodeCtor (math_ctx, OP_SIN,  (node))
#define COS_(node)         MathUnaryOpNodeCtor (math_ctx, OP_COS,  (node))

#define SH_(node)          MathUnaryOpNodeCtor (math_ctx, OP_SH,   (node))
#define CH_(node)          MathUnaryOpNodeCtor (math_ctx, OP_CH,   (node))

#define LN_(node)          MathUnaryOpNodeCtor (math_ctx, OP_LN,   (node))
#define SQRT_(node)        MathUnaryOpNodeCtor (math_ctx, OP_SQRT, (node))

#define EXP_(node)         DEG_(NUM_(EULER_NUMBER), (node))
#define SQR_(node)         DEG_((node), NUM_(2))

/* ====================================================================================== */

static TreeNode_t* MathDiffNumber    (MathCtx_t* math_ctx);

static TreeNode_t* MathDiffVariable  (MathCtx_t* math_ctx, size_t curr_var_ind, size_t diff_var_ind);

static TreeNode_t* MathDiffNode      (MathCtx_t* math_ctx, TreeNode_t* node,
                                      size_t diff_var_ind, MathCtx_t* src_math_ctx);

static TreeNode_t* MathDiffOperation (MathCtx_t* math_ctx, TreeNode_t* node,
                                      size_t diff_var_ind, MathCtx_t* src_math_ctx);

static TreeNode_t* MathDiffDeg       (MathCtx_t* math_ctx, TreeNode_t* node,
                                      size_t diff_var_ind, MathCtx_t* src_math_ctx);

static TreeNode_t* MathDiffLog       (MathCtx_t* math_ctx, TreeNode_t* node,
                                      size_t diff_var_ind, MathCtx_t* src_math_ctx);

//——————————————————————————————————————————————————————————————————————————————————————————

#define DECLARE_MATH_DIFF_OPER(op_name, formula)    \
static TreeNode_t*                                  \
MathDiff##op_name ( MathCtx_t*   math_ctx,          \
                    TreeNode_t*  node,              \
                    size_t       diff_var_ind,      \
                    MathCtx_t*   src_math_ctx )     \
{                                                   \
    assert(math_ctx != NULL);                       \
    assert(node     != NULL);                       \
                                                    \
    return formula;                                 \
}

//------------------------------------------------------------------------------------------

DECLARE_MATH_DIFF_OPER(Add,  ( ADD_(dL, dR) ));
DECLARE_MATH_DIFF_OPER(Sub,  ( SUB_(dL, dR) ));
DECLARE_MATH_DIFF_OPER(Mul,  ( ADD_(MUL_(dL, cR), MUL_(cL, dR)) ));
DECLARE_MATH_DIFF_OPER(Div,  ( DIV_(SUB_(MUL_(dL, cR), MUL_(cL, dR)), SQR_(cR)) ));

DECLARE_MATH_DIFF_OPER(Ln,   ( MUL_(DIV_(NUM_(1), cR), dR) ));
DECLARE_MATH_DIFF_OPER(Exp,  ( MUL_(cN, dR) ));
DECLARE_MATH_DIFF_OPER(Sqrt, ( DIV_(dR, MUL_(NUM_(2), SQRT_(cR))) ));

DECLARE_MATH_DIFF_OPER(Sin,  ( MUL_(COS_(cR), dR) ));
DECLARE_MATH_DIFF_OPER(Cos,  ( MUL_(MUL_(SIN_(cR), NUM_(-1)), dR) ));
DECLARE_MATH_DIFF_OPER(Tg,   ( MUL_(DIV_(NUM_(1), SQR_(COS_(cR))), dR) ));
DECLARE_MATH_DIFF_OPER(Ctg,  ( MUL_(DIV_(NUM_(-1), SQR_(SIN_(cR))), dR) ));

DECLARE_MATH_DIFF_OPER(Sh,   ( MUL_(CH_(cR), dR)) );
DECLARE_MATH_DIFF_OPER(Ch,   ( MUL_(SH_(cR), dR) ));
DECLARE_MATH_DIFF_OPER(Th,   ( MUL_(DIV_(NUM_(1), SQR_(CH_(cR))), dR) ));
DECLARE_MATH_DIFF_OPER(Cth,  ( MUL_(DIV_(NUM_(-1), SQR_(SH_(cR))), dR) ));

DECLARE_MATH_DIFF_OPER(Asin, ( DIV_(dR, SQRT_(SUB_(NUM_(1), SQR_(cR))))) );
DECLARE_MATH_DIFF_OPER(Acos, ( MUL_(NUM_(-1), DIV_(dR, SQRT_(SUB_(NUM_(1), SQR_(cR)))))) );
DECLARE_MATH_DIFF_OPER(Atg,  ( DIV_(dR, ADD_(NUM_(1), SQR_(cR)))) );
DECLARE_MATH_DIFF_OPER(Actg, ( MUL_(NUM_(-1), DIV_(dR, ADD_(NUM_(1), SQR_(cR))))) );

//------------------------------------------------------------------------------------------

#undef DECLARE_MATH_DIFF_OPER

//——————————————————————————————————————————————————————————————————————————————————————————

TreeNode_t* (* const MATH_DIFF_OPER_TABLE[]) (MathCtx_t*, TreeNode_t*, size_t, MathCtx_t*) =
{
    [OP_ADD]  = MathDiffAdd,
    [OP_SUB]  = MathDiffSub,
    [OP_MUL]  = MathDiffMul,
    [OP_DIV]  = MathDiffDiv,

    [OP_LOG]  = MathDiffLog,
    [OP_LN]   = MathDiffLn,

    [OP_DEG]  = MathDiffDeg,
    [OP_EXP]  = MathDiffExp,
    [OP_SQRT] = MathDiffSqrt,

    [OP_SIN]  = MathDiffSin,
    [OP_COS]  = MathDiffCos,
    [OP_TG]   = MathDiffTg,
    [OP_CTG]  = MathDiffCtg,

    [OP_SH]   = MathDiffSh,
    [OP_CH]   = MathDiffCh,
    [OP_TH]   = MathDiffTh,
    [OP_CTH]  = MathDiffCth,

    [OP_ASIN] = MathDiffAsin,
    [OP_ACOS] = MathDiffAcos,
    [OP_ATG]  = MathDiffAtg,
    [OP_ACTG] = MathDiffActg,
};

//——————————————————————————————————————————————————————————————————————————————————————————

MathErr_t MathDifferentiate(MathCtx_t* src_math_ctx, MathCtx_t* dest_math_ctx, const char* str_var, int tex_dump)
{
    assert(dest_math_ctx != NULL);
    assert(src_math_ctx  != NULL);
    assert(str_var       != NULL);

    DEBUG_TREE_CHECK(src_math_ctx, "MATH_DIFF");

    if (tex_dump)
    {
        MathTexSection("Дифференцирование");
        MathTexMessage("Очевидно, что");
    }

    if (!MathVarInTable(src_math_ctx, str_var))
    {
        cprintf(RED, "given variable for diff is not in original function\n");
        return MATH_SUCCESS;
    }

    MathErr_t error = MATH_SUCCESS;

    if ((error = MathAddVarToTable(dest_math_ctx, str_var)))
        return error;

    size_t var_index = 0;

    gl_tex_dump = tex_dump;

    TreeNode_t* root = MathDiffNode(dest_math_ctx, src_math_ctx->tree.dummy->right,
                                    var_index,     src_math_ctx);

    if (root == NULL)
    {
        PRINTERR("Differentiation failed");
        return MATH_NULL;
    }

    root->parent = dest_math_ctx->tree.dummy;
    dest_math_ctx->tree.dummy->right = root;

    TREE_CALL_DUMP(dest_math_ctx, "TREE AFTER DIFFERENTIATION");

    return MATH_SUCCESS;
}

//------------------------------------------------------------------------------------------

static TreeNode_t* MathDiffNode(MathCtx_t* math_ctx, TreeNode_t* node,
                                size_t diff_var_ind, MathCtx_t* src_math_ctx)
{
    assert(math_ctx != NULL);
    assert(node     != NULL);

    TreeNode_t* new_node = NULL;

    switch (node->data.type)
    {
        case TYPE_NUM:
            new_node = MathDiffNumber(math_ctx);
            break;

        case TYPE_VAR:
            new_node = MathDiffVariable(math_ctx, node->data.value.var, diff_var_ind);
            break;

        case TYPE_OP:
            new_node = MathDiffOperation(math_ctx, node, diff_var_ind, src_math_ctx);
            break;

        default:
            PRINTERR("Unknown math value type");
            return NULL;
    }

    if (gl_tex_dump)
        MathTexDumpDiffSubtree(node, new_node, math_ctx);

    return new_node;
}

//------------------------------------------------------------------------------------------

static TreeNode_t* MathDiffNumber(MathCtx_t* math_ctx)
{
    assert(math_ctx != NULL);

    return NUM_(0);
}

//------------------------------------------------------------------------------------------

static TreeNode_t* MathDiffVariable(MathCtx_t* math_ctx, size_t curr_var_ind, size_t diff_var_ind)
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
                                     size_t diff_var_ind, MathCtx_t* src_math_ctx)
{
    assert(math_ctx != NULL);
    assert(node     != NULL);

    return MATH_DIFF_OPER_TABLE[node->data.value.op](math_ctx, node, diff_var_ind, src_math_ctx);
}

//------------------------------------------------------------------------------------------

static TreeNode_t* MathDiffDeg(MathCtx_t* math_ctx, TreeNode_t* node,
                               size_t diff_var_ind, MathCtx_t* src_math_ctx)
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

    /* (f(x) ^ g(x))' = e^(ln(f)*g) * ( g * d/dx(f) * 1/f +  ln(f) * d/dx(g) ) */

    return MUL_(EXP_(MUL_(LN_(cL), cR)),
                ADD_(MUL_(MUL_(cR, dL),
                          DIV_(NUM_(1), cL)),
                     MUL_(LN_(cL), dR)));
}

//------------------------------------------------------------------------------------------

static TreeNode_t* MathDiffLog(MathCtx_t* math_ctx,     TreeNode_t* node,
                               size_t     diff_var_ind, MathCtx_t*  src_math_ctx)
{
    assert(math_ctx != NULL);
    assert(node     != NULL);

    if (typeL == TYPE_NUM) /* log(a, f(x)) */
    {
        return DIV_(dR, MUL_(cR, LN_(cL)));
    }
    else if (typeR == TYPE_NUM) /* log (f(x), a) */
    {
        return MUL_(NUM_(-1),
                    DIV_(MUL_(LN_(cR), dL),
                         MUL_(SQR_(LN_(cL)), cL)));
    }

    /* ( log (f(x), g(x)) )' = ( g' / g * ln(f) - f' / f * ln(g) ) / (ln(f) ^ 2) */

    return DIV_(SUB_(MUL_(DIV_(dR, cR), LN_(cL)),
                     MUL_(DIV_(dL, cL), LN_(cR))),
                SQR_(LN_(cL)));
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
#undef DEG_

#undef SIN_
#undef COS_

#undef SH_
#undef CH_

#undef LN_
#undef SQRT_

#undef EXP_
#undef SQR_

//==========================================================================================
