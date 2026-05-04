#include <string.h>
#include "tokenizer.c"

typedef enum _TreeNodeType
{
    tnBlock,
    tnEOC,
    tnGlobal,
    tnIdentity,
} TreeNodeType;

char *treenodetype_to_str(const TreeNodeType tt)
{
    switch (tt)
    {
        case tnBlock:
        {
            return "Block";
        }
        case tnEOC:
        {
            return "EOC";
        }
        case tnGlobal:
        {
            return "Global";
        }
        case tnIdentity:
        {
            return "Identity";
        }
    }
    return nullptr;
}

/*
    If no extra value, value is nullptr.
*/
typedef struct _TreeNode
{
    TreeNodeType type;
    char *value;
    struct _TreeNode **child;
} TreeNode;

/*
    Returns true as error occurs or false.
*/
bool print_tree(const TreeNode *const tree, const int depth)
{
    for (int i = depth; i--;)
    {
        if (print(stdout, " "))
        {
            return true;
        }
    }
    if (print(stdout, "%s", treenodetype_to_str(tree->type)))
    {
        return true;
    }
    if (tree->value)
    {
        if (print(stdout, "(\"%s\")", tree->value))
        {
            return true;
        }
    }
    if (print(stdout, "\n"))
    {
        return true;
    }
    for (int i = depth; i--;)
    {
        if (print(stdout, " "))
        {
            return true;
        }
    }
    if (print(stdout, "(\n"))
    {
        return true;
    }
    for (int i = 0; tree->child[i]->type != tnEOC; ++i)
    {
        if (print_tree(tree->child[i], depth + 2))
        {
            return true;
        }
    }
    for (int i = depth; i--;)
    {
        if (print(stdout, " "))
        {
            return true;
        }
    }
    if (print(stdout, ")\n"))
    {
        return true;
    }
    return false;
}

void free_tree(TreeNode *const tree)
{
    if (tree->value)
    {
        free(tree->value);
    }
    for (; (*tree->child)->type != tnEOC; ++tree->child)
    {
        free_tree(*tree->child);
    }
    free(tree);
}

const Token *token;
TreeNode *eoc;

static TreeNode *block(void)
{
    /*
        Block => ( Block ) Block | ! Identity Block | Identity Block | nul
    */
    switch (token->type)
    {
        case tBlockLeft:
        {
            ++token;
            TreeNode *const t0 = block();
            if (!t0)
            {
                return nullptr;
            }
            if (token->type != tBlockRight)
            {
                err(ERR_MSG_SYNTAX, "BlockRight", tokentype_to_str(token->type));
                return nullptr;
            }
            ++token;
            TreeNode *const t1 = block();
            if (!t1)
            {
                return nullptr;
            }
            TreeNode *const t2 = alloc(sizeof(TreeNode));
            if (!t2)
            {
                return nullptr;
            }
            t2->type = tnBlock;
            t2->value = nullptr;
            TreeNode **const t3 = alloc(3 * sizeof(TreeNode *));
            if (!t3)
            {
                return nullptr;
            }
            t3[0] = t0;
            t3[1] = t1;
            t3[2] = eoc;
            t2->child = t3;
            return t2;
        }
        case tBlockRight:
        {
            TreeNode *const t0 = alloc(sizeof(TreeNode));
            if (!t0)
            {
                return nullptr;
            }
            t0->type = tnBlock;
            t0->value = nullptr;
            TreeNode **const t1 = alloc(1 * sizeof(TreeNode *));
            if (!t1)
            {
                return nullptr;
            }
            t1[0] = eoc;
            t0->child = t1;
            return t0;
        }
        case tFuncCall:
        {
            ++token;
            if (token->type != tIdentity)
            {
                err(ERR_MSG_SYNTAX, "Identity", tokentype_to_str(token->type));
                return nullptr;
            }
            TreeNode *const t0 = alloc(sizeof(TreeNode));
            if (!t0)
            {
                return nullptr;
            }
            t0->type = tnIdentity;
            t0->value = strdup(token->value);
            TreeNode **const t1 = alloc(1 * sizeof(TreeNode *));
            if (!t1)
            {
                return nullptr;
            }
            t1[0] = eoc;
            t0->child = t1;
            ++token;
            TreeNode *const t2 = block();
            if (!t2)
            {
                return nullptr;
            }
            TreeNode *const t3 = alloc(sizeof(TreeNode));
            if (!t3)
            {
                return nullptr;
            }
            t3->type = tnBlock;
            t3->value = nullptr;
            TreeNode **const t4 = alloc(3 * sizeof(TreeNode *));
            if (!t3)
            {
                return nullptr;
            }
            t4[0] = t2;
            t4[1] = t0;
            t4[2] = eoc;
            t3->child = t4;
            return t3;
        }
        case tIdentity:
        {
            TreeNode *const t0 = alloc(sizeof(TreeNode));
            if (!t0)
            {
                return nullptr;
            }
            t0->type = tnIdentity;
            t0->value = strdup(token->value);
            TreeNode **const t1 = alloc(1 * sizeof(TreeNode *));
            if (!t1)
            {
                return nullptr;
            }
            t1[0] = eoc;
            t0->child = t1;
            ++token;
            TreeNode *const t2 = block();
            if (!t2)
            {
                return nullptr;
            }
            TreeNode *const t3 = alloc(sizeof(TreeNode));
            if (!t3)
            {
                return nullptr;
            }
            t3->type = tnBlock;
            t3->value = nullptr;
            TreeNode **const t4 = alloc(3 * sizeof(TreeNode *));
            if (!t4)
            {
                return nullptr;
            }
            t4[0] = t0;
            t4[1] = t2;
            t4[2] = eoc;
            t3->child = t4;
            return t3;
        }
        default:
        {
            err(ERR_MSG_SYNTAX, "BlockLeft or Identity", tokentype_to_str(token->type));
            return nullptr;
        }
    }
}

static TreeNode *global()
{
    /*
        Global => Identity ( Block ) Global | nul
    */
    switch (token->type)
    {
        case tEOT:
        {
            TreeNode *const t0 = alloc(sizeof(TreeNode));
            if (!t0)
            {
                return nullptr;
            }
            t0->type = tnGlobal;
            t0->value = nullptr;
            TreeNode **const t1 = alloc(1 * sizeof(TreeNode *));
            if (!t1)
            {
                return nullptr;
            }
            t1[0] = eoc;
            t0->child = t1;
            return t0;
        }
        case tIdentity:
        {
            TreeNode *const t0 = alloc(sizeof(TreeNode));
            if (!t0)
            {
                return nullptr;
            }
            t0->type = tnIdentity;
            t0->value = strdup(token->value);
            TreeNode **const t1 = alloc(1 * sizeof(TreeNode *));
            if (!t1)
            {
                return nullptr;
            }
            t1[0] = eoc;
            t0->child = t1;
            ++token;
            if (token->type != tBlockLeft)
            {
                err(ERR_MSG_SYNTAX, "BlockLeft", tokentype_to_str(token->type));
                return nullptr;
            }
            ++token;
            TreeNode *const t2 = block();
            if (!t2)
            {
                return nullptr;
            }
            if (token->type != tBlockRight)
            {
                err(ERR_MSG_SYNTAX, "BlockRight", tokentype_to_str(token->type));
                return nullptr;
            }
            ++token;
            TreeNode *const t3 = global();
            if (!t3)
            {
                return nullptr;
            }
            TreeNode *const t4 = alloc(sizeof(TreeNode));
            if (!t4)
            {
                return nullptr;
            }
            t4->type = tnGlobal;
            t4->value = nullptr;
            TreeNode **const t5 = alloc(4 * sizeof(TreeNode *));
            if (!t5)
            {
                return nullptr;
            }
            t5[0] = t0;
            t5[1] = t2;
            t5[2] = t3;
            t5[3] = eoc;
            t4->child = t5;
            return t4;
        }
        default:
        {
            err(ERR_MSG_SYNTAX, "EOT or Identity", tokentype_to_str(token->type));
            return nullptr;
        }
    }
}

TreeNode *treeize(const Token *t)
{
    token = t;
    eoc = alloc(sizeof(TreeNode));
    if (!eoc)
    {
        return nullptr;
    }
    eoc->type = tnEOC;
    eoc->value = nullptr;
    eoc->child = nullptr;
    return global();
}
