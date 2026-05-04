#include "treeizer.c"

FILE *dstf;
const char *dstn;
int tagidx;

/*
    Returns true as error occurs or false.
*/
bool data(const TreeNode *const tree, const char *const fn)
{
    if (tree->type == tnGlobal)
    {
        return tree->child[0]->type != tnEOC
           && ((tagidx = 0) || data(tree->child[1], tree->child[0]->value)
            || data(tree->child[2], nullptr));
    }
    switch (tree->child[0]->type)
    {
        case tnBlock:
        {
            return data(tree->child[0], fn)
                || data(tree->child[1], fn);
            }
        case tnEOC:
        {
            return false;
        }
        case tnGlobal:
        {
            /*
                Impossible.
            */
        }
        case tnIdentity:
        {
            if (print(dstf, "_$%s%d:\n    .asciz \"%s\\n\"\n_$$%s%d:\n", fn, tagidx, tree->child[0]->value, fn, tagidx))
            {
                return true;
            }
            ++tagidx;
            return data(tree->child[1], fn);
        }
    }
    return true;
}

/*
    Returns true as error occurs or false.
*/
bool code(const TreeNode *const tree, const char *const fn)
{
    switch (tree->child[0]->type)
    {
        case tnBlock:
        {
            return tree->child[0]->type != tnEOC && tree->child[1]->type == tnIdentity
                 ? print(dstf, "    call _%s\n", tree->child[1]->value)
                || code(tree->child[0], fn)
                 : code(tree->child[0], fn)
                || code(tree->child[1], fn);
        }
        case tnEOC:
        {
            return false;
        }
        case tnGlobal:
        {
            /*
                Impossible.
            */
        }
        case tnIdentity:
        {
            if (print(dstf, "    movq $1, %%rax\n    movq $(_$$%s%d - _$%s%d), %%rdx\n    leaq _$%s%d(%%rip), %%rsi\n    syscall\n", fn, tagidx, fn, tagidx, fn, tagidx))
            {
                return true;
            }
            ++tagidx;
            return code(tree->child[1], fn);
        }
    }
    return true;
}

/*
    Returns true as error occurs or false.
*/
bool func(const TreeNode *const tree)
{
    if (tree->child[0]->type == tnEOC)
    {
        return false;
    }
    if (print(dstf, ".type _%s, @function\n_%s:\n    pushq %%rbp\n    movq %%rsp, %%rbp\n    movq $1, %%rdi\n", tree->child[0]->value, tree->child[0]->value)
     || ((tagidx = 0) || code(tree->child[1], tree->child[0]->value))
     || print(dstf, "    popq %%rbp\n    ret\n"))
    {
        return true;
    }
    return func(tree->child[2]);
}

/*
    Returns true as error occurs or false.
*/
bool assemble(const TreeNode *const tree, FILE *const df, const char *const dn)
{
    dstf = df;
    dstn = dn;
    return print(dstf, ".section .data\n")
        || data(tree, nullptr)
        || print(dstf, ".section .text\n")
        || ((tagidx = 0) || func(tree))
        || print(dstf, ".globl main\nmain:\n    pushq %%rbp\n    movq %%rsp, %%rbp\n    call _main\n    movl $0, %%eax\n    popq %%rbp\n    ret\n.section .note.GNU-stack,\"\",@progbits\n");
}
