#ifndef DATA_READ_H
#define DATA_READ_H

//——————————————————————————————————————————————————————————————————————————————————————————

#include "tree_commands.h"
#include "tree_debug.h"
#include <sys/stat.h>
#include "math_funcs.h"
#include "text_parse.h"

//——————————————————————————————————————————————————————————————————————————————————————————

MathErr_t TreeReadInputData (MathCtx_t* math_ctx);

MathErr_t TreeReadData      (MathCtx_t*     math_ctx,
                             const char*    data_file_path);

int       ReadFile          (FILE*          fp,
                             char**         buffer_ptr,
                             const char*    file_path);

int       CountSize         (const char* file_path, size_t* size);

//——————————————————————————————————————————————————————————————————————————————————————————

#endif /* DATA_READ_H */
