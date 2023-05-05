assembler: utils.o symbols_table.o spread_macros.o data_handler.o commands_handler.o files_handler.o first_pass.o assembler.o second_pass.o
	gcc -g -Wall -ansi -pedantic utils.o symbols_table.o spread_macros.o data_handler.o commands_handler.o files_handler.o assembler.o first_pass.o second_pass.o -o assembler
first_pass.o: first_pass.c symbols_table.h data_handler.h commands_handler.h utils.h
	gcc -c -Wall -ansi -pedantic first_pass.c -o first_pass.o
second_pass.o: second_pass.c commands_handler.h files_handler.h utils.h global.h
	gcc -c -Wall -ansi -pedantic second_pass.c -o second_pass.o
spread_macros.o: spread_macros.c global.h spread_macros.h utils.h
	gcc -c -Wall -ansi -pedantic spread_macros.c -o spread_macros.o
symbols_table.o: symbols_table.c symbols_table.h
	gcc -c -Wall -ansi -pedantic symbols_table.c -o symbols_table.o
utils.o: utils.c global.h utils.h
	gcc -c -Wall -ansi -pedantic utils.c -o utils.o
data_handler.o: data_handler.c data_handler.h utils.h
	gcc -c -Wall -ansi -pedantic data_handler.c -o data_handler.o
commands_handler.o: commands_handler.c commands_handler.h utils.h
	gcc -c -Wall -ansi -pedantic commands_handler.c -o commands_handler.o
files_handler.o: files_handler.c files_handler.h utils.h
	gcc -c -Wall -ansi -pedantic files_handler.c -o files_handler.o
assembler.o: assembler.c global.h symbols_table.h data_handler.h files_handler.h utils.h assembler.h commands_handler.h
	gcc -c -Wall -ansi -pedantic assembler.c -o assembler.o