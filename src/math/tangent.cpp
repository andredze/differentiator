#include "tangent.h"

//------------------------------------------------------------------------------------------

#define VAR_(var_index)    MathVarNodeCtor     (tangent_ctx, (var_index))
#define NUM_(number)       MathNumberNodeCtor  (tangent_ctx, (number))
#define ADD_(lnode, rnode) MathBinaryOpNodeCtor(tangent_ctx, OP_ADD, (lnode), (rnode))
#define SUB_(lnode, rnode) MathBinaryOpNodeCtor(tangent_ctx, OP_SUB, (lnode), (rnode))
#define MUL_(lnode, rnode) MathBinaryOpNodeCtor(tangent_ctx, OP_MUL, (lnode), (rnode))

//------------------------------------------------------------------------------------------

MathErr_t MathGetTangent(MathCtx_t* math_ctx, MathCtx_t* diff_math_ctx,
                         double     point,    MathCtx_t* tangent_ctx)
{
    assert(math_ctx      != NULL);
    assert(diff_math_ctx != NULL);
    assert(tangent_ctx   != NULL);

    MathErr_t error = MATH_SUCCESS;

    if ((error = MathAddVarToTable(tangent_ctx, diff_math_ctx->vars.data[0].str)))
        return error;

    double k_in_point     = 0.0;
    double value_in_point = 0.0;

    if ((error = MathVarSetValue(math_ctx, point, 0)))
        return error;

    if ((error = MathEvaluateWSetValues(math_ctx, &value_in_point)))
        return error;

    if ((error = MathVarSetValue(diff_math_ctx, point, 0)))
        return error;

    if ((error = MathEvaluateWSetValues(diff_math_ctx, &k_in_point)))
        return error;

    tangent_ctx->tree.dummy->right = ADD_(MUL_(NUM_(k_in_point),
                                               SUB_(VAR_(0), NUM_(point))),
                                          NUM_(value_in_point));

    if (tangent_ctx->is_dump_enabled)
    {
        MathTexChapter        ("Касательная к графику");
        MathTexDumpFuncSubtree(tangent_ctx->tree.dummy->right, tangent_ctx);
        TREE_CALL_DUMP        (tangent_ctx, "Tangent in point %lg to %p", point, math_ctx);
    }

    return MATH_SUCCESS;
}

//------------------------------------------------------------------------------------------
