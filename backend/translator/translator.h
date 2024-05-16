#ifndef TRANSLATOR_H
#define TRANSLATOR_H

#include <elf.h>
#include <immintrin.h>
#include <inttypes.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>

#include "../../general/tree/include/tree.h"
#include "../../general/stack/stack.h"
#include "../hash_table/hash_table.h"

#define BASE_ADDR   0x40000
#define NONE        0x0

#define FUNC_PRINTSD_LIB_OFFSET 0x000
#define FUNC_SCANSD_LIB_OFFSET  0x1AE
#define FUNC_SQRRTSD_LIB_OFFSET 0x271
#define FUNC_EXIT_LIB_OFFSET    0x281

void TranslatorAsm(Tree expr_tree, const char *output_file_name);
void TranslatorBin(Tree expr_tree, char *text_section, const char *prog_name);

#endif //TRANSLATOR_H