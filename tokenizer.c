#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "errors.h"

typedef enum _TokenType
{
    tBlockLeft,
    tBlockRight,
    tEOT,
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
        case tIdentity:
        {
            return "Identity";
        }
    }
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
        if (printf("%s", tokentype_to_str(token->type)) == EOF)
        {
            ERR(ERR_MSG_FWRITE, "stdout");
            return true;
        }
        if (token->value)
        {
            if (printf("(\"%s\")", token->value) == EOF)
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
    int tsize = 0;
    int tcap = 1;
    Token *token = malloc(tcap * sizeof(Token));
    if (!token)
    {
        ERR(ERR_MSG_ALLOC, tcap);
        return nullptr;
    }
    int ssize = 0;
    int scap = 2;
    char *str = malloc(scap * sizeof(char));
    if (!str)
    {
        ERR(ERR_MSG_ALLOC, scap);
        return nullptr;
    }
    for (int c; (c = fgetc(srcf)) != EOF;)
    {
        if (c <= ' ')
        {
            if (state)
            {
                if (tsize == tcap)
                {
                    tcap <<= 1;
                    token = realloc(token, tcap * sizeof(Token));
                    if (!token)
                    {
                        ERR(ERR_MSG_ALLOC, tcap);
                        return nullptr;
                    }
                }
                token[tsize].type = tIdentity;
                token[tsize].value = str;
                ++tsize;
                ssize = 0;
                scap = 1;
                str = malloc(scap * sizeof(char));
            }
            state = false;
            continue;
        }
        switch (c)
        {
            case '(':
            case ')':
            {
                if (state)
                {
                    if (tsize == tcap)
                    {
                        tcap <<= 1;
                        token = realloc(token, tcap * sizeof(Token));
                        if (!token)
                        {
                            ERR(ERR_MSG_ALLOC, tcap);
                            return nullptr;
                        }
                    }
                    token[tsize].type = tIdentity;
                    token[tsize].value = str;
                    ++tsize;
                    ssize = 0;
                    scap = 2;
                    str = malloc(scap * sizeof(char));
                    str[0] = '\0';
                }
                if (tsize == tcap)
                {
                    tcap <<= 1;
                    token = realloc(token, tcap * sizeof(Token));
                    if (!token)
                    {
                        ERR(ERR_MSG_ALLOC, tcap);
                        return nullptr;
                    }
                }
                token[tsize].type = c == '(' ? tBlockLeft : tBlockRight;
                token[tsize].value = nullptr;
                ++tsize;
                state = false;
                break;
            }
            default:
            {
                if (state)
                {
                    // Note '\0'
                    if (ssize + 1 == scap)
                    {
                        scap <<= 1;
                        str = realloc(str, scap * sizeof(char));
                        if (!str)
                        {
                            ERR(ERR_MSG_ALLOC, scap);
                            return nullptr;
                        }
                    }
                    str[ssize] = (char)(c);
                    str[ssize + 1] = '\0';
                    ++ssize;
                }
                else
                {
                    ssize = 1;
                    str[0] = (char)(c);
                    str[1] = '\0';
                    state = true;
                }
            }
        }
    }
    if (state)
    {
        if (tsize == tcap)
        {
            tcap <<= 1;
            token = realloc(token, tcap * sizeof(Token));
            if (!token)
            {
                ERR(ERR_MSG_ALLOC, tcap);
                return nullptr;
            }
        }
        token[tsize].type = tIdentity;
        token[tsize].value = str;
        ++tsize;
    }
    if (tsize == tcap)
    {
        ++tcap;
        token = realloc(token, tcap * sizeof(Token));
        if (!token)
        {
            ERR(ERR_MSG_ALLOC, tcap);
            return nullptr;
        }
    }
    token[tsize].type = tEOT;
    token[tsize].value = nullptr;
    return token;
}
