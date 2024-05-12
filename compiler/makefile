CFLAGS = -D _DEBUG -ggdb3 -std=c++20 -O0 -Wall -Wextra -Weffc++ -Waggressive-loop-optimizations -Wc++14-compat -Wmissing-declarations -Wcast-align -Wcast-qual -Wchar-subscripts -Wconditionally-supported -Wconversion -Wctor-dtor-privacy -Wempty-body -Wfloat-equal -Wformat-nonliteral -Wformat-security -Wformat-signedness -Wformat=2 -Winline -Wlogical-op -Wnon-virtual-dtor -Wopenmp-simd -Woverloaded-virtual -Wpacked -Wpointer-arith -Winit-self -Wredundant-decls -Wshadow -Wsign-conversion -Wsign-promo -Wstrict-null-sentinel -Wstrict-overflow=2 -Wsuggest-attribute=noreturn -Wsuggest-final-methods -Wsuggest-final-types -Wsuggest-override -Wswitch-default -Wswitch-enum -Wsync-nand -Wundef -Wunreachable-code -Wunused -Wuseless-cast -Wvariadic-macros -Wno-literal-suffix -Wno-missing-field-initializers -Wno-narrowing -Wno-old-style-cast -Wno-varargs -Wstack-protector -fcheck-new -fsized-deallocation -fstack-protector -fstrict-overflow -flto-odr-type-merging -fno-omit-frame-pointer -Wlarger-than=8192 -Wstack-usage=8192 -pie -fPIE -Werror=vla -fsanitize=address,alignment,bool,bounds,enum,float-cast-overflow,float-divide-by-zero,integer-divide-by-zero,leak,nonnull-attribute,null,object-size,return,returns-nonnull-attribute,shift,signed-integer-overflow,undefined,unreachable,vla-bound,vptr

OBJDIR   = obj
GENDIR   = ../general
FRONTDIR = ../frontend
BACKDIR  = ../backend

objects = $(addprefix $(OBJDIR)/, main.o tokens.o source.o stack.o save_tree.o parser.o names_table.o tree_struct.o tree_data.o translator.o)

all: $(OBJDIR) ../cp

$(OBJDIR):
	@mkdir $(OBJDIR)

../cp: $(objects)
	@g++ $(CFLAGS)  $^ -o $@

$(OBJDIR)/main.o: main.cpp $(FRONTDIR)/frontend.h $(BACKDIR)/translator/translator.h $(FRONTDIR)/parser/parser.h $(FRONTDIR)/save_tree/save_tree.h $(FRONTDIR)/source/source.h $(FRONTDIR)/tokens/tokens.h $(FRONTDIR)/names_table/names_table.h $(GENDIR)/stack/stack.h $(wildcard $(GENDIR)/tree/include/*.h) $(wildcard $(GENDIR)/language/*.h)
	@g++ $(CFLAGS)  -c $< -o $@


$(OBJDIR)/tokens.o: $(FRONTDIR)/tokens/tokens.cpp $(FRONTDIR)/tokens/tokens.h $(wildcard $(GENDIR)/tree/include/*.h) $(wildcard $(GENDIR)/language/*.h)
	@g++ $(CFLAGS)  -c $< -o $@

$(OBJDIR)/source.o: $(FRONTDIR)/source/source.cpp $(FRONTDIR)/source/source.h
	@g++ $(CFLAGS)  -c $< -o $@

$(OBJDIR)/stack.o: $(GENDIR)/stack/stack.c $(GENDIR)/stack/stack.h
	@g++ $(CFLAGS)  -c $< -o $@

$(OBJDIR)/save_tree.o: $(FRONTDIR)/save_tree/save_tree.cpp $(FRONTDIR)/save_tree/save_tree.h $(wildcard $(GENDIR)/tree/include/*.h) $(wildcard $(GENDIR)/language/*.h)
	@g++ $(CFLAGS)  -c $< -o $@

$(OBJDIR)/parser.o: $(FRONTDIR)/parser/parser.cpp $(FRONTDIR)/parser/parser.h $(GENDIR)/stack/stack.h $(FRONTDIR)/source/source.h $(FRONTDIR)/tokens/tokens.h $(FRONTDIR)/names_table/names_table.h $(wildcard $(GENDIR)/tree/include/*.h) $(wildcard $(GENDIR)/language/*.h)
	@g++ $(CFLAGS)  -c $< -o $@

$(OBJDIR)/names_table.o: $(FRONTDIR)/names_table/names_table.cpp $(FRONTDIR)/names_table/names_table.h $(wildcard $(GENDIR)/language/*.h)
	@g++ $(CFLAGS)  -c $< -o $@

$(OBJDIR)/tree_struct.o: $(GENDIR)/tree/source/tree_struct.cpp $(wildcard $(GENDIR)/tree/include/*.h) $(wildcard $(GENDIR)/language/*.h)
	@g++ $(CFLAGS)  -c $< -o $@

$(OBJDIR)/tree_data.o: $(GENDIR)/tree/source/tree_data.cpp $(wildcard $(GENDIR)/tree/include/*.h) $(wildcard $(GENDIR)/language/*.h)
	@g++ $(CFLAGS)  -c $< -o $@

$(OBJDIR)/translator.o: $(BACKDIR)/translator/translator_asm.cpp $(BACKDIR)/translator/translator.h $(wildcard $(GENDIR)/tree/include/*.h) $(wildcard $(GENDIR)/language/*.h)
	@g++ $(CFLAGS)  -c $< -o $@