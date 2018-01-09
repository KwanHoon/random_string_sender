CC = gcc
OBJS = config.o queue.o strgen.o converter.o sender.o formatter.o main.o
INC = ./
TARGET = rand_str_sender
LDFLAGS = -lpthread

.SUFFIXES : .c .o

all : $(TARGET)

$(TARGET) : $(OBJS)
	$(CC) -g -o $@ $(OBJS) $(LDFLAGS)

clean :
	rm -f $(OBJS) $(TARGET)
