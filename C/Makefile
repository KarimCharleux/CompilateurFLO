INPUT = ecrire calculation lire si tantque fonction fibo test

assembleur_vers_exercutable: generation_code_nasm
	for a in $(INPUT); do echo "Assemblage: " $${a}; nasm -g output/nasm/$${a}.nasm -f elf -o output/object/$${a}.o; ld -m elf_i386   -s -o output/bin/$${a} output/object/$${a}.o; done;

generation_code_nasm: main
	mkdir -p output/nasm; 
	mkdir -p output/object; 
	mkdir -p output/bin; 
	for a in $(INPUT); do echo "Generation code nasm: " $${a}; ./main -n input/$${a}.flo > output/nasm/$${a}.nasm; done;

main: analyse_lexicale analyse_syntaxique
	gcc -Wall main.c analyse_lexicale.c analyse_syntaxique.c arbre_abstrait.c generation_code.c -o main

analyse_syntaxique :
	bison analyse_syntaxique.y --defines=symboles.h -o analyse_syntaxique.c
analyse_lexicale:
	flex -l -o analyse_lexicale.c analyse_lexicale.l
	
clear:
	rm -f analyse_lexicale.c main analyse_syntaxique.c output/nasm/* output/exemple*

