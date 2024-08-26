%ifidn __OUTPUT_FORMAT__,elf
section .note.GNU-stack noalloc noexec nowrite progbits
%endif
%ifidn __OUTPUT_FORMAT__,elf32
section .note.GNU-stack noalloc noexec nowrite progbits
%endif
%ifidn __OUTPUT_FORMAT__,elf64
section .note.GNU-stack noalloc noexec nowrite progbits
%endif

section .text

global SHA512_CalcBlock
global _SHA512_CalcBlock

%define a rbx
%define b rcx
%define c rsi
%define d rdi
%define e r8
%define f r9
%define g r10
%define h r11

;void SHA512_CalcBlock(UInt32 *Intermediate_Hash, const UInt8 *Message_Block)
;0 W
;640 rbx
;648 rbp
;656 retAddr
;rdi Intermediate_Hash
;rsi Message_Block
	align 16
SHA512_CalcBlock:
_SHA512_CalcBlock:
