#ifndef TREE_TYPES_H
#define TREE_TYPES_H

//——————————————————————————————————————————————————————————————————————————————————————————

#ifdef TREE_DEBUG
    #define DEBUG
#endif

//——————————————————————————————————————————————————————————————————————————————————————————

#include "common.h"

//——————————————————————————————————————————————————————————————————————————————————————————

typedef enum MathDataType
{
    TYPE_VAR,
    TYPE_NUM,
    TYPE_OP
} MathDataType_t;

//——————————————————————————————————————————————————————————————————————————————————————————

typedef enum MathOperations
{
    OP_ADD,
    OP_SUB,
    OP_MUL,
    OP_DIV,
    OP_SIN,
    OP_COS,
    OP_TG,
    OP_CTG,
    OP_LN,
    OP_DEG
} MathOp_t;

//——————————————————————————————————————————————————————————————————————————————————————————

typedef struct OpCase
{
    MathOp_t    code;
    const char* str;
    int         args_count;
    const char* color;
    const char* fillcolor;
    const char* fontcolor;
} OpCase_t;

//——————————————————————————————————————————————————————————————————————————————————————————

const OpCase_t OP_CASES_TABLE[] =
{
    [OP_ADD] = {OP_ADD, "+",   2, "#065f96ff", "#58bbf8ff", "#043351ff"},
    [OP_SUB] = {OP_SUB, "-",   2, "#065f96ff", "#58bbf8ff", "#043351ff"},
    [OP_MUL] = {OP_MUL, "*",   2, "#065f96ff", "#58bbf8ff", "#043351ff"},
    [OP_DIV] = {OP_DIV, "/",   2, "#065f96ff", "#58bbf8ff", "#043351ff"},
    [OP_SIN] = {OP_SIN, "sin", 1, "#065f96ff", "#58bbf8ff", "#043351ff"},
    [OP_COS] = {OP_COS, "cos", 1, "#065f96ff", "#58bbf8ff", "#043351ff"},
    [OP_TG]  = {OP_TG,  "tg",  1, "#065f96ff", "#58bbf8ff", "#043351ff"},
    [OP_CTG] = {OP_CTG, "ctg", 1, "#065f96ff", "#58bbf8ff", "#043351ff"},
    [OP_LN]  = {OP_LN,  "ln",  1, "#065f96ff", "#58bbf8ff", "#043351ff"},
    [OP_DEG] = {OP_DEG, "^",   2, "#065f96ff", "#58bbf8ff", "#043351ff"}
};

//——————————————————————————————————————————————————————————————————————————————————————————

const size_t OP_CASES_TABLE_SIZE = sizeof(OP_CASES_TABLE) / sizeof(OP_CASES_TABLE[0]);

//——————————————————————————————————————————————————————————————————————————————————————————

typedef union MathValue
{
    MathOp_t op;
    double   num;
    size_t   var;
} MathValue_t;

//——————————————————————————————————————————————————————————————————————————————————————————

typedef struct MathData
{
    MathDataType_t type;
    MathValue_t    value;
} MathData_t;

//——————————————————————————————————————————————————————————————————————————————————————————

typedef MathData_t TreeElem_t;

const TreeElem_t TREE_POISON = {TYPE_NUM, { .num = 0 }};

//——————————————————————————————————————————————————————————————————————————————————————————

typedef struct TreeNode
{
    TreeElem_t data;

    TreeNode*  left;
    TreeNode*  right;

    TreeNode*  parent;

} TreeNode_t;

//——————————————————————————————————————————————————————————————————————————————————————————

const int MAX_FILE_PATH_LEN = 256;
const int MAX_DIR_PATH_LEN  = 100;
const int MAX_FILE_NAME_LEN = 120;
const int MAX_STR_TIME_LEN  = 64;

//——————————————————————————————————————————————————————————————————————————————————————————

typedef struct TreeDebugData
{
    FILE* fp;

    char log_file_path   [MAX_FILE_PATH_LEN];
    char graph_file_name [MAX_FILE_NAME_LEN];
    char img_file_path   [MAX_FILE_PATH_LEN];
    char dot_file_path   [MAX_FILE_PATH_LEN];

    int graphs_count;

    char str_time [MAX_STR_TIME_LEN];
    char log_dir  [MAX_DIR_PATH_LEN];
    char img_dir  [MAX_DIR_PATH_LEN];
    char dot_dir  [MAX_DIR_PATH_LEN];

} TreeDebugData_t;

//——————————————————————————————————————————————————————————————————————————————————————————

typedef struct Tree
{
    TreeNode_t* dummy; // null (fake) element
    size_t      size;

    char* buffer;

} Tree_t;

//——————————————————————————————————————————————————————————————————————————————————————————

typedef enum TreeErr
{
    TREE_SUCCESS           = 0,
    TREE_NULL              = 1,
    TREE_SIZE_EXCEEDS_MAX  = 2,
    TREE_LOOP              = 3,

    TREE_CALLOC_ERROR      = 4,
    TREE_DUMP_ERROR        = 5,
    TREE_SYSTEM_FUNC_ERROR = 6,
    TREE_FILE_ERROR        = 7,
    TREE_STACK_ERROR       = 8,

    TREE_INVALID_INPUT     = 9
} TreeErr_t;

//——————————————————————————————————————————————————————————————————————————————————————————

const char* const TREE_STR_ERRORS[] =
{
    [TREE_SUCCESS]            = "Tree is ok",
    [TREE_NULL]               = "Nullptr given",
    [TREE_SIZE_EXCEEDS_MAX]   = "Tree size exceeded maximum possible value",
    [TREE_LOOP]               = "Tree is looped",
    [TREE_CALLOC_ERROR]       = "Memory allocation for tree failed",
    [TREE_DUMP_ERROR]         = "Error with dumping tree",
    [TREE_SYSTEM_FUNC_ERROR]  = "System function failed",
    [TREE_FILE_ERROR]         = "Error with opening/writing to file",
    [TREE_STACK_ERROR]        = "Error with stack commands",
    [TREE_INVALID_INPUT]      = "User input is invalid"
};

//——————————————————————————————————————————————————————————————————————————————————————————

typedef struct TreeDumpInfo
{
    TreeErr_t   error;
    const char* func;
    const char* file;
    int         line;
} TreeDumpInfo_t;

//——————————————————————————————————————————————————————————————————————————————————————————

typedef struct TypeCase
{
    MathDataType_t type;
    const char*    name;
    const char*    shape;
    const char*    color;
    const char*    fillcolor;
    const char*    fontcolor;
} TypeCase_t;

//——————————————————————————————————————————————————————————————————————————————————————————

const TypeCase_t TYPE_CASES_TABLE[] =
{
    [TYPE_VAR] = {TYPE_VAR, "VAR",  "Mrecord", "#006400", "#C0FFC0", "#006400"},
    [TYPE_NUM] = {TYPE_NUM, "NUM",  "Mrecord", "#990000", "#FFC0C0", "#990000"},
    [TYPE_OP] =  {TYPE_OP,  "OPER", "record",  "#000064", "#C0C0FF", "#000064"}
};

//——————————————————————————————————————————————————————————————————————————————————————————

const size_t TREE_MAX_SIZE = 1024 * 1024 * 1024;

//——————————————————————————————————————————————————————————————————————————————————————————

#endif /* TREE_TYPES_H */
