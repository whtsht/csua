TARGET = scant
CC = /usr/bin/gcc
CFLAGS = -g -DDEBUG -Wall
MEMORY = ../memory/memory.o ../memory/storage.o
CODEGEN = ../svm/opinfo.o codegenvisitor.o
OBJS = y.tab.o scanner.o keyword.o create.o visitor.o traversor.o util.o interface.o meanvisitor.o
EXEC = scantest.o

YACC = csua.y

cgent: $(MEMORY) $(OBJS) $(CODEGEN) codegentest.o
	make -C ../memory
	make -C ../svm	
	$(CC) -o $@ $^

meant: $(MEMORY) $(OBJS) meantest.o
	make -C ../memory
	$(CC) -o $@ $^

astt: $(MEMORY) $(OBJS) asttest.o
	make -C ../memory
	$(CC) -o $@ $^

treet: $(MEMORY) $(OBJS) treetest.o
	make -C ../memory
	$(CC) -c $(CFLAGS) -DSTORAGE create.c
	$(CC) -o $@ $^

memt: $(MEMORY) memtest.o
	make -C ../memory
	$(CC) -o $@ $^

prst: $(OBJS) parsetest.o $(MEMORY)
	make -C ../memory
	$(CC) -c $(CFLAGS) -DSTORAGE create.c
	$(CC) -o $@ $^

scant: $(OBJS) $(MEMORY) scantest.o
	make -C ../memory
	$(CC) -c $(CFLAGS) -DSTORAGE create.c
	$(CC) -o $@ $^


keyword.c: keyword.key
	gperf -tT $^ > $@
	
y.tab.c: $(YACC)
	bison --yacc -dv $^

.c.o:
	$(CC) -c $(CFLAGS) $*.c

clean:
	rm -rf $(TARGET) *~ keyword.c *.o y.tab.h y.tab.c y.output prst memt treet astt meant cgent
