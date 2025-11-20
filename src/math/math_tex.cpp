#include "math_tex.h"

//——————————————————————————————————————————————————————————————————————————————————————————

static void TexDumpTitle    ();
static void TexDumpEnding   ();
static void TexConvertToPdf ();

static void TexWriteNode(TreeNode_t* node, MathCtx_t* math_ctx);
static void TexWriteData(MathData_t  data, MathCtx_t* math_ctx);

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

    fprintf(fp, "\n\\[");

    TexWriteNode(math_ctx->tree.dummy->right, math_ctx);

    fprintf(fp, "\\]\n");

    fflush(fp);

    return MATH_SUCCESS;
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

\title{Название документа}
\author{Автор}
\date{\today}

\begin{document}

\maketitle

\section{Первая секция})");

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
            "pdflatex -interaction=nonstopmode \"$1\" | "
            "grep -E \"(Error|Warning|Overfull|Underfull)\" %s",
            TEX_FILE_NAME);

    system(command);

    DPRINTF("Converted %s to pdf\n", TEX_FILE_NAME);
}

//------------------------------------------------------------------------------------------

static void TexWriteNode(TreeNode_t* node, MathCtx_t* math_ctx)
{
    if (node == NULL)
    {
        PRINTERR("Node is a nullptr");
        return;
    }

    if (node->left != NULL)
        TexWriteNode(node->left, math_ctx);

    TexWriteData(node->data, math_ctx);

    if (node->right != NULL)
        TexWriteNode(node->right, math_ctx);
}

//------------------------------------------------------------------------------------------

static void TexWriteData(MathData_t data, MathCtx_t* math_ctx)
{
    switch (data.type)
    {
        case TYPE_NUM:
            fprintf(fp, "%lg ", data.value.num);
            break;

        case TYPE_OP:
            fprintf(fp, "%s ", OP_CASES_TABLE[data.value.op].str);
            break;

        case TYPE_VAR:
            fprintf(fp, "%s ", math_ctx->vars_table[data.value.var].str);
            break;

        default:
            PRINTERR("Unknown math data type");
            return;
    }
}

//------------------------------------------------------------------------------------------
