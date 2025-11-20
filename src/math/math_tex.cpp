#include "math_tex.h"

//——————————————————————————————————————————————————————————————————————————————————————————

static void TexDumpTitle (FILE* fp);
static void TexDumpEnding(FILE* fp);

static void TexWriteNode(TreeNode_t* node, MathCtx_t* math_ctx);
static void TexWriteData(MathData_t  data, MathCtx_t* math_ctx);

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

    FILE* fp = math_ctx->debug.tex_fp;

    vfprintf(fp, fmt, args);

    fprintf(fp, "\n\\[");

    TexWriteNode(math_ctx->tree.dummy->right, math_ctx);

    fprintf(fp, "\\]\n");

    fflush(fp);

    return MATH_SUCCESS;
}

//------------------------------------------------------------------------------------------

MathErr_t MathOpenTexFile(MathCtx_t* math_ctx)
{
    snprintf(math_ctx->debug.tex_file_name, MAX_FILENAME_LEN, "tex_log_%4zu.tex", (size_t) math_ctx % 100000);

    math_ctx->debug.tex_fp = fopen(math_ctx->debug.tex_file_name, "w");

    if (math_ctx->debug.tex_fp == NULL)
    {
        PRINTERR("Opening file %s failed", math_ctx->debug.tex_file_name);
        return MATH_FILE_ERROR;
    }

    TexDumpTitle(math_ctx->debug.tex_fp);

    return MATH_SUCCESS;
}

//------------------------------------------------------------------------------------------

MathErr_t MathCloseTexFile(MathCtx_t* math_ctx)
{
    TexDumpEnding(math_ctx->debug.tex_fp);

    fclose(math_ctx->debug.tex_fp);

    return MATH_SUCCESS;
}

//------------------------------------------------------------------------------------------

static void TexDumpTitle(FILE* fp)
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

static void TexDumpEnding(FILE* fp)
{
    fprintf(fp, "\\end{document}");
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
    FILE* fp = math_ctx->debug.tex_fp;

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
