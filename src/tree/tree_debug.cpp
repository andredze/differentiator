#include "tree_debug.h"

//------------------------------------------------------------------------------------------

int TreePrintElement(const TreeElem_t* data)
{
    switch (data->type)
    {
        case TYPE_OP:
            printf("{OP, {%s}}", OP_CASES_TABLE[data->value.op].str);
            break;
        case TYPE_NUM:
            printf("{NUM, {%lg}}", data->value.num);
            break;
        case TYPE_VAR:
            printf("{VAR, {%zu}}", data->value.var);
            // printf("{VAR, {%c [%d]}}", vars[data->value.var], data->value.var);
            break;
        default:
            printf("NaN");
            return -1;
    }

    return 0;
}

//------------------------------------------------------------------------------------------

int SetDirectories(char* log_filename, char* log_dir)
{
    assert(log_filename != NULL);
    assert(log_dir      != NULL);

    time_t rawtime = time(NULL);

    struct tm* info = localtime(&rawtime);

    char time_dir[MAX_FILENAME_LEN / 2] = "";

    static char image_dir [MAX_FILENAME_LEN] = "";
    static char dot_dir   [MAX_FILENAME_LEN] = "";

    strftime(time_dir, sizeof(time_dir), "[%Y-%m-%d_%H%M%S]", info);

    snprintf(log_dir, MAX_FILENAME_LEN, "log/%s", time_dir);
    DPRINTF("log_dir: log/%s\n", time_dir);
    mkdir(log_dir, 0777);

    snprintf(image_dir, sizeof(image_dir), "log/%s/svg", time_dir);
    mkdir(image_dir, 0777);

    snprintf(dot_dir, sizeof(dot_dir), "log/%s/dot", time_dir);
    mkdir(dot_dir, 0777);

    snprintf(log_filename, MAX_FILENAME_LEN, "log/%s/tree.html", time_dir);

    DPRINTF("Made logs: log_dir = %s;\n", log_dir);

    return 0;
}

//------------------------------------------------------------------------------------------

TreeErr_t TreeDump(MathCtx_t* math_ctx,
                   const TreeDumpInfo_t* dump_info,
                   const char* fmt, ...)
{
    va_list args = {};
    va_start(args, fmt);

    TreeErr_t result = vTreeDump(math_ctx, dump_info, fmt, args);

    va_end(args);

    return result;
}

//------------------------------------------------------------------------------------------

MathErr_t MathVarsTableDump(const MathCtx_t* math_ctx, const char* fmt, ...)
{
    assert(fmt != NULL);

    va_list args = {};
    va_start(args, fmt);

    FILE* fp = NULL;

    if (TreeOpenLogFile(&fp, NULL, NULL))
        return MATH_TREE_ERROR;

    fprintf(fp, "<pre><h4><font color=blue>");

    vfprintf(fp, fmt, args);

    fprintf(fp, "</h4></font>");

    fprintf(fp, "vars_table [%p]:\n\n"
                "size     = %zu\n"
                "capacity = %zu\n\n",
                math_ctx->vars_table,
                math_ctx->size,
                math_ctx->capacity);

    fprintf(fp, "index: ");

    for (size_t i = 0; i < math_ctx->capacity; i++)
    {
        fprintf(fp, "%12zu |", i);
    }

    fprintf(fp, "\nnames: ");

    for (size_t i = 0; i < math_ctx->capacity; i++)
    {
        fprintf(fp, "%12s |", math_ctx->vars_table[i].str);
    }

    fprintf(fp, "\nhash:  ");

    for (size_t i = 0; i < math_ctx->capacity; i++)
    {
        fprintf(fp, "%12zu |", math_ctx->vars_table[i].hash);
    }

    fclose(fp);

    va_end(args);

    return MATH_SUCCESS;
}

//------------------------------------------------------------------------------------------

TreeErr_t TreeReadBufferDump(const char* buffer, ssize_t pos, const char* fmt, ...)
{
    assert(fmt != NULL);

    va_list args = {};
    va_start(args, fmt);

    TreeErr_t error = TREE_SUCCESS;
    FILE* fp = NULL;

    if ((error = TreeOpenLogFile(&fp, NULL, NULL)))
        return error;

    fprintf(fp, "<pre><h4><font color=green>");

    vfprintf(fp, fmt, args);

    fprintf(fp, "</h4></font>\n"
                "<font color=gray>");

    fprintf(fp, "\"");

    for (int i = 0; i < pos; i++)
    {
        fprintf(fp, "%c", buffer[i]);
    }

    fprintf(fp, "</font><font color=red>%c</font>", buffer[pos]);

    if (*(buffer + pos) != '\0')
    {
        fprintf(fp, "<font color=blue>%s</font>\n\n", buffer + pos + 1);
    }

    fprintf(fp, "\"");

    fclose(fp);

    va_end(args);

    return TREE_SUCCESS;
}

//------------------------------------------------------------------------------------------

TreeErr_t vTreeDump(MathCtx_t* math_ctx,
                    const TreeDumpInfo_t* dump_info,
                    const char* fmt, va_list args)
{
    assert(math_ctx != NULL);
    assert(dump_info != NULL);

    Tree_t* tree = &math_ctx->tree;

    TreeErr_t error = TREE_SUCCESS;

    FILE* fp = NULL;
    int   calls_count = 0;
    char  log_dir[MAX_FILENAME_LEN] = "";

    if ((error = TreeOpenLogFile(&fp, &calls_count, log_dir)))
        return error;

    char graph_name[MAX_FILENAME_LEN] = {};

    snprintf(graph_name, sizeof(graph_name), "%04d", calls_count);

    fprintf(fp, "<pre>\n<h3><font color=blue>");

    vfprintf(fp, fmt, args);

    fprintf(fp, "</font></h3>");

    fprintf(fp, dump_info->error == TREE_SUCCESS ?
                "<font color=green><b>" :
                "<font color=red><b>ERROR: ");

    fprintf(fp, "%s (code %d)</b></font>\n"
                "TREE DUMP called from %s at %s:%d\n\n",
                TREE_STR_ERRORS[dump_info->error],
                dump_info->error,
                dump_info->func,
                dump_info->file,
                dump_info->line);

    fprintf(fp, "tree [%p]:\n\n"
                "size  = %zu;\n"
                "dummy = %p;\n",
                tree, tree->size, tree->dummy);

    TreeErr_t graph_error = TREE_SUCCESS;
    if ((graph_error = TreeGraphDump(math_ctx, graph_name, log_dir)))
    {
        fclose(fp);
        return graph_error;
    }

    int image_width = tree->size <= 5 ? 25 : 50;

    fprintf(fp, "\n<img src = svg/%s.svg width = %d%%>\n\n"
                "============================================================="
                "=============================================================\n\n",
                graph_name, image_width);

    fclose(fp);

    return TREE_SUCCESS;
}

//------------------------------------------------------------------------------------------

TreeErr_t TreeOpenLogFile(FILE** fp_ptr, int* calls_count_ptr, char* dest_log_dir)
{
    assert(fp_ptr != NULL);

    static int  calls_count = 0;
    static char log_filename [MAX_FILENAME_LEN] = "";
    static char log_dir      [MAX_FILENAME_LEN] = "";

    calls_count++;

    if (calls_count == 1)
    {
        SetDirectories(log_filename, log_dir);
    }

    *fp_ptr = fopen(log_filename, calls_count == 1 ? "w" : "a");

    if (*fp_ptr == NULL)
    {
        PRINTERR("Opening logfile %s failed", log_filename);
        return TREE_DUMP_ERROR;
    }

    if (dest_log_dir != NULL)
        strcpy(dest_log_dir, log_dir);

    if (calls_count_ptr != NULL)
        *calls_count_ptr = calls_count;

    return TREE_SUCCESS;
}

//------------------------------------------------------------------------------------------

TreeErr_t TreeGraphDump(MathCtx_t* math_ctx,
                        const char* graph_name,
                        const char* log_dir)
{
    assert(math_ctx != NULL);

    Tree_t* tree = &math_ctx->tree;

    if (tree == NULL)
    {
        PRINTERR("TREE_NULL");
        return    TREE_NULL;
    }

    char dot_file_path[MAX_FILENAME_LEN] = {};
    char svg_file_path[MAX_FILENAME_LEN] = {};

    SetGraphFilepaths(dot_file_path, svg_file_path, log_dir, graph_name);

    FILE* dot_file = fopen(dot_file_path, "w");

    if (dot_file == NULL)
    {
        PRINTERR("Failed opening logfile");
        return TREE_DUMP_ERROR;
    }

    DumpGraphTitle(dot_file);
    if (DumpTreeDefaultNode(tree->dummy, "type = PZN | value = PZN", "#3E3A22", "#ecede8", "#3E3A22", "record", dot_file))
    {
        return TREE_DUMP_ERROR;
    }
    if (DumpTreeEdges(tree->dummy, dot_file))
    {
        return TREE_DUMP_ERROR;
    }

    if (tree->dummy->right != NULL)
    {
        TreeErr_t error = TREE_SUCCESS;

        if ((error = TreeNodeDump(tree->dummy->right, dot_file, math_ctx)))
        {
            return error;
        }
    }

    fprintf(dot_file, "}\n");

    fclose(dot_file);

    TreeErr_t error = TREE_SUCCESS;
    if ((error = TreeConvertGraphFile(dot_file_path, svg_file_path)))
    {
        return error;
    }

    return TREE_SUCCESS;
}

//------------------------------------------------------------------------------------------

void SetGraphFilepaths(char*       dot_file_path,
                       char*       svg_file_path,
                       const char* log_dir,
                       const char* graph_name)
{
    assert(dot_file_path != NULL);
    assert(svg_file_path != NULL);
    assert(graph_name    != NULL);
    assert(log_dir       != NULL);

    snprintf(dot_file_path, MAX_FILENAME_LEN, "%s/dot/%s.dot", log_dir, graph_name);
    snprintf(svg_file_path, MAX_FILENAME_LEN, "%s/svg/%s.svg", log_dir, graph_name);
}

//------------------------------------------------------------------------------------------

void DumpGraphTitle(FILE* dot_file)
{
    assert(dot_file != NULL);

    fprintf(dot_file,
    "digraph Tree\n{\n\t"
    R"(ranksep=0.75;
    nodesep=0.5;
    node [
        fontname  = "Arial",
        shape     = "Mrecord",
        style     = "filled",
        color     = "#3E3A22",
        fillcolor = "#E3DFC9",
        fontcolor = "#3E3A22"
    ];)""\n");
}

//------------------------------------------------------------------------------------------

TreeErr_t TreeConvertGraphFile(const char* dot_file_path,
                               const char* svg_file_path)
{
    assert(dot_file_path != NULL);
    assert(svg_file_path != NULL);

    char command[MAX_COMMAND_LEN] = {};

    snprintf(command, sizeof(command), "dot %s -T svg -o %s", dot_file_path, svg_file_path);

    int result = system(command);

    if (result == 0)
    {
        DPRINTF("Generated graph dump: %s\n", svg_file_path);
    }
    else
    {
        PRINTERR("TREE_SYSTEM_FUNC_ERR");
        return    TREE_SYSTEM_FUNC_ERR;
    }

    return TREE_SUCCESS;
}

//------------------------------------------------------------------------------------------

TreeErr_t TreeNodeDump(const TreeNode_t* node, FILE* fp, MathCtx_t* math_ctx)
{
    assert(node != NULL);
    assert(fp   != NULL);

    TreeErr_t error = TREE_SUCCESS;

    if (node->left != NULL)
    {
        if ((error = TreeNodeDump(node->left, fp, math_ctx)))
            return error;
    }

    if ((error = DumpTreeNodeAndEdges(node, fp, math_ctx)))
        return error;

    if (node->right != NULL)
    {
        if ((error = TreeNodeDump(node->right, fp, math_ctx)))
            return error;
    }

    return TREE_SUCCESS;
}

//------------------------------------------------------------------------------------------

void TreeNodePrint(const TreeNode_t* node)
{
    assert(node != NULL);

    printf("(");

    if (node->left != NULL)
    {
        TreeNodePrint(node->left);
    }

    printf(" ");
    TreePrintElement(&node->data);
    printf(" ");

    if (node->right != NULL)
    {
        TreeNodePrint(node->right);
    }

    printf(")");
}

//------------------------------------------------------------------------------------------

TreeErr_t DumpTreeNodeAndEdges(const TreeNode_t* node, FILE* fp, MathCtx_t* math_ctx)
{
    if (DumpTreeSingleNode(node, fp, math_ctx))
    {
        return TREE_DUMP_ERROR;
    }
    if (DumpTreeEdges(node, fp))
    {
        return TREE_DUMP_ERROR;
    }

    return TREE_SUCCESS;
}

//------------------------------------------------------------------------------------------

int DumpTreeEdges(const TreeNode_t* node, FILE* fp)
{
    assert(fp   != NULL);
    assert(node != NULL);

    if (node->left  == NULL &&
        node->right == NULL)
    {
        return 0;
    }

    char name_node [MAX_NODE_NAME_LEN] = "";
    sprintf (name_node , "node%p", node       );

    if (node->left != NULL)
    {
        char name_left [MAX_NODE_NAME_LEN] = "";
        sprintf (name_left , "node%p", node->left );
        DumpEdge(name_node, name_left , "#404040", NULL, NULL, NULL, NULL, NULL, NULL, fp);
    }
    if (node->right != NULL)
    {
        char name_right[MAX_NODE_NAME_LEN] = "";
        sprintf (name_right, "node%p", node->right);
        DumpEdge(name_node, name_right, "#404040", NULL, NULL, NULL, NULL, NULL, NULL, fp);
    }

    return 0;
}

//------------------------------------------------------------------------------------------

int DumpTreeSingleNode(const TreeNode_t* node, FILE* fp, MathCtx_t* math_ctx)
{
    assert(node != NULL);
    assert(fp   != NULL);

    switch (node->data.type)
    {
        case TYPE_OP:
            DumpTreeOpNode(node, fp);
            break;

        case TYPE_NUM:
            DumpTreeNumNode(node, fp);
            break;

        case TYPE_VAR:
            DumpTreeVarNode(node, fp, math_ctx);
            break;

        default:
            return 1;
    }

    return 0;
}

//------------------------------------------------------------------------------------------

int DumpTreeOpNode(const TreeNode_t* node, FILE* fp)
{
    assert(node != NULL);
    assert(fp   != NULL);

    char str_data[MAX_DATA_LEN] = {};

    snprintf(str_data, sizeof(str_data), "type = %s | value = %s",
                                         TYPE_CASES_TABLE[node->data.type].name,
                                         OP_CASES_TABLE[node->data.value.op].str);

    return DumpTreeDefaultNode(node, str_data,
                               OP_CASES_TABLE[node->data.value.op].color,
                               OP_CASES_TABLE[node->data.value.op].fillcolor,
                               OP_CASES_TABLE[node->data.value.op].fontcolor,
                               TYPE_CASES_TABLE[node->data.type].shape,
                               fp);
}

//------------------------------------------------------------------------------------------

int DumpTreeVarNode(const TreeNode_t* node, FILE* fp, MathCtx_t* math_ctx)
{
    assert(math_ctx != NULL);
    assert(node     != NULL);
    assert(fp       != NULL);



    char str_data[MAX_DATA_LEN] = {};

    snprintf(str_data, sizeof(str_data), "type = %s | value = %s (%zu)",
                                         TYPE_CASES_TABLE[node->data.type].name,
                                         math_ctx->vars_table[node->data.value.var].str,
                                         node->data.value.var);

    return DumpTreeDefaultNode(node, str_data,
                               TYPE_CASES_TABLE[node->data.type].color,
                               TYPE_CASES_TABLE[node->data.type].fillcolor,
                               TYPE_CASES_TABLE[node->data.type].fontcolor,
                               TYPE_CASES_TABLE[node->data.type].shape,
                               fp);
}

//------------------------------------------------------------------------------------------

int DumpTreeNumNode(const TreeNode_t* node, FILE* fp)
{
    assert(node != NULL);
    assert(fp   != NULL);

    char str_data[MAX_DATA_LEN] = {};

    snprintf(str_data, sizeof(str_data), "type = %s | value = %lg",
                                         TYPE_CASES_TABLE[node->data.type].name,
                                         node->data.value.num);

    return DumpTreeDefaultNode(node, str_data,
                               TYPE_CASES_TABLE[node->data.type].color,
                               TYPE_CASES_TABLE[node->data.type].fillcolor,
                               TYPE_CASES_TABLE[node->data.type].fontcolor,
                               TYPE_CASES_TABLE[node->data.type].shape,
                               fp);
}

//------------------------------------------------------------------------------------------

int DumpTreeDefaultNode(const TreeNode_t* node,
                        const char* str_data,
                        const char* color,
                        const char* fillcolor,
                        const char* fontcolor,
                        const char* shape,
                        FILE* fp)
{
    assert(fp   != NULL);
    assert(node != NULL);

    char name[MAX_NODE_NAME_LEN] = "";

    sprintf(name, "node%p", node);

    char label[MAX_LABEL_LEN] = "";

    snprintf(label, MAX_LABEL_LEN, "{ %p | %s | { left = %p | right = %p }}",
                                   node, str_data, node->left, node->right);

    DumpNode(name, label, color, fillcolor, fontcolor, shape, fp);

    return 0;
}

//------------------------------------------------------------------------------------------

int DumpNode(const char* name,
             const char* label,
             const char* color,
             const char* fillcolor,
             const char* fontcolor,
             const char* shape,
             FILE* fp)
{
    assert(name != NULL);
    assert(fp   != NULL);

    fprintf(fp, "\t%s", name);

    int is_first_arg = 1;

    PrintArg("label",     label,     &is_first_arg, fp);
    PrintArg("color",     color,     &is_first_arg, fp);
    PrintArg("fillcolor", fillcolor, &is_first_arg, fp);
    PrintArg("fontcolor", fontcolor, &is_first_arg, fp);
    PrintArg("shape",     shape,     &is_first_arg, fp);

    if (is_first_arg == 0)
    {
        fprintf(fp, "]");
    }

    fprintf(fp, ";\n");

    return 0;
}

//------------------------------------------------------------------------------------------

int DumpEdge(const char* node1,
             const char* node2,
             const char* color,
             const char* constraint,
             const char* dir,
             const char* style,
             const char* arrowhead,
             const char* arrowtail,
             const char* label,
             FILE*       fp)
{
    assert(node1 != NULL);
    assert(node2 != NULL);

    fprintf(fp, "\t%s->%s", node1, node2);

    int is_first_arg = 1;

    PrintArg("color",      color,      &is_first_arg, fp);
    PrintArg("constraint", constraint, &is_first_arg, fp);
    PrintArg("dir",        dir,        &is_first_arg, fp);
    PrintArg("style",      style,      &is_first_arg, fp);
    PrintArg("arrowhead",  arrowhead,  &is_first_arg, fp);
    PrintArg("arrowtail",  arrowtail,  &is_first_arg, fp);
    PrintArg("label",      label,      &is_first_arg, fp);

    if (is_first_arg == 0)
    {
        fprintf(fp, "]");
    }

    fprintf(fp, ";\n");

    return 0;
}

//------------------------------------------------------------------------------------------

int PrintArg(const char* arg_name,
             const char* arg_value,
             int*        is_first_arg,
             FILE*       fp)
{
    assert(arg_name     != NULL);
    assert(is_first_arg != NULL);

    if (arg_value != NULL)
    {
        if (*is_first_arg)
        {
            fprintf(fp, " [");
            *is_first_arg = 0;
        }
        else
        {
            fprintf(fp, ", ");
        }

        fprintf(fp, "%s = \"%s\"", arg_name, arg_value);
    }

    return 0;
}

//------------------------------------------------------------------------------------------
