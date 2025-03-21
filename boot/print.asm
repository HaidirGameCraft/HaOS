print_:
	push si
.read:
	mov ah, 0x0e
	mov al, byte [si]
	inc si
	test al, al
	jz .done	

	int 0x10
	jmp .read
.done:
	pop si
	ret

print_hex:
	push di
	push si
	push dx

	mov dx, 4
.read:
	mov si, word_hex
	xor ax, ax

	mov al, byte [edi]
	shr al, 4
	and al, 0x0F
	add si, ax

	mov al, byte [si]
	mov ah, 0x0E
	int 0x10

	xor ax, ax
	mov si, word_hex
	mov al, byte [edi]

	and al, 0x0F
	add si, ax

	mov al, byte [si]
	mov ah, 0x0E
	int 0x10

	inc edi
	mov al, ' '
	mov ah, 0x0E
	int 0x10
	
	dec dx
	test dx, dx
	jnz .read	
.done
	pop dx
	pop si
	pop di

	ret

word_hex: db "0123456789ABCDEF"