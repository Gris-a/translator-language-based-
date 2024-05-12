#include <assert.h>
#include <limits.h>
#include <math.h>

#include "../include/tree.h"

bool IsNumber(Node *node)
{
    assert(node);
    return (node->type == NodeType::NUM);
}

bool IsSpecialChar(Node *node)
{
    assert(node);
    return (node->type == NodeType::SP_CH);
}

bool IsWord(Node *node)
{
    assert(node);
    return (node->type == NodeType::WORD);
}

bool IsKeyword(Node *node)
{
    assert(node);
    return (node->type == NodeType::KWORD);
}

bool IsOperator(Node *node)
{
    assert(node);
    return (node->type == NodeType::OP);
}

bool IsFunction(Node *node)
{
    assert(node);
    return (node->type == NodeType::FUNC);
}

bool IsVariable(Node *node)
{
    assert(node);
    return (node->type == NodeType::VAR);
}






double GetNumber(Node *node)
{
    assert(node && IsNumber(node));
    return node->data.num;
}

SpecialChar GetSpecialChar(Node *node)
{
    assert(node && IsSpecialChar(node));
    return node->data.sp_ch;
}

char *GetWord(Node *node)
{
    assert(node && IsWord(node));
    return node->data.word;
}

Keyword GetKeyword(Node *node)
{
    assert(node && IsKeyword(node));
    return node->data.kword;
}

Operator GetOperator(Node *node)
{
    assert(node && IsOperator(node));
    return node->data.op;
}

char *GetFunction(Node *node)
{
    assert(node && IsFunction(node));
    return node->data.func;
}

size_t GetVariable(Node *node)
{
    assert(node && IsVariable(node));
    return node->data.var_id;
}
