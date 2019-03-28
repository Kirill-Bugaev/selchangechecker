# includes and libs
LIBS = -lXt -lXaw -lX11

# flags
#DEBUG = -g -Og
SCCCFLAGS = $(DEBUG)
SCCLDFLAGS = $(LIBS) $(DEBUG)
