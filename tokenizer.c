#include "errors.h"

typedef enum _TokenType
{
    tBlockLeft,
    tBlockRight,
    tEOT,
    tFuncCall,
    tIdentity,
} TokenType;

char *tokentype_to_str(const TokenType tt)
{
    switch (tt)
    {
        case tBlockLeft:
        {
            return "BlockLeft";
        }
        case tBlockRight:
        {
            return "BlockRight";
        }
        case tEOT:
        {
            return "EOT";
        }
        case tFuncCall:
        {
            return "FuncCall";
        }
        case tIdentity:
        {
            return "Identity";
        }
    }
    return nullptr;
}

/*
    If no extra value, value is nullptr.
*/
typedef struct _Token
{
    TokenType type;
    char *value;
} Token;

/*
    Returns true as error occurs or false.
*/
bool print_token(const Token *token)
{
    for (; token->type != tEOT; ++token)
    {
        if (print(stdout, "%s", tokentype_to_str(token->type)))
        {
            return true;
        }
        if (token->value)
        {
            if (print(stdout, "(\"%s\")", token->value))
            {
                return true;
            }
        }
        if (print(stdout, "\n"))
        {
            return true;
        }
    }
    return false;
}

void free_token(Token *token)
{
    for (int i = 0; token[i].type != tEOT; ++i)
    {
        if (token[i].value)
        {
            free(token[i].value);
        }
    }
    free(token);
}

/*
    Requires non-nullptr srcf.
    The callee must free the return value.
*/
Token *tokenize(FILE *const srcf)
{
    /*
        s is the state in the state machine.
        s = false   =>  default state
        s = true    =>  reading identity
    */
    bool state = false;
    size_t tsize = 0;
    size_t tcap = 1;
    Token *token = alloc(tcap * sizeof(Token));
    if (!token)
    {
        return nullptr;
    }
    size_t ssize = 1;
    size_t scap = 4;
    char *str = alloc(scap * sizeof(char));
    if (!str)
    {
        return nullptr;
    }
    for (int c; (c = fgetc(srcf)) != EOF;)
    {
        if (c <= ' ')
        {
            if (state)
            {
                reserve((void **)(&token), tsize, &tcap, sizeof(Token), true);
                token[tsize].type = tIdentity;
                token[tsize].value = str;
                ++tsize;
                ssize = 1;
                scap = 1;
                str = alloc(scap * sizeof(char));
                if (!str)
                {
                    return nullptr;
                }
                str[0] = '\0';
            }
            state = false;
            continue;
        }
        switch (c)
        {
            case '(':
            case ')':
            case '!':
            {
                if (state)
                {
                    reserve((void **)(&token), tsize, &tcap, sizeof(Token), true);
                    token[tsize].type = tIdentity;
                    token[tsize].value = str;
                    ++tsize;
                    ssize = 1;
                    scap = 2;
                    str = alloc(scap * sizeof(char));
                    if (!str)
                    {
                        return nullptr;
                    }
                    str[0] = '\0';
                }
                reserve((void **)(&token), tsize, &tcap, sizeof(Token), true);
                switch (c)
                {
                    case '(':
                    {
                        token[tsize].type = tBlockLeft;
                        break;
                    }
                    case ')':
                    {
                        token[tsize].type = tBlockRight;
                        break;
                    }
                    case '!':
                    {
                        token[tsize].type = tFuncCall;
                        break;
                    }
                }
                token[tsize].value = nullptr;
                ++tsize;
                state = false;
                break;
            }
            default:
            {
                if (state)
                {
                    reserve((void **)(&str), ssize, &scap, sizeof(char), true);
                    str[ssize - 1] = (char)(c);
                    str[ssize] = '\0';
                    ++ssize;
                }
                else
                {
                    ssize = 2;
                    str[0] = (char)(c);
                    str[1] = '\0';
                    state = true;
                }
            }
        }
    }
    if (state)
    {
        reserve((void **)(&token), tsize, &tcap, sizeof(Token), true);
        token[tsize].type = tIdentity;
        token[tsize].value = str;
        ++tsize;
    }
    reserve((void **)(&token), tsize, &tcap, sizeof(Token), false);
    token[tsize].type = tEOT;
    token[tsize].value = nullptr;
    return token;
}
