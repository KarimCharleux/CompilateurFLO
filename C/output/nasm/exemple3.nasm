%include	'io.asm'

section	.bss
sinput:	resb	255	;reserve a 255 byte space in memory for the users input string

section	.text
global _start
_start:
	push	2
	push	4
	pop	ebx		 ; depile la seconde operande dans ebx
	pop	eax		 ; depile la permière operande dans eax
	add	eax, ebx		 ; operation +
	push	eax		 ; empile le résultat
	pop	eax
	cmp	eax, 1		 ;  on verifie la condition
	jmp	if0		 ; Aller au si
	jmp	else0		 ; Aller au sinon
	if0:		 ; Entrer dans le si
	push	10
	pop	eax
	call	iprintLF
	else0:		 ; Aller dans le sinon
	push	5
	pop	eax
	call	iprintLF
	endif0:		 ; Aller a la fin
	mov	eax, 1		 ; 1 est le code de SYS_EXIT
	int	0x80		 ; exit
