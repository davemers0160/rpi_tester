INCLUDE_DIR =../../x_compile/include
CC=gcc
CFLAGS=-I$(INCLUDE_DIR)

ODIR=obj
LDIR =../../x_compile/build

LIBS=-llibtest_gen

_DEPS = test_gen_lib.h
DEPS = $(patsubst %,$(INCLUDE_DIR)/%,$(_DEPS))

_OBJ = test.o
OBJ = $(patsubst %,$(ODIR)/%,$(_OBJ))


$(ODIR)/%.o: %.c $(DEPS)
	$(CC) -c -o $@ $< $(CFLAGS)

hellomake: $(OBJ)
	$(CC) -o $@ $^ $(CFLAGS) $(LDIR)/$(LIBS)

.PHONY: clean

clean:
	rm -f $(ODIR)/*.o *~ core $(INCDIR)/*~ 
