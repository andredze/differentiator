#include "data_read.h"

//------------------------------------------------------------------------------------------

#ifdef TREE_DEBUG
    #define BUFFER_DUMP(buffer, pos, fmt, ...)                      \
            BEGIN                                                   \
            TreeReadBufferDump(buffer, pos, fmt, ##__VA_ARGS__);    \
            END
#else
    #define BUFFER_DUMP(buffer, pos, fmt, ...) ;
#endif

//------------------------------------------------------------------------------------------

TreeErr_t TreeReadData(Tree_t* tree, const char* data_file_path)
{
    DEBUG_TREE_CHECK(tree, "ERROR BEFORE TREE READ DATA");

    assert(data_file_path != NULL);

    DPRINTF("Reading file %s\n", data_file_path);

    FILE* fp = fopen(data_file_path, "r");

    if (fp == NULL)
    {
        PRINTERR("Error with opening file: %s", data_file_path);
        return TREE_FILE_ERR;
    }

    char* buffer = NULL;

    if (ReadFile(fp, &buffer, data_file_path))
    {
        return TREE_FILE_ERR;
    }
    tree->buffer = buffer;

    ssize_t i = 0;

    tree->dummy->right = ReadNode(tree, buffer, &i);

    if (tree->dummy->right == NULL)
        return TREE_NULL;

    DEBUG_TREE_CHECK(tree, "ERROR AFTER TREE READ DATA");
    TREE_CALL_DUMP  (tree, "DUMP AFTER TREE READ DATA %s", data_file_path);

    return TREE_SUCCESS;
}

//------------------------------------------------------------------------------------------

TreeNode_t* ReadNode(Tree_t* tree, char* buffer, ssize_t* pos)
{
    assert(buffer != NULL);
    assert(tree   != NULL);
    assert(pos    != NULL);

    TreeNode_t* node = NULL;

    char first_char = buffer[*pos];

    if (first_char == '(')
    {
        (*pos)++;
        SkipSpaces(buffer, pos);
        BUFFER_DUMP(buffer, *pos, "BUFFER DUMP SKIPPING OPENING BRACKET");

        TreeElem_t data = TREE_POISON;

        if (ReadNodeData(buffer, pos, &data))
            return NULL;

        BUFFER_DUMP(buffer, *pos, "BUFFER DUMP AFTER READING %s", TYPE_CASES_TABLE[data.type].name);

        if ((node = TreeNodeCtor(tree, data, NULL, NULL)) == NULL)
            return NULL;

        node->left  = ReadNode(tree, buffer, pos);
        BUFFER_DUMP(buffer, *pos, "BUFFER DUMP AFTER READING NODE LEFT TO %s", TYPE_CASES_TABLE[data.type].name);

        node->right = ReadNode(tree, buffer, pos);
        BUFFER_DUMP(buffer, *pos, "BUFFER DUMP AFTER READING NODE RIGHT TO %s", TYPE_CASES_TABLE[data.type].name);

        TreeDumpInfo_t dump_info = {TREE_SUCCESS, __PRETTY_FUNCTION__, __FILE__, __LINE__};
        TreeDump(tree, &dump_info, "DUMP AFTER NODE CTOR %s", TYPE_CASES_TABLE[data.type].name);

        if (SkipLetter(buffer, pos, ')'))
            return NULL;

        SkipSpaces(buffer, pos);

        BUFFER_DUMP(buffer, *pos, "BUFFER DUMP AFTER SKIPPING CLOSING BRACKET after %s", TYPE_CASES_TABLE[data.type].name);
    }
    else if (strncmp(&buffer[*pos], "nil", 3) == 0)
    {
        (*pos) += 3;
        SkipSpaces(buffer, pos);
        BUFFER_DUMP(buffer, *pos, "BUFFER DUMP AFTER READING NULL");
    }
    else
    {
        PRINTERR("Syntax error in tree data (unknown symbol = \"%c\" )\n", first_char);
        return NULL;
    }

    return node;
}

//------------------------------------------------------------------------------------------

TreeErr_t ReadNodeData(char* buffer, ssize_t* pos, MathData_t* data)
{
    assert(buffer != NULL);
    assert(data   != NULL);
    assert(pos    != NULL);

    int data_len = 0;

    if (sscanf(&buffer[*pos], "\"%*[^\"]\"%n", &data_len) != 0)
    {
        PRINTERR("Error with reading data");
        return TREE_FILE_ERR;
    }
    DPRINTF("buffer + pos: %s, data_len = %zu\n", &buffer[*pos], data_len);

    if (GetMathData(buffer + *pos + 1, (size_t) (data_len - 2), data))
        return TREE_INVALID_INPUT;

    (*pos) += data_len; /* moving pos to the next data */

    SkipSpaces(buffer, pos);

    return TREE_SUCCESS;
}

//------------------------------------------------------------------------------------------

TreeErr_t GetMathData(char* str, size_t str_len, MathData_t* data)
{
    assert(data != NULL);
    assert(str  != NULL);

    if (ProcessMathDataOpCase(str, str_len, data) == 1)
        return TREE_SUCCESS;

    if (ProcessMathDataVarCase(str, str_len, data) == 1)
        return TREE_SUCCESS;

    if (ProcessMathDataNumCase(str, data) == 1)
        return TREE_SUCCESS;

    PRINTERR("Syntax error: input doesn't match format, input: %s, len = %d", str, str_len);

    return TREE_INVALID_INPUT;
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

int ProcessMathDataVarCase(char* str, size_t str_len, MathData_t* data)
{
    assert(data != NULL);
    assert(str  != NULL);

    char var = *str;

    if (str_len != 1 || !isalpha(var))
        return 0;

    data->type      = TYPE_VAR;
    data->value.var = var;

    return 1;
}

//------------------------------------------------------------------------------------------

int CompareDoubles(double val1, double val2)
{
    if (val1 + DOUBLE_EPS < val2 - DOUBLE_EPS)
    {
        return -1;
    }
    else if (val2 + DOUBLE_EPS < val1 - DOUBLE_EPS)
    {
        return 1;
    }

    return 0;
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
