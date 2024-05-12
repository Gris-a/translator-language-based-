#ifndef PARSER_H
#define PARSER_H

#include "../../general/tree/include/tree.h"
#include "../../general/stack/stack.h"
#include "../source/source.h"
#include "../tokens/tokens.h"
#include "../names_table/names_table.h"

#define SYN_ASSERT(cond) if(!(cond))\
                         {\
                            printf("Error: Syntax assert catched at %s:%s:%d\n", __FILE__, __PRETTY_FUNCTION__, __LINE__);\
                            (*is_syn_err) = true;\
                            return NULL;\
                         }\

void ClearTables(void);

Tree ParseCode(const char *file_name);

#endif //PARSER_H