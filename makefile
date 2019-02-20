LIB=-L/home/test/opus-1.3/libs-x86-64bit
INC+=-I/home/test/opus-1.3/include

bin := enc
src := $(wildcard *.c *.cpp)
obj := $(src:.c=.o)
obj := $(obj:.cpp=.o)
ld_flags := -lopus

all: $(bin)

enc: enc.o
	@gcc $^ -o $@ $(ld_flags) $(LIB) $(INC)
	@strip $@
	@echo "[gen] "$@

%.o:%.c
	@echo "[ cc] "$@
	@gcc -c -Werror -Wall $< -o $@ $(LIB) $(INC)

clean:
	@echo "cleaning..."
	@rm -f *.o $(bin)
	@echo "done."
