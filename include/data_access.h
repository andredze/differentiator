#ifndef DATA_ACCESS_H
#define DATA_ACCESS_H

//——————————————————————————————————————————————————————————————————————————————————————————

#include "tree_commands.h"
#include <ctype.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <time.h>

//——————————————————————————————————————————————————————————————————————————————————————————

void      SkipSpaces       (char* buffer, ssize_t* pos                  );
int       SkipLetter       (char* buffer, ssize_t* pos, char letter     );
TreeNode_t* ReadNode       (Tree_t* tree, char* buffer, ssize_t* pos    );
TreeErr_t   ReadNodeData   (char* buffer, ssize_t* pos, char** node_data);
TreeErr_t   TreeReadData   (Tree_t* tree, const char* data_file_path    );
TreeErr_t   TreeWriteData  (const Tree_t* tree, const char* file_path   );

void      WriteNode          (const TreeNode_t* node, FILE* fp, int rank);

//——————————————————————————————————————————————————————————————————————————————————————————

int  ReadFile         (FILE* fp, char** buffer_ptr, const char* file_path);
int  CountSize        (const char* file_path, size_t* size);
void MakeDataFilePath (char* data_file_path);

//——————————————————————————————————————————————————————————————————————————————————————————

const int MAX_TIME_LEN      = 64;
const int MAX_FILE_NAME_LEN = 1024;

//——————————————————————————————————————————————————————————————————————————————————————————

#endif /* DATA_ACCESS_H */
