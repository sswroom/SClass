section .text

%ifidn __OUTPUT_FORMAT__, macho64
%define addrpostfix
%else 
%define addrpostfix wrt ..plt
%endif
default rel

extern _MyString_StrDigit100U8
extern _MyString_StrDigit100U16
extern _MyString_StrDigit100U32
extern _MyString_StrHexArrU16
extern _MyString_StrHexArrU32

global MyString_StrConcat
global _MyString_StrConcat
global MyString_StrConcatS
global _MyString_StrConcatS
global MyString_StrConcatSUTF16
global _MyString_StrConcatSUTF16
global MyString_StrConcatUTF16
global _MyString_StrConcatUTF16
global MyString_StrConcatSUTF32
global _MyString_StrConcatSUTF32
global MyString_StrConcatUTF32
global _MyString_StrConcatUTF32

global MyString_StrInt32UTF16
global _MyString_StrInt32UTF16
global MyString_StrUInt32UTF16
global _MyString_StrUInt32UTF16
global MyString_StrInt64UTF16
global _MyString_StrInt64UTF16
global MyString_StrUInt64UTF16
global _MyString_StrUInt64UTF16
global MyString_StrInt32UTF32
global _MyString_StrInt32UTF32
global MyString_StrUInt32UTF32
global _MyString_StrUInt32UTF32
global MyString_StrInt64UTF32
global _MyString_StrInt64UTF32
global MyString_StrUInt64UTF32
global _MyString_StrUInt64UTF32
global MyString_StrInt32
global _MyString_StrInt32
global MyString_StrUInt32
global _MyString_StrUInt32
global MyString_StrInt64
global _MyString_StrInt64
global MyString_StrUInt64
global _MyString_StrUInt64

global MyString_StrCompare
global _MyString_StrCompare
global MyString_StrCompareICase
global _MyString_StrCompareICase
global MyString_StrCompareUTF16
global _MyString_StrCompareUTF16
global MyString_StrCompareICaseUTF16
global _MyString_StrCompareICaseUTF16
global MyString_StrCompareUTF32
global _MyString_StrCompareUTF32
global MyString_StrCompareICaseUTF32
global _MyString_StrCompareICaseUTF32

global MyString_StrCharCnt
global _MyString_StrCharCnt
global MyString_StrCharCntUTF16
global _MyString_StrCharCntUTF16
global MyString_StrCharCntUTF32
global _MyString_StrCharCntUTF32

global MyString_StrHexVal64VUTF16
global _MyString_StrHexVal64VUTF16
global MyString_StrHexVal32VUTF16
global _MyString_StrHexVal32VUTF16
global MyString_StrHexVal64VUTF32
global _MyString_StrHexVal64VUTF32
global MyString_StrHexVal32VUTF32
global _MyString_StrHexVal32VUTF32
global MyString_StrHexVal64UTF16
global _MyString_StrHexVal64UTF16
global MyString_StrHexVal64UTF32
global _MyString_StrHexVal64UTF32
global MyString_StrHexVal32UTF16
global _MyString_StrHexVal32UTF16
global MyString_StrHexVal32UTF32
global _MyString_StrHexVal32UTF32
global MyString_StrHexVal24UTF16
global _MyString_StrHexVal24UTF16
global MyString_StrHexVal24UTF32
global _MyString_StrHexVal24UTF32

global MyString_StrToInt32UTF8
global _MyString_StrToInt32UTF8
global MyString_StrToInt32UTF16
global _MyString_StrToInt32UTF16
global MyString_StrToInt32UTF32
global _MyString_StrToInt32UTF32
global MyString_StrToInt64UTF8
global _MyString_StrToInt64UTF8
global MyString_StrToInt64UTF16
global _MyString_StrToInt64UTF16
global MyString_StrToInt64UTF32
global _MyString_StrToInt64UTF32


;Char *MyString_StrConcat(Char *oriStr, const Char *strToJoin);
;rdi oriStr
;rsi strToJoin
	align 16
MyString_StrConcat:
_MyString_StrConcat:
	xor rdx,rdx
	align 16
sconcatlop:
	movzx eax,byte [rsi+rdx]
	mov byte [rdi+rdx],al
	lea rdx,[rdx+1]
	test al,al
	jnz sconcatlop
	lea rax,[rdi+rdx-1]
	ret

;Char *MyString_StrConcatS(Char *oriStr, const Char *strToJoin, OSInt buffSize);
;rdi oriStr
;rsi strToJoin
;rdx buffSize
	align 16
MyString_StrConcatS:
_MyString_StrConcatS:
			xor rcx,rcx
	align 16
sconcatslop:
	movzx eax,byte [rsi+rcx]
	mov byte [rdi+rcx],al
	test al,al
	jz sconcatslop2
	lea rcx,[rcx+1]
	dec rdx
	jnz sconcatslop
	mov byte [rdi+rcx],0
sconcatslop2:
	lea rax,[rdi+rcx]
	ret


;UTF16Char *MyString_StrConcatSUTF16(UTF16Char *oriStr, const UTF16Char *strToJoin, OSInt buffSize)
;rdi oriStr
;rsi strToJoin
;rdx buffSize
	align 16
MyString_StrConcatSUTF16:
_MyString_StrConcatSUTF16:
	cmp rdx,0
	jle sconcatsu16lop3
	xor rcx,rcx
	align 16
sconcatsu16lop:
	mov eax,dword [rsi+rcx]
	mov word [rdi+rcx],ax
	test ax,ax
	jz sconcatsu16lop2
	lea rcx,[rcx+2]
	dec rdx
	jnz sconcatsu16lop
	mov word [rdi+rcx],0
sconcatsu16lop2:
	lea rax,[rdi+rcx]
	ret

	align 16
sconcatsu16lop3:
	mov word [rdi],0
	mov rax,rdi
	ret

;UTF16Char *MyString_StrConcatUTF16(UTF16Char *oriStr, const UTF16Char *strToJoin)
;rdi oriStr
;rsi strToJoin
	align 16
MyString_StrConcatUTF16:
_MyString_StrConcatUTF16:
	xor rdx,rdx
	align 16
sconcatu16lop:
	mov eax,dword [rsi+rdx]
	mov word [rdi+rdx],ax
	lea rdx,[rdx+2]
	test ax,ax
	jnz sconcatu16lop
	lea rax,[rdi+rdx-2]
	ret

;UTF32Char *MyString_StrConcatSUTF32(UTF32Char *oriStr, const UTF32Char *strToJoin, OSInt buffSize)
;rdi oriStr
;rsi strToJoin
;rdx buffSize
	align 16
MyString_StrConcatSUTF32:
_MyString_StrConcatSUTF32:
	cmp rdx,0
	jle sconcatsu32lop3
	xor rcx,rcx
	align 16
sconcatsu32lop:
	mov eax,dword [rsi+rcx]
	mov dword [rdi+rcx],eax
	test eax,eax
	jz sconcatsu32lop2
	lea rcx,[rcx+4]
	dec rdx
	jnz sconcatsu32lop
	mov dword [rdi+rcx],0
sconcatsu32lop2:
	lea rax,[rdi+rcx]
	ret

	align 16
sconcatsu32lop3:
	mov dword [rdi],0
	mov rax,rdi
	ret

;UTF32Char *MyString_StrConcatUTF32(UTF32Char *oriStr, const UTF32Char *strToJoin)
;rdi oriStr
;rsi strToJoin
	align 16
MyString_StrConcatUTF32:
_MyString_StrConcatUTF32:
	xor rdx,rdx
	align 16
sconcatu32lop:
	mov eax,dword [rsi+rdx]
	mov dword [rdi+rdx],eax
	lea rdx,[rdx+4]
	test eax,eax
	jnz sconcatu32lop
	lea rax,[rdi+rdx-4]
	ret

;UTF16Char *MyString_StrInt32UTF16(UTF16Char *oriStr, Int32 val)
;rdi oriStr
;rsi val
	align 16
MyString_StrInt32UTF16:
_MyString_StrInt32UTF16:
	mov rax,rsi
	test eax,0x80000000
	jz si32u16lop
	neg eax
	mov word [rdi],'-'
	lea rdi,[rdi+2]
	jmp si32u16lop

;UTF16Char *MyString_StrUInt32UTF16(UTF16Char *oriStr, UInt32 val)
;0 retAddr
;rdi oriStr
;rsi val
	align 16
MyString_StrUInt32UTF16:
_MyString_StrUInt32UTF16:
	mov rax,rsi
	align 16
si32u16lop:
	lea r8,[_MyString_StrDigit100U16 addrpostfix]
	cmp eax,10
	jb sui32u16lop1
	cmp eax,100
	jb sui32u16lop2
	mov rcx,100
	xor rdx,rdx
	cmp eax,1000
	jb sui32u16lop3
	cmp eax,10000
	jb sui32u16lop4
	cmp eax,100000
	jb sui32u16lop5
	cmp eax,1000000
	jb sui32u16lop6
	cmp eax,10000000
	jb sui32u16lop7
	cmp eax,100000000
	jb sui32u16lop8
	cmp eax,1000000000
	jb sui32u16lop9
	jmp sui32u16lop10
	
	align 16
sui32u16lop1:
	movzx edx,word [rax*4+r8+2]
	mov dword [rdi],edx
	lea rax,[rdi+2]
	ret
	
	align 16
sui32u16lop2:
	mov edx,dword [rax*4+r8]
	mov dword [rdi],edx
	mov word [rdi+4],0
	lea rax,[rdi+4]
	ret

	align 16
sui32u16lop3:
	div ecx
	mov si,word [rax*4+r8+2]
	mov word [rdi],si
	mov esi,dword [rdx*4+r8]
	mov dword [rdi+2],esi
	mov word [rdi+6],0
	lea rax,[rdi+6]
	jmp sui32u16exit

	align 16
sui32u16lop4:
	div ecx
	mov esi,dword [rax*4+r8]
	mov dword [rdi],esi
	mov esi,dword [rdx*4+r8]
	mov dword [rdi+4],esi
	mov word [rdi+8],0
	lea rax,[rdi+8]
	jmp sui32u16exit


	align 16
sui32u16lop5:
	div ecx
	mov esi,dword [rdx*4+r8]
	xor rdx,rdx
	mov dword [rdi+6],esi
	div ecx
	mov esi,dword [rdx*4+r8]
	mov dword [rdi+2],esi
	mov si,word [rax*4+r8+2]
	mov word [rdi],si
	mov word [rdi+10],0
	lea rax,[rdi+10]
	jmp sui32u16exit

	align 16
sui32u16lop6:
	div ecx
	mov esi,dword [rdx*4+r8]
	xor rdx,rdx
	mov dword [rdi+8],esi
	div ecx
	mov esi,dword [rdx*4+r8]
	mov dword [rdi+4],esi
	mov esi,dword [rax*4+r8]
	mov dword [rdi],esi
	mov word [rdi+12],0
	lea rax,[rdi+12]
	jmp sui32u16exit

	align 16
sui32u16lop7:
	div ecx
	mov esi,dword [rdx*4+r8]
	xor rdx,rdx
	mov dword [rdi+10],esi
	div ecx
	mov esi,dword [rdx*4+r8]
	xor rdx,rdx
	mov dword [rdi+6],esi
	div ecx
	mov esi,dword [rdx*4+r8]
	mov dword [rdi+2],esi
	mov si,word [rax*4+r8+2]
	mov word [rdi],si
	mov word [rdi+14],0
	lea rax,[rdi+14]
	jmp sui32u16exit

	align 16
sui32u16lop8:
	div ecx
	mov esi,dword [rdx*4+r8]
	xor rdx,rdx
	mov dword [rdi+12],esi
	div ecx
	mov esi,dword [rdx*4+r8]
	xor rdx,rdx
	mov dword [rdi+8],esi
	div ecx
	mov esi,dword [rdx*4+r8]
	mov dword [rdi+4],esi
	mov esi,dword [rax*4+r8]
	mov dword [rdi],esi
	mov word [rdi+16],0
	lea rax,[rdi+16]
	jmp sui32u16exit

	align 16
sui32u16lop9:
	div ecx
	mov esi,dword [rdx*4+r8]
	xor rdx,rdx
	mov dword [rdi+14],esi
	div ecx
	mov esi,dword [rdx*4+r8]
	xor rdx,rdx
	mov dword [rdi+10],esi
	div ecx
	mov esi,dword [rdx*4+r8]
	xor rdx,rdx
	mov dword [rdi+6],esi
	div ecx
	mov esi,dword [rdx*4+r8]
	mov dword [rdi+2],esi
	mov si,word [rax*4+r8+2]
	mov word [rdi],si
	mov word [rdi+18],0
	lea rax,[rdi+18]
	jmp sui32u16exit

	align 16
sui32u16lop10:
	div ecx
	mov esi,dword [rdx*4+r8]
	xor rdx,rdx
	mov dword [rdi+16],esi
	div ecx
	mov esi,dword [rdx*4+r8]
	xor rdx,rdx
	mov dword [rdi+12],esi
	div ecx
	mov esi,dword [rdx*4+r8]
	xor rdx,rdx
	mov dword [rdi+8],esi
	div ecx
	mov esi,dword [rdx*4+r8]
	mov dword [rdi+4],esi
	mov esi,dword [rax*4+r8]
	mov dword [rdi],esi
	mov word [rdi+20],0
	lea rax,[rdi+20]
	jmp sui32u16exit
	
	align 16
sui32u16exit:
	ret

;UTF16Char *MyString_StrInt64UTF16(UTF16Char *oriStr, Int64 val)
;0 retAddr
;rdi oriStr
;rsi val
	align 16
MyString_StrInt64UTF16:
_MyString_StrInt64UTF16:
	mov rax,rsi
	cmp rax,0
	jge si64u16lop0
	neg rax
	mov word [rdi],'-'
	lea rdi,[rdi+2]
	jmp si64u16lop0
	
;UTF16Char *MyString_StrUInt64UTF16(UTF16Char *oriStr, Int64 val)
;0 retAddr
;rdi oriStr
;rsi val
	align 16
MyString_StrUInt64UTF16:
_MyString_StrUInt64UTF16:
	mov rax,rsi
	align 16
si64u16lop0:
	mov rdx,0xffffffff
	cmp rax,rdx
	jbe si32u16lop
	lea r8,[_MyString_StrDigit100U16 addrpostfix]
	mov rcx,100
	xor rdx,rdx
	mov rsi,10000000000
	cmp rax,rsi
	jb si64u16lop10
	mov rsi,100000000000
	cmp rax,rsi
	jb si64u16lop11
	mov rsi,1000000000000
	cmp rax,rsi
	jb si64u16lop12
	mov rsi,10000000000000
	cmp rax,rsi
	jb si64u16lop13
	mov rsi,100000000000000
	cmp rax,rsi
	jb si64u16lop14
	mov rsi,1000000000000000
	cmp rax,rsi
	jb si64u16lop15
	mov rsi,10000000000000000
	cmp rax,rsi
	jb si64u16lop16
	mov rsi,100000000000000000
	cmp rax,rsi
	jb si64u16lop17
	mov rsi,1000000000000000000
	cmp rax,rsi
	jb si64u16lop18
	mov rsi,10000000000000000000
	cmp rax,rsi
	jb si64u16lop19
	jmp si64u16lop20

	align 16
si64u16lop10:
	div rcx
	mov esi,dword [rdx*4+r8]
	xor rdx,rdx
	mov dword [rdi+16],esi
	div ecx
	mov esi,dword [rdx*4+r8]
	xor rdx,rdx
	mov dword [rdi+12],esi
	div ecx
	mov esi,dword [rdx*4+r8]
	xor rdx,rdx
	mov dword [rdi+8],esi
	div ecx
	mov esi,dword [rdx*4+r8]
	xor rdx,rdx
	mov dword [rdi+4],esi
	mov esi,dword [rax*4+r8]
	mov dword [rdi],esi
	mov word [rdi+20],0
	lea rax,[rdi+20]
	jmp si64u16exit

	align 16
si64u16lop11:
	div rcx
	mov esi,dword [rdx*4+r8]
	xor rdx,rdx
	mov dword [rdi+18],esi
	div ecx
	mov esi,dword [rdx*4+r8]
	xor rdx,rdx
	mov dword [rdi+14],esi
	div ecx
	mov esi,dword [rdx*4+r8]
	xor rdx,rdx
	mov dword [rdi+10],esi
	div ecx
	mov esi,dword [rdx*4+r8]
	xor rdx,rdx
	mov dword [rdi+6],esi
	div ecx
	mov esi,dword [rdx*4+r8]
	mov dword [rdi+2],esi
	mov si,word [rax*4+r8+2]
	mov word [rdi],si
	mov word [rdi+22],0
	lea rax,[rdi+22]
	jmp si64u16exit

	align 16
si64u16lop12:
	div rcx
	mov esi,dword [rdx*4+r8]
	xor rdx,rdx
	mov dword [rdi+20],esi
	div rcx
	mov esi,dword [rdx*4+r8]
	xor rdx,rdx
	mov dword [rdi+16],esi
	div ecx
	mov esi,dword [rdx*4+r8]
	xor rdx,rdx
	mov dword [rdi+12],esi
	div ecx
	mov esi,dword [rdx*4+r8]
	xor rdx,rdx
	mov dword [rdi+8],esi
	div ecx
	mov esi,dword [rdx*4+r8]
	mov dword [rdi+4],esi
	mov esi,dword [rax*4+r8]
	mov dword [rdi],esi
	mov word [rdi+24],0
	lea rax,[rdi+24]
	jmp si64u16exit

	align 16
si64u16lop13:
	div rcx
	mov esi,dword [rdx*4+r8]
	xor rdx,rdx
	mov dword [rdi+22],esi
	div rcx
	mov esi,dword [rdx*4+r8]
	xor rdx,rdx
	mov dword [rdi+18],esi
	div ecx
	mov esi,dword [rdx*4+r8]
	xor rdx,rdx
	mov dword [rdi+14],esi
	div ecx
	mov esi,dword [rdx*4+r8]
	xor rdx,rdx
	mov dword [rdi+10],esi
	div ecx
	mov esi,dword [rdx*4+r8]
	xor rdx,rdx
	mov dword [rdi+6],esi
	div ecx
	mov esi,dword [rdx*4+r8]
	mov dword [rdi+2],esi
	mov si,word [rax*4+r8+2]
	mov word [rdi],si
	mov word [rdi+26],0
	lea rax,[rdi+26]
	jmp si64u16exit

	align 16
si64u16lop14:
	div rcx
	mov esi,dword [rdx*4+r8]
	xor rdx,rdx
	mov dword [rdi+24],esi
	div rcx
	mov esi,dword [rdx*4+r8]
	xor rdx,rdx
	mov dword [rdi+20],esi
	div rcx
	mov esi,dword [rdx*4+r8]
	xor rdx,rdx
	mov dword [rdi+16],esi
	div ecx
	mov esi,dword [rdx*4+r8]
	xor rdx,rdx
	mov dword [rdi+12],esi
	div ecx
	mov esi,dword [rdx*4+r8]
	xor rdx,rdx
	mov dword [rdi+8],esi
	div ecx
	mov esi,dword [rdx*4+r8]
	mov dword [rdi+4],esi
	mov esi,dword [rax*4+r8]
	mov dword [rdi],esi
	mov word [rdi+28],0
	lea rax,[rdi+28]
	jmp si64u16exit

	align 16
si64u16lop15:
	div rcx
	mov esi,dword [rdx*4+r8]
	xor rdx,rdx
	mov dword [rdi+26],esi
	div rcx
	mov esi,dword [rdx*4+r8]
	xor rdx,rdx
	mov dword [rdi+22],esi
	div rcx
	mov esi,dword [rdx*4+r8]
	xor rdx,rdx
	mov dword [rdi+18],esi
	div ecx
	mov esi,dword [rdx*4+r8]
	xor rdx,rdx
	mov dword [rdi+14],esi
	div ecx
	mov esi,dword [rdx*4+r8]
	xor rdx,rdx
	mov dword [rdi+10],esi
	div ecx
	mov esi,dword [rdx*4+r8]
	xor rdx,rdx
	mov dword [rdi+6],esi
	div ecx
	mov esi,dword [rdx*4+r8]
	mov dword [rdi+2],esi
	mov si,word [rax*4+r8+2]
	mov word [rdi],si
	mov word [rdi+30],0
	lea rax,[rdi+30]
	jmp si64u16exit

	align 16
si64u16lop16:
	div rcx
	mov esi,dword [rdx*4+r8]
	xor rdx,rdx
	mov dword [rdi+28],esi
	div rcx
	mov esi,dword [rdx*4+r8]
	xor rdx,rdx
	mov dword [rdi+24],esi
	div rcx
	mov esi,dword [rdx*4+r8]
	xor rdx,rdx
	mov dword [rdi+20],esi
	div rcx
	mov esi,dword [rdx*4+r8]
	xor rdx,rdx
	mov dword [rdi+16],esi
	div ecx
	mov esi,dword [rdx*4+r8]
	xor rdx,rdx
	mov dword [rdi+12],esi
	div ecx
	mov esi,dword [rdx*4+r8]
	xor rdx,rdx
	mov dword [rdi+8],esi
	div ecx
	mov esi,dword [rdx*4+r8]
	mov dword [rdi+4],esi
	mov esi,dword [rax*4+r8]
	mov dword [rdi],esi
	mov word [rdi+32],0
	lea rax,[rdi+32]
	jmp si64u16exit

	align 16
si64u16lop17:
	div rcx
	mov esi,dword [rdx*4+r8]
	xor rdx,rdx
	mov dword [rdi+30],esi
	div rcx
	mov esi,dword [rdx*4+r8]
	xor rdx,rdx
	mov dword [rdi+26],esi
	div rcx
	mov esi,dword [rdx*4+r8]
	xor rdx,rdx
	mov dword [rdi+22],esi
	div rcx
	mov esi,dword [rdx*4+r8]
	xor rdx,rdx
	mov dword [rdi+18],esi
	div ecx
	mov esi,dword [rdx*4+r8]
	xor rdx,rdx
	mov dword [rdi+14],esi
	div ecx
	mov esi,dword [rdx*4+r8]
	xor rdx,rdx
	mov dword [rdi+10],esi
	div ecx
	mov esi,dword [rdx*4+r8]
	xor rdx,rdx
	mov dword [rdi+6],esi
	div ecx
	mov esi,dword [rdx*4+r8]
	mov dword [rdi+2],esi
	mov si,word [rax*4+r8+2]
	mov word [rdi],si
	mov word [rdi+34],0
	lea rax,[rdi+34]
	jmp si64u16exit

	align 16
si64u16lop18:
	div rcx
	mov esi,dword [rdx*4+r8]
	xor rdx,rdx
	mov dword [rdi+32],esi
	div rcx
	mov esi,dword [rdx*4+r8]
	xor rdx,rdx
	mov dword [rdi+28],esi
	div rcx
	mov esi,dword [rdx*4+r8]
	xor rdx,rdx
	mov dword [rdi+24],esi
	div rcx
	mov esi,dword [rdx*4+r8]
	xor rdx,rdx
	mov dword [rdi+20],esi
	div rcx
	mov esi,dword [rdx*4+r8]
	xor rdx,rdx
	mov dword [rdi+16],esi
	div ecx
	mov esi,dword [rdx*4+r8]
	xor rdx,rdx
	mov dword [rdi+12],esi
	div ecx
	mov esi,dword [rdx*4+r8]
	xor rdx,rdx
	mov dword [rdi+8],esi
	div ecx
	mov esi,dword [rdx*4+r8]
	mov dword [rdi+4],esi
	mov esi,dword [rax*4+r8]
	mov dword [rdi],esi
	mov word [rdi+36],0
	lea rax,[rdi+36]
	jmp si64u16exit

	align 16
si64u16lop19:
	div rcx
	mov esi,dword [rdx*4+r8]
	xor rdx,rdx
	mov dword [rdi+34],esi
	div rcx
	mov esi,dword [rdx*4+r8]
	xor rdx,rdx
	mov dword [rdi+30],esi
	div rcx
	mov esi,dword [rdx*4+r8]
	xor rdx,rdx
	mov dword [rdi+26],esi
	div rcx
	mov esi,dword [rdx*4+r8]
	xor rdx,rdx
	mov dword [rdi+22],esi
	div rcx
	mov esi,dword [rdx*4+r8]
	xor rdx,rdx
	mov dword [rdi+18],esi
	div ecx
	mov esi,dword [rdx*4+r8]
	xor rdx,rdx
	mov dword [rdi+14],esi
	div ecx
	mov esi,dword [rdx*4+r8]
	xor rdx,rdx
	mov dword [rdi+10],esi
	div ecx
	mov esi,dword [rdx*4+r8]
	xor rdx,rdx
	mov dword [rdi+6],esi
	div ecx
	mov esi,dword [rdx*4+r8]
	mov dword [rdi+2],esi
	mov si,word [rax*4+r8+2]
	mov word [rdi],si
	mov word [rdi+38],0
	lea rax,[rdi+38]
	jmp si64u16exit

	align 16
si64u16lop20:
	div rcx
	mov esi,dword [rdx*4+r8]
	xor rdx,rdx
	mov dword [rdi+36],esi
	div rcx
	mov esi,dword [rdx*4+r8]
	xor rdx,rdx
	mov dword [rdi+32],esi
	div rcx
	mov esi,dword [rdx*4+r8]
	xor rdx,rdx
	mov dword [rdi+28],esi
	div rcx
	mov esi,dword [rdx*4+r8]
	xor rdx,rdx
	mov dword [rdi+24],esi
	div rcx
	mov esi,dword [rdx*4+r8]
	xor rdx,rdx
	mov dword [rdi+20],esi
	div rcx
	mov esi,dword [rdx*4+r8]
	xor rdx,rdx
	mov dword [rdi+16],esi
	div ecx
	mov esi,dword [rdx*4+r8]
	xor rdx,rdx
	mov dword [rdi+12],esi
	div ecx
	mov esi,dword [rdx*4+r8]
	xor rdx,rdx
	mov dword [rdi+8],esi
	div ecx
	mov esi,dword [rdx*4+r8]
	mov dword [rdi+4],esi
	mov esi,dword [rax*4+r8]
	mov dword [rdi],esi
	mov word [rdi+40],0
	lea rax,[rdi+40]
;	jmp si64u16exit

	align 16
si64u16exit:
	ret

;UTF32Char *MyString_StrInt32UTF32(UTF32Char *oriStr, Int32 val)
;rdi oriStr
;rsi val
	align 16
MyString_StrInt32UTF32:
_MyString_StrInt32UTF32:
	mov rax,rsi
	test eax,0x80000000
	jz si32u32lop
	neg eax
	mov dword [rdi],'-'
	lea rdi,[rdi+4]
	jmp si32u32lop

;UTF32Char *MyString_StrUInt32UTF32(UTF32Char *oriStr, UInt32 val)
;0 retAddr
;rdi oriStr
;rsi val
	align 16
MyString_StrUInt32UTF32:
_MyString_StrUInt32UTF32:
	mov rax,rsi
	align 16
si32u32lop:
	lea r8,[_MyString_StrDigit100U32 addrpostfix]
	cmp eax,10
	jb sui32u32lop1
	cmp eax,100
	jb sui32u32lop2
	mov rcx,100
	xor rdx,rdx
	cmp eax,1000
	jb sui32u32lop3
	cmp eax,10000
	jb sui32u32lop4
	cmp eax,100000
	jb sui32u32lop5
	cmp eax,1000000
	jb sui32u32lop6
	cmp eax,10000000
	jb sui32u32lop7
	cmp eax,100000000
	jb sui32u32lop8
	cmp eax,1000000000
	jb sui32u32lop9
	jmp sui32u32lop10
	
	align 16
sui32u32lop1:
	mov edx,dword [rax*8+r8+4]
	mov qword [rdi],rdx
	lea rax,[rdi+4]
	ret
	
	align 16
sui32u32lop2:
	mov rdx,qword [rax*8+r8]
	mov qword [rdi],rdx
	mov dword [rdi+8],0
	lea rax,[rdi+8]
	ret

	align 16
sui32u32lop3:
	div ecx
	mov esi,dword [rax*8+r8+4]
	mov dword [rdi],esi
	mov rsi,qword [rdx*8+r8]
	mov qword [rdi+4],rsi
	mov dword [rdi+12],0
	lea rax,[rdi+12]
	jmp sui32u32exit

	align 16
sui32u32lop4:
	div ecx
	mov rsi,qword [rax*8+r8]
	mov qword [rdi],rsi
	mov rsi,qword [rdx*8+r8]
	mov qword [rdi+8],rsi
	mov dword [rdi+16],0
	lea rax,[rdi+16]
	jmp sui32u32exit


	align 16
sui32u32lop5:
	div ecx
	mov rsi,qword [rdx*8+r8]
	xor rdx,rdx
	mov qword [rdi+12],rsi
	div ecx
	mov rsi,qword [rdx*8+r8]
	mov qword [rdi+4],rsi
	mov esi,dword [rax*8+r8+4]
	mov dword [rdi],esi
	mov dword [rdi+20],0
	lea rax,[rdi+20]
	jmp sui32u32exit

	align 16
sui32u32lop6:
	div ecx
	mov rsi,qword [rdx*8+r8]
	xor rdx,rdx
	mov qword [rdi+16],rsi
	div ecx
	mov rsi,qword [rdx*8+r8]
	mov qword [rdi+8],rsi
	mov rsi,qword [rax*8+r8]
	mov qword [rdi],rsi
	mov dword [rdi+24],0
	lea rax,[rdi+24]
	jmp sui32u32exit

	align 16
sui32u32lop7:
	div ecx
	mov rsi,qword [rdx*8+r8]
	xor rdx,rdx
	mov qword [rdi+20],rsi
	div ecx
	mov rsi,qword [rdx*8+r8]
	xor rdx,rdx
	mov qword [rdi+12],rsi
	div ecx
	mov rsi,qword [rdx*8+r8]
	mov qword [rdi+4],rsi
	mov esi,dword [rax*8+r8+4]
	mov dword [rdi],esi
	mov dword [rdi+28],0
	lea rax,[rdi+28]
	jmp sui32u32exit

	align 16
sui32u32lop8:
	div ecx
	mov rsi,qword [rdx*8+r8]
	xor rdx,rdx
	mov qword [rdi+24],rsi
	div ecx
	mov rsi,qword [rdx*8+r8]
	xor rdx,rdx
	mov qword [rdi+16],rsi
	div ecx
	mov rsi,qword [rdx*8+r8]
	mov qword [rdi+8],rsi
	mov rsi,qword [rax*8+r8]
	mov qword [rdi],rsi
	mov dword [rdi+32],0
	lea rax,[rdi+32]
	jmp sui32u32exit

	align 16
sui32u32lop9:
	div ecx
	mov rsi,qword [rdx*8+r8]
	xor rdx,rdx
	mov qword [rdi+28],rsi
	div ecx
	mov rsi,qword [rdx*8+r8]
	xor rdx,rdx
	mov qword [rdi+20],rsi
	div ecx
	mov rsi,qword [rdx*8+r8]
	xor rdx,rdx
	mov qword [rdi+12],rsi
	div ecx
	mov rsi,qword [rdx*8+r8]
	mov qword [rdi+4],rsi
	mov esi,dword [rax*8+r8+4]
	mov dword [rdi],esi
	mov dword [rdi+36],0
	lea rax,[rdi+36]
	jmp sui32u32exit

	align 16
sui32u32lop10:
	div ecx
	mov rsi,qword [rdx*8+r8]
	xor rdx,rdx
	mov qword [rdi+32],rsi
	div ecx
	mov rsi,qword [rdx*8+r8]
	xor rdx,rdx
	mov qword [rdi+24],rsi
	div ecx
	mov rsi,qword [rdx*8+r8]
	xor rdx,rdx
	mov qword [rdi+16],rsi
	div ecx
	mov rsi,qword [rdx*8+r8]
	mov qword [rdi+8],rsi
	mov rsi,qword [rax*8+r8]
	mov qword [rdi],rsi
	mov dword [rdi+40],0
	lea rax,[rdi+40]
	jmp sui32u32exit
	
	align 16
sui32u32exit:
	ret

;UTF32Char *MyString_StrInt64UTF32(UTF32Char *oriStr, Int64 val)
;0 retAddr
;rdi oriStr
;rsi val
	align 16
MyString_StrInt64UTF32:
_MyString_StrInt64UTF32:
	mov rax,rsi
	cmp rax,0
	jge si64u32lop0
	neg rax
	mov dword [rdi],'-'
	lea rdi,[rdi+4]
	jmp si64u32lop0
	
;UTF32Char *MyString_StrUInt64UTF32(UTF32Char *oriStr, Int64 val)
;0 retAddr
;rdi oriStr
;rsi val
	align 16
MyString_StrUInt64UTF32:
_MyString_StrUInt64UTF32:
	mov rax,rsi
	align 16
si64u32lop0:
	mov rdx,0xffffffff
	cmp rax,rdx
	jbe si32u32lop
	lea r8,[_MyString_StrDigit100U32 addrpostfix]
	mov rcx,100
	xor rdx,rdx
	mov rsi,10000000000
	cmp rax,rsi
	jb si64u32lop10
	mov rsi,100000000000
	cmp rax,rsi
	jb si64u32lop11
	mov rsi,1000000000000
	cmp rax,rsi
	jb si64u32lop12
	mov rsi,10000000000000
	cmp rax,rsi
	jb si64u32lop13
	mov rsi,100000000000000
	cmp rax,rsi
	jb si64u32lop14
	mov rsi,1000000000000000
	cmp rax,rsi
	jb si64u32lop15
	mov rsi,10000000000000000
	cmp rax,rsi
	jb si64u32lop16
	mov rsi,100000000000000000
	cmp rax,rsi
	jb si64u32lop17
	mov rsi,1000000000000000000
	cmp rax,rsi
	jb si64u32lop18
	mov rsi,10000000000000000000
	cmp rax,rsi
	jb si64u32lop19
	jmp si64u32lop20

	align 16
si64u32lop10:
	div rcx
	mov rsi,qword [rdx*8+r8]
	xor rdx,rdx
	mov qword [rdi+32],rsi
	div ecx
	mov rsi,qword [rdx*8+r8]
	xor rdx,rdx
	mov qword [rdi+24],rsi
	div ecx
	mov rsi,qword [rdx*8+r8]
	xor rdx,rdx
	mov qword [rdi+16],rsi
	div ecx
	mov rsi,qword [rdx*8+r8]
	xor rdx,rdx
	mov qword [rdi+8],rsi
	mov rsi,qword [rax*8+r8]
	mov qword [rdi],rsi
	mov dword [rdi+40],0
	lea rax,[rdi+40]
	jmp si64u32exit

	align 16
si64u32lop11:
	div rcx
	mov rsi,qword [rdx*8+r8]
	xor rdx,rdx
	mov qword [rdi+36],rsi
	div ecx
	mov rsi,qword [rdx*8+r8]
	xor rdx,rdx
	mov qword [rdi+28],rsi
	div ecx
	mov rsi,qword [rdx*8+r8]
	xor rdx,rdx
	mov qword [rdi+20],rsi
	div ecx
	mov rsi,qword [rdx*8+r8]
	xor rdx,rdx
	mov qword [rdi+12],rsi
	div ecx
	mov rsi,qword [rdx*8+r8]
	mov qword [rdi+4],rsi
	mov esi,dword [rax*8+r8+4]
	mov dword [rdi],esi
	mov dword [rdi+44],0
	lea rax,[rdi+44]
	jmp si64u32exit

	align 16
si64u32lop12:
	div rcx
	mov rsi,qword [rdx*8+r8]
	xor rdx,rdx
	mov qword [rdi+40],rsi
	div rcx
	mov rsi,qword [rdx*8+r8]
	xor rdx,rdx
	mov qword [rdi+32],rsi
	div ecx
	mov rsi,qword [rdx*8+r8]
	xor rdx,rdx
	mov qword [rdi+24],rsi
	div ecx
	mov rsi,qword [rdx*8+r8]
	xor rdx,rdx
	mov qword [rdi+16],rsi
	div ecx
	mov rsi,qword [rdx*8+r8]
	mov qword [rdi+8],rsi
	mov rsi,qword [rax*8+r8]
	mov qword [rdi],rsi
	mov dword [rdi+48],0
	lea rax,[rdi+48]
	jmp si64u32exit

	align 16
si64u32lop13:
	div rcx
	mov rsi,qword [rdx*8+r8]
	xor rdx,rdx
	mov qword [rdi+44],rsi
	div rcx
	mov rsi,qword [rdx*8+r8]
	xor rdx,rdx
	mov qword [rdi+36],rsi
	div ecx
	mov rsi,qword [rdx*8+r8]
	xor rdx,rdx
	mov qword [rdi+28],rsi
	div ecx
	mov rsi,qword [rdx*8+r8]
	xor rdx,rdx
	mov qword [rdi+20],rsi
	div ecx
	mov rsi,qword [rdx*8+r8]
	xor rdx,rdx
	mov qword [rdi+12],rsi
	div ecx
	mov rsi,qword [rdx*8+r8]
	mov qword [rdi+4],rsi
	mov esi,dword [rax*8+r8+4]
	mov dword [rdi],esi
	mov dword [rdi+52],0
	lea rax,[rdi+52]
	jmp si64u32exit

	align 16
si64u32lop14:
	div rcx
	mov rsi,qword [rdx*8+r8]
	xor rdx,rdx
	mov qword [rdi+48],rsi
	div rcx
	mov rsi,qword [rdx*8+r8]
	xor rdx,rdx
	mov qword [rdi+40],rsi
	div rcx
	mov rsi,qword [rdx*8+r8]
	xor rdx,rdx
	mov qword [rdi+32],rsi
	div ecx
	mov rsi,qword [rdx*8+r8]
	xor rdx,rdx
	mov qword [rdi+24],rsi
	div ecx
	mov rsi,qword [rdx*8+r8]
	xor rdx,rdx
	mov qword [rdi+16],rsi
	div ecx
	mov rsi,qword [rdx*8+r8]
	mov qword [rdi+8],rsi
	mov rsi,qword [rax*8+r8]
	mov qword [rdi],rsi
	mov dword [rdi+56],0
	lea rax,[rdi+56]
	jmp si64u32exit

	align 16
si64u32lop15:
	div rcx
	mov rsi,qword [rdx*8+r8]
	xor rdx,rdx
	mov qword [rdi+52],rsi
	div rcx
	mov rsi,qword [rdx*8+r8]
	xor rdx,rdx
	mov qword [rdi+44],rsi
	div rcx
	mov rsi,qword [rdx*8+r8]
	xor rdx,rdx
	mov qword [rdi+36],rsi
	div ecx
	mov rsi,qword [rdx*8+r8]
	xor rdx,rdx
	mov qword [rdi+28],rsi
	div ecx
	mov rsi,qword [rdx*8+r8]
	xor rdx,rdx
	mov qword [rdi+20],rsi
	div ecx
	mov rsi,qword [rdx*8+r8]
	xor rdx,rdx
	mov qword [rdi+12],rsi
	div ecx
	mov rsi,qword [rdx*8+r8]
	mov qword [rdi+4],rsi
	mov esi,dword [rax*8+r8+4]
	mov dword [rdi],esi
	mov dword [rdi+60],0
	lea rax,[rdi+60]
	jmp si64u32exit

	align 16
si64u32lop16:
	div rcx
	mov rsi,qword [rdx*8+r8]
	xor rdx,rdx
	mov qword [rdi+56],rsi
	div rcx
	mov rsi,qword [rdx*8+r8]
	xor rdx,rdx
	mov qword [rdi+48],rsi
	div rcx
	mov rsi,qword [rdx*8+r8]
	xor rdx,rdx
	mov qword [rdi+40],rsi
	div rcx
	mov rsi,qword [rdx*8+r8]
	xor rdx,rdx
	mov qword [rdi+32],rsi
	div ecx
	mov rsi,qword [rdx*8+r8]
	xor rdx,rdx
	mov qword [rdi+24],rsi
	div ecx
	mov rsi,qword [rdx*8+r8]
	xor rdx,rdx
	mov qword [rdi+16],rsi
	div ecx
	mov rsi,qword [rdx*8+r8]
	mov qword [rdi+8],rsi
	mov rsi,qword [rax*8+r8]
	mov qword [rdi],rsi
	mov dword [rdi+64],0
	lea rax,[rdi+64]
	jmp si64u32exit

	align 16
si64u32lop17:
	div rcx
	mov rsi,qword [rdx*8+r8]
	xor rdx,rdx
	mov qword [rdi+60],rsi
	div rcx
	mov rsi,qword [rdx*8+r8]
	xor rdx,rdx
	mov qword [rdi+52],rsi
	div rcx
	mov rsi,qword [rdx*8+r8]
	xor rdx,rdx
	mov qword [rdi+44],rsi
	div rcx
	mov rsi,qword [rdx*8+r8]
	xor rdx,rdx
	mov qword [rdi+36],rsi
	div ecx
	mov rsi,qword [rdx*8+r8]
	xor rdx,rdx
	mov qword [rdi+28],rsi
	div ecx
	mov rsi,qword [rdx*8+r8]
	xor rdx,rdx
	mov qword [rdi+20],rsi
	div ecx
	mov rsi,qword [rdx*8+r8]
	xor rdx,rdx
	mov qword [rdi+12],rsi
	div ecx
	mov rsi,qword [rdx*8+r8]
	mov qword [rdi+4],rsi
	mov esi,dword [rax*8+r8+4]
	mov dword [rdi],esi
	mov dword [rdi+68],0
	lea rax,[rdi+68]
	jmp si64u32exit

	align 16
si64u32lop18:
	div rcx
	mov rsi,qword [rdx*8+r8]
	xor rdx,rdx
	mov qword [rdi+64],rsi
	div rcx
	mov rsi,qword [rdx*8+r8]
	xor rdx,rdx
	mov qword [rdi+56],rsi
	div rcx
	mov rsi,qword [rdx*8+r8]
	xor rdx,rdx
	mov qword [rdi+48],rsi
	div rcx
	mov rsi,qword [rdx*8+r8]
	xor rdx,rdx
	mov qword [rdi+40],rsi
	div rcx
	mov rsi,qword [rdx*8+r8]
	xor rdx,rdx
	mov qword [rdi+32],rsi
	div ecx
	mov rsi,qword [rdx*8+r8]
	xor rdx,rdx
	mov qword [rdi+24],rsi
	div ecx
	mov rsi,qword [rdx*8+r8]
	xor rdx,rdx
	mov qword [rdi+16],rsi
	div ecx
	mov rsi,qword [rdx*8+r8]
	mov qword [rdi+8],rsi
	mov rsi,qword [rax*8+r8]
	mov qword [rdi],rsi
	mov dword [rdi+72],0
	lea rax,[rdi+72]
	jmp si64u32exit

	align 16
si64u32lop19:
	div rcx
	mov rsi,qword [rdx*8+r8]
	xor rdx,rdx
	mov qword [rdi+68],rsi
	div rcx
	mov rsi,qword [rdx*8+r8]
	xor rdx,rdx
	mov qword [rdi+60],rsi
	div rcx
	mov rsi,qword [rdx*8+r8]
	xor rdx,rdx
	mov qword [rdi+52],rsi
	div rcx
	mov rsi,qword [rdx*8+r8]
	xor rdx,rdx
	mov qword [rdi+44],rsi
	div rcx
	mov rsi,qword [rdx*8+r8]
	xor rdx,rdx
	mov qword [rdi+36],rsi
	div ecx
	mov rsi,qword [rdx*8+r8]
	xor rdx,rdx
	mov qword [rdi+28],rsi
	div ecx
	mov rsi,qword [rdx*8+r8]
	xor rdx,rdx
	mov qword [rdi+20],rsi
	div ecx
	mov rsi,qword [rdx*8+r8]
	xor rdx,rdx
	mov qword [rdi+12],rsi
	div ecx
	mov rsi,qword [rdx*8+r8]
	mov qword [rdi+4],rsi
	mov esi,dword [rax*8+r8+4]
	mov dword [rdi],esi
	mov dword [rdi+76],0
	lea rax,[rdi+76]
	jmp si64u32exit

	align 16
si64u32lop20:
	div rcx
	mov rsi,qword [rdx*8+r8]
	xor rdx,rdx
	mov qword [rdi+72],rsi
	div rcx
	mov rsi,qword [rdx*8+r8]
	xor rdx,rdx
	mov qword [rdi+64],rsi
	div rcx
	mov rsi,qword [rdx*8+r8]
	xor rdx,rdx
	mov qword [rdi+56],rsi
	div rcx
	mov rsi,qword [rdx*8+r8]
	xor rdx,rdx
	mov qword [rdi+48],rsi
	div rcx
	mov rsi,qword [rdx*8+r8]
	xor rdx,rdx
	mov qword [rdi+40],rsi
	div rcx
	mov rsi,qword [rdx*8+r8]
	xor rdx,rdx
	mov qword [rdi+32],rsi
	div ecx
	mov rsi,qword [rdx*8+r8]
	xor rdx,rdx
	mov qword [rdi+24],rsi
	div ecx
	mov rsi,qword [rdx*8+r8]
	xor rdx,rdx
	mov qword [rdi+16],rsi
	div ecx
	mov rsi,qword [rdx*8+r8]
	mov qword [rdi+8],rsi
	mov rsi,qword [rax*8+r8]
	mov qword [rdi],rsi
	mov dword [rdi+80],0
	lea rax,[rdi+80]
;	jmp si64u32exit

	align 16
si64u32exit:
	ret

;Char *MyString_StrInt32(Char *oriStr, Int32 val)
;rdi oriStr
;rsi val
	align 16
MyString_StrInt32:
_MyString_StrInt32:
	mov rax,rsi
	test eax,0x80000000
	jz si32lop
	neg eax
	mov byte [rdi],'-'
	lea rdi,[rdi+1]
	jmp si32lop

;Char *MyString_StrUInt32(Char *oriStr, UInt32 val)
;0 retAddr
;rdi oriStr
;rsi val
	align 16
MyString_StrUInt32:
_MyString_StrUInt32:
	mov rax,rsi
	align 16
si32lop:
	lea r8,[_MyString_StrDigit100U8 addrpostfix]
	cmp eax,10
	jb sui32lop1
	cmp eax,100
	jb sui32lop2
	mov rcx,100
	xor rdx,rdx
	cmp eax,1000
	jb sui32lop3
	cmp eax,10000
	jb sui32lop4
	cmp eax,100000
	jb sui32lop5
	cmp eax,1000000
	jb sui32lop6
	cmp eax,10000000
	jb sui32lop7
	cmp eax,100000000
	jb sui32lop8
	cmp eax,1000000000
	jb sui32lop9
	jmp sui32lop10
	
	align 16
sui32lop1:
	movzx edx,byte [rax*2+r8+1]
	mov word [rdi],dx
	lea rax,[rdi+1]
	ret
	
	align 16
sui32lop2:
	mov dx,word [rax*2+r8]
	mov word [rdi],dx
	mov byte [rdi+2],0
	lea rax,[rdi+2]
	ret

	align 16
sui32lop3:
	div ecx
	mov r10b,byte [rax*2+r8+1]
	mov byte [rdi],r10b
	mov si,word [rdx*2+r8]
	mov word [rdi+1],si
	mov byte [rdi+3],0
	lea rax,[rdi+3]
	jmp sui32exit

	align 16
sui32lop4:
	div ecx
	mov si,word [rax*2+r8]
	mov word [rdi],si
	mov si,word [rdx*2+r8]
	mov word [rdi+2],si
	mov byte [rdi+4],0
	lea rax,[rdi+4]
	jmp sui32exit


	align 16
sui32lop5:
	div ecx
	mov si,word [rdx*2+r8]
	xor rdx,rdx
	mov word [rdi+3],si
	div ecx
	mov si,word [rdx*2+r8]
	mov word [rdi+1],si
	mov r10b,byte [rax*2+r8+1]
	mov byte [rdi],r10b
	mov byte [rdi+5],0
	lea rax,[rdi+5]
	jmp sui32exit

	align 16
sui32lop6:
	div ecx
	mov si,word [rdx*2+r8]
	xor rdx,rdx
	mov word [rdi+4],si
	div ecx
	mov si,word [rdx*2+r8]
	mov word [rdi+2],si
	mov si,word [rax*2+r8]
	mov word [rdi],si
	mov byte [rdi+6],0
	lea rax,[rdi+6]
	jmp sui32exit

	align 16
sui32lop7:
	div ecx
	mov si,word [rdx*2+r8]
	xor rdx,rdx
	mov word [rdi+5],si
	div ecx
	mov si,word [rdx*2+r8]
	xor rdx,rdx
	mov word [rdi+3],si
	div ecx
	mov si,word [rdx*2+r8]
	mov word [rdi+1],si
	mov r10b,byte [rax*2+r8+1]
	mov byte [rdi],r10b
	mov byte [rdi+7],0
	lea rax,[rdi+7]
	jmp sui32exit

	align 16
sui32lop8:
	div ecx
	mov si,word [rdx*2+r8]
	xor rdx,rdx
	mov word [rdi+6],si
	div ecx
	mov si,word [rdx*2+r8]
	xor rdx,rdx
	mov word [rdi+4],si
	div ecx
	mov si,word [rdx*2+r8]
	mov word [rdi+2],si
	mov si,word [rax*2+r8]
	mov word [rdi],si
	mov byte [rdi+8],0
	lea rax,[rdi+8]
	jmp sui32exit

	align 16
sui32lop9:
	div ecx
	mov si,word [rdx*2+r8]
	xor rdx,rdx
	mov word [rdi+7],si
	div ecx
	mov si,word [rdx*2+r8]
	xor rdx,rdx
	mov word [rdi+5],si
	div ecx
	mov si,word [rdx*2+r8]
	xor rdx,rdx
	mov word [rdi+3],si
	div ecx
	mov si,word [rdx*2+r8]
	mov word [rdi+1],si
	mov r10b,byte [rax*2+r8+1]
	mov byte [rdi],r10b
	mov byte [rdi+9],0
	lea rax,[rdi+9]
	jmp sui32exit

	align 16
sui32lop10:
	div ecx
	mov si,word [rdx*2+r8]
	xor rdx,rdx
	mov word [rdi+8],si
	div ecx
	mov si,word [rdx*2+r8]
	xor rdx,rdx
	mov word [rdi+6],si
	div ecx
	mov si,word [rdx*2+r8]
	xor rdx,rdx
	mov word [rdi+4],si
	div ecx
	mov si,word [rdx*2+r8]
	mov word [rdi+2],si
	mov si,word [rax*2+r8]
	mov word [rdi],si
	mov byte [rdi+10],0
	lea rax,[rdi+10]
	jmp sui32exit
	
	align 16
sui32exit:
	ret

;Char *MyString_StrInt64(Char *oriStr, Int64 val)
;0 retAddr
;rdi oriStr
;rsi val
	align 16
MyString_StrInt64:
	mov rax,rsi
	cmp rax,0
	jge si64lop0
	neg rax
	mov byte [rdi],'-'
	lea rdi,[rdi+1]
	jmp si64lop0
	
;Char *MyString_StrUInt64(Char *oriStr, Int64 val)
;0 retAddr
;rdi oriStr
;rsi val
	align 16
MyString_StrUInt64:
	mov rax,rsi
	align 16
si64lop0:
	mov rdx,0xffffffff
	cmp rax,rdx
	jbe si32lop
	lea r8,[_MyString_StrDigit100U8 addrpostfix]
	mov rcx,100
	xor rdx,rdx
	mov rsi,10000000000
	cmp rax,rsi
	jb si64lop10
	mov rsi,100000000000
	cmp rax,rsi
	jb si64lop11
	mov rsi,1000000000000
	cmp rax,rsi
	jb si64lop12
	mov rsi,10000000000000
	cmp rax,rsi
	jb si64lop13
	mov rsi,100000000000000
	cmp rax,rsi
	jb si64lop14
	mov rsi,1000000000000000
	cmp rax,rsi
	jb si64lop15
	mov rsi,10000000000000000
	cmp rax,rsi
	jb si64lop16
	mov rsi,100000000000000000
	cmp rax,rsi
	jb si64lop17
	mov rsi,1000000000000000000
	cmp rax,rsi
	jb si64lop18
	mov rsi,10000000000000000000
	cmp rax,rsi
	jb si64lop19
	jmp si64lop20

	align 16
si64lop10:
	div rcx
	mov si,word [rdx*2+r8]
	xor rdx,rdx
	mov word [rdi+8],si
	div ecx
	mov si,word [rdx*2+r8]
	xor rdx,rdx
	mov word [rdi+6],si
	div ecx
	mov si,word [rdx*2+r8]
	xor rdx,rdx
	mov word [rdi+4],si
	div ecx
	mov si,word [rdx*2+r8]
	xor rdx,rdx
	mov word [rdi+2],si
	mov si,word [rax*2+r8]
	mov word [rdi],si
	mov byte [rdi+10],0
	lea rax,[rdi+10]
	jmp si64exit

	align 16
si64lop11:
	div rcx
	mov si,word [rdx*2+r8]
	xor rdx,rdx
	mov word [rdi+9],si
	div ecx
	mov si,word [rdx*2+r8]
	xor rdx,rdx
	mov word [rdi+7],si
	div ecx
	mov si,word [rdx*2+r8]
	xor rdx,rdx
	mov word [rdi+5],si
	div ecx
	mov si,word [rdx*2+r8]
	xor rdx,rdx
	mov word [rdi+3],si
	div ecx
	mov si,word [rdx*2+r8]
	mov word [rdi+1],si
	mov r10b,byte [rax*2+r8+1]
	mov byte [rdi],r10b
	mov byte [rdi+11],0
	lea rax,[rdi+11]
	jmp si64exit

	align 16
si64lop12:
	div rcx
	mov si,word [rdx*2+r8]
	xor rdx,rdx
	mov word [rdi+10],si
	div rcx
	mov si,word [rdx*2+r8]
	xor rdx,rdx
	mov word [rdi+8],si
	div ecx
	mov si,word [rdx*2+r8]
	xor rdx,rdx
	mov word [rdi+6],si
	div ecx
	mov si,word [rdx*2+r8]
	xor rdx,rdx
	mov word [rdi+4],si
	div ecx
	mov si,word [rdx*2+r8]
	mov word [rdi+2],si
	mov si,word [rax*2+r8]
	mov word [rdi],si
	mov byte [rdi+12],0
	lea rax,[rdi+12]
	jmp si64exit

	align 16
si64lop13:
	div rcx
	mov si,word [rdx*2+r8]
	xor rdx,rdx
	mov word [rdi+11],si
	div rcx
	mov si,word [rdx*2+r8]
	xor rdx,rdx
	mov word [rdi+9],si
	div ecx
	mov si,word [rdx*2+r8]
	xor rdx,rdx
	mov word [rdi+7],si
	div ecx
	mov si,word [rdx*2+r8]
	xor rdx,rdx
	mov word [rdi+5],si
	div ecx
	mov si,word [rdx*2+r8]
	xor rdx,rdx
	mov word [rdi+3],si
	div ecx
	mov si,word [rdx*2+r8]
	mov word [rdi+1],si
	mov r10b,byte [rax*2+r8+1]
	mov byte [rdi],r10b
	mov byte [rdi+13],0
	lea rax,[rdi+13]
	jmp si64exit

	align 16
si64lop14:
	div rcx
	mov si,word [rdx*2+r8]
	xor rdx,rdx
	mov word [rdi+12],si
	div rcx
	mov si,word [rdx*2+r8]
	xor rdx,rdx
	mov word [rdi+10],si
	div rcx
	mov si,word [rdx*2+r8]
	xor rdx,rdx
	mov word [rdi+8],si
	div ecx
	mov si,word [rdx*2+r8]
	xor rdx,rdx
	mov word [rdi+6],si
	div ecx
	mov si,word [rdx*2+r8]
	xor rdx,rdx
	mov word [rdi+4],si
	div ecx
	mov si,word [rdx*2+r8]
	mov word [rdi+2],si
	mov si,word [rax*2+r8]
	mov word [rdi],si
	mov byte [rdi+14],0
	lea rax,[rdi+14]
	jmp si64exit

	align 16
si64lop15:
	div rcx
	mov si,word [rdx*2+r8]
	xor rdx,rdx
	mov word [rdi+13],si
	div rcx
	mov si,word [rdx*2+r8]
	xor rdx,rdx
	mov word [rdi+11],si
	div rcx
	mov si,word [rdx*2+r8]
	xor rdx,rdx
	mov word [rdi+9],si
	div ecx
	mov si,word [rdx*2+r8]
	xor rdx,rdx
	mov word [rdi+7],si
	div ecx
	mov si,word [rdx*2+r8]
	xor rdx,rdx
	mov word [rdi+5],si
	div ecx
	mov si,word [rdx*2+r8]
	xor rdx,rdx
	mov word [rdi+3],si
	div ecx
	mov si,word [rdx*2+r8]
	mov word [rdi+1],si
	mov r10b,byte [rax*2+r8+1]
	mov byte [rdi],r10b
	mov byte [rdi+15],0
	lea rax,[rdi+15]
	jmp si64exit

	align 16
si64lop16:
	div rcx
	mov si,word [rdx*2+r8]
	xor rdx,rdx
	mov word [rdi+14],si
	div rcx
	mov si,word [rdx*2+r8]
	xor rdx,rdx
	mov word [rdi+12],si
	div rcx
	mov si,word [rdx*2+r8]
	xor rdx,rdx
	mov word [rdi+10],si
	div rcx
	mov si,word [rdx*2+r8]
	xor rdx,rdx
	mov word [rdi+8],si
	div ecx
	mov si,word [rdx*2+r8]
	xor rdx,rdx
	mov word [rdi+6],si
	div ecx
	mov si,word [rdx*2+r8]
	xor rdx,rdx
	mov word [rdi+4],si
	div ecx
	mov si,word [rdx*2+r8]
	mov word [rdi+2],si
	mov si,word [rax*2+r8]
	mov word [rdi],si
	mov byte [rdi+16],0
	lea rax,[rdi+16]
	jmp si64exit

	align 16
si64lop17:
	div rcx
	mov si,word [rdx*2+r8]
	xor rdx,rdx
	mov word [rdi+15],si
	div rcx
	mov si,word [rdx*2+r8]
	xor rdx,rdx
	mov word [rdi+13],si
	div rcx
	mov si,word [rdx*2+r8]
	xor rdx,rdx
	mov word [rdi+11],si
	div rcx
	mov si,word [rdx*2+r8]
	xor rdx,rdx
	mov word [rdi+9],si
	div ecx
	mov si,word [rdx*2+r8]
	xor rdx,rdx
	mov word [rdi+7],si
	div ecx
	mov si,word [rdx*2+r8]
	xor rdx,rdx
	mov word [rdi+5],si
	div ecx
	mov si,word [rdx*2+r8]
	xor rdx,rdx
	mov word [rdi+3],si
	div ecx
	mov si,word [rdx*2+r8]
	mov word [rdi+1],si
	mov r10b,byte [rax*2+r8+1]
	mov byte [rdi],r10b
	mov byte [rdi+17],0
	lea rax,[rdi+17]
	jmp si64exit

	align 16
si64lop18:
	div rcx
	mov si,word [rdx*2+r8]
	xor rdx,rdx
	mov word [rdi+16],si
	div rcx
	mov si,word [rdx*2+r8]
	xor rdx,rdx
	mov word [rdi+14],si
	div rcx
	mov si,word [rdx*2+r8]
	xor rdx,rdx
	mov word [rdi+12],si
	div rcx
	mov si,word [rdx*2+r8]
	xor rdx,rdx
	mov word [rdi+10],si
	div rcx
	mov si,word [rdx*2+r8]
	xor rdx,rdx
	mov word [rdi+8],si
	div ecx
	mov si,word [rdx*2+r8]
	xor rdx,rdx
	mov word [rdi+6],si
	div ecx
	mov si,word [rdx*2+r8]
	xor rdx,rdx
	mov word [rdi+4],si
	div ecx
	mov si,word [rdx*2+r8]
	mov word [rdi+2],si
	mov si,word [rax*2+r8]
	mov word [rdi],si
	mov byte [rdi+18],0
	lea rax,[rdi+18]
	jmp si64exit

	align 16
si64lop19:
	div rcx
	mov si,word [rdx*2+r8]
	xor rdx,rdx
	mov word [rdi+17],si
	div rcx
	mov si,word [rdx*2+r8]
	xor rdx,rdx
	mov word [rdi+15],si
	div rcx
	mov si,word [rdx*2+r8]
	xor rdx,rdx
	mov word [rdi+13],si
	div rcx
	mov si,word [rdx*2+r8]
	xor rdx,rdx
	mov word [rdi+11],si
	div rcx
	mov si,word [rdx*2+r8]
	xor rdx,rdx
	mov word [rdi+9],si
	div ecx
	mov si,word [rdx*2+r8]
	xor rdx,rdx
	mov word [rdi+7],si
	div ecx
	mov si,word [rdx*2+r8]
	xor rdx,rdx
	mov word [rdi+5],si
	div ecx
	mov si,word [rdx*2+r8]
	xor rdx,rdx
	mov word [rdi+3],si
	div ecx
	mov si,word [rdx*2+r8]
	mov word [rdi+1],si
	mov r10b,byte [rax*2+r8+1]
	mov byte [rdi],r10b
	mov byte [rdi+19],0
	lea rax,[rdi+19]
	jmp si64exit

	align 16
si64lop20:
	div rcx
	mov si,word [rdx*2+r8]
	xor rdx,rdx
	mov word [rdi+18],si
	div rcx
	mov si,word [rdx*2+r8]
	xor rdx,rdx
	mov word [rdi+16],si
	div rcx
	mov si,word [rdx*2+r8]
	xor rdx,rdx
	mov word [rdi+14],si
	div rcx
	mov si,word [rdx*2+r8]
	xor rdx,rdx
	mov word [rdi+12],si
	div rcx
	mov si,word [rdx*2+r8]
	xor rdx,rdx
	mov word [rdi+10],si
	div rcx
	mov si,word [rdx*2+r8]
	xor rdx,rdx
	mov word [rdi+8],si
	div ecx
	mov si,word [rdx*2+r8]
	xor rdx,rdx
	mov word [rdi+6],si
	div ecx
	mov si,word [rdx*2+r8]
	xor rdx,rdx
	mov word [rdi+4],si
	div ecx
	mov si,word [rdx*2+r8]
	mov word [rdi+2],si
	mov si,word [rax*2+r8]
	mov word [rdi],si
	mov byte [rdi+20],0
	lea rax,[rdi+20]
;	jmp si64u16exit

	align 16
si64exit:
	ret

;OSInt MyString_StrCompare(const Char *str1, const Char *str2)
;0 retAddr
;rdi str1
;rsi str2
	align 16
MyString_StrCompare:
_MyString_StrCompare:
scmplop:
	movzx rcx,byte [rdi]
	movzx rdx,byte [rsi]
	test cl,cl
	jz scmplop2
	test dl,dl
	jz scmplop3

	cmp cl,'9'
	ja scmplop5
	cmp dl,'9'
	ja scmplop5
	cmp cl,'0'
	jb scmplop5
	cmp dl,'0'
	jb scmplop5
	
	xor rax,rax
	xor rdx,rdx
	align 16
scmplop6:
	movzx rcx,byte [rdi]
	cmp cl,'0'
	jb scmplop7
	cmp cl,'9'
	ja scmplop7
	lea rax,[rax+rax*4]
	lea rdi,[rdi+1]
	lea rax,[rcx+rax*2-48]
	jmp scmplop6

	align 16
scmplop2:
	test dl,dl
	jz scmplop4
	mov rax,-1
	jmp scmpret
	
	align 16
scmplop3:
	mov rax,1
	jmp scmpret
	
	align 16
scmplop4:
	mov rax,0
	jmp scmpret

	align 16
scmplop7:
	movzx rcx,byte [rsi]
	cmp cl,'0'
	jb scmplop8
	cmp cl,'9'
	ja scmplop8
	lea rdx,[rdx+rdx*4]
	lea rsi,[rsi+1]
	lea rdx,[rcx+rdx*2-48]
	jmp scmplop7
	
	align 16
scmplop8:
	cmp rax,rdx
	jz scmplop
	ja scmplop10
scmplop9:
	mov rax,-1
	jmp scmpret
	
	align 16
scmplop10:
	mov rax,1
	jmp scmpret
	
	align 16
scmplop5:
	cmp cl,dl
	ja scmplop10
	jb scmplop9
	lea rsi,[rsi+1]
	lea rdi,[rdi+1]
	jmp scmplop

	align 16	
scmpret:
	ret

;OSInt MyString_StrCompareICase(const Char *str1, const Char *str2)
;0 retAddr
;rdi str1
;rsi str2
	align 16
MyString_StrCompareICase:
_MyString_StrCompareICase:
scmpiclop:
	movzx rcx,byte [rdi]
	movzx rdx,byte [rsi]
	test cl,cl
	jz scmpiclop2
	test dl,dl
	jz scmpiclop3

	cmp cl,'9'
	ja scmpiclop5
	cmp dl,'9'
	ja scmpiclop5
	cmp cl,'0'
	jb scmpiclop5
	cmp dl,'0'
	jb scmpiclop5
	
	xor rax,rax
	xor rdx,rdx
	align 16
scmpiclop6:
	movzx rcx,byte [rdi]
	cmp cl,'0'
	jb scmpiclop7
	cmp cl,'9'
	ja scmpiclop7
	lea rax,[rax+rax*4]
	lea rdi,[rdi+1]
	lea rax,[rcx+rax*2-48]
	jmp scmpiclop6

	align 16
scmpiclop2:
	test dl,dl
	jz scmpiclop4
	mov rax,-1
	jmp scmpicret
	
	align 16
scmpiclop3:
	mov rax,1
	jmp scmpicret
	
	align 16
scmpiclop4:
	mov rax,0
	jmp scmpicret

	align 16
scmpiclop7:
	movzx rcx,byte [rsi]
	cmp cl,'0'
	jb scmpiclop8
	cmp cl,'9'
	ja scmpiclop8
	lea rdx,[rdx+rdx*4]
	lea rsi,[rsi+1]
	lea rdx,[rcx+rdx*2-48]
	jmp scmpiclop7
	
	align 16
scmpiclop8:
	cmp rax,rdx
	jz scmpiclop
	ja scmpiclop10
	
scmpiclop9:
	mov rax,-1
	jmp scmpicret
	
	align 16
scmpiclop10:
	mov rax,1
	jmp scmpicret
	
	align 16
scmpiclop5:
	cmp cl,'a'
	jb scmpiclop11
	cmp cl,'z'
	ja scmpiclop11
	sub cl,0x20
	align 16
scmpiclop11:
	cmp dl,'a'
	jb scmpiclop12
	cmp dl,'z'
	ja scmpiclop12
	sub dl,0x20
	align 16
scmpiclop12:
	cmp cl,dl
	ja scmpiclop10
	jb scmpiclop9
	lea rsi,[rsi+1]
	lea rdi,[rdi+1]
	jmp scmpiclop
	
	align 16
scmpicret:
	ret

;OSInt MyString_StrCompareUTF16(const UTF16Char *str1, const UTF16Char *str2)
;0 retAddr
;rdi str1
;rsi str2
	align 16
MyString_StrCompareUTF16:
_MyString_StrCompareUTF16:
scmpu16lop:
	movzx ecx,word [rdi]
	movzx edx,word [rsi]
	test cx,cx
	jz scmpu16lop2
	test dx,dx
	jz scmpu16lop3

	cmp cx,'9'
	ja scmpu16lop5
	cmp dx,'9'
	ja scmpu16lop5
	cmp cx,'0'
	jb scmpu16lop5
	cmp dx,'0'
	jb scmpu16lop5
	
	xor rax,rax
	xor rdx,rdx
	align 16
scmpu16lop6:
	movzx ecx,word [rdi]
	cmp cx,'0'
	jb scmpu16lop7
	cmp cx,'9'
	ja scmpu16lop7
	lea rax,[rax+rax*4]
	lea rdi,[rdi+2]
	lea rax,[rcx+rax*2-48]
	jmp scmpu16lop6

	align 16
scmpu16lop2:
	test dx,dx
	jz scmpu16lop4
	mov rax,-1
	jmp scmpu16ret
	
	align 16
scmpu16lop3:
	mov rax,1
	jmp scmpu16ret
	
	align 16
scmpu16lop4:
	mov rax,0
	jmp scmpu16ret

	align 16
scmpu16lop7:
	movzx ecx,word [rsi]
	cmp cx,'0'
	jb scmpu16lop8
	cmp cx,'9'
	ja scmpu16lop8
	lea rdx,[rdx+rdx*4]
	lea rsi,[rsi+2]
	lea rdx,[rcx+rdx*2-48]
	jmp scmpu16lop7
	
	align 16
scmpu16lop8:
	cmp rax,rdx
	jz scmpu16lop
	ja scmpu16lop10
scmpu16lop9:
	mov rax,-1
	jmp scmpu16ret
	
	align 16
scmpu16lop10:
	mov rax,1
	jmp scmpu16ret
	
	align 16
scmpu16lop5:
	cmp cx,dx
	ja scmpu16lop10
	jb scmpu16lop9
	lea rsi,[rsi+2]
	lea rdi,[rdi+2]
	jmp scmpu16lop

	align 16	
scmpu16ret:
	ret

;OSInt MyString_StrCompareICaseUTF16(const UTF16Char *str1, const UTF16Char *str2)
;0 retAddr
;rdi str1
;rsi str2
	align 16
MyString_StrCompareICaseUTF16:
_MyString_StrCompareICaseUTF16:
scmpicu16lop:
	movzx rcx,word [rdi]
	movzx rdx,word [rsi]
	test cx,cx
	jz scmpicu16lop2
	test dx,dx
	jz scmpicu16lop3

	cmp cx,'9'
	ja scmpicu16lop5
	cmp dx,'9'
	ja scmpicu16lop5
	cmp cx,'0'
	jb scmpicu16lop5
	cmp dx,'0'
	jb scmpicu16lop5
	
	xor rax,rax
	xor rdx,rdx
	align 16
scmpicu16lop6:
	movzx rcx,word [rdi]
	cmp cx,'0'
	jb scmpicu16lop7
	cmp cx,'9'
	ja scmpicu16lop7
	lea rax,[rax+rax*4]
	lea rdi,[rdi+2]
	lea rax,[rcx+rax*2-48]
	jmp scmpicu16lop6

	align 16
scmpicu16lop2:
	test dx,dx
	jz scmpicu16lop4
	mov rax,-1
	jmp scmpicu16ret
	
	align 16
scmpicu16lop3:
	mov rax,1
	jmp scmpicu16ret
	
	align 16
scmpicu16lop4:
	mov rax,0
	jmp scmpicu16ret

	align 16
scmpicu16lop7:
	movzx rcx,word [rsi]
	cmp cx,'0'
	jb scmpicu16lop8
	cmp cx,'9'
	ja scmpicu16lop8
	lea rdx,[rdx+rdx*4]
	lea rsi,[rsi+2]
	lea rdx,[rcx+rdx*2-48]
	jmp scmpicu16lop7
	
	align 16
scmpicu16lop8:
	cmp rax,rdx
	jz scmpicu16lop
	ja scmpicu16lop10
	
scmpicu16lop9:
	mov rax,-1
	jmp scmpicu16ret
	
	align 16
scmpicu16lop10:
	mov rax,1
	jmp scmpicu16ret
	
	align 16
scmpicu16lop5:
	cmp cx,'a'
	jb scmpicu16lop11
	cmp cx,'z'
	ja scmpicu16lop11
	sub cx,0x20
	align 16
scmpicu16lop11:
	cmp dx,'a'
	jb scmpicu16lop12
	cmp dx,'z'
	ja scmpicu16lop12
	sub dx,0x20
	align 16
scmpicu16lop12:
	cmp cx,dx
	ja scmpicu16lop10
	jb scmpicu16lop9
	lea rsi,[rsi+2]
	lea rdi,[rdi+2]
	jmp scmpicu16lop
	
	align 16
scmpicu16ret:
	ret

;OSInt MyString_StrCompareUTF32(const UTF32Char *str1, const UTF32Char *str2)
;0 retAddr
;rdi str1
;rsi str2
	align 16
MyString_StrCompareUTF32:
_MyString_StrCompareUTF32:
	xor rcx,rcx
	align 16
scmpu32lop:
	mov ecx,dword [rdi]
	mov edx,dword [rsi]
	test ecx,ecx
	jz scmpu32lop2
	test edx,edx
	jz scmpu32lop3

	cmp ecx,'9'
	ja scmpu32lop5
	cmp edx,'9'
	ja scmpu32lop5
	cmp ecx,'0'
	jb scmpu32lop5
	cmp edx,'0'
	jb scmpu32lop5
	
	xor rax,rax
	xor rdx,rdx
	align 16
scmpu32lop6:
	mov ecx,dword [rdi]
	cmp ecx,'0'
	jb scmpu32lop7
	cmp ecx,'9'
	ja scmpu32lop7
	lea rax,[rax+rax*4]
	lea rdi,[rdi+4]
	lea rax,[rcx+rax*2-48]
	jmp scmpu32lop6

	align 16
scmpu32lop2:
	test edx,edx
	jz scmpu32lop4
	mov rax,-1
	jmp scmpu32ret
	
	align 16
scmpu32lop3:
	mov rax,1
	jmp scmpu32ret
	
	align 16
scmpu32lop4:
	mov rax,0
	jmp scmpu32ret

	align 16
scmpu32lop7:
	mov ecx,dword [rsi]
	cmp ecx,'0'
	jb scmpu32lop8
	cmp ecx,'9'
	ja scmpu32lop8
	lea rdx,[rdx+rdx*4]
	lea rsi,[rsi+4]
	lea rdx,[rcx+rdx*2-48]
	jmp scmpu32lop7
	
	align 16
scmpu32lop8:
	cmp rax,rdx
	jz scmpu32lop
	ja scmpu32lop10
scmpu32lop9:
	mov rax,-1
	jmp scmpu32ret
	
	align 16
scmpu32lop10:
	mov rax,1
	jmp scmpu32ret
	
	align 16
scmpu32lop5:
	cmp ecx,edx
	ja scmpu32lop10
	jb scmpu32lop9
	lea rsi,[rsi+4]
	lea rdi,[rdi+4]
	jmp scmpu32lop

	align 16	
scmpu32ret:
	ret

;OSInt MyString_StrCompareICaseUTF32(const UTF32Char *str1, const UTF32Char *str2)
;0 retAddr
;rdi str1
;rsi str2
	align 16
MyString_StrCompareICaseUTF32:
_MyString_StrCompareICaseUTF32:
	xor rcx,rcx
	align 16
scmpicu32lop:
	mov ecx,dword [rdi]
	mov edx,dword [rsi]
	test ecx,ecx
	jz scmpicu32lop2
	test edx,edx
	jz scmpicu32lop3

	cmp ecx,'9'
	ja scmpicu32lop5
	cmp edx,'9'
	ja scmpicu32lop5
	cmp ecx,'0'
	jb scmpicu32lop5
	cmp edx,'0'
	jb scmpicu32lop5
	
	xor rax,rax
	xor rdx,rdx
	align 16
scmpicu32lop6:
	mov ecx,dword [rdi]
	cmp ecx,'0'
	jb scmpicu32lop7
	cmp ecx,'9'
	ja scmpicu32lop7
	lea rax,[rax+rax*4]
	lea rdi,[rdi+4]
	lea rax,[rcx+rax*2-48]
	jmp scmpicu32lop6

	align 16
scmpicu32lop2:
	test edx,edx
	jz scmpicu32lop4
	mov rax,-1
	jmp scmpicu32ret
	
	align 16
scmpicu32lop3:
	mov rax,1
	jmp scmpicu32ret
	
	align 16
scmpicu32lop4:
	mov rax,0
	jmp scmpicu32ret

	align 16
scmpicu32lop7:
	mov ecx,dword [rsi]
	cmp ecx,'0'
	jb scmpicu32lop8
	cmp ecx,'9'
	ja scmpicu32lop8
	lea rdx,[rdx+rdx*4]
	lea rsi,[rsi+4]
	lea rdx,[rcx+rdx*2-48]
	jmp scmpicu32lop7
	
	align 16
scmpicu32lop8:
	cmp rax,rdx
	jz scmpicu32lop
	ja scmpicu32lop10
	
scmpicu32lop9:
	mov rax,-1
	jmp scmpicu32ret
	
	align 16
scmpicu32lop10:
	mov rax,1
	jmp scmpicu32ret
	
	align 16
scmpicu32lop5:
	cmp ecx,'a'
	jb scmpicu32lop11
	cmp ecx,'z'
	ja scmpicu32lop11
	sub ecx,0x20
	align 16
scmpicu32lop11:
	cmp edx,'a'
	jb scmpicu32lop12
	cmp edx,'z'
	ja scmpicu32lop12
	sub edx,0x20
	align 16
scmpicu32lop12:
	cmp ecx,edx
	ja scmpicu32lop10
	jb scmpicu32lop9
	lea rsi,[rsi+4]
	lea rdi,[rdi+4]
	jmp scmpicu32lop
	
	align 16
scmpicu32ret:
	ret

;OSInt MyString_StrCharCnt(const Char *s)
;0 retAdddr
;rdi s
	align 16
MyString_StrCharCnt:
_MyString_StrCharCnt:
	mov rax,rdi
	align 16
scclop:
	movzx edx,byte [rax]
	lea rax,[rax+1]
	test dl,dl
	jnz scclop
	sub rax,rdi
	dec rax
	ret

;OSInt MyString_StrCharCntUTF16(const UTF16Char *s)
;0 retAddr
;rdi s
	align 16
MyString_StrCharCntUTF16:
_MyString_StrCharCntUTF16:
	mov rax,rdi
	align 16
sccu16lop:
	movzx edx,word [rax]
	lea rax,[rax+2]
	test dx,dx
	jnz sccu16lop
	sub rax,rdi
	shr rax,1
	dec rax
	ret
	
;OSInt MyString_StrCharCntUTF32(const UTF32Char *s)
;0 retAddr
;rdi s
	align 16
MyString_StrCharCntUTF32:
_MyString_StrCharCntUTF32:
	mov rax,rdi
	align 16
sccu32lop:
	mov edx,dword [rax]
	lea rax,[rax+4]
	test edx,edx
	jnz sccu32lop
	sub rax,rdi
	shr rax,2
	dec rax
	ret
	
;UTF16Char *MyString_StrHexVal64VUTF16(UTF16Char *oriStr, Int64 val)
;0 retAddr
;rdi oriStr
;rsi val
	align 16
MyString_StrHexVal64VUTF16:
_MyString_StrHexVal64VUTF16:
	lea r8,[_MyString_StrHexArrU16 addrpostfix]
	mov rax,rsi
	shr rax,32
	jz shv64vu16lop8v
	test eax,0xfffffff0
	jz shv64vu16lop9
	test eax,0xffffff00
	jz shv64vu16lop10
	test eax,0xfffff000
	jz shv64vu16lop11
	test eax,0xffff0000
	jz shv64vu16lop12
	test eax,0xfff00000
	jz shv64vu16lop13
	test eax,0xff000000
	jz shv64vu16lop14
	test eax,0xf0000000
	jz shv64vu16lop15
	jmp shv64vu16lop16
	
	align 16
shv64vu16lop15:
	mov edx,eax
	shr edx,24
	movzx rdx,dl
	mov dx,word [rdx*4+r8+2]
	mov word [rdi],dx
	lea rdi,[rdi+2]
	jmp shv64vu16lop14
	
	align 16
shv64vu16lop13:
	mov edx,eax
	shr edx,16
	movzx rdx,dl
	mov dx,word [rdx*4+r8+2]
	mov word [rdi],dx
	lea rdi,[rdi+2]
	jmp shv64vu16lop12
	
	align 16
shv64vu16lop11:
	mov edx,eax
	shr edx,8
	movzx rdx,dl
	mov dx,word [rdx*4+r8+2]
	mov word [rdi],dx
	lea rdi,[rdi+2]
	jmp shv64vu16lop10
	
	align 16
shv64vu16lop9:
	mov dx,word [rax*4+r8+2]
	mov word [rdi],dx
	lea rdi,[rdi+2]
	jmp shv64vu16lop8
	
	align 16
shv64vu16lop8v:
	mov rax,rsi
	test eax,0xfffffff0
	jz shv64vu16lop1
	test eax,0xffffff00
	jz shv64vu16lop2
	test eax,0xfffff000
	jz shv64vu16lop3
	test eax,0xffff0000
	jz shv64vu16lop4
	test eax,0xfff00000
	jz shv64vu16lop5
	test eax,0xff000000
	jz shv64vu16lop6
	test eax,0xf0000000
	jz shv64vu16lop7
	
	mov edx,eax
	shr edx,24
	movzx rdx,dl
	mov edx,dword [rdx*4+r8]
	mov dword [rdi],edx
	lea rdi,[rdi+4]
	jmp shv64vu16lop6

	align 16
shv64vu16lop7:
	mov edx,eax
	shr edx,24
	movzx rdx,dl
	mov dx,word [rdx*4+r8+2]
	mov word [rdi],dx
	lea rdi,[rdi+2]
	jmp shv64vu16lop6
	
	align 16
shv64vu16lop5:
	mov edx,eax
	shr edx,16
	movzx rdx,dl
	mov dx,word [rdx*4+r8+2]
	mov word [rdi],dx
	lea rdi,[rdi+2]
	jmp shv64vu16lop4
	
	align 16
shv64vu16lop3:
	mov edx,eax
	shr edx,8
	movzx rdx,dl
	mov dx,word [rdx*4+r8+2]
	mov word [rdi],dx
	lea rdi,[rdi+2]
	jmp shv64vu16lop2
	
	align 16
shv64vu16lop1:
	mov dx,word [rax*4+r8+2]
	mov word [rdi],dx
	lea rdi,[rdi+2]
	mov word [rdi],0
	jmp shv64vu16exit
	
	align 16
shv64vu16lop16:
	mov edx,eax
	shr edx,24
	movzx rdx,dl
	mov edx,dword [rdx*4+r8]
	mov dword [rdi],edx
	lea rdi,[rdi+4]
	align 16
shv64vu16lop14:
	mov edx,eax
	shr edx,16
	movzx rdx,dl
	mov edx,dword [rdx*4+r8]
	mov dword [rdi],edx
	lea rdi,[rdi+4]
	align 16
shv64vu16lop12:
	mov edx,eax
	shr edx,8
	movzx rdx,dl
	mov edx,dword [rdx*4+r8]
	mov dword [rdi],edx
	lea rdi,[rdi+4]
	align 16
shv64vu16lop10:
	movzx rdx,al
	mov edx,dword [rdx*4+r8]
	mov dword [rdi],edx
	lea rdi,[rdi+4]
	align 16
shv64vu16lop8:
	mov edx,esi
	shr edx,24
	movzx rdx,dl
	mov edx,dword [rdx*4+r8]
	mov dword [rdi],edx
	lea rdi,[rdi+4]
	align 16
shv64vu16lop6:
	mov edx,esi
	shr edx,16
	movzx rdx,dl
	mov edx,dword [rdx*4+r8]
	mov dword [rdi],edx
	lea rdi,[rdi+4]
	align 16
shv64vu16lop4:
	mov edx,esi
	shr edx,8
	movzx rdx,dl
	mov edx,dword [rdx*4+r8]
	mov dword [rdi],edx
	lea rdi,[rdi+4]
	align 16
shv64vu16lop2:
	mov edx,esi
	movzx rdx,dl
	mov edx,dword [rdx*4+r8]
	mov dword [rdi],edx
	lea rdi,[rdi+4]
	mov word [rdi],0
	align 16
shv64vu16exit:
	mov rax,rdi
	ret

;UTF16Char *MyString_StrHexVal32VUTF16(UTF16Char *oriStr, Int32 val)
;0 retAddr
;rdi oriStr
;rsi val
	align 16
MyString_StrHexVal32VUTF16:
_MyString_StrHexVal32VUTF16:
	lea r8,[_MyString_StrHexArrU16 addrpostfix]
	jmp shv64vu16lop8v

;UTF32Char *MyString_StrHexVal64VUTF32(UTF32Char *oriStr, Int64 val)
;0 retAddr
;rdi oriStr
;rsi val
	align 16
MyString_StrHexVal64VUTF32:
_MyString_StrHexVal64VUTF32:
	lea r8,[_MyString_StrHexArrU32 addrpostfix]
	mov rax,rsi
	shr rax,32
	jz shv64vu32lop8v
	test eax,0xfffffff0
	jz shv64vu32lop9
	test eax,0xffffff00
	jz shv64vu32lop10
	test eax,0xfffff000
	jz shv64vu32lop11
	test eax,0xffff0000
	jz shv64vu32lop12
	test eax,0xfff00000
	jz shv64vu32lop13
	test eax,0xff000000
	jz shv64vu32lop14
	test eax,0xf0000000
	jz shv64vu32lop15
	jmp shv64vu32lop16
	
	align 16
shv64vu32lop15:
	mov edx,eax
	shr edx,24
	movzx rdx,dl
	mov edx,dword [rdx*8+r8+4]
	mov dword [rdi],edx
	lea rdi,[rdi+4]
	jmp shv64vu32lop14
	
	align 16
shv64vu32lop13:
	mov edx,eax
	shr edx,16
	movzx rdx,dl
	mov edx,dword [rdx*8+r8+4]
	mov dword [rdi],edx
	lea rdi,[rdi+4]
	jmp shv64vu32lop12
	
	align 16
shv64vu32lop11:
	mov edx,eax
	shr edx,8
	movzx rdx,dl
	mov edx,dword [rdx*8+r8+4]
	mov dword [rdi],edx
	lea rdi,[rdi+4]
	jmp shv64vu32lop10
	
	align 16
shv64vu32lop9:
	mov edx,dword [rax*8+r8+4]
	mov dword [rdi],edx
	lea rdi,[rdi+4]
	jmp shv64vu32lop8
	
	align 16
shv64vu32lop8v:
	mov rax,rsi
	test eax,0xfffffff0
	jz shv64vu32lop1
	test eax,0xffffff00
	jz shv64vu32lop2
	test eax,0xfffff000
	jz shv64vu32lop3
	test eax,0xffff0000
	jz shv64vu32lop4
	test eax,0xfff00000
	jz shv64vu32lop5
	test eax,0xff000000
	jz shv64vu32lop6
	test eax,0xf0000000
	jz shv64vu32lop7
	
	mov edx,eax
	shr edx,24
	movzx rdx,dl
	mov rdx,qword [rdx*8+r8]
	mov qword [rdi],rdx
	lea rdi,[rdi+8]
	jmp shv64vu32lop6

	align 16
shv64vu32lop7:
	mov edx,eax
	shr edx,24
	movzx rdx,dl
	mov edx,dword [rdx*8+r8+4]
	mov dword [rdi],edx
	lea rdi,[rdi+4]
	jmp shv64vu32lop6
	
	align 16
shv64vu32lop5:
	mov edx,eax
	shr edx,16
	movzx rdx,dl
	mov edx,dword [rdx*8+r8+4]
	mov dword [rdi],edx
	lea rdi,[rdi+4]
	jmp shv64vu32lop4
	
	align 16
shv64vu32lop3:
	mov edx,eax
	shr edx,8
	movzx rdx,dl
	mov edx,dword [rdx*8+r8+4]
	mov dword [rdi],edx
	lea rdi,[rdi+4]
	jmp shv64vu32lop2
	
	align 16
shv64vu32lop1:
	mov edx,dword [rax*8+r8+4]
	mov dword [rdi],edx
	lea rdi,[rdi+4]
	mov word [rdi],0
	jmp shv64vu32exit
	
	align 16
shv64vu32lop16:
	mov edx,eax
	shr edx,24
	movzx rdx,dl
	mov rdx,qword [rdx*8+r8]
	mov qword [rdi],rdx
	lea rdi,[rdi+8]
	align 16
shv64vu32lop14:
	mov edx,eax
	shr edx,16
	movzx rdx,dl
	mov rdx,qword [rdx*8+r8]
	mov qword [rdi],rdx
	lea rdi,[rdi+8]
	align 16
shv64vu32lop12:
	mov edx,eax
	shr edx,8
	movzx rdx,dl
	mov rdx,qword [rdx*8+r8]
	mov qword [rdi],rdx
	lea rdi,[rdi+8]
	align 16
shv64vu32lop10:
	movzx rdx,al
	mov rdx,qword [rdx*8+r8]
	mov qword [rdi],rdx
	lea rdi,[rdi+8]
	align 16
shv64vu32lop8:
	mov edx,esi
	shr edx,24
	movzx rdx,dl
	mov rdx,qword [rdx*8+r8]
	mov qword [rdi],rdx
	lea rdi,[rdi+8]
	align 16
shv64vu32lop6:
	mov edx,esi
	shr edx,16
	movzx rdx,dl
	mov rdx,qword [rdx*8+r8]
	mov qword [rdi],rdx
	lea rdi,[rdi+8]
	align 16
shv64vu32lop4:
	mov edx,esi
	shr edx,8
	movzx rdx,dl
	mov rdx,qword [rdx*8+r8]
	mov qword [rdi],rdx
	lea rdi,[rdi+8]
	align 16
shv64vu32lop2:
	mov edx,esi
	movzx rdx,dl
	mov rdx,qword [rdx*8+r8]
	mov qword [rdi],rdx
	lea rdi,[rdi+8]
	mov dword [rdi],0
	align 16
shv64vu32exit:
	mov rax,rdi
	ret

;UTF32Char *MyString_StrHexVal32VUTF32(UTF32Char *oriStr, Int32 val)
;0 retAddr
;rdi oriStr
;rsi val
	align 16
MyString_StrHexVal32VUTF32:
_MyString_StrHexVal32VUTF32:
	lea r8,[_MyString_StrHexArrU32 addrpostfix]
	jmp shv64vu32lop8v

;UTF16Char *MyString_StrHexVal64UTF16(UTF16Char *oriStr, Int64 val)
;0 retAddr
;rdi oriStr
;rsi val
	align 16
MyString_StrHexVal64UTF16:
_MyString_StrHexVal64UTF16:
	mov rdx,rsi
	lea r8,[_MyString_StrHexArrU16 addrpostfix]
	mov word [rdi+32],0

	movzx rax,dl
	mov eax,dword [rax*4+r8]
	mov dword [rdi+28],eax
	shr rdx,8
	movzx eax,dl
	mov eax,dword [rax*4+r8]
	mov dword [rdi+24],eax
	shr rdx,8
	movzx eax,dl
	mov eax,dword [rax*4+r8]
	mov dword [rdi+20],eax
	shr rdx,8
	movzx eax,dl
	mov eax,dword [rax*4+r8]
	mov dword [rdi+16],eax
	shr rdx,8
	movzx eax,dl
	mov eax,dword [rax*4+r8]
	mov dword [rdi+12],eax
	shr rdx,8
	movzx eax,dl
	mov eax,dword [rax*4+r8]
	mov dword [rdi+8],eax
	shr rdx,8
	movzx eax,dl
	mov eax,dword [rax*4+r8]
	mov dword [rdi+4],eax
	shr rdx,8
	movzx eax,dl
	mov eax,dword [rax*4+r8]
	mov dword [rdi+0],eax

	lea rax,[rdi+32]
	ret

;UTF32Char *MyString_StrHexVal64UTF32(UTF32Char *oriStr, Int64 val)
;0 retAddr
;rdi oriStr
;rsi val
	align 16
MyString_StrHexVal64UTF32:
_MyString_StrHexVal64UTF32:
	mov rdx,rsi
	lea r8,[_MyString_StrHexArrU32 addrpostfix]
	mov dword [rdi+64],0

	movzx rax,dl
	mov rax,qword [rax*8+r8]
	mov qword [rdi+56],rax
	shr rdx,8
	movzx rax,dl
	mov rax,qword [rax*8+r8]
	mov qword [rdi+48],rax
	shr rdx,8
	movzx rax,dl
	mov rax,qword [rax*8+r8]
	mov qword [rdi+40],rax
	shr rdx,8
	movzx rax,dl
	mov rax,qword [rax*8+r8]
	mov qword [rdi+32],rax
	shr rdx,8
	movzx rax,dl
	mov rax,qword [rax*8+r8]
	mov qword [rdi+24],rax
	shr rdx,8
	movzx rax,dl
	mov rax,qword [rax*8+r8]
	mov qword [rdi+16],rax
	shr rdx,8
	movzx rax,dl
	mov rax,qword [rax*8+r8]
	mov qword [rdi+8],rax
	shr rdx,8
	movzx rax,dl
	mov rax,qword [rax*8+r8]
	mov qword [rdi+0],rax

	lea rax,[rdi+64]
	ret

;UTF16Char *MyString_StrHexVal32UTF16(UTF16Char *oriStr, Int32 val)
;0 retAddr
;rdi oriStr
;rsi val
	align 16
MyString_StrHexVal32UTF16:
_MyString_StrHexVal32UTF16:
	mov rdx,rsi
	lea r8,[_MyString_StrHexArrU16 addrpostfix]
	mov word [rdi+16],0

	movzx rax,dl
	mov eax,dword [rax*4+r8]
	mov dword [rdi+12],eax
	shr edx,8
	movzx eax,dl
	mov eax,dword [rax*4+r8]
	mov dword [rdi+8],eax
	shr edx,8
	movzx eax,dl
	mov eax,dword [rax*4+r8]
	mov dword [rdi+4],eax
	shr edx,8
	movzx eax,dl
	mov eax,dword [rax*4+r8]
	mov dword [rdi+0],eax

	lea rax,[rdi+16]
	ret

;UTF32Char *MyString_StrHexVal32UTF32(UTF32Char *oriStr, Int32 val)
;0 retAddr
;rdi oriStr
;rsi val
	align 16
MyString_StrHexVal32UTF32:
_MyString_StrHexVal32UTF32:
	mov rdx,rsi
	lea r8,[_MyString_StrHexArrU32 addrpostfix]
	mov dword [rdi+32],0

	movzx rax,dl
	mov rax,qword [rax*8+r8]
	mov qword [rdi+24],rax
	shr edx,8
	movzx rax,dl
	mov rax,qword [rax*8+r8]
	mov qword [rdi+16],rax
	shr edx,8
	movzx rax,dl
	mov rax,qword [rax*8+r8]
	mov qword [rdi+8],rax
	shr edx,8
	movzx rax,dl
	mov rax,qword [rax*8+r8]
	mov qword [rdi+0],rax

	lea rax,[rdi+32]
	ret

;UTF16Char *MyString_StrHexVal24UTF16(UTF16Char *oriStr, Int32 val)
;0 retAddr
;rdi oriStr
;rsi val
	align 16
MyString_StrHexVal24UTF16:
_MyString_StrHexVal24UTF16:
	mov rdx,rsi
	lea r8,[_MyString_StrHexArrU16 addrpostfix]
	mov word [rdi+12],0

	movzx rax,dl
	mov eax,dword [rax*4+r8]
	mov dword [rdi+8],eax
	shr edx,8
	movzx eax,dl
	mov eax,dword [rax*4+r8]
	mov dword [rdi+4],eax
	shr edx,8
	movzx eax,dl
	mov eax,dword [rax*4+r8]
	mov dword [rdi+0],eax

	lea rax,[rdi+12]
	ret

;UTF32Char *MyString_StrHexVal24UTF32(UTF32Char *oriStr, Int32 val)
;0 retAddr
;rdi oriStr
;rsi val
	align 16
MyString_StrHexVal24UTF32:
_MyString_StrHexVal24UTF32:
	mov rdx,rsi
	lea r8,[_MyString_StrHexArrU32 addrpostfix]
	mov dword [rdi+24],0

	movzx rax,dl
	mov rax,qword [rax*8+r8]
	mov qword [rdi+16],rax
	shr edx,8
	movzx rax,dl
	mov rax,qword [rax*8+r8]
	mov qword [rdi+8],rax
	shr edx,8
	movzx rax,dl
	mov rax,qword [rax*8+r8]
	mov qword [rdi+0],rax

	lea rax,[rdi+24]
	ret

;Int32 MyString_StrToInt32UTF8(const UTF8Char *intStr)
;0 retAddr
;rdi intStr
	align 16
MyString_StrToInt32UTF8:
_MyString_StrToInt32UTF8:
	xor rax,rax
	cmp byte [rdi], 0x2d
	jnz sti32u8lop
	lea rdi,[rdi+1]
	
	align 16
sti32u8lop5:
	movzx edx,byte [rdi]
	cmp edx,0x3a
	jnb sti32u8lop8
	cmp edx,0x30
	jb sti32u8lop8
	lea rdi,[rdi+1]
	lea eax,[eax*4+eax]
	lea eax,[eax*2+edx-0x30]
	jmp sti32u8lop5

	align 16
sti32u8lop6:
	neg rax
	jmp sti32u8lop4

	align 16
sti32u8lop8:
	test edx,edx
	jz sti32u8lop6
	xor rax,rax
	jmp sti32u8lop4

	align 16
sti32u8lop:
	movzx edx,byte [rdi]
	cmp edx,0x3a
	jnb sti32u8lop2
	cmp edx,0x30
	jb sti32u8lop2
	lea rdi,[rdi+1]
	lea eax,[eax*4+eax]
	lea eax,[eax*2+edx-0x30]
	jmp sti32u8lop

	align 16
sti32u8lop2:
	test edx,edx
	jz sti32u8lop4
	xor rax,rax
	align 16
sti32u8lop4:
	ret

;Int32 MyString_StrToInt32UTF16(const UTF16Char *intStr)
;0 retAddr
;rdi intStr
	align 16
MyString_StrToInt32UTF16:
_MyString_StrToInt32UTF16:
	xor rax,rax
	cmp word [rdi], 0x2d
	jnz sti32u16lop
	lea rdi,[rdi+2]
	
	align 16
sti32u16lop5:
	movzx edx,word [rdi]
	cmp edx,0x3a
	jnb sti32u16lop8
	cmp edx,0x30
	jb sti32u16lop8
	lea rdi,[rdi+2]
	lea eax,[eax*4+eax]
	lea eax,[eax*2+edx-0x30]
	jmp sti32u16lop5

	align 16
sti32u16lop6:
	neg rax
	jmp sti32u16lop4

	align 16
sti32u16lop8:
	test edx,edx
	jz sti32u16lop6
	xor rax,rax
	jmp sti32u16lop4

	align 16
sti32u16lop:
	movzx edx,word [rdi]
	cmp edx,0x3a
	jnb sti32u16lop2
	cmp edx,0x30
	jb sti32u16lop2
	lea rdi,[rdi+2]
	lea eax,[eax*4+eax]
	lea eax,[eax*2+edx-0x30]
	jmp sti32u16lop

	align 16
sti32u16lop2:
	test edx,edx
	jz sti32u16lop4
	xor rax,rax
	align 16
sti32u16lop4:
	ret

;Int32 MyString_StrToInt32UTF32(const UTF32Char *intStr)
;0 retAddr
;rdi intStr
	align 16
MyString_StrToInt32UTF32:
_MyString_StrToInt32UTF32:
	xor rax,rax
	cmp dword [rdi], 0x2d
	jnz sti32u32lop
	lea rdi,[rdi+4]
	
	align 16
sti32u32lop5:
	mov edx,dword [rdi]
	cmp edx,0x3a
	jnb sti32u32lop8
	cmp edx,0x30
	jb sti32u32lop8
	lea rdi,[rdi+4]
	lea eax,[eax*4+eax]
	lea eax,[eax*2+edx-0x30]
	jmp sti32u32lop5

	align 16
sti32u32lop6:
	neg rax
	jmp sti32u32lop4

	align 16
sti32u32lop8:
	test edx,edx
	jz sti32u32lop6
	xor rax,rax
	jmp sti32u32lop4

	align 16
sti32u32lop:
	mov edx,dword [rdi]
	cmp edx,0x3a
	jnb sti32u32lop2
	cmp edx,0x30
	jb sti32u32lop2
	lea rdi,[rdi+4]
	lea eax,[eax*4+eax]
	lea eax,[eax*2+edx-0x30]
	jmp sti32u32lop

	align 16
sti32u32lop2:
	test edx,edx
	jz sti32u32lop4
	xor rax,rax
	align 16
sti32u32lop4:
	ret

;Int64 MyString_StrToInt64UTF8(const UTF8Char *str1)
;0 retAddr
;rdi str1
	align 16
MyString_StrToInt64UTF8:
_MyString_StrToInt64UTF8:
	xor rax,rax
	xor rdx,rdx
	cmp word [rdi], 0x7830
	jz sti64u8loph
	cmp byte [rdi], 0x2d
	jnz sti64u8lop
	lea rdi,[rdi+1]
	
	align 16
sti64u8lop5:
	movzx edx,byte [rdi]
	cmp edx,0x3a
	jnb sti64u8lop8
	cmp edx,0x30
	jb sti64u8lop8
	lea rdi,[rdi+1]
	lea rax,[rax*4+rax]
	lea rax,[rax*2+rdx-0x30]
	jmp sti64u8lop5

	align 16
sti64u8lop6:
	xor rax,rax
	jmp sti64u8lop4

	align 16
sti64u8lop8:
	test edx,edx
	jnz sti64u8lop6
	neg rax
	jmp sti64u8lop4

	align 16
sti64u8loph:
	add rdi,2
	
	align 16
sti64u8loph2:
	movzx edx,byte [rdi]
	cmp edx,0x30
	jb sti64u8lop4
	cmp edx,0x3a
	jb sti64u8loph3
	cmp edx,0x41
	jb sti64u8loph4
	cmp edx,0x47
	jb sti64u8loph5
	cmp edx,0x61
	jb sti64u8loph4
	sub edx,0x20
	cmp edx,0x47
	jb sti64u8loph5
	align 16
sti64u8loph4:
	xor rax,rax
	jmp sti64u8lop4

	align 16
sti64u8loph3:
	inc rdi
	shl rax,4
	lea rax,[rax+rdx-0x30]
	jmp sti64u8loph2


	align 16
sti64u8loph5:
	inc rdi
	shl rax,4
	lea rax,[rax+rdx-0x37]
	jmp sti64u8loph2

	align 16
sti64u8lop:
	movzx edx,byte [rdi]
	cmp edx,0x3a
	jnb sti64u8lop2
	cmp edx,0x30
	jb sti64u8lop2
	lea rdi,[rdi+1]
	lea rax,[rax*4+rax]
	lea rax,[rax*2+rdx-0x30]
	jmp sti64u8lop

	align 16
sti64u8lop2:
	test edx,edx
	jz sti64u8lop4
	xor rax,rax
	align 16
sti64u8lop4:
	ret

;Int64 MyString_StrToInt64UTF16(const UTF16Char *str1)
;0 retAddr
;rdi str1
	align 16
MyString_StrToInt64UTF16:
_MyString_StrToInt64UTF16:
	xor rax,rax
	xor rdx,rdx
	cmp word [rdi], 0x2d
	jnz sti64u16lop
	lea rdi,[rdi+2]
	
	align 16
sti64u16lop5:
	movzx edx,word [rdi]
	cmp edx,0x3a
	jnb sti64u16lop8
	cmp edx,0x30
	jb sti64u16lop8
	lea rdi,[rdi+2]
	lea rax,[rax*4+rax]
	lea rax,[rax*2+rdx-0x30]
	jmp sti64u16lop5

	align 16
sti64u16lop6:
	xor rax,rax
	jmp sti64u16lop4

	align 16
sti64u16lop8:
	test edx,edx
	jnz sti64u16lop6
	neg rax
	jmp sti64u16lop4

	align 16
sti64u16lop:
	movzx edx,word [rdi]
	cmp edx,0x3a
	jnb sti64u16lop2
	cmp edx,0x30
	jb sti64u16lop2
	lea rdi,[rdi+2]
	lea rax,[rax*4+rax]
	lea rax,[rax*2+rdx-0x30]
	jmp sti64u16lop

	align 16
sti64u16lop2:
	test edx,edx
	jz sti64u16lop4
	xor rax,rax
	align 16
sti64u16lop4:
	ret

;Int64 MyString_StrToInt64UTF32(const UTF32Char *str1)
;0 retAddr
;rdi str1
	align 16
MyString_StrToInt64UTF32:
_MyString_StrToInt64UTF32:
	xor rax,rax
	xor rdx,rdx
	cmp dword [rdi], 0x2d
	jnz sti64u32lop
	lea rdi,[rdi+4]
	
	align 16
sti64u32lop5:
	mov edx,dword [rdi]
	cmp edx,0x3a
	jnb sti64u32lop8
	cmp edx,0x30
	jb sti64u32lop8
	lea rdi,[rdi+4]
	lea rax,[rax*4+rax]
	lea rax,[rax*2+rdx-0x30]
	jmp sti64u32lop5

	align 16
sti64u32lop6:
	xor rax,rax
	jmp sti64u32lop4

	align 16
sti64u32lop8:
	test edx,edx
	jnz sti64u32lop6
	neg rax
	jmp sti64u32lop4

	align 16
sti64u32lop:
	mov edx,dword [rdi]
	cmp edx,0x3a
	jnb sti64u32lop2
	cmp edx,0x30
	jb sti64u32lop2
	lea rdi,[rdi+4]
	lea rax,[rax*4+rax]
	lea rax,[rax*2+rdx-0x30]
	jmp sti64u32lop

	align 16
sti64u32lop2:
	test edx,edx
	jz sti64u32lop4
	xor rax,rax
	align 16
sti64u32lop4:
	ret

