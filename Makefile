CPP = g++
PROGRAM = blang
BLIB = blang.a

blang.tab.cc blang.tab.hh: blang.y
	bison -d blang.y -o blang.tab.cc

lex.yy.cc: blang.l blang.tab.hh
	flex -o lex.yy.cc blang.l

blang: lex.yy.cc blang.tab.cc blang.tab.hh
	$(CPP) -o $(PROGRAM) blang.tab.cc lex.yy.cc -Iinclude -DBLANG

lib: lex.yy.o blang.tab.o
	ar ru libblang.a $^ 
	ranlib libblang.a

clean:
	$(RM) *.cc *.hh *.o *.a $(PROGRAM) $(BLIB)
