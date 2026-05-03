#include "treeizer.c"

FILE *dstf;
const char *dstn;
int tagidx;

/*
    Returns true as error occurs or false.
    Guarantees tree->type is Block.
*/
bool data(const TreeNode *const tree)
{
    switch (tree->child[0]->type)
    {
        case tnBlock:
        {
            return data(tree->child[0])
                || data(tree->child[1]);
        }
        case tnIdentity:
        {
            if (fprintf(dstf, "s%d:\n", tagidx) == EOF
             || fprintf(dstf, "    .asciz \"%s\\n\"\n", tree->child[0]->value) == EOF
             || fprintf(dstf, "e%d:\n", tagidx) == EOF)
            {
                ERR(ERR_MSG_FWRITE, dstn);
                return true;
            }
            ++tagidx;
            return data(tree->child[1]);
        }
        case tnEOC:
        {
            return false;
        }
    }
}

/*
    Returns true as error occurs or false.
*/
bool code(const TreeNode *const tree)
{
    switch (tree->child[0]->type)
    {
        case tnBlock:
        {
            return code(tree->child[0])
                || code(tree->child[1]);
        }
        case tnIdentity:
        {
            if (fputs("    movq $1, %rax\n", dstf) == EOF
             || fprintf(dstf, "    movq $(e%d - s%d), %%rdx\n", tagidx, tagidx) == EOF
             || fprintf(dstf, "    leaq s%d(%%rip), %%rsi\n", tagidx) == EOF
             || fputs("    syscall\n", dstf) == EOF)
            {
                ERR(ERR_MSG_FWRITE, dstn);
                return true;
            }
            ++tagidx;
            return code(tree->child[1]);
        }
        case tnEOC:
        {
            return false;
        }
    }
}

/*
    Returns true as error occurs or false.
*/
bool assemble(const TreeNode *const tree, FILE *const df, const char *const dn)
{
    dstf = df;
    dstn = dn;
    if (fputs(".section .data\n", dstf) == EOF
     || data(tree)
     || fputs(".section .text\n", dstf) == EOF
     || fputs(".globl main\n", dstf) == EOF
     || fputs("main:\n", dstf) == EOF)
    {
        ERR(ERR_MSG_FWRITE, dstn);
        return true;
    }
    if (tagidx)
    {
        tagidx = 0;
        if (fputs("    movq $1, %rdi\n", dstf) == EOF)
        {
            ERR(ERR_MSG_FWRITE, dstn);
            return true;
        }
    }
    if (code(tree)
     || fputs("    movq $60, %rax\n", dstf) == EOF
     || fputs("    xorq %rdi, %rdi\n", dstf) == EOF
     || fputs("    syscall\n", dstf) == EOF
     || fputs(".section .note.GNU-stack,\"\",@progbits\n", dstf) == EOF)
    {
        ERR(ERR_MSG_FWRITE, dstn);
        return true;
    }
    return false;
}
