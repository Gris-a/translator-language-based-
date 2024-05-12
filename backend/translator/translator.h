#ifndef TRANSLATOR_H
#define TRANSLATOR_H

#include <immintrin.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "../../general/tree/include/tree.h"


void TranslatorAsm(Tree expr_tree, const char *output_file_name);

#endif //TRANSLATOR_H