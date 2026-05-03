#include "assembler.c"

int main(const int argc, const char *argv[])
{
    if (argc != 3)
    {
        ERR(ERR_MSG_ARGC, argc);
        return 1;
    }
    FILE *const srcf = fopen(argv[1], "r");
    if (!srcf)
    {
        ERR(ERR_MSG_FOPEN, argv[1]);
        return 1;
    }
    FILE *const dstf = fopen(argv[2], "w+");
    if (!dstf)
    {
        fclose(srcf);
        ERR(ERR_MSG_FOPEN, argv[2]);
        return 1;
    }
    Token *const token = tokenize(srcf);
    if (!token)
    {
        return 1;
    }
    if (puts("Tokenizer:") == EOF)
    {
        ERR(ERR_MSG_FWRITE, "stdout");
        return 1;
    }
    if (print_token(token))
    {
        return 1;
    }
    if (puts("Treeizer:") == EOF)
    {
        ERR(ERR_MSG_FWRITE, "stdout");
        return 1;
    }
    TreeNode *const tree = treeize(token);
    if (!tree)
    {
        return 1;
    }
    if (print_tree(tree, 0))
    {
        return 1;
    }
    if (puts("Assembler:") == EOF)
    {
        ERR(ERR_MSG_FWRITE, "stdout");
        return 1;
    }
    if (assemble(tree, dstf, argv[2]))
    {
        perror("fputs");
        perror("fprintf");
        return 1;
    }
    rewind(dstf);
    for (int c; (c = fgetc(dstf)) != EOF;)
    {
        if (putchar(c) == EOF)
        {
            ERR(ERR_MSG_FWRITE, "stdout");
        }
    }
    if (fclose(srcf) == EOF)
    {
        ERR(ERR_MSG_FCLOSE, argv[1]);
        return 1;
    }
    if (fclose(dstf) == EOF)
    {
        ERR(ERR_MSG_FCLOSE, argv[2]);
        return 1;
    }
    free_token(token);
    free_tree(tree);
    free(eoc);
    return 0;
}
