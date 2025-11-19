#include "data_access.h"

//------------------------------------------------------------------------------------------

TreeErr_t TreeWriteData(const Tree_t* tree, const char* file_path)
{
    DEBUG_TREE_CHECK(tree, "ERROR BEFORE TREE WRITE DATA");

    char data_file_path[MAX_FILE_NAME_LEN] = "";

    if (file_path == NULL)
    {
        MakeDataFilePath(data_file_path);
    }
    else
    {
        strcpy(data_file_path, file_path);
    }

    FILE* fp = fopen(data_file_path, "w");

    if (fp == NULL)
    {
        PRINTERR("Error with opening file: %s", data_file_path);
        return TREE_FILE_ERR;
    }

    WriteNode(tree->dummy->right, fp, 0);

    fclose(fp);

    DPRINTF("���� ������ �������� � ����: %s\n", data_file_path);

    return TREE_SUCCESS;
}

//------------------------------------------------------------------------------------------

void MakeDataFilePath(char* data_file_path)
{
    time_t rawtime = time(NULL);

    struct tm* info = localtime(&rawtime);

    char time[MAX_TIME_LEN] = "";

    strftime(time, sizeof(time), "data_%d-%b-%Y_%H-%M-%S", info);

    snprintf(data_file_path, MAX_FILE_NAME_LEN, "data/%s.txt", time);
}

//------------------------------------------------------------------------------------------

void WriteNode(const TreeNode_t* node, FILE* fp, int rank)
{
    assert(fp != NULL);

    if (node == NULL)
    {
        fprintf(fp, " nil");
        return;
    }
    for (int i = 0; i < rank; i++)
    {
        fprintf(fp, "\t");
    }

    // fprintf(fp, "( \"%s\"", node->data);

    if (node->left != NULL)
    {
        fprintf(fp, "\n");
    }

    WriteNode(node->left, fp, rank + 1);

    WriteNode(node->right, fp, rank + 1);

    if (node->left != NULL)
    {
        for (int i = 0; i < rank; i++)
        {
            fprintf(fp, "\t");
        }
    }
    else
    {
        fprintf(fp, " ");
    }
    fprintf(fp, ")\n");
}

//------------------------------------------------------------------------------------------
