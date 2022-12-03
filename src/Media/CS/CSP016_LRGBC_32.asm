section .text

global _CSP016_LRGBC_VerticalFilterLRGB
global CSP016_LRGBC_VerticalFilterLRGB
global _CSP016_LRGBC_do_yv12rgb8
global CSP016_LRGBC_do_yv12rgb8
global _CSP016_LRGBC_do_yv12rgb2
global CSP016_LRGBC_do_yv12rgb2

;void CSP016_LRGBC_VerticalFilterLRGB(UInt8 *inYPt, UInt8 *inUVPt, UInt8 *outPt, OSInt width, OSInt height, OSInt tap, OSInt *index, Int64 *weight, Int32 isFirst, Int32 isLast, UInt8 *csLineBuff, UInt8 *csLineBuff2, OSInt ystep, OSInt dstep, Int64 *yuv2rgb, Int64 *rgbGammaCorr)
; 0 tmpV
; 16 cSub
; 20 sWidth
; 24 edi
; 28 esi
; 32 ebx
; 36 ebp
; 40 retAddr
; 44 inYPt
; 48 inUVPt
; 52 outPt
; 56 width
; 60 height / currHeight
; 64 tap
; 68 index
; 72 weight
; 76 isFirst
; 80 isLast
; 84 csLineBuff
; 88 csLineBuff2
; 92 ystep / yAdd
; 96 dstep
; 100 yuv2rgb
; 104 rgbGammaCorr

	align 16
_CSP016_LRGBC_VerticalFilterLRGB:
CSP016_LRGBC_VerticalFilterLRGB:
	mov eax,32768
	push ebp
	push ebx
	push esi
	push edi
	push eax
	push eax
	push eax						;Int32 tmpV[4] = {32768, 32768, 32768, 32768};
	push eax
	push eax
	push eax
	mov edx,dword [esp+56] ;width
	mov eax,edx
	mov ecx,edx
	sub dword [esp+92],edx			;OSInt yAdd = ystep - width;
	shr eax,1
	shr ecx,3
	sub eax,2
	mov dword [esp+20],ecx ;sWidth	OSInt sWidth = width >> 3;
	mov dword [esp+16],eax ;cSub	OSInt cSub = (width >> 1) - 2;
	mov ecx,dword [esp+64] ;tap
	cmp ecx,4
	jnz vflexit						; if (tap == 4)
	and edx,7
	jz vflstart
									; if (width & 7)
	shr edx,2
	push edx						; OSInt widthLeft = (width & 7) >> 2;
	movdqu xmm7,[esp+4] ;tmpV

	align 16
vf7lop:
	mov ebx,dword [esp+76] ;weight
	mov esi,dword [esp+91] ;csLineBuff2
	mov ecx,dword [esp+52] ;inUVPt

	movdqa xmm5,[ebx+16]
	movdqa xmm6,[ebx+32]

	mov ebp,dword [esp+24] ;sWidth
	ALIGN 16
vf7lop2:
	mov eax,dword [ebx]
	movdqa xmm1,xmm7
	movdqu xmm0,[ecx+eax]
	mov eax,dword [ebx+4]
	movdqa xmm3,xmm7
	movdqu xmm2,[ecx+eax]
	movdqa xmm4,xmm0
	punpcklwd xmm0,xmm2
	punpckhwd xmm4,xmm2
	psrlw xmm0,1
	psrlw xmm4,1
	pmaddwd xmm0, xmm5
	pmaddwd xmm4, xmm5
	paddd xmm1,xmm0
	paddd xmm3,xmm4

	mov eax,dword [ebx+8]
	movdqu xmm0,[ecx+eax]
	mov eax,dword [ebx+12]
	movdqa xmm4,xmm0
	movdqu xmm2,[ecx+eax]
	punpcklwd xmm0,xmm2
	punpckhwd xmm4,xmm2
	psrlw xmm0,1
	psrlw xmm4,1
	pmaddwd xmm0, xmm6
	pmaddwd xmm4, xmm6
	paddd xmm1,xmm0
	paddd xmm3,xmm4

	psrad xmm1,16
	psrad xmm3,16
	packssdw xmm1,xmm3

	movdqu [esi],xmm1
	add esi,16
	add ecx,8
	dec ebp
	jnz vf7lop2

	mov ebp,dword [esp+0] ;widthLeft
	test ebp,ebp
	jz vf7lop3b
	ALIGN 16
vf7lop3:
	mov eax,dword [ebx]
	movdqa xmm1,xmm7
	movdqa xmm3,xmm7
	movdqu xmm0,[ecx+eax]
	mov eax,dword [ebx+4]
	movdqa xmm4,xmm0
	movdqu xmm2,[ecx+eax]
	punpcklwd xmm0,xmm2
	punpckhwd xmm4,xmm2
	psrlw xmm0,1
	psrlw xmm4,1
	pmaddwd xmm0, xmm5
	pmaddwd xmm4, xmm5
	paddd xmm1,xmm0
	paddd xmm3,xmm4

	mov eax,dword [ebx+8]
	movdqu xmm0,[ecx+eax]
	mov eax,dword [ebx+12]
	movdqa xmm4,xmm0
	movdqu xmm2,[ecx+eax]
	punpcklwd xmm0,xmm2
	punpckhwd xmm4,xmm2
	psrlw xmm0,1
	psrlw xmm4,1
	pmaddwd xmm0, xmm6
	pmaddwd xmm4, xmm6
	paddd xmm1,xmm0
	paddd xmm3,xmm4

	psrad xmm1,16
	psrad xmm3,16
	packssdw xmm1,xmm3
	movdqu [esi],xmm1
	lea esi,[esi+16]
	lea ecx,[ecx+8]
;	dec ebp
;	jnz vf7lop3

	align 16
vf7lop3b:
	mov ebp,dword [esp+60] ;width
	and ebp,3
	jz vf7lop3c
	mov eax,dword [ebx]
	movdqa xmm1,xmm7
	movdqu xmm0,[ecx+eax]
	movdqa xmm3,xmm7
	mov eax,dword [ebx+4]
	movdqa xmm4,xmm0
	movdqu xmm2,[ecx+eax]
	punpcklwd xmm0,xmm2
	punpckhwd xmm4,xmm2
	psrlw xmm0,1
	psrlw xmm4,1
	pmaddwd xmm0, xmm5
	pmaddwd xmm4, xmm5
	paddd xmm1,xmm0
	paddd xmm3,xmm4

	mov eax,dword [ebx+8]
	movdqu xmm0,[ecx+eax]
	mov eax,dword [ebx+12]
	movdqa xmm4,xmm0
	movdqu xmm2,[ecx+eax]
	punpcklwd xmm0,xmm2
	punpckhwd xmm4,xmm2
	psrlw xmm0,1
	psrlw xmm4,1
	pmaddwd xmm0, xmm6
	pmaddwd xmm4, xmm6
	paddd xmm1,xmm0
	paddd xmm3,xmm4

	psrad xmm1,16
	psrad xmm3,16
	packssdw xmm1,xmm3
	movd [esi],xmm1
	lea esi,[esi+4]

	align 16
vf7lop3c:

	mov ebx,dword [esp+104] ;yuv2rgb
	mov ebp,dword [esp+20] ;cSub
	mov edi,dword [esp+88] ;csLineBuff
	mov esi,dword [esp+48] ;inYPt
	mov ecx,dword [esp+92] ;csLineBuff2

	movzx eax,word [esi]
	movq xmm2,[ebx+eax * 8]
	movzx eax,word [esi+2]
	movq xmm4,[ebx+eax * 8]
	punpcklqdq xmm2,xmm4

	movzx eax,word [ecx]
	movq xmm0,[ebx+eax*8 + 524288]
	movzx eax,word [ecx+2]
	movq xmm1,[ebx+eax*8 + 1048576]
	paddsw xmm0,xmm1

	pxor xmm3,xmm3
	punpcklqdq xmm3,xmm0
	psraw xmm3,1
	paddsw xmm2,xmm0
	paddsw xmm2,xmm3

	lea esi,[esi+4]
	lea ecx,[ecx+4]
	ALIGN 16
vf7lop2b:
	movzx eax,word [ecx]
	movq xmm0,[ebx+eax*8 + 524288]
	movzx eax,word [ecx+2]
	movq xmm1,[ebx+eax*8 + 1048576]
	paddsw xmm0,xmm1

	pxor xmm3,xmm3
	punpcklqdq xmm3,xmm0
	psraw xmm3,1
	paddsw xmm2,xmm3
	movdqa [edi],xmm2

	movzx eax,word [esi]
	movq xmm2,[ebx+eax * 8]
	movzx eax,word [esi+2]
	movq xmm4,[ebx+eax * 8]
	punpcklqdq xmm2,xmm4

	paddsw xmm2,xmm0
	paddsw xmm2,xmm3
	lea edi,[edi+16]
	lea esi,[esi+4]
	lea ecx,[ecx+4]
	dec ebp
	jnz vf7lop2b

	movzx eax,word [ecx]
	movq xmm0,[ebx+eax*8 + 524288]
	movzx eax,word [ecx+2]
	movq xmm1,[ebx+eax*8 + 1048576]
	paddsw xmm0,xmm1

	pxor xmm3,xmm3
	punpcklqdq xmm3,xmm0
	por xmm0,xmm3
	psraw xmm3,1
	paddsw xmm2,xmm3
	movdqa [edi],xmm2

	movzx eax,word [esi]
	movq xmm2,[ebx+eax * 8]
	movzx eax,word [esi+2]
	movq xmm4,[ebx+eax * 8]
	punpcklqdq xmm2,xmm4
	paddsw xmm2,xmm0
	movdqa [edi+16],xmm2
	lea esi,[esi+4]
	lea ecx,[ecx+4]
	add esi,dword [esp+96] ;yAdd
	mov dword [esp+48],esi ;inYPt

	mov ecx,dword [esp+60] ;width
	mov esi,dword [esp+88] ;csLineBuff
	mov edi,dword [esp+56] ;outPt
	mov ebx,dword [esp+108] ;rgbGammaCorr
	ALIGN 16
vf7lop5:
	movzx eax,word [esi+4]
	movq xmm5,[ebx+eax*8]
	movzx eax,word [esi+2]
	movq xmm6,[ebx+eax*8+524288]
	paddsw xmm5,xmm6
	movzx eax,word [esi]
	movq xmm6,[ebx+eax*8+1048576]
	paddsw xmm5,xmm6
	movq [edi],xmm5
	lea esi,[esi+8]
	lea edi,[edi+8]
	dec ecx
	jnz vf7lop5

	add dword [esp+76],48 ;weight
	mov eax,dword [esp+100] ;dstep
	add dword [esp+56],eax ;outPt

	dec dword [esp+64] ;currHeight
	jnz vf7lop
	pop eax
	jmp vflexit

	align 16
vflstart:
	movdqu xmm7,[esp+0] ;tmpV
	ALIGN 16
vflop:
	mov ebx,dword [esp+72] ;weight
	mov esi,dword [esp+88] ;csLineBuff2
	mov ecx,dword [esp+48] ;inUVPt

	movdqa xmm5,[ebx+16]
	movdqa xmm6,[ebx+32]

	mov ebp,dword [esp+20] ;sWidth
	ALIGN 16
vflop2:
	mov eax,dword [ebx]
	movdqu xmm1,[ecx+eax]
	mov eax,dword [ebx+4]
	movdqa xmm3,xmm1
	movdqu xmm4,[ecx+eax]
	punpcklwd xmm1,xmm4
	punpckhwd xmm3,xmm4
	psrlw xmm1,1
	psrlw xmm3,1

	mov eax,dword [ebx+8]
	pmaddwd xmm1, xmm5
	movdqu xmm0,[ecx+eax]
	pmaddwd xmm3,xmm5
	mov eax,dword [ebx+12]
	movdqa xmm4,xmm0
	movq xmm2,[ecx+eax]
	punpcklwd xmm0,xmm2
	punpckhwd xmm4,xmm2
	psrlw xmm0,1
	psrlw xmm4,1
	pmaddwd xmm0,xmm6
	pmaddwd xmm4,xmm6
	paddd xmm1,xmm0
	paddd xmm3,xmm4
	paddd xmm1,xmm7
	paddd xmm3,xmm7

	psrad xmm1,16
	psrad xmm3,16
	packssdw xmm1,xmm3
	movdqa [esi],xmm1
	lea ecx,[ecx+8]
	lea esi,[esi+16]
	dec ebp
	jnz vflop2

	mov ebx,dword [esp+100] ;yuv2rgb
	mov ebp,dword [esp+16] ;cSub
	mov edi,dword [esp+84] ;csLineBuff
	mov esi,dword [esp+44] ;inYPt
	mov ecx,dword [esp+88] ;csLineBuff2

	mov edx,dword [esi]
	movzx eax,dx
	movq xmm2,[ebx+eax * 8]
	shr edx,16
	movq xmm4,[ebx+edx * 8]
	punpcklqdq xmm2,xmm4

	mov edx,dword [ecx]
	movzx eax,dx
	movq xmm0,[ebx+eax*8 + 524288]
	shr edx,16
	movq xmm1,[ebx+edx*8 + 1048576]
	paddsw xmm0,xmm1

	pxor xmm3,xmm3
	punpcklqdq xmm3,xmm0
	psraw xmm3,1
	paddsw xmm2,xmm0
	paddsw xmm2,xmm3

	lea esi,[esi+2]
	lea ecx,[ecx+4]
	ALIGN 16
y2rllop2b:
	mov edx,dword [ecx]
	movzx eax,dx
	shr edx,16
	movq xmm0,[ebx+eax*8 + 524288]
	pxor xmm3,xmm3
	movq xmm1,[ebx+edx*8 + 1048576]
	paddsw xmm0,xmm1

	punpcklqdq xmm3,xmm0
	psraw xmm3,1
	paddsw xmm2,xmm3
	movdqa [edi],xmm2

	mov edx,dword [esi]
	movzx eax,dx
	movq xmm2,[ebx+eax * 8]
	shr edx,16
	movq xmm4,[ebx+edx * 8]
	punpcklqdq xmm2,xmm4

	paddsw xmm2,xmm0
	paddsw xmm2,xmm3
	lea edi,[edi+16]
	lea esi,[esi+4]
	lea ecx,[ecx+4]
	dec ebp
	jnz y2rllop2b

	mov edx,dword [ecx]
	movzx eax,dx
	movq xmm0,[ebx+eax*8 + 524288]
	shr edx,16
	movq xmm1,[ebx+edx*8 + 1048576]
	paddsw xmm0,xmm1

	pxor xmm3,xmm3
	punpcklqdq xmm3,xmm0
	por xmm0,xmm3
	psraw xmm3,1
	paddsw xmm2,xmm3
	movdqa [edi],xmm2

	mov edx,dword [esi]
	movzx eax,dx
	movq xmm2,[ebx+eax * 8]
	shr edx,16
	movq xmm4,[ebx+edx * 8]
	punpcklqdq xmm2,xmm4
	paddsw xmm2,xmm0
	movdqa [edi+16],xmm2

	add esi,dword [esp+92] ;yAdd
	lea esi,[esi+4]
	mov dword [esp+44],esi ;inYPt

	mov ebp,dword [esp+56] ;width
	mov esi,dword [esp+84] ;csLineBuff
	mov edi,dword [esp+52] ;outPt
	mov ebx,dword [esp+104] ;rgbGammaCorr
	shr ebp,1
	ALIGN 16
y2rllop5:
	movdqa xmm1,[esi]
	pextrw eax,xmm1,0
	movq xmm4,[ebx+eax*8+1048576]
	pextrw eax,xmm1,4
	movq xmm0,[ebx+eax*8+1048576]

	pextrw eax,xmm1,1
	movq xmm6,[ebx+eax*8+524288]
	pextrw eax,xmm1,5
	movq xmm5,[ebx+eax*8+524288]
	paddsw xmm4,xmm6
	paddsw xmm0,xmm5

	pextrw eax,xmm1,2
	movq xmm6,[ebx+eax*8]
	pextrw eax,xmm1,6
	movq xmm5,[ebx+eax*8]
	paddsw xmm4,xmm6
	paddsw xmm0,xmm5

	punpcklqdq xmm4,xmm0
	movntdq [edi],xmm4
	lea esi,[esi+16]
	lea edi,[edi+16]
	dec ebp
	jnz y2rllop5

	mov eax,dword [esp+96] ;dstep
	add dword [esp+72],48 ;weight
	add dword [esp+52],eax ;outPt

	dec dword [esp+60] ;currHeight
	jnz vflop

	align 16
vflexit:
	add esp,24
	pop edi
	pop esi
	pop ebx
	pop ebp
	ret

;void CSP016_LRGBC_do_yv12rgb8(UInt8 *yPtr, UInt8 *uvPtr, UInt8 *dest, OSInt width, OSInt height, OSInt dbpl, OSInt isFirst, OSInt isLast, UInt8 *csLineBuff, UInt8 *csLineBuff2, OSInt yBpl, OSInt uvBpl, Int64 *yuv2rgb, Int64 *rgbGammaCorr)
;0 widthLeft
;4 cSub
;8 cSize
;12 edi
;16 esi
;20 ebx
;24 ebp
;28 retAddr
;32 yPtr
;36 uvPtr
;40 dest
;44 width
;48 height
;52 dbpl
;56 isFirst
;60 isLast
;64 csLineBuff
;68 csLineBuff2
;72 yBpl / yAdd
;76 uvBpl / uvAdd
;80 yuv2rgb
;84 rgbGammaCorr

	align 16
_CSP016_LRGBC_do_yv12rgb8:
CSP016_LRGBC_do_yv12rgb8:
	push ebp
	push ebx
	push esi
	push edi
	push eax
	push eax
	push eax
	mov edx,dword [esp+44] ;width
	lea eax,[edx*8]
	sub dword [esp+72],edx
	sub dword [esp+72],edx ;yAdd					Int32 yAdd = yBpl - width * 2;
	mov dword [esp+8],eax ;cSize					Int32 cSize = width << 3;
	sub dword [esp+76],edx ;uvAdd					Int32 uvAdd = uvBpl - width;
	shr edx,1
	sub edx,2
	mov dword [esp+4],edx ;cSub						Int32 cSub = (width >> 1) - 2;

	mov ecx,dword [esp+48] ;height
	shr ecx,1
	mov ebx,dword [esp+60] ;isLast
	and ebx,1
	shl ebx,1
	sub ecx,ebx
	mov dword [esp+48],ecx ;heightLeft

	mov esi,dword [esp+32] ;yPtr
	mov ecx,dword [esp+44] ;width
	mov edi,dword [esp+64] ;csLineBuff
	mov ebx,dword [esp+80] ;yuv2rgb

	shr ecx,3
	align 16
y2r8flop2a:
	movzx eax,word [esi]
	movq xmm0,[ebx+eax * 8]
	movzx eax,word [esi+2]
	movq xmm1,[ebx+eax * 8]
	punpcklqdq xmm0,xmm1
	movdqa [edi],xmm0

	movzx eax,word [esi+4]
	movq xmm0,[ebx+eax * 8]
	movzx eax,word [esi+6]
	movq xmm1,[ebx+eax * 8]
	punpcklqdq xmm0,xmm1
	movdqa [edi+16],xmm0

	movzx eax,word [esi+8]
	movq xmm0,[ebx+eax * 8]
	movzx eax,word [esi+10]
	movq xmm1,[ebx+eax * 8]
	punpcklqdq xmm0,xmm1
	movdqa [edi+32],xmm0

	movzx eax,word [esi+12]
	movq xmm0,[ebx+eax * 8]
	movzx eax,word [esi+14]
	movq xmm1,[ebx+eax * 8]
	punpcklqdq xmm0,xmm1
	movdqa [edi+48],xmm0

	add esi,16
	add edi,64
	dec ecx
	jnz y2r8flop2a

	mov ecx,dword [esp+44] ;width
	add esi,dword [esp+72] ;yAdd
	shr ecx,3
	ALIGN 16
y2r8flop2a2:
	movzx eax,word [esi]
	movq xmm0,[ebx+eax * 8]
	movzx eax,word [esi+2]
	movq xmm1,[ebx+eax * 8]
	punpcklqdq xmm0,xmm1
	movdqa [edi],xmm0

	movzx eax,word [esi+4]
	movq xmm0,[ebx+eax * 8]
	movzx eax,word [esi+6]
	movq xmm1,[ebx+eax * 8]
	punpcklqdq xmm0,xmm1
	movdqa [edi+16],xmm0

	movzx eax,word [esi+8]
	movq xmm0,[ebx+eax * 8]
	movzx eax,word [esi+10]
	movq xmm1,[ebx+eax * 8]
	punpcklqdq xmm0,xmm1
	movdqa [edi+32],xmm0

	movzx eax,word [esi+12]
	movq xmm0,[ebx+eax * 8]
	movzx eax,word [esi+14]
	movq xmm1,[ebx+eax * 8]
	punpcklqdq xmm0,xmm1
	movdqa [edi+48],xmm0

	lea esi,[esi+16]
	lea edi,[edi+64]
	dec ecx
	jnz y2r8flop2a2
	add esi,dword [esp+72] ;yAdd
	mov dword [esp+32],esi ;yPtr

	mov ecx,dword [esp+4] ;cSub
	mov edx,dword [esp+8] ;cSize
	mov esi,dword [esp+36] ;uvPtr
	mov edi,dword [esp+64] ;csLineBuff
	shr ecx,1
	mov dword [esp+0],ecx ;widthLeft

	movzx eax,word [esi]
	movq xmm0,[ebx+eax*8 + 524288]
	movzx eax,word [esi+2]
	movq xmm1,[ebx+eax*8 + 1048576]
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
	add esi,4

	align 16
y2r8flop3a:
	movzx eax,word [esi]
	movq xmm0,[ebx+eax*8 + 524288]
	movzx eax,word [esi+4]
	movq xmm1,[ebx+eax*8 + 524288]
	movzx eax,word [esi+2]
	movq xmm4,[ebx+eax*8 + 1048576]
	movzx eax,word [esi+6]
	movq xmm5,[ebx+eax*8 + 1048576]
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
	lea esi,[esi+8]
	dec dword [esp+0] ;widthLeft
	jnz y2r8flop3a

	pxor xmm4,xmm4
	movzx eax,word [esi]
	movq xmm0,[ebx+eax*8 + 524288]
	movzx eax,word [esi+2]
	movq xmm1,[ebx+eax*8 + 1048576]
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
	add esi,4
	mov ebp,dword [esp+80] ;uvAdd
	add esi,ebp
	add ecx,ebp
	mov dword [esp+36],esi ;uvPtr

	align 16
y2r8flop:
	mov esi,dword [esp+32] ;yPtr
	mov ecx,dword [esp+44] ;width
	mov edi,dword [esp+68] ;csLineBuff2
	mov ebx,dword [esp+80] ;yuv2rgb
	
	shr ecx,1
	align 16
y2r8flop2:
	mov edx,dword [esi]
	movzx eax,dx
	movq xmm0,[ebx+eax*8]
	shr edx,16
	movq xmm1,[ebx+edx*8]
	punpcklqdq xmm0,xmm1
	movdqa [edi],xmm0
	lea esi,[esi+4]
	lea edi,[edi+16]
	dec ecx
	jnz y2r8flop2

	mov ecx,dword [esp+44] ;width
	add esi,dword [esp+72] ;yAdd
	shr ecx,1
	align 16
y2r8flop2_2:
	mov edx,dword [esi]
	movzx eax,dx
	movq xmm0,[ebx+eax*8]
	shr edx,16
	movq xmm1,[ebx+edx*8]
	punpcklqdq xmm0,xmm1
	movdqa [edi],xmm0
	lea esi,[esi+4]
	lea edi,[edi+16]
	dec ecx
	jnz y2r8flop2_2

	add esi,dword [esp+72] ;yAdd
	mov dword [esp+32],esi ;yPtr

	mov ecx,dword [esp+4] ;cSub
	mov esi,dword [esp+36] ;uvPtr
	mov ebp,dword [esp+64] ;csLineBuff
	mov edi,dword [esp+68] ;csLineBuff2
	shr ecx,1
	mov dword [esp+0],ecx ;widthLeft
	mov ecx,dword [esp+8] ;cSize

	pxor xmm4,xmm4
	movzx eax,word [esi]
	movq xmm0,[ebx+eax*8 + 524288]
	movzx eax,word [esi+2]
	movq xmm1,[ebx+eax*8 + 1048576]
	paddsw xmm0,xmm1

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


	add ebp,16
	add edi,16
	add esi,4

	ALIGN 16
y2r8flop3:
	movzx eax,word [esi]
	movq xmm0,[ebx+eax*8 + 524288]
	movzx eax,word [esi+4]
	movq xmm1,[ebx+eax*8 + 524288]
	movzx eax,word [esi+2]
	movq xmm4,[ebx+eax*8 + 1048576]
	movzx eax,word [esi+6]
	movq xmm5,[ebx+eax*8 + 1048576]
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

	lea ebp,[ebp+32]
	lea edi,[edi+32]
	lea esi,[esi+8]
	dec dword [esp+0] ;widthLeft
	jnz y2r8flop3

	movzx eax,word [esi]
	movq xmm0,[ebx+eax*8 + 524288]
	movzx eax,word [esi+2]
	movq xmm1,[ebx+eax*8 + 1048576]
	paddsw xmm0,xmm1

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
	movdqa xmm3,[edi+ecx]
	movdqa xmm6,[ebp+ecx]
	paddsw xmm2,xmm0
	movdqa [edi],xmm2
	psraw xmm0,1
	paddsw xmm3,xmm0
	paddsw xmm6,xmm0
	movdqa [edi+ecx],xmm3
	movdqa [ebp+ecx],xmm6

	lea ebp,[ebp+16]
	lea edi,[edi+16]
	add esi,4
	add esi,dword [esp+76] ;uvAdd
	mov dword [esp+36],esi ;uvPtr

	mov edi,dword [esp+40] ;dest
	mov ecx,dword [esp+44] ;width
	mov esi,dword [esp+64] ;csLineBuff
	mov ebx,dword [esp+84] ;rgbGammaCorr
	ALIGN 16
y2r8flop5:
	movzx eax,word [esi+4]
	movq xmm7,[ebx+eax*8]
	movzx eax,word [esi+2]
	movq xmm6,[ebx+eax*8+524288]
	paddsw xmm7,xmm6
	movzx eax,word [esi]
	movq xmm6,[ebx+eax*8+1048576]
	paddsw xmm7,xmm6
	movq [edi],xmm7
	lea esi,[esi+8]
	lea edi,[edi+8]
	dec ecx
	jnz y2r8flop5

	mov eax,dword [esp+52] ;dbpl
	add dword [esp+40],eax ;dest

	mov edi,dword [esp+40] ;dest
	mov ecx,dword [esp+44] ;width
	ALIGN 16
y2r8flop6:
	movzx eax,word [esi+4]
	movq xmm7,[ebx+eax*8]
	movzx eax,word [esi+2]
	movq xmm6,[ebx+eax*8+524288]
	paddsw xmm7,xmm6
	movzx eax,word [esi]
	movq xmm6,[ebx+eax*8+1048576]
	paddsw xmm7,xmm6
	movq [edi],xmm7
	lea esi,[esi+8]
	lea edi,[edi+8]
	dec ecx
	jnz y2r8flop6

	mov eax,dword [esp+52] ;dbpl
	add dword [esp+40],eax ;dest
	mov eax,dword [esp+64] ;csLineBuff
	xchg eax,dword [esp+68] ;csLineBuff2
	mov dword [esp+64],eax ;;csLineBuff

	dec dword [esp+48] ;heightLeft
	jnz y2r8flop

	test dword [esp+60],1 ;isLast
	jz yv2r8flopexit

	mov esi,dword [esp+32] ;yPtr
	mov ecx,dword [esp+44] ;width
	mov edi,dword [esp+68] ;csLineBuff2
	mov ebx,dword [esp+80] ;yuv2rgb

	shr ecx,1
	ALIGN 16
y2r8flop2b:
	movzx eax,word [esi]
	movq xmm0,[ebx+eax*8]
	movzx eax,word [esi+2]
	movq xmm1,[ebx+eax*8]
	punpcklqdq xmm0,xmm1
	movdqa [edi],xmm0
	lea esi,[esi+4]
	lea edi,[edi+16]
	dec ecx
	jnz y2r8flop2b
	add esi,dword [esp+72] ;yAdd

	mov ecx,dword [esp+44] ;width
	shr ecx,1
	ALIGN 16
y2r8flop2b2:
	movzx eax,word [esi]
	movq xmm0,[ebx+eax*8]
	movzx eax,word [esi+2]
	movq xmm1,[ebx+eax*8]
	punpcklqdq xmm0,xmm1
	movdqa [edi],xmm0
	lea esi,[esi+4]
	lea edi,[edi+16]
	dec ecx
	jnz y2r8flop2b2
	add esi,dword [esp+72] ;yAdd
	mov dword [esp+32],esi ;yPtr

	mov ebp,dword [esp+64] ;csLineBuff
	mov ecx,dword [esp+4] ;cSub
	mov edi,dword [esp+68] ;csLineBuff2
	mov esi,dword [esp+36] ;uvPtr
	shr ecx,1
	mov dword [esp+0],ecx ;widthLeft
	mov ecx,dword [esp+8] ;cSize

	pxor xmm1,xmm1
	movzx eax,word [esi]
	movq xmm0,[ebx+eax*8 + 524288]
	movzx eax,word [esi+2]
	movq xmm4,[ebx+eax*8 + 1048576]
	paddsw xmm0,xmm4

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

	lea ebp,[ebp+16]
	lea edi,[edi+16]
	add esi,4

	align 16
y2r8flop3b:
	movzx eax,word [esi]
	movq xmm0,[ebx+eax*8 + 524288]
	movzx eax,word [esi+4]
	movq xmm1,[ebx+eax*8 + 524288]
	movzx eax,word [esi+2]
	movq xmm4,[ebx+eax*8 + 1048576]
	movzx eax,word [esi+6]
	movq xmm5,[ebx+eax*8 + 1048576]
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

	lea ebp,[ebp+32]
	lea edi,[edi+32]
	lea esi,[esi+8]
	dec dword [esp+0] ;widthLeft
	jnz y2r8flop3b

	pxor xmm1,xmm1
	movzx eax,word [esi]
	movq xmm0,[ebx+eax*8+524288]
	movzx eax,word [esi+2]
	movq xmm4,[ebx+eax*8+1048576]
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

	lea ebp,[ebp+16]
	lea edi,[ebp+16]
	add esi,4
	add esi,dword [esp+76] ;uvAdd
	mov dword [esp+36],esi ;uPtr

	mov ecx,dword [esp+44] ;width
	mov esi,dword [esp+64] ;csLineBuff
	mov edi,dword [esp+40] ;dest
	mov ebx,dword [esp+84] ;rgbGammaCorr
	ALIGN 16
y2r8flop5b:
	movzx eax,word [esi+4]
	movq xmm7,[ebx+eax*8]
	movzx eax,word [esi+2]
	movq xmm6,[ebx+eax*8+524288]
	paddsw xmm7,xmm6
	movzx eax,word [esi]
	movq xmm6,[ebx+eax*8+1048576]
	paddsw xmm7,xmm6
	movq [edi],xmm7
	lea esi,[esi+8]
	lea edi,[edi+8]
	dec ecx
	jnz y2r8flop5b

	mov eax,dword [esp+52] ;dbpl
	add dword [esp+40],eax ;dest

	mov ecx,dword [esp+44] ;width
	mov edi,dword [esp+40] ;dest
	ALIGN 16
y2r8flop6b:
	movzx eax,word [esi+4]
	movq xmm7,[ebx+eax*8]
	movzx eax,word [esi+2]
	movq xmm6,[ebx+eax*8+524288]
	paddsw xmm7,xmm6
	movzx eax,word [esi]
	movq xmm6,[ebx+eax*8+1048576]
	paddsw xmm7,xmm6
	movq [edi],xmm7
	lea esi,[esi+8]
	lea edi,[edi+8]
	dec ecx
	jnz y2r8flop6b

	mov eax,dword [esp+52] ;dbpl
	add dword [esp+40],eax ;dest

	mov eax,dword [esp+64] ;csLineBuff
	xchg eax,dword [esp+68] ;csLineBuff2
	mov dword [esp+64],eax ;csLineBuff

	mov edi,dword [esp+40] ;dest
	mov ecx,dword [esp+44] ;width
	mov esi,dword [esp+64] ;csLineBuff
	mov ebx,dword [esp+84] ;rgbGammaCorr
	ALIGN 16
y2r8flop5c:
	movzx eax,word [esi+4]
	movq xmm7,[ebx+eax*8]
	movzx eax,word [esi+2]
	movq xmm6,[ebx+eax*8+524288]
	paddsw xmm7,xmm6
	movzx eax,word [esi]
	movq xmm6,[ebx+eax*8+1048576]
	paddsw xmm7,xmm6
	movq [edi],xmm7
	lea esi,[esi+8]
	lea edi,[edi+8]
	dec ecx
	jnz y2r8flop5c

	mov eax,dword [esp+54] ;dbpl
	add dword [esp+40],eax ;dest

	mov ecx,dword [esp+44] ;width
	mov edi,dword [esp+40] ;dest
	ALIGN 16
y2r8flop6c:
	movzx eax,word [esi+4]
	movq xmm7,[ebx+eax*8]
	movzx eax,word [esi+2]
	movq xmm6,[ebx+eax*8+524288]
	paddsw xmm7,xmm6
	movzx eax,word [esi]
	movq xmm6,[ebx+eax*8+1048576]
	paddsw xmm7,xmm6
	movq [edi],xmm7
	lea esi,[esi+8]
	lea edi,[edi+8]
	dec ecx
	jnz y2r8flop6c

	mov eax,dword [esp+52] ;dbpl
	add dword [esp+40],eax ;dest

	mov eax,dword [esp+64] ;csLineBuff
	xchg eax,dword [esp+68] ;csLineBuff2
	mov dword [esp+64],eax ;csLineBuff

	align 16
yv2r8flopexit:
	add esp,12
	pop edi
	pop esi
	pop ebx
	pop ebp
	ret

;void CSP016_LRGBC_do_yv12rgb2(UInt8 *yPtr, UInt8 *uvPtr, UInt8 *dest, OSInt width, OSInt height, OSInt dbpl, OSInt isFirst, OSInt isLast, UInt8 *csLineBuff, UInt8 *csLineBuff2, OSInt yBpl, OSInt uvBpl, Int64 *yuv2rgb, Int64 *rgbGammaCorr)
;0 widthLeft
;4 cSub
;8 cSize
;12 edi
;16 esi
;20 ebx
;24 ebp
;28 retAddr
;32 yPtr
;36 uvPtr
;40 dest
;44 width
;48 height
;52 dbpl
;56 isFirst
;60 isLast
;64 csLineBuff
;68 csLineBuff2
;72 yBpl / yAdd
;76 uvBpl / uvAdd
;80 yuv2rgb
;84 rgbGammaCorr

	align 16
_CSP016_LRGBC_do_yv12rgb2:
CSP016_LRGBC_do_yv12rgb2:
	push ebp
	push ebx
	push esi
	push edi
	push eax
	push eax
	push eax
	mov edx,dword [esp+44] ;width
	lea ecx,[edx-2]
	lea eax,[edx*8]
	sub dword [esp+72],ecx ;yAdd					Int32 yAdd = yBpl - width - 2;
	shr edx,1
	inc edx
	mov dword [esp+8],eax ;cSize					Int32 cSize = width << 3;
	sub dword [esp+76],edx ;uvAdd					Int32 uvAdd = uvBpl - ((width + 2) >> 1);
	sub edx,2
	mov dword [esp+4],edx ;cSub						Int32 cSub = (width >> 1) - 2;

	mov ecx,dword [esp+48] ;height
	shr ecx,1
	mov ebx,dword [esp+60] ;isLast
	and ebx,1
	shl ebx,1
	sub ecx,ebx
	mov dword [esp+48],ecx ;heightLeft

	mov ecx,dword [esp+44] ;width
	shr ecx,2
	mov edi,dword [esp+64] ;csLineBuff
	mov ebx,dword [esp+80] ;yuv2rgb
	mov esi,dword [esp+32] ;yPtr

	mov ebp,ecx
	align 16
y2rflop2a:
	movzx eax,word [esi]
	movq xmm0,[ebx+eax * 8]
	movq [edi],xmm0
	movzx eax,word [esi+2]
	movq xmm0,[ebx+eax * 8]
	movq [edi+8],xmm0
	movzx eax,word [esi+4]
	movq xmm0,[ebx+eax * 8]
	movq [edi+16],xmm0
	movzx eax,word [esi+6]
	movq xmm0,[ebx+eax * 8]
	movq [edi+24],xmm0

	add esi,8
	add edi,32
	dec ecx
	jnz y2rflop2a
	movzx eax,word [esi]
	movq xmm0,[ebx+eax * 8]
	movq [edi],xmm0
	movzx eax,word [esi+2]
	movq xmm0,[ebx+eax * 8]
	movq [edi+8],xmm0
	add esi,8
	add edi,16
	add esi,dword [esp+72] ;yAdd
	mov ecx,ebp

	align 16
y2rflop2c:
	movzx eax,word [esi]
	movq xmm0,[ebx+eax * 8]
	movq [edi],xmm0
	movzx eax,word [esi+2]
	movq xmm0,[ebx+eax * 8]
	movq [edi+8],xmm0
	movzx eax,word [esi+4]
	movq xmm0,[ebx+eax * 8]
	movq [edi+16],xmm0
	movzx eax,word [esi+6]
	movq xmm0,[ebx+eax * 8]
	movq [edi+24],xmm0

	add esi,8
	add edi,32
	dec ecx
	jnz y2rflop2c
	movzx eax,word [esi]
	movq xmm0,[ebx+eax * 8]
	movq [edi],xmm0
	movzx eax,word [esi+2]
	movq xmm0,[ebx+eax * 8]
	movq [edi+8],xmm0
	add esi,8
	add edi,16
	add esi,dword [esp+72] ;yAdd
	mov dword [esp+32],esi ;yPtr

	mov ecx,dword [esp+4] ;cSub
	mov edi,dword [esp+64] ;csLineBuff
	mov edx,dword [esp+8] ;cSize
	mov esi,dword [esp+36] ;uvPtr
	mov dword [esp+0],ecx ;widthLeft

	movzx eax,word [esi]
	movq xmm0,[ebx+eax*8 + 524288]
	movzx eax,word [esi+2]
	movq xmm1,[ebx+eax*8 + 1048576]

	movq xmm2,[edi]
	paddsw xmm2,xmm0
	paddsw xmm2,xmm1
	movq [edi],xmm2
	psraw xmm0,1
	psraw xmm1,1
	movq xmm2,[edi+edx]
	paddsw xmm2,xmm0
	paddsw xmm2,xmm1
	movq [edi+edx],xmm2
	movq xmm2,[edi+8]
	paddsw xmm2,xmm0
	paddsw xmm2,xmm1
	movq [edi+8],xmm2
	psraw xmm0,1
	psraw xmm1,1
	movq xmm2,[edi+edx+8]
	paddsw xmm2,xmm0
	paddsw xmm2,xmm1
	movq [edi+edx+8],xmm2

	add edi,16
	add esi,4

	align 16
y2rflop3a:
	movzx eax,word [esi]
	movq xmm0,[ebx+eax*8 + 524288]
	movzx eax,word [esi+2]
	movq xmm1,[ebx+eax*8 + 1048576]

	movq xmm2,[edi]
	paddsw xmm2,xmm0
	paddsw xmm2,xmm1
	movq [edi],xmm2
	psraw xmm0,1
	psraw xmm1,1
	movq xmm2,[edi+edx]
	paddsw xmm2,xmm0
	paddsw xmm2,xmm1
	movq [edi+edx],xmm2
	movq xmm2,[edi+8]
	paddsw xmm2,xmm0
	paddsw xmm2,xmm1
	movq [edi+8],xmm2
	movq xmm2,[edi-8]
	paddsw xmm2,xmm0
	paddsw xmm2,xmm1
	movq [edi-8],xmm2
	psraw xmm0,1
	psraw xmm1,1
	movq xmm2,[edi+edx+8]
	paddsw xmm2,xmm0
	paddsw xmm2,xmm1
	movq [edi+edx+8],xmm2
	movq xmm2,[edi+edx-8]
	paddsw xmm2,xmm0
	paddsw xmm2,xmm1
	movq [edi+edx-8],xmm2

	add edi,16
	add esi,4
	dec dword [esp+0] ;widthLeft
	jnz y2rflop3a


	movzx eax,word [esi]
	movq xmm0,[ebx+eax*8 + 524288]
	movzx eax,word [esi+2]
	movq xmm1,[ebx+eax*8 + 1048576]

	movq xmm2,[edi]
	paddsw xmm2,xmm0
	paddsw xmm2,xmm1
	movq [edi],xmm2
	movq xmm2,[edi+8]
	paddsw xmm2,xmm0
	paddsw xmm2,xmm1
	movq [edi+8],xmm2
	psraw xmm0,1
	psraw xmm1,1
	movq xmm2,[edi+edx]
	paddsw xmm2,xmm0
	paddsw xmm2,xmm1
	movq [edi+edx],xmm2
	movq xmm2,[edi-8]
	paddsw xmm2,xmm0
	paddsw xmm2,xmm1
	movq [edi-8],xmm2
	movq xmm2,[edi+edx+8]
	paddsw xmm2,xmm0
	paddsw xmm2,xmm1
	movq [edi+edx+8],xmm2
	psraw xmm0,1
	psraw xmm1,1
	movq xmm2,[edi+edx-8]
	paddsw xmm2,xmm0
	paddsw xmm2,xmm1
	movq [edi+edx-8],xmm2

	add edi,16
	add esi,4
	add ecx,2
	add esi,dword [esp+76] ;uvAdd
	mov dword [esp+36],esi ;uvPtr
	align 16
y2rflop:

	mov ecx,dword [esp+44] ;width
	mov edi,dword [esp+68] ;csLineBuff2
	mov ebx,dword [esp+80] ;yuv2rgb
	mov esi,dword [esp+32] ;yPtr

	align 16
y2rflop2:
	movzx eax,word [esi]
	movq xmm0,[ebx+eax*8]
	movq [edi],xmm0
	add esi,2
	add edi,8
	dec ecx
	jnz y2rflop2
	add esi,4
	add esi,dword [esp+72] ;yAdd

	mov ecx,dword [esp+44] ;width
	align 16
y2rflop2d:
	movzx eax,word [esi]
	movq xmm0,[ebx+eax*8]
	movq [edi],xmm0
	add esi,2
	add edi,8
	dec ecx
	jnz y2rflop2d
	add esi,4
	add esi,dword [esp+72] ;yAdd

	mov dword [esp+32],esi ;yPtr

	mov ebp,dword [esp+64] ;csLineBuff
	mov ecx,dword [esp+4] ;cSub
	mov edi,dword [esp+68] ;csLineBuff2
	mov esi,dword [esp+36] ;uvPtr
	mov dword [esp+0],ecx ;widthLeft
	mov ecx,dword [esp+8] ;cSize

	movzx eax,word [esi]
	movq xmm0,[ebx+eax*8 + 524288]
	movzx eax,word [esi+2]
	movq xmm1,[ebx+eax*8 + 1048576]

	movq xmm2,[edi]
	paddsw xmm2,xmm0
	paddsw xmm2,xmm1
	movq [edi],xmm2
	psraw xmm0,1
	psraw xmm1,1
	movq xmm2,[edi+ecx]
	paddsw xmm2,xmm0
	paddsw xmm2,xmm1
	movq [edi+ecx],xmm2
	movq xmm2,[ebp+ecx]
	paddsw xmm2,xmm0
	paddsw xmm2,xmm1
	movq [ebp+ecx],xmm2
	movq xmm2,[edi+8]
	paddsw xmm2,xmm0
	paddsw xmm2,xmm1
	movq [edi+8],xmm2
	psraw xmm0,1
	psraw xmm1,1
	movq xmm2,[edi+ecx+8]
	paddsw xmm2,xmm0
	paddsw xmm2,xmm1
	movq [edi+ecx+8],xmm2
	movq xmm2,[ebp+ecx+8]
	paddsw xmm2,xmm0
	paddsw xmm2,xmm1
	movq [ebp+ecx+8],xmm2

	lea ebp,[ebp+16]
	lea edi,[edi+16]
	add esi,4

	align 16
y2rflop3:
	movzx eax,word [esi]
	movq xmm0,[ebx+eax*8 + 524288]
	movzx eax,word [esi+2]
	movq xmm1,[ebx+eax*8 + 1048576]

	movq xmm2,[edi]
	paddsw xmm2,xmm0
	paddsw xmm2,xmm1
	movq [edi],xmm2
	psraw xmm0,1
	psraw xmm1,1
	movq xmm2,[edi+ecx]
	paddsw xmm2,xmm0
	paddsw xmm2,xmm1
	movq [edi+ecx],xmm2
	movq xmm2,[ebp+ecx]
	paddsw xmm2,xmm0
	paddsw xmm2,xmm1
	movq [ebp+ecx],xmm2
	movq xmm2,[edi+8]
	paddsw xmm2,xmm0
	paddsw xmm2,xmm1
	movq [edi+8],xmm2
	movq xmm2,[edi-8]
	paddsw xmm2,xmm0
	paddsw xmm2,xmm1
	movq [edi-8],xmm2
	psraw xmm0,1
	psraw xmm1,1
	movq xmm2,[edi+ecx+8]
	paddsw xmm2,xmm0
	paddsw xmm2,xmm1
	movq [edi+ecx+8],xmm2
	movq xmm2,[ebp+ecx+8]
	paddsw xmm2,xmm0
	paddsw xmm2,xmm1
	movq [ebp+ecx+8],xmm2
	movq xmm2,[edi+ecx-8]
	paddsw xmm2,xmm0
	paddsw xmm2,xmm1
	movq [edi+ecx-8],xmm2
	movq xmm2,[ebp+ecx-8]
	paddsw xmm2,xmm0
	paddsw xmm2,xmm1
	movq [ebp+ecx-8],xmm2

	lea ebp,[ebp+16]
	lea edi,[edi+16]
	add esi,4
	dec dword [esp+0] ;widthLeft
	jnz y2rflop3

	movzx eax,word [esi]
	movq xmm0,[ebx+eax*8 + 524288]
	movzx eax,word [esi+2]
	movq xmm1,[ebx+eax*8 + 1048576]

	movq xmm2,[edi]
	paddsw xmm2,xmm0
	paddsw xmm2,xmm1
	movq [edi],xmm2
	movq xmm2,[edi+8]
	paddsw xmm2,xmm0
	paddsw xmm2,xmm1
	movq [edi+8],xmm2
	psraw xmm0,1
	psraw xmm1,1
	movq xmm2,[edi+ecx]
	paddsw xmm2,xmm0
	paddsw xmm2,xmm1
	movq [edi+ecx],xmm2
	movq xmm2,[ebp+ecx]
	paddsw xmm2,xmm0
	paddsw xmm2,xmm1
	movq [ebp+ecx],xmm2
	movq xmm2,[edi+ecx+8]
	paddsw xmm2,xmm0
	paddsw xmm2,xmm1
	movq [edi+ecx+8],xmm2
	movq xmm2,[ebp+ecx+8]
	paddsw xmm2,xmm0
	paddsw xmm2,xmm1
	movq [ebp+ecx+8],xmm2
	movq xmm2,[edi-8]
	paddsw xmm2,xmm0
	paddsw xmm2,xmm1
	movq [edi-8],xmm2
	psraw xmm0,1
	psraw xmm1,1
	movq xmm2,[edi+ecx-8]
	paddsw xmm2,xmm0
	paddsw xmm2,xmm1
	movq [edi+ecx-8],xmm2
	movq xmm2,[ebp+ecx-8]
	paddsw xmm2,xmm0
	paddsw xmm2,xmm1
	movq [ebp+ecx-8],xmm2

	lea ebp,[ebp+16]
	add edi,16
	add esi,4
	add esi,dword [esp+76] ;uvAdd
	mov dword [esp+36],esi ;uvPtr

	mov ecx,dword [esp+44] ;width
	mov esi,dword [esp+64] ;csLineBuff
	mov edi,dword [esp+40] ;dest
	mov ebx,dword [esp+84] ;rgbGammaCorr
	align 16
y2rflop5:
	movzx eax,word [esi+4]
	movq xmm7,[ebx+eax*8]
	movzx eax,word [esi+2]
	movq xmm6,[ebx+eax*8+524288]
	paddsw xmm7,xmm6
	movzx eax,word [esi]
	movq xmm6,[ebx+eax*8+1048576]
	paddsw xmm7,xmm6
	movq [edi],xmm7
	add esi,8
	add edi,8
	dec ecx
	jnz y2rflop5

	mov eax,dword [esp+52] ;dbpl
	add dword [esp+40],eax ;dest

	mov ecx,dword [esp+44] ;width
	mov edi,dword [esp+40] ;dest
	align 16
y2rflop6:
	movzx eax,word [esi+4]
	movq xmm7,[ebx+eax*8]
	movzx eax,word [esi+2]
	movq xmm6,[ebx+eax*8+524288]
	paddsw xmm7,xmm6
	movzx eax,word [esi]
	movq xmm6,[ebx+eax*8+1048576]
	paddsw xmm7,xmm6
	movq [edi],xmm7
	add esi,8
	add edi,8
	dec ecx
	jnz y2rflop6

	mov eax,dword [esp+52] ;dbpl
	add dword [esp+40],eax ;dest

	mov eax,dword [esp+64] ;csLineBuff
	xchg eax,dword [esp+68] ;csLineBuff2
	mov dword [esp+64],eax ;csLineBuff

	dec dword [esp+48] ;heightLeft
	jnz y2rflop

	test dword [esp+60],1 ;isLast
	jz yv2rflopexit

	mov ecx,dword [esp+44] ;width
	mov edi,dword [esp+68] ;csLineBuff2
	mov ebx,dword [esp+80] ;yuv2rgb
	mov esi,dword [esp+32] ;yPtr

	align 16
y2rflop2b:
	movzx eax,word [esi]
	movq xmm0,[ebx+eax*8]
	movq [edi],xmm0
	add esi,2
	add edi,8
	dec ecx
	jnz y2rflop2b
	add esi,2

	mov ecx,dword [esp+44] ;width
	align 16
y2rflop2e:
	movzx eax,word [esi]
	movq xmm0,[ebx+eax*8]
	movq [edi],xmm0
	add esi,2
	add edi,8
	dec ecx
	jnz y2rflop2e
	add esi,2
	add esi,dword [esp+72] ;yAdd
	mov dword [esp+32],esi ;yPtr

	mov ebp,dword [esp+64] ;csLineBuff
	mov ecx,dword [esp+4] ;cSub
	mov edi,dword [esp+68] ;csLineBuff2
	mov esi,dword [esp+36] ;uvPtr
	mov dword [esp+0],ecx ;widthLeft
	mov ecx,dword [esp+8] ;cSize

	movzx eax,word [esi]
	movq xmm0,[ebx+eax*8 + 524288]
	movzx eax,word [esi+2]
	movq xmm1,[ebx+eax*8 + 1048576]

	movq xmm2,[edi]
	paddsw xmm2,xmm0
	paddsw xmm2,xmm1
	movq [edi],xmm2
	movq xmm2,[edi+ecx]
	paddsw xmm2,xmm0
	paddsw xmm2,xmm1
	movq [edi+ecx],xmm2
	psraw xmm0,1
	psraw xmm1,1
	movq xmm2,[ebp+ecx]
	paddsw xmm2,xmm0
	paddsw xmm2,xmm1
	movq [ebp+ecx],xmm2
	movq xmm2,[edi+8]
	paddsw xmm2,xmm0
	paddsw xmm2,xmm1
	movq [edi+8],xmm2
	movq xmm2,[edi+ecx+8]
	paddsw xmm2,xmm0
	paddsw xmm2,xmm1
	movq [edi+ecx+8],xmm2
	psraw xmm0,1
	psraw xmm1,1
	movq xmm2,[ebp+ecx+8]
	paddsw xmm2,xmm0
	paddsw xmm2,xmm1
	movq [ebp+ecx+8],xmm2

	lea ebp,[ebp+16]
	lea edi,[edi+16]
	add esi,4

	align 16
y2rflop3b:
	movzx eax,word [esi]
	movq xmm0,[ebx+eax*8 + 524288]
	movzx eax,word [esi+2]
	movq xmm1,[ebx+eax*8 + 1048576]

	movq xmm2,[edi]
	paddsw xmm2,xmm0
	paddsw xmm2,xmm1
	movq [edi],xmm2
	movq xmm2,[edi+ecx]
	paddsw xmm2,xmm0
	paddsw xmm2,xmm1
	movq [edi+ecx],xmm2
	psraw xmm0,1
	psraw xmm1,1
	movq xmm2,[ebp+ecx]
	paddsw xmm2,xmm0
	paddsw xmm2,xmm1
	movq [ebp+ecx],xmm2
	movq xmm2,[edi+8]
	paddsw xmm2,xmm0
	paddsw xmm2,xmm1
	movq [edi+8],xmm2
	movq xmm2,[edi-8]
	paddsw xmm2,xmm0
	paddsw xmm2,xmm1
	movq [edi-8],xmm2
	movq xmm2,[edi+ecx+8]
	paddsw xmm2,xmm0
	paddsw xmm2,xmm1
	movq [edi+ecx+8],xmm2
	movq xmm2,[edi+ecx-8]
	paddsw xmm2,xmm0
	paddsw xmm2,xmm1
	movq [edi+ecx-8],xmm2
	psraw xmm0,1
	psraw xmm1,1
	movq xmm2,[ebp+ecx+8]
	paddsw xmm2,xmm0
	paddsw xmm2,xmm1
	movq [ebp+ecx+8],xmm2
	movq xmm2,[ebp+ecx-8]
	paddsw xmm2,xmm0
	paddsw xmm2,xmm1
	movq [ebp+ecx-8],xmm2

	lea ebp,[ebp+16]
	lea edi,[edi+16]
	add esi,4
	dec dword [esp+0] ;widthLeft
	jnz y2rflop3b

	movzx eax,word [esi]
	movq xmm0,[ebx+eax*8 + 524288]
	movzx eax,word [esi+2]
	movq xmm1,[ebx+eax*8 + 1048576]

	movq xmm2,[edi]
	paddsw xmm2,xmm0
	paddsw xmm2,xmm1
	movq [edi],xmm2
	movq xmm2,[edi+8]
	paddsw xmm2,xmm0
	paddsw xmm2,xmm1
	movq [edi+8],xmm2
	movq xmm2,[edi+ecx]
	paddsw xmm2,xmm0
	paddsw xmm2,xmm1
	movq [edi+ecx],xmm2
	movq xmm2,[edi+ecx+8]
	paddsw xmm2,xmm0
	paddsw xmm2,xmm1
	movq [edi+ecx+8],xmm2
	psraw xmm0,1
	psraw xmm1,1
	movq xmm2,[ebp+ecx]
	paddsw xmm2,xmm0
	paddsw xmm2,xmm1
	movq [ebp+ecx],xmm2
	movq xmm2,[ebp+ecx+8]
	paddsw xmm2,xmm0
	paddsw xmm2,xmm1
	movq [ebp+ecx+8],xmm2
	movq xmm2,[edi-8]
	movq [edi-8],xmm2
	psraw xmm0,1
	psraw xmm1,1
	movq xmm2,[edi+ecx-8]
	paddsw xmm2,xmm0
	paddsw xmm2,xmm1
	movq [edi+ecx-8],xmm2
	paddsw xmm2,xmm0
	paddsw xmm2,xmm1
	movq xmm2,[ebp+ecx-8]
	paddsw xmm2,xmm0
	paddsw xmm2,xmm1
	movq [ebp+ecx-8],xmm2

	lea ebp,[ebp+16]
	lea edi,[edi+16]
	add esi,4
	add esi,dword [esp+76] ;uvAdd
	mov dword [esp+36],esi ;uvPtr

	mov ecx,dword [esp+44] ;width
	mov esi,dword [esp+64] ;csLineBuff
	mov edi,dword [esp+40] ;dest
	mov ebx,dword [esp+84] ;rgbGammaCorr
	align 16
y2rflop5b:
	movzx eax,word [esi+4]
	movq xmm7,[ebx+eax*8]
	movzx eax,word [esi+2]
	movq xmm6,[ebx+eax*8+524288]
	paddsw xmm7,xmm6
	movzx eax,word [esi]
	movq xmm6,[ebx+eax*8+1048576]
	paddsw xmm7,xmm6
	movq [edi],xmm7
	add esi,8
	add edi,8
	dec ecx
	jnz y2rflop5b

	mov eax,dword [esp+52] ;dbpl
	add dword [esp+40],eax ;dest

	mov ecx,dword [esp+44] ;width
	mov edi,dword [esp+40] ;dest
	align 16
y2rflop6b:
	movzx eax,word [esi+4]
	movq xmm7,[ebx+eax*8]
	movzx eax,word [esi+2]
	movq xmm6,[ebx+eax*8+524288]
	paddsw xmm7,xmm6
	movzx eax,word [esi]
	movq xmm6,[ebx+eax*8+1048576]
	paddsw xmm7,xmm6
	movq [edi],xmm7
	add esi,8
	add edi,8
	dec ecx
	jnz y2rflop6b

	mov eax,dword [esp+52] ;dbpl
	add dword [esp+40],eax ;dest

	mov esi,dword [esp+64] ;csLineBuff
	mov edi,dword [esp+68] ;csLineBuff2
	mov dword [esp+68],esi ;csLineBuff2
	mov dword [esp+64],edi ;csLineBuff

	mov ecx,dword [esp+44] ;width
	mov esi,dword [esp+64] ;csLineBuff
	mov edi,dword [esp+40] ;dest
	mov ebx,dword [esp+84] ;rgbGammaCorr
	align 16
y2rflop5c:
	movzx eax,word [esi+4]
	movq xmm7,[ebx+eax*8]
	movzx eax,word [esi+2]
	movq xmm6,[ebx+eax*8+524288]
	paddsw xmm7,xmm6
	movzx eax,word [esi]
	movq xmm6,[ebx+eax*8+1048576]
	paddsw xmm7,xmm6
	movq [edi],xmm7
	add esi,8
	add edi,8
	dec ecx
	jnz y2rflop5c

	mov eax,dword [esp+52] ;dbpl
	add dword [esp+40],eax ;dest

	mov ecx,dword [esp+44] ;width
	mov edi,dword [esp+40] ;dest
	align 16
y2rflop6c:
	movzx eax,word [esi+4]
	movq xmm7,[ebx+eax*8]
	movzx eax,word [esi+2]
	movq xmm6,[ebx+eax*8+524288]
	paddsw xmm7,xmm6
	movzx eax,word [esi]
	movq xmm6,[ebx+eax*8+1048576]
	paddsw xmm7,xmm6
	movq [edi],xmm7
	add esi,8
	add edi,8
	dec ecx
	jnz y2rflop6c

	mov eax,dword [esp+52] ;dbpl
	add dword [esp+40],eax ;dest

	mov esi,dword [esp+64] ;csLineBuff
	mov edi,dword [esp+68] ;csLineBuff2
	mov dword [esp+68],esi ;csLineBuff2
	mov dword [esp+64],edi ;csLineBuff

	align 16
yv2rflopexit:
	add esp,12
	pop edi
	pop esi
	pop ebx
	pop ebp
	ret
