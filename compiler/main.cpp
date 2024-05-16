#include <stdlib.h>
#include <sys/mman.h>

#include "../frontend/frontend.h"
#include "../backend/translator/translator.h"

int main(int argc, const char *argv[])
{
    if(argc != 4)
    {
        printf("Error: Input expected: cp <input_file> <output_type> <output_name>\n"
               "<output_type>:\n"
               "\t-T\t\ttree     format\n"
               "\t-B\t\tbinary   format\n"
               "\t-S\t\tassembly format\n");
        return EXIT_FAILURE;
    }

    Tree tree = ParseCode(argv[1]);
    if(tree.root == NULL)
    {
        return EXIT_FAILURE;
    }

    if(strcmp(argv[2], "-T") == 0)
    {
        TreeDot(&tree, "tree.png");
        SaveTree(&tree, argv[3]);
    }
    else if(strcmp(argv[2], "-B") == 0)
    {
        const size_t prog_sz_max = 1 << 16;
        char *arr = (char *)calloc(prog_sz_max, sizeof(char));
        assert(arr);

        memset(arr, 0xC3, prog_sz_max);
        TranslatorBin(tree, arr, argv[3]);

        free(arr);
    }
    else if(strcmp(argv[2], "-S") == 0)
    {
        TranslatorAsm(tree, argv[3]);
    }
    else
    {
        printf("Error: Unsupported format.\n\n"
               "available formats:\n"
               "\t-T\t\ttree     format\n"
               "\t-B\t\tbinary   format\n"
               "\t-S\t\tassembly format\n");
        return EXIT_FAILURE;
    }

    TreeDtor(&tree);

    return EXIT_SUCCESS;
}