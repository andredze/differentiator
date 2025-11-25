#ifndef MATH_TYPES_H
#define MATH_TYPES_H

//——————————————————————————————————————————————————————————————————————————————————————————

#include "tree_types.h"

//——————————————————————————————————————————————————————————————————————————————————————————

typedef struct VarCase
{
    char*   str;
    double  value;
} VarCase_t;

//——————————————————————————————————————————————————————————————————————————————————————————

typedef struct Vars
{
    VarCase_t* data;
    size_t     size;
    size_t     capacity;
} Vars_t;

//——————————————————————————————————————————————————————————————————————————————————————————

typedef struct MathContext
{
    Tree_t tree;
    Vars_t vars;
} MathCtx_t;

//——————————————————————————————————————————————————————————————————————————————————————————

typedef enum MathErr
{
    MATH_SUCCESS = 0,
    MATH_NULL,
    MATH_ALLOC_ERROR,
    MATH_TREE_ERROR,
    MATH_UNKNOWN_OP,
    MATH_UNKNOWN_TYPE,
    MATH_INVALID_INPUT,
    MATH_FILE_ERROR
} MathErr_t;

//——————————————————————————————————————————————————————————————————————————————————————————

const char* const MATH_STR_ERRORS[] =
{
    [MATH_SUCCESS]       = "Math context is fine",
    [MATH_NULL]          = "Error: nullptr",
    [MATH_ALLOC_ERROR]   = "Memory allocation failed",
    [MATH_TREE_ERROR]    = "Error with tree",
    [MATH_UNKNOWN_OP]    = "Error: unknown math operation",
    [MATH_UNKNOWN_TYPE]  = "Error: unknown math data type",
    [MATH_INVALID_INPUT] = "Error with input",
    [MATH_FILE_ERROR]    = "Error with file operations"
};

//——————————————————————————————————————————————————————————————————————————————————————————

const double EULER_NUMBER = 2.71828;
const double PI_NUMBER    = 3.14159;

//——————————————————————————————————————————————————————————————————————————————————————————

#endif /* MATH_TYPES_H */
