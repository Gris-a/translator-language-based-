#include "translator.h"


struct JMPInf
{
    size_t pos;
    char *label;
};



static void TranslateCompare(Node *comp, char *text_section, size_t *offset, struct QuadHashTable *table, struct Stack *stack, size_t n_func_params, int comp_t);
static void TranslateAbove(Node *above, char *text_section, size_t *offset, struct QuadHashTable *table, struct Stack *stack, size_t n_func_params);
static void TranslateAboveEq(Node *abeq, char *text_section, size_t *offset, struct QuadHashTable *table, struct Stack *stack, size_t n_func_params);
static void TranslateLess(Node *less, char *text_section, size_t *offset, struct QuadHashTable *table, struct Stack *stack, size_t n_func_params);
static void TranslateLessEq(Node *leeq, char *text_section, size_t *offset, struct QuadHashTable *table, struct Stack *stack, size_t n_func_params);
static void TranslateEq(Node *eq, char *text_section, size_t *offset, struct QuadHashTable *table, struct Stack *stack, size_t n_func_params);
static void TranslateNotEq(Node *neq, char *text_section, size_t *offset, struct QuadHashTable *table, struct Stack *stack, size_t n_func_params);

static void TranslateAnd(Node *_and, char *text_section, size_t *offset, struct QuadHashTable *table, struct Stack *stack, size_t n_func_params);
static void TranslateOr(Node *_or, char *text_section, size_t *offset, struct QuadHashTable *table, struct Stack *stack, size_t n_func_params);

static void TranslateBinaryOp(Node *op, char *text_section, size_t *offset, struct QuadHashTable *table, struct Stack *stack, size_t n_func_params, char op_code);
static void TranslateAdd(Node *add, char *text_section, size_t *offset, struct QuadHashTable *table, struct Stack *stack, size_t n_func_params);
static void TranslateSub(Node *sub, char *text_section, size_t *offset, struct QuadHashTable *table, struct Stack *stack, size_t n_func_params);
static void TranslateMul(Node *mul, char *text_section, size_t *offset, struct QuadHashTable *table, struct Stack *stack, size_t n_func_params);
static void TranslateDiv(Node *div, char *text_section, size_t *offset, struct QuadHashTable *table, struct Stack *stack, size_t n_func_params);

static void TranslateNum(Node *num_node, char *text_section, size_t *offset);
static void TranslateVariable(Node *var, char *text_section, size_t *offset, size_t n_func_params);
static void TranslateExpression(Node *expr, char *text_section, size_t *offset, struct QuadHashTable *table, struct Stack *stack, size_t n_func_params);

static void TranslateAssignment(Node *assig, char *text_section, size_t *offset, struct QuadHashTable *table, struct Stack *stack, size_t n_func_params);
static void TranslateFuncCall(Node *func, char *text_section, size_t *offset, struct QuadHashTable *table, struct Stack *stack, size_t n_func_params);
static void TranslateRet(Node *_return, char *text_section, size_t *offset, struct QuadHashTable *table, struct Stack *stack, size_t n_func_params);

static void TranslateIf(Node *_if, char *text_section, size_t *offset, struct QuadHashTable *table, struct Stack *stack, size_t n_func_params);
static void TranslateWhile(Node *_while, char *text_section, size_t *offset, struct QuadHashTable *table, struct Stack *stack, size_t n_func_params);
static void TranslateBody(Node *body, char *text_section, size_t *offset, struct QuadHashTable *table, struct Stack *stack, size_t n_func_params);

static long GetMaxVarId(Node *func);
static long GetNFuncParams(Node *func);
static void TranslateFunction(Node *func, char *text_section, size_t *offset, struct QuadHashTable *table, struct Stack *stack);


static void TranslateCompare(Node *comp, char *text_section, size_t *offset, struct QuadHashTable *table, struct Stack *stack, size_t n_func_params, int comp_t)
{
    TranslateExpression(comp->right, text_section, offset, table, stack, n_func_params);
    TranslateExpression(comp->left , text_section, offset, table, stack, n_func_params);

    text_section[(*offset)++] = (char)0xF2; // movsd xmm0, [rsp]
    text_section[(*offset)++] = (char)0x0F;
    text_section[(*offset)++] = (char)0x10;
    text_section[(*offset)++] = (char)0x04;
    text_section[(*offset)++] = (char)0x24;

    text_section[(*offset)++] = (char)0x48; // add rsp, 8
    text_section[(*offset)++] = (char)0x83;
    text_section[(*offset)++] = (char)0xC4;
    text_section[(*offset)++] = (char)0x08;

    text_section[(*offset)++] = (char)0xF2; // movsd xmm1, [rsp]
    text_section[(*offset)++] = (char)0x0F;
    text_section[(*offset)++] = (char)0x10;
    text_section[(*offset)++] = (char)0x0C;
    text_section[(*offset)++] = (char)0x24;

    text_section[(*offset)++] = (char)0xF2; // cmpsd xmm0, xmm1, comp_t
    text_section[(*offset)++] = (char)0x0F;
    text_section[(*offset)++] = (char)0xC2;
    text_section[(*offset)++] = (char)0xC1;
    text_section[(*offset)++] = (char)comp_t;

    text_section[(*offset)++] = (char)0x48; // mov rax, 1.0
    text_section[(*offset)++] = (char)0xB8;
    double num = 1.0;
    memcpy(text_section + *offset, &num, sizeof(double));
    *offset += sizeof(double);

    text_section[(*offset)++] = (char)0x66; // movq rbx, xmm0
    text_section[(*offset)++] = (char)0x48;
    text_section[(*offset)++] = (char)0x0F;
    text_section[(*offset)++] = (char)0x7E;
    text_section[(*offset)++] = (char)0xC3;

    text_section[(*offset)++] = (char)0x48; // and rax, rbx
    text_section[(*offset)++] = (char)0x21;
    text_section[(*offset)++] = (char)0xD8;

    text_section[(*offset)++] = (char)0x48; // mov [rsp], rax
    text_section[(*offset)++] = (char)0x89;
    text_section[(*offset)++] = (char)0x04;
    text_section[(*offset)++] = (char)0x24;
}

static void TranslateAbove(Node *above, char *text_section, size_t *offset, struct QuadHashTable *table, struct Stack *stack, size_t n_func_params)
{
    TranslateCompare(above, text_section, offset, table, stack, n_func_params, _CMP_GT_OS);
}

static void TranslateAboveEq(Node *abeq, char *text_section, size_t *offset, struct QuadHashTable *table, struct Stack *stack, size_t n_func_params)
{
    TranslateCompare(abeq, text_section, offset, table, stack, n_func_params, _CMP_GE_OS);
}

static void TranslateLess(Node *less, char *text_section, size_t *offset, struct QuadHashTable *table, struct Stack *stack, size_t n_func_params)
{
    TranslateCompare(less, text_section, offset, table, stack, n_func_params, _CMP_LT_OS);
}

static void TranslateLessEq(Node *leeq, char *text_section, size_t *offset, struct QuadHashTable *table, struct Stack *stack, size_t n_func_params)
{
    TranslateCompare(leeq, text_section, offset, table, stack, n_func_params, _CMP_LE_OS);
}

static void TranslateEq(Node *eq, char *text_section, size_t *offset, struct QuadHashTable *table, struct Stack *stack, size_t n_func_params)
{
    TranslateCompare(eq, text_section, offset, table, stack, n_func_params, _CMP_EQ_OS);
}

static void TranslateNotEq(Node *neq, char *text_section, size_t *offset, struct QuadHashTable *table, struct Stack *stack, size_t n_func_params)
{
    TranslateCompare(neq, text_section, offset, table, stack, n_func_params, _CMP_NEQ_OS);
}


static void TranslateAnd(Node *_and, char *text_section, size_t *offset, struct QuadHashTable *table, struct Stack *stack, size_t n_func_params)
{
    TranslateExpression(_and->left, text_section, offset, table, stack, n_func_params);

    text_section[(*offset)++] = (char)0x5E; //pop rsi

    text_section[(*offset)++] = (char)0x48; // test rsi, rsi
    text_section[(*offset)++] = (char)0x85;
    text_section[(*offset)++] = (char)0xF6;

    text_section[(*offset)++] = (char)0x0F; // jz and_false
    text_section[(*offset)++] = (char)0x84;
    size_t jmp1_pos = *offset;
    (*offset) += sizeof(int32_t);

    TranslateExpression(_and->right, text_section, offset, table, stack, n_func_params);

    text_section[(*offset)++] = (char)0x5E; //pop rsi

    text_section[(*offset)++] = (char)0x48; // test rsi, rsi
    text_section[(*offset)++] = (char)0x85;
    text_section[(*offset)++] = (char)0xF6;

    text_section[(*offset)++] = (char)0x0F; // jz and_false
    text_section[(*offset)++] = (char)0x84;
    size_t jmp2_pos = *offset;
    (*offset) += sizeof(int32_t);

    text_section[(*offset)++] = (char)0x48; // mov rax, 1.0
    text_section[(*offset)++] = (char)0xB8;
    double num = 1.0;
    memcpy(text_section + *offset, &num, sizeof(double));
    *offset += sizeof(double);

    text_section[(*offset)++] = (char)0x50; // push rax

    text_section[(*offset)++] = (char)0xE9; // jmp and_end
    size_t jmp3_pos = *offset;
    (*offset) += sizeof(int32_t);

    /*and_false:*/
    int32_t jmp1_offset = (int32_t)(*offset - jmp1_pos) - (int32_t)sizeof(int32_t); // load jmp offset to first jump
    memcpy(text_section + jmp1_pos, &jmp1_offset, sizeof(int32_t));

    int32_t jmp2_offset = (int32_t)(*offset - jmp2_pos) - (int32_t)sizeof(int32_t); // load jmp offset to second jump
    memcpy(text_section + jmp2_pos, &jmp2_offset, sizeof(int32_t));

    text_section[(*offset)++] = (char)0x6A; // push 0
    text_section[(*offset)++] = (char)0x00;

    /*and_end:*/
    int32_t jmp3_offset = (int32_t)(*offset - jmp3_pos) - (int32_t)sizeof(int32_t); // load jmp offset to third jump
    memcpy(text_section + jmp3_pos, &jmp3_offset, sizeof(int32_t));
}

static void TranslateOr(Node *_or, char *text_section, size_t *offset, struct QuadHashTable *table, struct Stack *stack, size_t n_func_params)
{
    TranslateExpression(_or->left , text_section, offset, table, stack, n_func_params);

    text_section[(*offset)++] = (char)0x5E; //pop rsi

    text_section[(*offset)++] = (char)0x48; // test rsi, rsi
    text_section[(*offset)++] = (char)0x85;
    text_section[(*offset)++] = (char)0xF6;

    text_section[(*offset)++] = (char)0x0F; // jnz or_true
    text_section[(*offset)++] = (char)0x85;
    size_t jmp1_pos = *offset;
    (*offset) += sizeof(int32_t);

    TranslateExpression(_or->right, text_section, offset, table, stack, n_func_params);

    text_section[(*offset)++] = (char)0x5E; //pop rsi

    text_section[(*offset)++] = (char)0x48; // test rsi, rsi
    text_section[(*offset)++] = (char)0x85;
    text_section[(*offset)++] = (char)0xF6;

    text_section[(*offset)++] = (char)0x0F; // jnz or_true
    text_section[(*offset)++] = (char)0x85;
    size_t jmp2_pos = *offset;
    (*offset) += sizeof(int32_t);

    text_section[(*offset)++] = (char)0x6A; // push 0
    text_section[(*offset)++] = (char)0x00;

    text_section[(*offset)++] = (char)0xE9; // jmp or_end
    size_t jmp3_pos = *offset;
    (*offset) += sizeof(int32_t);

    /*or_true:*/
    int32_t jmp1_offset = (int32_t)(*offset - jmp1_pos) - (int32_t)sizeof(int32_t); // load jmp offset to first jump
    memcpy(text_section + jmp1_pos, &jmp1_offset, sizeof(int32_t));

    int32_t jmp2_offset = (int32_t)(*offset - jmp2_pos) - (int32_t)sizeof(int32_t); // load jmp offset to second jump
    memcpy(text_section + jmp2_pos, &jmp2_offset, sizeof(int32_t));

    text_section[(*offset)++] = (char)0x48; // mov rax, 1.0
    text_section[(*offset)++] = (char)0xB8;
    double num = 1.0;
    memcpy(text_section + *offset, &num, sizeof(double));
    *offset += sizeof(double);

    text_section[(*offset)++] = (char)0x50; // push rax

    /*or_end*/
    int32_t jmp3_offset = (int32_t)(*offset - jmp3_pos) - (int32_t)sizeof(int32_t); // load jmp offset to third jump
    memcpy(text_section + jmp3_pos, &jmp3_offset, sizeof(int32_t));
}


static void TranslateBinaryOp(Node *op, char *text_section, size_t *offset, struct QuadHashTable *table, struct Stack *stack, size_t n_func_params, char op_code)
{
    TranslateExpression(op->right, text_section, offset, table, stack, n_func_params);
    TranslateExpression(op->left , text_section, offset, table, stack, n_func_params);

    text_section[(*offset)++] = (char)0xF2; // movsd xmm0, [rsp]
    text_section[(*offset)++] = (char)0x0F;
    text_section[(*offset)++] = (char)0x10;
    text_section[(*offset)++] = (char)0x04;
    text_section[(*offset)++] = (char)0x24;

    text_section[(*offset)++] = (char)0x48; // add rsp, 8
    text_section[(*offset)++] = (char)0x83;
    text_section[(*offset)++] = (char)0xC4;
    text_section[(*offset)++] = (char)0x08;

    text_section[(*offset)++] = (char)0xF2; // movsd xmm1, [rsp]
    text_section[(*offset)++] = (char)0x0F;
    text_section[(*offset)++] = (char)0x10;
    text_section[(*offset)++] = (char)0x0C;
    text_section[(*offset)++] = (char)0x24;

    text_section[(*offset)++] = (char)0xF2; // op xmm0, xmm1
    text_section[(*offset)++] = (char)0x0F;
    text_section[(*offset)++] = op_code;
    text_section[(*offset)++] = (char)0xC1;

    text_section[(*offset)++] = (char)0xF2; // movsd [rsp], xmm0
    text_section[(*offset)++] = (char)0x0F;
    text_section[(*offset)++] = (char)0x11;
    text_section[(*offset)++] = (char)0x04;
    text_section[(*offset)++] = (char)0x24;
}

static void TranslateAdd(Node *add, char *text_section, size_t *offset, struct QuadHashTable *table, struct Stack *stack, size_t n_func_params)
{
    TranslateBinaryOp(add, text_section, offset, table, stack, n_func_params, 0x58);
}

static void TranslateSub(Node *sub, char *text_section, size_t *offset, struct QuadHashTable *table, struct Stack *stack, size_t n_func_params)
{
    TranslateBinaryOp(sub, text_section, offset, table, stack, n_func_params, 0x5C);
}

static void TranslateMul(Node *mul, char *text_section, size_t *offset, struct QuadHashTable *table, struct Stack *stack, size_t n_func_params)
{
    TranslateBinaryOp(mul, text_section, offset, table, stack, n_func_params, 0x59);
}

static void TranslateDiv(Node *div, char *text_section, size_t *offset, struct QuadHashTable *table, struct Stack *stack, size_t n_func_params)
{
    TranslateBinaryOp(div, text_section, offset, table, stack, n_func_params, 0x5E);
}


static void TranslateNum(Node *num_node, char *text_section, size_t *offset)
{
    double num = GetNumber(num_node);
    text_section[(*offset)++] = (char)0x48; // mov rax, num
    text_section[(*offset)++] = (char)0xB8;
    memcpy(text_section + *offset, &num, sizeof(double));
    *offset += sizeof(double);

    text_section[(*offset)++] = (char)0x50; // push rax
}

static void TranslateFuncCall(Node *func, char *text_section, size_t *offset, struct QuadHashTable *table, struct Stack *stack, size_t n_func_params)
{
    size_t arg_counter = 0;

    for(Node *arg = func->left; arg != NULL; arg = arg->right)
    {
        arg_counter++;
        TranslateExpression(arg->left, text_section, offset, table, stack, n_func_params);
    }

    text_section[(*offset)++] = (char)0xE8; // call func
    struct JMPInf jmp_info = {.pos = *offset, .label = func->data.func};
    StackPush(stack, &jmp_info);
    *offset += sizeof(int32_t);

    if(arg_counter != 0)
    {
        text_section[(*offset)++] = (char)0x48; // add rsp, arg_counter * sizeof(double)
        text_section[(*offset)++] = (char)0x81;
        text_section[(*offset)++] = (char)0xC4;
        int32_t src = (int32_t)(arg_counter * sizeof(double));
        memcpy(text_section + *offset, &src, sizeof(int32_t));
        *offset += sizeof(int32_t);
    }

    text_section[(*offset)++] = (char)0x50; // push rax
}

static void TranslateVariable(Node *var, char *text_section, size_t *offset, size_t n_func_params)
{
    int32_t rbp_offset = 0;

    size_t var_id = GetVariable(var);
    if(var_id < n_func_params)
    {
        rbp_offset = (int32_t)(var_id * sizeof(double) + 16);
    }
    else
    {
        rbp_offset = -(int32_t)((var_id + 1 - n_func_params) * sizeof(double));
    }

    text_section[(*offset)++] = (char)0xFF; // push qword [rbp + rbp_offset]
    text_section[(*offset)++] = (char)0xB5;
    memcpy(text_section + *offset, &rbp_offset, sizeof(int32_t));
    *offset += sizeof(int32_t);
}

static void TranslateExpression(Node *expr, char *text_section, size_t *offset, struct QuadHashTable *table, struct Stack *stack, size_t n_func_params)
{
    switch(expr->type)
    {
        case NodeType::NUM:
        {
            TranslateNum(expr, text_section, offset);
            return;
        }
        case NodeType::VAR:
        {
            TranslateVariable(expr, text_section, offset, n_func_params);
            return;
        }
        case NodeType::FUNC:
        {
            TranslateFuncCall(expr, text_section, offset, table, stack, n_func_params);
            break;
        }
        case NodeType::OP:
        {
            switch(GetOperator(expr))
            {
                case Operator::ABOVE:
                {
                    TranslateAbove(expr, text_section, offset, table, stack, n_func_params);
                    break;
                }
                case Operator::ABOVEEQ:
                {
                    TranslateAboveEq(expr, text_section, offset, table, stack, n_func_params);
                    break;
                }
                case Operator::LESS:
                {
                    TranslateLess(expr, text_section, offset, table, stack, n_func_params);
                    break;
                }
                case Operator::LESSEQ:
                {
                    TranslateLessEq(expr, text_section, offset, table, stack, n_func_params);
                    break;
                }
                case Operator::EQ:
                {
                    TranslateEq(expr, text_section, offset, table, stack, n_func_params);
                    break;
                }
                case Operator::NOTEQ:
                {
                    TranslateNotEq(expr, text_section, offset, table, stack, n_func_params);
                    break;
                }
                case Operator::ADD:
                {
                    TranslateAdd(expr, text_section, offset, table, stack, n_func_params);
                    break;
                }
                case Operator::SUB:
                {
                    TranslateSub(expr, text_section, offset, table, stack, n_func_params);
                    break;
                }
                case Operator::MUL:
                {
                    TranslateMul(expr, text_section, offset, table, stack, n_func_params);
                    break;
                }
                case Operator::DIV:
                {
                    TranslateDiv(expr, text_section, offset, table, stack, n_func_params);
                    break;
                }
                case Operator::AND:
                {
                    TranslateAnd(expr, text_section, offset, table, stack, n_func_params);
                    break;
                }
                case Operator::OR:
                {
                    TranslateOr(expr, text_section, offset, table, stack, n_func_params);
                    break;
                }
                default: return;
            }
            break;
        }
        default: return;
    }
}


static void TranslateAssignment(Node *assig, char *text_section, size_t *offset, struct QuadHashTable *table, struct Stack *stack, size_t n_func_params)
{
    TranslateExpression(assig->right, text_section, offset, table, stack, n_func_params);

    text_section[(*offset)++] = (char)0x58; // pop rax

    text_section[(*offset)++] = (char)0x66; // movq xmm0, rax
    text_section[(*offset)++] = (char)0x48;
    text_section[(*offset)++] = (char)0x0F;
    text_section[(*offset)++] = (char)0x6E;
    text_section[(*offset)++] = (char)0xC0;

    int32_t rbp_offset = 0;

    size_t var_id = GetVariable(assig->left);
    if(var_id < n_func_params)
    {
        rbp_offset = (int32_t)(var_id * sizeof(double) + 16);
    }
    else
    {
        rbp_offset = -(int32_t)((var_id + 1 - n_func_params) * sizeof(double));
    }

    text_section[(*offset)++] = (char)0xF2; // movsd [rbp + rbp_offset], xmm0
    text_section[(*offset)++] = (char)0x0F;
    text_section[(*offset)++] = (char)0x11;
    text_section[(*offset)++] = (char)0x85;
    memcpy(text_section + *offset, &rbp_offset, sizeof(int32_t));
    *offset += sizeof(int32_t);
}

static void TranslateIf(Node *_if, char *text_section, size_t *offset, struct QuadHashTable *table, struct Stack *stack, size_t n_func_params)
{
    TranslateExpression(_if->left->right, text_section, offset, table, stack, n_func_params);

    text_section[(*offset)++] = (char)0x58; // pop rax

    text_section[(*offset)++] = (char)0x48; // test rax, rax
    text_section[(*offset)++] = (char)0x85;
    text_section[(*offset)++] = (char)0xC0;

    text_section[(*offset)++] = (char)0x0F; // jz else
    text_section[(*offset)++] = (char)0x84;
    size_t jmp1_pos = *offset;
    (*offset) += sizeof(int32_t);

    TranslateBody(_if->left->left, text_section, offset, table, stack, n_func_params);

    text_section[(*offset)++] = (char)0xE9; // jmp if_end
    size_t jmp2_pos = *offset;
    (*offset) += sizeof(int32_t);

    /*else:*/
    int32_t jmp1_offset = (int32_t)(*offset - jmp1_pos) - (int32_t)sizeof(int32_t); // load jmp offset to first jump
    memcpy(text_section + jmp1_pos, &jmp1_offset, sizeof(int32_t));

    if(_if->right != NULL)
    {
        TranslateBody(_if->right->left, text_section, offset, table, stack, n_func_params);
    }

    /*if_end:*/
    int32_t jmp2_offset = (int32_t)(*offset - jmp2_pos) - (int32_t)sizeof(int32_t); // load jmp offset to second jump
    memcpy(text_section + jmp2_pos, &jmp2_offset, sizeof(int32_t));
}

static void TranslateWhile(Node *_while, char *text_section, size_t *offset, struct QuadHashTable *table, struct Stack *stack, size_t n_func_params)
{
    /*while:*/
    int32_t while_pos = *offset;

    TranslateExpression(_while->left->right, text_section, offset, table, stack, n_func_params);

    text_section[(*offset)++] = (char)0x58; // pop rax

    text_section[(*offset)++] = (char)0x48; // test rax, rax
    text_section[(*offset)++] = (char)0x85;
    text_section[(*offset)++] = (char)0xC0;

    text_section[(*offset)++] = (char)0x0F; // jz while_end
    text_section[(*offset)++] = (char)0x84;
    size_t jmp1_pos = *offset;
    (*offset) += sizeof(int32_t);

    TranslateBody(_while->left->left, text_section, offset, table, stack, n_func_params);

    text_section[(*offset)++] = (char)0xE9; // jmp while
    int32_t jmp2_offset = (int32_t)(while_pos - (int32_t)(*offset)) - (int32_t)sizeof(int32_t);
    memcpy(text_section + *offset, &jmp2_offset, sizeof(int32_t));
    (*offset) += sizeof(int32_t);

    /*while_end:*/
    int32_t jmp1_offset = (int32_t)(*offset - jmp1_pos) - (int32_t)sizeof(int32_t); // load jmp offset to first jump
    memcpy(text_section + jmp1_pos, &jmp1_offset, sizeof(int32_t));
}

static void TranslateRet(Node *_return, char *text_section, size_t *offset, struct QuadHashTable *table, struct Stack *stack, size_t n_func_params)
{
    TranslateExpression(_return->left, text_section, offset, table, stack, n_func_params);

    text_section[(*offset)++] = (char)0x58; // pop rax

    text_section[(*offset)++] = (char)0x48; // mov rsp, rbp
    text_section[(*offset)++] = (char)0x89;
    text_section[(*offset)++] = (char)0xEC;

    text_section[(*offset)++] = (char)0x5D; // pop rbp

    text_section[(*offset)++] = (char)0xC3; // ret
}

static void TranslateBody(Node *body, char *text_section, size_t *offset, struct QuadHashTable *table, struct Stack *stack, size_t n_func_params)
{
    for(; body != NULL; body = body->right)
    {
        if(IsKeyword(body->left))
        {
            switch(GetKeyword(body->left))
            {
                case Keyword::IF:
                {
                    TranslateIf(body->left, text_section, offset, table, stack, n_func_params);
                    break;
                }
                case Keyword::WHILE:
                {
                    TranslateWhile(body->left, text_section, offset, table, stack, n_func_params);
                    break;
                }
                case Keyword::RET:
                {
                    TranslateRet(body->left, text_section, offset, table, stack, n_func_params);
                    break;
                }
                default: return;
            }
        }
        else if(IsOperator(body->left) && (GetOperator(body->left) == Operator::ASS))
        {
            TranslateAssignment(body->left, text_section, offset, table, stack, n_func_params);
        }
        else
        {
            TranslateExpression(body->left, text_section, offset, table, stack, n_func_params);
        }
    }
}


static long GetMaxVarId(Node *func)
{
    if(func == NULL) return -1;

    long left_max_var_id  = GetMaxVarId(func->left);
    long right_max_var_id = GetMaxVarId(func->right);

    long max_var_id = (left_max_var_id > right_max_var_id) ? left_max_var_id : right_max_var_id;
    if(IsVariable(func)) max_var_id = (max_var_id > (long)GetVariable(func)) ? max_var_id : (long)GetVariable(func);

    return max_var_id;
}

static long GetNFuncParams(Node *func)
{
    long n_params = 0;
    for(Node *param = func->right; param != NULL; param = param->right) n_params++;

    return n_params;
}

static void TranslateFunction(Node *func, char *text_section, size_t *offset, struct QuadHashTable *table, struct Stack *stack)
{
    QuadHashTableInsert(table, GetFunction(func), *offset);

    text_section[(*offset)++] = (char)0x55; // push rbp
    text_section[(*offset)++] = (char)0x48;

    text_section[(*offset)++] = (char)0x89; // mov rbp, rsp
    text_section[(*offset)++] = (char)0xE5;

    long n_params = GetNFuncParams(func);
    long n_local  = GetMaxVarId(func) - n_params + 1;

    text_section[(*offset)++] = (char)0x48; // sub rsp, num
    text_section[(*offset)++] = (char)0x81;
    text_section[(*offset)++] = (char)0xEC;

    int32_t src = (int32_t)(n_local * sizeof(double));
    memcpy(text_section + *offset, &src, sizeof(int32_t));
    *offset += sizeof(int32_t);

    TranslateBody(func->left, text_section, offset, table, stack, (size_t)n_params);

    text_section[(*offset)++] = (char)0x48; // mov rsp, rbp
    text_section[(*offset)++] = (char)0x89;
    text_section[(*offset)++] = (char)0xEC;

    text_section[(*offset)++] = (char)0x5D; // pop rbp

    text_section[(*offset)++] = (char)0xC3; // ret
}


static void BuildElf(char *text_section, size_t text_section_sz, const char *prog_name)
{
    char sh_string_tbl[] = "\0.text"
                           "\0.shstrtab";

    Elf64_Ehdr elf_header =
    {
        .e_ident     = {ELFMAG0, ELFMAG1, ELFMAG2, ELFMAG3, ELFCLASS64, ELFDATA2LSB, EV_CURRENT, ELFOSABI_NONE},
        .e_type      = ET_EXEC,
        .e_machine   = EM_X86_64,
        .e_version   = EV_CURRENT,
        .e_entry     = BASE_ADDR +
                       sizeof(Elf64_Ehdr) + 2 * sizeof(Elf64_Shdr) + sizeof(Elf64_Phdr),
        .e_phoff     = sizeof(Elf64_Ehdr) + 2 * sizeof(Elf64_Shdr),
        .e_shoff     = sizeof(Elf64_Ehdr),
        .e_flags     = NONE,
        .e_ehsize    = sizeof(Elf64_Ehdr),
        .e_phentsize = sizeof(Elf64_Phdr),
        .e_phnum     = 1,
        .e_shentsize = sizeof(Elf64_Shdr),
        .e_shnum     = 2,
        .e_shstrndx  = 1
    };

    Elf64_Shdr text_header =
    {
        .sh_name      = 0x01,
        .sh_type      = SHT_PROGBITS,
        .sh_flags     = SHF_ALLOC | SHF_EXECINSTR,
        .sh_addr      = BASE_ADDR +
                        sizeof(Elf64_Ehdr) + 2 * sizeof(Elf64_Shdr) + sizeof(Elf64_Phdr),
        .sh_offset    = sizeof(Elf64_Ehdr) + 2 * sizeof(Elf64_Shdr) + sizeof(Elf64_Phdr),
        .sh_size      = text_section_sz,
        .sh_link      = NONE,
        .sh_info      = NONE,
        .sh_addralign = 0x01,
        .sh_entsize   = 0x00
    };

    Elf64_Shdr shstrtab_header =
    {
        .sh_name      = 0x07,
        .sh_type      = SHT_STRTAB,
        .sh_flags     = NONE,
        .sh_addr      = NONE,
        .sh_offset    = sizeof(Elf64_Ehdr) + 2 * sizeof(Elf64_Shdr) + sizeof(Elf64_Phdr) + text_section_sz,
        .sh_size      = sizeof(sh_string_tbl),
        .sh_link      = NONE,
        .sh_info      = NONE,
        .sh_addralign = 0x01,
        .sh_entsize   = 0x00
    };

    Elf64_Phdr program_header =
    {
        .p_type   = PT_LOAD,
        .p_flags  = PF_R | PF_X,
        .p_offset = sizeof(Elf64_Ehdr) + 2 * sizeof(Elf64_Shdr) + sizeof(Elf64_Phdr),
        .p_vaddr  = BASE_ADDR +
                    sizeof(Elf64_Ehdr) + 2 * sizeof(Elf64_Shdr) + sizeof(Elf64_Phdr),
        .p_paddr  = BASE_ADDR +
                    sizeof(Elf64_Ehdr) + 2 * sizeof(Elf64_Shdr) + sizeof(Elf64_Phdr),
        .p_filesz = text_section_sz,
        .p_memsz  = text_section_sz,
        .p_align  = NONE
    };


    FILE *executable = fopen(prog_name, "w");
    assert(executable);

    fwrite(&elf_header     , sizeof(char), sizeof(elf_header)     , executable);
    fwrite(&text_header    , sizeof(char), sizeof(text_header)    , executable);
    fwrite(&shstrtab_header, sizeof(char), sizeof(shstrtab_header), executable);
    fwrite(&program_header , sizeof(char), sizeof(program_header) , executable);
    fwrite(text_section    , sizeof(char), text_section_sz        , executable);
    fwrite(sh_string_tbl   , sizeof(char), sizeof(sh_string_tbl)  , executable);

    fclose(executable);

    const size_t str_sz = 128;
    char sys_exec[str_sz] = {};

    snprintf(sys_exec, str_sz, "chmod +x %s", prog_name);
    system(sys_exec);
}


static void CatLib(char *text_section, size_t *offset, struct QuadHashTable *table)
{
    struct stat file_stat = {};
    stat("compiler/lib.bin", &file_stat);

    FILE *lib = fopen("compiler/lib.bin", "r");
    assert(lib);
    fread(text_section + *offset, sizeof(char), file_stat.st_size, lib);
    fclose(lib);

    QuadHashTableInsert(table, "printsd", *offset + FUNC_PRINTSD_LIB_OFFSET);
    QuadHashTableInsert(table, "scansd" , *offset + FUNC_SCANSD_LIB_OFFSET );
    QuadHashTableInsert(table, "sqrrtsd", *offset + FUNC_SQRRTSD_LIB_OFFSET);
    QuadHashTableInsert(table, "exit"   , *offset + FUNC_EXIT_LIB_OFFSET   );

    *offset += file_stat.st_size;
}

static void UpdateJmpAddr(char *text_section, struct QuadHashTable *table, struct Stack *stack)
{
    while(stack->size != 0)
    {
        struct JMPInf jmp_info = {};
        StackPop(stack, &jmp_info);

        struct HashMemb *memb = Find(table, jmp_info.label);
        if(memb != NULL)
        {
            int32_t src = (int32_t)memb->val - (int32_t)jmp_info.pos - (int32_t)sizeof(int32_t);
            memcpy(text_section + jmp_info.pos, &src, sizeof(int32_t));
        }
    }
}

static void MakeStart(char *text_section, size_t *offset, struct Stack *stack)
{
    struct JMPInf jmp_info = {};

    text_section[(*offset)++] = (char)0xE8; // call main
    jmp_info = (struct JMPInf){.pos = *offset, .label = "main"};
    StackPush(stack, &jmp_info);
    (*offset) += sizeof(int32_t);

    text_section[(*offset)++] = (char)0x31; // xor edi, edi
    text_section[(*offset)++] = (char)0xFF;

    text_section[(*offset)++] = (char)0xE8; // call exit
    jmp_info = (struct JMPInf){.pos = *offset, .label = "exit"};
    StackPush(stack, &jmp_info);
    (*offset) += sizeof(int32_t);
}


void TranslatorBin(Tree expr_tree, char *text_section, const char *prog_name)
{
    size_t offset = 0;

    struct QuadHashTable *table = QuadHashTableCtr(0);
    struct Stack         *stack = StackCtr(sizeof(JMPInf));

    MakeStart(text_section, &offset, stack);
    for(Node *parse_node = expr_tree.root; parse_node != NULL; parse_node = parse_node->right)
    {
        TranslateFunction(parse_node->left, text_section, &offset, table, stack);
    }

    CatLib(text_section, &offset, table);
    UpdateJmpAddr(text_section, table, stack);

    table = QuadHashTableDtr(table);
    stack = StackDtr(stack);

    BuildElf(text_section, offset, prog_name);
}