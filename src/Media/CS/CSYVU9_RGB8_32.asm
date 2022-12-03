section .text

global _CSYVU9_RGB8_do_yvu9rgb8
global CSYVU9_RGB8_do_yvu9rgb8

;void CSYVU9_RGB8_do_yvu9rgb8(UInt8 *yPtr, UInt8 *uPtr, UInt8 *vPtr, UInt8 *dest, OSInt width, OSInt height, OSInt dbpl, OSInt isFirst, OSInt isLast, UInt8 *csLineBuff, UInt8 *csLineBuff2, Int64 *yuv2rgb, UInt8 *rgbGammaCorr)
;-12 cSub
;-8 cSize
;-4 cWidth4
;0 edi
;4 esi
;8 ebx
;12 ebp
;16 retAddr
;20 yPtr
;24 uPtr
;28 vptr
;32 dest
;36 width
;40 height
;44 dbpl
;48 isFirst
;52 isLast
;56 csLineBuff
;60 csLineBuff2
;64 yuv2rgb
;68 rgbGammaCorr

	align 16
_CSYVU9_RGB8_do_yvu9rgb8:
CSYVU9_RGB8_do_yvu9rgb8:
	push ebp
	push ebx
	push esi
	push edi
	
	mov eax,dword [esp+36] ;width
	lea edx,[eax*8]
	mov ecx,eax
	mov ebx,eax
	shr ecx,1
	shr ebx,2
	sub ecx,2
	mov dword [esp-12],ecx ;cSub					Int32 cSub = (width >> 1) - 2;
	mov dword [esp-8],edx ;cSize					Int32 cSize = width << 3;
	mov dword [esp-4],ebx ;cWidth4					Int32 cWidth4 = width >> 2;

	mov ecx,dword [esp+40] ;height
	mov ebx,dword [esp+52] ;isLast
	shr ecx,1
	and ebx,1
	shl ebx,1
	sub ecx,ebx
	mov dword [esp+40],ecx ;heightLeft

	mov ecx,dword [esp+36] ;width
	shr ecx,2
	mov edi,dword [esp+56] ;csLineBuff
	mov ebx,dword [esp+64] ;yuv2rgb
	mov esi,dword [esp+20] ;yPtr

	align 16
y2rflop2a:
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

	lea esi,[esi+8]
	lea edi,[edi+64]
	dec ecx
	jnz y2rflop2a

	mov dword [esp+20],esi ;yPtr

	mov ebp,dword [esp-12] ;cSub
	mov edx,dword [esp-8] ;cSize
	mov esi,dword [esp+24] ;uPtr
	mov ecx,dword [esp+28] ;vPtr
	mov edi,dword [esp+56] ;csLineBuff
	shr ebp,1


	movzx eax,byte [esi]
	movq xmm0,[ebx+eax*8 + 2048]
	movzx eax,byte [ecx]
	movq xmm1,[ebx+eax*8 + 4096]
	paddsw xmm0,xmm1

	pxor xmm1,xmm1
	punpcklqdq xmm1,xmm0
	psraw xmm1,1
	por xmm0,xmm1

	movdqa xmm6,[edi]
	movdqa xmm7,[edi+edx]
	paddsw xmm6,xmm0
	psraw xmm0,1
	paddsw xmm7,xmm0

	lea edi,[edi+16]

	align 16
y2rflop3a:
	movzx eax,byte [esi]
	movq xmm0,[ebx+eax*8 + 2048]
	movzx eax,byte [esi+1]
	movq xmm1,[ebx+eax*8 + 2048]
	movzx eax,byte [ecx]
	movq xmm4,[ebx+eax*8 + 4096]
	movzx eax,byte [ecx+1]
	movq xmm5,[ebx+eax*8 + 4096]
	paddsw xmm0,xmm4
	paddsw xmm1,xmm5

	pxor xmm4,xmm4
	pxor xmm5,xmm5
	punpcklqdq xmm4,xmm0
	punpcklqdq xmm5,xmm1
	psraw xmm4,1
	psraw xmm5,1
	por xmm0,xmm4
	por xmm1,xmm5
	paddsw xmm0,xmm5

	paddsw xmm6,xmm4
	psraw xmm4,1
	movdqa [edi-16],xmm6
	paddsw xmm7,xmm4
	movdqa [edi+edx-16],xmm7

	movdqa xmm2,[edi]
	movdqa xmm3,[edi+edx]
	paddsw xmm2,xmm0
	psraw xmm0,1
	movdqa [edi],xmm2
	paddsw xmm3,xmm0
	movdqa [edi+edx],xmm3

	movdqa xmm6,[edi+16]
	movdqa xmm7,[edi+edx+16]
	paddsw xmm6,xmm1
	psraw xmm1,1
	paddsw xmm7,xmm1

	lea edi,[edi+32]
	inc esi
	inc ecx
	dec ebp
	jnz y2rflop3a

	pxor xmm4,xmm4
	movzx eax,byte [esi]
	movq xmm0,[ebx+eax*8 + 2048]
	movzx eax,byte [ecx]
	movq xmm1,[ebx+eax*8 + 4096]
	paddsw xmm0,xmm1

	punpcklqdq xmm4,xmm0
	punpcklqdq xmm0,xmm0

	psraw xmm4,1
	paddsw xmm6,xmm4
	psraw xmm4,1
	paddsw xmm7,xmm4
	movdqa [edi-16],xmm6
	movdqa [edi+edx-16],xmm7

	movdqa xmm2,[edi]
	movdqa xmm3,[edi+edx]
	paddsw xmm2,xmm0
	psraw xmm0,1
	movdqa [edi],xmm2
	paddsw xmm3,xmm0
	movdqa [edi+edx],xmm3

	lea edi,[edi+16]
	inc esi
	inc ecx

	align 16
y2rflop:
	mov esi,dword [esp+20] ;yPtr
	mov ecx,dword [esp+36] ;width
	mov edi,dword [esp+60] ;csLineBuff2
	mov ebx,dword [esp+64] ;yuv2rgb

	align 16
y2rflop2:
	mov dx,word [esi]
	movzx eax,dl
	movq xmm0,[ebx+eax*8]
	movzx eax,dh
	movq xmm1,[ebx+eax*8]
	punpcklqdq xmm0,xmm1
	movdqa [edi],xmm0
	lea esi,[esi+2]
	lea edi,[edi+16]
	dec ecx
	jnz y2rflop2
	mov dword [esp+20],esi ;yPtr

	mov ecx,dword [esp-12] ;cSub
	mov esi,dword [esp+56] ;csLineBuff
	mov edi,dword [esp+60] ;csLineBuff2
	mov dword [esp-20],esi ;buffTemp
	mov esi,dword [esp+24] ;uPtr
	mov edx,dword [esp+28] ;vPtr
	shr ecx,1
	mov dword [esp-16],ecx ;widthLeft
	mov ecx,dword [esp-8] ;cSize

	pxor xmm4,xmm4
	movzx eax,byte [esi]
	movq xmm0,[ebx+eax*8 + 2048]
	movzx eax,byte [edx]
	movq xmm1,[ebx+eax*8 + 4096]
	paddsw xmm0,xmm1

	mov ebp,dword [esp-20] ;buffTemp

	punpcklqdq xmm4,xmm0
	psraw xmm4,1
	por xmm0,xmm4

	movdqa xmm2,[edi]
	movdqa xmm3,[edi+ecx]
	paddsw xmm2,xmm0
	psraw xmm0,1
	paddsw xmm3,xmm0
	movdqa xmm6,[ebp+ecx]
	paddsw xmm6,xmm0


	add dword [esp-20],16 ;buffTemp

	add edi,16

	ALIGN 16
y2rflop3:
	mov ax,word [esi]
	movzx ebp,al
	movq xmm0,[ebx+ebp*8 + 2048]
	movzx ebp,ah
	movq xmm1,[ebx+ebp*8 + 2048]
	mov ax,word [edx]
	movzx ebp,al
	movq xmm4,[ebx+ebp*8 + 4096]
	movzx ebp,ah
	movq xmm5,[ebx+ebp*8 + 4096]
	paddsw xmm0,xmm4
	paddsw xmm1,xmm5

	mov ebp,dword [esp-20] ;buffTemp
	pxor xmm4,xmm4
	pxor xmm5,xmm5

	punpcklqdq xmm4,xmm0
	punpcklqdq xmm5,xmm1
	psraw xmm4,1
	psraw xmm5,1
	por xmm0,xmm4
	por xmm1,xmm5
	paddsw xmm0,xmm5

	paddsw xmm2,xmm4
	psraw xmm4,1
	movdqa [edi-16],xmm2
	paddsw xmm3,xmm4
	paddsw xmm6,xmm4
	movdqa [edi+ecx-16],xmm3
	movdqa [ebp+ecx-16],xmm6

	movdqa xmm2,[edi]
	movdqa xmm3,[edi+ecx]
	paddsw xmm2,xmm0
	psraw xmm0,1
	movdqa [edi],xmm2
	paddsw xmm3,xmm0
	movdqa xmm6,[ebp+ecx]
	paddsw xmm6,xmm0
	movdqa [edi+ecx],xmm3
	movdqa [ebp+ecx],xmm6

	movdqa xmm2,[edi+16]
	movdqa xmm3,[edi+ecx+16]
	paddsw xmm2,xmm1
	psraw xmm1,1
	paddsw xmm3,xmm1
	movdqa xmm6,[ebp+ecx+16]
	paddsw xmm6,xmm1

	add dword [esp-20], 32 ;buffTemp

	lea edi,[edi+32]
	inc esi
	inc edx
	dec dword [esp-16] ;widthLeft
	jnz y2rflop3

	movzx eax,byte [esi]
	movq xmm0,[ebx+eax*8 + 2048]
	movzx eax,byte [edx]
	movq xmm1,[ebx+eax*8 + 4096]
	paddsw xmm0,xmm1

	mov ebp,dword [esp-20] ;buffTemp

	pxor xmm4,xmm4
	punpcklqdq xmm4,xmm0
	por xmm0,xmm4
	psraw xmm4,1

	paddsw xmm2,xmm4
	movdqa [edi-16],xmm2
	psraw xmm4,1
	paddsw xmm3,xmm4
	paddsw xmm6,xmm4
	movdqa [edi+ecx-16],xmm3
	movdqa [ebp+ecx-16],xmm6

	movdqa xmm2,[edi]
	movdqa xmm3,[ebp+ecx]
	movdqa xmm6,[ebp+ecx]
	paddsw xmm2,xmm0
	psraw xmm0,1
	movdqa [edi],xmm2
	paddsw xmm3,xmm0
	paddsw xmm6,xmm0
	movdqa [edi+ecx],xmm3
	movdqa [ebp+ecx],xmm6

	add dword [esp-20], 16 ;buffTemp

	add edi,16
	inc esi
	inc edx
	test dword [esp+40], 1 ;heightLeft
	jnz y2rflop3c
	mov dword [esp+24],esi ;uPtr
	mov dword [esp+28],edx ;vPtr

	align 16
y2rflop3c:
	mov edi,dword [esp+32] ;dest
	mov ecx,dword [esp+36] ;width
	mov esi,dword [esp+56] ;csLineBuff
	mov ebx,dword [esp+68] ;rgbGammaCorr
	ALIGN 16
y2rflop5:
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
	jnz y2rflop5

	mov eax,dword [esp+44] ;dbpl
	add dword [esp+32],eax ;dest

	mov ecx,dword [esp+36] ;width
	mov edi,dword [esp+32] ;dest
	ALIGN 16
y2rflop6:
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
	jnz y2rflop6

	mov eax,dword [esp+44] ;dbpl
	add dword [esp+32],eax ;dest

	mov eax,dword [esp+56] ;csLineBuff
	xchg eax,dword [esp+60] ;csLineBuff2
	mov dword [esp+60],eax ;csLineBuff

	dec dword [esp+40] ;heightLeft
	jnz y2rflop

	test dword [esp+52],1 ;isLast
	jz yv2rflopexit

	mov esi,dword [esp+20] ;yPtr
	mov ecx,dword [esp+36] ;width
	mov edi,dword [esp+60] ;csLineBuff2
	mov ebx,dword [esp+64] ;yuv2rgb

	align 16
y2rflop2b:
	movzx eax,byte [esi]
	movq xmm0,[ebx+eax*8]
	movzx eax,byte [esi+1]
	movq xmm1,[ebx+eax*8]
	punpcklqdq xmm0,xmm1
	movdqa [edi],xmm0
	lea esi,[esi+2]
	lea edi,[edi+16]
	dec ecx
	jnz y2rflop2b
	mov dword [esp+20],esi ;yPtr

	mov ecx,dword [esp-12] ;cSub
	mov esi,dword [esp+56] ;csLineBuff
	mov edi,dword [esp+60] ;csLineBuff2
	mov dword [esp-20],esi ;buffTemp
	mov esi,dword [esp+24] ;uPtr
	mov edx,dword [esp+28] ;vPtr
	shr ecx,1
	mov dword [esp-16],ecx ;widthLeft
	mov ecx,dword [esp-8] ;cSize

	pxor xmm1,xmm1
	movzx eax,byte [esi]
	movq xmm0,[ebx+eax*8 + 2048]
	movzx eax,byte [edx]
	movq xmm4,[ebx+eax*8 + 4096]
	paddsw xmm0,xmm4

	mov ebp,dword [esp-20] ;buffTemp

	punpcklqdq xmm1,xmm0
	psraw xmm1,1
	por xmm0,xmm1

	movdqa xmm2,[edi]
	movdqa xmm3,[edi+ecx]
	movdqa xmm6,[ebp+ecx]
	paddsw xmm2,xmm0
	paddsw xmm3,xmm0
	psraw xmm0,1
	paddsw xmm6,xmm0

	add ebp,16 ;buffTemp
	add edi,16

	align 16
y2rflop3b:
	movzx eax,byte [esi]
	movq xmm0,[ebx+eax*8 + 2048]
	movzx eax,byte [esi+1]
	movq xmm1,[ebx+eax*8 + 2048]
	movzx eax,byte [edx]
	movq xmm4,[ebx+eax*8 + 4096]
	movzx eax,byte [edx+1]
	movq xmm5,[ebx+eax*8 + 4096]
	paddsw xmm0,xmm4
	paddsw xmm1,xmm5

	pxor xmm4,xmm4
	pxor xmm5,xmm5

	punpcklqdq xmm4,xmm0
	punpcklqdq xmm5,xmm1
	psraw xmm4,1
	psraw xmm5,1
	por xmm0,xmm4
	por xmm1,xmm5
	paddsw xmm0,xmm5

	paddsw xmm2,xmm4
	paddsw xmm3,xmm4
	movdqa [edi-16],xmm2
	psraw xmm4,1
	movdqa [edi+ecx-16],xmm3
	paddsw xmm6,xmm4
	movdqa [ebp+ecx-16],xmm6

	movdqa xmm2,[edi]
	movdqa xmm3,[edi+ecx]
	movdqa xmm6,[ebp+ecx]
	paddsw xmm2,xmm0
	paddsw xmm3,xmm0
	movdqa [edi],xmm2
	psraw xmm0,1
	movdqa [edi+ecx],xmm3
	paddsw xmm6,xmm0
	movdqa [ebp+ecx],xmm6
	movdqa xmm2,[edi+16]
	movdqa xmm3,[edi+ecx+16]
	movdqa xmm6,[ebp+ecx+16]
	paddsw xmm2,xmm1
	paddsw xmm3,xmm1
	psraw xmm1,1
	paddsw xmm6,xmm1

	add ebp, 32
	add edi,32
	inc esi
	inc edx
	dec dword [esp-16] ;widthLeft
	jnz y2rflop3b

	pxor xmm1,xmm1
	movzx eax,byte [esi]
	movq xmm0,[ebx+eax*8+2048]
	movzx eax,byte [edx]
	movq xmm4,[ebx+eax*8+4096]
	paddsw xmm0,xmm4


	punpcklqdq xmm1,xmm0
	punpcklqdq xmm0,xmm0
	psraw xmm1,1

	paddsw xmm2,xmm1
	paddsw xmm3,xmm1
	movdqa [edi-16],xmm2
	psraw xmm1,1
	movdqa [edi+ecx-16],xmm3
	paddsw xmm6,xmm1
	movdqa [ebp+ecx-16],xmm6
	movdqa xmm2,[edi]
	movdqa xmm3,[edi+ecx]
	movdqa xmm6,[ebp+ecx]
	paddsw xmm2,xmm0
	paddsw xmm3,xmm0
	movdqa [edi],xmm2
	psraw xmm0,1
	movdqa [edi+ecx],xmm3
	paddsw xmm6,xmm0
	movdqa [ebp+ecx],xmm6

	add ebp,16
	add edi,16
	inc esi
	inc edx
	mov dword [esp+24],esi ;uPtr
	mov dword [esp+28],edx ;vPtr

	mov edi,dword [esp+32] ;dest
	mov ecx,dword [esp+36] ;width
	mov esi,dword [esp+56] ;csLineBuff
	mov ebx,dword [esp+68] ;rgbGammaCorr
	align 16
y2rflop5b:
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
	jnz y2rflop5b

	mov eax,dword [esp+44] ;dbpl
	add dword [esp+32],eax ;dest

	mov edi,dword [esp+32] ;dest
	mov ecx,dword [esp+36] ;width
	align 16
y2rflop6b:
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
	jnz y2rflop6b

	mov eax,dword [esp+44] ;dbpl
	add dword [esp+32],eax ;dest

	mov esi,dword [esp+56] ;csLineBuff
	mov edi,dword [esp+60] ;csLineBuff2
	mov dword [esp+60],esi ;csLineBuff2
	mov dword [esp+56],edi ;csLineBuff

	mov edi,dword [esp+32] ;dest
	mov ecx,dword [esp+36] ;width
	mov esi,dword [esp+56] ;csLineBuff
	mov ebx,dword [esp+68] ;rgbGammaCorr
	align 16
y2rflop5c:
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
	jnz y2rflop5c

	mov eax,dword [esp+44] ;dbpl
	add dword [esp+32],eax ;dest

	mov edi,dword [esp+32] ;dest
	mov ecx,dword [esp+36] ;width
	align 16
y2rflop6c:
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
	jnz y2rflop6c

	mov eax,dword [esp+44] ;dbpl
	add dword [esp+32],eax ;dest

	mov esi,dword [esp+56] ;csLineBuff
	mov edi,dword [esp+60] ;csLineBuff2
	mov dword [esp+60],esi ;csLineBuff2
	mov dword [esp+56],edi ;csLineBuff

	align 16
yv2rflopexit:
	pop edi
	pop esi
	pop ebx
	pop ebp
	ret
