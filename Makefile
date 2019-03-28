.POSIX:

include config.mk

SRC = selchangechecker.c
OBJ = $(SRC:.c=.o)

all: options selchangechecker
	
options:
	@echo selchangechecker build options:
	@echo "CFLAGS  = $(SCCCFLAGS)"
	@echo "LDFLAGS = $(SCCLDFLAGS)"
	@echo "CC      = $(CC)"

.c.o:
	$(CC) $(SCCCFLAGS) -c $<
	
$(OBJ): config.mk

selchangechecker: $(OBJ)
	$(CC) -o $@ $(OBJ) $(SCCLDFLAGS)
	
clean:
	rm -f selchangechecker $(OBJ)
