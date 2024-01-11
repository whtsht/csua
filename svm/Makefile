CC = /usr/bin/gcc

TARGET = svm
CFLAGS = -c -g -DDEBUG -Wall
MEMORY = ../memory/memory.o ../memory/storage.o

OBJS = svm.o opinfo.o native.o

all: $(TARGET)

$(TARGET): $(OBJS) $(MEMORY)
	make -C ../memory
	$(CC) -o $@ $^ -lm

.c.o:
	$(CC) $(CFLAGS) $*.c

clean:
	rm -rf $(TARGET) *.o *~ disasm svm
