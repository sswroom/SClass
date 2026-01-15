section .text

extern _MyString_StrDigit100U8
extern _MyString_StrDigit100U16
extern _MyString_StrDigit100U32
extern _MyString_StrHexArrU16
extern _MyString_StrHexArrU32

;global _MyString_StrConcat
;global _MyString_StrConcatF
;global _MyString_StrConcatSUTF16
;global _MyString_StrConcatUTF16
;global _MyString_StrConcatSUTF32
;global _MyString_StrConcatUTF32

;global _MyString_StrInt32UTF16
;global _MyString_StrUInt32UTF16
;global _MyString_StrInt64UTF16
;global _MyString_StrUInt64UTF16
;global _MyString_StrInt32UTF32
;global _MyString_StrUInt32UTF32
;global _MyString_StrInt64UTF32
;global _MyString_StrUInt64UTF32

global _MyString_StrCompare
global _MyString_StrCompareICase
global _MyString_StrCompareUTF16
global _MyString_StrCompareICaseUTF16
global _MyString_StrCompareUTF32
global _MyString_StrCompareICaseUTF32

global _MyString_StrCharCnt
global _MyString_StrCharCntUTF16
global _MyString_StrCharCntUTF32

global _MyString_StrHexVal64VUTF16
global _MyString_StrHexVal32VUTF16
global _MyString_StrHexVal64VUTF32
global _MyString_StrHexVal32VUTF32
global _MyString_StrHexVal64UTF16
global _MyString_StrHexVal64UTF32
global _MyString_StrHexVal32UTF16
global _MyString_StrHexVal32UTF32
global _MyString_StrHexVal24UTF16
global _MyString_StrHexVal24UTF32

global _MyString_StrToInt32UTF8
global _MyString_StrToInt32UTF16
global _MyString_StrToInt32UTF32
global _MyString_StrToInt64UTF8
global _MyString_StrToInt64UTF16
global _MyString_StrToInt64UTF32

;global MyString_StrConcat
;global MyString_StrConcatF
;global MyString_StrConcatSUTF16
;global MyString_StrConcatUTF16
;global MyString_StrConcatSUTF32
;global MyString_StrConcatUTF32

;global MyString_StrInt32UTF16
;global MyString_StrUInt32UTF16
;global MyString_StrInt64UTF16
;global MyString_StrUInt64UTF16
;global MyString_StrInt32UTF32
;global MyString_StrUInt32UTF32
;global MyString_StrInt64UTF32
;global MyString_StrUInt64UTF32

global MyString_StrCompare
global MyString_StrCompareICase
global MyString_StrCompareUTF16
global MyString_StrCompareICaseUTF16
global MyString_StrCompareUTF32
global MyString_StrCompareICaseUTF32

global MyString_StrCharCnt
global MyString_StrCharCntUTF16
global MyString_StrCharCntUTF32

global MyString_StrHexVal64VUTF16
global MyString_StrHexVal32VUTF16
global MyString_StrHexVal64VUTF32
global MyString_StrHexVal32VUTF32
global MyString_StrHexVal64UTF16
global MyString_StrHexVal64UTF32
global MyString_StrHexVal32UTF16
global MyString_StrHexVal32UTF32
global MyString_StrHexVal24UTF16
global MyString_StrHexVal24UTF32

global MyString_StrToInt32UTF8
global MyString_StrToInt32UTF16
global MyString_StrToInt32UTF32
global MyString_StrToInt64UTF8
global MyString_StrToInt64UTF16
global MyString_StrToInt64UTF32

;Char *MyString_StrConcat(Char *oriStr, const Char *strToJoin)
;0 ebx
;4 retAddr
;8 oriStr
;12 strToJoin
	align 16
MyString_StrConcat:
_MyString_StrConcat:
	push ebx
	mov ecx,dword [esp+8]
	mov edx,dword [esp+12]
	xor ebx,ebx
	align 16
sconcatlop:
	mov al,byte [edx+ebx]
	mov byte [ecx+ebx],al
	lea ebx,[ebx+1]
	test al,al
	jnz sconcatlop
	lea eax,[ecx+ebx-1]
	pop ebx
	ret

;Char *MyString_StrConcatF(Char *oriStr, const Char *strToJoin, IntOS buffSize)
;0 esi
;4 ebx
;8 retAddr
;12 oriStr
;16 strToJoin
;20 bufFSize
	align 16
MyString_StrConcatF:
_MyString_StrConcatF:
	push ebx
	push esi
	mov ecx,dword [esp+12]
	mov edx,dword [esp+16]
	mov esi,dword [esp+20]
	xor ebx,ebx
	align 16
sconcatflop:
	mov al,byte [edx+ebx]
	mov byte [ecx+ebx],al
	lea ebx,[ebx+1]
	dec esi
	jnz sconcatflop
	mov byte [ecx+ebx],0
	lea eax,[ecx+ebx]
	pop esi
	pop ebx
	ret

;UTF16Char *MyString_StrConcatSUTF16(UTF16Char *oriStr, const UTF16Char *strToJoin, IntOS buffSize)
;0 esi
;4 ebx
;8 retAddr
;12 oriStr
;16 strToJoin
;20 buffSize
	align 16
MyString_StrConcatSUTF16:
_MyString_StrConcatSUTF16:
	mov edx,dword [esp+12]
	mov ecx,dword [esp+4]
	cmp edx,0
	jle sconcatsu16lop3
	push esi
	push ebx
	mov esi,dword [esp+16]
	xor ebx,ebx
	align 16
sconcatsu16lop:
	mov ax,word [esi+ebx]
	mov word [ecx+ebx],ax
	test ax,ax
	jz sconcatsu16lop2
	lea ebx,[ebx+2]
	dec edx
	jnz sconcatsu16lop
	mov word [ecx+ebx],0
sconcatsu16lop2:
	lea eax,[ecx+ebx]
	pop ebx
	pop esi
	ret

	align 16
sconcatsu16lop3:
	mov word [ecx],0
	mov eax,ecx
	ret

;UTF16Char *MyString_StrConcatUTF16(UTF16Char *oriStr, const UTF16Char *strToJoin)
;0 ebx
;4 retAddr
;8 oriStr
;12 strToJoin
	align 16
MyString_StrConcatUTF16:
_MyString_StrConcatUTF16:
	push ebx
	mov ecx,dword [esp+8]
	mov edx,dword [esp+12]
	xor ebx,ebx
	align 16
sconcatu16lop:
	mov ax,word [edx+ebx]
	mov word [ecx+ebx],ax
	lea ebx,[ebx+2]
	test ax,ax
	jnz sconcatu16lop
	lea eax,[ecx+ebx-2]
	pop ebx
	ret

;UTF32Char *MyString_StrConcatSUTF32(UTF32Char *oriStr, const UTF32Char *strToJoin, IntOS buffSize)
;0 esi
;4 ebx
;8 retAddr
;12 oriStr
;16 strToJoin
;20 buffSize
	align 16
MyString_StrConcatSUTF32:
_MyString_StrConcatSUTF32:
	mov edx,dword [esp+12]
	mov ecx,dword [esp+4]
	cmp edx,0
	jle sconcatsu32lop3
	push esi
	push ebx
	mov esi,dword [esp+16]
	xor ebx,ebx
	align 16
sconcatsu32lop:
	mov eax,dword [esi+ebx]
	mov dword [ecx+ebx],eax
	test eax,eax
	jz sconcatsu32lop2
	lea ebx,[ebx+4]
	dec edx
	jnz sconcatsu32lop
	mov dword [ecx+ebx],0
sconcatsu32lop2:
	lea eax,[ecx+ebx]
	pop ebx
	pop esi
	ret

	align 16
sconcatsu32lop3:
	mov dword [ecx],0
	mov eax,ecx
	ret

;UTF32Char *MyString_StrConcatUTF32(UTF32Char *oriStr, const UTF32Char *strToJoin)
;0 ebx
;4 retAddr
;8 oriStr
;12 strToJoin
	align 16
MyString_StrConcatUTF32:
_MyString_StrConcatUTF32:
	push ebx
	mov ecx,dword [esp+8]
	mov edx,dword [esp+12]
	xor ebx,ebx
	align 16
sconcatu32lop:
	mov eax,dword [edx+ebx]
	mov dword [ecx+ebx],eax
	lea ebx,[ebx+4]
	test eax,eax
	jnz sconcatu32lop
	lea eax,[ecx+ebx-4]
	pop ebx
	ret

;UTF16Char *MyString_StrInt32UTF16(UTF16Char *oriStr, Int32 val)
;0 retAddr
;4 oriStr
;8 val
	align 16
MyString_StrInt32UTF16:
_MyString_StrInt32UTF16:
	mov eax,dword [esp+8] ;val
	mov ecx,dword [esp+4] ;oriStr
	test eax,0x80000000
	jz si32u16lop
	neg eax
	mov word [ecx],'-'
	lea ecx,[ecx+2]
	jmp si32u16lop

;UTF16Char *MyString_StrUInt32UTF16(UTF16Char *oriStr, UInt32 val)
;0 retAddr
;4 oriStr
;8 val
	align 16
MyString_StrUInt32UTF16:
_MyString_StrUInt32UTF16:
	mov eax,dword [esp+8] ;val
	mov ecx,dword [esp+4] ;oriStr
si32u16lop:
	cmp eax,10
	jb sui32u16lop1
	cmp eax,100
	jb sui32u16lop2
	push esi
	push edi
	mov esi,100
	xor edx,edx
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
	movzx edx,word [eax*4+_MyString_StrDigit100U16+2]
	mov dword [ecx],edx
	lea eax,[ecx+2]
	ret
	
	align 16
sui32u16lop2:
	mov edx,dword [eax*4+_MyString_StrDigit100U16]
	mov dword [ecx],edx
	mov word [ecx+4],0
	lea eax,[ecx+4]
	ret

	align 16
sui32u16lop3:
	div esi
	mov di,word [eax*4+_MyString_StrDigit100U16+2]
	mov word [ecx],di
	mov edi,dword [edx*4+_MyString_StrDigit100U16]
	mov dword [ecx+2],edi
	mov word [ecx+6],0
	lea eax,[ecx+6]
	jmp sui32u16exit

	align 16
sui32u16lop4:
	div esi
	mov edi,dword [eax*4+_MyString_StrDigit100U16]
	mov dword [ecx],edi
	mov edi,dword [edx*4+_MyString_StrDigit100U16]
	mov dword [ecx+4],edi
	mov word [ecx+8],0
	lea eax,[ecx+8]
	jmp sui32u16exit

	align 16
sui32u16lop5:
	div esi
	mov edi,dword [edx*4+_MyString_StrDigit100U16]
	xor edx,edx
	mov dword [ecx+6],edi
	div esi
	mov edi,dword [edx*4+_MyString_StrDigit100U16]
	mov dword [ecx+2],edi
	mov di,word [eax*4+_MyString_StrDigit100U16+2]
	mov word [ecx],di
	mov word [ecx+10],0
	lea eax,[ecx+10]
	jmp sui32u16exit

	align 16
sui32u16lop6:
	div esi
	mov edi,dword [edx*4+_MyString_StrDigit100U16]
	xor edx,edx
	mov dword [ecx+8],edi
	div esi
	mov edi,dword [edx*4+_MyString_StrDigit100U16]
	mov dword [ecx+4],edi
	mov edi,dword [eax*4+_MyString_StrDigit100U16]
	mov dword [ecx],edi
	mov word [ecx+12],0
	lea eax,[ecx+12]
	jmp sui32u16exit

	align 16
sui32u16lop7:
	div esi
	mov edi,dword [edx*4+_MyString_StrDigit100U16]
	xor edx,edx
	mov dword [ecx+10],edi
	div esi
	mov edi,dword [edx*4+_MyString_StrDigit100U16]
	xor edx,edx
	mov dword [ecx+6],edi
	div esi
	mov edi,dword [edx*4+_MyString_StrDigit100U16]
	mov dword [ecx+2],edi
	mov di,word [eax*4+_MyString_StrDigit100U16+2]
	mov word [ecx],di
	mov word [ecx+14],0
	lea eax,[ecx+14]
	jmp sui32u16exit

	align 16
sui32u16lop8:
	div esi
	mov edi,dword [edx*4+_MyString_StrDigit100U16]
	xor edx,edx
	mov dword [ecx+12],edi
	div esi
	mov edi,dword [edx*4+_MyString_StrDigit100U16]
	xor edx,edx
	mov dword [ecx+8],edi
	div esi
	mov edi,dword [edx*4+_MyString_StrDigit100U16]
	movnti dword [ecx+4],edi
	mov edi,dword [eax*4+_MyString_StrDigit100U16]
	mov dword [ecx],edi
	mov word [ecx+16],0
	lea eax,[ecx+16]
	jmp sui32u16exit

	align 16
sui32u16lop9:
	div esi
	mov edi,dword [edx*4+_MyString_StrDigit100U16]
	xor edx,edx
	mov dword [ecx+14],edi
	div esi
	mov edi,dword [edx*4+_MyString_StrDigit100U16]
	xor edx,edx
	mov dword [ecx+10],edi
	div esi
	mov edi,dword [edx*4+_MyString_StrDigit100U16]
	xor edx,edx
	mov dword [ecx+6],edi
	div esi
	mov edi,dword [edx*4+_MyString_StrDigit100U16]
	mov dword [ecx+2],edi
	mov di,word [eax*4+_MyString_StrDigit100U16+2]
	mov word [ecx],di
	mov word [ecx+18],0
	lea eax,[ecx+18]
	jmp sui32u16exit

	align 16
sui32u16lop10:
	div esi
	mov edi,dword [edx*4+_MyString_StrDigit100U16]
	xor edx,edx
	mov dword [ecx+16],edi
	div esi
	mov edi,dword [edx*4+_MyString_StrDigit100U16]
	xor edx,edx
	mov dword [ecx+12],edi
	div esi
	mov edi,dword [edx*4+_MyString_StrDigit100U16]
	xor edx,edx
	mov dword [ecx+8],edi
	div esi
	mov edi,dword [edx*4+_MyString_StrDigit100U16]
	mov dword [ecx+4],edi
	mov edi,dword [eax*4+_MyString_StrDigit100U16]
	mov dword [ecx],edi
	mov word [ecx+20],0
	lea eax,[ecx+20]

	align 16
sui32u16exit:
	pop edi
	pop esi
	ret

;UTF16Char *MyString_StrInt64UTF16(UTF16Char *oriStr, Int64 val)
;-12 edi
;-8 esi
;-4 ebx
;0 retAddr
;4 oriStr
;8 val
	align 16
MyString_StrInt64UTF16:
_MyString_StrInt64UTF16:
	mov eax,dword [esp+8] ;val
	mov ecx,dword [esp+12] ;val[4]
	cmp ecx,0
	jg si64u16lop2
	jl si64u16lop7
	mov ecx,dword [esp+4]
	jmp si32u16lop

	align 16
si64u16lop0:
	mov ecx,dword [esp+4]
	mov word [ecx],'-'
	lea ecx,[ecx+2]
	jmp si32u16lop

	align 16
si64u16lop7:
	not eax
	not ecx
	add eax,1
	adc ecx,0
	jz si64u16lop0
	mov dword [esp-4],ebx
	mov dword [esp-8],esi
	mov dword [esp-12],edi
	mov edi,1
	mov ebx,100
	lea esi,[esp-12]
	jmp si64u16lop3

	align 16
si64u16lop2:
	mov dword [esp-12],edi
	mov dword [esp-8],esi
	mov dword [esp-4],ebx
	xor edi,edi
	mov ebx,100
	lea esi,[esp-12]
	align 16
si64u16lop3:
	test ecx,ecx
	jz si64u16lop2b
	xor edx,edx
	xchg eax,ecx
	div ebx
	xchg eax,ecx
	div ebx
	mov edx,dword [edx*4+_MyString_StrDigit100U16]
	lea esi,[esi-4]
	mov dword [esi],edx
	jmp si64u16lop3

	align 16
si64u16lop2b:
	xor edx,edx
	div ebx
	mov ecx,dword [edx*4+_MyString_StrDigit100U16]
	lea esi,[esi-4]
	mov dword [esi],ecx
	cmp eax,ebx
	jnb si64u16lop2b

	cmp eax,10
	jb si64u16lop2b2
	mov eax,dword [eax*4+_MyString_StrDigit100U16]
	lea esi,[esi-4]
	mov dword [esi],eax
	jmp si64u16lop2c
	
	align 16
si64u16lop2b2:
	mov ax,word [eax*4+_MyString_StrDigit100U16+2]
	lea esi,[esi-2]
	mov word [esi],ax
	align 16
si64u16lop2c:
	cld
	mov ecx,edi
	lea ebx,[esp-12]
	mov edi,dword [esp+4] ;oriStr
	test ecx,ecx
	jz si64u16lop6
	mov word [edi],'-'
	lea edi,[edi+2]
	align 16
si64u16lop6:
	movsw
	cmp esi,ebx
	jb si64u16lop6
	mov word [edi],0
	mov eax,edi
	align 16
si64u16exit:
	mov edi,dword [esp-12]
	mov esi,dword [esp-8]
	mov ebx,dword [esp-4]
	ret
	
;UTF16Char *MyString_StrUInt64UTF16(UTF16Char *oriStr, UInt64 val)
;-12 edi
;-8 esi
;-4 ebx
;0 retAddr
;4 oriStr
;8 val
	align 16
MyString_StrUInt64UTF16:
_MyString_StrUInt64UTF16:
	mov eax,dword [esp+8] ;val
	mov ecx,dword [esp+12] ;val[4]
	test ecx,ecx
	jnz sui64u16lop2
	mov ecx,dword [esp+4]
	jmp si32u16lop

	align 16
sui64u16lop2:
	mov dword [esp-12],edi
	mov dword [esp-8],esi
	mov dword [esp-4],ebx
	mov ebx,100
	lea esi,[esp-12]
	align 16
sui64u16lop3:
	xor edx,edx
	xchg eax,ecx
	div ebx
	xchg eax,ecx
	div ebx
	mov edx,dword [edx*4+_MyString_StrDigit100U16]
	lea esi,[esi-4]
	mov dword [esi],edx
	test ecx,ecx
	jnz sui64u16lop3

	align 16
sui64u16lop2b:
	xor edx,edx
	div ebx
	mov ecx,dword [edx*4+_MyString_StrDigit100U16]
	lea esi,[esi-4]
	mov dword [esi],ecx
	cmp eax,ebx
	jnb sui64u16lop2b

	cmp eax,10
	jb sui64u16lop2b2
	mov eax,dword [eax*4+_MyString_StrDigit100U16]
	lea esi,[esi-4]
	mov dword [esi],eax
	jmp sui64u16lop2c
	
	align 16
sui64u16lop2b2:
	mov ax,word [eax*4+_MyString_StrDigit100U16+2]
	lea esi,[esi-2]
	mov word [esi],ax
	align 16
sui64u16lop2c:
	cld
	lea ecx,[esp-12]
	mov edi,dword [esp+4] ;oriStr
	sub ecx,esi
	shr ecx,1
	rep movsw
	mov word [edi],0
	mov eax,edi
	align 16
sui64u16exit:
	mov edi,dword [esp-12]
	mov esi,dword [esp-8]
	mov ebx,dword [esp-4]
	ret

;UTF32Char *MyString_StrInt32UTF32(UTF32Char *oriStr, Int32 val)
;0 retAddr
;4 oriStr
;8 val
	align 16
MyString_StrInt32UTF32:
_MyString_StrInt32UTF32:
	mov eax,dword [esp+8] ;val
	mov ecx,dword [esp+4] ;oriStr
	test eax,0x80000000
	jz si32u32lop
	neg eax
	mov dword [ecx],'-'
	lea ecx,[ecx+4]
	jmp si32u32lop

;UTF32Char *MyString_StrUInt32UTF32(UTF32Char *oriStr, UInt32 val)
;0 retAddr
;4 oriStr
;8 val
	align 16
MyString_StrUInt32UTF32:
_MyString_StrUInt32UTF32:
	mov eax,dword [esp+8] ;val
	mov ecx,dword [esp+4] ;oriStr
si32u32lop:
	cmp eax,10
	jb sui32u32lop1
	cmp eax,100
	jb sui32u32lop2
	push esi
	push edi
	mov esi,100
	xor edx,edx
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
	mov edx,dword [eax*8+_MyString_StrDigit100U32+4]
	mov dword [ecx],edx
	mov dword [ecx+4],0
	lea eax,[ecx+4]
	ret
	
	align 16
sui32u32lop2:
	mov edx,dword [eax*8+_MyString_StrDigit100U32]
	mov dword [ecx],edx
	mov edx,dword [eax*8+_MyString_StrDigit100U32+4]
	mov dword [ecx+4],edx
	mov dword [ecx+8],0
	lea eax,[ecx+8]
	ret

	align 16
sui32u32lop3:
	div esi
	mov edi,dword [eax*8+_MyString_StrDigit100U32+4]
	mov dword [ecx],edi
	mov edi,dword [edx*8+_MyString_StrDigit100U32]
	mov dword [ecx+4],edi
	mov edi,dword [edx*8+_MyString_StrDigit100U32+4]
	mov dword [ecx+8],edi
	mov dword [ecx+12],0
	lea eax,[ecx+12]
	jmp sui32u32exit

	align 16
sui32u32lop4:
	div esi
	mov edi,dword [eax*8+_MyString_StrDigit100U32]
	mov dword [ecx],edi
	mov edi,dword [eax*8+_MyString_StrDigit100U32+4]
	mov dword [ecx+4],edi
	mov edi,dword [edx*8+_MyString_StrDigit100U32]
	mov dword [ecx+8],edi
	mov edi,dword [edx*8+_MyString_StrDigit100U32+4]
	mov dword [ecx+12],edi
	mov dword [ecx+16],0
	lea eax,[ecx+16]
	jmp sui32u32exit

	align 16
sui32u32lop5:
	div esi
	mov edi,dword [edx*8+_MyString_StrDigit100U32]
	mov dword [ecx+12],edi
	mov edi,dword [edx*8+_MyString_StrDigit100U32+4]
	xor edx,edx
	mov dword [ecx+16],edi
	div esi
	mov edi,dword [edx*8+_MyString_StrDigit100U32]
	mov dword [ecx+4],edi
	mov edi,dword [edx*8+_MyString_StrDigit100U32+4]
	mov dword [ecx+8],edi
	mov edi,dword [eax*8+_MyString_StrDigit100U32+4]
	mov dword [ecx],edi
	mov dword [ecx+20],0
	lea eax,[ecx+20]
	jmp sui32u32exit

	align 16
sui32u32lop6:
	div esi
	mov edi,dword [edx*8+_MyString_StrDigit100U32]
	mov dword [ecx+16],edi
	mov edi,dword [edx*8+_MyString_StrDigit100U32+4]
	xor edx,edx
	mov dword [ecx+20],edi
	div esi
	mov edi,dword [edx*8+_MyString_StrDigit100U32]
	mov dword [ecx+8],edi
	mov edi,dword [edx*8+_MyString_StrDigit100U32+4]
	mov dword [ecx+12],edi
	mov edi,dword [eax*8+_MyString_StrDigit100U32]
	mov dword [ecx],edi
	mov edi,dword [eax*8+_MyString_StrDigit100U32+4]
	mov dword [ecx+4],edi
	mov dword [ecx+24],0
	lea eax,[ecx+24]
	jmp sui32u32exit

	align 16
sui32u32lop7:
	div esi
	mov edi,dword [edx*8+_MyString_StrDigit100U32]
	mov dword [ecx+20],edi
	mov edi,dword [edx*8+_MyString_StrDigit100U32+4]
	xor edx,edx
	mov dword [ecx+24],edi
	div esi
	mov edi,dword [edx*8+_MyString_StrDigit100U32]
	mov dword [ecx+12],edi
	mov edi,dword [edx*8+_MyString_StrDigit100U32+4]
	xor edx,edx
	mov dword [ecx+16],edi
	div esi
	mov edi,dword [edx*8+_MyString_StrDigit100U32]
	mov dword [ecx+4],edi
	mov edi,dword [edx*8+_MyString_StrDigit100U32+4]
	mov dword [ecx+8],edi
	mov edi,dword [eax*8+_MyString_StrDigit100U32+4]
	mov dword [ecx],edi
	mov dword [ecx+28],0
	lea eax,[ecx+28]
	jmp sui32u32exit

	align 16
sui32u32lop8:
	div esi
	mov edi,dword [edx*8+_MyString_StrDigit100U32]
	mov dword [ecx+24],edi
	mov edi,dword [edx*8+_MyString_StrDigit100U32+4]
	xor edx,edx
	mov dword [ecx+28],edi
	div esi
	mov edi,dword [edx*8+_MyString_StrDigit100U32]
	mov dword [ecx+16],edi
	mov edi,dword [edx*8+_MyString_StrDigit100U32+4]
	xor edx,edx
	mov dword [ecx+20],edi
	div esi
	mov edi,dword [edx*8+_MyString_StrDigit100U32]
	mov dword [ecx+8],edi
	mov edi,dword [edx*8+_MyString_StrDigit100U32+4]
	mov dword [ecx+12],edi
	mov edi,dword [eax*8+_MyString_StrDigit100U32]
	mov dword [ecx],edi
	mov edi,dword [eax*8+_MyString_StrDigit100U32+4]
	mov dword [ecx+4],edi
	mov dword [ecx+32],0
	lea eax,[ecx+32]
	jmp sui32u32exit

	align 16
sui32u32lop9:
	div esi
	mov edi,dword [edx*8+_MyString_StrDigit100U32]
	mov dword [ecx+28],edi
	mov edi,dword [edx*8+_MyString_StrDigit100U32+4]
	xor edx,edx
	mov dword [ecx+32],edi
	div esi
	mov edi,dword [edx*8+_MyString_StrDigit100U32]
	mov dword [ecx+20],edi
	mov edi,dword [edx*8+_MyString_StrDigit100U32+4]
	xor edx,edx
	mov dword [ecx+24],edi
	div esi
	mov edi,dword [edx*8+_MyString_StrDigit100U32]
	mov dword [ecx+12],edi
	mov edi,dword [edx*8+_MyString_StrDigit100U32+4]
	xor edx,edx
	mov dword [ecx+16],edi
	div esi
	mov edi,dword [edx*8+_MyString_StrDigit100U32]
	mov dword [ecx+4],edi
	mov edi,dword [edx*8+_MyString_StrDigit100U32+4]
	mov dword [ecx+8],edi
	mov edi,dword [eax*8+_MyString_StrDigit100U32+4]
	mov dword [ecx],edi
	mov dword [ecx+36],0
	lea eax,[ecx+36]
	jmp sui32u32exit

	align 16
sui32u32lop10:
	div esi
	mov edi,dword [edx*8+_MyString_StrDigit100U32]
	mov dword [ecx+32],edi
	mov edi,dword [edx*8+_MyString_StrDigit100U32+4]
	xor edx,edx
	mov dword [ecx+36],edi
	div esi
	mov edi,dword [edx*8+_MyString_StrDigit100U32]
	mov dword [ecx+24],edi
	mov edi,dword [edx*8+_MyString_StrDigit100U32+4]
	xor edx,edx
	mov dword [ecx+28],edi
	div esi
	mov edi,dword [edx*8+_MyString_StrDigit100U32]
	mov dword [ecx+16],edi
	mov edi,dword [edx*8+_MyString_StrDigit100U32+4]
	xor edx,edx
	mov dword [ecx+20],edi
	div esi
	mov edi,dword [edx*8+_MyString_StrDigit100U32]
	mov dword [ecx+8],edi
	mov edi,dword [edx*8+_MyString_StrDigit100U32+4]
	mov dword [ecx+12],edi
	mov edi,dword [eax*8+_MyString_StrDigit100U32]
	mov dword [ecx],edi
	mov edi,dword [eax*8+_MyString_StrDigit100U32+4]
	mov dword [ecx+4],edi
	mov dword [ecx+40],0
	lea eax,[ecx+40]

	align 16
sui32u32exit:
	pop edi
	pop esi
	ret

;UTF32Char *MyString_StrInt64UTF32(UTF32Char *oriStr, Int64 val)
;-12 edi
;-8 esi
;-4 ebx
;0 retAddr
;4 oriStr
;8 val
	align 16
MyString_StrInt64UTF32:
_MyString_StrInt64UTF32:
	mov eax,dword [esp+8] ;val
	mov ecx,dword [esp+12] ;val[4]
	cmp ecx,0
	jg si64u32lop2
	jl si64u32lop7
	mov ecx,dword [esp+4]
	jmp si32u32lop

	align 16
si64u32lop0:
	mov ecx,dword [esp+4]
	mov dword [ecx],'-'
	lea ecx,[ecx+4]
	jmp si32u32lop

	align 16
si64u32lop7:
	not eax
	not ecx
	add eax,1
	adc ecx,0
	jz si64u32lop0
	mov dword [esp-4],ebx
	mov dword [esp-8],esi
	mov dword [esp-12],edi

	mov ecx,dword [esp+4]
	mov dword [ecx],'-'
	add dword [esp+4],4
	mov ebx,100
	lea esi,[esp-12]
	jmp si64u32lop3

	align 16
si64u32lop2:
	mov dword [esp-12],edi
	mov dword [esp-8],esi
	mov dword [esp-4],ebx
	mov ebx,100
	lea esi,[esp-12]
	align 16
si64u32lop3:
	test ecx,ecx
	jz si64u32lop2b
	xor edx,edx
	xchg eax,ecx
	div ebx
	xchg eax,ecx
	div ebx
	mov edi,dword [edx*8+_MyString_StrDigit100U32]
	lea esi,[esi-8]
	mov dword [esi],edi
	mov edi,dword [edx*8+_MyString_StrDigit100U32+4]
	mov dword [esi+4],edi
	jmp si64u32lop3

	align 16
si64u32lop2b:
	xor edx,edx
	div ebx
	mov ecx,dword [edx*8+_MyString_StrDigit100U32]
	lea esi,[esi-8]
	mov dword [esi],ecx
	mov ecx,dword [edx*8+_MyString_StrDigit100U32+4]
	mov dword [esi+4],ecx
	cmp eax,ebx
	jnb si64u32lop2b

	cmp eax,10
	jb si64u32lop2b2
	mov edi,dword [eax*8+_MyString_StrDigit100U32]
	lea esi,[esi-8]
	mov dword [esi],edi
	mov edi,dword [eax*8+_MyString_StrDigit100U32+4]
	mov dword [esi+4],edi
	jmp si64u32lop2c
	
	align 16
si64u32lop2b2:
	mov eax,dword [eax*8+_MyString_StrDigit100U32+4]
	lea esi,[esi-4]
	mov dword [esi],eax
	align 16
si64u32lop2c:
	cld
	lea ebx,[esp-12]
	mov edi,dword [esp+4] ;oriStr
	align 16
si64u32lop6:
	movsd
	cmp esi,ebx
	jb si64u32lop6
	mov dword [edi],0
	mov eax,edi
	align 16
si64u32exit:
	mov edi,dword [esp-12]
	mov esi,dword [esp-8]
	mov ebx,dword [esp-4]
	ret
	
;UTF32Char *MyString_StrUInt64UTF32(UTF32Char *oriStr, UInt64 val)
;-12 edi
;-8 esi
;-4 ebx
;0 retAddr
;4 oriStr
;8 val
	align 16
MyString_StrUInt64UTF32:
_MyString_StrUInt64UTF32:
	mov eax,dword [esp+8] ;val
	mov ecx,dword [esp+12] ;val[4]
	test ecx,ecx
	jnz sui64u32lop2
	mov ecx,dword [esp+4]
	jmp si32u32lop

	align 16
sui64u32lop2:
	mov dword [esp-12],edi
	mov dword [esp-8],esi
	mov dword [esp-4],ebx
	mov ebx,100
	lea esi,[esp-12]
	align 16
sui64u32lop3:
	xor edx,edx
	xchg eax,ecx
	div ebx
	xchg eax,ecx
	div ebx
	mov edi,dword [edx*8+_MyString_StrDigit100U32]
	mov dword [esi],edi
	mov edi,dword [edx*8+_MyString_StrDigit100U32+4]
	lea esi,[esi-8]
	mov dword [esi+4],edi
	test ecx,ecx
	jnz sui64u32lop3

	align 16
sui64u32lop2b:
	xor edx,edx
	div ebx
	mov ecx,dword [edx*8+_MyString_StrDigit100U32]
	mov dword [esi],ecx
	mov ecx,dword [edx*8+_MyString_StrDigit100U32+4]
	lea esi,[esi-8]
	mov dword [esi+4],ecx
	cmp eax,ebx
	jnb sui64u32lop2b

	cmp eax,10
	jb sui64u32lop2b2
	mov eax,dword [eax*8+_MyString_StrDigit100U32]
	mov dword [esi],eax
	mov eax,dword [eax*8+_MyString_StrDigit100U32+4]
	lea esi,[esi-8]
	mov dword [esi+4],eax
	jmp sui64u32lop2c
	
	align 16
sui64u32lop2b2:
	mov eax,dword [eax*8+_MyString_StrDigit100U32+4]
	lea esi,[esi-4]
	mov dword [esi],eax
	align 16
sui64u32lop2c:
	cld
	lea ecx,[esp-12]
	mov edi,dword [esp+4] ;oriStr
	sub ecx,esi
	shr ecx,2
	rep movsd
	mov dword [edi],0
	mov eax,edi
	align 16
sui64u32exit:
	mov edi,dword [esp-12]
	mov esi,dword [esp-8]
	mov ebx,dword [esp-4]
	ret

;IntOS MyString_StrCompare(const Char *str1, const Char *str2)
;0 rsi
;4 rdi
;8 retAddr
;12 str1
;16 str2
	align 16
MyString_StrCompare:
_MyString_StrCompare:
	push edi
	push esi
	mov esi,dword [esp+12]
	mov edi,dword [esp+16]
	
	align 16
scmplop:
	movzx ecx,byte [esi]
	movzx edx,byte [edi]
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
	
	xor eax,eax
	xor edx,edx
	align 16
scmplop6:
	movzx ecx,byte [esi]
	cmp cl,'0'
	jb scmplop7
	cmp cl,'9'
	ja scmplop7
	lea eax,[eax+eax*4]
	lea esi,[esi+1]
	lea eax,[ecx+eax*2-48]
	jmp scmplop6

	align 16
scmplop2:
	test dl,dl
	jz scmplop4
	mov eax,1
	jmp scmpret
	
	align 16
scmplop3:
	mov eax,-1
	jmp scmpret
	
	align 16
scmplop4:
	mov eax,0
	jmp scmpret

	align 16
scmplop7:
	movzx ecx,byte [edi]
	cmp cl,'0'
	jb scmplop8
	cmp cl,'9'
	ja scmplop8
	lea edx,[edx+edx*4]
	lea edi,[edi+1]
	lea edx,[ecx+edx*2-48]
	jmp scmplop7
	
	align 16
scmplop8:
	cmp eax,edx
	jz scmplop
	ja scmplop10
scmplop9:
	mov eax,-1
	jmp scmpret
	
	align 16
scmplop10:
	mov eax,1
	jmp scmpret
	
	align 16
scmplop5:
	cmp cl,dl
	ja scmplop10
	jb scmplop9
	lea esi,[esi+1]
	lea edi,[edi+1]
	jmp scmplop

	align 16	
scmpret:
	pop esi
	pop edi
	ret

;IntOS MyString_StrCompareICase(const Char *str1, const Char *str2)
;0 esi
;4 edi
;8 retAddr
;12 str1
;16 str2
	align 16
MyString_StrCompareICase:
_MyString_StrCompareICase:
	push edi
	push esi
	mov esi,dword [esp+12]
	mov edi,dword [esp+16]

	align 16
scmpiclop:
	movzx ecx,byte [esi]
	movzx edx,byte [edi]
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
	
	xor eax,eax
	xor edx,edx
	align 16
scmpiclop6:
	movzx ecx,byte [esi]
	cmp cl,'0'
	jb scmpiclop7
	cmp cl,'9'
	ja scmpiclop7
	lea eax,[eax+eax*4]
	lea esi,[esi+1]
	lea eax,[ecx+eax*2-48]
	jmp scmpiclop6

	align 16
scmpiclop2:
	test dl,dl
	jz scmpiclop4
	mov eax,1
	jmp scmpicret
	
	align 16
scmpiclop3:
	mov eax,-1
	jmp scmpicret
	
	align 16
scmpiclop4:
	mov eax,0
	jmp scmpicret

	align 16
scmpiclop7:
	movzx ecx,byte [edi]
	cmp cl,'0'
	jb scmpiclop8
	cmp cl,'9'
	ja scmpiclop8
	lea edx,[edx+edx*4]
	lea edi,[edi+1]
	lea edx,[ecx+edx*2-48]
	jmp scmpiclop7
	
	align 16
scmpiclop8:
	cmp eax,edx
	jz scmpiclop
	ja scmpiclop10
	
scmpiclop9:
	mov eax,-1
	jmp scmpicret
	
	align 16
scmpiclop10:
	mov eax,1
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
	lea esi,[esi+1]
	lea edi,[edi+1]
	jmp scmpiclop
	
	align 16
scmpicret:
	pop esi
	pop edi
	ret

;IntOS MyString_StrCompareUTF16(const UTF16Char *str1, const UTF16Char *str2)
;0 esi
;4 edi
;8 retAddr
;12 str1
;16 str2
	align 16
MyString_StrCompareUTF16:
_MyString_StrCompareUTF16:
	push edi
	push esi
	mov esi,dword [esp+12]
	mov edi,dword [esp+16]
	
	align 16
scmpu16lop:
	movzx ecx,word [esi]
	movzx edx,word [edi]
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
	
	xor eax,eax
	xor edx,edx
	align 16
scmpu16lop6:
	movzx ecx,word [esi]
	cmp cx,'0'
	jb scmpu16lop7
	cmp cx,'9'
	ja scmpu16lop7
	lea eax,[eax+eax*4]
	lea esi,[esi+2]
	lea eax,[ecx+eax*2-48]
	jmp scmpu16lop6

	align 16
scmpu16lop2:
	test dx,dx
	jz scmpu16lop4
	mov eax,1
	jmp scmpu16ret
	
	align 16
scmpu16lop3:
	mov eax,-1
	jmp scmpu16ret
	
	align 16
scmpu16lop4:
	mov eax,0
	jmp scmpu16ret

	align 16
scmpu16lop7:
	movzx ecx,word [edi]
	cmp cx,'0'
	jb scmpu16lop8
	cmp cx,'9'
	ja scmpu16lop8
	lea edx,[edx+edx*4]
	lea edi,[edi+2]
	lea edx,[ecx+edx*2-48]
	jmp scmpu16lop7
	
	align 16
scmpu16lop8:
	cmp eax,edx
	jz scmpu16lop
	ja scmpu16lop10
scmpu16lop9:
	mov eax,-1
	jmp scmpu16ret
	
	align 16
scmpu16lop10:
	mov eax,1
	jmp scmpu16ret
	
	align 16
scmpu16lop5:
	cmp cx,dx
	ja scmpu16lop10
	jb scmpu16lop9
	lea esi,[esi+2]
	lea edi,[edi+2]
	jmp scmpu16lop

	align 16	
scmpu16ret:
	pop esi
	pop edi
	ret

;IntOS MyString_StrCompareICaseUTF16(const UTF16Char *str1, const WChar *str2)
;0 esi
;4 edi
;8 retAddr
;12 str1
;16 str2
	align 16
MyString_StrCompareICaseUTF16:
_MyString_StrCompareICaseUTF16:
	push edi
	push esi
	mov esi,dword [esp+12]
	mov edi,dword [esp+16]

	align 16
scmpicu16lop:
	movzx ecx,word [esi]
	movzx edx,word [edi]
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
	
	xor eax,eax
	xor edx,edx
	align 16
scmpicu16lop6:
	movzx ecx,word [esi]
	cmp cx,'0'
	jb scmpicu16lop7
	cmp cx,'9'
	ja scmpicu16lop7
	lea eax,[eax+eax*4]
	lea esi,[esi+2]
	lea eax,[ecx+eax*2-48]
	jmp scmpicu16lop6

	align 16
scmpicu16lop2:
	test dx,dx
	jz scmpicu16lop4
	mov eax,1
	jmp scmpicu16ret
	
	align 16
scmpicu16lop3:
	mov eax,-1
	jmp scmpicu16ret
	
	align 16
scmpicu16lop4:
	mov eax,0
	jmp scmpicu16ret

	align 16
scmpicu16lop7:
	movzx ecx,word [edi]
	cmp cx,'0'
	jb scmpicu16lop8
	cmp cx,'9'
	ja scmpicu16lop8
	lea edx,[edx+edx*4]
	lea edi,[edi+2]
	lea edx,[ecx+edx*2-48]
	jmp scmpicu16lop7
	
	align 16
scmpicu16lop8:
	cmp eax,edx
	jz scmpicu16lop
	ja scmpicu16lop10
	
scmpicu16lop9:
	mov eax,-1
	jmp scmpicu16ret
	
	align 16
scmpicu16lop10:
	mov eax,1
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
	lea esi,[esi+2]
	lea edi,[edi+2]
	jmp scmpicu16lop
	
	align 16
scmpicu16ret:
	pop esi
	pop edi
	ret

;IntOS MyString_StrCompareUTF32(const UTF32Char *str1, const UTF32Char *str2)
;0 esi
;4 edi
;8 retAddr
;12 str1
;16 str2
	align 16
MyString_StrCompareUTF32:
_MyString_StrCompareUTF32:
	push edi
	push esi
	mov esi,dword [esp+12]
	mov edi,dword [esp+16]
	
	align 16
scmpu32lop:
	mov ecx,dword [esi]
	mov edx,dword [edi]
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
	
	xor eax,eax
	xor edx,edx
	align 16
scmpu32lop6:
	mov ecx,dword [esi]
	cmp ecx,'0'
	jb scmpu32lop7
	cmp ecx,'9'
	ja scmpu32lop7
	lea eax,[eax+eax*4]
	lea esi,[esi+4]
	lea eax,[ecx+eax*2-48]
	jmp scmpu32lop6

	align 16
scmpu32lop2:
	test edx,edx
	jz scmpu32lop4
	mov eax,1
	jmp scmpu32ret
	
	align 16
scmpu32lop3:
	mov eax,-1
	jmp scmpu32ret
	
	align 16
scmpu32lop4:
	mov eax,0
	jmp scmpu32ret

	align 16
scmpu32lop7:
	mov ecx,dword [edi]
	cmp ecx,'0'
	jb scmpu32lop8
	cmp ecx,'9'
	ja scmpu32lop8
	lea edx,[edx+edx*4]
	lea edi,[edi+4]
	lea edx,[ecx+edx*2-48]
	jmp scmpu32lop7
	
	align 16
scmpu32lop8:
	cmp eax,edx
	jz scmpu32lop
	ja scmpu32lop10
scmpu32lop9:
	mov eax,-1
	jmp scmpu32ret
	
	align 16
scmpu32lop10:
	mov eax,1
	jmp scmpu32ret
	
	align 16
scmpu32lop5:
	cmp ecx,edx
	ja scmpu32lop10
	jb scmpu32lop9
	lea esi,[esi+4]
	lea edi,[edi+4]
	jmp scmpu32lop

	align 16	
scmpu32ret:
	pop esi
	pop edi
	ret

;IntOS MyString_StrCompareICaseUTF32(const UTF32Char *str1, const UTF32Char *str2)
;0 esi
;4 edi
;8 retAddr
;12 str1
;16 str2
	align 16
MyString_StrCompareICaseUTF32:
_MyString_StrCompareICaseUTF32:
	push edi
	push esi
	mov esi,dword [esp+12]
	mov edi,dword [esp+16]

	align 16
scmpicu32lop:
	mov ecx,dword [esi]
	mov edx,dword [edi]
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
	
	xor eax,eax
	xor edx,edx
	align 16
scmpicu32lop6:
	mov ecx,dword [esi]
	cmp ecx,'0'
	jb scmpicu16lop7
	cmp ecx,'9'
	ja scmpicu32lop7
	lea eax,[eax+eax*4]
	lea esi,[esi+4]
	lea eax,[ecx+eax*2-48]
	jmp scmpicu32lop6

	align 16
scmpicu32lop2:
	test edx,edx
	jz scmpicu32lop4
	mov eax,1
	jmp scmpicu32ret
	
	align 16
scmpicu32lop3:
	mov eax,-1
	jmp scmpicu32ret
	
	align 16
scmpicu32lop4:
	mov eax,0
	jmp scmpicu32ret

	align 16
scmpicu32lop7:
	mov ecx,dword [edi]
	cmp ecx,'0'
	jb scmpicu32lop8
	cmp ecx,'9'
	ja scmpicu32lop8
	lea edx,[edx+edx*4]
	lea edi,[edi+4]
	lea edx,[ecx+edx*2-48]
	jmp scmpicu32lop7
	
	align 16
scmpicu32lop8:
	cmp eax,edx
	jz scmpicu32lop
	ja scmpicu32lop10
	
scmpicu32lop9:
	mov eax,-1
	jmp scmpicu32ret
	
	align 16
scmpicu32lop10:
	mov eax,1
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
	lea esi,[esi+4]
	lea edi,[edi+4]
	jmp scmpicu32lop
	
	align 16
scmpicu32ret:
	pop esi
	pop edi
	ret

;IntOS MyString_StrCharCnt(const Char *s)
;0 retAdddr
;4 s
	align 16
MyString_StrCharCnt:
_MyString_StrCharCnt:
	mov eax,dword [esp+4]
	mov ecx,eax
	align 16
scclop:
	movzx edx,byte [eax]
	lea eax,[eax+1]
	test dl,dl
	jnz scclop
	sub eax,ecx
	dec eax
	ret

;IntOS MyString_StrCharCntUTF16(const UTF16Char *s)
;0 retAddr
;4 s
	align 16
MyString_StrCharCntUTF16:
_MyString_StrCharCntUTF16:
	mov eax,dword [esp+4]
	mov ecx,eax
	align 16
sccu16lop:
	movzx edx,word [eax]
	lea eax,[eax+2]
	test dx,dx
	jnz sccu16lop
	sub eax,ecx
	shr eax,1
	dec eax
	ret

;IntOS MyString_StrCharCntUTF32(const UTF32Char *s)
;0 retAddr
;4 s
	align 16
MyString_StrCharCntUTF32:
_MyString_StrCharCntUTF32:
	mov eax,dword [esp+4]
	mov ecx,eax
	align 16
sccu32lop:
	mov edx,dword [eax]
	lea eax,[eax+4]
	test edx,edx
	jnz sccu32lop
	sub eax,ecx
	shr eax,2
	dec eax
	ret

;UTF16Char *MyString_StrHexVal64VUTF16(UTF16Char *oriStr, Int64 val)
;0 retAddr
;4 oriStr
;8 val
	align 16
MyString_StrHexVal64VUTF16:
_MyString_StrHexVal64VUTF16:
	mov edx,dword [esp+4] ;oriStr
	mov eax,dword [esp+12] ;val[4]
	test eax,eax
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
	mov ecx,eax
	shr ecx,24
	mov cx,word [ecx*4+_MyString_StrHexArrU16+2]
	mov word [edx],cx
	lea edx,[edx+2]
	jmp shv64vu16lop14
	
	align 16
shv64vu16lop13:
	mov ecx,eax
	shr ecx,16
	movzx ecx,cl
	mov cx,word [ecx*4+_MyString_StrHexArrU16+2]
	mov word [edx],cx
	lea edx,[edx+2]
	jmp shv64vu16lop12
	
	align 16
shv64vu16lop11:
	movzx ecx,ah
	mov cx,word [ecx*4+_MyString_StrHexArrU16+2]
	mov word [edx],cx
	lea edx,[edx+2]
	jmp shv64vu16lop10
	
	align 16
shv64vu16lop9:
	mov cx,word [eax*4+_MyString_StrHexArrU16+2]
	mov word [edx],cx
	lea edx,[edx+2]
	jmp shv64vu16lop8
	
	align 16
shv64vu16lop8v:
	mov eax,dword [esp+8] ;val
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
	
	mov ecx,eax
	shr ecx,24
	mov ecx,dword [ecx*4+_MyString_StrHexArrU16]
	mov dword [edx],ecx
	lea edx,[edx+4]
	jmp shv64vu16lop6

	align 16
shv64vu16lop7:
	mov ecx,eax
	shr ecx,24
	mov cx,word [ecx*4+_MyString_StrHexArrU16+2]
	mov word [edx],cx
	lea edx,[edx+2]
	jmp shv64vu16lop6
	
	align 16
shv64vu16lop5:
	mov ecx,eax
	shr ecx,16
	movzx ecx,cl
	mov cx,word [ecx*4+_MyString_StrHexArrU16+2]
	mov word [edx],cx
	lea edx,[edx+2]
	jmp shv64vu16lop4
	
	align 16
shv64vu16lop3:
	movzx ecx,ah
	mov cx,word [ecx*4+_MyString_StrHexArrU16+2]
	mov word [edx],cx
	lea edx,[edx+2]
	jmp shv64vu16lop2
	
	align 16
shv64vu16lop1:
	mov cx,word [eax*4+_MyString_StrHexArrU16+2]
	mov word [edx],cx
	lea edx,[edx+2]
	mov word [edx],0
	jmp shv64vu16exit
	
	align 16
shv64vu16lop16:
	mov ecx,eax
	shr ecx,24
	mov ecx,dword [ecx*4+_MyString_StrHexArrU16]
	mov dword [edx],ecx
	lea edx,[edx+4]
	align 16
shv64vu16lop14:
	mov ecx,eax
	shr ecx,16
	movzx ecx,cl
	mov ecx,dword [ecx*4+_MyString_StrHexArrU16]
	mov dword [edx],ecx
	lea edx,[edx+4]
	align 16
shv64vu16lop12:
	movzx ecx,ah
	mov ecx,dword [ecx*4+_MyString_StrHexArrU16]
	mov dword [edx],ecx
	lea edx,[edx+4]
	align 16
shv64vu16lop10:
	movzx ecx,al
	mov ecx,dword [ecx*4+_MyString_StrHexArrU16]
	mov dword [edx],ecx
	lea edx,[edx+4]
	align 16
shv64vu16lop8:
	mov eax,dword [esp+8]
	
	mov ecx,eax
	shr ecx,24
	mov ecx,dword [ecx*4+_MyString_StrHexArrU16]
	mov dword [edx],ecx
	lea edx,[edx+4]
	align 16
shv64vu16lop6:
	mov ecx,eax
	shr ecx,16
	movzx ecx,cl
	mov ecx,dword [ecx*4+_MyString_StrHexArrU16]
	mov dword [edx],ecx
	lea edx,[edx+4]
	align 16
shv64vu16lop4:
	movzx ecx,ah
	mov ecx,dword [ecx*4+_MyString_StrHexArrU16]
	mov dword [edx],ecx
	lea edx,[edx+4]
	align 16
shv64vu16lop2:
	movzx ecx,al
	mov ecx,dword [ecx*4+_MyString_StrHexArrU16]
	mov dword [edx],ecx
	lea edx,[edx+4]
	mov word [edx],0
	align 16
shv64vu16exit:
	mov eax,edx
	ret

;UTF16Char *MyString_StrHexVal32VUTF16(UTF16Char *oriStr, Int32 val)
;0 retAddr
;4 oriStr
;8 val
	align 16
MyString_StrHexVal32VUTF16:
_MyString_StrHexVal32VUTF16:
	mov edx,dword [esp+4] ;oriStr
	jmp shv64vu16lop8v

;UTF32Char *MyString_StrHexVal64VUTF32(UTF32Char *oriStr, Int64 val)
;0 retAddr
;4 oriStr
;8 val
	align 16
MyString_StrHexVal64VUTF32:
_MyString_StrHexVal64VUTF32:
	push ebx
	mov edx,dword [esp+4] ;oriStr
	mov eax,dword [esp+12] ;val[4]
	test eax,eax
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
	mov ecx,eax
	shr ecx,24
	mov ecx,dword [ecx*8+_MyString_StrHexArrU32+4]
	mov dword [edx],ecx
	lea edx,[edx+4]
	jmp shv64vu32lop14
	
	align 16
shv64vu32lop13:
	mov ecx,eax
	shr ecx,16
	movzx ecx,cl
	mov ecx,dword [ecx*8+_MyString_StrHexArrU32+4]
	mov dword [edx],ecx
	lea edx,[edx+4]
	jmp shv64vu32lop12
	
	align 16
shv64vu32lop11:
	movzx ecx,ah
	mov ecx,dword [ecx*8+_MyString_StrHexArrU32+4]
	mov dword [edx],ecx
	lea edx,[edx+4]
	jmp shv64vu16lop10
	
	align 16
shv64vu32lop9:
	mov ecx,dword [eax*8+_MyString_StrHexArrU32+4]
	mov dword [edx],ecx
	lea edx,[edx+4]
	jmp shv64vu16lop8
	
	align 16
shv64vu32lop8v:
	mov eax,dword [esp+8] ;val
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
	
	mov ecx,eax
	shr ecx,24
	mov ebx,dword [ecx*8+_MyString_StrHexArrU32]
	mov dword [edx],ebx
	mov ebx,dword [ecx*8+_MyString_StrHexArrU32+4]
	mov dword [edx+4],ebx
	lea edx,[edx+8]
	jmp shv64vu32lop6

	align 16
shv64vu32lop7:
	mov ecx,eax
	shr ecx,24
	mov ecx,dword [ecx*8+_MyString_StrHexArrU32+4]
	mov dword [edx],ecx
	lea edx,[edx+4]
	jmp shv64vu32lop6
	
	align 16
shv64vu32lop5:
	mov ecx,eax
	shr ecx,16
	movzx ecx,cl
	mov ecx,dword [ecx*8+_MyString_StrHexArrU32+4]
	mov dword [edx],ecx
	lea edx,[edx+4]
	jmp shv64vu32lop4
	
	align 16
shv64vu32lop3:
	movzx ecx,ah
	mov ecx,dword [ecx*8+_MyString_StrHexArrU32+4]
	mov dword [edx],ecx
	lea edx,[edx+4]
	jmp shv64vu32lop2
	
	align 16
shv64vu32lop1:
	mov ecx,dword [eax*8+_MyString_StrHexArrU32+4]
	mov dword [edx],ecx
	lea edx,[edx+4]
	mov dword [edx],0
	jmp shv64vu32exit
	
	align 16
shv64vu32lop16:
	mov ecx,eax
	shr ecx,24
	mov ebx,dword [ecx*8+_MyString_StrHexArrU32]
	mov dword [edx],ebx
	mov ebx,dword [ecx*8+_MyString_StrHexArrU32+4]
	mov dword [edx+4],ebx
	lea edx,[edx+8]
	align 16
shv64vu32lop14:
	mov ecx,eax
	shr ecx,16
	movzx ecx,cl
	mov ebx,dword [ecx*8+_MyString_StrHexArrU32]
	mov dword [edx],ebx
	mov ebx,dword [ecx*8+_MyString_StrHexArrU32+4]
	mov dword [edx+4],ebx
	lea edx,[edx+8]
	align 16
shv64vu32lop12:
	movzx ecx,ah
	mov ebx,dword [ecx*8+_MyString_StrHexArrU32]
	mov dword [edx],ebx
	mov ebx,dword [ecx*8+_MyString_StrHexArrU32+4]
	mov dword [edx+4],ebx
	lea edx,[edx+8]
	align 16
shv64vu32lop10:
	movzx ecx,al
	mov ebx,dword [ecx*8+_MyString_StrHexArrU32]
	mov dword [edx],ebx
	mov ebx,dword [ecx*8+_MyString_StrHexArrU32+4]
	mov dword [edx+4],ebx
	lea edx,[edx+8]
	align 16
shv64vu32lop8:
	mov eax,dword [esp+8]
	
	mov ecx,eax
	shr ecx,24
	mov ebx,dword [ecx*8+_MyString_StrHexArrU32]
	mov dword [edx],ebx
	mov ebx,dword [ecx*8+_MyString_StrHexArrU32+4]
	mov dword [edx+4],ebx
	lea edx,[edx+8]
	align 16
shv64vu32lop6:
	mov ecx,eax
	shr ecx,16
	movzx ecx,cl
	mov ebx,dword [ecx*8+_MyString_StrHexArrU32]
	mov dword [edx],ebx
	mov ebx,dword [ecx*8+_MyString_StrHexArrU32+4]
	mov dword [edx+4],ebx
	lea edx,[edx+8]
	align 16
shv64vu32lop4:
	movzx ecx,ah
	mov ebx,dword [ecx*8+_MyString_StrHexArrU32]
	mov dword [edx],ebx
	mov ebx,dword [ecx*8+_MyString_StrHexArrU32+4]
	mov dword [edx+4],ebx
	lea edx,[edx+8]
	align 16
shv64vu32lop2:
	movzx ecx,al
	mov ebx,dword [ecx*8+_MyString_StrHexArrU32]
	mov dword [edx],ebx
	mov ebx,dword [ecx*8+_MyString_StrHexArrU32+4]
	mov dword [edx+4],ebx
	lea edx,[edx+8]
	mov dword [edx],0
	align 16
shv64vu32exit:
	mov eax,edx
	pop ebx
	ret

;UTF32Char *MyString_StrHexVal32VUTF32(UTF32Char *oriStr, Int32 val)
;0 retAddr
;4 oriStr
;8 val
	align 16
MyString_StrHexVal32VUTF32:
_MyString_StrHexVal32VUTF32:
	push ebx
	mov edx,dword [esp+4] ;oriStr
	jmp shv64vu32lop8v

;UTF16Char *MyString_StrHexVal64UTF16(UTF16Char *oriStr, Int64 val)
;0 retAddr
;4 oriStr
;8 val
	align 16
MyString_StrHexVal64UTF16:
_MyString_StrHexVal64UTF16:
	mov edx,dword [esp+4] ;oriStr
	mov ecx,dword [esp+8] ;val
	mov word [edx+32],0

	movzx eax,cl
	mov eax,dword [eax*4+_MyString_StrHexArrU16]
	mov dword [edx+28],eax
	shr ecx,8
	movzx eax,cl
	mov eax,dword [eax*4+_MyString_StrHexArrU16]
	mov dword [edx+24],eax
	shr ecx,8
	movzx eax,cl
	mov eax,dword [eax*4+_MyString_StrHexArrU16]
	mov dword [edx+20],eax
	shr ecx,8
	movzx eax,cl
	mov eax,dword [eax*4+_MyString_StrHexArrU16]
	mov ecx,dword [esp+12] ;val[4]
	mov dword [edx+16],eax

	movzx eax,cl
	mov eax,dword [eax*4+_MyString_StrHexArrU16]
	mov dword [edx+12],eax
	shr ecx,8
	movzx eax,cl
	mov eax,dword [eax*4+_MyString_StrHexArrU16]
	mov dword [edx+8],eax
	shr ecx,8
	movzx eax,cl
	mov eax,dword [eax*4+_MyString_StrHexArrU16]
	mov dword [edx+4],eax
	shr ecx,8
	movzx eax,cl
	mov eax,dword [eax*4+_MyString_StrHexArrU16]
	mov dword [edx+0],eax

	lea eax,[edx+32]
	ret

;UTF32Char *MyString_StrHexVal64UTF32(UTF32Char *oriStr, Int64 val)
;0 retAddr
;4 oriStr
;8 val
	align 16
MyString_StrHexVal64UTF32:
_MyString_StrHexVal64UTF32:
	push ebx
	mov edx,dword [esp+4] ;oriStr
	mov ecx,dword [esp+8] ;val
	mov word [edx+64],0

	movzx eax,cl
	mov ebx,dword [eax*8+_MyString_StrHexArrU32]
	mov dword [edx+56],ebx
	mov ebx,dword [eax*8+_MyString_StrHexArrU32+4]
	mov dword [edx+60],ebx
	shr ecx,8
	movzx eax,cl
	mov ebx,dword [eax*8+_MyString_StrHexArrU32]
	mov dword [edx+48],ebx
	mov ebx,dword [eax*8+_MyString_StrHexArrU32+4]
	mov dword [edx+52],ebx
	shr ecx,8
	movzx eax,cl
	mov ebx,dword [eax*8+_MyString_StrHexArrU32]
	mov dword [edx+40],ebx
	mov ebx,dword [eax*8+_MyString_StrHexArrU32+4]
	mov dword [edx+44],ebx
	shr ecx,8
	movzx eax,cl
	mov ebx,dword [eax*8+_MyString_StrHexArrU32]
	mov dword [edx+32],ebx
	mov ebx,dword [eax*8+_MyString_StrHexArrU32+4]
	mov dword [edx+36],ebx
	mov ecx,dword [esp+12] ;val[4]

	movzx eax,cl
	mov ebx,dword [eax*8+_MyString_StrHexArrU32]
	mov dword [edx+24],ebx
	mov ebx,dword [eax*8+_MyString_StrHexArrU32+4]
	mov dword [edx+28],ebx
	shr ecx,8
	movzx eax,cl
	mov ebx,dword [eax*8+_MyString_StrHexArrU32]
	mov dword [edx+16],ebx
	mov ebx,dword [eax*8+_MyString_StrHexArrU32+4]
	mov dword [edx+20],ebx
	shr ecx,8
	movzx eax,cl
	mov ebx,dword [eax*8+_MyString_StrHexArrU32]
	mov dword [edx+8],ebx
	mov ebx,dword [eax*8+_MyString_StrHexArrU32+4]
	mov dword [edx+12],ebx
	shr ecx,8
	movzx eax,cl
	mov ebx,dword [eax*8+_MyString_StrHexArrU32]
	mov dword [edx+0],ebx
	mov ebx,dword [eax*8+_MyString_StrHexArrU32+4]
	mov dword [edx+4],ebx

	lea eax,[edx+64]
	pop ebx
	ret

;UTF16Char *MyString_StrHexVal32UTF16(UTF16Char *oriStr, Int32 val)
;0 retAddr
;4 oriStr
;8 val
	align 16
MyString_StrHexVal32UTF16:
_MyString_StrHexVal32UTF16:
	mov edx,dword [esp+4] ;oriStr
	mov eax,dword [esp+8] ;val
	mov word [edx+16],0

	movzx ecx,al
	mov ecx,dword [ecx*4+_MyString_StrHexArrU16]
	mov dword [edx+12],ecx
	shr eax,8
	movzx ecx,al
	mov ecx,dword [ecx*4+_MyString_StrHexArrU16]
	mov dword [edx+8],ecx
	shr eax,8
	movzx ecx,al
	mov ecx,dword [ecx*4+_MyString_StrHexArrU16]
	mov dword [edx+4],ecx
	shr eax,8
	movzx ecx,al
	mov ecx,dword [ecx*4+_MyString_StrHexArrU16]
	mov dword [edx+0],ecx

	lea eax,[edx+16]
	ret
	
;UTF32Char *MyString_StrHexVal32UTF32(UTF32Char *oriStr, Int32 val)
;0 retAddr
;4 oriStr
;8 val
	align 16
MyString_StrHexVal32UTF32:
_MyString_StrHexVal32UTF32:
	push ebx
	mov edx,dword [esp+4] ;oriStr
	mov eax,dword [esp+8] ;val
	mov word [edx+32],0

	movzx ecx,al
	mov ebx,dword [ecx*8+_MyString_StrHexArrU32]
	mov dword [edx+24],ebx
	mov ebx,dword [ecx*8+_MyString_StrHexArrU32+4]
	mov dword [edx+28],ebx
	shr eax,8
	movzx ecx,al
	mov ebx,dword [ecx*8+_MyString_StrHexArrU32]
	mov dword [edx+16],ebx
	mov ebx,dword [ecx*8+_MyString_StrHexArrU32+4]
	mov dword [edx+20],ebx
	shr eax,8
	movzx ecx,al
	mov ebx,dword [ecx*8+_MyString_StrHexArrU32]
	mov dword [edx+8],ebx
	mov ebx,dword [ecx*8+_MyString_StrHexArrU32+4]
	mov dword [edx+12],ebx
	shr eax,8
	movzx ecx,al
	mov ebx,dword [ecx*8+_MyString_StrHexArrU32]
	mov dword [edx+0],ebx
	mov ebx,dword [ecx*8+_MyString_StrHexArrU32+4]
	mov dword [edx+4],ebx

	lea eax,[edx+32]
	pop ebx
	ret
	
;UTF16Char *MyString_StrHexVal24UTF16(UTF16Char *oriStr, Int32 val)
;0 retAddr
;4 oriStr
;8 val
	align 16
MyString_StrHexVal24UTF16:
_MyString_StrHexVal24UTF16:
	mov edx,dword [esp+4] ;oriStr
	mov eax,dword [esp+8] ;val
	mov word [edx+12],0

	movzx ecx,al
	mov ecx,dword [ecx*4+_MyString_StrHexArrU16]
	mov dword [edx+8],ecx
	shr eax,8
	movzx ecx,al
	mov ecx,dword [ecx*4+_MyString_StrHexArrU16]
	mov dword [edx+4],ecx
	shr eax,8
	movzx ecx,al
	mov ecx,dword [ecx*4+_MyString_StrHexArrU16]
	mov dword [edx+0],ecx

	lea eax,[edx+12]
	ret

;UTF32Char *MyString_StrHexVal24UTF32(UTF32Char *oriStr, Int32 val)
;0 retAddr
;4 oriStr
;8 val
	align 16
MyString_StrHexVal24UTF32:
_MyString_StrHexVal24UTF32:
	push ebx
	mov edx,dword [esp+4] ;oriStr
	mov eax,dword [esp+8] ;val
	mov dword [edx+24],0

	movzx ecx,al
	mov ebx,dword [ecx*8+_MyString_StrHexArrU32]
	mov dword [edx+16],ebx
	mov ebx,dword [ecx*8+_MyString_StrHexArrU32+4]
	mov dword [edx+20],ebx
	shr eax,8
	movzx ecx,al
	mov ebx,dword [ecx*8+_MyString_StrHexArrU32]
	mov dword [edx+8],ebx
	mov ebx,dword [ecx*8+_MyString_StrHexArrU32+4]
	mov dword [edx+12],ebx
	shr eax,8
	movzx ecx,al
	mov ebx,dword [ecx*8+_MyString_StrHexArrU32]
	mov dword [edx+0],ebx
	mov ebx,dword [ecx*8+_MyString_StrHexArrU32+4]
	mov dword [edx+4],ebx

	lea eax,[edx+24]
	pop ebx
	ret


;Int32 MyString_StrToInt32UTF8(const UTF8Char *intStr)
;0 retAddr
;4 intStr
	align 16
MyString_StrToInt32UTF8:
_MyString_StrToInt32UTF8:
	xor eax,eax
	mov edx,dword [esp+4] ;intStr
	cmp byte [edx], 0x2d
	jnz sti32u8lop
	lea edx,[edx+1]
	
	align 16
sti32u8lop5:
	movzx ecx,byte [edx]
	cmp ecx,0x3a
	jnb sti32u8lop8
	cmp ecx,0x30
	jb sti32u8lop8
	lea edx,[edx+1]
	lea eax,[eax*4+eax]
	lea eax,[eax*2+ecx-0x30]
	jmp sti32u8lop5

	align 16
sti32u8lop6:
	neg eax
	jmp sti32u8lop4

	align 16
sti32u8lop8:
	test ecx,ecx
	jz sti32u8lop6
	xor eax,eax
	jmp sti32u8lop4

	align 16
sti32u8lop:
	movzx ecx,byte [edx]
	cmp ecx,0x3a
	jnb sti32u16lop2
	cmp ecx,0x30
	jb sti32u8lop2
	lea edx,[edx+1]
	lea eax,[eax*4+eax]
	lea eax,[eax*2+ecx-0x30]
	jmp sti32u8lop

	align 16
sti32u8lop2:
	test ecx,ecx
	jz sti32u8lop4
	xor eax,eax
	align 16
sti32u8lop4:
	ret

;Int32 MyString_StrToInt32UTF16(const UTF16Char *intStr)
;0 retAddr
;4 intStr
	align 16
MyString_StrToInt32UTF16:
_MyString_StrToInt32UTF16:
	xor eax,eax
	mov edx,dword [esp+4] ;intStr
	cmp word [edx], 0x2d
	jnz sti32u16lop
	lea edx,[edx+2]
	
	align 16
sti32u16lop5:
	movzx ecx,word [edx]
	cmp ecx,0x3a
	jnb sti32u16lop8
	cmp ecx,0x30
	jb sti32u16lop8
	lea edx,[edx+2]
	lea eax,[eax*4+eax]
	lea eax,[eax*2+ecx-0x30]
	jmp sti32u16lop5

	align 16
sti32u16lop6:
	neg eax
	jmp sti32u16lop4

	align 16
sti32u16lop8:
	test ecx,ecx
	jz sti32u16lop6
	xor eax,eax
	jmp sti32u16lop4

	align 16
sti32u16lop:
	movzx ecx,word [edx]
	cmp ecx,0x3a
	jnb sti32u16lop2
	cmp ecx,0x30
	jb sti32u16lop2
	lea edx,[edx+2]
	lea eax,[eax*4+eax]
	lea eax,[eax*2+ecx-0x30]
	jmp sti32u16lop

	align 16
sti32u16lop2:
	test ecx,ecx
	jz sti32u16lop4
	xor eax,eax
	align 16
sti32u16lop4:
	ret

;Int32 MyString_StrToInt32UTF32(const UTF32Char *intStr)
;0 retAddr
;4 intStr
	align 16
MyString_StrToInt32UTF32:
_MyString_StrToInt32UTF32:
	xor eax,eax
	mov edx,dword [esp+4] ;intStr
	cmp dword [edx], 0x2d
	jnz sti32u32lop
	lea edx,[edx+4]
	
	align 16
sti32u32lop5:
	mov ecx,dword [edx]
	cmp ecx,0x3a
	jnb sti32u32lop8
	cmp ecx,0x30
	jb sti32u32lop8
	lea edx,[edx+4]
	lea eax,[eax*4+eax]
	lea eax,[eax*2+ecx-0x30]
	jmp sti32u32lop5

	align 16
sti32u32lop6:
	neg eax
	jmp sti32u32lop4

	align 16
sti32u32lop8:
	test ecx,ecx
	jz sti32u32lop6
	xor eax,eax
	jmp sti32u32lop4

	align 16
sti32u32lop:
	mov ecx,dword [edx]
	cmp ecx,0x3a
	jnb sti32u32lop2
	cmp ecx,0x30
	jb sti32u32lop2
	lea edx,[edx+4]
	lea eax,[eax*4+eax]
	lea eax,[eax*2+ecx-0x30]
	jmp sti32u32lop

	align 16
sti32u32lop2:
	test ecx,ecx
	jz sti32u32lop4
	xor eax,eax
	align 16
sti32u32lop4:
	ret

;Int64 MyString_StrToInt64UTF8(const UTF8Char *str1)
;-12 edi
;-8 esi
;-4 ebx
;0 retAddr
;4 str1
	align 16
MyString_StrToInt64UTF8:
_MyString_StrToInt64UTF8:
	mov dword [esp-4],ebx
	mov dword [esp-8],esi
	mov dword [esp-12],edi
	xor ebx,ebx
	xor eax,eax
	mov esi,dword [esp+4] ;str1
	mov ecx,10
	cmp byte [esi], 0x2d
	jnz sti64u8plop
	lea esi,[esi+1]
	align 16
sti64u8nlop:
	movzx edi,byte [esi]
	cmp edi,0x3a
	jnb sti64u8nlop2
	cmp edi,0x30
	jb sti64u8nlop2
	lea esi,[esi+1]
	lea edi,[edi-0x30]
	lea ebx,[ebx*4+ebx]
	mul ecx
	lea ebx,[ebx*2+edx]
	add eax,edi
	adc ebx,0
	jmp sti64u8nlop

	align 16
sti64u8nlop3:
	not eax
	not ebx
	inc eax
	adc ebx,0
	jmp sti64u8exit

	align 16
sti64u8nlop2:
	test edi,edi
	jz sti64u8nlop3
	xor eax,eax
	xor ebx,ebx
	jmp sti64u8exit

	align 16
sti64u8plop:
	movzx edi,byte [esi]
	cmp edi,0x3a
	jnb sti64u8plop2
	cmp edi,0x30
	jb sti64u8plop2
	lea esi,[esi+1]
	lea edi,[edi-0x30]
	lea ebx,[ebx*4+ebx]
	mul ecx
	lea ebx,[ebx*2+edx]
	add eax,edi
	adc ebx,0
	jmp sti64u8plop

	align 16
sti64u8plop2:
	test edi,edi
	jz sti64u8exit
	xor eax,eax
	xor ebx,ebx
	align 16
sti64u8exit:
	mov edx,ebx
	mov edi,dword [esp-12]
	mov esi,dword [esp-8]
	mov ebx,dword [esp-4]
	ret

;Int64 MyString_StrToInt64UTF16(const UTF16Char *str1)
;-12 edi
;-8 esi
;-4 ebx
;0 retAddr
;4 str1
	align 16
MyString_StrToInt64UTF16:
_MyString_StrToInt64UTF16:
	mov dword [esp-4],ebx
	mov dword [esp-8],esi
	mov dword [esp-12],edi
	xor ebx,ebx
	xor eax,eax
	mov esi,dword [esp+4] ;str1
	mov ecx,10
	cmp word [esi], 0x2d
	jnz sti64u16plop
	lea esi,[esi+2]
	align 16
sti64u16nlop:
	movzx edi,word [esi]
	cmp edi,0x3a
	jnb sti64u16nlop2
	cmp edi,0x30
	jb sti64u16nlop2
	lea esi,[esi+2]
	lea edi,[edi-0x30]
	lea ebx,[ebx*4+ebx]
	mul ecx
	lea ebx,[ebx*2+edx]
	add eax,edi
	adc ebx,0
	jmp sti64u16nlop

	align 16
sti64u16nlop3:
	not eax
	not ebx
	inc eax
	adc ebx,0
	jmp sti64u16exit

	align 16
sti64u16nlop2:
	test edi,edi
	jz sti64u16nlop3
	xor eax,eax
	xor ebx,ebx
	jmp sti64u16exit

	align 16
sti64u16plop:
	movzx edi,word [esi]
	cmp edi,0x3a
	jnb sti64u16plop2
	cmp edi,0x30
	jb sti64u16plop2
	lea esi,[esi+2]
	lea edi,[edi-0x30]
	lea ebx,[ebx*4+ebx]
	mul ecx
	lea ebx,[ebx*2+edx]
	add eax,edi
	adc ebx,0
	jmp sti64u16plop

	align 16
sti64u16plop2:
	test edi,edi
	jz sti64u16exit
	xor eax,eax
	xor ebx,ebx
	align 16
sti64u16exit:
	mov edx,ebx
	mov edi,dword [esp-12]
	mov esi,dword [esp-8]
	mov ebx,dword [esp-4]
	ret

;Int64 MyString_StrToInt64UTF32(const UTF32Char *str1)
;-12 edi
;-8 esi
;-4 ebx
;0 retAddr
;4 str1
	align 16
MyString_StrToInt64UTF32:
_MyString_StrToInt64UTF32:
	mov dword [esp-4],ebx
	mov dword [esp-8],esi
	mov dword [esp-12],edi
	xor ebx,ebx
	xor eax,eax
	mov esi,dword [esp+4] ;str1
	mov ecx,10
	cmp dword [esi], 0x2d
	jnz sti64u32plop
	lea esi,[esi+4]
	align 16
sti64u32nlop:
	mov edi,dword [esi]
	cmp edi,0x3a
	jnb sti64u32nlop2
	cmp edi,0x30
	jb sti64u32nlop2
	lea esi,[esi+4]
	lea edi,[edi-0x30]
	lea ebx,[ebx*4+ebx]
	mul ecx
	lea ebx,[ebx*2+edx]
	add eax,edi
	adc ebx,0
	jmp sti64u32nlop

	align 16
sti64u32nlop3:
	not eax
	not ebx
	inc eax
	adc ebx,0
	jmp sti64u32exit

	align 16
sti64u32nlop2:
	test edi,edi
	jz sti64u32nlop3
	xor eax,eax
	xor ebx,ebx
	jmp sti64u32exit

	align 16
sti64u32plop:
	mov edi,dword [esi]
	cmp edi,0x3a
	jnb sti64u32plop2
	cmp edi,0x30
	jb sti64u32plop2
	lea esi,[esi+4]
	lea edi,[edi-0x30]
	lea ebx,[ebx*4+ebx]
	mul ecx
	lea ebx,[ebx*2+edx]
	add eax,edi
	adc ebx,0
	jmp sti64u32plop

	align 16
sti64u32plop2:
	test edi,edi
	jz sti64u32exit
	xor eax,eax
	xor ebx,ebx
	align 16
sti64u32exit:
	mov edx,ebx
	mov edi,dword [esp-12]
	mov esi,dword [esp-8]
	mov ebx,dword [esp-4]
	ret

