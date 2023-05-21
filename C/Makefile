INPUT = exemple1 exemple2 exemple3 exemple4

assembleur_vers_exercutable: generation_code_nasm main
	for a in $(INPUT); do echo "Assemblage: " $${a}; nasm -g output/nasm/$${a}.nasm -f elf -o output/$${a}.o; rm output/$${a}.o;  done;
#ld -arch i386  -static -o output/$${a} output/$${a}.o; done;


generation_code_nasm:
	for a in $(INPUT); do echo "Generation code nasm: " $${a}; ./main -n input/$${a}.flo > output/nasm/$${a}.nasm; done;

main: analyse_lexicale analyse_syntaxique
	gcc -Wall main.c analyse_lexicale.c analyse_syntaxique.c arbre_abstrait.c generation_code.c -o main
analyse_syntaxique :
	/opt/homebrew/Cellar/bison/3.8.2/bin/bison analyse_syntaxique.y --defines=symboles.h -o analyse_syntaxique.c
analyse_lexicale:
	flex -l -o analyse_lexicale.c analyse_lexicale.l
	
clear:
	rm analyse_lexicale.c main analyse_syntaxique.c

