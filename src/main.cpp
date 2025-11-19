#include "tree_commands.h"
#include "data_read.h"

//------------------------------------------------------------------------------------------

int main()
{
    Tree_t tree = {};

    if (TreeCtor(&tree))
        return EXIT_FAILURE;

    do {
        if (TreeReadData(&tree, "data/data.txt"))
            break;
    } while (0);

    if (TreeDtor(&tree))
        return EXIT_FAILURE;

    return EXIT_SUCCESS;
}

//------------------------------------------------------------------------------------------
