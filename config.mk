# includes and libs
LIBS = -lXt -lXaw -lX11

# paths
PREFIX = /usr

# flags
#DEBUG = -g -Og
SCCCFLAGS = $(DEBUG)
SCCLDFLAGS = $(LIBS) $(DEBUG)
