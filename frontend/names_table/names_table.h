#ifndef NAMES_TABLE_H
#define NAMES_TABLE_H

#include "../../general/language/types.h"

enum class NameType
{
    KWORD = 0,
    OP    = 1,
    FUNC  = 2,
    VAR   = 3,
};

union name_t
{
    Keyword kword;
    Operator   op;
    size_t n_args;
    size_t var_id;
};

struct Name
{
    char   *ident;
    NameType type;
    name_t    val;
};

struct NamesTable
{
    Name *names;

    size_t capacity;
    size_t     size;
};

const size_t INIT_TABLE_CAPACITY = 100;


Name *SearchName(NamesTable *table, const char *ident);

Name *SearchNameTyped(NamesTable *table, const char *ident, NameType type);

Name *AddName(NamesTable *table, const char *ident, NameType type, name_t name_data);

NamesTable *NamesTableCtorLang(void);

NamesTable *NamesTableCtor(void);

int NamesTableDtor(NamesTable *table);

void NamesTableDump(NamesTable *table);

#endif //NAMES_TABLE_H
