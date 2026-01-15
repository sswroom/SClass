section .text

extern MyString_StrDigit100U8
extern MyString_StrDigit100U16
extern MyString_StrDigit100U32
extern MyString_StrHexArrU16

;global MyString_StrConcat
;global MyString_StrConcatS
;global MyString_StrConcatSUTF16
;global MyString_StrConcatUTF16
;global MyString_StrConcatUTF32

;global MyString_StrInt32UTF16
;global MyString_StrUInt32UTF16
;global MyString_StrInt64UTF16
;global MyString_StrUInt64UTF16

;global MyString_StrInt32
;global MyString_StrUInt32
;global MyString_StrInt64
;global MyString_StrUInt64

global MyString_StrCompare
global MyString_StrCompareICase
global MyString_StrCompareUTF16
global MyString_StrCompareICaseUTF16

global MyString_StrCharCnt
global MyString_StrCharCntUTF16
global MyString_StrCharCntUTF32

global MyString_StrHexVal64VUTF16
global MyString_StrHexVal64UTF16
global MyString_StrHexVal32VUTF16
global MyString_StrHexVal32UTF16
global MyString_StrHexVal24UTF16
global MyString_StrHexVal64VUTF32
global MyString_StrHexVal64UTF32
global MyString_StrHexVal32VUTF32
global MyString_StrHexVal32UTF32
global MyString_StrHexVal24UTF32

;global MyString_StrToInt32UTF8
;global MyString_StrToInt32UTF16
;global MyString_StrToInt32UTF32
global MyString_StrToInt64UTF8
global MyString_StrToInt64UTF16
global MyString_StrToInt64UTF32


;Char *MyString_StrConcat(Char *oriStr, const Char *strToJoin);
;rcx oriStr
;rdx strToJoin
	align 16
MyString_StrConcat:
	mov r9,rbx
	xor rbx,rbx
	align 16
sconcatlop:
	movzx eax,byte [rdx+rbx]
	mov byte [rcx+rbx],al
	lea rbx,[rbx+1]
	test al,al
	jnz sconcatlop
	lea rax,[rcx+rbx-1]
	mov rbx,r9
	ret

;Char *MyString_StrConcatS(Char *oriStr, const Char *strToJoin, IntOS buffSize);
;rcx oriStr
;rdx strToJoin
;r8 buffSize
	align 16
MyString_StrConcatS:
	mov r9,rbx
	xor rbx,rbx
	align 16
sconcatslop:
	movzx eax,byte [rdx+rbx]
	mov byte [rcx+rbx],al
	test al,al
	jz sconcatslop2
	lea rbx,[rbx+1]
	dec r8
	jnz sconcatslop
	mov byte [rcx+rbx],0
	align 16
sconcatslop2:
	lea rax,[rcx+rbx]
	mov rbx,r9
	ret


;UTF16Char *MyString_StrConcatSUTF16(UTF16Char *oriStr, const UTF16Char *strToJoin, IntOS buffSize)
;rcx oriStr
;rdx strToJoin
;r8 buffSize
	align 16
MyString_StrConcatSUTF16:
	cmp r8,0
	jle sconcatsu16lop3
	mov r9,rsi
	xor rsi,rsi
	align 16
sconcatsu16lop:
	mov eax,dword [rdx+rsi]
	mov word [rcx+rsi],ax
	test ax,ax
	jz sconcatsu16lop2
	lea rsi,[rsi+2]
	dec r8
	jnz sconcatsu16lop
	mov word [rcx+rsi],0
sconcatsu16lop2:
	lea rax,[rcx+rsi]
	mov rsi,r9
	ret

	align 16
sconcatsu16lop3:
	mov word [rcx],0
	mov rax,rcx
	ret

;UTF16Char *MyString_StrConcatUTF16(UTF16Char *oriStr, const UTF16Char *strToJoin)
;rcx oriStr
;rdx strToJoin
	align 16
MyString_StrConcatUTF16:
	mov r8,rsi
	xor rsi,rsi
	align 16
sconcatu16lop:
	mov eax,dword [rdx+rsi]
	mov word [rcx+rsi],ax
	lea rsi,[rsi+2]
	test ax,ax
	jnz sconcatu16lop
	lea rax,[rcx+rsi-2]
	mov rsi,r8
	ret

;UTF32Char *MyString_StrConcatUTF32(UTF32Char *oriStr, const UTF32Char *strToJoin)
;rcx oriStr
;rdx strToJoin
	align 16
MyString_StrConcatUTF32:
	mov r8,rsi
	xor rsi,rsi
	align 16
sconcatu32lop:
	mov eax,dword [rdx+rsi]
	mov dword [rcx+rsi],eax
	lea rsi,[rsi+4]
	test eax,eax
	jnz sconcatu32lop
	lea rax,[rcx+rsi-4]
	mov rsi,r8
	ret

;UTF16Char *MyString_StrInt32UTF16(UTF16Char *oriStr, Int32 val)
;rcx oriStr
;rdx val
	align 16
MyString_StrInt32UTF16:
	xor rax,rax
	mov eax,edx
	test eax,0x80000000
	jz si32u16lop
	neg eax
	mov word [rcx],'-'
	lea rcx,[rcx+2]
	jmp si32u16lop

;UTF16Char *MyString_StrUInt32UTF16(UTF16Char *oriStr, UInt32 val)
;0 retAddr
;rcx oriStr
;rdx val
	align 16
MyString_StrUInt32UTF16:
	xor rax,rax
	mov eax,edx
	align 16
si32u16lop:
	lea r8,[rel MyString_StrDigit100U16]
	cmp eax,10
	jb sui32wlop1
	cmp eax,100
	jb sui32wlop2
	push rsi
	push rdi
	mov rsi,100
	xor rdx,rdx
	cmp eax,1000
	jb sui32wlop3
	cmp eax,10000
	jb sui32wlop4
	cmp eax,100000
	jb sui32wlop5
	cmp eax,1000000
	jb sui32wlop6
	cmp eax,10000000
	jb sui32wlop7
	cmp eax,100000000
	jb sui32wlop8
	cmp eax,1000000000
	jb sui32wlop9
	jmp sui32wlop10
	
	align 16
sui32wlop1:
	movzx edx,word [rax*4+r8+2]
	mov dword [rcx],edx
	lea rax,[rcx+2]
	ret
	
	align 16
sui32wlop2:
	mov edx,dword [rax*4+r8]
	mov dword [rcx],edx
	mov word [rcx+4],0
	lea rax,[rcx+4]
	ret

	align 16
sui32wlop3:
	div esi
	mov di,word [rax*4+r8+2]
	mov word [rcx],di
	mov edi,dword [rdx*4+r8]
	mov dword [rcx+2],edi
	mov word [rcx+6],0
	lea rax,[rcx+6]
	jmp sui32wexit

	align 16
sui32wlop4:
	div esi
	mov edi,dword [rax*4+r8]
	mov dword [rcx],edi
	mov edi,dword [rdx*4+r8]
	mov dword [rcx+4],edi
	mov word [rcx+8],0
	lea rax,[rcx+8]
	jmp sui32wexit


	align 16
sui32wlop5:
	div esi
	mov edi,dword [rdx*4+r8]
	xor rdx,rdx
	mov dword [rcx+6],edi
	div esi
	mov edi,dword [rdx*4+r8]
	mov dword [rcx+2],edi
	mov di,word [rax*4+r8+2]
	mov word [rcx],di
	mov word [rcx+10],0
	lea rax,[rcx+10]
	jmp sui32wexit

	align 16
sui32wlop6:
	div esi
	mov edi,dword [rdx*4+r8]
	xor rdx,rdx
	mov dword [rcx+8],edi
	div esi
	mov edi,dword [rdx*4+r8]
	mov dword [rcx+4],edi
	mov edi,dword [rax*4+r8]
	mov dword [rcx],edi
	mov word [rcx+12],0
	lea rax,[rcx+12]
	jmp sui32wexit

	align 16
sui32wlop7:
	div esi
	mov edi,dword [rdx*4+r8]
	xor rdx,rdx
	mov dword [rcx+10],edi
	div esi
	mov edi,dword [rdx*4+r8]
	xor rdx,rdx
	mov dword [rcx+6],edi
	div esi
	mov edi,dword [rdx*4+r8]
	mov dword [rcx+2],edi
	mov di,word [rax*4+r8+2]
	mov word [rcx],di
	mov word [rcx+14],0
	lea rax,[rcx+14]
	jmp sui32wexit

	align 16
sui32wlop8:
	div esi
	mov edi,dword [rdx*4+r8]
	xor rdx,rdx
	mov dword [rcx+12],edi
	div esi
	mov edi,dword [rdx*4+r8]
	xor rdx,rdx
	mov dword [rcx+8],edi
	div esi
	mov edi,dword [rdx*4+r8]
	mov dword [rcx+4],edi
	mov edi,dword [rax*4+r8]
	mov dword [rcx],edi
	mov word [rcx+16],0
	lea rax,[rcx+16]
	jmp sui32wexit

	align 16
sui32wlop9:
	div esi
	mov edi,dword [rdx*4+r8]
	xor rdx,rdx
	mov dword [rcx+14],edi
	div esi
	mov edi,dword [rdx*4+r8]
	xor rdx,rdx
	mov dword [rcx+10],edi
	div esi
	mov edi,dword [rdx*4+r8]
	xor rdx,rdx
	mov dword [rcx+6],edi
	div esi
	mov edi,dword [rdx*4+r8]
	mov dword [rcx+2],edi
	mov di,word [rax*4+r8+2]
	mov word [rcx],di
	mov word [rcx+18],0
	lea rax,[rcx+18]
	jmp sui32wexit

	align 16
sui32wlop10:
	div esi
	mov edi,dword [rdx*4+r8]
	xor rdx,rdx
	mov dword [rcx+16],edi
	div esi
	mov edi,dword [rdx*4+r8]
	xor rdx,rdx
	mov dword [rcx+12],edi
	div esi
	mov edi,dword [rdx*4+r8]
	xor rdx,rdx
	mov dword [rcx+8],edi
	div esi
	mov edi,dword [rdx*4+r8]
	mov dword [rcx+4],edi
	mov edi,dword [rax*4+r8]
	mov dword [rcx],edi
	mov word [rcx+20],0
	lea rax,[rcx+20]
	jmp sui32wexit


	align 16
sui32wexit:
	pop rdi
	pop rsi
	ret

;WChar *MyString_StrInt64UTF16(WChar *oriStr, Int64 val)
;-12 edi
;-8 esi
;-4 ebx
;0 retAddr
;rcx oriStr
;rdx val
	align 16
MyString_StrInt64UTF16:
	mov rax,rdx
	cmp rax,0
	jge si64u16lop0
	neg rax
	mov word [rcx],'-'
	lea rcx,[rcx+2]
	jmp si64u16lop0
	
;WChar *MyString_StrUInt64UTF16(WChar *oriStr, Int64 val)
;-12 edi
;-8 esi
;-4 ebx
;0 retAddr
;rcx oriStr
;rdx val
	align 16
MyString_StrUInt64UTF16:
	mov rax,rdx
	align 16
si64u16lop0:
	mov rdx,0xffffffff
	cmp rax,rdx
	jbe si32u16lop
	lea r8,[rel MyString_StrDigit100U16]
	push rsi
	push rdi
	mov rsi,100
	xor rdx,rdx
	mov rdi,10000000000
	cmp rax,rdi
	jb si64wlop10
	mov rdi,100000000000
	cmp rax,rdi
	jb si64wlop11
	mov rdi,1000000000000
	cmp rax,rdi
	jb si64wlop12
	mov rdi,10000000000000
	cmp rax,rdi
	jb si64wlop13
	mov rdi,100000000000000
	cmp rax,rdi
	jb si64wlop14
	mov rdi,1000000000000000
	cmp rax,rdi
	jb si64wlop15
	mov rdi,10000000000000000
	cmp rax,rdi
	jb si64wlop16
	mov rdi,100000000000000000
	cmp rax,rdi
	jb si64wlop17
	mov rdi,1000000000000000000
	cmp rax,rdi
	jb si64wlop18
	mov rdi,10000000000000000000
	cmp rax,rdi
	jb si64wlop19
	jmp si64wlop20

	align 16
si64wlop10:
	div rsi
	mov edi,dword [rdx*4+r8]
	xor rdx,rdx
	mov dword [rcx+16],edi
	div esi
	mov edi,dword [rdx*4+r8]
	xor rdx,rdx
	mov dword [rcx+12],edi
	div esi
	mov edi,dword [rdx*4+r8]
	xor rdx,rdx
	mov dword [rcx+8],edi
	div esi
	mov edi,dword [rdx*4+r8]
	xor rdx,rdx
	mov dword [rcx+4],edi
	mov edi,dword [rax*4+r8]
	mov dword [rcx],edi
	mov word [rcx+20],0
	lea rax,[rcx+20]
	jmp si64wexit

	align 16
si64wlop11:
	div rsi
	mov edi,dword [rdx*4+r8]
	xor rdx,rdx
	mov dword [rcx+18],edi
	div esi
	mov edi,dword [rdx*4+r8]
	xor rdx,rdx
	mov dword [rcx+14],edi
	div esi
	mov edi,dword [rdx*4+r8]
	xor rdx,rdx
	mov dword [rcx+10],edi
	div esi
	mov edi,dword [rdx*4+r8]
	xor rdx,rdx
	mov dword [rcx+6],edi
	div esi
	mov edi,dword [rdx*4+r8]
	mov dword [rcx+2],edi
	mov di,word [rax*4+r8+2]
	mov word [rcx],di
	mov word [rcx+22],0
	lea rax,[rcx+22]
	jmp si64wexit

	align 16
si64wlop12:
	div rsi
	mov edi,dword [rdx*4+r8]
	xor rdx,rdx
	mov dword [rcx+20],edi
	div rsi
	mov edi,dword [rdx*4+r8]
	xor rdx,rdx
	mov dword [rcx+16],edi
	div esi
	mov edi,dword [rdx*4+r8]
	xor rdx,rdx
	mov dword [rcx+12],edi
	div esi
	mov edi,dword [rdx*4+r8]
	xor rdx,rdx
	mov dword [rcx+8],edi
	div esi
	mov edi,dword [rdx*4+r8]
	mov dword [rcx+4],edi
	mov edi,dword [rax*4+r8]
	mov dword [rcx],edi
	mov word [rcx+24],0
	lea rax,[rcx+24]
	jmp si64wexit

	align 16
si64wlop13:
	div rsi
	mov edi,dword [rdx*4+r8]
	xor rdx,rdx
	mov dword [rcx+22],edi
	div rsi
	mov edi,dword [rdx*4+r8]
	xor rdx,rdx
	mov dword [rcx+18],edi
	div esi
	mov edi,dword [rdx*4+r8]
	xor rdx,rdx
	mov dword [rcx+14],edi
	div esi
	mov edi,dword [rdx*4+r8]
	xor rdx,rdx
	mov dword [rcx+10],edi
	div esi
	mov edi,dword [rdx*4+r8]
	xor rdx,rdx
	mov dword [rcx+6],edi
	div esi
	mov edi,dword [rdx*4+r8]
	mov dword [rcx+2],edi
	mov di,word [rax*4+r8+2]
	mov word [rcx],di
	mov word [rcx+26],0
	lea rax,[rcx+26]
	jmp si64wexit

	align 16
si64wlop14:
	div rsi
	mov edi,dword [rdx*4+r8]
	xor rdx,rdx
	mov dword [rcx+24],edi
	div rsi
	mov edi,dword [rdx*4+r8]
	xor rdx,rdx
	mov dword [rcx+20],edi
	div rsi
	mov edi,dword [rdx*4+r8]
	xor rdx,rdx
	mov dword [rcx+16],edi
	div esi
	mov edi,dword [rdx*4+r8]
	xor rdx,rdx
	mov dword [rcx+12],edi
	div esi
	mov edi,dword [rdx*4+r8]
	xor rdx,rdx
	mov dword [rcx+8],edi
	div esi
	mov edi,dword [rdx*4+r8]
	mov dword [rcx+4],edi
	mov edi,dword [rax*4+r8]
	mov dword [rcx],edi
	mov word [rcx+28],0
	lea rax,[rcx+28]
	jmp si64wexit

	align 16
si64wlop15:
	div rsi
	mov edi,dword [rdx*4+r8]
	xor rdx,rdx
	mov dword [rcx+26],edi
	div rsi
	mov edi,dword [rdx*4+r8]
	xor rdx,rdx
	mov dword [rcx+22],edi
	div rsi
	mov edi,dword [rdx*4+r8]
	xor rdx,rdx
	mov dword [rcx+18],edi
	div esi
	mov edi,dword [rdx*4+r8]
	xor rdx,rdx
	mov dword [rcx+14],edi
	div esi
	mov edi,dword [rdx*4+r8]
	xor rdx,rdx
	mov dword [rcx+10],edi
	div esi
	mov edi,dword [rdx*4+r8]
	xor rdx,rdx
	mov dword [rcx+6],edi
	div esi
	mov edi,dword [rdx*4+r8]
	mov dword [rcx+2],edi
	mov di,word [rax*4+r8+2]
	mov word [rcx],di
	mov word [rcx+30],0
	lea rax,[rcx+30]
	jmp si64wexit

	align 16
si64wlop16:
	div rsi
	mov edi,dword [rdx*4+r8]
	xor rdx,rdx
	mov dword [rcx+28],edi
	div rsi
	mov edi,dword [rdx*4+r8]
	xor rdx,rdx
	mov dword [rcx+24],edi
	div rsi
	mov edi,dword [rdx*4+r8]
	xor rdx,rdx
	mov dword [rcx+20],edi
	div rsi
	mov edi,dword [rdx*4+r8]
	xor rdx,rdx
	mov dword [rcx+16],edi
	div esi
	mov edi,dword [rdx*4+r8]
	xor rdx,rdx
	mov dword [rcx+12],edi
	div esi
	mov edi,dword [rdx*4+r8]
	xor rdx,rdx
	mov dword [rcx+8],edi
	div esi
	mov edi,dword [rdx*4+r8]
	mov dword [rcx+4],edi
	mov edi,dword [rax*4+r8]
	mov dword [rcx],edi
	mov word [rcx+32],0
	lea rax,[rcx+32]
	jmp si64wexit

	align 16
si64wlop17:
	div rsi
	mov edi,dword [rdx*4+r8]
	xor rdx,rdx
	mov dword [rcx+30],edi
	div rsi
	mov edi,dword [rdx*4+r8]
	xor rdx,rdx
	mov dword [rcx+26],edi
	div rsi
	mov edi,dword [rdx*4+r8]
	xor rdx,rdx
	mov dword [rcx+22],edi
	div rsi
	mov edi,dword [rdx*4+r8]
	xor rdx,rdx
	mov dword [rcx+18],edi
	div esi
	mov edi,dword [rdx*4+r8]
	xor rdx,rdx
	mov dword [rcx+14],edi
	div esi
	mov edi,dword [rdx*4+r8]
	xor rdx,rdx
	mov dword [rcx+10],edi
	div esi
	mov edi,dword [rdx*4+r8]
	xor rdx,rdx
	mov dword [rcx+6],edi
	div esi
	mov edi,dword [rdx*4+r8]
	mov dword [rcx+2],edi
	mov di,word [rax*4+r8+2]
	mov word [rcx],di
	mov word [rcx+34],0
	lea rax,[rcx+34]
	jmp si64wexit

	align 16
si64wlop18:
	div rsi
	mov edi,dword [rdx*4+r8]
	xor rdx,rdx
	mov dword [rcx+32],edi
	div rsi
	mov edi,dword [rdx*4+r8]
	xor rdx,rdx
	mov dword [rcx+28],edi
	div rsi
	mov edi,dword [rdx*4+r8]
	xor rdx,rdx
	mov dword [rcx+24],edi
	div rsi
	mov edi,dword [rdx*4+r8]
	xor rdx,rdx
	mov dword [rcx+20],edi
	div rsi
	mov edi,dword [rdx*4+r8]
	xor rdx,rdx
	mov dword [rcx+16],edi
	div esi
	mov edi,dword [rdx*4+r8]
	xor rdx,rdx
	mov dword [rcx+12],edi
	div esi
	mov edi,dword [rdx*4+r8]
	xor rdx,rdx
	mov dword [rcx+8],edi
	div esi
	mov edi,dword [rdx*4+r8]
	mov dword [rcx+4],edi
	mov edi,dword [rax*4+r8]
	mov dword [rcx],edi
	mov word [rcx+36],0
	lea rax,[rcx+36]
	jmp si64wexit

	align 16
si64wlop19:
	div rsi
	mov edi,dword [rdx*4+r8]
	xor rdx,rdx
	mov dword [rcx+34],edi
	div rsi
	mov edi,dword [rdx*4+r8]
	xor rdx,rdx
	mov dword [rcx+30],edi
	div rsi
	mov edi,dword [rdx*4+r8]
	xor rdx,rdx
	mov dword [rcx+26],edi
	div rsi
	mov edi,dword [rdx*4+r8]
	xor rdx,rdx
	mov dword [rcx+22],edi
	div rsi
	mov edi,dword [rdx*4+r8]
	xor rdx,rdx
	mov dword [rcx+18],edi
	div esi
	mov edi,dword [rdx*4+r8]
	xor rdx,rdx
	mov dword [rcx+14],edi
	div esi
	mov edi,dword [rdx*4+r8]
	xor rdx,rdx
	mov dword [rcx+10],edi
	div esi
	mov edi,dword [rdx*4+r8]
	xor rdx,rdx
	mov dword [rcx+6],edi
	div esi
	mov edi,dword [rdx*4+r8]
	mov dword [rcx+2],edi
	mov di,word [rax*4+r8+2]
	mov word [rcx],di
	mov word [rcx+38],0
	lea rax,[rcx+38]
	jmp si64wexit

	align 16
si64wlop20:
	div rsi
	mov edi,dword [rdx*4+r8]
	xor rdx,rdx
	mov dword [rcx+36],edi
	div rsi
	mov edi,dword [rdx*4+r8]
	xor rdx,rdx
	mov dword [rcx+32],edi
	div rsi
	mov edi,dword [rdx*4+r8]
	xor rdx,rdx
	mov dword [rcx+28],edi
	div rsi
	mov edi,dword [rdx*4+r8]
	xor rdx,rdx
	mov dword [rcx+24],edi
	div rsi
	mov edi,dword [rdx*4+r8]
	xor rdx,rdx
	mov dword [rcx+20],edi
	div rsi
	mov edi,dword [rdx*4+r8]
	xor rdx,rdx
	mov dword [rcx+16],edi
	div esi
	mov edi,dword [rdx*4+r8]
	xor rdx,rdx
	mov dword [rcx+12],edi
	div esi
	mov edi,dword [rdx*4+r8]
	xor rdx,rdx
	mov dword [rcx+8],edi
	div esi
	mov edi,dword [rdx*4+r8]
	mov dword [rcx+4],edi
	mov edi,dword [rax*4+r8]
	mov dword [rcx],edi
	mov word [rcx+40],0
	lea rax,[rcx+40]
;	jmp si64wexit

	align 16
si64wexit:
	pop rdi
	pop rsi
	ret

;Char *MyString_StrInt32(Char *oriStr, Int32 val)
;rcx oriStr
;rdx val
	align 16
MyString_StrInt32:
	xor rax,rax
	mov eax,edx
	test eax,0x80000000
	jz si32lop
	neg eax
	mov byte [rcx],'-'
	lea rcx,[rcx+1]
	jmp si32lop

;Char *MyString_StrUInt32(Char *oriStr, UInt32 val)
;0 retAddr
;rcx oriStr
;rdx val
	align 16
MyString_StrUInt32:
	xor rax,rax
	mov eax,edx
	align 16
si32lop:
	lea r8,[rel MyString_StrDigit100U8]
	cmp eax,10
	jb sui32lop1
	cmp eax,100
	jb sui32lop2
	push rsi
	push rdi
	mov rsi,100
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
	mov word [rcx],dx
	lea rax,[rcx+1]
	ret
	
	align 16
sui32lop2:
	mov dx,word [rax*2+r8]
	mov word [rcx],dx
	mov byte [rcx+2],0
	lea rax,[rcx+2]
	ret

	align 16
sui32lop3:
	div esi
	mov r9b,byte [rax*2+r8+1]
	mov byte [rcx],r9b
	mov di,word [rdx*2+r8]
	mov word [rcx+1],di
	mov byte [rcx+3],0
	lea rax,[rcx+3]
	jmp sui32exit

	align 16
sui32lop4:
	div esi
	mov di,word [rax*2+r8]
	mov word [rcx],di
	mov di,word [rdx*2+r8]
	mov word [rcx+2],di
	mov byte [rcx+4],0
	lea rax,[rcx+4]
	jmp sui32exit

	align 16
sui32lop5:
	div esi
	mov di,word [rdx*2+r8]
	xor rdx,rdx
	mov word [rcx+3],di
	div esi
	mov di,word [rdx*2+r8]
	mov word [rcx+1],di
	mov r9b,byte [rax*2+r8+1]
	mov byte [rcx],r9b
	mov byte [rcx+5],0
	lea rax,[rcx+5]
	jmp sui32exit

	align 16
sui32lop6:
	div esi
	mov di,word [rdx*2+r8]
	xor rdx,rdx
	mov word [rcx+4],di
	div esi
	mov di,word [rdx*2+r8]
	mov word [rcx+2],di
	mov di,word [rax*2+r8]
	mov word [rcx],di
	mov byte [rcx+6],0
	lea rax,[rcx+6]
	jmp sui32exit

	align 16
sui32lop7:
	div esi
	mov di,word [rdx*2+r8]
	xor rdx,rdx
	mov word [rcx+5],di
	div esi
	mov di,word [rdx*2+r8]
	xor rdx,rdx
	mov word [rcx+3],di
	div esi
	mov di,word [rdx*2+r8]
	mov word [rcx+1],di
	mov r9b,byte [rax*2+r8+1]
	mov byte [rcx],r9b
	mov byte [rcx+7],0
	lea rax,[rcx+7]
	jmp sui32exit

	align 16
sui32lop8:
	div esi
	mov di,word [rdx*2+r8]
	xor rdx,rdx
	mov word [rcx+6],di
	div esi
	mov di,word [rdx*2+r8]
	xor rdx,rdx
	mov word [rcx+4],di
	div esi
	mov di,word [rdx*2+r8]
	mov word [rcx+2],di
	mov di,word [rax*2+r8]
	mov word [rcx],di
	mov byte [rcx+8],0
	lea rax,[rcx+8]
	jmp sui32exit

	align 16
sui32lop9:
	div esi
	mov di,word [rdx*2+r8]
	xor rdx,rdx
	mov word [rcx+7],di
	div esi
	mov di,word [rdx*2+r8]
	xor rdx,rdx
	mov word [rcx+5],di
	div esi
	mov di,word [rdx*2+r8]
	xor rdx,rdx
	mov word [rcx+3],di
	div esi
	mov di,word [rdx*2+r8]
	mov word [rcx+1],di
	mov r9b,byte [rax*2+r8+1]
	mov byte [rcx],r9b
	mov byte [rcx+9],0
	lea rax,[rcx+9]
	jmp sui32exit

	align 16
sui32lop10:
	div esi
	mov di,word [rdx*2+r8]
	xor rdx,rdx
	mov word [rcx+8],di
	div esi
	mov di,word [rdx*2+r8]
	xor rdx,rdx
	mov word [rcx+6],di
	div esi
	mov di,word [rdx*2+r8]
	xor rdx,rdx
	mov word [rcx+4],di
	div esi
	mov di,word [rdx*2+r8]
	mov word [rcx+2],di
	mov di,word [rax*2+r8]
	mov word [rcx],di
	mov byte [rcx+10],0
	lea rax,[rcx+10]
	jmp sui32exit


	align 16
sui32exit:
	pop rdi
	pop rsi
	ret

;Char *MyString_StrInt64(Char *oriStr, Int64 val)
;-12 edi
;-8 esi
;-4 ebx
;0 retAddr
;rcx oriStr
;rdx val
	align 16
MyString_StrInt64:
	mov rax,rdx
	cmp rax,0
	jge si64lop0
	neg rax
	mov byte [rcx],'-'
	lea rcx,[rcx+1]
	jmp si64lop0
	
;Char *MyString_StrUInt64(Char *oriStr, Int64 val)
;-12 edi
;-8 esi
;-4 ebx
;0 retAddr
;rcx oriStr
;rdx val
	align 16
MyString_StrUInt64:
	mov rax,rdx
	align 16
si64lop0:
	mov rdx,0xffffffff
	cmp rax,rdx
	jbe si32lop
	lea r8,[rel MyString_StrDigit100U8]
	push rsi
	push rdi
	mov rsi,100
	xor rdx,rdx
	mov rdi,10000000000
	cmp rax,rdi
	jb si64lop10
	mov rdi,100000000000
	cmp rax,rdi
	jb si64lop11
	mov rdi,1000000000000
	cmp rax,rdi
	jb si64lop12
	mov rdi,10000000000000
	cmp rax,rdi
	jb si64lop13
	mov rdi,100000000000000
	cmp rax,rdi
	jb si64lop14
	mov rdi,1000000000000000
	cmp rax,rdi
	jb si64lop15
	mov rdi,10000000000000000
	cmp rax,rdi
	jb si64lop16
	mov rdi,100000000000000000
	cmp rax,rdi
	jb si64lop17
	mov rdi,1000000000000000000
	cmp rax,rdi
	jb si64lop18
	mov rdi,10000000000000000000
	cmp rax,rdi
	jb si64lop19
	jmp si64lop20

	align 16
si64lop10:
	div rsi
	mov di,word [rdx*2+r8]
	xor rdx,rdx
	mov word [rcx+8],di
	div esi
	mov di,word [rdx*2+r8]
	xor rdx,rdx
	mov word [rcx+6],di
	div esi
	mov di,word [rdx*2+r8]
	xor rdx,rdx
	mov word [rcx+4],di
	div esi
	mov di,word [rdx*2+r8]
	xor rdx,rdx
	mov word [rcx+2],di
	mov di,word [rax*2+r8]
	mov word [rcx],di
	mov byte [rcx+10],0
	lea rax,[rcx+10]
	jmp si64exit

	align 16
si64lop11:
	div rsi
	mov di,word [rdx*2+r8]
	xor rdx,rdx
	mov word [rcx+9],di
	div esi
	mov di,word [rdx*2+r8]
	xor rdx,rdx
	mov word [rcx+7],di
	div esi
	mov di,word [rdx*2+r8]
	xor rdx,rdx
	mov word [rcx+5],di
	div esi
	mov di,word [rdx*2+r8]
	xor rdx,rdx
	mov word [rcx+3],di
	div esi
	mov di,word [rdx*2+r8]
	mov word [rcx+1],di
	mov r9b,byte [rax*2+r8+1]
	mov byte [rcx],r9b
	mov byte [rcx+11],0
	lea rax,[rcx+11]
	jmp si64exit

	align 16
si64lop12:
	div rsi
	mov di,word [rdx*2+r8]
	xor rdx,rdx
	mov word [rcx+10],di
	div rsi
	mov di,word [rdx*2+r8]
	xor rdx,rdx
	mov word [rcx+8],di
	div esi
	mov di,word [rdx*2+r8]
	xor rdx,rdx
	mov word [rcx+6],di
	div esi
	mov di,word [rdx*2+r8]
	xor rdx,rdx
	mov word [rcx+4],di
	div esi
	mov di,word [rdx*2+r8]
	mov word [rcx+2],di
	mov di,word [rax*2+r8]
	mov word [rcx],di
	mov byte [rcx+12],0
	lea rax,[rcx+12]
	jmp si64exit

	align 16
si64lop13:
	div rsi
	mov di,word [rdx*2+r8]
	xor rdx,rdx
	mov word [rcx+11],di
	div rsi
	mov di,word [rdx*2+r8]
	xor rdx,rdx
	mov word [rcx+9],di
	div esi
	mov di,word [rdx*2+r8]
	xor rdx,rdx
	mov word [rcx+7],di
	div esi
	mov di,word [rdx*2+r8]
	xor rdx,rdx
	mov word [rcx+5],di
	div esi
	mov di,word [rdx*2+r8]
	xor rdx,rdx
	mov word [rcx+3],di
	div esi
	mov di,word [rdx*2+r8]
	mov word [rcx+1],di
	mov r9b,byte [rax*2+r8+1]
	mov byte [rcx],r9b
	mov word [rcx+13],0
	lea rax,[rcx+13]
	jmp si64exit

	align 16
si64lop14:
	div rsi
	mov di,word [rdx*2+r8]
	xor rdx,rdx
	mov word [rcx+12],di
	div rsi
	mov di,word [rdx*2+r8]
	xor rdx,rdx
	mov word [rcx+10],di
	div rsi
	mov di,word [rdx*2+r8]
	xor rdx,rdx
	mov word [rcx+8],di
	div esi
	mov di,word [rdx*2+r8]
	xor rdx,rdx
	mov word [rcx+6],di
	div esi
	mov di,word [rdx*2+r8]
	xor rdx,rdx
	mov word [rcx+4],di
	div esi
	mov di,word [rdx*2+r8]
	mov word [rcx+2],di
	mov di,word [rax*2+r8]
	mov word [rcx],di
	mov byte [rcx+14],0
	lea rax,[rcx+14]
	jmp si64exit

	align 16
si64lop15:
	div rsi
	mov di,word [rdx*2+r8]
	xor rdx,rdx
	mov word [rcx+13],di
	div rsi
	mov di,word [rdx*2+r8]
	xor rdx,rdx
	mov word [rcx+11],di
	div rsi
	mov di,word [rdx*2+r8]
	xor rdx,rdx
	mov word [rcx+9],di
	div esi
	mov di,word [rdx*2+r8]
	xor rdx,rdx
	mov word [rcx+7],di
	div esi
	mov di,word [rdx*2+r8]
	xor rdx,rdx
	mov word [rcx+5],di
	div esi
	mov di,word [rdx*2+r8]
	xor rdx,rdx
	mov word [rcx+3],di
	div esi
	mov di,word [rdx*2+r8]
	mov word [rcx+1],di
	mov r9b,byte [rax*2+r8+1]
	mov byte [rcx],r9b
	mov byte [rcx+15],0
	lea rax,[rcx+15]
	jmp si64exit

	align 16
si64lop16:
	div rsi
	mov di,word [rdx*2+r8]
	xor rdx,rdx
	mov word [rcx+14],di
	div rsi
	mov di,word [rdx*2+r8]
	xor rdx,rdx
	mov word [rcx+12],di
	div rsi
	mov di,word [rdx*2+r8]
	xor rdx,rdx
	mov word [rcx+10],di
	div rsi
	mov di,word [rdx*2+r8]
	xor rdx,rdx
	mov word [rcx+8],di
	div esi
	mov di,word [rdx*2+r8]
	xor rdx,rdx
	mov word [rcx+6],di
	div esi
	mov di,word [rdx*2+r8]
	xor rdx,rdx
	mov word [rcx+4],di
	div esi
	mov di,word [rdx*2+r8]
	mov word [rcx+2],di
	mov di,word [rax*2+r8]
	mov word [rcx],di
	mov byte [rcx+16],0
	lea rax,[rcx+16]
	jmp si64exit

	align 16
si64lop17:
	div rsi
	mov di,word [rdx*2+r8]
	xor rdx,rdx
	mov word [rcx+15],di
	div rsi
	mov di,word [rdx*2+r8]
	xor rdx,rdx
	mov word [rcx+13],di
	div rsi
	mov di,word [rdx*2+r8]
	xor rdx,rdx
	mov word [rcx+11],di
	div rsi
	mov di,word [rdx*2+r8]
	xor rdx,rdx
	mov word [rcx+9],di
	div esi
	mov di,word [rdx*2+r8]
	xor rdx,rdx
	mov word [rcx+7],di
	div esi
	mov di,word [rdx*2+r8]
	xor rdx,rdx
	mov word [rcx+5],di
	div esi
	mov di,word [rdx*2+r8]
	xor rdx,rdx
	mov word [rcx+3],di
	div esi
	mov di,word [rdx*2+r8]
	mov word [rcx+1],di
	mov r9b,byte [rax*2+r8+1]
	mov byte [rcx],r9b
	mov byte [rcx+17],0
	lea rax,[rcx+17]
	jmp si64exit

	align 16
si64lop18:
	div rsi
	mov di,word [rdx*2+r8]
	xor rdx,rdx
	mov word [rcx+16],di
	div rsi
	mov di,word [rdx*2+r8]
	xor rdx,rdx
	mov word [rcx+14],di
	div rsi
	mov di,word [rdx*2+r8]
	xor rdx,rdx
	mov word [rcx+12],di
	div rsi
	mov di,word [rdx*2+r8]
	xor rdx,rdx
	mov word [rcx+10],di
	div rsi
	mov di,word [rdx*2+r8]
	xor rdx,rdx
	mov word [rcx+8],di
	div esi
	mov di,word [rdx*2+r8]
	xor rdx,rdx
	mov word [rcx+6],di
	div esi
	mov di,word [rdx*2+r8]
	xor rdx,rdx
	mov word [rcx+4],di
	div esi
	mov di,word [rdx*2+r8]
	mov word [rcx+2],di
	mov di,word [rax*2+r8]
	mov word [rcx],di
	mov byte [rcx+18],0
	lea rax,[rcx+18]
	jmp si64exit

	align 16
si64lop19:
	div rsi
	mov di,word [rdx*2+r8]
	xor rdx,rdx
	mov word [rcx+17],di
	div rsi
	mov di,word [rdx*2+r8]
	xor rdx,rdx
	mov word [rcx+15],di
	div rsi
	mov di,word [rdx*2+r8]
	xor rdx,rdx
	mov word [rcx+13],di
	div rsi
	mov di,word [rdx*2+r8]
	xor rdx,rdx
	mov word [rcx+11],di
	div rsi
	mov di,word [rdx*2+r8]
	xor rdx,rdx
	mov word [rcx+9],di
	div esi
	mov di,word [rdx*2+r8]
	xor rdx,rdx
	mov word [rcx+7],di
	div esi
	mov di,word [rdx*2+r8]
	xor rdx,rdx
	mov word [rcx+5],di
	div esi
	mov di,word [rdx*2+r8]
	xor rdx,rdx
	mov word [rcx+3],di
	div esi
	mov di,word [rdx*2+r8]
	mov word [rcx+1],di
	mov r9b,byte [rax*2+r8+1]
	mov byte [rcx],r9b
	mov byte [rcx+19],0
	lea rax,[rcx+19]
	jmp si64exit

	align 16
si64lop20:
	div rsi
	mov di,word [rdx*2+r8]
	xor rdx,rdx
	mov word [rcx+18],di
	div rsi
	mov di,word [rdx*2+r8]
	xor rdx,rdx
	mov word [rcx+16],di
	div rsi
	mov di,word [rdx*2+r8]
	xor rdx,rdx
	mov word [rcx+14],di
	div rsi
	mov di,word [rdx*2+r8]
	xor rdx,rdx
	mov word [rcx+12],di
	div rsi
	mov di,word [rdx*2+r8]
	xor rdx,rdx
	mov word [rcx+10],di
	div rsi
	mov di,word [rdx*2+r8]
	xor rdx,rdx
	mov word [rcx+8],di
	div esi
	mov di,word [rdx*2+r8]
	xor rdx,rdx
	mov word [rcx+6],di
	div esi
	mov di,word [rdx*2+r8]
	xor rdx,rdx
	mov word [rcx+4],di
	div esi
	mov di,word [rdx*2+r8]
	mov word [rcx+2],di
	mov di,word [rax*2+r8]
	mov word [rcx],di
	mov byte [rcx+20],0
	lea rax,[rcx+20]
;	jmp si64exit

	align 16
si64exit:
	pop rdi
	pop rsi
	ret

;IntOS MyString_StrCompare(const Char *str1, const Char *str2)
;0 retAddr
;rcx str1
;rdx str2
	align 16
MyString_StrCompare:
	mov r8,rsi
	mov r9,rdi
	mov rsi,rcx
	mov rdi,rdx
	
	align 16
scmplop:
	movzx rcx,byte [rsi]
	movzx rdx,byte [rdi]
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
	movzx rcx,byte [rsi]
	cmp cl,'0'
	jb scmplop7
	cmp cl,'9'
	ja scmplop7
	lea rax,[rax+rax*4]
	lea rsi,[rsi+1]
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
	movzx rcx,byte [rdi]
	cmp cl,'0'
	jb scmplop8
	cmp cl,'9'
	ja scmplop8
	lea rdx,[rdx+rdx*4]
	lea rdi,[rdi+1]
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
	mov rsi,r8
	mov rdi,r9
	ret

;IntOS MyString_StrCompareICase(const Char *str1, const Char *str2)
;0 retAddr
;rcx str1
;rdx str2
	align 16
MyString_StrCompareICase:
	mov r8,rsi
	mov r9,rdi
	mov rsi,rcx
	mov rdi,rdx

	align 16
scmpiclop:
	movzx rcx,byte [rsi]
	movzx rdx,byte [rdi]
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
	movzx rcx,byte [rsi]
	cmp cl,'0'
	jb scmpiclop7
	cmp cl,'9'
	ja scmpiclop7
	lea rax,[rax+rax*4]
	lea rsi,[rsi+1]
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
	movzx rcx,byte [rdi]
	cmp cl,'0'
	jb scmpiclop8
	cmp cl,'9'
	ja scmpiclop8
	lea rdx,[rdx+rdx*4]
	lea rdi,[rdi+1]
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
	mov rsi,r8
	mov rdi,r9
	ret

;IntOS MyString_StrCompareUTF16(const UTF16Char *str1, const UTF16Char *str2)
;0 retAddr
;rcx str1
;rdx str2
	align 16
MyString_StrCompareUTF16:
	mov r8,rsi
	mov r9,rdi
	mov rsi,rcx
	mov rdi,rdx
	
	align 16
scmpu16lop:
	mov cx,word [rsi]
	mov dx,word [rdi]
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
	movzx ecx,word [rsi]
	cmp cx,'0'
	jb scmpu16lop7
	cmp cx,'9'
	ja scmpu16lop7
	lea rax,[rax+rax*4]
	lea rsi,[rsi+2]
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
	movzx ecx,word [rdi]
	cmp cx,'0'
	jb scmpu16lop8
	cmp cx,'9'
	ja scmpu16lop8
	lea rdx,[rdx+rdx*4]
	lea rdi,[rdi+2]
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
	mov rsi,r8
	mov rdi,r9
	ret

;IntOS MyString_StrCompareICaseUTF16(const UTF16Char *str1, const UTF16Char *str2)
;0 retAddr
;rcx str1
;rdx str2
	align 16
MyString_StrCompareICaseUTF16:
	mov r8,rsi
	mov r9,rdi
	mov rsi,rcx
	mov rdi,rdx

	align 16
scmpicu16lop:
	movzx rcx,word [rsi]
	movzx rdx,word [rdi]
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
	movzx rcx,word [rsi]
	cmp cx,'0'
	jb scmpicu16lop7
	cmp cx,'9'
	ja scmpicu16lop7
	lea rax,[rax+rax*4]
	lea rsi,[rsi+2]
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
	movzx rcx,word [rdi]
	cmp cx,'0'
	jb scmpicu16lop8
	cmp cx,'9'
	ja scmpicu16lop8
	lea rdx,[rdx+rdx*4]
	lea rdi,[rdi+2]
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
	mov rsi,r8
	mov rdi,r9
	ret

;IntOS MyString_StrCharCnt(const Char *s)
;0 retAdddr
;rcx s
	align 16
MyString_StrCharCnt:
	mov rax,rcx
	align 16
scclop:
	test byte[rax],0xff
	jz scclop0
	test byte[rax+1],0xff
	jz scclop1
	test byte[rax+2],0xff
	jz scclop2
	test byte[rax+3],0xff
	jz scclop3
	lea rax,[rax+4]
	jmp scclop
	align 16
scclop0:
	sub rax,rcx
	ret
	align 16
scclop1:
	sub rax,rcx
	inc rax
	ret
	align 16
scclop2:
	sub rax,rcx
	add rax,2
	ret
	align 16
scclop3:
	sub rax,rcx
	add rax,3
	ret

;IntOS MyString_StrCharCntUTF16(const UTF16Char *s)
;0 retAddr
;rcx s
	align 16
MyString_StrCharCntUTF16:
	mov rax,rcx
	align 16
sccu16lop:
	movzx edx,word [rax]
	lea rax,[rax+2]
	test dx,dx
	jnz sccu16lop
	sub rax,rcx
	shr rax,1
	dec rax
	ret
	
;IntOS MyString_StrCharCntUTF32(const UTF32Char *s)
;0 retAddr
;rcx s
	align 16
MyString_StrCharCntUTF32:
	mov rax,rcx
	align 16
sccu32lop:
	mov edx,dword [rax]
	lea rax,[rax+4]
	test edx,edx
	jnz sccu16lop
	sub rax,rcx
	shr rax,2
	dec rax
	ret
	
;UTF16Char *MyString_StrHexVal64VUTF16(UTF16Char *oriStr, Int64 val)
;0 rbx
;8 retAddr
;rcx oriStr
;rdx val
	align 16
MyString_StrHexVal64VUTF16:
	push rbx
	lea r8,[rel MyString_StrHexArrU16]
	mov rax,rdx
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
	mov ebx,eax
	shr ebx,24
	movzx rbx,bl
	mov bx,word [rbx*4+r8+2]
	mov word [rcx],bx
	lea rcx,[rcx+2]
	jmp shv64vu16lop14
	
	align 16
shv64vu16lop13:
	mov ebx,eax
	shr ebx,16
	movzx rbx,bl
	mov bx,word [rbx*4+r8+2]
	mov word [rcx],bx
	lea rcx,[rcx+2]
	jmp shv64vu16lop12
	
	align 16
shv64vu16lop11:
	mov ebx,eax
	shr ebx,8
	movzx rbx,bl
	mov bx,word [rbx*4+r8+2]
	mov word [rcx],bx
	lea rcx,[rcx+2]
	jmp shv64vu16lop10
	
	align 16
shv64vu16lop9:
	mov bx,word [rax*4+r8+2]
	mov word [rcx],bx
	lea rcx,[rcx+2]
	jmp shv64vu16lop8
	
	align 16
shv64vu16lop8v:
	mov rax,rdx
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
	
	mov ebx,eax
	shr ebx,24
	movzx rbx,bl
	mov ebx,dword [rbx*4+r8]
	mov dword [rcx],ebx
	lea rcx,[rcx+4]
	jmp shv64vu16lop6

	align 16
shv64vu16lop7:
	mov ebx,eax
	shr ebx,24
	movzx rbx,bl
	mov bx,word [rbx*4+r8+2]
	mov word [rcx],bx
	lea rcx,[rcx+2]
	jmp shv64vu16lop6
	
	align 16
shv64vu16lop5:
	mov ebx,eax
	shr ebx,16
	movzx rbx,bl
	mov bx,word [rbx*4+r8+2]
	mov word [rcx],bx
	lea rcx,[rcx+2]
	jmp shv64vu16lop4
	
	align 16
shv64vu16lop3:
	mov ebx,eax
	shr ebx,8
	movzx rbx,bl
	mov bx,word [rbx*4+r8+2]
	mov word [rcx],bx
	lea rcx,[rcx+2]
	jmp shv64vu16lop2
	
	align 16
shv64vu16lop1:
	mov bx,word [rax*4+r8+2]
	mov word [rcx],bx
	lea rcx,[rcx+2]
	mov word [rcx],0
	jmp shv64vu16exit
	
	align 16
shv64vu16lop16:
	mov ebx,eax
	shr ebx,24
	movzx rbx,bl
	mov ebx,dword [rbx*4+r8]
	mov dword [rcx],ebx
	lea rcx,[rcx+4]
	align 16
shv64vu16lop14:
	mov ebx,eax
	shr ebx,16
	movzx rbx,bl
	mov ebx,dword [rbx*4+r8]
	mov dword [rcx],ebx
	lea rcx,[rcx+4]
	align 16
shv64vu16lop12:
	mov ebx,eax
	shr ebx,8
	movzx rbx,bl
	mov ebx,dword [rbx*4+r8]
	mov dword [rcx],ebx
	lea rcx,[rcx+4]
	align 16
shv64vu16lop10:
	movzx rbx,al
	mov ebx,dword [rbx*4+r8]
	mov dword [rcx],ebx
	lea rcx,[rcx+4]
	align 16
shv64vu16lop8:
	mov ebx,edx
	shr ebx,24
	movzx rbx,bl
	mov ebx,dword [rbx*4+r8]
	mov dword [rcx],ebx
	lea rcx,[rcx+4]
	align 16
shv64vu16lop6:
	mov ebx,edx
	shr ebx,16
	movzx rbx,bl
	mov ebx,dword [rbx*4+r8]
	mov dword [rcx],ebx
	lea rcx,[rcx+4]
	align 16
shv64vu16lop4:
	mov ebx,edx
	shr ebx,8
	movzx rbx,bl
	mov ebx,dword [rbx*4+r8]
	mov dword [rcx],ebx
	lea rcx,[rcx+4]
	align 16
shv64vu16lop2:
	movzx rbx,dl
	mov ebx,dword [rbx*4+r8]
	mov dword [rcx],ebx
	lea rcx,[rcx+4]
	mov word [rcx],0
	align 16
shv64vu16exit:
	mov rax,rcx
	pop rbx
	ret

;UTF16Char *MyString_StrHexVal64UTF16(UTF16Char *oriStr, Int64 val)
;0 retAddr
;rcx oriStr
;rdx val
	align 16
MyString_StrHexVal64UTF16:
	lea r8,[rel MyString_StrHexArrU16]
	mov word [rcx+32],0

	movzx rax,dl
	mov eax,dword [rax*4+r8]
	mov dword [rcx+28],eax
	shr rdx,8
	movzx eax,dl
	mov eax,dword [rax*4+r8]
	mov dword [rcx+24],eax
	shr rdx,8
	movzx eax,dl
	mov eax,dword [rax*4+r8]
	mov dword [rcx+20],eax
	shr rdx,8
	movzx eax,dl
	mov eax,dword [rax*4+r8]
	mov dword [rcx+16],eax
	shr rdx,8
	movzx eax,dl
	mov eax,dword [rax*4+r8]
	mov dword [rcx+12],eax
	shr rdx,8
	movzx eax,dl
	mov eax,dword [rax*4+r8]
	mov dword [rcx+8],eax
	shr rdx,8
	movzx eax,dl
	mov eax,dword [rax*4+r8]
	mov dword [rcx+4],eax
	shr rdx,8
	movzx eax,dl
	mov eax,dword [rax*4+r8]
	mov dword [rcx+0],eax

	lea rax,[rcx+32]
	ret

;UTF16Char *MyString_StrHexVal32VUTF16(UTF16Char *oriStr, Int32 val)
;0 rbx
;8 retAddr
;rcx oriStr
;rdx val
	align 16
MyString_StrHexVal32VUTF16:
	push rbx
	lea r8,[rel MyString_StrHexArrU16]
	jmp shv64vu16lop8v

;UTF16Char *MyString_StrHexVal32UTF16(UTF16Char *oriStr, Int32 val)
;0 retAddr
;rcx oriStr
;rdx val
	align 16
MyString_StrHexVal32UTF16:
	lea r8,[rel MyString_StrHexArrU16]
	mov word [rcx+16],0

	movzx rax,dl
	mov eax,dword [rax*4+r8]
	mov dword [rcx+12],eax
	shr edx,8
	movzx eax,dl
	mov eax,dword [rax*4+r8]
	mov dword [rcx+8],eax
	shr edx,8
	movzx eax,dl
	mov eax,dword [rax*4+r8]
	mov dword [rcx+4],eax
	shr edx,8
	movzx eax,dl
	mov eax,dword [rax*4+r8]
	mov dword [rcx+0],eax

	lea rax,[rcx+16]
	ret
	
;UTF16Char *MyString_StrHexVal24UTF16(UTF16Char *oriStr, Int32 val)
;0 retAddr
;rcx oriStr
;rdx val
	align 16
MyString_StrHexVal24UTF16:
	lea r8,[rel MyString_StrHexArrU16]
	mov word [rcx+12],0

	movzx rax,dl
	mov eax,dword [rax*4+r8]
	mov dword [rcx+8],eax
	shr edx,8
	movzx eax,dl
	mov eax,dword [rax*4+r8]
	mov dword [rcx+4],eax
	shr edx,8
	movzx eax,dl
	mov eax,dword [rax*4+r8]
	mov dword [rcx+0],eax

	lea rax,[rcx+12]
	ret

;UTF16Char *MyString_StrHexVal64VUTF32(UTF16Char *oriStr, Int64 val)
;0 rbx
;8 retAddr
;rcx oriStr
;rdx val
	align 16
MyString_StrHexVal64VUTF32:
	push rbx
	lea r8,[rel MyString_StrHexArrU16]
	mov rax,rdx
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
	mov ebx,eax
	shr ebx,24
	movzx rbx,bl
	movzx ebx,word [rbx*4+r8+2]
	mov dword [rcx],ebx
	lea rcx,[rcx+4]
	jmp shv64vu32lop14
	
	align 16
shv64vu32lop13:
	mov ebx,eax
	shr ebx,16
	movzx rbx,bl
	movzx ebx,word [rbx*4+r8+2]
	mov dword [rcx],ebx
	lea rcx,[rcx+4]
	jmp shv64vu32lop12
	
	align 16
shv64vu32lop11:
	mov ebx,eax
	shr ebx,8
	movzx rbx,bl
	movzx ebx,word [rbx*4+r8+2]
	mov dword [rcx],ebx
	lea rcx,[rcx+4]
	jmp shv64vu32lop10
	
	align 16
shv64vu32lop9:
	movzx ebx,word [rax*4+r8+2]
	mov dword [rcx],ebx
	lea rcx,[rcx+4]
	jmp shv64vu32lop8
	
	align 16
shv64vu32lop8v:
	mov rax,rdx
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
	
	mov ebx,eax
	shr ebx,24
	movzx rbx,bl
	movzx r9d,word [rbx*4+r8]
	movzx ebx,word [rbx*4+r8+2]
	mov dword [rcx],r9d
	mov dword [rcx+4],ebx
	lea rcx,[rcx+8]
	jmp shv64vu32lop6

	align 16
shv64vu32lop7:
	mov ebx,eax
	shr ebx,24
	movzx rbx,bl
	movzx ebx,word [rbx*4+r8+2]
	mov dword [rcx],ebx
	lea rcx,[rcx+4]
	jmp shv64vu32lop6
	
	align 16
shv64vu32lop5:
	mov ebx,eax
	shr ebx,16
	movzx rbx,bl
	movzx ebx,word [rbx*4+r8+2]
	mov dword [rcx],ebx
	lea rcx,[rcx+4]
	jmp shv64vu32lop4
	
	align 16
shv64vu32lop3:
	mov ebx,eax
	shr ebx,8
	movzx rbx,bl
	movzx ebx,word [rbx*4+r8+2]
	mov dword [rcx],ebx
	lea rcx,[rcx+4]
	jmp shv64vu32lop2
	
	align 16
shv64vu32lop1:
	movzx ebx,word [rax*4+r8+2]
	mov dword [rcx],ebx
	lea rcx,[rcx+4]
	mov dword [rcx],0
	jmp shv64vu32exit
	
	align 16
shv64vu32lop16:
	mov ebx,eax
	shr ebx,24
	movzx rbx,bl
	movzx r9d,word [rbx*4+r8]
	movzx ebx,word [rbx*4+r8+2]
	mov dword [rcx],r9d
	mov dword [rcx+4],ebx
	lea rcx,[rcx+8]
	align 16
shv64vu32lop14:
	mov ebx,eax
	shr ebx,16
	movzx rbx,bl
	movzx r9d,word [rbx*4+r8]
	movzx ebx,word [rbx*4+r8+2]
	mov dword [rcx],r9d
	mov dword [rcx+4],ebx
	lea rcx,[rcx+8]
	align 16
shv64vu32lop12:
	mov ebx,eax
	shr ebx,8
	movzx rbx,bl
	movzx r9d,word [rbx*4+r8]
	movzx ebx,word [rbx*4+r8+2]
	mov dword [rcx],r9d
	mov dword [rcx+4],ebx
	lea rcx,[rcx+8]
	align 16
shv64vu32lop10:
	movzx rbx,al
	movzx r9d,word [rbx*4+r8]
	movzx ebx,word [rbx*4+r8+2]
	mov dword [rcx],r9d
	mov dword [rcx+4],ebx
	lea rcx,[rcx+8]
	align 16
shv64vu32lop8:
	mov ebx,edx
	shr ebx,24
	movzx rbx,bl
	movzx r9d,word [rbx*4+r8]
	movzx ebx,word [rbx*4+r8+2]
	mov dword [rcx],r9d
	mov dword [rcx+4],ebx
	lea rcx,[rcx+8]
	align 16
shv64vu32lop6:
	mov ebx,edx
	shr ebx,16
	movzx rbx,bl
	movzx r9d,word [rbx*4+r8]
	movzx ebx,word [rbx*4+r8+2]
	mov dword [rcx],r9d
	mov dword [rcx+4],ebx
	lea rcx,[rcx+8]
	align 16
shv64vu32lop4:
	mov ebx,edx
	shr ebx,8
	movzx rbx,bl
	movzx r9d,word [rbx*4+r8]
	movzx ebx,word [rbx*4+r8+2]
	mov dword [rcx],r9d
	mov dword [rcx+4],ebx
	lea rcx,[rcx+8]
	align 16
shv64vu32lop2:
	movzx rbx,dl
	movzx r9d,word [rbx*4+r8]
	movzx ebx,word [rbx*4+r8+2]
	mov dword [rcx],r9d
	mov dword [rcx+4],ebx
	lea rcx,[rcx+8]
	mov dword [rcx],0
	align 16
shv64vu32exit:
	mov rax,rcx
	pop rbx
	ret

;UTF16Char *MyString_StrHexVal64UTF32(UTF16Char *oriStr, Int64 val)
;0 retAddr
;rcx oriStr
;rdx val
	align 16
MyString_StrHexVal64UTF32:
	lea r8,[rel MyString_StrHexArrU16]
	mov dword [rcx+64],0

	movzx rax,dl
	movzx r9d,word [rax*4+r8]
	movzx eax,word [rax*4+r8+2]
	mov dword [rcx+56],r9d
	mov dword [rcx+60],eax
	shr rdx,8
	movzx eax,dl
	movzx r9d,word [rax*4+r8]
	movzx eax,word [rax*4+r8+2]
	mov dword [rcx+48],r9d
	mov dword [rcx+52],eax
	shr rdx,8
	movzx eax,dl
	movzx r9d,word [rax*4+r8]
	movzx eax,word [rax*4+r8+2]
	mov dword [rcx+40],r9d
	mov dword [rcx+44],eax
	shr rdx,8
	movzx eax,dl
	movzx r9d,word [rax*4+r8]
	movzx eax,word [rax*4+r8+2]
	mov dword [rcx+32],r9d
	mov dword [rcx+36],eax
	shr rdx,8
	movzx eax,dl
	movzx r9d,word [rax*4+r8]
	movzx eax,word [rax*4+r8+2]
	mov dword [rcx+24],r9d
	mov dword [rcx+28],eax
	shr rdx,8
	movzx eax,dl
	movzx r9d,word [rax*4+r8]
	movzx eax,word [rax*4+r8+2]
	mov dword [rcx+16],r9d
	mov dword [rcx+20],eax
	shr rdx,8
	movzx eax,dl
	movzx r9d,word [rax*4+r8]
	movzx eax,word [rax*4+r8+2]
	mov dword [rcx+8],r9d
	mov dword [rcx+12],eax
	shr rdx,8
	movzx eax,dl
	movzx r9d,word [rax*4+r8]
	movzx eax,word [rax*4+r8+2]
	mov dword [rcx+0],r9d
	mov dword [rcx+4],eax

	lea rax,[rcx+64]
	ret

;UTF16Char *MyString_StrHexVal32VUTF32(UTF16Char *oriStr, Int32 val)
;0 rbx
;8 retAddr
;rcx oriStr
;rdx val
	align 16
MyString_StrHexVal32VUTF32:
	push rbx
	lea r8,[rel MyString_StrHexArrU16]
	jmp shv64vu32lop8v

;UTF16Char *MyString_StrHexVal32UTF32(UTF16Char *oriStr, Int32 val)
;0 retAddr
;rcx oriStr
;rdx val
	align 16
MyString_StrHexVal32UTF32:
	lea r8,[rel MyString_StrHexArrU16]
	mov dword [rcx+32],0

	movzx rax,dl
	movzx r9d,word [rax*4+r8]
	movzx eax,word [rax*4+r8+2]
	mov dword [rcx+24],r9d
	mov dword [rcx+28],eax
	shr edx,8
	movzx eax,dl
	movzx r9d,word [rax*4+r8]
	movzx eax,word [rax*4+r8+2]
	mov dword [rcx+16],r9d
	mov dword [rcx+20],eax
	shr edx,8
	movzx eax,dl
	movzx r9d,word [rax*4+r8]
	movzx eax,word [rax*4+r8+2]
	mov dword [rcx+8],r9d
	mov dword [rcx+12],eax
	shr edx,8
	movzx eax,dl
	movzx r9d,word [rax*4+r8]
	movzx eax,word [rax*4+r8+2]
	mov dword [rcx+0],r9d
	mov dword [rcx+4],eax

	lea rax,[rcx+32]
	ret
	
;UTF16Char *MyString_StrHexVal24UTF32(UTF16Char *oriStr, Int32 val)
;0 retAddr
;rcx oriStr
;rdx val
	align 16
MyString_StrHexVal24UTF32:
	lea r8,[rel MyString_StrHexArrU16]
	mov word [rcx+24],0

	movzx rax,dl
	movzx r9d,word [rax*4+r8]
	movzx eax,word [rax*4+r8+2]
	mov dword [rcx+16],r9d
	mov dword [rcx+20],eax
	shr edx,8
	movzx eax,dl
	movzx r9d,word [rax*4+r8]
	movzx eax,word [rax*4+r8+2]
	mov dword [rcx+8],r9d
	mov dword [rcx+12],eax
	shr edx,8
	movzx eax,dl
	movzx r9d,word [rax*4+r8]
	movzx eax,word [rax*4+r8+2]
	mov dword [rcx+0],r9d
	mov dword [rcx+4],eax

	lea rax,[rcx+24]
	ret

;Int32 MyString_StrToInt32UTF8(const UTF8Char *intStr)
;0 retAddr
;rcx intStr
	align 16
MyString_StrToInt32UTF8:
	xor rax,rax
	cmp byte [rcx], 0x2d
	jnz sti32u8lop
	lea rcx,[rcx+1]
	
	align 16
sti32u8lop5:
	movzx edx,byte [rcx]
	cmp edx,0x3a
	jnb sti32u8lop8
	cmp edx,0x30
	jb sti32u8lop8
	lea rcx,[rcx+1]
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
	movzx edx,byte [rcx]
	cmp edx,0x3a
	jnb sti32u8lop2
	cmp edx,0x30
	jb sti32u8lop2
	lea rcx,[rcx+1]
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
;rcx intStr
	align 16
MyString_StrToInt32UTF16:
	xor rax,rax
	cmp word [rcx], 0x2d
	jnz sti32u16lop
	lea rcx,[rcx+2]
	
	align 16
sti32u16lop5:
	movzx edx,word [rcx]
	cmp edx,0x3a
	jnb sti32u16lop8
	cmp edx,0x30
	jb sti32u16lop8
	lea rcx,[rcx+2]
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
	movzx edx,word [rcx]
	cmp edx,0x3a
	jnb sti32u16lop2
	cmp edx,0x30
	jb sti32u16lop2
	lea rcx,[rcx+2]
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
;rcx intStr
	align 16
MyString_StrToInt32UTF32:
	xor rax,rax
	cmp dword [rcx], 0x2d
	jnz sti32u32lop
	lea rcx,[rcx+4]
	
	align 16
sti32u32lop5:
	mov edx,dword [rcx]
	cmp edx,0x3a
	jnb sti32u32lop8
	cmp edx,0x30
	jb sti32u32lop8
	lea rcx,[rcx+4]
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
	mov edx,dword [rcx]
	cmp edx,0x3a
	jnb sti32u32lop2
	cmp edx,0x30
	jb sti32u32lop2
	lea rcx,[rcx+4]
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
;rcx str1
	align 16
MyString_StrToInt64UTF8:
	xor rax,rax
	xor rdx,rdx
	cmp word [rcx], 0x7830
	jz sti64u8loph
	cmp byte [rcx], 0x2d
	jnz sti64u8lop
	lea rcx,[rcx+1]
	
	align 16
sti64u8lop5:
	movzx edx,byte [rcx]
	cmp edx,0x3a
	jnb sti64u8lop8
	cmp edx,0x30
	jb sti64u8lop8
	lea rcx,[rcx+1]
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
	add rcx,2
	
	align 16
sti64u8loph2:
	movzx edx,byte [rcx]
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
	inc rcx
	shl rax,4
	lea rax,[rax+rdx-0x30]
	jmp sti64u8loph2


	align 16
sti64u8loph5:
	inc rcx
	shl rax,4
	lea rax,[rax+rdx-0x37]
	jmp sti64u8loph2

	align 16
sti64u8lop:
	movzx edx,byte [rcx]
	cmp edx,0x3a
	jnb sti64u8lop2
	cmp edx,0x30
	jb sti64u8lop2
	lea rcx,[rcx+1]
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
;rcx str1
	align 16
MyString_StrToInt64UTF16:
	xor rax,rax
	xor rdx,rdx
	cmp word [rcx], 0x2d
	jnz sti64u16lop
	lea rcx,[rcx+2]
	
	align 16
sti64u16lop5:
	movzx edx,word [rcx]
	cmp edx,0x3a
	jnb sti64u16lop8
	cmp edx,0x30
	jb sti64u16lop8
	lea rcx,[rcx+2]
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
	movzx edx,word [rcx]
	cmp edx,0x3a
	jnb sti64u16lop2
	cmp edx,0x30
	jb sti64u16lop2
	lea rcx,[rcx+2]
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
;rcx str1
	align 16
MyString_StrToInt64UTF32:
	xor rax,rax
	xor rdx,rdx
	cmp dword [rcx], 0x2d
	jnz sti64u32lop
	lea rcx,[rcx+4]
	
	align 16
sti64u32lop5:
	mov edx,dword [rcx]
	cmp edx,0x3a
	jnb sti64u32lop8
	cmp edx,0x30
	jb sti64u32lop8
	lea rcx,[rcx+4]
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
	mov edx,dword [rcx]
	cmp edx,0x3a
	jnb sti64u32lop2
	cmp edx,0x30
	jb sti64u32lop2
	lea rcx,[rcx+4]
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