#include "text_parse.h"

/* ==================== Domain Specific Language for reading data ========================== */

// lnode means left_node
// rnode means right_node

/* if operation has 1 argument, it should be placed in right node */

#define ISVALUE_(node, number) (node->data.type == TYPE_NUM && \
                                CompareDoubles(node->data.value.num, (number)) == 0)

#define NUM_(number)       MathNodeCtor(math_ctx, {TYPE_NUM, { .num = (number) }}, NULL, NULL)

#define ADD_(lnode, rnode) MathNodeCtor(math_ctx, {TYPE_OP,  { .op  = OP_ADD }}, (lnode), (rnode))
#define SUB_(lnode, rnode) MathNodeCtor(math_ctx, {TYPE_OP,  { .op  = OP_SUB }}, (lnode), (rnode))
#define MUL_(lnode, rnode) MathNodeCtor(math_ctx, {TYPE_OP,  { .op  = OP_MUL }}, (lnode), (rnode))
#define DIV_(lnode, rnode) MathNodeCtor(math_ctx, {TYPE_OP,  { .op  = OP_DIV }}, (lnode), (rnode))
#define DEG_(lnode, rnode) MathNodeCtor(math_ctx, {TYPE_OP,  { .op  = OP_DEG }}, (lnode), (rnode))

#define SIN_(node)         MathNodeCtor(math_ctx, {TYPE_OP,  { .op  = OP_SIN }}, NULL, (node))
#define COS_(node)         MathNodeCtor(math_ctx, {TYPE_OP,  { .op  = OP_COS }}, NULL, (node))
#define LN_(node)          MathNodeCtor(math_ctx, {TYPE_OP,  { .op  = OP_LN  }}, NULL, (node))

#define SQRT_(node)        DEG_((node), NUM_(0.5))
#define SQR_(node)         DEG_((node), NUM_(2))
#define EXP_(node)         DEG_(NUM_(EULER_NUMBER), (node))

/* ====================================================================================== */

#ifdef TREE_DEBUG
    #define TREE_READ_BUFFER_DUMP(expr, fmt, ...)                                                      \
            BEGIN                                                                                      \
            TreeReadBufferDump((expr->buffer), (expr->cur_p - expr->buffer), (fmt), ##__VA_ARGS__);    \
            END
    #define MATH_VARS_DUMP(math_ctx, fmt, ...)                      \
            BEGIN                                                   \
            MathVarsTableDump((math_ctx), (fmt), ##__VA_ARGS__);    \
            END
#else
    #define TREE_READ_BUFFER_DUMP(expr, fmt, ...) ;
    #define MATH_VARS_DUMP(math_ctx, fmt, ...)    ;
#endif

//------------------------------------------------------------------------------------------

static void PrintSyntaxError(Expr_t* expr,   const char* file, const char* func,
                             const int line, const char* fmt, ...);

static void SkipSpaces(Expr_t* expr);

static TreeNode_t* GetG (MathCtx_t* math_ctx, Expr_t* expr);
static TreeNode_t* GetE (MathCtx_t* math_ctx, Expr_t* expr);
static TreeNode_t* GetT (MathCtx_t* math_ctx, Expr_t* expr);
static TreeNode_t* GetP (MathCtx_t* math_ctx, Expr_t* expr);
static TreeNode_t* GetPBracketsCase(MathCtx_t* math_ctx, Expr_t* expr);
static TreeNode_t* GetN (MathCtx_t* math_ctx, Expr_t* expr);
static TreeNode_t* GetV (MathCtx_t* math_ctx, Expr_t* expr);

static MathErr_t PutVarInTable(MathCtx_t* math_ctx, char* str, size_t str_len, MathData_t* data);

//——————————————————————————————————————————————————————————————————————————————————————————

#define SYNTAX_ERROR(math_ctx, expr, fmt, ...)                                                      \
        {   TreeDumpInfo_t dump_info = {TREE_SUCCESS, __PRETTY_FUNCTION__, __FILE__, __LINE__};     \
            if (TreeDump((math_ctx), &dump_info, (fmt), ##__VA_ARGS__))                             \
            {                                                                                       \
                return NULL;                                                                        \
            }                                                                                       \
            PrintSyntaxError(expr, __FILE__, __PRETTY_FUNCTION__, __LINE__, (fmt), ##__VA_ARGS__);  \
            return NULL; }

//------------------------------------------------------------------------------------------

MathErr_t MathParseText(MathCtx_t* math_ctx, Expr_t* expr)
{
    assert(expr != NULL);

    DEBUG_TREE_CHECK(math_ctx, "START OF PARSE TEXT");

    TreeNode_t* root = GetG(math_ctx, expr);

    if (root == NULL)
        return MATH_FILE_ERROR;

    math_ctx->tree.dummy->right = root;
    root->parent = math_ctx->tree.dummy;

    TREE_CALL_DUMP(math_ctx, "PARSED TEXT");
    MathTexSection("Исходное выражение");
    MathCtxTexDump(math_ctx, NULL);

    return MATH_SUCCESS;
}

//------------------------------------------------------------------------------------------

static void SkipSpaces(Expr_t* expr)
{
    assert(expr != NULL);

    char ch = '\0';

    while ((ch = *expr->cur_p) != '\0' && isspace(ch))
    {
        expr->cur_p++;
    }
}

//------------------------------------------------------------------------------------------

static void PrintSyntaxError(Expr_t* expr, const char* file, const char* func, const int line, const char* fmt, ...)
{
    char message[MAX_SYNTAX_ERR_MESSAGE_LEN] = {};

    va_list args = {};
    va_start(args, fmt);

    vsnprintf(message, sizeof(message), fmt, args);

    va_end(args);

    fcprintf(stderr, RED, "ERROR from %s at %s:%d\n\tSyntax error: %s (\"%c\" at expr->cur_p[%d])\n\tbuffer: ",
                           func, file, line, message, *expr->cur_p, expr->cur_p - expr->buffer);

    for (int i = 0; i < expr->cur_p - expr->buffer; i++)
        fcprintf(stderr, GRAY, "%c", expr->buffer[i]);

    fcprintf(stderr, RED, "%c", *expr->cur_p);

    fcprintf(stderr, BLUE, "%s\n", expr->cur_p + 1);
}

//------------------------------------------------------------------------------------------

static TreeNode_t* GetG(MathCtx_t* math_ctx, Expr_t* expr)
{
    TreeNode_t* node = GetE(math_ctx, expr);

    if (node == NULL)
        return NULL;

    SkipSpaces(expr);

    if (*expr->cur_p != END_SYMBOL)
        SYNTAX_ERROR(math_ctx, expr, "Unknown symbol at end");

    expr->cur_p++;

    return node;
}

//------------------------------------------------------------------------------------------

static TreeNode_t* GetE(MathCtx_t* math_ctx, Expr_t* expr)
{
    TreeNode_t* node1 = GetT(math_ctx, expr);

    if (node1 == NULL)
        return NULL;

    SkipSpaces(expr);

    while (*expr->cur_p == '+' || *expr->cur_p == '-')
    {
        int operation = *expr->cur_p;
        expr->cur_p++;

        TREE_READ_BUFFER_DUMP(expr, "GET E: READ \"+ | -\"");

        TreeNode_t* node2 = GetT(math_ctx, expr);

        if (node2 == NULL)
            return NULL;

        if (operation == '+')
            node1 = ADD_(node1, node2);
        else
            node1 = SUB_(node1, node2);
    }

    return node1;
}

//------------------------------------------------------------------------------------------

static TreeNode_t* GetT(MathCtx_t* math_ctx, Expr_t* expr)
{
    TreeNode_t* node1 = GetP(math_ctx, expr);

    if (node1 == NULL)
        return NULL;

    SkipSpaces(expr);

    while (*expr->cur_p == '*' || *expr->cur_p == '/')
    {
        int operation = *expr->cur_p;
        expr->cur_p++;
        TREE_READ_BUFFER_DUMP(expr, "GET T: READ \"* | /\"");

        TreeNode_t* node2 = GetP(math_ctx, expr);

        if (node2 == NULL)
            return NULL;

        if (operation == '*')
            node1 = MUL_(node1, node2);
        else
        {
            if (ISVALUE_(node2, 0.0))
                SYNTAX_ERROR(math_ctx, expr, "Division by zero");

            node1 = DIV_(node1, node2);
        }
    }

    return node1;
}

//------------------------------------------------------------------------------------------

static TreeNode_t* GetP(MathCtx_t* math_ctx, Expr_t* expr)
{
    TreeNode_t* node = 0;

    SkipSpaces(expr);

    if (*expr->cur_p == '(')
        node = GetPBracketsCase(math_ctx, expr);
    else if (isdigit(*expr->cur_p))
        node = GetN(math_ctx, expr);
    else
        node = GetV(math_ctx, expr);

    return node;
}

//------------------------------------------------------------------------------------------

static TreeNode_t* GetPBracketsCase(MathCtx_t* math_ctx, Expr_t* expr)
{
    expr->cur_p++;
    TREE_READ_BUFFER_DUMP(expr, "GET P: READ \"(\"");

    TreeNode_t* node = GetE(math_ctx, expr);

    if (node == NULL)
        return NULL;

    SkipSpaces(expr);

    if (*expr->cur_p != ')')
        SYNTAX_ERROR(math_ctx, expr, "no matching bracket");

    expr->cur_p++;
    TREE_READ_BUFFER_DUMP(expr, "GET P: READ \")\"");

    return node;
}

//------------------------------------------------------------------------------------------

static TreeNode_t* GetN(MathCtx_t* math_ctx, Expr_t* expr)
{
    SkipSpaces(expr);

    const char* start = expr->cur_p;
    double      value = 0;

    while ('0' <= *expr->cur_p && *expr->cur_p <= '9')
    {
        value = value * 10 + (*expr->cur_p - '0');
        expr->cur_p++;
    }

    if (start == expr->cur_p)
        SYNTAX_ERROR(math_ctx, expr, "Unknown symbol: not a number");

    TreeNode_t* node = NUM_(value);

    TREE_READ_BUFFER_DUMP(expr, "GET N: READ number \"%lg\"", value);

    return node;
}

//------------------------------------------------------------------------------------------

// TODO - переделать однобуквенные переменные на многобуквенные

static TreeNode_t* GetV(MathCtx_t* math_ctx, Expr_t* expr)
{
    SkipSpaces(expr);

    if (!isalpha(*expr->cur_p))
        SYNTAX_ERROR(math_ctx, expr, "not a letter at start of variable");

    TreeNode_t* node = MathNodeCtor(math_ctx, {.type = TYPE_VAR}, NULL, NULL);

    if (node == NULL)
        return NULL;

    if (PutVarInTable(math_ctx, expr->cur_p, 1, &node->data))
        return NULL;

    expr->cur_p++;

    TREE_READ_BUFFER_DUMP(expr, "GET V: READ variable \"vars[%d] = %s\"",
                                node->data.value.var,
                                math_ctx->vars.data[node->data.value.var].str);

    return node;
}

//------------------------------------------------------------------------------------------

static MathErr_t PutVarInTable(MathCtx_t* math_ctx, char* str, size_t str_len, MathData_t* data)
{
    assert(math_ctx != NULL);
    assert(data     != NULL);
    assert(str      != NULL);

    MathErr_t error = MATH_SUCCESS;

    if (math_ctx->vars.size >= math_ctx->vars.capacity)
    {
        if ((error = MathVarsTableRealloc(math_ctx)))
            return error;
    }

    for (size_t i = 0; i < math_ctx->vars.size; i++)
    {
        if (strncmp(math_ctx->vars.data[i].str, str, str_len) == 0)
            return MATH_SUCCESS;
    }

    char* var_str = strndup(str, str_len);

    if (var_str == NULL)
    {
        PRINTERR("Memory allocation failed");
        return MATH_ALLOC_ERROR;
    }

    math_ctx->vars.data[math_ctx->vars.size].str = var_str;

    data->value.var = math_ctx->vars.size;

    math_ctx->vars.size++;

    MATH_VARS_DUMP(math_ctx, "DUMP AFTER PUTTING VARIABLE %s (index = %zu)",
                   var_str,
                   math_ctx->vars.size - 1);

    return MATH_SUCCESS;
}

//==========================================================================================

#undef ISVALUE_

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
