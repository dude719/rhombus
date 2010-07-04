; Copyright 2009 Nick Johnson
; ISC Licensed, see LICENSE for details

; FPU/SSE init/save/load functions

[bits 32]

section .data

global can_use_fpu

can_use_fpu: 
	dd 0

section .text

global init_fpu
global fpu_save
global fpu_load

init_fpu:
	push ebx

	; check CPUID for FXSR bit
	mov eax, 1
	cpuid
	test edx, 0x01000000
;	jne .nofx

	; set OSFXSR bit in CR4
	mov eax, cr4
	or eax, 0x200
	mov cr4, eax

	mov eax, 1
	mov [can_use_fpu], eax

	; check CPUID for FPU bit
	test edx, 0x00000001
	jne .nofpu
	
	; set FPU control word
.cw:
	mov ax, 0x37F
	mov [.cw], ax
	fldcw [.cw]

.nofpu:
	; check CPUID for SSE bit
	test edx, 0x02000000
	jne .nosse

	; initialize SSE
	mov eax, cr0
	and eax, 0xFFFFFFFB
	or eax, 0x2
	mov cr0, eax
	mov eax, cr4
	or eax, 0x00000600
	mov cr4, eax

.nosse:
.nofx:
	pop ebx
	ret

fpu_save:
	mov eax, [can_use_fpu]
	cmp eax, 0
	je .blank
	mov ecx, [esp+4]
	fxsave [ecx]
.blank:
	ret

fpu_load:
	mov eax, [can_use_fpu]
	cmp eax, 0
	je .blank
	mov ecx, [esp+4]
	fxrstor [ecx]
.blank:
	ret
