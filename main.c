#include "assembler.c"

int main(const int argc, const char *argv[])
{
    if (argc != 3)
    {
        err(ERR_MSG_ARGC, argc);
        return 1;
    }
    FILE *const srcf = fopen(argv[1], "r");
    if (!srcf)
    {
        err(ERR_MSG_FOPEN, argv[1]);
        return 1;
    }
    FILE *const dstf = fopen(argv[2], "w+");
    if (!dstf)
    {
        fclose(srcf);
        err(ERR_MSG_FOPEN, argv[2]);
        return 1;
    }
    print(stdout, "Tokenizer:\n");
    Token *const token = tokenize(srcf);
    if (!token)
    {
        return 1;
    }
    if (print_token(token))
    {
        return 1;
    }
    print(stdout, "Treeizer:\n");
    TreeNode *const tree = treeize(token);
    if (!tree)
    {
        return 1;
    }
    if (print_tree(tree, 0))
    {
        return 1;
    }
    print(stdout, "Assembler:\n");
    if (assemble(tree, dstf, argv[2]))
    {
        return 1;
    }
    rewind(dstf);
    for (int c; (c = fgetc(dstf)) != EOF;)
    {
        print(stdout, "%c", c);
    }
    if (fclose(srcf) == EOF)
    {
        err(ERR_MSG_FCLOSE, argv[1]);
        return 1;
    }
    if (fclose(dstf) == EOF)
    {
        err(ERR_MSG_FCLOSE, argv[2]);
        return 1;
    }
    free_token(token);
    free_tree(tree);
    free(eoc);
    return 0;
}
