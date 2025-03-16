section .multiboot_header
header_start:
	; mnumber
	dd 0xe85250d6
	; arch
	dd 0 ; protected i386
	dd header_end - header_start
	; csum
	dd 0x100000000 - (0xe85250d6 + 0 + (header_end - header_start))

	dw 0
	dw 0
	dd 8
header_end:
