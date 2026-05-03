#include "tokenizer.c"

typedef enum _TreeNodeType
{
    tnBlock,
    tnEOC,
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
        case tnIdentity:
        {
            return "Identity";
        }
    }
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
        if (putchar(' ') == EOF)
        {
            ERR(ERR_MSG_FWRITE, "stdout");
            return true;
        }
    }
    if (printf("%s", treenodetype_to_str(tree->type)) == EOF)
    {
        ERR(ERR_MSG_FWRITE, "stdout");
        return true;
    }
    if (tree->value)
    {
        if (printf("(\"%s\")", tree->value) == EOF)
        {
            ERR(ERR_MSG_FWRITE, "stdout");
            return true;
        }
    }
    if (putchar('\n') == EOF)
    {
        ERR(ERR_MSG_FWRITE, "stdout");
        return true;
    }
    for (int i = depth; i--;)
    {
        if (putchar(' ') == EOF)
        {
            ERR(ERR_MSG_FWRITE, "stdout");
            return true;
        }
    }
    if (puts("(") == EOF)
    {
        ERR(ERR_MSG_FWRITE, "stdout");
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
        if (putchar(' ') == EOF)
        {
            ERR(ERR_MSG_FWRITE, "stdout");
            return true;
        }
    }
    if (puts(")") == EOF)
    {
        ERR(ERR_MSG_FWRITE, "stdout");
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
        Block => ( Block ) Block | Identity Block | nul
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
                ERR(ERR_MSG_SYNTAX, "BlockRight", tokentype_to_str(token->type));
                return nullptr;
            }
            ++token;
            TreeNode *const t1 = block();
            if (!t1)
            {
                return nullptr;
            }
            TreeNode *const t2 = malloc(sizeof(TreeNode));
            if (!t2)
            {
                ERR(ERR_MSG_ALLOC, (int)(sizeof(TreeNode)));
                return nullptr;
            }
            t2->type = tnBlock;
            t2->value = nullptr;
            TreeNode **const t3 = malloc(3 * sizeof(TreeNode *));
            if (!t3)
            {
                ERR(ERR_MSG_ALLOC, (int)(3 * sizeof(TreeNode *)));
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
            TreeNode *const t0 = malloc(sizeof(TreeNode));
            if (!t0)
            {
                ERR(ERR_MSG_ALLOC, (int)(sizeof(TreeNode)));
                return nullptr;
            }
            t0->type = tnBlock;
            t0->value = nullptr;
            TreeNode **const t1 = malloc(1 * sizeof(TreeNode *));
            if (!t1)
            {
                ERR(ERR_MSG_ALLOC, (int)(1 * sizeof(TreeNode *)));
                return nullptr;
            }
            t1[0] = eoc;
            t0->child = t1;
            return t0;
        }
        case tIdentity:
        {
            TreeNode *const t0 = malloc(sizeof(TreeNode));
            if (!t0)
            {
                ERR(ERR_MSG_ALLOC, (int)(sizeof(TreeNode)));
                return nullptr;
            }
            t0->type = tnIdentity;
            t0->value = strdup(token->value);
            TreeNode **const t1 = malloc(1 * sizeof(TreeNode *));
            if (!t1)
            {
                ERR(ERR_MSG_ALLOC, (int)(1 * sizeof(TreeNode *)));
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
            TreeNode *const t3 = malloc(sizeof(TreeNode));
            if (!t3)
            {
                ERR(ERR_MSG_ALLOC, (int)(sizeof(TreeNode)));
                return nullptr;
            }
            t3->type = tnBlock;
            t3->value = nullptr;
            TreeNode **const t4 = malloc(3 * sizeof(TreeNode *));
            if (!t4)
            {
                ERR(ERR_MSG_ALLOC, (int)(3 * sizeof(TreeNode *)));
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
            ERR(ERR_MSG_SYNTAX, "BlockLeft or Identity", tokentype_to_str(token->type));
            return nullptr;
        }
    }
}

TreeNode *treeize(const Token *t)
{
    /*
        Global => ( Block )
    */
    token = t;
    if (token->type != tBlockLeft)
    {
        ERR(ERR_MSG_SYNTAX, "BlockLeft", tokentype_to_str(token->type));
        return nullptr;
    }
    ++token;
    eoc = malloc(sizeof(TreeNode));
    if (!eoc)
    {
        ERR(ERR_MSG_ALLOC, (int)(sizeof(TreeNode)));
        return nullptr;
    }
    eoc->type = tnEOC;
    eoc->value = nullptr;
    eoc->child = nullptr;
    TreeNode *const t0 = block();
    if (!t0)
    {
        return nullptr;
    }
    if (token->type != tBlockRight)
    {
        ERR(ERR_MSG_SYNTAX, "BlockRight", tokentype_to_str(token->type));
        return nullptr;
    }
    return t0;
}
