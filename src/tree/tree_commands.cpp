#include "tree_commands.h"

//------------------------------------------------------------------------------------------

#ifdef TREE_DEBUG

TreeErr_t TreeCheck(const Tree_t* tree,
                    const char*   func,
                    const char*   file,
                    int           line,
                    const char*   fmt, ...)
{
    assert(func != NULL);
    assert(file != NULL);
    assert(fmt  != NULL);

    TreeErr_t verify_status = TREE_SUCCESS;

    if ((verify_status = TreeVerify(tree)))
    {
        PRINTERR("%s (TreeVerify not passed! Check \"tree.html\")", TREE_STR_ERRORS[verify_status]);

        // TreeDumpInfo_t dump_info = {verify_status, func, file, line};

        va_list args = {};
        va_start(args, fmt);

        // if (vTreeDump(tree, &dump_info, fmt, args))
        // {
        //     return TREE_DUMP_ERROR;
        // }

        va_end(args);
    }

    return verify_status;
}

#endif /* TREE_DEBUG */

//------------------------------------------------------------------------------------------

TreeErr_t TreeCtor(Tree_t* tree)
{
    if (tree == NULL)
    {
        PRINTERR("Tree is a nullptr");
        return TREE_NULL;
    }

    tree->dummy = TreeNodeCtor(tree, {TYPE_NUM, { .num = 0 }}, NULL, NULL);

    if (tree->dummy == NULL)
    {
        PRINTERR("Dummy construction failed");
        return TREE_NULL;
    }

    tree->buffer = NULL;
    tree->size   = 0;

    DEBUG_TREE_CHECK(tree, "ERROR DUMP AFTER CTOR");
    DPRINTF("> TreeCtor   END\n");

    return TREE_SUCCESS;
}

//------------------------------------------------------------------------------------------

TreeNode_t* TreeNodeCtor(Tree_t*        tree,
                         TreeElem_t     data,
                         TreeNode_t*    left,
                         TreeNode_t*    right)
{
    if (tree == NULL)
    {
        PRINTERR("Tree is a nullptr");
        return NULL;
    }

    TreeNode_t* node = (TreeNode_t*) calloc(1, sizeof(TreeNode_t));

    if (node == NULL)
    {
        PRINTERR("Memory allocation for a new node failed");
        return NULL;
    }

    node->data  = data;
    node->left  = left;
    node->right = right;

    tree->size++;

    return node;
}

//------------------------------------------------------------------------------------------

TreeNode_t* TreeCopySubtree(Tree_t* dest_tree, TreeNode_t* node)
{
    DPRINTF("node = %p\n", node);

    assert(dest_tree != NULL);

    if (node == NULL)
    {
        return NULL;
    }

    return TreeNodeCtor(dest_tree, node->data,
                        TreeCopySubtree(dest_tree, node->left),
                        TreeCopySubtree(dest_tree, node->right));
}

//------------------------------------------------------------------------------------------

TreeErr_t TreeDtor(Tree_t* tree)
{
    assert(tree != NULL);

    TreeErr_t error = TREE_SUCCESS;

    if ((error = TreeNodeDtor(tree->dummy)))
    {
        return error;
    }

    if (tree->buffer)
    {
        free(tree->buffer);
        tree->buffer = NULL;
    }

    return TREE_SUCCESS;
}

//------------------------------------------------------------------------------------------

TreeErr_t TreeLeftSubtreeDtor(TreeNode_t* node)
{
    assert(node != NULL);

    return TreeSubtreeDtor(&node->left);
}

//------------------------------------------------------------------------------------------

TreeErr_t TreeRightSubtreeDtor(TreeNode_t* node)
{
    assert(node != NULL);

    return TreeSubtreeDtor(&node->right);
}

//------------------------------------------------------------------------------------------

TreeErr_t TreeSubtreeDtor(TreeNode_t** node_ptr)
{
    assert(node_ptr != NULL);

    if (*node_ptr == NULL)
    {
        return TREE_SUCCESS;
    }

    TreeErr_t error = TREE_SUCCESS;

    if ((error = TreeNodeDtor(*node_ptr)))
    {
        return error;
    }

    *node_ptr = NULL;

    return TREE_SUCCESS;
}

//------------------------------------------------------------------------------------------

TreeErr_t TreeNodeDtor(TreeNode_t* node)
{
    if (node == NULL)
        return TREE_NULL;

    TreeErr_t error = TREE_SUCCESS;

    if (node->left != NULL)
    {
        if ((error = TreeNodeDtor(node->left)))
            return error;
    }

    if (node->right != NULL)
    {
        if ((error = TreeNodeDtor(node->right)))
            return error;
    }

    if ((error = TreeSingleNodeDtor(node)))
        return error;

    return TREE_SUCCESS;
}

//------------------------------------------------------------------------------------------

TreeErr_t TreeSingleNodeDtor(TreeNode_t* node)
{
    if (node == NULL)
        return TREE_NULL;

    node->data.type = TYPE_NUM;
    node->data.value.num = 0;

    node->left   = NULL;
    node->right  = NULL;

    free(node);

    return TREE_SUCCESS;
}

//------------------------------------------------------------------------------------------

TreeErr_t TreeNodeVerify(const Tree_t* tree, TreeNode_t* node, size_t* calls_count)
{
    assert(calls_count != NULL);
    assert(tree        != NULL);

    if (*calls_count > tree->size)
    {
        return TREE_LOOP;
    }

    if (node == NULL)
    {
        return TREE_NULL;
    }

    (*calls_count)++;

    TreeErr_t error = TREE_SUCCESS;

    if (node->left != NULL)
    {
        if ((error = TreeNodeVerify(tree, node->left, calls_count)))
            return error;
    }

    if (node->right != NULL)
    {
        if ((error = TreeNodeVerify(tree, node->right, calls_count)))
            return error;
    }

    return TREE_SUCCESS;
}

//------------------------------------------------------------------------------------------

TreeErr_t TreeVerify(const Tree_t* tree)
{
    if (tree == NULL)
    {
        return TREE_NULL;
    }
    if (tree->dummy == NULL)
    {
        return TREE_NULL;
    }
    if (tree->size > TREE_MAX_SIZE)
    {
        return TREE_SIZE_EXCEEDS_MAX;
    }

    size_t calls_count = 0;
    TreeErr_t error = TREE_SUCCESS;

    if ((error = TreeNodeVerify(tree, tree->dummy, &calls_count)))
        return error;

    return TREE_SUCCESS;
}

//------------------------------------------------------------------------------------------

int CompareDoubles(double val1, double val2)
{
    if (val1 + DOUBLE_EPS < val2 - DOUBLE_EPS)
    {
        return -1;
    }
    else if (val2 + DOUBLE_EPS < val1 - DOUBLE_EPS)
    {
        return 1;
    }

    return 0;
}

//------------------------------------------------------------------------------------------
