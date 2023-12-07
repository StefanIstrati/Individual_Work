exec = calculator.exe
sources = $(wildcard src/*.c)
objects = $(sources:.c=.o)
flags = -g


$(exec): $(objects)
	gcc $(objects) $(flags) -o $(exec)

%.o: %.c include/%.h
	gcc -c $(flags) $< -o $@

clean:
	del /Q /F calculator.exe
	del /Q /F src\lexer.o src\token.o src\main.o src\AST.o
