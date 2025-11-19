#ifndef MATH_TYPES_H
#define MATH_TYPES_H

//——————————————————————————————————————————————————————————————————————————————————————————

#include "tree_types.h"

//——————————————————————————————————————————————————————————————————————————————————————————

typedef struct VarCase
{
    char*  str;
    size_t hash;
    double value;
} VarCase_t;

//——————————————————————————————————————————————————————————————————————————————————————————

typedef struct MathContext
{
    Tree_t tree;

    VarCase_t* vars_table;
    size_t     size;
    size_t     capacity;

} MathCtx_t;

//——————————————————————————————————————————————————————————————————————————————————————————

typedef enum MathErr
{
    MATH_SUCCESS,
    MATH_NULL,
    MATH_ALLOC_ERROR,
    MATH_TREE_ERROR,
    MATH_UNKNOWN_OP,
    MATH_INVALID_INPUT
} MathErr_t;

//——————————————————————————————————————————————————————————————————————————————————————————

#endif /* MATH_TYPES_H */
