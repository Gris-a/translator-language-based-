CFLAGS = -Wall -Wextra -O2

OBJDIR   = obj
GENDIR   = ../general
FRONTDIR = ../frontend
BACKDIR  = ../backend

objects = $(addprefix $(OBJDIR)/, main.o tokens.o source.o stack.o save_tree.o parser.o names_table.o tree_struct.o tree_data.o translator_asm.o translator_bin.o hash_table.o)

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

$(OBJDIR)/translator_asm.o: $(BACKDIR)/translator/translator_asm.cpp $(BACKDIR)/translator/translator.h $(wildcard $(GENDIR)/tree/include/*.h) $(wildcard $(GENDIR)/language/*.h)
	@g++ $(CFLAGS)  -c $< -o $@

$(OBJDIR)/translator_bin.o: $(BACKDIR)/translator/translator_bin.cpp $(BACKDIR)/translator/translator.h $(wildcard $(GENDIR)/tree/include/*.h) $(wildcard $(GENDIR)/language/*.h)
	@g++ $(CFLAGS)  -c $< -o $@

$(OBJDIR)/hash_table.o: $(BACKDIR)/hash_table/hash_table.c $(BACKDIR)/hash_table/hash_table.h
	@g++ $(CFLAGS)  -c $< -o $@