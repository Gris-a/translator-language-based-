#include "tokens.h"

struct Token *TokenListCtr(void)
{
    struct Token *token = (struct Token *)malloc(sizeof(struct Token));
    assert(token);

    *token = (struct Token){
        .lexeme = NodeCtor({.sp_ch = SpecialChar::END}, NodeType::SP_CH),
        .line   = 0,
        .pos    = 0,
        .next   = token,
        .prev   = token
    };

    return token;
}

struct Token *TokenListDtr(struct Token *token_list)
{
    struct Token *head = token_list;
    for(struct Token *curr = head->next, *next = curr->next; curr != head; curr = next, next = curr->next)
    {
        NodeDtor(curr->lexeme);
        free(curr);
    }

    NodeDtor(head->lexeme);
    free(head);

    return NULL;
}

struct Token *TokenListClear(struct Token *token_list)
{
    struct Token *head = token_list;
    for(struct Token *curr = head->next, *next = curr->next; curr != head; curr = next, next = curr->next)
    {
        if(IsSpecialChar(curr->lexeme) && !(GetSpecialChar(curr->lexeme) == SpecialChar::SEMICOLON) &&
                                          !(GetSpecialChar(curr->lexeme) == SpecialChar::COMMA))
        {
            NodeDtor(curr->lexeme);
        }
        free(curr);
    }

    NodeDtor(head->lexeme);
    free(head);

    return NULL;
}


struct Token *AddToken(struct Token *token_list, node_t data, NodeType type, size_t line, size_t l_pos)
{
    struct Token *head = token_list;
    return InsertToken(head->prev, data, type, line, l_pos);
}

struct Token *InsertToken(struct Token *token_prev, node_t data, NodeType type, size_t line, size_t l_pos)
{
    struct Token *token = (struct Token *)malloc(sizeof(struct Token));
    assert(token);

    *token = (struct Token){
        .lexeme = NodeCtor(data, type),
        .line   = line,
        .pos    = l_pos
    };

    token->next = token_prev->next;
    token_prev->next->prev = token;

    token->prev = token_prev;
    token_prev->next = token;

    return token;
}