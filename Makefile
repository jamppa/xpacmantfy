#
#	Makefile for xpacmantfy - Jani Arvonen
# 

CC			=	gcc
LINK		=	-lX11 -lXft
FREE_TYPE_I	=	/usr/include/freetype2
FLAGS		=	-c -Wall -O2 -I$(FREE_TYPE_I)
RM			= 	rm -f
OUT			= 	xpacmantfy
STD			=	_GNU_SOURCE
OBJS		=	xpacmantfy.o pacman.o x11.o event.o x11_util.o signal.o


.c.o:
		$(CC) $(FLAGS) -D$(STD) $<

all:	xpacmantfy
		
xpacmantfy:		$(OBJS)
		$(CC) $(LINK) $(OBJS) -o $(OUT)

clean:
		$(RM) *.o core $(OUT)
