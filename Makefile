CFLAGS = -Wall -Wextra -O2
LDFLAGS = -lncurses
TARGET = snake.out
RM = rm -f
CC = gcc

SRCS = main.c
OBJS = $(SRCS:.c=.o)

all: ${TARGET}

$(TARGET): $(OBJS)
	$(CC) ${LDFLAGS} -o $@ $^

$(SRCS:.c=.d):%.d:%.c
	$(CC) $(CFLAGS) -MM $< >$@

include $(SRCS:.c=.d)

clean:
	-${RM} ${TARGET} ${OBJS} $(SRCS:.c=.d)

