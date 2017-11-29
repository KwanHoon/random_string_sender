CC = gcc
OBJS = config.o queue.o str_gen.o converter.o rand_str_sender.o
INC = ./
TARGET = rand_str_sender
LDFLAGS = -lpthread

.SUFFIXES : .c .o

all : $(TARGET)

$(TARGET) : $(OBJS)
	$(CC) -o $@ $(OBJS) $(LDFLAGS)

clean :
	rm -f $(OBJS) $(TARGET)
