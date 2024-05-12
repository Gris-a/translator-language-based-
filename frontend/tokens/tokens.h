#ifndef TOKENS_H
#define TOKENS_H

#include <assert.h>
#include <stdlib.h>

#include "../../general/tree/include/tree.h"

struct Token
{
    Node *lexeme;

    size_t line;
    size_t pos;

    Token *next;
    Token *prev;
};


Token *TokenListCtr(void);
Token *TokenListDtr(Token *token_list);
Token *TokenListClear(Token *token_list);

Token *AddToken(Token *token_list, node_t data, NodeType type, size_t line, size_t l_pos);
Token *InsertToken(Token *token_prev, node_t data, NodeType type, size_t line, size_t l_pos);

#endif //TOKENS_H