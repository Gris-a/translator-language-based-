#include <assert.h>
#include <stdlib.h>
#include <string.h>

#include "names_table.h"

Name *SearchName(NamesTable *table, const char *ident)
{
    for(size_t i = 0; i < table->size; i++)
    {
        if(strcmp(table->names[i].ident, ident) == 0)
        {
            return table->names + i;
        }
    }

    return NULL;
}

Name *SearchNameTyped(NamesTable *table, const char *ident, NameType type)
{
    for(size_t i = 0; i < table->size; i++)
    {
        if((strcmp(table->names[i].ident, ident) == 0) && (table->names[i].type == type))
        {
            return table->names + i;
        }
    }

    return NULL;
}

Name *AddName(NamesTable *table, const char *ident, NameType type, name_t name_data)
{
    if(table->size == table->capacity)
    {
        Name *temp = (Name *)realloc(table->names, sizeof(Name) * (table->capacity *= 2));
        if(!temp) return NULL;

        table->names = temp;
    }

    table->names[table->size] = {.ident = strdup(ident),
                                 .type  = type,
                                 .val   = name_data};

    return table->names + (table->size++);
}

#define KEYWORD(enum, keyword)  AddName(table, keyword, NameType::KWORD, {.kword = Keyword::enum});
#define OPERATOR(enum, keyword, ...) AddName(table, keyword, NameType::OP   , {.op    = Operator::enum});
NamesTable *NamesTableCtorLang(void)
{
    NamesTable *table = NamesTableCtor();

    #include "../../general/language/keywords.h"
    #include "../../general/language/operators.h"

    return table;
}
#undef KEYWORD
#undef OPERATOR


NamesTable *NamesTableCtor(void)
{
    NamesTable *table = (NamesTable *)malloc(sizeof(NamesTable));
    assert(table);

    *table = {.names    = (Name *)calloc(INIT_TABLE_CAPACITY, sizeof(Name)),
              .capacity = INIT_TABLE_CAPACITY,
              .size     = 0};
    assert(table->names);

    return table;
}

int NamesTableDtor(NamesTable *table)
{
    for(size_t i = 0; i < table->size; i++)
    {
        free(table->names[i].ident);
    }

    free(table->names);
    free(table);

    return EXIT_SUCCESS;
}