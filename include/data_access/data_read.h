#ifndef DATA_READ_H
#define DATA_READ_H

//——————————————————————————————————————————————————————————————————————————————————————————

#include "tree_commands.h"
#include "tree_debug.h"
#include <ctype.h>
#include <sys/stat.h>
#include "math_funcs.h"

//——————————————————————————————————————————————————————————————————————————————————————————

MathErr_t TreeReadInputData(MathCtx_t* math_ctx);

MathErr_t TreeReadData  (MathCtx_t* math_ctx, const char* data_file_path);
MathErr_t ReadNode      (MathCtx_t* math_ctx, char* buffer, ssize_t* pos, TreeNode_t** pnode);
MathErr_t ReadNodeData  (MathCtx_t* math_ctx, char* buffer, ssize_t* pos, MathData_t* data);
MathErr_t GetMathData   (MathCtx_t* math_ctx, char* str,  size_t str_len, MathData_t* data);

//——————————————————————————————————————————————————————————————————————————————————————————

int ProcessMathDataOpCase (                     char* str, size_t str_len, MathData_t* data);
int ProcessMathDataNumCase(                     char* str,                 MathData_t* data);
int ProcessMathDataVarCase(MathCtx_t* math_ctx, char* str, size_t str_len, MathData_t* data);

MathErr_t PutVarInTable(MathCtx_t* math_ctx, char* str, size_t str_len, MathData_t* data);

//——————————————————————————————————————————————————————————————————————————————————————————

int  SkipLetter      (char* buffer, ssize_t* pos, char letter);
int  ReadFile        (FILE* fp, char** buffer_ptr, const char* file_path);
int  CountSize       (const char* file_path, size_t* size);
void SkipSpaces      (char* buffer, ssize_t* pos);

//——————————————————————————————————————————————————————————————————————————————————————————

#endif /* DATA_READ_H */
