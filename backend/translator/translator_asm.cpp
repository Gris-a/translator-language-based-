#include "translator.h"


static int LABEL_N = 0;


static void TranslateCompare(Node *comp, FILE *asm_file, size_t n_func_params, int comp_t);
static void TranslateAbove(Node *above, FILE *asm_file, size_t n_func_params);
static void TranslateAboveEq(Node *abeq, FILE *asm_file, size_t n_func_params);
static void TranslateLess(Node *less, FILE *asm_file, size_t n_func_params);
static void TranslateLessEq(Node *leeq, FILE *asm_file, size_t n_func_params);
static void TranslateEq(Node *eq, FILE *asm_file, size_t n_func_params);
static void TranslateNotEq(Node *neq, FILE *asm_file, size_t n_func_params);

static void TranslateAnd(Node *_and, FILE *asm_file, size_t n_func_params);
static void TranslateOr(Node *_or, FILE *asm_file, size_t n_func_params);

static void TranslateBinaryOp(Node *op, FILE *asm_file, size_t n_func_params, char *op_asm);
static void TranslateAdd(Node *add, FILE *asm_file, size_t n_func_params);
static void TranslateSub(Node *sub, FILE *asm_file, size_t n_func_params);
static void TranslateMul(Node *mul, FILE *asm_file, size_t n_func_params);
static void TranslateDiv(Node *div, FILE *asm_file, size_t n_func_params);

static void TranslateNum(Node *num_node, FILE *asm_file, size_t n_func_params);
static void TranslateVariable(Node *var, FILE *asm_file, size_t n_func_params);
static void TranslateExpression(Node *expr, FILE *asm_file, size_t n_func_params);

static void TranslateAssignment(Node *assig, FILE *asm_file, size_t n_func_params);
static void TranslateFuncCall(Node *func, FILE *asm_file, size_t n_func_params);
static void TranslateRet(Node *_return, FILE *asm_file, size_t n_func_params);

static void TranslateIf(Node *_if, FILE *asm_file, size_t n_func_params);
static void TranslateWhile(Node *_while, FILE *asm_file, size_t n_func_params);
static void TranslateBody(Node *body, FILE *asm_file, size_t n_func_params);

static long GetMaxVarId(Node *func);
static long GetNFuncParams(Node *func);
static void TranslateFunction(Node *func, FILE *asm_file);

static void MakeHeader(FILE *asm_file);


static void TranslateCompare(Node *comp, FILE *asm_file, size_t n_func_params, int comp_t)
{
    TranslateExpression(comp->right, asm_file, n_func_params);
    TranslateExpression(comp->left , asm_file, n_func_params);

    fprintf(asm_file, "\n"
                      "\tmovsd xmm0, [rsp]\n"
                      "\tadd rsp, 8\n"
                      "\tmovsd xmm1, [rsp]\n"
                      "\tcmpsd xmm0, xmm1, %d\n"
                      "\n"
                      "\tmovsd [rsp], xmm0\n"
                      "\n", comp_t);
}

static void TranslateAbove(Node *above, FILE *asm_file, size_t n_func_params)
{
    TranslateCompare(above, asm_file, n_func_params, _CMP_GT_OS);
}

static void TranslateAboveEq(Node *abeq, FILE *asm_file, size_t n_func_params)
{
    TranslateCompare(abeq, asm_file, n_func_params, _CMP_GE_OS);
}

static void TranslateLess(Node *less, FILE *asm_file, size_t n_func_params)
{
    TranslateCompare(less, asm_file, n_func_params, _CMP_LT_OS);
}

static void TranslateLessEq(Node *leeq, FILE *asm_file, size_t n_func_params)
{
    TranslateCompare(leeq, asm_file, n_func_params, _CMP_LE_OS);
}

static void TranslateEq(Node *eq, FILE *asm_file, size_t n_func_params)
{
    TranslateCompare(eq, asm_file, n_func_params, _CMP_EQ_OS);
}

static void TranslateNotEq(Node *neq, FILE *asm_file, size_t n_func_params)
{
    TranslateCompare(neq, asm_file, n_func_params, _CMP_NEQ_OS);
}


static void TranslateAnd(Node *_and, FILE *asm_file, size_t n_func_params)
{
    int false_label   = LABEL_N++;
    int and_end_label = LABEL_N++;

    TranslateExpression(_and->left, asm_file, n_func_params);
    fprintf(asm_file, "\n"
                      "\tpop rsi\n"
                      "\ttest rsi, rsi\n"
                      "\tjz near AND_FALSE%d\n"
                      "\n", false_label);

    TranslateExpression(_and->right, asm_file, n_func_params);
    fprintf(asm_file, "\n"
                      "\tpop rsi\n"
                      "\ttest rsi, rsi\n"
                      "\tjz near AND_FALSE%d\n"
                      "\n", false_label);

    fprintf(asm_file, "\tpush 1\n"
                      "\tjmp near AND_END%d\n"
                      "AND_FALSE%d:\n"
                      "\tpush 0\n"
                      "AND_END%d:\n"
                      "\n", and_end_label, false_label, and_end_label);
}

static void TranslateOr(Node *_or, FILE *asm_file, size_t n_func_params)
{
    int true_label   = LABEL_N++;
    int or_end_label = LABEL_N++;

    TranslateExpression(_or->left , asm_file, n_func_params);
    fprintf(asm_file, "\n"
                      "\tpop rsi\n"
                      "\ttest rsi, rsi\n"
                      "\tjnz near OR_TRUE%d\n"
                      "\n", true_label);

    TranslateExpression(_or->right, asm_file, n_func_params);
    fprintf(asm_file, "\n"
                      "\tpop rsi\n"
                      "\ttest rsi, rsi\n"
                      "\tjnz near OR_TRUE%d\n"
                      "\n", true_label);

    fprintf(asm_file, "\tpush 0\n"
                      "\tjmp near OR_END%d\n"
                      "OR_TRUE%d:\n"
                      "\tpush 1\n"
                      "OR_END%d:\n", or_end_label, true_label, or_end_label);
}


static void TranslateBinaryOp(Node *op, FILE *asm_file, size_t n_func_params, char *op_asm)
{
    TranslateExpression(op->right, asm_file, n_func_params);
    TranslateExpression(op->left , asm_file, n_func_params);

    fprintf(asm_file, "\n"
                      "\tmovsd xmm0, [rsp]\n"
                      "\tadd rsp, 8\n"
                      "\tmovsd xmm1, [rsp]\n"
                      "\t%s xmm0, xmm1\n"
                      "\n"
                      "\tmovsd [rsp], xmm0\n"
                      "\n", op_asm);
}

static void TranslateAdd(Node *add, FILE *asm_file, size_t n_func_params)
{
    TranslateBinaryOp(add, asm_file, n_func_params, "addsd");
}

static void TranslateSub(Node *sub, FILE *asm_file, size_t n_func_params)
{
    TranslateBinaryOp(sub, asm_file, n_func_params, "subsd");
}

static void TranslateMul(Node *mul, FILE *asm_file, size_t n_func_params)
{
    TranslateBinaryOp(mul, asm_file, n_func_params, "mulsd");
}

static void TranslateDiv(Node *div, FILE *asm_file, size_t n_func_params)
{
    TranslateBinaryOp(div, asm_file, n_func_params, "divsd");
}


static void TranslateFuncCall(Node *func, FILE *asm_file, size_t n_func_params)
{
    size_t arg_counter = 0;
    fputc('\n', asm_file);
    for(Node *arg = func->left; arg != NULL; arg = arg->right)
    {
        arg_counter++;
        TranslateExpression(arg->left, asm_file, n_func_params);
    }
    fputc('\n', asm_file);

    fprintf(asm_file, "\tcall %s\n"
                      "\n", func->data.func);

    if(arg_counter != 0) fprintf(asm_file, "\tadd rsp, %zu\n", arg_counter * 8);

    fprintf(asm_file, "\tpush rax\n"
                      "\n");
}


static void TranslateNum(Node *num_node, FILE *asm_file, size_t n_func_params)
{
    double num = GetNumber(num_node);
    fprintf(asm_file, "\tmov rax, %ld\n"
                      "\tpush rax\n", *(long *)(&num));
}

static void TranslateVariable(Node *var, FILE *asm_file, size_t n_func_params)
{
    size_t var_id = GetVariable(var);
    if(var_id < n_func_params)
    {
        fprintf(asm_file, "\tpush qword [rbp + %zu]\n", var_id * 8 + 16);
    }
    else
    {
        fprintf(asm_file, "\tpush qword [rbp - %zu]\n", (var_id + 1 - n_func_params) * 8);
    }
}


static void TranslateExpression(Node *expr, FILE *asm_file, size_t n_func_params)
{
    switch(expr->type)
    {
        case NodeType::NUM:
        {
            TranslateNum(expr, asm_file, n_func_params);
            return;
        }
        case NodeType::VAR:
        {
            TranslateVariable(expr, asm_file, n_func_params);
            return;
        }
        case NodeType::FUNC:
        {
            TranslateFuncCall(expr, asm_file, n_func_params);
            break;
        }
        case NodeType::OP:
        {
            switch(GetOperator(expr))
            {
                case Operator::ABOVE:
                {
                    TranslateAbove(expr, asm_file, n_func_params);
                    break;
                }
                case Operator::ABOVEEQ:
                {
                    TranslateAboveEq(expr, asm_file, n_func_params);
                    break;
                }
                case Operator::LESS:
                {
                    TranslateLess(expr, asm_file, n_func_params);
                    break;
                }
                case Operator::LESSEQ:
                {
                    TranslateLessEq(expr, asm_file, n_func_params);
                    break;
                }
                case Operator::EQ:
                {
                    TranslateEq(expr, asm_file, n_func_params);
                    break;
                }
                case Operator::NOTEQ:
                {
                    TranslateNotEq(expr, asm_file, n_func_params);
                    break;
                }
                case Operator::ADD:
                {
                    TranslateAdd(expr, asm_file, n_func_params);
                    break;
                }
                case Operator::SUB:
                {
                    TranslateSub(expr, asm_file, n_func_params);
                    break;
                }
                case Operator::MUL:
                {
                    TranslateMul(expr, asm_file, n_func_params);
                    break;
                }
                case Operator::DIV:
                {
                    TranslateDiv(expr, asm_file, n_func_params);
                    break;
                }
                case Operator::AND:
                {
                    TranslateAnd(expr, asm_file, n_func_params);
                    break;
                }
                case Operator::OR:
                {
                    TranslateOr(expr, asm_file, n_func_params);
                    break;
                }
                default: return;
            }
            break;
        }
        default: return;
    }
}


static void TranslateAssignment(Node *assig, FILE *asm_file, size_t n_func_params)
{
    TranslateExpression(assig->right, asm_file, n_func_params);
    fprintf(asm_file, "\tmovsd xmm0, [rsp]\n");

    size_t var_id = GetVariable(assig->left);
    if(var_id < n_func_params)
    {
        fprintf(asm_file, "\tmovsd [rbp + %zu], xmm0\n", var_id * 8 + 16);
    }
    else
    {
        fprintf(asm_file, "\tmovsd [rbp - %zu], xmm0\n", (var_id + 1 - n_func_params) * 8);
    }
}

static void TranslateIf(Node *_if, FILE *asm_file, size_t n_func_params)
{
    int if_label = LABEL_N++;
    int if_yes   = LABEL_N++;
    int if_no    = LABEL_N++;

    fprintf(asm_file, "\n"
                      "IF%d:\n", if_label);

    TranslateExpression(_if->left->right, asm_file, n_func_params);
    fprintf(asm_file, "\n"
                      "\tpop rax\n"
                      "\ttest rax, rax\n"
                      "\tjz near ELSE%d\n"
                      "\n", if_no);

    TranslateBody(_if->left->left, asm_file, n_func_params);
    fprintf(asm_file, "\n"
                      "\tjmp near IF_END%d\n"
                      "\n"
                      "ELSE%d:\n", if_yes, if_no);

    if(_if->right)
    {
        TranslateBody(_if->right->left, asm_file, n_func_params);
    }
    fprintf(asm_file, "\n"
                      "IF_END%d:\n", if_yes);
}

static void TranslateWhile(Node *_while, FILE *asm_file, size_t n_func_params)
{
    int while_begin = LABEL_N++;
    int while_end   = LABEL_N++;

    fprintf(asm_file, "\n"
                      "WHILE_BEGIN%d:\n", while_begin);

    TranslateExpression(_while->left->right, asm_file, n_func_params);
    fprintf(asm_file, "\n"
                      "\tpop rax\n"
                      "\ttest rax, rax\n"
                      "\tjz near WHILE_END%d\n"
                      "\n", while_end);

    TranslateBody(_while->left->left, asm_file, n_func_params);
    fprintf(asm_file, "\n"
                      "\tjmp near WHILE_BEGIN%d\n"
                      "WHILE_END%d:\n"
                      "\n", while_begin, while_end);
}

static void TranslateRet(Node *_return, FILE *asm_file, size_t n_func_params)
{
    TranslateExpression(_return->left, asm_file, n_func_params);
    fprintf(asm_file, "\n"
                      "\tpop rax\n"
                      "\tmov rsp, rbp\n"
                      "\tpop rbp\n"
                      "\tret\n"
                      "\n");
}

static void TranslateBody(Node *body, FILE *asm_file, size_t n_func_params)
{
    for(; body != NULL; body = body->right)
    {
        if(IsKeyword(body->left))
        {
            switch(GetKeyword(body->left))
            {
                case Keyword::IF:
                {
                    TranslateIf(body->left, asm_file, n_func_params);
                    break;
                }
                case Keyword::WHILE:
                {
                    TranslateWhile(body->left, asm_file, n_func_params);
                    break;
                }
                case Keyword::RET:
                {
                    TranslateRet(body->left, asm_file, n_func_params);
                    break;
                }
                default: return;
            }
        }
        else if(IsOperator(body->left) && (GetOperator(body->left) == Operator::ASS))
        {
            TranslateAssignment(body->left, asm_file, n_func_params);
        }
        else
        {
            TranslateExpression(body->left, asm_file, n_func_params);
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

static void TranslateFunction(Node *func, FILE *asm_file)
{
    fprintf(asm_file, "%s:\n"
                      "\tpush rbp\n"
                      "\tmov rbp, rsp\n", GetFunction(func));

    long n_params = GetNFuncParams(func);
    long n_local  = GetMaxVarId(func) - n_params + 1;

    fprintf(asm_file, "\tsub rsp, %ld\n", n_local * 8);
    TranslateBody(func->left, asm_file, (size_t)n_params);
    fprintf(asm_file, "\tmov rsp, rbp\n"
                      "\tpop rbp\n"
                      "\tret\n");
}


static void MakeHeader(FILE *asm_file)
{
    fprintf(asm_file, "extern exit\n"
                      "extern printsd\n"
                      "extern scansd\n"
                      "extern sqrrtsd\n"
                      "\n"
                      "section .text\n"
                      "global _start\n"
                      "\n"
                      "_start:\n"
                      "\tcall main\n"
                      "\n"
                      "\txor edi, edi\n"
                      "\tcall exit\n"
                      "\n");
}

void TranslatorAsm(Tree expr_tree, const char *output_file_name)
{
    FILE *asm_file = fopen(output_file_name, "w");
    assert(asm_file);

    MakeHeader(asm_file);
    for(Node *parse_node = expr_tree.root; parse_node != NULL; parse_node = parse_node->right)
    {
        TranslateFunction(parse_node->left, asm_file);
    }

    fclose(asm_file);
}