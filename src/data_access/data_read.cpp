#include "data_read.h"

//------------------------------------------------------------------------------------------

#ifdef TREE_DEBUG
    #define TREE_READ_BUFFER_DUMP(buffer, pos, fmt, ...)                  \
            BEGIN                                                         \
            TreeReadBufferDump((buffer), (pos), (fmt), ##__VA_ARGS__);    \
            END
    #define MATH_VARS_DUMP(math_ctx, fmt, ...)                      \
            BEGIN                                                   \
            MathVarsTableDump((math_ctx), (fmt), ##__VA_ARGS__);    \
            END
#else
    #define TREE_READ_BUFFER_DUMP(buffer, pos, fmt, ...) ;
    #define MATH_VARS_DUMP(math_ctx, fmt, ...)           ;
#endif

//------------------------------------------------------------------------------------------

MathErr_t TreeReadInputData(MathCtx_t* math_ctx)
{
    char file_path[MAX_FILENAME_LEN] = {};

    printf("Enter database file path: ");

    if (scanf("%s", file_path) != 1)
    {
        PRINTERR("scanf failed");
        return MATH_INVALID_INPUT;
    }

    getchar();

    return TreeReadData(math_ctx, file_path);
}

//------------------------------------------------------------------------------------------

MathErr_t TreeReadData(MathCtx_t* math_ctx, const char* data_file_path)
{
    DEBUG_TREE_CHECK(math_ctx, "ERROR BEFORE TREE READ DATA");

    assert(data_file_path != NULL);

    DPRINTF("Reading file %s\n", data_file_path);

    FILE* fp = fopen(data_file_path, "r");

    if (fp == NULL)
    {
        PRINTERR("Error with opening file: %s", data_file_path);
        return MATH_FILE_ERROR;
    }

    char* buffer = NULL;

    if (ReadFile(fp, &buffer, data_file_path))
    {
        return MATH_FILE_ERROR;
    }
    math_ctx->tree.buffer = buffer;

    ssize_t i = 0;
    MathErr_t error = MATH_SUCCESS;

    if ((error = ReadNode(math_ctx, buffer, &i, &math_ctx->tree.dummy->right)))
        return error;

    DEBUG_TREE_CHECK(math_ctx, "ERROR AFTER TREE READ DATA");
    TREE_CALL_DUMP  (math_ctx, "DUMP AFTER TREE READ DATA %s", data_file_path);
    MathCtxTexDump  (math_ctx, "Вот такое выражение прочел я");

    return MATH_SUCCESS;
}

//------------------------------------------------------------------------------------------

MathErr_t ReadNode(MathCtx_t* math_ctx, char* buffer, ssize_t* pos, TreeNode_t** pnode)
{
    assert(math_ctx != NULL);
    assert(buffer   != NULL);
    assert(pnode    != NULL);
    assert(pos      != NULL);

    char first_char = buffer[*pos];
    MathErr_t error = MATH_SUCCESS;

    if (first_char == '(')
    {
        (*pos)++;
        SkipSpaces(buffer, pos);
        TREE_READ_BUFFER_DUMP(buffer, *pos, "BUFFER DUMP SKIPPING OPENING BRACKET");

        TreeElem_t data = TREE_POISON;

        if ((error = ReadNodeData(math_ctx, buffer, pos, &data)))
            return error;

        TREE_READ_BUFFER_DUMP(buffer, *pos, "BUFFER DUMP AFTER READING %s", TYPE_CASES_TABLE[data.type].name);

        if ((*pnode = TreeNodeCtor(&math_ctx->tree, data, NULL, NULL)) == NULL)
            return MATH_NULL;

        TREE_CALL_DUMP(math_ctx, "DUMP AFTER NODE CTOR %s", TYPE_CASES_TABLE[data.type].name);

        if ((error = ReadNode(math_ctx, buffer, pos, &(*pnode)->left)))
            return error;
        TREE_READ_BUFFER_DUMP(buffer, *pos, "BUFFER DUMP AFTER READING NODE LEFT TO %s", TYPE_CASES_TABLE[data.type].name);

        if ((error = ReadNode(math_ctx, buffer, pos, &(*pnode)->right)))
            return error;
        TREE_READ_BUFFER_DUMP(buffer, *pos, "BUFFER DUMP AFTER READING NODE RIGHT TO %s", TYPE_CASES_TABLE[data.type].name);

        if (SkipLetter(buffer, pos, ')'))
            return MATH_INVALID_INPUT;

        SkipSpaces(buffer, pos);

        TREE_READ_BUFFER_DUMP(buffer, *pos, "BUFFER DUMP AFTER SKIPPING CLOSING BRACKET after %s", TYPE_CASES_TABLE[data.type].name);
    }
    else if (strncmp(&buffer[*pos], "nil", 3) == 0)
    {
        (*pos) += 3;
        SkipSpaces(buffer, pos);
        TREE_READ_BUFFER_DUMP(buffer, *pos, "BUFFER DUMP AFTER READING NULL");
    }
    else
    {
        PRINTERR("Syntax error in tree data (unknown symbol = \"%c\" )\n", first_char);
        return MATH_INVALID_INPUT;
    }

    return MATH_SUCCESS;
}

//------------------------------------------------------------------------------------------

MathErr_t ReadNodeData(MathCtx_t* math_ctx, char* buffer, ssize_t* pos, MathData_t* data)
{
    assert(buffer != NULL);
    assert(data   != NULL);
    assert(pos    != NULL);

    int data_len = 0;

    if (sscanf(&buffer[*pos], "\"%*[^\"]\"%n", &data_len) != 0)
    {
        PRINTERR("Error with reading data");
        return MATH_FILE_ERROR;
    }
    // DPRINTF("buffer + pos: %s, data_len = %zu\n", &buffer[*pos], data_len);

    if (GetMathData(math_ctx, buffer + *pos + 1, (size_t) (data_len - 2), data))
        return MATH_INVALID_INPUT;

    (*pos) += data_len; /* moving pos to the next data */

    SkipSpaces(buffer, pos);

    return MATH_SUCCESS;
}

//------------------------------------------------------------------------------------------

MathErr_t GetMathData(MathCtx_t* math_ctx, char* str, size_t str_len, MathData_t* data)
{
    assert(data != NULL);
    assert(str  != NULL);

    if (ProcessMathDataOpCase(str, str_len, data) == 1)
        return MATH_SUCCESS;

    if (ProcessMathDataNumCase(str, data) == 1)
        return MATH_SUCCESS;

    if (ProcessMathDataVarCase(math_ctx, str, str_len, data) == 1)
        return MATH_SUCCESS;

    PRINTERR("Syntax error: input doesn't match format, input: %s, len = %d", str, str_len);

    return MATH_INVALID_INPUT;
}

//------------------------------------------------------------------------------------------

int ProcessMathDataOpCase(char* str, size_t str_len, MathData_t* data)
{
    assert(data != NULL);
    assert(str  != NULL);

    for (size_t i = 0; i < OP_CASES_TABLE_SIZE; i++)
    {
        if (strncmp(str, OP_CASES_TABLE[i].str, str_len) == 0)
        {
            data->type     = TYPE_OP;
            data->value.op = OP_CASES_TABLE[i].code;
            return 1;
        }
    }

    return 0;
}

//------------------------------------------------------------------------------------------

int ProcessMathDataNumCase(char* str, MathData_t* data)
{
    assert(data != NULL);
    assert(str  != NULL);

    double number = atof(str);

    if (CompareDoubles(number, 0.0) == 0 && *str != '0')
        return 0;

    data->type      = TYPE_NUM;
    data->value.num = number;

    return 1;
}

//------------------------------------------------------------------------------------------

int ProcessMathDataVarCase(MathCtx_t* math_ctx, char* str, size_t str_len, MathData_t* data)
{
    assert(data != NULL);
    assert(str  != NULL);

// TODO: сделать список допустимых символов
    if (!isalpha(*str))
        return 0;

    data->type = TYPE_VAR;

    if (PutVarInTable(math_ctx, str, str_len, data))
        return -1;

    return 1;
}

//------------------------------------------------------------------------------------------

MathErr_t PutVarInTable(MathCtx_t* math_ctx, char* str, size_t str_len, MathData_t* data)
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

//------------------------------------------------------------------------------------------

void SkipSpaces(char* buffer, ssize_t* pos)
{
    assert(buffer != NULL);
    assert(pos    != NULL);

    char ch = '\0';

    while ((ch = buffer[*pos]) != '\0' && isspace(ch))
    {
        (*pos)++;
    }
}

//------------------------------------------------------------------------------------------

int SkipLetter(char* buffer, ssize_t* pos, char letter)
{
    assert(buffer != NULL);
    assert(pos    != NULL);

    char current_char = buffer[*pos];

    if (current_char != letter)
    {
        PRINTERR("Syntax error: expected %c, got %c (%d)", letter, current_char, current_char);
        return 1;
    }
    (*pos)++;

    return 0;
}

//------------------------------------------------------------------------------------------

int ReadFile(FILE* fp, char** buffer_ptr, const char* file_path)
{
    assert(buffer_ptr != NULL);
    assert(file_path  != NULL);
    assert(fp         != NULL);

    size_t size = 0;

    if (CountSize(file_path, &size))
    {
        return 1;
    }

    char* buffer = (char*) calloc(size, 1);

    if (buffer == NULL)
    {
        PRINTERR("Memory allocation failed");
        return 1;
    }

    if (fread(buffer, size - 1, 1, fp) != 1)
    {
        PRINTERR("Reading file error");
        return 1;
    }

    buffer[size - 1] = '\0'; /* set null-term */

    *buffer_ptr = buffer;

    return 0;
}

//------------------------------------------------------------------------------------------

int CountSize(const char* file_path, size_t* size)
{
    struct stat fileinfo = {};

    if (stat(file_path, &fileinfo) == -1)
    {
        PRINTERR("Error with stat()");
        return 1;
    }

    *size = (size_t) fileinfo.st_size + 1;

    return 0;
}

//------------------------------------------------------------------------------------------
