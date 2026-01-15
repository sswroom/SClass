section .text

global _CSYUV420_LRGBC_VerticalFilterLRGB
global CSYUV420_LRGBC_VerticalFilterLRGB
global _CSYUV420_LRGBC_do_yv12rgb8
global CSYUV420_LRGBC_do_yv12rgb8
global _CSYUV420_LRGBC_do_yv12rgb2
global CSYUV420_LRGBC_do_yv12rgb2

;void CSYUV420_LRGBC_VerticalFilterLRGB(UInt8 *inYPt, UInt8 *inUPt, UInt8 *inVPt, UInt8 *outPt, IntOS width, IntOS height, IntOS tap, IntOS *index, Int64 *weight, Int32 isFirst, Int32 isLast, UInt8 *csLineBuff, UInt8 *csLineBuff2, IntOS ystep, IntOS dstep, Int64 *yuv2rgb, Int64 *rgbGammaCorr)
; 0 tmpV
; 16 cSub
; 20 sWidth
; 24 edi
; 28 esi
; 32 ebx
; 36 ebp
; 40 retAddr
; 44 inYPt
; 48 inUPt
; 52 inVPt
; 56 outPt
; 60 width
; 64 height / currHeight
; 68 tap
; 72 index
; 76 weight
; 80 isFirst
; 84 isLast
; 88 csLineBuff
; 92 csLineBuff2
; 96 ystep / yAdd
; 100 dstep
; 104 yuv2rgb
; 108 rgbGammaCorr

	align 16
_CSYUV420_LRGBC_VerticalFilterLRGB:
CSYUV420_LRGBC_VerticalFilterLRGB:
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
	mov edx,dword [esp+60] ;width
	mov eax,edx
	mov ecx,edx
	sub dword [esp+96],edx			;IntOS yAdd = ystep - width;
	shr eax,1
	shr ecx,3
	sub eax,2
	mov dword [esp+20],ecx ;sWidth	IntOS sWidth = width >> 3;
	mov dword [esp+16],eax ;cSub	IntOS cSub = (width >> 1) - 2;
	mov ecx,dword [esp+68] ;tap
	cmp ecx,4
	jnz vflexit						; if (tap == 4)
	and edx,7
	jz vflstart
									; if (width & 7)
	shr edx,2
	push edx						; IntOS widthLeft = (width & 7) >> 2;
	movdqu xmm7,[esp+4] ;tmpV

	align 16
vf7lop:
	mov ebx,dword [esp+80] ;weight
	mov esi,dword [esp+96] ;csLineBuff2
	mov ecx,dword [esp+52] ;inUPt
	mov edx,dword [esp+56] ;inVPt

	movdqa xmm5,[ebx+16]
	movdqa xmm6,[ebx+32]

	mov ebp,dword [esp+24] ;sWidth
	ALIGN 16
vf7lop2:
	mov eax,dword [ebx]
	movdqa xmm1,xmm7
	movd xmm4,[ecx+eax]
	movd xmm0,[edx+eax]
	punpcklbw xmm4,xmm0
	pxor xmm0,xmm0
	punpcklbw xmm0,xmm4
	mov eax,dword [ebx+4]
	movdqa xmm3,xmm7
	movd xmm4,[ecx+eax]
	movd xmm2,[edx+eax]
	punpcklbw xmm4,xmm2
	pxor xmm2,xmm2
	punpcklbw xmm2,xmm4
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
	movd xmm4,[ecx+eax]
	movd xmm0,[edx+eax]
	punpcklbw xmm4,xmm0
	pxor xmm0,xmm0
	mov eax,dword [ebx+12]
	punpcklbw xmm0,xmm4
	movd xmm4,[ecx+eax]
	movd xmm2,[edx+eax]
	punpcklbw xmm4,xmm2
	pxor xmm2,xmm2
	punpcklbw xmm2,xmm4
	movdqa xmm4,xmm0
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
	add ecx,4
	add edx,4
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
	movd xmm4,[ecx+eax]
	movd xmm0,[edx+eax]
	punpcklbw xmm4,xmm0
	pxor xmm0,xmm0
	mov eax,dword [ebx+4]
	punpcklbw xmm0,xmm4
	movd xmm4,[ecx+eax]
	movd xmm2,[edx+eax]
	punpcklbw xmm4,xmm2
	pxor xmm2,xmm2
	punpcklbw xmm2,xmm4
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
	movd xmm4,[ecx+eax]
	movd xmm0,[edx+eax]
	punpcklbw xmm4,xmm0
	pxor xmm0,xmm0
	mov eax,dword [ebx+12]
	punpcklbw xmm0,xmm4
	movd xmm4,[ecx+eax]
	movd xmm2,[edx+eax]
	punpcklbw xmm4,xmm2
	pxor xmm2,xmm2
	punpcklbw xmm2,xmm4
	movdqa xmm4,xmm0
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
	lea ecx,[ecx+4]
	lea edx,[edx+4]
;	dec ebp
;	jnz vf7lop3

	align 16
vf7lop3b:
	mov ebp,dword [esp+64] ;width
	and ebp,3
	jz vf7lop3c
	mov eax,dword [ebx]
	movdqa xmm1,xmm7
	movdqa xmm3,xmm7
	movd xmm4,[ecx+eax]
	movd xmm0,[edx+eax]
	punpcklbw xmm4,xmm0
	pxor xmm0,xmm0
	mov eax,dword [ebx+4]
	punpcklbw xmm0,xmm4
	movd xmm4,[ecx+eax]
	movd xmm2,[edx+eax]
	punpcklbw xmm4,xmm2
	pxor xmm2,xmm2
	punpcklbw xmm2,xmm4
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
	movd xmm4,[ecx+eax]
	movd xmm0,[edx+eax]
	punpcklbw xmm4,xmm0
	pxor xmm0,xmm0
	mov eax,dword [ebx+12]
	punpcklbw xmm0,xmm4
	movd xmm4,[ecx+eax]
	movd xmm2,[edx+eax]
	punpcklbw xmm4,xmm2
	pxor xmm2,xmm2
	punpcklbw xmm2,xmm4
	movdqa xmm4,xmm0
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

	mov ebx,dword [esp+108] ;yuv2rgb
	mov ebp,dword [esp+20] ;cSub
	mov edi,dword [esp+92] ;csLineBuff
	mov esi,dword [esp+48] ;inYPt
	mov ecx,dword [esp+96] ;csLineBuff2

	movzx eax,byte [esi]
	movq xmm2,[ebx+eax * 8]
	movzx eax,byte [esi+1]
	movq xmm4,[ebx+eax * 8]
	punpcklqdq xmm2,xmm4

	movzx eax,word [ecx]
	movq xmm0,[ebx+eax*8 + 2048]
	movzx eax,word [ecx+2]
	movq xmm1,[ebx+eax*8 + 526336]
	paddsw xmm0,xmm1

	pxor xmm3,xmm3
	punpcklqdq xmm3,xmm0
	psraw xmm3,1
	paddsw xmm2,xmm0
	paddsw xmm2,xmm3

	lea esi,[esi+2]
	lea ecx,[ecx+4]
	ALIGN 16
vf7lop2b:
	movzx eax,word [ecx]
	movq xmm0,[ebx+eax*8 + 2048]
	movzx eax,word [ecx+2]
	movq xmm1,[ebx+eax*8 + 526336]
	paddsw xmm0,xmm1

	pxor xmm3,xmm3
	punpcklqdq xmm3,xmm0
	psraw xmm3,1
	paddsw xmm2,xmm3
	movdqa [edi],xmm2

	movzx eax,byte [esi]
	movq xmm2,[ebx+eax * 8]
	movzx eax,byte [esi+1]
	movq xmm4,[ebx+eax * 8]
	punpcklqdq xmm2,xmm4

	paddsw xmm2,xmm0
	paddsw xmm2,xmm3
	lea edi,[edi+16]
	lea esi,[esi+2]
	lea ecx,[ecx+4]
	dec ebp
	jnz vf7lop2b

	movzx eax,word [ecx]
	movq xmm0,[ebx+eax*8 + 2048]
	movzx eax,word [ecx+2]
	movq xmm1,[ebx+eax*8 + 526336]
	paddsw xmm0,xmm1

	pxor xmm3,xmm3
	punpcklqdq xmm3,xmm0
	por xmm0,xmm3
	psraw xmm3,1
	paddsw xmm2,xmm3
	movdqa [edi],xmm2

	movzx eax,byte [esi]
	movq xmm2,[ebx+eax * 8]
	movzx eax,byte [esi+1]
	movq xmm4,[ebx+eax * 8]
	punpcklqdq xmm2,xmm4
	paddsw xmm2,xmm0
	movdqa [edi+16],xmm2
	lea esi,[esi+2]
	lea ecx,[ecx+4]
	add esi,dword [esp+100] ;yAdd
	mov dword [esp+48],esi ;inYPt

	mov ecx,dword [esp+64] ;width
	mov esi,dword [esp+92] ;csLineBuff
	mov edi,dword [esp+60] ;outPt
	mov ebx,dword [esp+112] ;rgbGammaCorr
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

	add dword [esp+80],48 ;weight
	mov eax,dword [esp+104] ;dstep
	add dword [esp+60],eax ;outPt

	dec dword [esp+68] ;currHeight
	jnz vf7lop
	pop eax
	jmp vflexit

	align 16
vflstart:
	movdqu xmm7,[esp+0] ;tmpV
	ALIGN 16
vflop:
	mov ebx,dword [esp+76] ;weight
	mov esi,dword [esp+92] ;csLineBuff2
	mov ecx,dword [esp+48] ;inUPt
	mov edx,dword [esp+52] ;inVPt

	movdqa xmm5,[ebx+16]
	movdqa xmm6,[ebx+32]

	mov ebp,dword [esp+20] ;sWidth
	ALIGN 16
vflop2:
	mov eax,dword [ebx]
	movd xmm2,[ecx+eax]
	pxor xmm1,xmm1
	movd xmm4,[edx+eax]
	punpcklbw xmm2,xmm4
	mov eax,dword [ebx+4]
	punpcklbw xmm1,xmm2
	movd xmm2,[ecx+eax]
	movdqa xmm3,xmm1
	movd xmm4,[edx+eax]
	punpcklbw xmm2,xmm4
	pxor xmm4,xmm4
	punpcklbw xmm4,xmm2
	punpcklwd xmm1,xmm4
	punpckhwd xmm3,xmm4
	psrlw xmm1,1
	psrlw xmm3,1

	mov eax,dword [ebx+8]
	pmaddwd xmm1, xmm5
	movd xmm2,[ecx+eax]
	pmaddwd xmm3,xmm5
	movd xmm4,[edx+eax]
	pxor xmm0,xmm0
	punpcklbw xmm2,xmm4
	mov eax,dword [ebx+12]
	punpcklbw xmm0,xmm2
	movd xmm4,[ecx+eax]
	movd xmm2,[edx+eax]
	punpcklbw xmm4,xmm2
	pxor xmm2,xmm2
	punpcklbw xmm2,xmm4
	movdqa xmm4,xmm0
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
	lea ecx,[ecx+4]
	lea edx,[edx+4]
	lea esi,[esi+16]
	dec ebp
	jnz vflop2

	mov ebx,dword [esp+104] ;yuv2rgb
	mov ebp,dword [esp+16] ;cSub
	mov edi,dword [esp+88] ;csLineBuff
	mov esi,dword [esp+44] ;inYPt
	mov ecx,dword [esp+92] ;csLineBuff2

	movzx edx,word [esi]
	movzx eax,dl
	movq xmm2,[ebx+eax * 8]
	shr edx,8
	movq xmm4,[ebx+edx * 8]
	punpcklqdq xmm2,xmm4

	mov edx,dword [ecx]
	movzx eax,dx
	movq xmm0,[ebx+eax*8 + 2048]
	shr edx,16
	movq xmm1,[ebx+edx*8 + 526336]
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
	movq xmm0,[ebx+eax*8 + 2048]
	pxor xmm3,xmm3
	movq xmm1,[ebx+edx*8 + 526336]
	paddsw xmm0,xmm1

	punpcklqdq xmm3,xmm0
	psraw xmm3,1
	paddsw xmm2,xmm3
	movdqa [edi],xmm2

	movzx edx,word [esi]
	movzx eax,dl
	movq xmm2,[ebx+eax * 8]
	shr edx,8
	movq xmm4,[ebx+edx * 8]
	punpcklqdq xmm2,xmm4

	paddsw xmm2,xmm0
	paddsw xmm2,xmm3
	lea edi,[edi+16]
	lea esi,[esi+2]
	lea ecx,[ecx+4]
	dec ebp
	jnz y2rllop2b

	mov edx,dword [ecx]
	movzx eax,dx
	movq xmm0,[ebx+eax*8 + 2048]
	shr edx,16
	movq xmm1,[ebx+edx*8 + 526336]
	paddsw xmm0,xmm1

	pxor xmm3,xmm3
	punpcklqdq xmm3,xmm0
	por xmm0,xmm3
	psraw xmm3,1
	paddsw xmm2,xmm3
	movdqa [edi],xmm2

	movzx edx,word [esi]
	movzx eax,dl
	movq xmm2,[ebx+eax * 8]
	shr edx,8
	movq xmm4,[ebx+edx * 8]
	punpcklqdq xmm2,xmm4
	paddsw xmm2,xmm0
	movdqa [edi+16],xmm2

	add esi,dword [esp+96] ;yAdd
	lea esi,[esi+2]
	mov dword [esp+44],esi ;inYPt

	mov ebp,dword [esp+60] ;width
	mov esi,dword [esp+88] ;csLineBuff
	mov edi,dword [esp+56] ;outPt
	mov ebx,dword [esp+108] ;rgbGammaCorr
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

	mov eax,dword [esp+100] ;dstep
	add dword [esp+76],48 ;weight
	add dword [esp+56],eax ;outPt

	dec dword [esp+64] ;currHeight
	jnz vflop

	align 16
vflexit:
	add esp,24
	pop edi
	pop esi
	pop ebx
	pop ebp
	ret

;void CSYUV420_LRGBC_do_yv12rgb8(UInt8 *yPtr, UInt8 *uPtr, UInt8 *vPtr, UInt8 *dest, IntOS width, IntOS height, IntOS dbpl, IntOS isFirst, IntOS isLast, UInt8 *csLineBuff, UInt8 *csLineBuff2, IntOS yBpl, IntOS uvBpl, Int64 *yuv2rgb, Int64 *rgbGammaCorr)
;0 widthLeft
;4 cSub
;8 cSize
;12 edi
;16 esi
;20 ebx
;24 ebp
;28 retAddr
;32 yPtr
;36 uPtr
;40 vPtr
;44 dest
;48 width
;52 height
;56 dbpl
;60 isFirst
;64 isLast
;68 csLineBuff
;72 csLineBuff2
;76 yBpl / yAdd
;80 uvBpl / uvAdd
;84 yuv2rgb
;88 rgbGammaCorr

	align 16
_CSYUV420_LRGBC_do_yv12rgb8:
CSYUV420_LRGBC_do_yv12rgb8:
	push ebp
	push ebx
	push esi
	push edi
	push eax
	push eax
	push eax
	mov edx,dword [esp+48] ;width
	lea eax,[edx*8]
	sub dword [esp+76],edx ;yAdd					Int32 yAdd = yBpl - width;
	shr edx,1
	mov dword [esp+8],eax ;cSize					Int32 cSize = width << 3;
	sub dword [esp+80],edx ;uvAdd					Int32 uvAdd = uvBpl - (width >> 1);
	sub edx,2
	mov dword [esp+4],edx ;cSub						Int32 cSub = (width >> 1) - 2;

	mov ecx,dword [esp+52] ;height
	shr ecx,1
	mov ebx,dword [esp+64] ;isLast
	and ebx,1
	shl ebx,1
	sub ecx,ebx
	mov dword [esp+52],ecx ;heightLeft

	mov esi,dword [esp+32] ;yPtr
	mov ecx,dword [esp+48] ;width
	mov edi,dword [esp+68] ;csLineBuff
	mov ebx,dword [esp+84] ;yuv2rgb

	shr ecx,3
	align 16
y2r8flop2a:
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
	jnz y2r8flop2a

	mov ecx,dword [esp+48] ;width
	add esi,dword [esp+76] ;yAdd
	shr ecx,3
	ALIGN 16
y2r8flop2a2:
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
	jnz y2r8flop2a2
	add esi,dword [esp+76] ;yAdd
	mov dword [esp+32],esi ;yPtr

	mov ecx,dword [esp+4] ;cSub
	mov edx,dword [esp+8] ;cSize
	mov esi,dword [esp+36] ;uPtr
	mov edi,dword [esp+68] ;csLineBuff
	shr ecx,1
	mov dword [esp+0],ecx ;widthLeft
	mov ecx,dword [esp+40] ;vPtr

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
	inc esi
	inc ecx

	align 16
y2r8flop3a:
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
	lea esi,[esi+2]
	lea ecx,[ecx+2]
	dec dword [esp+0] ;widthLeft
	jnz y2r8flop3a

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
	mov ebp,dword [esp+80] ;uvAdd
	add esi,ebp
	add ecx,ebp
	mov dword [esp+36],esi ;uPtr
	mov dword [esp+40],ecx ;vPtr

	align 16
y2r8flop:
	mov esi,dword [esp+32] ;yPtr
	mov ecx,dword [esp+48] ;width
	mov edi,dword [esp+68] ;csLineBuff2
	mov ebx,dword [esp+84] ;yuv2rgb
	
	shr ecx,1
	align 16
y2r8flop2:
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
	jnz y2r8flop2

	mov ecx,dword [esp+48] ;width
	add esi,dword [esp+76] ;yAdd
	shr ecx,1
	align 16
y2r8flop2_2:
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
	jnz y2r8flop2_2

	add esi,dword [esp+76] ;yAdd
	mov dword [esp+32],esi ;yPtr

	mov ecx,dword [esp+4] ;cSub
	mov esi,dword [esp+36] ;uPtr
	mov edx,dword [esp+40] ;vPtr
	mov ebp,dword [esp+68] ;csLineBuff
	mov edi,dword [esp+72] ;csLineBuff2
	shr ecx,1
	mov dword [esp+0],ecx ;widthLeft
	mov ecx,dword [esp+8] ;cSize

	pxor xmm4,xmm4
	movzx eax,byte [esi]
	movq xmm0,[ebx+eax*8 + 2048]
	movzx eax,byte [edx]
	movq xmm1,[ebx+eax*8 + 4096]
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
	inc esi
	inc edx

	ALIGN 16
y2r8flop3:
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
	lea esi,[esi+2]
	lea edx,[edx+2]
	dec dword [esp+0] ;widthLeft
	jnz y2r8flop3

	movzx eax,byte [esi]
	movq xmm0,[ebx+eax*8 + 2048]
	movzx eax,byte [edx]
	movq xmm1,[ebx+eax*8 + 4096]
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
	inc esi
	inc edx
	add esi,dword [esp+80] ;uvAdd
	add edx,dword [esp+80] ;uvAdd
	mov dword [esp+36],esi ;uPtr
	mov dword [esp+40],edx ;vPtr

	mov edi,dword [esp+44] ;dest
	mov ecx,dword [esp+48] ;width
	mov esi,dword [esp+68] ;csLineBuff
	mov ebx,dword [esp+88] ;rgbGammaCorr
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

	mov eax,dword [esp+56] ;dbpl
	add dword [esp+44],eax ;dest

	mov edi,dword [esp+44] ;dest
	mov ecx,dword [esp+48] ;width
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

	mov eax,dword [esp+56] ;dbpl
	add dword [esp+44],eax ;dest
	mov eax,dword [esp+68] ;csLineBuff
	xchg eax,dword [esp+72] ;csLineBuff2
	mov dword [esp+68],eax ;;csLineBuff

	dec dword [esp+52] ;heightLeft
	jnz y2r8flop

	test dword [esp+64],1 ;isLast
	jz yv2r8flopexit

	mov esi,dword [esp+32] ;yPtr
	mov ecx,dword [esp+48] ;width
	mov edi,dword [esp+72] ;csLineBuff2
	mov ebx,dword [esp+84] ;yuv2rgb

	shr ecx,1
	ALIGN 16
y2r8flop2b:
	movzx eax,byte [esi]
	movq xmm0,[ebx+eax*8]
	movzx eax,byte [esi+1]
	movq xmm1,[ebx+eax*8]
	punpcklqdq xmm0,xmm1
	movdqa [edi],xmm0
	lea esi,[esi+2]
	lea edi,[edi+16]
	dec ecx
	jnz y2r8flop2b
	add esi,dword [esp+76] ;yAdd

	mov ecx,dword [esp+48] ;width
	shr ecx,1
	ALIGN 16
y2r8flop2b2:
	movzx eax,byte [esi]
	movq xmm0,[ebx+eax*8]
	movzx eax,byte [esi+1]
	movq xmm1,[ebx+eax*8]
	punpcklqdq xmm0,xmm1
	movdqa [edi],xmm0
	lea esi,[esi+2]
	lea edi,[edi+16]
	dec ecx
	jnz y2r8flop2b2
	add esi,dword [esp+76] ;yAdd
	mov dword [esp+32],esi ;yPtr

	mov ebp,dword [esp+68] ;csLineBuff
	mov ecx,dword [esp+4] ;cSub
	mov edi,dword [esp+72] ;csLineBuff2
	mov edx,dword [esp+40] ;vPtr
	mov esi,dword [esp+36] ;uPtr
	shr ecx,1
	mov dword [esp+0],ecx ;widthLeft
	mov ecx,dword [esp+8] ;cSize

	pxor xmm1,xmm1
	movzx eax,byte [esi]
	movq xmm0,[ebx+eax*8 + 2048]
	movzx eax,byte [edx]
	movq xmm4,[ebx+eax*8 + 4096]
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
	inc esi
	inc edx

	align 16
y2r8flop3b:
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

	lea ebp,[ebp+32]
	lea edi,[edi+32]
	lea esi,[esi+2]
	lea edx,[edx+2]
	dec dword [esp+0] ;widthLeft
	jnz y2r8flop3b

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

	lea ebp,[ebp+16]
	lea edi,[ebp+16]
	inc esi
	inc edx
	add esi,dword [esp+80] ;uvAdd
	add edx,dword [esp+80] ;uvAdd
	mov dword [esp+36],esi ;uPtr
	mov dword [esp+40],edx ;vPtr

	mov ecx,dword [esp+48] ;width
	mov esi,dword [esp+68] ;csLineBuff
	mov edi,dword [esp+44] ;dest
	mov ebx,dword [esp+88] ;rgbGammaCorr
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

	mov eax,dword [esp+56] ;dbpl
	add dword [esp+44],eax ;dest

	mov ecx,dword [esp+48] ;width
	mov edi,dword [esp+44] ;dest
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

	mov eax,dword [esp+56] ;dbpl
	add dword [esp+44],eax ;dest

	mov eax,dword [esp+68] ;csLineBuff
	xchg eax,dword [esp+72] ;csLineBuff2
	mov dword [esp+68],eax ;csLineBuff

	mov edi,dword [esp+44] ;dest
	mov ecx,dword [esp+48] ;width
	mov esi,dword [esp+68] ;csLineBuff
	mov ebx,dword [esp+88] ;rgbGammaCorr
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

	mov eax,dword [esp+56] ;dbpl
	add dword [esp+44],eax ;dest

	mov ecx,dword [esp+48] ;width
	mov edi,dword [esp+44] ;dest
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

	mov eax,dword [esp+56] ;dbpl
	add dword [esp+44],eax ;dest

	mov eax,dword [esp+68] ;csLineBuff
	xchg eax,dword [esp+72] ;csLineBuff2
	mov dword [esp+68],eax ;csLineBuff

	align 16
yv2r8flopexit:
	add esp,12
	pop edi
	pop esi
	pop ebx
	pop ebp
	ret

;void CSYUV420_LRGBC_do_yv12rgb2(UInt8 *yPtr, UInt8 *uPtr, UInt8 *vPtr, UInt8 *dest, IntOS width, IntOS height, IntOS dbpl, IntOS isFirst, IntOS isLast, UInt8 *csLineBuff, UInt8 *csLineBuff2, IntOS yBpl, IntOS uvBpl, Int64 *yuv2rgb, Int64 *rgbGammaCorr)
;0 widthLeft
;4 cSub
;8 cSize
;12 edi
;16 esi
;20 ebx
;24 ebp
;28 retAddr
;32 yPtr
;36 uPtr
;40 vPtr
;44 dest
;48 width
;52 height
;56 dbpl
;60 isFirst
;64 isLast
;68 csLineBuff
;72 csLineBuff2
;76 yBpl / yAdd
;80 uvBpl / uvAdd
;84 yuv2rgb
;88 rgbGammaCorr

	align 16
_CSYUV420_LRGBC_do_yv12rgb2:
CSYUV420_LRGBC_do_yv12rgb2:
	push ebp
	push ebx
	push esi
	push edi
	push eax
	push eax
	push eax
	mov edx,dword [esp+48] ;width
	lea ecx,[edx-2]
	lea eax,[edx*8]
	sub dword [esp+76],ecx ;yAdd					Int32 yAdd = yBpl - width - 2;
	shr edx,1
	inc edx
	mov dword [esp+8],eax ;cSize					Int32 cSize = width << 3;
	sub dword [esp+80],edx ;uvAdd					Int32 uvAdd = uvBpl - ((width + 2) >> 1);
	sub edx,2
	mov dword [esp+4],edx ;cSub						Int32 cSub = (width >> 1) - 2;

	mov ecx,dword [esp+52] ;height
	shr ecx,1
	mov ebx,dword [esp+64] ;isLast
	and ebx,1
	shl ebx,1
	sub ecx,ebx
	mov dword [esp+52],ecx ;heightLeft

	mov ecx,dword [esp+48] ;width
	shr ecx,2
	mov edi,dword [esp+68] ;csLineBuff
	mov ebx,dword [esp+84] ;yuv2rgb
	mov esi,dword [esp+32] ;yPtr

	mov ebp,ecx
	align 16
y2rflop2a:
	movzx eax,byte [esi]
	movq xmm0,[ebx+eax * 8]
	movq [edi],xmm0
	movzx eax,byte [esi+1]
	movq xmm0,[ebx+eax * 8]
	movq [edi+8],xmm0
	movzx eax,byte [esi+2]
	movq xmm0,[ebx+eax * 8]
	movq [edi+16],xmm0
	movzx eax,byte [esi+3]
	movq xmm0,[ebx+eax * 8]
	movq [edi+24],xmm0

	add esi,4
	add edi,32
	dec ecx
	jnz y2rflop2a
	movzx eax,byte [esi]
	movq xmm0,[ebx+eax * 8]
	movq [edi],xmm0
	movzx eax,byte [esi+1]
	movq xmm0,[ebx+eax * 8]
	movq [edi+8],xmm0
	add esi,4
	add edi,16
	add esi,dword [esp+76] ;yAdd
	mov ecx,ebp

	align 16
y2rflop2c:
	movzx eax,byte [esi]
	movq xmm0,[ebx+eax * 8]
	movq [edi],xmm0
	movzx eax,byte [esi+1]
	movq xmm0,[ebx+eax * 8]
	movq [edi+8],xmm0
	movzx eax,byte [esi+2]
	movq xmm0,[ebx+eax * 8]
	movq [edi+16],xmm0
	movzx eax,byte [esi+3]
	movq xmm0,[ebx+eax * 8]
	movq [edi+24],xmm0

	add esi,4
	add edi,32
	dec ecx
	jnz y2rflop2c
	movzx eax,byte [esi]
	movq xmm0,[ebx+eax * 8]
	movq [edi],xmm0
	movzx eax,byte [esi+1]
	movq xmm0,[ebx+eax * 8]
	movq [edi+8],xmm0
	add esi,4
	add edi,16
	add esi,dword [esp+76] ;yAdd
	mov dword [esp+32],esi ;yPtr

	mov ecx,dword [esp+4] ;cSub
	mov edi,dword [esp+68] ;csLineBuff
	mov edx,dword [esp+8] ;cSize
	mov esi,dword [esp+36] ;uPtr
	mov dword [esp+0],ecx ;widthLeft
	mov ecx,dword [esp+40] ;vPtr


	movzx eax,byte [esi]
	movq xmm0,[ebx+eax*8 + 2048]
	movzx eax,byte [ecx]
	movq xmm1,[ebx+eax*8 + 4096]

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
	inc esi
	inc ecx

	align 16
y2rflop3a:
	movzx eax,byte [esi]
	movq xmm0,[ebx+eax*8 + 2048]
	movzx eax,byte [ecx]
	movq xmm1,[ebx+eax*8 + 4096]

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
	inc esi
	inc ecx
	dec dword [esp+0] ;widthLeft
	jnz y2rflop3a


	movzx eax,byte [esi]
	movq xmm0,[ebx+eax*8 + 2048]
	movzx eax,byte [ecx]
	movq xmm1,[ebx+eax*8 + 4096]

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
	add esi,2
	add ecx,2
	add esi,dword [esp+80] ;uvAdd
	add ecx,dword [esp+80] ;uvAdd
	mov dword [esp+36],esi ;uPtr
	mov dword [esp+40],ecx ;vPtr
	align 16
y2rflop:

	mov ecx,dword [esp+48] ;width
	mov edi,dword [esp+72] ;csLineBuff2
	mov ebx,dword [esp+84] ;yuv2rgb
	mov esi,dword [esp+32] ;yPtr

	align 16
y2rflop2:
	movzx eax,byte [esi]
	movq xmm0,[ebx+eax*8]
	movq [edi],xmm0
	inc esi
	add edi,8
	dec ecx
	jnz y2rflop2
	add esi,2
	add esi,dword [esp+76] ;yAdd

	mov ecx,dword [esp+48] ;width
	align 16
y2rflop2d:
	movzx eax,byte [esi]
	movq xmm0,[ebx+eax*8]
	movq [edi],xmm0
	inc esi
	add edi,8
	dec ecx
	jnz y2rflop2d
	add esi,2
	add esi,dword [esp+76] ;yAdd

	mov dword [esp+32],esi ;yPtr

	mov ebp,dword [esp+68] ;csLineBuff
	mov ecx,dword [esp+4] ;cSub
	mov edi,dword [esp+72] ;csLineBuff2
	mov edx,dword [esp+40] ;vPtr
	mov esi,dword [esp+36] ;uPtr
	mov dword [esp+0],ecx ;widthLeft
	mov ecx,dword [esp+8] ;cSize

	movzx eax,byte [esi]
	movq xmm0,[ebx+eax*8 + 2048]
	movzx eax,byte [edx]
	movq xmm1,[ebx+eax*8 + 4096]

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
	inc esi
	inc edx

	align 16
y2rflop3:
	movzx eax,byte [esi]
	movq xmm0,[ebx+eax*8 + 2048]
	movzx eax,byte [edx]
	movq xmm1,[ebx+eax*8 + 4096]

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
	inc esi
	inc edx
	dec dword [esp+0] ;widthLeft
	jnz y2rflop3

	movzx eax,byte [esi]
	movq xmm0,[ebx+eax*8 + 2048]
	movzx eax,byte [edx]
	movq xmm1,[ebx+eax*8 + 4096]

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
	add esi,2
	add edx,2
	add esi,dword [esp+80] ;uvAdd
	add edi,dword [esp+80] ;uvAdd
	mov dword [esp+36],esi ;uPtr
	mov dword [esp+40],edx ;vPtr

	mov ecx,dword [esp+48] ;width
	mov esi,dword [esp+68] ;csLineBuff
	mov edi,dword [esp+44] ;dest
	mov ebx,dword [esp+88] ;rgbGammaCorr
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

	mov eax,dword [esp+56] ;dbpl
	add dword [esp+44],eax ;dest

	mov ecx,dword [esp+48] ;width
	mov edi,dword [esp+44] ;dest
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

	mov eax,dword [esp+56] ;dbpl
	add dword [esp+44],eax ;dest

	mov eax,dword [esp+68] ;csLineBuff
	xchg eax,dword [esp+72] ;csLineBuff2
	mov dword [esp+68],eax ;csLineBuff

	dec dword [esp+52] ;heightLeft
	jnz y2rflop

	test dword [esp+64],1 ;isLast
	jz yv2rflopexit

	mov ecx,dword [esp+48] ;width
	mov edi,dword [esp+72] ;csLineBuff2
	mov ebx,dword [esp+84] ;yuv2rgb
	mov esi,dword [esp+32] ;yPtr

	align 16
y2rflop2b:
	movzx eax,byte [esi]
	movq xmm0,[ebx+eax*8]
	movq [edi],xmm0
	inc esi
	add edi,8
	dec ecx
	jnz y2rflop2b
	inc esi

	mov ecx,dword [esp+48] ;width
	align 16
y2rflop2e:
	movzx eax,byte [esi]
	movq xmm0,[ebx+eax*8]
	movq [edi],xmm0
	inc esi
	add edi,8
	dec ecx
	jnz y2rflop2e
	inc esi
	add esi,dword [esp+76] ;yAdd
	mov dword [esp+32],esi ;yPtr

	mov ebp,dword [esp+68] ;csLineBuff
	mov ecx,dword [esp+4] ;cSub
	mov edi,dword [esp+72] ;csLineBuff2
	mov edx,dword [esp+40] ;vPtr
	mov esi,dword [esp+36] ;uPtr
	mov dword [esp+0],ecx ;widthLeft
	mov ecx,dword [esp+8] ;cSize

	movzx eax,byte [esi]
	movq xmm0,[ebx+eax*8 + 2048]
	movzx eax,byte [edx]
	movq xmm1,[ebx+eax*8 + 4096]

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
	inc esi
	inc edx

	align 16
y2rflop3b:
	movzx eax,byte [esi]
	movq xmm0,[ebx+eax*8 + 2048]
	movzx eax,byte [edx]
	movq xmm1,[ebx+eax*8 + 4096]

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
	inc esi
	inc edx
	dec dword [esp+0] ;widthLeft
	jnz y2rflop3b

	movzx eax,byte [esi]
	movq xmm0,[ebx+eax*8 + 2048]
	movzx eax,byte [edx]
	movq xmm1,[ebx+eax*8 + 4096]

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
	add esi,2
	add edx,2
	add esi,dword [esp+80] ;uvAdd
	add edx,dword [esp+80] ;uvAdd
	mov dword [esp+36],esi ;uPtr
	mov dword [esp+40],edx ;vPtr

	mov ecx,dword [esp+48] ;width
	mov esi,dword [esp+68] ;csLineBuff
	mov edi,dword [esp+44] ;dest
	mov ebx,dword [esp+88] ;rgbGammaCorr
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

	mov eax,dword [esp+56] ;dbpl
	add dword [esp+44],eax ;dest

	mov ecx,dword [esp+48] ;width
	mov edi,dword [esp+44] ;dest
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

	mov eax,dword [esp+56] ;dbpl
	add dword [esp+44],eax ;dest

	mov esi,dword [esp+68] ;csLineBuff
	mov edi,dword [esp+72] ;csLineBuff2
	mov dword [esp+72],esi ;csLineBuff2
	mov dword [esp+68],edi ;csLineBuff

	mov ecx,dword [esp+48] ;width
	mov esi,dword [esp+68] ;csLineBuff
	mov edi,dword [esp+44] ;dest
	mov ebx,dword [esp+88] ;rgbGammaCorr
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

	mov eax,dword [esp+56] ;dbpl
	add dword [esp+44],eax ;dest

	mov ecx,dword [esp+48] ;width
	mov edi,dword [esp+44] ;dest
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

	mov eax,dword [esp+56] ;dbpl
	add dword [esp+44],eax ;dest

	mov esi,dword [esp+68] ;csLineBuff
	mov edi,dword [esp+72] ;csLineBuff2
	mov dword [esp+72],esi ;csLineBuff2
	mov dword [esp+68],edi ;csLineBuff

	align 16
yv2rflopexit:
	add esp,12
	pop edi
	pop esi
	pop ebx
	pop ebp
	ret
