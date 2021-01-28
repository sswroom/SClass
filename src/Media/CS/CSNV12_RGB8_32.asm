section .text

global _CSNV12_RGB8_do_nv12rgb8
global CSNV12_RGB8_do_nv12rgb8
global _CSNV12_RGB8_do_nv12rgb2
global CSNV12_RGB8_do_nv12rgb2

;void CSNV12_RGB8_do_nv12rgb8(UInt8 *yPtr, UInt8 *uvPtr, UInt8 *dest, OSInt width, OSInt height, OSInt dbpl, OSInt isFirst, OSInt isLast, UInt8 *csLineBuff, UInt8 *csLineBuff2, Int64 *yuv2rgb, UInt8 *rgbGammaCorr);
;-16 cofst
;-12 cWidth4
;-8 cSub
;-4 cSize
;0 edi
;4 esi
;8 ebx
;12 ebp
;16 retAddr
;20 yPtr
;24 uvPtr
;28 dest
;32 width
;36 height heightLeft
;40 dbpl
;44 isFirst
;48 isLast
;52 csLineBuff
;56 csLineBuff2
;60 yuv2rgb
;64 rgbGammaCorr

	align 16
_CSNV12_RGB8_do_nv12rgb8:
CSNV12_RGB8_do_nv12rgb8:
	push ebp
	push ebx
	push esi
	push edi
	
	mov eax,dword [esp+32] ;width
	lea edx,[eax * 8]
	mov dword [esp-4],edx ;OSInt cSize = width << 3;
	mov ecx,eax
	lea edx,[eax-4]
	shr ecx,2
	shr edx,1
	mov dword [esp-8],edx ;OSInt cSub = (width >> 1) - 2;
	mov dword [esp-12],ecx ;OSInt cWidth4 = width >> 2;
	mov dword [esp-16],0 ;Int32 cofst = 0;//this->cofst;

	mov ecx,dword [esp+36] ;height
	shr ecx,1
	mov ebx,dword [esp+48] ;isLast
	and ebx,1
	shl ebx,1
	sub ecx,ebx
	mov dword [esp+36],ecx ;heightLeft

	mov ecx,dword [esp+32] ;width
	shr ecx,2
	mov edi,dword [esp+52] ;csLineBuff
	mov ebx,dword [esp+60] ;yuv2rgb
	mov esi,dword [esp+20] ;yPtr

	align 16
n2r8lop2a:
	movzx eax,byte [esi]
	movq xmm0,[ebx+eax * 8]
	movzx eax,byte [esi+1]
	movq xmm1,[ebx+eax * 8]
	punpcklqdq xmm0,xmm1
	movdqa [edi],xmm0

	movzx eax,byte [esi+2]
	movq xmm0,[ebx+eax * 8]
	movzx eax,byte [esi+3]
	movq xmm1,[ebx+eax * 8]
	punpcklqdq xmm0,xmm1
	movdqa [edi+16],xmm0

	movzx eax,byte [esi+4]
	movq xmm0,[ebx+eax * 8]
	movzx eax,byte [esi+5]
	movq xmm1,[ebx+eax * 8]
	punpcklqdq xmm0,xmm1
	movdqa [edi+32],xmm0

	movzx eax,byte [esi+6]
	movq xmm0,[ebx+eax * 8]
	movzx eax,byte [esi+7]
	movq xmm1,[ebx+eax * 8]
	punpcklqdq xmm0,xmm1
	movdqa [edi+48],xmm0

	add esi,8
	add edi,64
	dec ecx
	jnz n2r8lop2a

	mov dword [esp+20],esi ;yPtr

	mov ebp,dword [esp-8] ;cSub
	mov edx,dword [esp-4] ;cSize
	mov esi,dword [esp+24] ;uvPtr
	mov edi,dword [esp+52] ;csLineBuff
	shr ebp,1 ;widthLeft

	movzx eax,byte [esi]
	movq xmm0,[ebx+eax*8 + 2048]
	movzx eax,byte [esi+1]
	movq xmm1,[ebx+eax*8 + 4096]
	paddsw xmm0,xmm1

	pxor xmm1,xmm1
	punpcklqdq xmm1,xmm0
	psraw xmm1,1
	por xmm0,xmm1

	movdqa xmm2,[edi]
	movdqa xmm3,[edi+edx]
	paddsw xmm2,xmm0
	psraw xmm0,1
	movdqa [edi],xmm2
	paddsw xmm3,xmm0
	movdqa [edi+edx],xmm3

	add edi,16
	add esi,2

	align 16
n2r8lop3a:
	movzx eax,byte [esi]
	movq xmm0,[ebx+eax*8 + 2048]
	movzx eax,byte [esi+1]
	movq xmm6,[ebx+eax*8 + 4096]
	movzx eax,byte [esi+2]
	movq xmm1,[ebx+eax*8 + 2048]
	movzx eax,byte [esi+3]
	movq xmm7,[ebx+eax*8 + 4096]
	paddsw xmm0,xmm6
	paddsw xmm1,xmm7

	pxor xmm4,xmm4
	pxor xmm5,xmm5
	punpcklqdq xmm4,xmm0
	punpcklqdq xmm5,xmm1
	psraw xmm4,1
	psraw xmm5,1
	por xmm0,xmm4
	por xmm1,xmm5
	paddsw xmm0,xmm5

	movdqa xmm2,[edi+16]
	movdqa xmm3,[edi+edx+16]
	paddsw xmm2,xmm1
	psraw xmm1,1
	movdqa [edi+16],xmm2
	paddsw xmm3,xmm1
	movdqa [edi+edx+16],xmm3

	movdqa xmm2,[edi]
	movdqa xmm3,[edi+edx]
	paddsw xmm2,xmm0
	psraw xmm0,1
	movdqa [edi],xmm2
	paddsw xmm3,xmm0
	movdqa [edi+edx],xmm3

	movdqa xmm2,[edi-16]
	movdqa xmm3,[edi+edx-16]
	paddsw xmm2,xmm4
	psraw xmm4,1
	movdqa [edi-16],xmm2
	paddsw xmm3,xmm4
	movdqa [edi+edx-16],xmm3

	add edi,32
	add esi,4
	dec ebp
	jnz n2r8lop3a

	pxor xmm4,xmm4
	movzx eax,byte [esi]
	movq xmm0,[ebx+eax*8 + 2048]
	movzx eax,byte [esi+1]
	movq xmm1,[ebx+eax*8 + 4096]
	paddsw xmm0,xmm1

	punpcklqdq xmm4,xmm0
	punpcklqdq xmm0,xmm0

	movdqa xmm2,[edi]
	movdqa xmm3,[edi+edx]
	paddsw xmm2,xmm0
	psraw xmm0,1
	movdqa [edi],xmm2
	paddsw xmm3,xmm0
	movdqa [edi+edx],xmm3

	psraw xmm4,1
	movdqa xmm2,[edi-16]
	paddsw xmm2,xmm4
	movdqa xmm3,[edi+edx-16]
	psraw xmm4,1
	paddsw xmm2,xmm4
	movdqa [edi-16],xmm2
	movdqa [edi+edx-16],xmm3

	add edi,16
	add esi,2
	mov dword [esp+24],esi ;uvPtr

	align 16
n2r8lop:
	mov ecx,dword [esp+32] ;width
	mov edi,dword [esp+56] ;csLineBuff2
	mov ebx,dword [esp+60] ;yuv2rgb
	mov esi,dword [esp+20] ;yPtr

	align 16
n2r8lop2:
	movzx eax,byte [esi]
	movq xmm0,[ebx+eax*8]
	movzx eax,byte [esi+1]
	movq xmm1,[ebx+eax*8]
	punpcklqdq xmm0,xmm1
	movdqa [edi],xmm0
	add esi,2
	add edi,16
	dec ecx
	jnz n2r8lop2
	mov dword [esp+20],esi ;yPtr

	mov ebp,dword [esp-8] ;cSub
	mov ecx,dword [esp-4] ;cSize
	mov edx,dword [esp+24] ;uvPtr
	mov esi,dword [esp+52] ;csLineBuff
	mov edi,dword [esp+56] ;csLineBuff2
	shr ebp,1 ;widthLeft

	pxor xmm4,xmm4
	movzx eax,byte [edx]
	movq xmm0,[ebx+eax*8 + 2048]
	movzx eax,byte [edx+1]
	movq xmm1,[ebx+eax*8 + 4096]
	paddsw xmm0,xmm1

	punpcklqdq xmm4,xmm0
	psraw xmm4,1
	por xmm0,xmm4

	movdqa xmm2,[edi]
	movdqa xmm3,[edi+ecx]
	paddsw xmm2,xmm0
	psraw xmm0,1
	movdqa [edi],xmm2
	paddsw xmm3,xmm0
	movdqa xmm2,[esi+ecx]
	paddsw xmm2,xmm0
	movdqa [edi+ecx],xmm3
	movdqa [esi+ecx],xmm2

	add esi,16
	add edi,16
	add edx,2

	ALIGN 16
n2r8lop3:
	pxor xmm4,xmm4
	pxor xmm5,xmm5
	movzx eax,byte [edx]
	movq xmm0,[ebx+eax*8 + 2048]
	movzx eax,byte [edx+1]
	movq xmm6,[ebx+eax*8 + 4096]
	movzx eax,byte [edx+2]
	movq xmm1,[ebx+eax*8 + 2048]
	movzx eax,byte [edx+3]
	movq xmm7,[ebx+eax*8 + 4096]
	paddsw xmm0,xmm6
	paddsw xmm1,xmm7

	punpcklqdq xmm4,xmm0
	punpcklqdq xmm5,xmm1
	psraw xmm4,1
	psraw xmm5,1
	por xmm0,xmm4
	por xmm1,xmm5
	paddsw xmm0,xmm5

	movdqa xmm2,[edi+16]
	movdqa xmm3,[edi+ecx+16]
	paddsw xmm2,xmm1
	psraw xmm1,1
	movdqa [edi+16],xmm2
	paddsw xmm3,xmm1
	movdqa xmm2,[esi+ecx+16]
	paddsw xmm2,xmm1
	movdqa [edi+ecx+16],xmm3
	movdqa [esi+ecx+16],xmm2

	movdqa xmm2,[edi]
	movdqa xmm3,[edi+ecx]
	paddsw xmm2,xmm0
	psraw xmm0,1
	movdqa [edi],xmm2
	paddsw xmm3,xmm0
	movdqa xmm2,[esi+ecx]
	paddsw xmm2,xmm0
	movdqa [edi+ecx],xmm3
	movdqa [esi+ecx],xmm2

	movdqa xmm2,[edi-16]
	movdqa xmm3,[edi+ecx-16]
	paddsw xmm2,xmm4
	psraw xmm4,1
	movdqa [edi-16],xmm2
	paddsw xmm3,xmm4
	movdqa xmm2,[esi+ecx-16]
	paddsw xmm2,xmm4
	movdqa [edi+ecx-16],xmm3
	movdqa [esi+ecx-16],xmm2

	add esi, 32

	add edi,32
	add edx,4
	dec ebp ;widthLeft
	jnz n2r8lop3

	movzx eax,byte [edx]
	movq xmm0,[ebx+eax*8 + 2048]
	movzx eax,byte [edx+1]
	movq xmm1,[ebx+eax*8 + 4096]
	paddsw xmm0,xmm1

	pxor xmm4,xmm4
	punpcklqdq xmm4,xmm0
	psraw xmm4,1
	por xmm0,xmm4

	movdqa xmm2,[edi]
	movdqa xmm3,[edi+ecx]
	paddsw xmm2,xmm0
	paddsw xmm3,xmm0
	movdqa [edi],xmm2
	movdqa [edi+ecx],xmm3
	movdqa xmm2,[edi-16]
	movdqa xmm3,[edi+ecx-16]
	paddsw xmm2,xmm4
	paddsw xmm3,xmm4
	movdqa xmm2,[edi-16]
	movdqa xmm3,[edi+ecx-16]
	psraw xmm4,1
	psraw xmm0,1
	movdqa xmm2,[esi+ecx]
	movdqa xmm3,[esi+ecx-16]
	paddsw xmm2,xmm0
	paddsw xmm3,xmm4
	movdqa [esi+ecx],xmm2
	movdqa [esi+ecx-16],xmm3

	add esi,16
	add edi,16
	add edx,2
	mov dword [esp+24],edx ;uvPtr

	mov ecx,dword [esp+32] ;width
	mov esi,dword [esp+52] ;csLineBuff
	mov edi,dword [esp+28] ;dest
	mov ebx,dword [esp+64] ;rgbGammaCorr
	
	align 16
n2r8lop5:
	movzx eax,word [esi+4]
	movzx edx,byte [ebx+eax]
	shl edx,16
	movzx eax,word [esi]
	mov dl,byte [ebx+eax+131072]
	movzx eax,word [esi+2]
	mov dh,byte [ebx+eax+65536]
	mov dword [edi],edx
	add esi,8
	add edi,4
	dec ecx
	jnz n2r8lop5

	mov eax,dword [esp+40] ;dbpl
	add dword [esp+28],eax ;dest

	mov ecx,dword [esp+32] ;width
	mov edi,dword [esp+28] ;dest
	align 16
n2r8lop6:
	movzx eax,word [esi+4]
	movzx edx,byte [ebx+eax]
	shl edx,16
	movzx eax,word [esi]
	mov dl,byte [ebx+eax+131072]
	movzx eax,word [esi+2]
	mov dh,byte [ebx+eax+65536]
	mov dword [edi],edx
	add esi,8
	add edi,4
	dec ecx
	jnz n2r8lop6

	mov eax,dword [esp+40] ;dbpl
	add dword [esp+28],eax ;dest

	mov esi,dword [esp+52] ;csLineBuff
	mov edi,dword [esp+56] ;csLineBuff2
	mov dword [esp+56],esi ;csLineBuff2
	mov dword [esp+52],edi ;csLineBuff

	dec dword [esp+36] ;heightLeft
	jnz n2r8lop

	test dword [esp+48],1 ;isLast
	jz n2r8lopexit

	mov esi,dword [esp+20] ;yPtr
	mov ecx,dword [esp+32] ;width
	mov edi,dword [esp+56] ;csLineBuff2
	mov ebx,dword [esp+60] ;yuv2rgb

	align 16
n2r8lop2b:
	movzx eax,byte [esi]
	movq xmm0,[ebx+eax*8]
	movzx eax,byte [esi+1]
	movq xmm1,[ebx+eax*8]
	punpcklqdq xmm0,xmm1
	movdqa [edi],xmm0
	add esi,2
	add edi,16
	dec ecx
	jnz n2r8lop2b
	mov dword [esp+20],esi ;yPtr

	mov ebp,dword [esp-8] ;cSub
	mov ecx,dword [esp-4] ;cSize
	mov edx,dword [esp+24] ;uvPtr
	mov esi,dword [esp+52] ;csLineBuff
	mov edi,dword [esp+56] ;csLineBuff2
	shr ebp,1 ;widthLeft

	pxor xmm1,xmm1
	movzx eax,byte [edx]
	movq xmm0,[ebx+eax*8 + 2048]
	movzx eax,byte [edx+1]
	movq xmm4,[ebx+eax*8 + 4096]
	paddsw xmm0,xmm4

	punpcklqdq xmm1,xmm0
	psraw xmm1,1
	por xmm0,xmm1

	movdqa xmm2,[edi]
	movdqa xmm3,[edi+ecx]
	paddsw xmm2,xmm0
	paddsw xmm3,xmm0
	movdqa [edi],xmm2
	movdqa [edi+ecx],xmm3
	psraw xmm0,1
	movdqa xmm2,[esi+ecx]
	paddsw xmm2,xmm0
	movdqa [esi+ecx],xmm2

	add esi,16
	add edi,16
	add edx,2

	align 16
n2r8lop3b:
	pxor xmm4,xmm4
	pxor xmm5,xmm5
	movzx eax,byte [edx]
	movq xmm0,[ebx+eax*8 + 2048]
	movzx eax,byte [edx+1]
	movq xmm6,[ebx+eax*8 + 4096]
	movzx eax,byte [edx+2]
	movq xmm1,[ebx+eax*8 + 2048]
	movzx eax,byte [edx+3]
	movq xmm7,[ebx+eax*8 + 4096]
	paddsw xmm0,xmm6
	paddsw xmm1,xmm7

	punpcklqdq xmm4,xmm0
	punpcklqdq xmm5,xmm1
	psraw xmm4,1
	psraw xmm5,1
	por xmm0,xmm4
	por xmm1,xmm5
	paddsw xmm0,xmm5

	movdqa xmm2,[edi]
	movdqa xmm3,[edi+16]
	movdqa xmm6,[edi+ecx]
	movdqa xmm7,[edi+ecx+16]
	paddsw xmm2,xmm0
	paddsw xmm3,xmm1
	paddsw xmm6,xmm0
	paddsw xmm7,xmm1
	movdqa [edi],xmm2
	movdqa [edi+16],xmm3
	movdqa [edi+ecx],xmm6
	movdqa [edi+ecx+16],xmm7
	psraw xmm0,1
	psraw xmm1,1
	movdqa xmm2,[esi+ecx]
	movdqa xmm3,[esi+ecx+16]
	paddsw xmm2,xmm0
	paddsw xmm3,xmm1
	movdqa [esi+ecx],xmm2
	movdqa [esi+ecx+16],xmm3
	movdqa xmm2,[edi-16]
	movdqa xmm3,[edi+ecx-16]
	paddsw xmm2,xmm4
	paddsw xmm3,xmm4
	movdqa [edi-16],xmm2
	movdqa [edi+ecx-16],xmm3
	psraw xmm4,1
	movdqa xmm2,[esi+ecx-16]
	paddsw xmm2,xmm4
	movdqa [esi+ecx-16],xmm2

	add esi,32
	add edi,32
	add edx,4
	dec ebp ;widthLeft//ecx
	jnz n2r8lop3b

	pxor xmm1,xmm1
	movzx eax,byte [edx]
	movq xmm0,[ebx+eax*8+2048]
	movzx eax,byte [edx+1]
	movq xmm4,[ebx+eax*8+4096]
	paddsw xmm0,xmm4

	punpcklqdq xmm1,xmm0
	punpcklqdq xmm0,xmm0
	psraw xmm1,1

	movdqa xmm2,[edi]
	movdqa xmm3,[edi+ecx]
	movdqa xmm4,[edi-16]
	movdqa xmm5,[edi+ecx-16]
	paddsw xmm2,xmm0
	paddsw xmm3,xmm0
	paddsw xmm4,xmm1
	paddsw xmm5,xmm1
	movdqa [edi],xmm2
	movdqa [edi+ecx],xmm3
	movdqa [edi-16],xmm4
	movdqa [edi+ecx-16],xmm5
	psraw xmm0,1
	psraw xmm1,1
	movdqa xmm2,[esi+ecx]
	movdqa xmm3,[esi+ecx-16]
	paddsw xmm2,xmm0
	paddsw xmm3,xmm1
	movdqa [esi+ecx],xmm2
	movdqa [esi+ecx-16],xmm3

	add esi,16
	add edi,16
	add edx,2
	mov dword [esp+24],edx ;uvPtr

	mov edi,dword [esp+28] ;dest
	mov ecx,dword [esp+32] ;width
	mov esi,dword [esp+52] ;csLineBuff
	mov ebx,dword [esp+64] ;rgbGammaCorr

	align 16
n2r8lop5b:
	movzx eax,word [esi+4]
	movzx edx,byte [ebx+eax]
	shl edx,16
	movzx eax,word [esi]
	mov dl,byte [ebx+eax+131072]
	movzx eax,word [esi+2]
	mov dh,byte [ebx+eax+65536]
	mov dword [edi],edx
	add esi,8
	add edi,4
	dec ecx
	jnz n2r8lop5b

	mov eax,dword [esp+40] ;dbpl
	add dword [esp+28],eax ;dest

	mov edi,dword [esp+28] ;dest
	mov ecx,dword [esp+32] ;width
	align 16
n2r8lop6b:
	movzx eax,word [esi+4]
	movzx edx,byte [ebx+eax]
	shl edx,16
	movzx eax,word [esi]
	mov dl,byte [ebx+eax+131072]
	movzx eax,word [esi+2]
	mov dh,byte [ebx+eax+65536]
	mov dword [edi],edx
	add esi,8
	add edi,4
	dec ecx
	jnz n2r8lop6b

	mov eax,dword [esp+40] ;dbpl
	add dword [esp+28],eax ;dest

	mov esi,dword [esp+52] ;csLineBuff
	mov edi,dword [esp+56] ;csLineBuff2
	mov dword [esp+56],esi ;csLineBuff2
	mov dword [esp+52],edi ;csLineBuff

	mov edi,dword [esp+28] ;dest
	mov ecx,dword [esp+32] ;width
	mov esi,dword [esp+52] ;csLineBuff
	mov ebx,dword [esp+64] ;rgbGammaCorr
	align 16
n2r8lop5c:
	movzx eax,word [esi+4]
	movzx edx,byte [ebx+eax]
	shl edx,16
	movzx eax,word [esi]
	mov dl,byte [ebx+eax+131072]
	movzx eax,word [esi+2]
	mov dh,byte [ebx+eax+65536]
	mov dword [edi],edx
	add esi,8
	add edi,4
	dec ecx
	jnz n2r8lop5c

	mov eax,dword [esp+40] ;dbpl
	add dword [esp+28],eax ;dest

	mov edi,dword [esp+28] ;dest
	mov ecx,dword [esp+32] ;width

	align 16
n2r8lop6c:
	movzx eax,word [esi+4]
	movzx edx,byte [ebx+eax]
	shl edx,16
	movzx eax,word [esi]
	mov dl,byte [ebx+eax+131072]
	movzx eax,word [esi+2]
	mov dh,byte [ebx+eax+65536]
	mov dword [edi],edx
	add esi,8
	add edi,4
	dec ecx
	jnz n2r8lop6c

	mov eax,dword [esp+40] ;dbpl
	add dword [esp+28],eax ;dest

	mov esi,dword [esp+52] ;csLineBuff
	mov edi,dword [esp+56] ;csLineBuff2
	mov dword [esp+56],esi ;csLineBuff2
	mov dword [esp+52],edi ;csLineBuff

	align 16
n2r8lopexit:
	pop edi
	pop esi
	pop ebx
	pop ebp
	ret

;void CSNV12_RGB8_do_nv12rgb2(UInt8 *yPtr, UInt8 *uvPtr, UInt8 *dest, OSInt width, OSInt height, OSInt dbpl, OSInt isFirst, OSInt isLast, UInt8 *csLineBuff, UInt8 *csLineBuff2, Int64 *yuv2rgb, UInt8 *rgbGammaCorr);
;0 edi
;4 esi
;8 ebx
;12 ebp
;16 retAddr
;20 yPtr
;24 uvPtr
;28 dest
;32 width
;36 height
;40 dbpl
;44 isFirst
;48 isLast
;48 csLineBuff
;52 csLineBuff2
;56 yuv2rgb
;60 rgbGammaCorr

	align 16
_CSNV12_RGB8_do_nv12rgb2:
CSNV12_RGB8_do_nv12rgb2:
	ret
