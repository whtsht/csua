CC = /usr/bin/gcc

TARGET = memtest
CFLAGS = -c -g -DDEBUG -Wall

OBJS = memtest.o memory.o storage.o

all: $(TARGET)

$(TARGET): $(OBJS)
#	$(CC) -o $@ ./memtest.c ./memory.c
	$(CC) -o $@ $(OBJS)

.c.o:
	$(CC) $(CFLAGS) $*.c

clean:
	rm -rf $(TARGET) *.o *~
