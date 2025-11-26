#include "math_tex.h"

//——————————————————————————————————————————————————————————————————————————————————————————

static void TexDumpTitle    ();
static void TexDumpEnding   ();
static void TexConvertToPdf ();

static void MathTexDumpNode             (TreeNode_t* node, MathCtx_t* math_ctx);
static void MathTexDumpNodeInorder      (TreeNode_t* node, MathCtx_t* math_ctx);
static void MathTexDumpNodeDivCase      (TreeNode_t* node, MathCtx_t* math_ctx);
static void MathTexDumpUnaryOp          (TreeNode_t* node, MathCtx_t* math_ctx);
static void MathTexDumpData             (MathData_t  data, MathCtx_t* math_ctx);
static void MathTexDumpBracketIfNeeded  (TreeNode_t* node, char bracket);
static void MathTexDumpBraceIfNeeded    (TreeNode_t* node, char bracket);
static void MathTexDumpNumber           (double num);
static int  MathNodeMatchesOp           (TreeNode_t* node, MathOp_t op);

//——————————————————————————————————————————————————————————————————————————————————————————

static FILE* fp = NULL;

//——————————————————————————————————————————————————————————————————————————————————————————

MathErr_t MathCtxTexDump(MathCtx_t* math_ctx, const char* fmt, ...)
{
    assert(math_ctx != NULL);
    assert(fmt      != NULL);

    va_list args = {};
    va_start(args, fmt);

    MathErr_t ret = vMathCtxTexDump(math_ctx, fmt, args);

    va_end(args);

    return ret;
}

//------------------------------------------------------------------------------------------

MathErr_t vMathCtxTexDump(MathCtx_t* math_ctx, const char* fmt, va_list args)
{
    assert(math_ctx != NULL);
    assert(fmt      != NULL);

    DPRINTF("   > Start of TEX dump\n");

    if (math_ctx->tree.dummy->right == NULL)
    {
        PRINTERR("MathCtx has empty tree");
        return MATH_SUCCESS;
    }

    vfprintf(fp, fmt, args);

    MathTexDumpSubtree(math_ctx->tree.dummy->right, math_ctx);

    fflush(fp);

    return MATH_SUCCESS;
}

//------------------------------------------------------------------------------------------

void MathTexDumpSubtree(TreeNode_t* node, MathCtx_t* math_ctx)
{
    fprintf(fp, "\n\\[");

    MathTexDumpNode(node, math_ctx);

    fprintf(fp, "\\]\n");
}

//------------------------------------------------------------------------------------------

void MathTexDumpDiffSubtree(TreeNode_t* node, TreeNode_t* diff_node, MathCtx_t* math_ctx)
{
    fprintf(fp, "\n\\[");
    fprintf(fp, R"(\frac{d}{d%s})""(", math_ctx->vars.data[0].str);

    // TODO: как не терять переменные??? нужно хранить еще и src_mathctx????

    MathTexDumpNode(node, math_ctx);

    fprintf(fp, ") = ");

    MathTexDumpNode(diff_node, math_ctx);

    fprintf(fp, "\\]\n");
}

//------------------------------------------------------------------------------------------

MathErr_t MathOpenTexFile()
{
    fp = fopen(TEX_FILE_NAME, "w");

    if (fp == NULL)
    {
        PRINTERR("Opening file %s failed", TEX_FILE_NAME);
        return MATH_FILE_ERROR;
    }

    TexDumpTitle();

    return MATH_SUCCESS;
}

//------------------------------------------------------------------------------------------

MathErr_t MathCloseTexFile()
{
    TexDumpEnding();

    fclose(fp);

    TexConvertToPdf();

    return MATH_SUCCESS;
}

//------------------------------------------------------------------------------------------

static void TexDumpTitle()
{
    fprintf(fp,
R"(\documentclass[12pt, a4paper]{article}
\usepackage[utf8]{inputenc}
\usepackage[T2A]{fontenc}
\usepackage[russian]{babel}

\title{ХУИТА (МАТАН)}
\author{Киселев Андрей}
\date{\today}

\begin{document}

\maketitle

\section{Исходное выражение})");

}

//------------------------------------------------------------------------------------------

static void TexDumpEnding()
{
    fprintf(fp, "\\end{document}");
}

//------------------------------------------------------------------------------------------

static void TexConvertToPdf()
{
    char command[MAX_COMMAND_LEN] = {};

    snprintf(command, sizeof(command),
            "pdflatex -interaction=batchmode math_log.tex %s",
            TEX_FILE_NAME);

    system(command);

    DPRINTF("Converted %s to pdf\n", TEX_FILE_NAME);
}

//------------------------------------------------------------------------------------------

static void MathTexDumpNode(TreeNode_t* node, MathCtx_t* math_ctx)
{
    assert(math_ctx != NULL);

    if (node == NULL)
    {
        PRINTERR("Node is a nullptr");
        return;
    }

    if (node->data.type == TYPE_OP && OP_CASES_TABLE[node->data.value.op].args_count == 1)
    {
        MathTexDumpUnaryOp(node, math_ctx);
        return;
    }

    if (MathNodeMatchesOp(node, OP_DIV))
        MathTexDumpNodeDivCase(node, math_ctx);
    else
        MathTexDumpNodeInorder(node, math_ctx);
}

//------------------------------------------------------------------------------------------

static int MathNodeMatchesOp(TreeNode_t* node, MathOp_t op)
{
    return node->data.type == TYPE_OP && node->data.value.op == op;
}

//------------------------------------------------------------------------------------------

static void MathTexDumpNodeDivCase(TreeNode_t* node, MathCtx_t* math_ctx)
{
    fprintf(fp, "\\frac{");

    if (node->left != NULL)
        MathTexDumpNode(node->left, math_ctx);

    fprintf(fp, "}{");

    if (node->right != NULL)
        MathTexDumpNode(node->right, math_ctx);

    fprintf(fp, "}");
}

//------------------------------------------------------------------------------------------

static void MathTexDumpNodeInorder(TreeNode_t* node, MathCtx_t* math_ctx)
{
    MathTexDumpBracketIfNeeded(node, '(');

    if (node->left != NULL)
        MathTexDumpNode(node->left, math_ctx);

    MathTexDumpData(node->data, math_ctx);

    MathTexDumpBraceIfNeeded(node, '{');

    if (node->right != NULL)
        MathTexDumpNode(node->right, math_ctx);

    MathTexDumpBraceIfNeeded(node, '}');

    MathTexDumpBracketIfNeeded(node, ')');
}

//------------------------------------------------------------------------------------------

static void MathTexDumpBracketIfNeeded(TreeNode_t* node, char bracket)
{
    // TODO: if (node->parent->data.value.op == OP_MUL || OP_DIV)
    if (node->data.type != TYPE_OP)
        return;

    if (node->data.value.op == OP_ADD ||
        node->data.value.op == OP_SUB)
        fprintf(fp, "%c", bracket);
}

//------------------------------------------------------------------------------------------

static void MathTexDumpBraceIfNeeded(TreeNode_t* node, char bracket)
{
    if (node->data.type != TYPE_OP)
        return;

    if (node->data.value.op == OP_DEG)
        fprintf(fp, "%c", bracket);
}

//------------------------------------------------------------------------------------------

static void MathTexDumpData(MathData_t data, MathCtx_t* math_ctx)
{
    assert(math_ctx != NULL);

    // NOTE: предварительное решение
    // TODO: проверять приоритеты с помощью узлов родителя

    switch (data.type)
    {
        case TYPE_NUM:
            MathTexDumpNumber(data.value.num);
            break;

        case TYPE_OP:
            fprintf(fp, "%s ", OP_CASES_TABLE[data.value.op].str);
            break;

        case TYPE_VAR:
            fprintf(fp, "%s ", math_ctx->vars.data[data.value.var].str);
            break;

        default:
            PRINTERR("Unknown math data type");
            return;
    }
}

//------------------------------------------------------------------------------------------

static void MathTexDumpUnaryOp(TreeNode_t* node, MathCtx_t* math_ctx)
{
    assert(math_ctx != NULL);
    assert(node     != NULL);

    fprintf(fp, "%s(", OP_CASES_TABLE[node->data.value.op].str);

    MathTexDumpNode(node->right, math_ctx);

    fprintf(fp, ")");
}

//------------------------------------------------------------------------------------------

static void MathTexDumpNumber(double num)
{
    if (CompareDoubles(num, EULER_NUMBER) == 0)
    {
        fprintf(fp, "e ");
    }
    else if (CompareDoubles(num, PI_NUMBER) == 0)
    {
        fprintf(fp, "\\pi  ");
    }
    else
    {
        fprintf(fp, "%lg ", num);
    }
}

//------------------------------------------------------------------------------------------
