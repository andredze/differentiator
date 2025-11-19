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
    OP_DEG
} MathOp_t;

//——————————————————————————————————————————————————————————————————————————————————————————

typedef struct OpCase
{
    MathOp_t    code;
    const char* str;
    const char* color;
    const char* fillcolor;
    const char* fontcolor;
} OpCase_t;

//——————————————————————————————————————————————————————————————————————————————————————————

const OpCase_t OP_CASES_TABLE[] =
{
    [OP_ADD] = {OP_ADD, "+",   "#065f96ff", "#58bbf8ff", "#043351ff"},
    [OP_SUB] = {OP_SUB, "-",   "#065f96ff", "#58bbf8ff", "#043351ff"},
    [OP_MUL] = {OP_MUL, "*",   "#065f96ff", "#58bbf8ff", "#043351ff"},
    [OP_DIV] = {OP_DIV, "/",   "#065f96ff", "#58bbf8ff", "#043351ff"},
    [OP_SIN] = {OP_SIN, "sin", "#065f96ff", "#58bbf8ff", "#043351ff"},
    [OP_COS] = {OP_COS, "cos", "#065f96ff", "#58bbf8ff", "#043351ff"},
    [OP_DEG] = {OP_DEG, "^",   "#065f96ff", "#58bbf8ff", "#043351ff"}
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

} TreeNode_t;

//——————————————————————————————————————————————————————————————————————————————————————————

typedef struct Tree
{
    TreeNode_t* dummy; // null (fake) element
    size_t      size;
    char*       buffer;
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
    TREE_SYSTEM_FUNC_ERR   = 6,
    TREE_FILE_ERR          = 7,
    TREE_STACK_ERR         = 8,

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
    [TREE_SYSTEM_FUNC_ERR]    = "System function failed",
    [TREE_FILE_ERR]           = "Error with opening/writing to file",
    [TREE_STACK_ERR]          = "Error with stack commands",
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
