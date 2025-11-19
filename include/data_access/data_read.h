#ifndef DATA_READ_H
#define DATA_READ_H

//——————————————————————————————————————————————————————————————————————————————————————————

#include "tree_commands.h"
#include <ctype.h>
#include <sys/stat.h>

//——————————————————————————————————————————————————————————————————————————————————————————

TreeErr_t ReadNode      (Tree_t* tree, char* buffer, ssize_t* pos, TreeNode_t** pnode);
TreeErr_t TreeReadData  (Tree_t* tree, const char* data_file_path);
TreeErr_t ReadNodeData  (char* buffer, ssize_t* pos,   MathData_t* data);
TreeErr_t GetMathData   (char* str,    size_t str_len, MathData_t* data);

//——————————————————————————————————————————————————————————————————————————————————————————

int ProcessMathDataOpCase (char* str, size_t str_len, MathData_t* data);
int ProcessMathDataNumCase(char* str,                 MathData_t* data);
int ProcessMathDataVarCase(char* str, size_t str_len, MathData_t* data);

//——————————————————————————————————————————————————————————————————————————————————————————

int  CompareDoubles  (double val1, double val2);
int  SkipLetter      (char* buffer, ssize_t* pos, char letter);
int  ReadFile        (FILE* fp, char** buffer_ptr, const char* file_path);
int  CountSize       (const char* file_path, size_t* size);
void SkipSpaces      (char* buffer, ssize_t* pos);

//——————————————————————————————————————————————————————————————————————————————————————————

const double DOUBLE_EPS = 1e-7;

//——————————————————————————————————————————————————————————————————————————————————————————

#endif /* DATA_READ_H */
