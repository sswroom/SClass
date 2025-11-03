	section .text

global ImageUtil_SwapRGB
global ImageUtil_ColorReplace32
global ImageUtil_ColorReplace32A
global ImageUtil_ColorReplace32A2
global ImageUtil_ColorFill32
global ImageUtil_ImageColorReplace32
global ImageUtil_ImageMaskABlend32
global ImageUtil_ImageMask2ABlend32
global ImageUtil_ImageColorBuffer32
global ImageUtil_ImageColorFill32
global ImageUtil_ImageColorBlend32
global ImageUtil_ImageFillAlpha32
global ImageUtil_ImageAlphaMul32
global ImageUtil_ImageColorMul32
global ImageUtil_DrawRectNA32
global ImageUtil_ConvP1_B8G8R8A8
global ImageUtil_ConvP2_B8G8R8A8
global ImageUtil_ConvP4_B8G8R8A8
global ImageUtil_ConvP8_B8G8R8A8
global ImageUtil_ConvP1_A1_B8G8R8A8
global ImageUtil_ConvP2_A1_B8G8R8A8
global ImageUtil_ConvP4_A1_B8G8R8A8
global ImageUtil_ConvP8_A1_B8G8R8A8
global ImageUtil_ConvB5G5R5_B8G8R8A8
global ImageUtil_ConvB5G6R5_B8G8R8A8
global ImageUtil_ConvB8G8R8_B8G8R8A8
global ImageUtil_ConvR8G8B8_B8G8R8A8
global ImageUtil_ConvB8G8R8A8_B8G8R8
global ImageUtil_ConvR8G8B8A8_B8G8R8A8
global ImageUtil_ConvR8G8B8N8_B8G8R8A8
global ImageUtil_ConvB16G16R16_B8G8R8A8
global ImageUtil_ConvR16G16B16_B8G8R8A8
global ImageUtil_ConvB16G16R16A16_B8G8R8A8
global ImageUtil_ConvR16G16B16A16_B8G8R8A8
global ImageUtil_ConvA2B10G10R10_B8G8R8A8
global ImageUtil_ConvFB32G32R32A32_B8G8R8A8
global ImageUtil_ConvFR32G32B32A32_B8G8R8A8
global ImageUtil_ConvFB32G32R32_B8G8R8A8
global ImageUtil_ConvFR32G32B32_B8G8R8A8
global ImageUtil_ConvFW32A32_B8G8R8A8
global ImageUtil_ConvFW32_B8G8R8A8
global ImageUtil_ConvP1_B16G16R16A16
global ImageUtil_ConvP2_B16G16R16A16
global ImageUtil_ConvP4_B16G16R16A16
global ImageUtil_ConvP8_B16G16R16A16
global ImageUtil_ConvP1_A1_B16G16R16A16
global ImageUtil_ConvP2_A1_B16G16R16A16
global ImageUtil_ConvP4_A1_B16G16R16A16
global ImageUtil_ConvP8_A1_B16G16R16A16
global ImageUtil_ConvB5G5R5_B16G16R16A16
global ImageUtil_ConvB5G6R5_B16G16R16A16
global ImageUtil_ConvB8G8R8_B16G16R16A16
global ImageUtil_ConvR8G8B8_B16G16R16A16
global ImageUtil_ConvR8G8B8A8_B16G16R16A16
global ImageUtil_ConvB8G8R8A8_B16G16R16A16
global ImageUtil_ConvB16G16R16_B16G16R16A16
global ImageUtil_ConvR16G16B16_B16G16R16A16
global ImageUtil_ConvR16G16B16A16_B16G16R16A16
global ImageUtil_ConvA2B10G10R10_B16G16R16A16
global ImageUtil_ConvFB32G32R32A32_B16G16R16A16
global ImageUtil_ConvFR32G32B32A32_B16G16R16A16
global ImageUtil_ConvFB32G32R32_B16G16R16A16
global ImageUtil_ConvFR32G32B32_B16G16R16A16
global ImageUtil_ConvFW32A32_B16G16R16A16
global ImageUtil_ConvFW32_B16G16R16A16
global ImageUtil_ConvW16_B8G8R8A8
global ImageUtil_ConvW16A16_B8G8R8A8
global ImageUtil_ConvW8A8_B8G8R8A8
global ImageUtil_ConvW16_B16G16R16A16
global ImageUtil_ConvW16A16_B16G16R16A16
global ImageUtil_ConvW8A8_B16G16R16A16
global ImageUtil_ConvP1_P8
global ImageUtil_Rotate32_CW90
global ImageUtil_Rotate32_CW180
global ImageUtil_Rotate32_CW270
global ImageUtil_Rotate64_CW90
global ImageUtil_Rotate64_CW180
global ImageUtil_Rotate64_CW270
global ImageUtil_HFlip32
global ImageUtil_HFRotate32_CW90
global ImageUtil_HFRotate32_CW180
global ImageUtil_HFRotate32_CW270
global ImageUtil_HFlip64
global ImageUtil_HFRotate64_CW90
global ImageUtil_HFRotate64_CW180
global ImageUtil_HFRotate64_CW270
global ImageUtil_CopyShiftW
global ImageUtil_UVInterleaveShiftW
global ImageUtil_YUV_Y416ShiftW

;void ImageUtil_SwapRGB(UInt8 *imgPtr, OSInt pixelCnt, OSInt bpp)
;0 rsi
;8 retAddr
;rcx imgPtr
;rdx pixelCnt
;r8 bpp
	align 16
ImageUtil_SwapRGB:
	push rsi
	mov rsi,rcx ;imgData
	mov rcx,rdx ;pixelCnt
	
	cmp r8,48
	jz srgb48start
	cmp r8,64
	jz srgb64start
	cmp r8,24
	jz srgb24start
	cmp r8,32
	jz srgb32start
	cmp r8,96
	jz srgb96start
	cmp r8,128
	jz srgb128start
	jmp srgbexit

	align 16
srgb48start:
	mov ax,word [rsi]
	mov dx,word [rsi+4]
	mov word [rsi],dx
	mov word [rsi+4],ax
	add rsi,6
	dec rcx
	jnz srgb48start
	jmp srgbexit
	
	align 16
srgb64start:
	mov ax,word [rsi]
	mov dx,word [rsi+4]
	mov word [rsi],dx
	mov word [rsi+4],ax
	add rsi,8
	dec rcx
	jnz srgb64start
	jmp srgbexit
	
	align 16
srgb24start:
	mov al,byte [rsi]
	mov dl,byte [rsi+2]
	mov byte [rsi],dl
	mov byte [rsi+2],al
	add rsi,3
	dec rcx
	jnz srgb24start
	jmp srgbexit

	align 16
srgb32start:
	mov al,byte [rsi]
	mov dl,byte [rsi+2]
	mov byte [rsi],dl
	mov byte [rsi+2],al
	add rsi,4
	dec rcx
	jnz srgb32start
	jmp srgbexit

	align 16
srgb96start:
	mov eax,dword [rsi]
	mov edx,dword [rsi+8]
	mov dword [rsi],edx
	mov dword [rsi+8],eax
	lea rsi,[rsi+12]
	dec rcx
	jnz srgb96start
	jmp srgbexit

	align 16
srgb128start:
	mov eax,dword [rsi]
	mov edx,dword [rsi+8]
	mov dword [rsi],edx
	mov dword [rsi+8],eax
	lea rsi,[rsi+16]
	dec rcx
	jnz srgb128start
	jmp srgbexit

	align 16
srgbexit:
	pop rsi
	ret
	
;void ImageUtil_ColorReplace32(UInt8 *pixelPtr, OSInt w, OSInt h, Int32 col);
;0 rbx
;8 retAddr
;rcx pixelPtr
;rdx w
;r8 h
;r9 col

	align 16
ImageUtil_ColorReplace32:
	push rbx
	mov rax,rdx ;w
	mul r8 ;h
	mov rbx,r9 ;col
	align 16
clop:
	mov edx,dword [rcx]
	test edx,edx
	jz clop2
	mov dword [rcx],ebx
	align 16
clop2:
	lea rcx,[rcx+4]
	dec rax
	jnz clop
	pop rbx
	ret

;void ImageUtil_ColorReplace32A(UInt8 *pixelPtr, OSInt w, OSInt h, Int32 col);
;0 retAddr
;rcx pixelPtr
;rdx w
;r8 h
;r9 col
	align 16
ImageUtil_ColorReplace32A:
	mov rax,r8
	mul rdx
	mov rdx,rcx
	mov rcx,rax
	movd xmm4,r9d
	pxor xmm5,xmm5
	punpckldq xmm4,xmm4
	punpcklbw xmm4,xmm4
	shr rcx,1
	jnb dslop1
	movd xmm0,dword [rdx]
	punpcklbw xmm0,xmm0
	punpcklbw xmm0,xmm5
	pmulhuw xmm0,xmm4
	packuswb xmm0,xmm5
	movd dword [rdx],xmm0
	lea rdx,[rdx+4]
	align 16
dslop1:
	movq xmm0,[rdx]
	punpcklbw xmm0,xmm5
	pmulhuw xmm0,xmm4
	packuswb xmm0,xmm5
	movq [rdx],xmm0
	lea rdx,[rdx+8]
	dec rcx
	jnz dslop1
	ret

;void ImageUtil_ColorReplace32A2(UInt8 *pixelPtr, OSInt w, OSInt h, Int32 col);
;0 retAddr
;rcx pixelPtr
;rdx w
;r8 h
;r9 col
	align 16
ImageUtil_ColorReplace32A2:
	mov rax,r8
	mul rdx
	mov rdx,rcx ;imgPtr
	pxor xmm1,xmm1

	mov rcx,rax ;pxCnt
	movd xmm2,r9d
	punpcklbw xmm2,xmm1
	align 16
cr32a2lop2a:
	mov eax,dword [rdx]
	test eax,eax
	jz cr32a2lop2b
	movd xmm0,eax
	punpcklwd xmm0,xmm0
	punpcklbw xmm0,xmm0
	psrlw xmm0,1
	movdqa xmm3,xmm0
	pmullw xmm0,xmm2
	pmulhw xmm3,xmm2
	punpcklwd xmm0,xmm3
	psrld xmm0,15
	packssdw xmm0,xmm1
	packuswb xmm0,xmm1
	movd [rdx],xmm0

	align 16
cr32a2lop2b:
	lea rdx,[rdx+4]
	dec rcx
	jnz cr32a2lop2a
	ret

;void ImageUtil_ColorFill32(UInt8 *pixelPtr, OSInt pixelCnt, Int32 color);
;0 rdi
;8 retAddr
;rcx pixelPtr
;rdx pixelCnt
;r8 color
	align 16
ImageUtil_ColorFill32:
	push rdi
	mov rdi,rcx ;pixelPtr
	cld
	mov rcx,rdx ;pixelCnt
	mov rax,r8 ;color
	rep stosd
	pop rdi
	ret

;void ImageUtil_ImageColorReplace32(UInt8 *srcPtr, UInt8 *destPtr, OSInt w, OSInt h, OSInt sbpl, OSInt dbpl, Int32 col)
;0 edi
;8 esi
;16 retAddr
;rcx srcPtr
;rdx destPtr
;r8 w
;r9 h
;56 sbpl
;64 dbpl
;72 col
	align 16
ImageUtil_ImageColorReplace32:
	push rsi
	push rdi
	lea rax,[r8 * 4] ;w
	sub qword [rsp+56],rax ;sbpl
	sub qword [rsp+64],rax ;dbpl
	mov rsi,rcx ;srcPtr
	mov rdi,rdx ;destPtr

	mov eax,dword [rsp+72] ;col
	align 16
icr32lop10:
	mov rdx,r8 ;w
	align 16
icr32lop11:
	cmp dword [rsi],0
	jz icr32lop12
	mov dword [rdi],eax
icr32lop12:
	lea rsi,[rsi+4]
	lea rdi,[rdi+4]
	dec rdx
	jnz icr32lop11

	add rsi,qword [rsp+56] ;sbpl
	add rdi,qword [rsp+64] ;dbpl
	dec r9 ;h
	jnz icr32lop10
	
	pop rdi
	pop rsi
	ret
	
;void ImageUtil_ImageMaskABlend32(UInt8 *maskPtr, UInt8 *destPtr, OSInt w, OSInt h, OSInt sbpl, OSInt dbpl, Int32 col);
;0 edi
;8 esi
;16 retAddr
;rcx maskPtr
;rdx destPtr
;r8 w
;r9 h
;56 sbpl
;64 dbpl
;72 col
	align 16
ImageUtil_ImageMaskABlend32:
	push rsi
	push rdi
	mov rsi,rcx ;maskPtr
	mov rdi,rdx ;destPtr
	
	lea rax,[r8*4] ;w
	sub qword [rsp+56],rax ;sbpl
	sub qword [rsp+64],rax ;dbpl

	mov eax,dword [rsp+72] ;col
	mov edx,eax
	mov ecx,0xff
	shr edx,24
	sub ecx,edx
	and eax,0xffffff
	movd xmm4,edx
	movd xmm5,ecx
	movd xmm2,eax
	mov eax,0xff000000
	and eax,dword [rsp+36] ;col
	movd xmm3,eax
	punpcklbw xmm4,xmm4
	punpcklbw xmm5,xmm5
	punpcklbw xmm4,xmm4
	punpcklbw xmm5,xmm5
	punpcklbw xmm4,xmm4
	punpcklbw xmm5,xmm5
	punpcklbw xmm3,xmm3
	punpcklbw xmm2,xmm2
	pmulhuw xmm4,xmm2
	por xmm4,xmm3
	pxor xmm3,xmm3

	align 16
imab32lop10a:
	mov rdx,r8 ;w
	align 16
imab32lop11a:
	cmp dword [rsi],0
	jz imab32lop12a
	movd xmm0,dword [rdi]
	punpcklbw xmm0,xmm0
	pmulhuw xmm0,xmm5
	paddusw xmm0,xmm4
	psrlw xmm0,8
	packuswb xmm0,xmm3
	movd dword [rdi],xmm0
	align 16
imab32lop12a:
	lea rsi,[rsi+4]
	lea rdi,[rdi+4]
	dec rdx
	jnz imab32lop11a

	add rsi,qword [rsp+56] ;sbpl
	add rdi,qword [rsp+64] ;dbpl
	dec r9 ;h
	jnz imab32lop10a
	pop rdi
	pop rsi
	ret

;void ImageUtil_ImageMask2ABlend32(UInt8 *maskPtr, UInt8 *destPtr, OSInt w, OSInt h, OSInt sbpl, OSInt dbpl, Int32 col1, Int32 col2)
;0 xmm6
;16 xmm7
;32 rdi
;40 rsi
;48 retAddr
;rcx maskPtr
;rdx destPtr
;r8 w
;r9 h
;88 sbpl
;96 dbpl
;104 col1
;112 col2
	align 16
ImageUtil_ImageMask2ABlend32:
	push rsi
	push rdi
	sub rsp,32
	movdqu [rsp],xmm6
	movdqu [rsp+16],xmm7
	mov rsi,rcx ;maskPtr
	mov rdi,rdx ;destPtr
	
	lea rax,[r8*4] ;w
	sub qword [rsp+88],rax ;sbpl
	sub qword [rsp+96],rax ;dbpl

	mov edx,0xff000000
	mov ecx,0xffffffff
	movd xmm3,edx
	movd xmm4,ecx
	punpcklbw xmm3,xmm3 ;mortmp
	punpcklbw xmm4,xmm4 ;mtmp

	movzx eax,byte [rsp+107] ;col1[3]
	movd xmm1,eax
	punpcklbw xmm1,xmm1
	punpcklbw xmm1,xmm1
	punpcklbw xmm1,xmm1
		
	movq xmm2,xmm1
	pxor xmm1,xmm4 ;mtmp
	movq xmm6,xmm1 ;cmul1
	por xmm2,xmm3 ;mortmp

	movd xmm7,[rsp+104] ;col1
	punpcklbw xmm7,xmm7
	pmulhuw xmm7,xmm2 ;cadd1

	movzx eax,byte [rsp+115] ;col2[3]
	movd xmm1,eax
	punpcklbw xmm1,xmm1
	punpcklbw xmm1,xmm1
	punpcklbw xmm1,xmm1
	
	movq xmm2,xmm1
	pxor xmm4,xmm1 ;mtmp cmul2
	por xmm2,xmm3 ;mortmp
	
	movd xmm5,[rsp+112] ;col2
	punpcklbw xmm5,xmm5
	pmulhuw xmm5,xmm2 ;cadd2
	
	cld

	align 16
im2ab32lop3:
	mov rcx,r8 ;w
	align 16
im2ab32lop:
	lodsd
	cmp eax,0xff000000
	jz im2ab32lop1
	cmp eax,0x00000000
	jz im2ab32lop2

	movd xmm0,[rdi]
	punpcklbw xmm0,xmm0

	pmulhuw xmm0,xmm4 ;cmul2
	paddusw xmm0,xmm5 ;cadd2
	psrlw xmm0,8
	packuswb xmm0,xmm0
	movd [rdi],xmm0

	jmp im2ab32lop1
	align 16
im2ab32lop2:
	movd xmm0,[rdi]
	punpcklbw xmm0,xmm0

	pmulhuw xmm0,xmm6 ;cmul1
	paddusw xmm0,xmm7 ;cadd1
	psrlw xmm0,8
	packuswb xmm0,xmm0
	movd [rdi],xmm0

	align 16
im2ab32lop1:
	add rdi,4
	dec rcx
	jnz im2ab32lop
	
	add rsi,qword [rsp+88] ;sbpl
	add rdi,qword [rsp+96] ;dbpl
	dec r9 ;h
	jnz im2ab32lop3
	
	movdqu xmm6,[rsp]
	movdqu xmm7,[rsp+16]
	add rsp,32
	pop rdi
	pop rsi
	ret
	
;void ImageUtil_ImageColorBuffer32(UInt8 *pixelPtr, OSInt w, OSInt h, OSInt bpl, OSInt buffSize)
;0 rdi
;8 rsi
;16 rbx
;24 rbp
;32 retAddr
;rcx pixelPtr
;rdx w r11
;r8 h
;r9 bpl
;72 buffSize
	align 16
ImageUtil_ImageColorBuffer32:
	push rbp
	push rbx
	push rsi
	push rdi
	mov rsi,rcx ;pixelPtr
	mov r11,rdx
	mov rbx,r9 ;bpl
	lea rax,[r11*4] ;w
	sub rbx,rax ;bpl
	mov r10,rbx ;bpl - w * 4
	mov rbx,qword [rsp+72] ;buffSize
	
	align 16
icb32lop:
	mov rcx,r11 ;w
	
	align 16
icb32lop2:
	test dword [rsi],0x808080
	jz icb32lop3
	mov rbp,rsi
	mov rdi,rsi
	xor rax,rax
	align 16
icb32lop4:
	mov rdx,rbx ;buffSize
	sub rdx,rax
	align 16
icb32lop7:
	cmp dword [rbp+rdx*4],0
	jnz icb32lop6a
	mov dword [rbp+rdx*4],0x7f7f7f
icb32lop6a:
	cmp dword [rdi+rdx*4],0
	jnz icb32lop6c
	mov dword [rdi+rdx*4],0x7f7f7f
icb32lop6c:
	neg rdx
	cmp dword [rbp+rdx*4],0
	jnz icb32lop6b
	mov dword [rbp+rdx*4],0x7f7f7f
icb32lop6b:
	cmp dword [rdi+rdx*4],0
	jnz icb32lop6d
	mov dword [rdi+rdx*4],0x7f7f7f
icb32lop6d:
	neg rdx
	jnb icb32lop5
	dec rdx
	jmp icb32lop7
	
	align 16
icb32lop5:
	inc rax
	add rbp,r9 ;bpl
	sub rdi,r9 ;bpl
	cmp rax,rbx ;buffSize
	jbe icb32lop4

	align 16
icb32lop3:
	add rsi,4
	dec rcx
	jnz icb32lop2
	add rsi,r10 ;bpl - w * 4
	dec r8 ;h
	jnz icb32lop
	
	pop rdi
	pop rsi
	pop rbx
	pop rbp
	ret

;void ImageUtil_ImageColorFill32(UInt8 *pixelPtr, OSInt w, OSInt h, OSInt bpl, Int32 col)
;0 edi
;8 retAddr
;rcx pixelPtr
;rdx w
;r8 h
;r9 bpl
;48 col
	align 16
ImageUtil_ImageColorFill32:
	push rdi
	cld
	lea rax,[rdx*4] ;w
	mov rdi,rcx ;pixelPtr
	sub r9,rax ;bpl
	mov eax,dword [rsp+48] ;col
	align 16
icf32lop4:
	mov rcx,rdx
	rep stosd
	add rdi,r9
	dec r8
	jnz icf32lop4
	pop rdi
	ret

;void ImageUtil_ImageColorBlend32(UInt8 *pixelPtr, OSInt w, OSInt h, OSInt bpl, Int32 col)
;0 xmm6
;16 xmm7
;32 retAddr
;rcx pixelPtr
;rdx w
;r8 h
;r9 bpl
;72 col
	align 16
ImageUtil_ImageColorBlend32:
	sub rsp,32
	movdqu [rsp],xmm6
	movdqu [rsp+16],xmm7
	mov r10,rcx
	mov r11,rdx
	movzx eax,byte [rsp+75] ;col[3]
	mov edx,0xff000000
	mov ecx,0xffffffff
	movd xmm1,eax
	punpcklbw xmm1,xmm1
	punpcklbw xmm1,xmm1
	punpcklbw xmm1,xmm1
	
	movd xmm3,edx
	movd xmm4,ecx
	punpcklbw xmm3,xmm3 ;mortmp
	punpcklbw xmm4,xmm4 ;mtmp
	movq xmm2,xmm1
	pxor xmm1,xmm4
	movdqa xmm5,xmm1 ;cimul
	por xmm2,xmm3 ;mortmp

	movd xmm0,[rsp+72] ;col
	punpcklbw xmm0,xmm0
	pmulhuw xmm0,xmm2
	movq xmm6,xmm0 ;cadd
	pxor xmm3,xmm3

	lea rax,[r11*4] ;w
	sub r9,rax ;bpl
	mov rdx,r10 ;initOfst
	align 16
icbl32lop:
	mov rcx,r11 ;w2
	shr rcx,1
	align 16
icbl32lop2:
	movd xmm0,[rdx]
	movd xmm1,[rdx+4]
	punpcklbw xmm0,xmm0
	punpcklbw xmm1,xmm1

	pmulhuw xmm0,xmm5 ;cimul
	paddusw xmm0,xmm6 ;cadd
	pmulhuw xmm1,xmm5 ;cimul
	paddusw xmm1,xmm6 ;cadd
	punpcklqdq xmm0,xmm1
	psrlw xmm0,8
	packuswb xmm0,xmm1
	movq [rdx],xmm0

	lea rdx,[rdx+8]
	dec rcx
	jnz icbl32lop2

	test r11,1 ;w
	jz icbl32lop3

	movd xmm0,[rdx]
	punpcklbw xmm0,xmm0

	pmulhuw xmm0,xmm5 ;cimul
	paddusw xmm0,xmm6 ;cadd
	psrlw xmm0,8
	packuswb xmm0,xmm3
	movd [rdx],xmm0
	lea rdx,[rdx+4]
	
	align 16
icbl32lop3:
	add rdx,r9 ;bpl
	dec r8 ;hleft
	jnz icbl32lop
	movdqu xmm6,[rsp]
	movdqu xmm7,[rsp+16]
	add rsp,32
	ret
	
;void ImageUtil_ImageFillAlpha32(UInt8 *pixelPtr, OSInt w, OSInt h, OSInt bpl, UInt8 a);
;0 retAddr
;rcx pixelPtr
;rdx w
;r8 h
;r9 bpl
;40 a

	align 16
ImageUtil_ImageFillAlpha32:
	mov r11,rdx
	lea rax,[rdx*4] ;w
	sub r9,rax ;bpl
	
	mov al,byte [rsp+40] ;a
	align 16
ifa32lop:
	mov rdx,r11 ;w
	align 16
ifa32lop2:
	mov byte [rcx+3],al
	add rcx,4
	dec rdx
	jnz ifa32lop2
	add rcx,r9 ;bpl
	dec r8 ;h
	jnz ifa32lop
	ret

;void ImageUtil_ImageAlphaMul32(UInt8 *pixelPtr, OSInt w, OSInt h, OSInt bpl, UInt32 a);
;0 rdi
;8 rbx
;16 retAddr
;rcx pixelPtr
;rdx w
;r8 h
;r9 bpl
;56 a

	align 16
ImageUtil_ImageAlphaMul32:
	push rbx
	push rdi
	mov r11,rdx
	lea rax,[rdx*4] ;w
	sub r9,rax ;bpl
	mov rdi,rcx ;pbits
	mov ebx,dword [rsp+56] ;v
	align 16
iam32lop2:
	mov rcx,r11 ;w
	
	align 16
iam32lop:
	movzx eax,byte [rdi+3]
	mul ebx
	shr eax,16
	mov byte [rdi+3],al
	add rdi,4
	dec rcx
	jnz iam32lop
	add rdi,r9 ;bpl
	dec r8 ;h
	jnz iam32lop2
	pop rdi
	pop rbx
	ret

;void ImageUtil_ImageColorMul32(UInt8 *pixelPtr, OSInt w, OSInt h, OSInt bpl, UInt32 c);
;0 rdi
;8 rbx
;16 retAddr
;rcx pixelPtr
;rdx w
;r8 h
;r9 bpl
;56 c

	align 16
ImageUtil_ImageColorMul32:
	push rbx
	push rdi
	mov r11,rdx
	lea rax,[rdx*4] ;w
	sub r9,rax ;bpl
	mov rdi,rcx ;pbits
	movd xmm1,dword [rsp+56] ;c
	punpcklbw xmm1,xmm1
	pxor xmm2,xmm2
	align 16
icm32lop2:
	mov rcx,r11 ;w
	
	align 16
icm32lop:
	movd xmm0, dword [rdi]
	punpcklbw xmm0,xmm2
	pmulhuw xmm0,xmm1
	packuswb xmm0,xmm2
	movd dword [rdi],xmm0
	add rdi,4
	dec rcx
	jnz icm32lop
	add rdi,r9 ;bpl
	dec r8 ;h
	jnz icm32lop2
	pop rdi
	pop rbx
	ret
	
;void ImageUtil_DrawRectNA32(UInt8 *pixelPtr, OSInt w, OSInt h, OSInt bpl, UInt32 col)
;0 rbx
;8 retAddr
;rcx pixelPtr
;rdx w
;r8 h
;r9 bpl
;48 col

	align 16
ImageUtil_DrawRectNA32:
	push rbx
	mov r10,r9
	lea rbx,[rdx*4]
	mov eax,dword [rsp+48] ;col
	sub r10,rbx
	sub r8,2
	jb drna32exit
	jz drna32lop
	
	mov rbx,rdx
	align 16
drna32lop2:
	mov dword [rcx],eax
	lea rcx,[rcx+4]
	dec rbx
	jnz drna32lop2
	lea rcx,[rcx+r10]
	
	align 16
drna32lop3:
	mov dword [rcx],eax
	mov dword [rcx+rdx*4-4],eax
	lea rcx,[rcx+r9]
	dec r8
	jnz drna32lop3
	
	mov rbx,rdx
	align 16
drna32lop4:
	mov dword [rcx],eax
	lea rcx,[rcx+4]
	dec rbx
	jnz drna32lop4
	jmp drna32exit

	align 16
drna32lop:	
	mov rbx,rdx
	align 16
drna32lop5:
	mov dword [rcx],eax
	lea rcx,[rcx+4]
	dec rbx
	jnz drna32lop5
	lea rcx,[rcx+r10]

	mov rbx,rdx
	align 16
drna32lop6:
	mov dword [rcx],eax
	lea rcx,[rcx+4]
	dec rbx
	jnz drna32lop6
	
	align 16
drna32exit:
	pop rbx
	ret

;void ImageUtil_ConvP1_B8G8R8A8(UInt8 *srcPtr, UInt8 *destPtr, OSInt w, OSInt h, OSInt sbpl, OSInt dbpl, UInt8 *pal);
;0 rsi
;8 retAddr
;rcx srcPtr
;rdx destPtr
;r8 w
;r9 h
;48 sbpl
;56 dbpl
;64 pal rsi
	align 16
ImageUtil_ConvP1_B8G8R8A8:
	push rsi
	mov rsi,qword [rsp+64] ;pal
	lea rax,[r8*4]
	test r8,7
	jnz cp1_bgra8lop
	
	shr r8,3
	sub qword [rsp+48],r8 ;sbpl
	sub qword [rsp+56],rax ;dbpl
	align 16
cp1_bgra8lop2:
	mov r10,r8
	align 16
cp1_bgra8lop3:
	movzx rax,byte [rcx]
	shr rax,7
	mov eax,dword [rsi+rax*4]
	movnti dword [rdx],eax
	movzx rax,byte [rcx]
	shr rax,6
	and rax,1
	mov eax,dword [rsi+rax*4]
	movnti dword [rdx+4],eax
	movzx rax,byte [rcx]
	shr rax,5
	and rax,1
	mov eax,dword [rsi+rax*4]
	movnti dword [rdx+8],eax
	movzx rax,byte [rcx]
	shr rax,4
	and rax,1
	mov eax,dword [rsi+rax*4]
	movnti dword [rdx+12],eax
	movzx rax,byte [rcx]
	shr rax,3
	and rax,1
	mov eax,dword [rsi+rax*4]
	movnti dword [rdx+16],eax
	movzx rax,byte [rcx]
	shr rax,2
	and rax,1
	mov eax,dword [rsi+rax*4]
	movnti dword [rdx+20],eax
	movzx rax,byte [rcx]
	shr rax,1
	and rax,1
	mov eax,dword [rsi+rax*4]
	movnti dword [rdx+24],eax
	movzx rax,byte [rcx]
	and rax,1
	mov eax,dword [rsi+rax*4]
	lea rcx,[rcx+1]
	movnti dword [rdx+28],eax
	lea rdx,[rdx+32]
	dec r10
	jnz cp1_bgra8lop3
	
	add rcx,qword [rsp+48] ;sbpl
	add rdx,qword [rsp+56] ;dbpl
	dec r9
	jnz cp1_bgra8lop2
	jmp cp1_bgra8exit
	
	align 16
cp1_bgra8lop:
	push rdi
	mov r11,r8
	shr r8,3
	sub qword [rsp+56],r8 ;sbpl
	sub qword [rsp+64],rax ;dbpl
	align 16
cp1_bgra8lop4:
	mov r10,r8
	test r10,r10
	jz cp1_bgra8lop7

	align 16
cp1_bgra8lop5:
	movzx rax,byte [rcx]
	xor rdi,rdi
	shl al,1
	adc rdi,0
	mov edi,dword [rsi+rdi*4]
	movnti dword [rdx],edi
	xor rdi,rdi
	shl al,1
	adc rdi,0
	mov edi,dword [rsi+rdi*4]
	movnti dword [rdx+4],edi
	xor rdi,rdi
	shl al,1
	adc rdi,0
	mov edi,dword [rsi+rdi*4]
	movnti dword [rdx+8],edi
	xor rdi,rdi
	shl al,1
	adc rdi,0
	mov edi,dword [rsi+rdi*4]
	movnti dword [rdx+12],edi
	xor rdi,rdi
	shl al,1
	adc rdi,0
	mov edi,dword [rsi+rdi*4]
	movnti dword [rdx+16],edi
	xor rdi,rdi
	shl al,1
	adc rdi,0
	mov edi,dword [rsi+rdi*4]
	movnti dword [rdx+20],edi
	xor rdi,rdi
	shl al,1
	adc rdi,0
	mov edi,dword [rsi+rdi*4]
	movnti dword [rdx+24],edi
	xor rdi,rdi
	shl al,1
	adc rdi,0
	mov edi,dword [rsi+rdi*4]
	movnti dword [rdx+28],edi
	lea rcx,[rcx+1]
	lea rdx,[rdx+32]
	dec r10
	jnz cp1_bgra8lop5

	align 16
cp1_bgra8lop7:
	mov r10,r11
	and r10,7
	movzx rax,byte [rcx]
	align 16
cp1_bgra8lop6:
	xor rdi,rdi
	shl al,1
	adc rdi,0
	mov edi,dword [rsi+rdi*4]
	movnti dword [rdx],edi
	lea rdx,[rdx+4]
	dec r10
	jnz cp1_bgra8lop6
	
	add rcx,qword [rsp+56] ;sbpl
	add rdx,qword [rsp+64] ;dbpl
	dec r9
	jnz cp1_bgra8lop4
	pop rdi

	align 16
cp1_bgra8exit:
	pop rsi
	ret

;void ImageUtil_ConvP2_B8G8R8A8(UInt8 *srcPtr, UInt8 *destPtr, OSInt w, OSInt h, OSInt sbpl, OSInt dbpl, UInt8 *pal);
;0 rsi
;8 retAddr
;rcx srcPtr
;rdx destPtr
;r8 w
;r9 h
;48 sbpl
;56 dbpl
;64 pal rsi
	align 16
ImageUtil_ConvP2_B8G8R8A8:
	push rsi
	mov rsi,qword [rsp+64] ;pal
	lea rax,[r8*4]
	test r8,3
	jnz cp2_bgra8lop
	
	shr r8,2
	sub qword [rsp+48],r8 ;sbpl
	sub qword [rsp+56],rax ;dbpl
	align 16
cp2_bgra8lop2:
	mov r10,r8
	align 16
cp2_bgra8lop3:
	movzx rax,byte [rcx]
	shr rax,6
	and rax,3
	mov eax,dword [rsi+rax*4]
	movnti dword [rdx],eax
	movzx rax,byte [rcx]
	shr rax,4
	and rax,3
	mov eax,dword [rsi+rax*4]
	movnti dword [rdx+4],eax
	movzx rax,byte [rcx]
	shr rax,2
	and rax,3
	mov eax,dword [rsi+rax*4]
	movnti dword [rdx+8],eax
	movzx rax,byte [rcx]
	and rax,3
	mov eax,dword [rsi+rax*4]
	movnti dword [rdx+12],eax
	lea rcx,[rcx+1]
	lea rdx,[rdx+16]
	dec r10
	jnz cp2_bgra8lop3
	
	add rcx,qword [rsp+48] ;sbpl
	add rdx,qword [rsp+56] ;dbpl
	dec r9
	jnz cp2_bgra8lop2
	jmp cp2_bgra8exit
	
	align 16
cp2_bgra8lop:
	push rdi
	mov r11,r8
	shr r8,2
	sub qword [rsp+56],r8 ;sbpl
	sub qword [rsp+64],rax ;dbpl
	align 16
cp2_bgra8lop4:
	mov r10,r8
	test r10,r10
	jz cp2_bgra8lop7
	
	align 16
cp2_bgra8lop5:
	movzx rax,byte [rcx]
	rol al,2
	mov rdi,rax
	and rdi,3
	mov edi,dword [rsi+rdi*4]
	movnti dword [rdx],edi
	rol al,2
	mov rdi,rax
	and rdi,3
	mov edi,dword [rsi+rdi*4]
	movnti dword [rdx+4],edi
	rol al,2
	mov rdi,rax
	and rdi,3
	mov edi,dword [rsi+rdi*4]
	movnti dword [rdx+8],edi
	rol al,2
	mov rdi,rax
	and rdi,3
	mov edi,dword [rsi+rdi*4]
	movnti dword [rdx+12],edi
	lea rcx,[rcx+1]
	lea rdx,[rdx+16]
	dec r10
	jnz cp2_bgra8lop5

	align 16
cp2_bgra8lop7:
	mov r10,r11
	and r10,3
	movzx rax,byte [rcx]
	align 16
cp2_bgra8lop6:
	rol al,2
	mov rdi,rax
	and rdi,3
	mov edi,dword [rsi+rdi*4]
	movnti dword [rdx],edi
	lea rdx,[rdx+4]
	dec r10
	jnz cp2_bgra8lop6
	
	add rcx,qword [rsp+56] ;sbpl
	add rdx,qword [rsp+64] ;dbpl
	dec r9
	jnz cp2_bgra8lop4
	pop rdi

	align 16
cp2_bgra8exit:
	pop rsi
	ret

;void ImageUtil_ConvP4_B8G8R8A8(UInt8 *srcPtr, UInt8 *destPtr, OSInt w, OSInt h, OSInt sbpl, OSInt dbpl, UInt8 *pal);
;0 rsi
;8 retAddr
;rcx srcPtr
;rdx destPtr
;r8 w
;r9 h
;48 sbpl
;56 dbpl
;64 pal rsi
	align 16
ImageUtil_ConvP4_B8G8R8A8:
	push rsi
	mov rsi,qword [rsp+64] ;pal
	lea rax,[r8*4]
	shr r8,1
	jb cp4_bgra8lop
	
	sub qword [rsp+48],r8 ;sbpl
	sub qword [rsp+56],rax ;dbpl
	align 16
cp4_bgra8lop2:
	mov r10,r8
	align 16
cp4_bgra8lop3:
	movzx rax,byte [rcx]
	shr rax,4
	mov eax,dword [rsi+rax*4]
	movnti dword [rdx],eax
	movzx rax,byte [rcx]
	and rax,15
	mov eax,dword [rsi+rax*4]
	lea rcx,[rcx+1]
	movnti dword [rdx+4],eax
	lea rdx,[rdx+8]
	dec r10
	jnz cp4_bgra8lop3
	
	add rcx,qword [rsp+48] ;sbpl
	add rdx,qword [rsp+56] ;dbpl
	dec r9
	jnz cp4_bgra8lop2
	jmp cp4_bgra8exit
	
	align 16
cp4_bgra8lop:
	sub qword [rsp+48],r8 ;sbpl
	sub qword [rsp+56],rax ;dbpl
	align 16
cp4_bgra8lop4:
	mov r10,r8
	test r10,r10
	jz cp4_bgra8lop7
	
	align 16
cp4_bgra8lop5:
	movzx rax,byte [rcx]
	shr rax,4
	mov eax,dword [rsi+rax*4]
	movnti dword [rdx],eax
	movzx rax,byte [rcx]
	and rax,15
	mov eax,dword [rsi+rax*4]
	lea rcx,[rcx+1]
	movnti dword [rdx+4],eax
	lea rdx,[rdx+8]
	dec r10
	jnz cp4_bgra8lop5

	align 16
cp4_bgra8lop7:
	movzx rax,byte [rcx]
	shr rax,4
	mov eax,dword [rsi+rax*4]
	movnti dword [rdx],eax
	lea rdx,[rdx+4]
	
	add rcx,qword [rsp+48] ;sbpl
	add rdx,qword [rsp+56] ;dbpl
	dec r9
	jnz cp4_bgra8lop4

	align 16
cp4_bgra8exit:
	pop rsi
	ret

;void ImageUtil_ConvP8_B8G8R8A8(UInt8 *srcPtr, UInt8 *destPtr, OSInt w, OSInt h, OSInt sbpl, OSInt dbpl, UInt8 *pal);
;0 rsi
;8 retAddr
;rcx srcPtr
;rdx destPtr
;r8 w
;r9 h
;48 sbpl
;56 dbpl
;64 pal

	align 16
ImageUtil_ConvP8_B8G8R8A8:
	push rsi
	mov rsi,qword [rsp+64] ;pal
	
	lea rax,[r8*4]
	sub qword [rsp+48],r8 ;sbpl
	sub qword [rsp+56],rax ;dbpl
	align 16
cp8_bgra8lop:
	mov r10,r8
	align 16
cp8_bgra8lop2:
	movzx rax,byte [rcx]
	mov eax,dword [rsi+rax*4]
	lea rcx,[rcx+1]
	movnti dword [rdx],eax
	lea rdx,[rdx+4]
	dec r10
	jnz cp8_bgra8lop2
	
	add rcx,qword [rsp+48] ;sbpl
	add rdx,qword [rsp+56] ;dbpl
	dec r9
	jnz cp8_bgra8lop
	
	pop rsi
	ret

;void ImageUtil_ConvP1_A1_B8G8R8A8(UInt8 *srcPtr, UInt8 *destPtr, OSInt w, OSInt h, OSInt sbpl, OSInt dbpl, UInt8 *pal);
;0 rsi
;8 retAddr
;rcx srcPtr
;rdx destPtr
;r8 w
;r9 h
;48 sbpl
;56 dbpl
;64 pal rsi
	align 16
ImageUtil_ConvP1_A1_B8G8R8A8:
	push rsi
	mov rsi,qword [rsp+64] ;pal
	mov r11,rdx
	
	align 16
cp1_a1_bgra8lop2:
	mov r10,r8
	mov rdx,r11
	shr r10,3
	align 16
cp1_a1_bgra8lop3:
	movzx rax,byte [rcx]
	shr rax,7
	mov eax,dword [rsi+rax*4]
	mov dword [rdx],eax
	movzx rax,byte [rcx]
	shr rax,6
	and rax,1
	mov eax,dword [rsi+rax*4]
	mov dword [rdx+4],eax
	movzx rax,byte [rcx]
	shr rax,5
	and rax,1
	mov eax,dword [rsi+rax*4]
	mov dword [rdx+8],eax
	movzx rax,byte [rcx]
	shr rax,4
	and rax,1
	mov eax,dword [rsi+rax*4]
	mov dword [rdx+12],eax
	movzx rax,byte [rcx]
	shr rax,3
	and rax,1
	mov eax,dword [rsi+rax*4]
	mov dword [rdx+16],eax
	movzx rax,byte [rcx]
	shr rax,2
	and rax,1
	mov eax,dword [rsi+rax*4]
	mov dword [rdx+20],eax
	movzx rax,byte [rcx]
	shr rax,1
	and rax,1
	mov eax,dword [rsi+rax*4]
	mov dword [rdx+24],eax
	movzx rax,byte [rcx]
	and rax,1
	mov eax,dword [rsi+rax*4]
	lea rcx,[rcx+1]
	mov dword [rdx+28],eax
	lea rdx,[rdx+32]
	dec r10
	jnz cp1_a1_bgra8lop3
	
	mov r10,r8
	and r10,7
	jz cp1_a1_bgra8lop3b
	movq xmm0,rbx
	mov bl,byte [rcx]
	lea rcx,[rcx+1]
	align 16
cp1_a1_bgra8lop3a:
	rol bl,1
	movzx rax,bl
	and rax,1
	mov eax,dword [rsi+rax*4]
	mov dword [rdx],eax

	lea rdx,[rdx+4]
	dec r10
	jnz cp1_a1_bgra8lop3a
	movq rbx,xmm0
	
	align 16
cp1_a1_bgra8lop3b:

	mov r10,r8
	mov rdx,r11
	shr r10,3
	align 16
cp1_a1_bgra8lop4:
	movzx rax,byte [rcx]
	shr rax,7
	neg al
	mov byte [rdx+3],al

	movzx rax,byte [rcx]
	shr rax,6
	and rax,1
	neg al
	mov byte [rdx+7],al

	movzx rax,byte [rcx]
	shr rax,5
	and rax,1
	neg al
	mov byte [rdx+11],al

	movzx rax,byte [rcx]
	shr rax,4
	and rax,1
	neg al
	mov byte [rdx+15],al

	movzx rax,byte [rcx]
	shr rax,3
	and rax,1
	neg al
	mov byte [rdx+19],al

	movzx rax,byte [rcx]
	shr rax,2
	and rax,1
	neg al
	mov byte [rdx+23],al

	movzx rax,byte [rcx]
	shr rax,1
	and rax,1
	neg al
	mov byte [rdx+27],al

	movzx rax,byte [rcx]
	and rax,1
	neg al
	mov byte [rdx+31],al

	lea rcx,[rcx+1]
	lea rdx,[rdx+32]
	dec r10
	jnz cp1_a1_bgra8lop4
	
	mov r10,r8
	and r10,7
	jz cp1_a1_bgra8lop4b
	mov ah,byte [rcx]
	lea rcx,[rcx+1]
	align 16
cp1_a1_bgra8lop4a:
	rol ah,1
	mov al,ah
	neg al
	mov byte [rdx+3],al

	lea rdx,[rdx+4]
	dec r10
	jnz cp1_a1_bgra8lop4a
	
	align 16
cp1_a1_bgra8lop4b:
	
	add r11,qword [rsp+56] ;dbpl
	dec r9
	jnz cp1_a1_bgra8lop2

	pop rsi
	ret

;void ImageUtil_ConvP2_A1_B8G8R8A8(UInt8 *srcPtr, UInt8 *destPtr, OSInt w, OSInt h, OSInt sbpl, OSInt dbpl, UInt8 *pal);
;0 rsi
;8 retAddr
;rcx srcPtr
;rdx destPtr
;r8 w
;r9 h
;48 sbpl
;56 dbpl
;64 pal rsi
	align 16
ImageUtil_ConvP2_A1_B8G8R8A8:
	push rsi
	mov rsi,qword [rsp+64] ;pal
	mov r11,rdx

	align 16
cp2_a1_bgra8lop2:
	mov r10,r8
	mov rdx,r11
	shr r10,2
	align 16
cp2_a1_bgra8lop3:
	movzx rax,byte [rcx]
	shr rax,6
	and rax,3
	mov eax,dword [rsi+rax*4]
	movnti dword [rdx],eax
	movzx rax,byte [rcx]
	shr rax,4
	and rax,3
	mov eax,dword [rsi+rax*4]
	movnti dword [rdx+4],eax
	movzx rax,byte [rcx]
	shr rax,2
	and rax,3
	mov eax,dword [rsi+rax*4]
	movnti dword [rdx+8],eax
	movzx rax,byte [rcx]
	and rax,3
	mov eax,dword [rsi+rax*4]
	movnti dword [rdx+12],eax
	lea rcx,[rcx+1]
	lea rdx,[rdx+16]
	dec r10
	jnz cp2_a1_bgra8lop3

	mov r10,r8
	and r10,3
	jz cp2_a1_bgra8lop3b
	movq xmm0,rbx
	mov bl,byte [rcx]
	lea rcx,[rcx+1]
	align 16
cp2_a1_bgra8lop3a:
	rol bl,2
	movzx rax,bl
	and rax,3
	mov eax,dword [rsi+rax*4]
	mov dword [rdx],eax

	lea rdx,[rdx+4]
	dec r10
	jnz cp2_a1_bgra8lop3a
	movq rbx,xmm0
	
	align 16
cp2_a1_bgra8lop3b:
	
	mov r10,r8
	mov rdx,r11
	shr r10,3
	align 16
cp2_a1_bgra8lop4:
	movzx rax,byte [rcx]
	shr rax,7
	neg al
	mov byte [rdx+3],al

	movzx rax,byte [rcx]
	shr rax,6
	and rax,1
	neg al
	mov byte [rdx+7],al

	movzx rax,byte [rcx]
	shr rax,5
	and rax,1
	neg al
	mov byte [rdx+11],al

	movzx rax,byte [rcx]
	shr rax,4
	and rax,1
	neg al
	mov byte [rdx+15],al

	movzx rax,byte [rcx]
	shr rax,3
	and rax,1
	neg al
	mov byte [rdx+19],al

	movzx rax,byte [rcx]
	shr rax,2
	and rax,1
	neg al
	mov byte [rdx+23],al

	movzx rax,byte [rcx]
	shr rax,1
	and rax,1
	neg al
	mov byte [rdx+27],al

	movzx rax,byte [rcx]
	and rax,1
	neg al
	mov byte [rdx+31],al

	lea rcx,[rcx+1]
	lea rdx,[rdx+32]
	dec r10
	jnz cp2_a1_bgra8lop4
	
	mov r10,r8
	and r10,7
	jz cp2_a1_bgra8lop4b
	mov ah,byte [rcx]
	lea rcx,[rcx+1]
	align 16
cp2_a1_bgra8lop4a:
	rol ah,1
	mov al,ah
	neg al
	mov byte [rdx+3],al

	lea rdx,[rdx+4]
	dec r10
	jnz cp2_a1_bgra8lop4a
	
	align 16
cp2_a1_bgra8lop4b:

	add r11,qword [rsp+56] ;dbpl
	dec r9
	jnz cp2_a1_bgra8lop2

	pop rsi
	ret

;void ImageUtil_ConvP4_A1_B8G8R8A8(UInt8 *srcPtr, UInt8 *destPtr, OSInt w, OSInt h, OSInt sbpl, OSInt dbpl, UInt8 *pal);
;0 rsi
;8 retAddr
;rcx srcPtr
;rdx destPtr
;r8 w
;r9 h
;48 sbpl
;56 dbpl
;64 pal rsi
	align 16
ImageUtil_ConvP4_A1_B8G8R8A8:
	push rsi
	mov rsi,qword [rsp+64] ;pal
	mov r11,rdx
	
	align 16
cp4_a1_bgra8lop2:
	mov r10,r8
	mov rdx,r11
	shr r10,1
	align 16
cp4_a1_bgra8lop3:
	movzx rax,byte [rcx]
	shr rax,4
	mov eax,dword [rsi+rax*4]
	movnti dword [rdx],eax
	movzx rax,byte [rcx]
	and rax,15
	mov eax,dword [rsi+rax*4]
	lea rcx,[rcx+1]
	movnti dword [rdx+4],eax
	lea rdx,[rdx+8]
	dec r10
	jnz cp4_a1_bgra8lop3

	test r8,1
	jz cp4_a1_bgra8lop3b
	movzx rax,byte [rcx]
	lea rcx,[rcx+1]
	shr al,4
	mov eax,dword [rsi+rax*4]
	mov dword [rdx],eax
	lea rdx,[rdx+4]
	
	align 16
cp4_a1_bgra8lop3b:
	
	mov r10,r8
	mov rdx,r11
	shr r10,3
	align 16
cp4_a1_bgra8lop4:
	movzx rax,byte [rcx]
	shr rax,7
	neg al
	mov byte [rdx+3],al

	movzx rax,byte [rcx]
	shr rax,6
	and rax,1
	neg al
	mov byte [rdx+7],al

	movzx rax,byte [rcx]
	shr rax,5
	and rax,1
	neg al
	mov byte [rdx+11],al

	movzx rax,byte [rcx]
	shr rax,4
	and rax,1
	neg al
	mov byte [rdx+15],al

	movzx rax,byte [rcx]
	shr rax,3
	and rax,1
	neg al
	mov byte [rdx+19],al

	movzx rax,byte [rcx]
	shr rax,2
	and rax,1
	neg al
	mov byte [rdx+23],al

	movzx rax,byte [rcx]
	shr rax,1
	and rax,1
	neg al
	mov byte [rdx+27],al

	movzx rax,byte [rcx]
	and rax,1
	neg al
	mov byte [rdx+31],al

	lea rcx,[rcx+1]
	lea rdx,[rdx+32]
	dec r10
	jnz cp4_a1_bgra8lop4
	
	mov r10,r8
	and r10,7
	jz cp4_a1_bgra8lop4b
	mov ah,byte [rcx]
	lea rcx,[rcx+1]
	align 16
cp4_a1_bgra8lop4a:
	rol ah,1
	mov al,ah
	neg al
	mov byte [rdx+3],al

	lea rdx,[rdx+4]
	dec r10
	jnz cp4_a1_bgra8lop4a
	
	align 16
cp4_a1_bgra8lop4b:

	add r11,qword [rsp+56] ;dbpl
	dec r9
	jnz cp4_a1_bgra8lop2

	pop rsi
	ret

;void ImageUtil_ConvP8_A1_B8G8R8A8(UInt8 *srcPtr, UInt8 *destPtr, OSInt w, OSInt h, OSInt sbpl, OSInt dbpl, UInt8 *pal);
;0 rsi
;8 retAddr
;rcx srcPtr
;rdx destPtr
;r8 w
;r9 h
;48 sbpl
;56 dbpl
;64 pal

	align 16
ImageUtil_ConvP8_A1_B8G8R8A8:
	push rsi
	mov rsi,qword [rsp+64] ;pal
	mov r11,rdx
	
	align 16
cp8_a1_bgra8lop:
	mov r10,r8
	mov rdx,r11
	align 16
cp8_a1_bgra8lop2:
	movzx rax,byte [rcx]
	mov eax,dword [rsi+rax*4]
	lea rcx,[rcx+1]
	movnti dword [rdx],eax
	lea rdx,[rdx+4]
	dec r10
	jnz cp8_a1_bgra8lop2
	
	mov r10,r8
	mov rdx,r11
	shr r10,3
	align 16
cp8_a1_bgra8lop3:
	movzx rax,byte [rcx]
	shr rax,7
	neg al
	mov byte [rdx+3],al

	movzx rax,byte [rcx]
	shr rax,6
	and rax,1
	neg al
	mov byte [rdx+7],al

	movzx rax,byte [rcx]
	shr rax,5
	and rax,1
	neg al
	mov byte [rdx+11],al

	movzx rax,byte [rcx]
	shr rax,4
	and rax,1
	neg al
	mov byte [rdx+15],al

	movzx rax,byte [rcx]
	shr rax,3
	and rax,1
	neg al
	mov byte [rdx+19],al

	movzx rax,byte [rcx]
	shr rax,2
	and rax,1
	neg al
	mov byte [rdx+23],al

	movzx rax,byte [rcx]
	shr rax,1
	and rax,1
	neg al
	mov byte [rdx+27],al

	movzx rax,byte [rcx]
	and rax,1
	neg al
	mov byte [rdx+31],al

	lea rcx,[rcx+1]
	lea rdx,[rdx+32]
	dec r10
	jnz cp8_a1_bgra8lop3
	
	mov r10,r8
	and r10,7
	jz cp8_a1_bgra8lop4b
	mov ah,byte [rcx]
	lea rcx,[rcx+1]
	align 16
cp8_a1_bgra8lop4a:
	rol ah,1
	mov al,ah
	neg al
	mov byte [rdx+3],al

	lea rdx,[rdx+4]
	dec r10
	jnz cp8_a1_bgra8lop4a
	
	align 16
cp8_a1_bgra8lop4b:

	add r11,qword [rsp+56] ;dbpl
	dec r9
	jnz cp8_a1_bgra8lop
	
	pop rsi
	ret

;void ImageUtil_ConvB5G5R5_B8G8R8A8(UInt8 *srcPtr, UInt8 *destPtr, OSInt w, OSInt h, OSInt sbpl, OSInt dbpl);
;0 retAddr
;rcx srcPtr
;rdx destPtr
;r8 w
;r9 h
;40 sbpl
;48 dbpl

	align 16
ImageUtil_ConvB5G5R5_B8G8R8A8:
	lea rax,[r8*2]
	sub qword [rsp+40],rax ;sbpl
	lea rax,[r8*4]
	sub qword [rsp+48],rax ;dbpl
	mov eax,0x00840084
	movd xmm2,eax
	pxor xmm1,xmm1
	punpckldq xmm2,xmm2

	align 16
cb5g5r5_bgra8lop:
	mov r10,r8
	
	align 16
cb5g5r5_bgra8lop2:
	movzx eax,word [ecx]
	and eax,0x7fff
	shl eax,6
	shr ax,3
	shr al,3
	movd xmm0, eax
	punpcklbw xmm0,xmm1
	pmullw xmm0, xmm2
	psrlw xmm0,4
	packuswb xmm0,xmm1
	movd dword [rdx],xmm0
	mov byte [rdx+3],0xff

	lea rcx,[rcx+2]
	lea rdx,[rdx+4]
	dec r10
	jnz cb5g5r5_bgra8lop2
	
	add rcx,qword [rsp+40] ;sbpl
	add rdx,qword [rsp+48] ;dbpl
	dec r9
	jnz cb5g5r5_bgra8lop
	ret

;void ImageUtil_ConvB5G6R5_B8G8R8A8(UInt8 *srcPtr, UInt8 *destPtr, OSInt w, OSInt h, OSInt sbpl, OSInt dbpl);
;0 retAddr
;rcx srcPtr
;rdx destPtr
;r8 w
;r9 h
;40 sbpl
;48 dbpl

	align 16
ImageUtil_ConvB5G6R5_B8G8R8A8:
	lea rax,[r8*2]
	sub qword [rsp+40],rax ;sbpl
	lea rax,[r8*4]
	sub qword [rsp+48],rax ;dbpl
	mov eax,0x00410084
	movd xmm2,eax
	pxor xmm1,xmm1
	punpckldq xmm2,xmm2

	align 16
cb5g6r5_bgra8lop:
	mov r10,r8
	
	align 16
cb5g6r5_bgra8lop2:
	movzx eax,word [ecx]
	shl eax,5
	shr ax,2
	shr al,3
	movd xmm0, eax
	punpcklbw xmm0,xmm1
	pmullw xmm0, xmm2
	psrlw xmm0,4
	packuswb xmm0,xmm1
	movd dword [rdx],xmm0
	mov byte [rdx+3],0xff

	lea rcx,[rcx+2]
	lea rdx,[rdx+4]
	dec r10
	jnz cb5g6r5_bgra8lop2
	
	add rcx,qword [rsp+40] ;sbpl
	add rdx,qword [rsp+48] ;dbpl
	dec r9
	jnz cb5g6r5_bgra8lop
	ret

;void ImageUtil_ConvB8G8R8_B8G8R8A8(UInt8 *srcPtr, UInt8 *destPtr, OSInt w, OSInt h, OSInt sbpl, OSInt dbpl);
;0 retAddr
;rcx srcPtr
;rdx destPtr
;r8 w
;r9 h
;40 sbpl
;48 dbpl
	align 16
ImageUtil_ConvB8G8R8_B8G8R8A8:
	lea rax,[r8+r8*2]
	sub qword [rsp+40],rax ;sbpl
	lea rax,[r8*4]
	sub qword [rsp+48],rax ;dbpl
	
	align 16
cbgr8_bgra8lop:
	mov r10,r8
	align 16
cbgr8_bgra8lop2:
	movzx eax,byte [rcx+2]
	or eax,0xff00
	shl eax,16
	mov ax,word [rcx]
	lea rcx,[rcx+3]
	movnti dword [rdx],eax
	lea rdx,[rdx+4]
	dec r10
	jnz cbgr8_bgra8lop2
	
	add rcx,qword [rsp+40] ;sbpl
	add rdx,qword [rsp+48] ;dbpl
	dec r9
	jnz cbgr8_bgra8lop
	ret
	
;void ImageUtil_ConvR8G8B8_B8G8R8A8(UInt8 *srcPtr, UInt8 *destPtr, OSInt w, OSInt h, OSInt sbpl, OSInt dbpl);
;0 retAddr
;rcx srcPtr
;rdx destPtr
;r8 w
;r9 h
;40 sbpl
;48 dbpl
	align 16
ImageUtil_ConvR8G8B8_B8G8R8A8:
	lea rax,[r8+r8*2]
	sub qword [rsp+40],rax ;sbpl
	lea rax,[r8*4]
	sub qword [rsp+48],rax ;dbpl
	
	align 16
crgb8_bgra8lop:
	mov r10,r8
	align 16
crgb8_bgra8lop2:
	movzx eax,byte [rcx]
	or eax,0xff00
	shl eax,16
	mov ah,byte [rcx+1]
	mov al,byte [rcx+2]
	lea rcx,[rcx+3]
	movnti dword [rdx],eax
	lea rdx,[rdx+4]
	dec r10
	jnz crgb8_bgra8lop2
	
	add rcx,qword [rsp+40] ;sbpl
	add rdx,qword [rsp+48] ;dbpl
	dec r9
	jnz crgb8_bgra8lop
	ret
	
;void ImageUtil_ConvB8G8R8A8_B8G8R8(UInt8 *srcPtr, UInt8 *destPtr, OSInt w, OSInt h, OSInt sbpl, OSInt dbpl);
;0 retAddr
;rcx srcPtr
;rdx destPtr
;r8 w
;r9 h
;40 sbpl
;48 dbpl
	align 16
ImageUtil_ConvB8G8R8A8_B8G8R8:
	lea rax,[r8+r8*2]
	sub qword [rsp+48],rax ;dbpl
	lea rax,[r8*4]
	sub qword [rsp+40],rax ;sbpl
	
	align 16
cbgra8_bgr8lop:
	mov r10,r8
	align 16
cbgra8_bgr8lop2:
	mov ax,word [rcx]
	mov word [rdx],ax
	mov al,byte [rcx+2]
	mov byte [rdx+2],al
	lea rcx,[rcx+4]
	lea rdx,[rdx+3]
	dec r10
	jnz cbgra8_bgr8lop2
	
	add rcx,qword [rsp+40] ;sbpl
	add rdx,qword [rsp+48] ;dbpl
	dec r9
	jnz cbgra8_bgr8lop
	ret
	
;void ImageUtil_ConvR8G8B8A8_B8G8R8A8(UInt8 *srcPtr, UInt8 *destPtr, OSInt w, OSInt h, OSInt sbpl, OSInt dbpl);
;0 retAddr
;rcx srcPtr
;rdx destPtr
;r8 w
;r9 h
;40 sbpl
;48 dbpl
	align 16
ImageUtil_ConvR8G8B8A8_B8G8R8A8:
	lea rax,[r8*4]
	sub qword [rsp+40],rax ;sbpl
	sub qword [rsp+48],rax ;dbpl
	
	align 16
crgba8_bgra8lop:
	mov r10,r8
	align 16
crgba8_bgra8lop2:
	movzx eax,byte [rcx]
	mov ah,byte [rcx+3]
	shl eax,16
	mov ah,byte [rcx+1]
	mov al,byte [rcx+2]
	lea rcx,[rcx+4]
	movnti dword [rdx],eax
	lea rdx,[rdx+4]
	dec r10
	jnz crgba8_bgra8lop2
	
	add rcx,qword [rsp+40] ;sbpl
	add rdx,qword [rsp+48] ;dbpl
	dec r9
	jnz crgba8_bgra8lop
	ret
	
;void ImageUtil_ConvR8G8B8N8_B8G8R8A8(UInt8 *srcPtr, UInt8 *destPtr, OSInt w, OSInt h, OSInt sbpl, OSInt dbpl);
;0 retAddr
;rcx srcPtr
;rdx destPtr
;r8 w
;r9 h
;40 sbpl
;48 dbpl
	align 16
ImageUtil_ConvR8G8B8N8_B8G8R8A8:
	lea rax,[r8*4]
	sub qword [rsp+40],rax ;sbpl
	sub qword [rsp+48],rax ;dbpl
	
	align 16
crgbn8_bgra8lop:
	mov r10,r8
	align 16
crgbn8_bgra8lop2:
	movzx eax,byte [rcx]
	mov ah,0xff
	shl eax,16
	mov ah,byte [rcx+1]
	mov al,byte [rcx+2]
	lea rcx,[rcx+4]
	movnti dword [rdx],eax
	lea rdx,[rdx+4]
	dec r10
	jnz crgbn8_bgra8lop2
	
	add rcx,qword [rsp+40] ;sbpl
	add rdx,qword [rsp+48] ;dbpl
	dec r9
	jnz crgbn8_bgra8lop
	ret

;void ImageUtil_ConvB16G16R16_B8G8R8A8(UInt8 *srcPtr, UInt8 *destPtr, OSInt w, OSInt h, OSInt sbpl, OSInt dbpl);
;0 retAddr
;rcx srcPtr
;rdx destPtr
;r8 w
;r9 h
;40 sbpl
;48 dbpl
	align 16
ImageUtil_ConvB16G16R16_B8G8R8A8:
	lea rax,[r8+r8*2]
	shl rax,1
	sub qword [rsp+40],rax ;sbpl
	lea rax,[r8*4]
	sub qword [rsp+48],rax ;dbpl
	
	align 16
cbgr16_bgra8lop:
	mov r10,r8
	align 16
cbgr16_bgra8lop2:
	movzx eax,byte [rcx+5]
	or eax,0xff00
	shl eax,16
	mov ah,byte [rcx+3]
	mov al,byte [rcx+1]
	lea rcx,[rcx+6]
	movnti dword [rdx],eax
	lea rdx,[rdx+4]
	dec r10
	jnz cbgr16_bgra8lop2
	
	add rcx,qword [rsp+40] ;sbpl
	add rdx,qword [rsp+48] ;dbpl
	dec r9
	jnz cbgr16_bgra8lop
	ret

;void ImageUtil_ConvR16G16B16_B8G8R8A8(UInt8 *srcPtr, UInt8 *destPtr, OSInt w, OSInt h, OSInt sbpl, OSInt dbpl);
;0 retAddr
;rcx srcPtr
;rdx destPtr
;r8 w
;r9 h
;40 sbpl
;48 dbpl
	align 16
ImageUtil_ConvR16G16B16_B8G8R8A8:
	lea rax,[r8+r8*2]
	shl rax,1
	sub qword [rsp+40],rax ;sbpl
	lea rax,[r8*4]
	sub qword [rsp+48],rax ;dbpl
	
	align 16
crgb16_bgra8lop:
	mov r10,r8
	align 16
crgb16_bgra8lop2:
	movzx eax,byte [rcx+1]
	or eax,0xff00
	shl eax,16
	mov ah,byte [rcx+3]
	mov al,byte [rcx+5]
	lea rcx,[rcx+6]
	movnti dword [rdx],eax
	lea rdx,[rdx+4]
	dec r10
	jnz crgb16_bgra8lop2
	
	add rcx,qword [rsp+40] ;sbpl
	add rdx,qword [rsp+48] ;dbpl
	dec r9
	jnz crgb16_bgra8lop
	ret

;void ImageUtil_ConvB16G16R16A16_B8G8R8A8(UInt8 *srcPtr, UInt8 *destPtr, OSInt w, OSInt h, OSInt sbpl, OSInt dbpl);
;0 retAddr
;rcx srcPtr
;rdx destPtr
;r8 w
;r9 h
;40 sbpl
;48 dbpl
	align 16
ImageUtil_ConvB16G16R16A16_B8G8R8A8:
	lea rax,[r8*8]
	sub qword [rsp+40],rax ;sbpl
	lea rax,[r8*4]
	sub qword [rsp+48],rax ;dbpl
	
	align 16
cbgra16_bgra8lop:
	mov r10,r8
	align 16
cbgra16_bgra8lop2:
	movq xmm0,[rcx]
	psrlw xmm0,8
	lea rcx,[rcx+8]
	packuswb xmm0,xmm0
	movd dword [rdx],xmm0
	lea rdx,[rdx+4]
	dec r10
	jnz cbgra16_bgra8lop2
	
	add rcx,qword [rsp+40] ;sbpl
	add rdx,qword [rsp+48] ;dbpl
	dec r9
	jnz cbgra16_bgra8lop
	ret

;void ImageUtil_ConvR16G16B16A16_B8G8R8A8(UInt8 *srcPtr, UInt8 *destPtr, OSInt w, OSInt h, OSInt sbpl, OSInt dbpl);
;0 retAddr
;rcx srcPtr
;rdx destPtr
;r8 w
;r9 h
;40 sbpl
;48 dbpl
	align 16
ImageUtil_ConvR16G16B16A16_B8G8R8A8:
	lea rax,[r8*8]
	sub qword [rsp+40],rax ;sbpl
	lea rax,[r8*4]
	sub qword [rsp+48],rax ;dbpl
	
	align 16
crgba16_bgra8lop:
	mov r10,r8
	align 16
crgba16_bgra8lop2:
	movq xmm0,[rcx]
	psrlw xmm0,8
	lea rcx,[rcx+8]
	packuswb xmm0,xmm0
	movd dword [rdx],xmm0
	lea rdx,[rdx+4]
	dec r10
	jnz crgba16_bgra8lop2
	
	add rcx,qword [rsp+40] ;sbpl
	add rdx,qword [rsp+48] ;dbpl
	dec r9
	jnz crgba16_bgra8lop
	ret

;void ImageUtil_ConvA2B10G10R10_B8G8R8A8(UInt8 *srcPtr, UInt8 *destPtr, OSInt w, OSInt h, OSInt sbpl, OSInt dbpl);
;0 retAddr
;rcx srcPtr
;rdx destPtr
;r8 w
;r9 h
;40 sbpl
;48 dbpl
	align 16
ImageUtil_ConvA2B10G10R10_B8G8R8A8:
	lea rax,[r8*4]
	sub qword [rsp+40],rax ;sbpl
	lea rax,[r8*4]
	sub qword [rsp+48],rax ;dbpl
	
	align 16
ca2b10g10r10_bgra8lop:
	mov r10,r8
	align 16
ca2b10g10r10_bgra8lop2:
	mov eax,dword [rcx]
	shr eax,2
	mov byte [rdx+2],al
	shr eax,10
	mov byte [rdx+1],al
	shr eax,10
	mov byte [rdx+0],al
	mov al,ah
	shl al,2
	or al,ah
	mov ah,al
	shl al,4
	or al,ah
	mov byte [rdx+3],al
	lea rcx,[rcx+4]
	lea rdx,[rdx+4]
	dec r10
	jnz ca2b10g10r10_bgra8lop2
	
	add rcx,qword [rsp+40] ;sbpl
	add rdx,qword [rsp+48] ;dbpl
	dec r9
	jnz ca2b10g10r10_bgra8lop
	ret

;void ImageUtil_ConvFB32G32R32A32_B8G8R8A8(UInt8 *srcPtr, UInt8 *destPtr, OSInt w, OSInt h, OSInt sbpl, OSInt dbpl);
;0 retAddr
;rcx srcPtr
;rdx destPtr
;r8 w
;r9 h
;40 sbpl
;48 dbpl
	align 16
ImageUtil_ConvFB32G32R32A32_B8G8R8A8:
	lea rax,[r8*4]
	sub qword [rsp+48],rax ;dbpl
	lea rax,[rax*4]
	sub qword [rsp+40],rax ;sbpl
	mov rax,255
	cvtsi2ss xmm1,rax
	unpcklps xmm1,xmm1
	unpcklps xmm1,xmm1
	
	align 16
cfbgra32_bgra8lop:
	mov r10,r8
	align 16
cfbgra32_bgra8lop2:
	movups xmm0,[rcx]
	mulps xmm0,xmm1
	cvtps2dq xmm0,xmm0
	packssdw xmm0,xmm0
	packuswb xmm0,xmm0
	movd [rdx],xmm0
	lea rcx,[rcx+16]
	lea rdx,[rdx+4]
	dec r10
	jnz cfbgra32_bgra8lop2
	
	add rcx,qword [rsp+40] ;sbpl
	add rdx,qword [rsp+48] ;dbpl
	dec r9
	jnz cfbgra32_bgra8lop
	ret

;void ImageUtil_ConvFR32G32B32A32_B8G8R8A8(UInt8 *srcPtr, UInt8 *destPtr, OSInt w, OSInt h, OSInt sbpl, OSInt dbpl);
;0 retAddr
;rcx srcPtr
;rdx destPtr
;r8 w
;r9 h
;40 sbpl
;48 dbpl
	align 16
ImageUtil_ConvFR32G32B32A32_B8G8R8A8:
	lea rax,[r8*4]
	sub qword [rsp+48],rax ;dbpl
	lea rax,[rax*4]
	sub qword [rsp+40],rax ;sbpl
	mov rax,255
	cvtsi2ss xmm1,rax
	unpcklps xmm1,xmm1
	unpcklps xmm1,xmm1
	
	align 16
cfrgba32_bgra8lop:
	mov r10,r8
	align 16
cfrgba32_bgra8lop2:
	movups xmm0,[rcx]
	pshufd xmm0,xmm0,0xc6
	mulps xmm0,xmm1
	cvtps2dq xmm0,xmm0
	packssdw xmm0,xmm0
	packuswb xmm0,xmm0
	movd [rdx],xmm0
	lea rcx,[rcx+16]
	lea rdx,[rdx+4]
	dec r10
	jnz cfrgba32_bgra8lop2
	
	add rcx,qword [rsp+40] ;sbpl
	add rdx,qword [rsp+48] ;dbpl
	dec r9
	jnz cfrgba32_bgra8lop
	ret

;void ImageUtil_ConvFB32G32R32_B8G8R8A8(UInt8 *srcPtr, UInt8 *destPtr, OSInt w, OSInt h, OSInt sbpl, OSInt dbpl);
;0 retAddr
;rcx srcPtr
;rdx destPtr
;r8 w
;r9 h
;40 sbpl
;48 dbpl
	align 16
ImageUtil_ConvFB32G32R32_B8G8R8A8:
	lea rax,[r8*4]
	sub qword [rsp+48],rax ;dbpl
	lea rax,[rax*2+rax]
	sub qword [rsp+40],rax ;sbpl
	mov rax,255
	cvtsi2ss xmm1,rax
	unpcklps xmm1,xmm1
	unpcklps xmm1,xmm1
	
	align 16
cfbgr32_bgra8lop:
	mov r10,r8
	align 16
cfbgr32_bgra8lop2:
	movq xmm0,[rcx]
	movss xmm2,[rcx+8]
	punpcklqdq xmm0,xmm2
	mulps xmm0,xmm1
	cvtps2dq xmm0,xmm0
	packssdw xmm0,xmm0
	packuswb xmm0,xmm0
	movd [rdx],xmm0
	mov byte [rdx+3],0xff
	lea rcx,[rcx+12]
	lea rdx,[rdx+4]
	dec r10
	jnz cfbgr32_bgra8lop2
	
	add rcx,qword [rsp+40] ;sbpl
	add rdx,qword [rsp+48] ;dbpl
	dec r9
	jnz cfbgr32_bgra8lop
	ret

;void ImageUtil_ConvFR32G32B32_B8G8R8A8(UInt8 *srcPtr, UInt8 *destPtr, OSInt w, OSInt h, OSInt sbpl, OSInt dbpl);
;0 retAddr
;rcx srcPtr
;rdx destPtr
;r8 w
;r9 h
;40 sbpl
;48 dbpl
	align 16
ImageUtil_ConvFR32G32B32_B8G8R8A8:
	lea rax,[r8*4]
	sub qword [rsp+48],rax ;dbpl
	lea rax,[rax*2+rax]
	sub qword [rsp+40],rax ;sbpl
	mov rax,255
	cvtsi2ss xmm1,rax
	unpcklps xmm1,xmm1
	unpcklps xmm1,xmm1
	
	align 16
cfrgb32_bgra8lop:
	mov r10,r8
	align 16
cfrgb32_bgra8lop2:
	movq xmm0,[rcx]
	movss xmm2,[rcx+8]
	punpcklqdq xmm0,xmm2
	pshufd xmm0,xmm0,0xc6
	mulps xmm0,xmm1
	cvtps2dq xmm0,xmm0
	packssdw xmm0,xmm0
	packuswb xmm0,xmm0
	movd [rdx],xmm0
	mov byte [rdx+3],0xff
	lea rcx,[rcx+12]
	lea rdx,[rdx+4]
	dec r10
	jnz cfrgb32_bgra8lop2
	
	add rcx,qword [rsp+40] ;sbpl
	add rdx,qword [rsp+48] ;dbpl
	dec r9
	jnz cfrgb32_bgra8lop
	ret

;void ImageUtil_ConvFW32A32_B8G8R8A8(UInt8 *srcPtr, UInt8 *destPtr, OSInt w, OSInt h, OSInt sbpl, OSInt dbpl);
;0 retAddr
;rcx srcPtr
;rdx destPtr
;r8 w
;r9 h
;40 sbpl
;48 dbpl
	align 16
ImageUtil_ConvFW32A32_B8G8R8A8:
	lea rax,[r8*4]
	sub qword [rsp+48],rax ;dbpl
	lea rax,[rax*2]
	sub qword [rsp+40],rax ;sbpl
	mov rax,255
	cvtsi2ss xmm1,rax
	unpcklps xmm1,xmm1
	unpcklps xmm1,xmm1
	
	align 16
cfwa32_bgra8lop:
	mov r10,r8
	align 16
cfwa32_bgra8lop2:
	movq xmm0,[rcx]
	mulps xmm0,xmm1
	cvtps2dq xmm0,xmm0
	packssdw xmm0,xmm0
	packuswb xmm0,xmm0
	movd eax,xmm0
	mov byte [rdx],al
	mov byte [rdx+1],al
	mov word [rdx+2],ax
	lea rcx,[rcx+8]
	lea rdx,[rdx+4]
	dec r10
	jnz cfwa32_bgra8lop2
	
	add rcx,qword [rsp+40] ;sbpl
	add rdx,qword [rsp+48] ;dbpl
	dec r9
	jnz cfwa32_bgra8lop
	ret

;void ImageUtil_ConvFW32_B8G8R8A8(UInt8 *srcPtr, UInt8 *destPtr, OSInt w, OSInt h, OSInt sbpl, OSInt dbpl);
;0 retAddr
;rcx srcPtr
;rdx destPtr
;r8 w
;r9 h
;40 sbpl
;48 dbpl
	align 16
ImageUtil_ConvFW32_B8G8R8A8:
	lea rax,[r8*4]
	sub qword [rsp+48],rax ;dbpl
	sub qword [rsp+40],rax ;sbpl
	mov rax,255
	cvtsi2ss xmm1,rax
	unpcklps xmm1,xmm1
	unpcklps xmm1,xmm1
	
	align 16
cfw32_bgra8lop:
	mov r10,r8
	align 16
cfw32_bgra8lop2:
	movss xmm0,[rcx]
	mulss xmm0,xmm1
	cvtps2dq xmm0,xmm0
	packssdw xmm0,xmm0
	packuswb xmm0,xmm0
	movd eax,xmm0
	mov byte [rdx],al
	mov byte [rdx+1],al
	mov byte [rdx+2],al
	mov byte [rdx+3],0xff
	lea rcx,[rcx+4]
	lea rdx,[rdx+4]
	dec r10
	jnz cfw32_bgra8lop2
	
	add rcx,qword [rsp+40] ;sbpl
	add rdx,qword [rsp+48] ;dbpl
	dec r9
	jnz cfw32_bgra8lop
	ret

;void ImageUtil_ConvP1_B16G16R16A16(UInt8 *srcPtr, UInt8 *destPtr, OSInt w, OSInt h, OSInt sbpl, OSInt dbpl, UInt8 *pal);
;0 rsi
;8 retAddr
;rcx srcPtr
;rdx destPtr
;r8 w
;r9 h
;48 sbpl
;56 dbpl
;64 pal rsi
	align 16
ImageUtil_ConvP1_B16G16R16A16:
	push rsi
	mov rsi,qword [rsp+64] ;pal
	lea rax,[r8*8]
	test r8,7
	jnz cp1_bgra16lop
	
	shr r8,3
	sub qword [rsp+48],r8 ;sbpl
	sub qword [rsp+56],rax ;dbpl
	align 16
cp1_bgra16lop2:
	mov r10,r8
	align 16
cp1_bgra16lop3:
	movzx rax,byte [rcx]
	shr rax,7
	movd xmm0,dword [rsi+rax*4]
	punpcklbw xmm0,xmm0
	movq [rdx],xmm0
	movzx rax,byte [rcx]
	shr rax,6
	and rax,1
	movd xmm0,dword [rsi+rax*4]
	punpcklbw xmm0,xmm0
	movq [rdx+8],xmm0
	movzx rax,byte [rcx]
	shr rax,5
	and rax,1
	movd xmm0,dword [rsi+rax*4]
	punpcklbw xmm0,xmm0
	movq [rdx+16],xmm0
	movzx rax,byte [rcx]
	shr rax,4
	and rax,1
	movd xmm0,dword [rsi+rax*4]
	punpcklbw xmm0,xmm0
	movq [rdx+24],xmm0
	movzx rax,byte [rcx]
	shr rax,3
	and rax,1
	movd xmm0,dword [rsi+rax*4]
	punpcklbw xmm0,xmm0
	movq [rdx+32],xmm0
	movzx rax,byte [rcx]
	shr rax,2
	and rax,1
	movd xmm0,dword [rsi+rax*4]
	punpcklbw xmm0,xmm0
	movq [rdx+40],xmm0
	movzx rax,byte [rcx]
	shr rax,1
	and rax,1
	movd xmm0,dword [rsi+rax*4]
	punpcklbw xmm0,xmm0
	movq [rdx+48],xmm0
	movzx rax,byte [rcx]
	and rax,1
	movd xmm0,dword [rsi+rax*4]
	punpcklbw xmm0,xmm0
	lea rcx,[rcx+1]
	movq [rdx+56],xmm0
	lea rdx,[rdx+64]
	dec r10
	jnz cp1_bgra16lop3
	
	add rcx,qword [rsp+48] ;sbpl
	add rdx,qword [rsp+56] ;dbpl
	dec r9
	jnz cp1_bgra16lop2
	jmp cp1_bgra16exit
	
	align 16
cp1_bgra16lop:
	push rdi
	mov r11,r8
	shr r8,3
	sub qword [rsp+56],r8 ;sbpl
	sub qword [rsp+64],rax ;dbpl
	align 16
cp1_bgra16lop4:
	mov r10,r8
	align 16
cp1_bgra16lop5:
	movzx rax,byte [rcx]
	xor rdi,rdi
	shl al,1
	adc rdi,0
	movd xmm0,dword [rsi+rdi*4]
	punpcklbw xmm0,xmm0
	movq [rdx],xmm0
	xor rdi,rdi
	shl al,1
	adc rdi,0
	movd xmm0,dword [rsi+rdi*4]
	punpcklbw xmm0,xmm0
	movq [rdx+8],xmm0
	xor rdi,rdi
	shl al,1
	adc rdi,0
	movd xmm0,dword [rsi+rdi*4]
	punpcklbw xmm0,xmm0
	movq [rdx+16],xmm0
	xor rdi,rdi
	shl al,1
	adc rdi,0
	movd xmm0,dword [rsi+rdi*4]
	punpcklbw xmm0,xmm0
	movq [rdx+24],xmm0
	xor rdi,rdi
	shl al,1
	adc rdi,0
	movd xmm0,dword [rsi+rdi*4]
	punpcklbw xmm0,xmm0
	movq [rdx+32],xmm0
	xor rdi,rdi
	shl al,1
	adc rdi,0
	movd xmm0,dword [rsi+rdi*4]
	punpcklbw xmm0,xmm0
	movq [rdx+40],xmm0
	xor rdi,rdi
	shl al,1
	adc rdi,0
	movd xmm0,dword [rsi+rdi*4]
	punpcklbw xmm0,xmm0
	movq [rdx+48],xmm0
	xor rdi,rdi
	shl al,1
	adc rdi,0
	movd xmm0,dword [rsi+rdi*4]
	punpcklbw xmm0,xmm0
	movq [rdx+56],xmm0
	lea rcx,[rcx+1]
	lea rdx,[rdx+64]
	dec r10
	jnz cp1_bgra16lop5

	mov r10,r11
	and r10,7
	movzx rax,byte [rcx]
	align 16
cp1_bgra16lop6:
	xor rdi,rdi
	shl al,1
	adc rdi,0
	movd xmm0,dword [rsi+rdi*4]
	punpcklbw xmm0,xmm0
	movq [rdx],xmm0
	lea rdx,[rdx+8]
	dec r10
	jnz cp1_bgra16lop6
	
	add rcx,qword [rsp+56] ;sbpl
	add rdx,qword [rsp+64] ;dbpl
	dec r9
	jnz cp1_bgra16lop4
	pop rdi

	align 16
cp1_bgra16exit:
	pop rsi
	ret

;void ImageUtil_ConvP2_B16G16R16A16(UInt8 *srcPtr, UInt8 *destPtr, OSInt w, OSInt h, OSInt sbpl, OSInt dbpl, UInt8 *pal);
;0 rsi
;8 retAddr
;rcx srcPtr
;rdx destPtr
;r8 w
;r9 h
;48 sbpl
;56 dbpl
;64 pal rsi
	align 16
ImageUtil_ConvP2_B16G16R16A16:
	push rsi
	mov rsi,qword [rsp+64] ;pal
	lea rax,[r8*4]
	test r8,3
	jnz cp2_bgra16lop
	
	shr r8,2
	sub qword [rsp+48],r8 ;sbpl
	sub qword [rsp+56],rax ;dbpl
	align 16
cp2_bgra16lop2:
	mov r10,r8
	align 16
cp2_bgra16lop3:
	movzx rax,byte [rcx]
	shr rax,6
	and rax,3
	movd xmm0,dword [rsi+rax*4]
	punpcklbw xmm0,xmm0
	movq [rdx],xmm0
	movzx rax,byte [rcx]
	shr rax,4
	and rax,3
	movd xmm0,dword [rsi+rax*4]
	punpcklbw xmm0,xmm0
	movq [rdx+8],xmm0
	movzx rax,byte [rcx]
	shr rax,2
	and rax,3
	movd xmm0,dword [rsi+rax*4]
	punpcklbw xmm0,xmm0
	movq [rdx+16],xmm0
	movzx rax,byte [rcx]
	and rax,3
	movd xmm0,dword [rsi+rax*4]
	punpcklbw xmm0,xmm0
	movq [rdx+24],xmm0
	lea rcx,[rcx+1]
	lea rdx,[rdx+32]
	dec r10
	jnz cp2_bgra16lop3
	
	add rcx,qword [rsp+48] ;sbpl
	add rdx,qword [rsp+56] ;dbpl
	dec r9
	jnz cp2_bgra16lop2
	jmp cp2_bgra16exit
	
	align 16
cp2_bgra16lop:
	push rdi
	mov r11,r8
	shr r8,2
	sub qword [rsp+56],r8 ;sbpl
	sub qword [rsp+64],rax ;dbpl
	align 16
cp2_bgra16lop4:
	mov r10,r8
	align 16
cp2_bgra16lop5:
	movzx rax,byte [rcx]
	rol al,2
	mov rdi,rax
	and rdi,3
	movd xmm0,dword [rsi+rdi*4]
	punpcklbw xmm0,xmm0
	movq [rdx],xmm0
	rol al,2
	mov rdi,rax
	and rdi,3
	movd xmm0,dword [rsi+rdi*4]
	punpcklbw xmm0,xmm0
	movq [rdx+8],xmm0
	rol al,2
	mov rdi,rax
	and rdi,3
	movd xmm0,dword [rsi+rdi*4]
	punpcklbw xmm0,xmm0
	movq [rdx+16],xmm0
	rol al,2
	mov rdi,rax
	and rdi,3
	movd xmm0,dword [rsi+rdi*4]
	punpcklbw xmm0,xmm0
	movq [rdx+24],xmm0
	lea rcx,[rcx+1]
	lea rdx,[rdx+32]
	dec r10
	jnz cp2_bgra16lop5

	mov r10,r11
	and r10,3
	movzx rax,byte [rcx]
	align 16
cp2_bgra16lop6:
	rol al,2
	mov rdi,rax
	and rdi,3
	movd xmm0,dword [rsi+rdi*4]
	punpcklbw xmm0,xmm0
	movq [rdx],xmm0
	lea rdx,[rdx+8]
	dec r10
	jnz cp2_bgra16lop6
	
	add rcx,qword [rsp+56] ;sbpl
	add rdx,qword [rsp+64] ;dbpl
	dec r9
	jnz cp2_bgra16lop4
	pop rdi

	align 16
cp2_bgra16exit:
	pop rsi
	ret

;void ImageUtil_ConvP4_B16G16R16A16(UInt8 *srcPtr, UInt8 *destPtr, OSInt w, OSInt h, OSInt sbpl, OSInt dbpl, UInt8 *pal);
;0 rsi
;8 retAddr
;rcx srcPtr
;rdx destPtr
;r8 w
;r9 h
;48 sbpl
;56 dbpl
;64 pal rsi
	align 16
ImageUtil_ConvP4_B16G16R16A16:
	push rsi
	mov rsi,qword [rsp+64] ;pal
	lea rax,[r8*8]
	shr r8,1
	jb cp4_bgra16lop
	
	sub qword [rsp+48],r8 ;sbpl
	sub qword [rsp+56],rax ;dbpl
	align 16
cp4_bgra16lop2:
	mov r10,r8
	align 16
cp4_bgra16lop3:
	movzx rax,byte [rcx]
	shr rax,4
	movd xmm0,dword [rsi+rax*4]
	punpcklbw xmm0,xmm0
	movq [rdx],xmm0
	movzx rax,byte [rcx]
	and rax,15
	movd xmm0,dword [rsi+rax*4]
	lea rcx,[rcx+1]
	punpcklbw xmm0,xmm0
	movq [rdx+8],xmm0
	lea rdx,[rdx+16]
	dec r10
	jnz cp4_bgra16lop3
	
	add rcx,qword [rsp+48] ;sbpl
	add rdx,qword [rsp+56] ;dbpl
	dec r9
	jnz cp4_bgra16lop2
	jmp cp4_bgra16exit
	
	align 16
cp4_bgra16lop:
	sub qword [rsp+48],r8 ;sbpl
	sub qword [rsp+56],rax ;dbpl
	align 16
cp4_bgra16lop4:
	mov r10,r8
	align 16
cp4_bgra16lop5:
	movzx rax,byte [rcx]
	shr rax,4
	movd xmm0,dword [rsi+rax*4]
	punpcklbw xmm0,xmm0
	movq [rdx],xmm0
	movzx rax,byte [rcx]
	and rax,15
	movd xmm0,dword [rsi+rax*4]
	lea rcx,[rcx+1]
	punpcklbw xmm0,xmm0
	movq [rdx+8],xmm0
	lea rdx,[rdx+16]
	dec r10
	jnz cp4_bgra16lop5

	movzx rax,byte [rcx]
	shr rax,4
	movd xmm0,dword [rsi+rax*4]
	punpcklbw xmm0,xmm0
	movq [rdx],xmm0
	lea rdx,[rdx+8]
	
	add rcx,qword [rsp+48] ;sbpl
	add rdx,qword [rsp+56] ;dbpl
	dec r9
	jnz cp4_bgra16lop4

	align 16
cp4_bgra16exit:
	pop rsi
	ret

;void ImageUtil_ConvP8_B16G16R16A16(UInt8 *srcPtr, UInt8 *destPtr, OSInt w, OSInt h, OSInt sbpl, OSInt dbpl, UInt8 *pal);
;0 rsi
;8 retAddr
;rcx srcPtr
;rdx destPtr
;r8 w
;r9 h
;48 sbpl
;56 dbpl
;64 pal

	align 16
ImageUtil_ConvP8_B16G16R16A16:
	push rsi
	mov rsi,qword [rsp+64] ;pal
	
	lea rax,[r8*8]
	sub qword [rsp+48],r8 ;sbpl
	sub qword [rsp+56],rax ;dbpl
	align 16
cp8_bgra16lop:
	mov r10,r8
	align 16
cp8_bgra16lop2:
	movzx rax,byte [rcx]
	movd xmm0,dword [rsi+rax*4]
	lea rcx,[rcx+1]
	punpcklbw xmm0,xmm0
	movq [rdx],xmm0
	lea rdx,[rdx+4]
	dec r10
	jnz cp8_bgra16lop2
	
	add rcx,qword [rsp+48] ;sbpl
	add rdx,qword [rsp+56] ;dbpl
	dec r9
	jnz cp8_bgra16lop
	
	pop rsi
	ret

;void ImageUtil_ConvP1_A1_B16G16R16A16(UInt8 *srcPtr, UInt8 *destPtr, OSInt w, OSInt h, OSInt sbpl, OSInt dbpl, UInt8 *pal);
;0 rsi
;8 retAddr
;rcx srcPtr
;rdx destPtr
;r8 w
;r9 h
;48 sbpl
;56 dbpl
;64 pal rsi
	align 16
ImageUtil_ConvP1_A1_B16G16R16A16:
	push rsi
	mov rsi,qword [rsp+64] ;pal
	mov r11,rdx

	align 16
cp1_a1_bgra16lop2:
	mov r10,r8
	mov rdx,r11
	shr r10,3
	align 16
cp1_a1_bgra16lop3:
	movzx rax,byte [rcx]
	shr rax,7
	movd xmm0,dword [rsi+rax*4]
	punpcklbw xmm0,xmm0
	movq [rdx],xmm0
	movzx rax,byte [rcx]
	shr rax,6
	and rax,1
	movd xmm0,dword [rsi+rax*4]
	punpcklbw xmm0,xmm0
	movq [rdx+8],xmm0
	movzx rax,byte [rcx]
	shr rax,5
	and rax,1
	movd xmm0,dword [rsi+rax*4]
	punpcklbw xmm0,xmm0
	movq [rdx+16],xmm0
	movzx rax,byte [rcx]
	shr rax,4
	and rax,1
	movd xmm0,dword [rsi+rax*4]
	punpcklbw xmm0,xmm0
	movq [rdx+24],xmm0
	movzx rax,byte [rcx]
	shr rax,3
	and rax,1
	movd xmm0,dword [rsi+rax*4]
	punpcklbw xmm0,xmm0
	movq [rdx+32],xmm0
	movzx rax,byte [rcx]
	shr rax,2
	and rax,1
	movd xmm0,dword [rsi+rax*4]
	punpcklbw xmm0,xmm0
	movq [rdx+40],xmm0
	movzx rax,byte [rcx]
	shr rax,1
	and rax,1
	movd xmm0,dword [rsi+rax*4]
	punpcklbw xmm0,xmm0
	movq [rdx+48],xmm0
	movzx rax,byte [rcx]
	and rax,1
	movd xmm0,dword [rsi+rax*4]
	punpcklbw xmm0,xmm0
	lea rcx,[rcx+1]
	movq [rdx+56],xmm0
	lea rdx,[rdx+64]
	dec r10
	jnz cp1_a1_bgra16lop3
	
	mov r10,r8
	and r10,7
	jz cp1_a1_bgra16lop3b
	movq xmm1,rbx
	mov bl,byte [rcx]
	lea rcx,[rcx+1]
	align 16
cp1_a1_bgra16lop3a:
	rol bl,1
	movzx rax,bl
	and rax,1
	movd xmm0,dword [rsi+rax*4]
	punpcklbw xmm0,xmm0
	movq [rdx],xmm0

	lea rdx,[rdx+8]
	dec r10
	jnz cp1_a1_bgra16lop3a
	movq rbx,xmm1
	align 16
cp1_a1_bgra16lop3b:

	mov r10,r8
	mov rdx,r11
	shr r10,3
	align 16
cp1_a1_bgra16lop4:
	movzx rax,byte [rcx]
	shr rax,7
	neg ax
	mov word [rdx+6],ax

	movzx rax,byte [rcx]
	shr rax,6
	and rax,1
	neg ax
	mov word [rdx+14],ax

	movzx rax,byte [rcx]
	shr rax,5
	and rax,1
	neg ax
	mov word [rdx+22],ax

	movzx rax,byte [rcx]
	shr rax,4
	and rax,1
	neg ax
	mov word [rdx+30],ax

	movzx rax,byte [rcx]
	shr rax,3
	and rax,1
	neg ax
	mov word [rdx+38],ax

	movzx rax,byte [rcx]
	shr rax,2
	and rax,1
	neg ax
	mov word [rdx+46],ax

	movzx rax,byte [rcx]
	shr rax,1
	and rax,1
	neg ax
	mov word [rdx+54],ax

	movzx rax,byte [rcx]
	and rax,1
	neg ax
	mov word [rdx+62],ax

	lea rcx,[rcx+1]
	lea rdx,[rdx+64]
	dec r10
	jnz cp1_a1_bgra16lop4
	
	mov r10,r8
	and r10,7
	jz cp1_a1_bgra16lop4b
	mov ah,byte [rcx]
	lea rcx,[rcx+1]
	align 16
cp1_a1_bgra16lop4a:
	rol ah,1
	mov al,ah
	neg al
	mov byte [rdx+6],al
	mov byte [rdx+7],al

	lea rdx,[rdx+8]
	dec r10
	jnz cp1_a1_bgra16lop4a
	align 16
cp1_a1_bgra16lop4b:
	
	add r11,qword [rsp+56] ;dbpl
	dec r9
	jnz cp1_a1_bgra16lop2

	pop rsi
	ret

;void ImageUtil_ConvP2_A1_B16G16R16A16(UInt8 *srcPtr, UInt8 *destPtr, OSInt w, OSInt h, OSInt sbpl, OSInt dbpl, UInt8 *pal);
;0 rsi
;8 retAddr
;rcx srcPtr
;rdx destPtr
;r8 w
;r9 h
;48 sbpl
;56 dbpl
;64 pal rsi
	align 16
ImageUtil_ConvP2_A1_B16G16R16A16:
	push rsi
	mov rsi,qword [rsp+64] ;pal
	mov r11,rdx

	align 16
cp2_a1_bgra16lop2:
	mov r10,r8
	mov rdx,r11
	shr r10,2
	align 16
cp2_a1_bgra16lop3:
	movzx rax,byte [rcx]
	shr rax,6
	and rax,3
	movd xmm0,dword [rsi+rax*4]
	punpcklbw xmm0,xmm0
	movq [rdx],xmm0
	movzx rax,byte [rcx]
	shr rax,4
	and rax,3
	movd xmm0,dword [rsi+rax*4]
	punpcklbw xmm0,xmm0
	movq [rdx+8],xmm0
	movzx rax,byte [rcx]
	shr rax,2
	and rax,3
	movd xmm0,dword [rsi+rax*4]
	punpcklbw xmm0,xmm0
	movq [rdx+16],xmm0
	movzx rax,byte [rcx]
	and rax,3
	movd xmm0,dword [rsi+rax*4]
	punpcklbw xmm0,xmm0
	movq [rdx+24],xmm0
	lea rcx,[rcx+1]
	lea rdx,[rdx+32]
	dec r10
	jnz cp2_a1_bgra16lop3

	mov r10,r8
	and r10,3
	jz cp2_a1_bgra16lop3b
	movq xmm1,rbx
	mov bl,byte [rcx]
	lea rcx,[rcx+1]
	align 16
cp2_a1_bgra16lop3a:
	rol bl,2
	movzx rax,bl
	and rax,3
	movd xmm0,dword [rsi+rax*4]
	punpcklbw xmm0,xmm0
	movq [rdx],xmm0

	lea rdx,[rdx+8]
	dec r10
	jnz cp2_a1_bgra16lop3a
	movq rbx,xmm1
	align 16
cp2_a1_bgra16lop3b:
	
	mov r10,r8
	mov rdx,r11
	shr r10,3
	align 16
cp2_a1_bgra16lop4:
	movzx rax,byte [rcx]
	shr rax,7
	neg ax
	mov word [rdx+6],ax

	movzx rax,byte [rcx]
	shr rax,6
	and rax,1
	neg ax
	mov word [rdx+14],ax

	movzx rax,byte [rcx]
	shr rax,5
	and rax,1
	neg ax
	mov word [rdx+22],ax

	movzx rax,byte [rcx]
	shr rax,4
	and rax,1
	neg ax
	mov word [rdx+30],ax

	movzx rax,byte [rcx]
	shr rax,3
	and rax,1
	neg ax
	mov word [rdx+38],ax

	movzx rax,byte [rcx]
	shr rax,2
	and rax,1
	neg ax
	mov word [rdx+46],ax

	movzx rax,byte [rcx]
	shr rax,1
	and rax,1
	neg ax
	mov word [rdx+54],ax

	movzx rax,byte [rcx]
	and rax,1
	neg ax
	mov word [rdx+62],ax

	lea rcx,[rcx+1]
	lea rdx,[rdx+64]
	dec r10
	jnz cp2_a1_bgra16lop4

	mov r10,r8
	and r10,7
	jz cp2_a1_bgra16lop4b
	mov ah,byte [rcx]
	lea rcx,[rcx+1]
	align 16
cp2_a1_bgra16lop4a:
	rol ah,1
	mov al,ah
	neg al
	mov byte [rdx+6],al
	mov byte [rdx+7],al

	lea rdx,[rdx+8]
	dec r10
	jnz cp2_a1_bgra16lop4a
	align 16
cp2_a1_bgra16lop4b:
	
	add r11,qword [rsp+56] ;dbpl
	dec r9
	jnz cp2_a1_bgra16lop2

	pop rsi
	ret

;void ImageUtil_ConvP4_A1_B16G16R16A16(UInt8 *srcPtr, UInt8 *destPtr, OSInt w, OSInt h, OSInt sbpl, OSInt dbpl, UInt8 *pal);
;0 rsi
;8 retAddr
;rcx srcPtr
;rdx destPtr
;r8 w
;r9 h
;48 sbpl
;56 dbpl
;64 pal rsi
	align 16
ImageUtil_ConvP4_A1_B16G16R16A16:
	push rsi
	mov rsi,qword [rsp+64] ;pal
	mov r11,rdx

	align 16
cp4_a1_bgra16lop2:
	mov r10,r8
	mov rdx,r11
	shr r10,1
	align 16
cp4_a1_bgra16lop3:
	movzx rax,byte [rcx]
	shr rax,4
	movd xmm0,dword [rsi+rax*4]
	punpcklbw xmm0,xmm0
	movq [rdx],xmm0
	movzx rax,byte [rcx]
	and rax,15
	movd xmm0,dword [rsi+rax*4]
	lea rcx,[rcx+1]
	punpcklbw xmm0,xmm0
	movq [rdx+8],xmm0
	lea rdx,[rdx+16]
	dec r10
	jnz cp4_a1_bgra16lop3

	test r8,1
	jz cp4_a1_bgra16lop3b
	movzx rax,byte [rcx]
	lea rcx,[rcx+1]
	shr al,4
	movd xmm0,dword [rsi+rax*4]
	punpcklbw xmm0,xmm0
	movq [rdx],xmm0
	lea rdx,[rdx+8]
	
	align 16
cp4_a1_bgra16lop3b:
	
	mov r10,r8
	mov rdx,r11
	shr r10,3
	align 16
cp4_a1_bgra16lop4:
	movzx rax,byte [rcx]
	shr rax,7
	neg ax
	mov word [rdx+6],ax

	movzx rax,byte [rcx]
	shr rax,6
	and rax,1
	neg ax
	mov word [rdx+14],ax

	movzx rax,byte [rcx]
	shr rax,5
	and rax,1
	neg ax
	mov word [rdx+22],ax

	movzx rax,byte [rcx]
	shr rax,4
	and rax,1
	neg ax
	mov word [rdx+30],ax

	movzx rax,byte [rcx]
	shr rax,3
	and rax,1
	neg ax
	mov word [rdx+38],ax

	movzx rax,byte [rcx]
	shr rax,2
	and rax,1
	neg ax
	mov word [rdx+46],ax

	movzx rax,byte [rcx]
	shr rax,1
	and rax,1
	neg ax
	mov word [rdx+54],ax

	movzx rax,byte [rcx]
	and rax,1
	neg ax
	mov word [rdx+62],ax

	lea rcx,[rcx+1]
	lea rdx,[rdx+64]
	dec r10
	jnz cp4_a1_bgra16lop4
	
	mov r10,r8
	and r10,7
	jz cp4_a1_bgra16lop4b
	mov ah,byte [rcx]
	lea rcx,[rcx+1]
	align 16
cp4_a1_bgra16lop4a:
	rol ah,1
	mov al,ah
	neg al
	mov byte [rdx+6],al
	mov byte [rdx+7],al

	lea rdx,[rdx+8]
	dec r10
	jnz cp4_a1_bgra16lop4a
	align 16
cp4_a1_bgra16lop4b:
	
	add r11,qword [rsp+56] ;dbpl
	dec r9
	jnz cp4_a1_bgra16lop2

	pop rsi
	ret

;void ImageUtil_ConvP8_A1_B16G16R16A16(UInt8 *srcPtr, UInt8 *destPtr, OSInt w, OSInt h, OSInt sbpl, OSInt dbpl, UInt8 *pal);
;0 rsi
;8 retAddr
;rcx srcPtr
;rdx destPtr
;r8 w
;r9 h
;48 sbpl
;56 dbpl
;64 pal

	align 16
ImageUtil_ConvP8_A1_B16G16R16A16:
	push rsi
	mov rsi,qword [rsp+64] ;pal
	mov r11,rdx

	align 16
cp8_a1_bgra16lop:
	mov r10,r8
	mov rdx,r11
	
	align 16
cp8_a1_bgra16lop2:
	movzx rax,byte [rcx]
	movd xmm0,dword [rsi+rax*4]
	lea rcx,[rcx+1]
	punpcklbw xmm0,xmm0
	movq [rdx],xmm0
	lea rdx,[rdx+8]
	dec r10
	jnz cp8_a1_bgra16lop2
	
	mov r10,r8
	mov rdx,r11
	shr r10,3
	align 16
cp8_a1_bgra16lop3:
	movzx rax,byte [rcx]
	shr rax,7
	neg ax
	mov word [rdx+6],ax

	movzx rax,byte [rcx]
	shr rax,6
	and rax,1
	neg ax
	mov word [rdx+14],ax

	movzx rax,byte [rcx]
	shr rax,5
	and rax,1
	neg ax
	mov word [rdx+22],ax

	movzx rax,byte [rcx]
	shr rax,4
	and rax,1
	neg ax
	mov word [rdx+30],ax

	movzx rax,byte [rcx]
	shr rax,3
	and rax,1
	neg ax
	mov word [rdx+38],ax

	movzx rax,byte [rcx]
	shr rax,2
	and rax,1
	neg ax
	mov word [rdx+46],ax

	movzx rax,byte [rcx]
	shr rax,1
	and rax,1
	neg ax
	mov word [rdx+54],ax

	movzx rax,byte [rcx]
	and rax,1
	neg ax
	mov word [rdx+62],ax

	lea rcx,[rcx+1]
	lea rdx,[rdx+64]
	dec r10
	jnz cp8_a1_bgra16lop3
	
	mov r10,r8
	and r10,7
	jz cp8_a1_bgra16lop4b
	mov ah,byte [rcx]
	lea rcx,[rcx+1]
	align 16
cp8_a1_bgra16lop4a:
	rol ah,1
	mov al,ah
	neg al
	mov byte [rdx+6],al
	mov byte [rdx+7],al

	lea rdx,[rdx+8]
	dec r10
	jnz cp8_a1_bgra16lop4a
	align 16
cp8_a1_bgra16lop4b:
	
	add r11,qword [rsp+56] ;dbpl
	dec r9
	jnz cp8_a1_bgra16lop
	
	pop rsi
	ret

;void ImageUtil_ConvB5G5R5_B16G16R16A16(UInt8 *srcPtr, UInt8 *destPtr, OSInt w, OSInt h, OSInt sbpl, OSInt dbpl);
;0 retAddr
;rcx srcPtr
;rdx destPtr
;r8 w
;r9 h
;40 sbpl
;48 dbpl

	align 16
ImageUtil_ConvB5G5R5_B16G16R16A16:
	lea rax,[r8*2]
	sub qword [rsp+40],rax ;sbpl
	lea rax,[r8*8]
	sub qword [rsp+48],rax ;dbpl
	mov eax,0x08420842
	movd xmm2,eax
	pxor xmm1,xmm1
	punpckldq xmm2,xmm2

	align 16
cb5g5r5_bgra16lop:
	mov r10,r8
	
	align 16
cb5g5r5_bgra16lop2:
	movzx eax,word [ecx]
	and eax,0x7fff
	shl eax,6
	shr ax,3
	shr al,3
	movd xmm0, eax
	punpcklbw xmm0,xmm1
	pmullw xmm0, xmm2
	movq [rdx],xmm0
	mov word [rdx+6],0xffff

	lea rcx,[rcx+2]
	lea rdx,[rdx+8]
	dec r10
	jnz cb5g5r5_bgra16lop2
	
	add rcx,qword [rsp+40] ;sbpl
	add rdx,qword [rsp+48] ;dbpl
	dec r9
	jnz cb5g5r5_bgra16lop
	ret

;void ImageUtil_ConvB5G6R5_B16G16R16A16(UInt8 *srcPtr, UInt8 *destPtr, OSInt w, OSInt h, OSInt sbpl, OSInt dbpl);
;0 retAddr
;rcx srcPtr
;rdx destPtr
;r8 w
;r9 h
;40 sbpl
;48 dbpl

	align 16
ImageUtil_ConvB5G6R5_B16G16R16A16:
	lea rax,[r8*2]
	sub qword [rsp+40],rax ;sbpl
	lea rax,[r8*8]
	sub qword [rsp+48],rax ;dbpl
	mov eax,0x04100842
	movd xmm2,eax
	pxor xmm1,xmm1
	punpckldq xmm2,xmm2

	align 16
cb5g6r5_bgra16lop:
	mov r10,r8
	
	align 16
cb5g6r5_bgra16lop2:
	movzx eax,word [ecx]
	shl eax,5
	shr ax,2
	shr al,3
	movd xmm0, eax
	punpcklbw xmm0,xmm1
	pmullw xmm0, xmm2
	movq [rdx],xmm0
	mov word [rdx+6],0xffff

	lea rcx,[rcx+2]
	lea rdx,[rdx+8]
	dec r10
	jnz cb5g6r5_bgra16lop2
	
	add rcx,qword [rsp+40] ;sbpl
	add rdx,qword [rsp+48] ;dbpl
	dec r9
	jnz cb5g6r5_bgra16lop
	ret

;void ImageUtil_ConvB8G8R8_B16G16R16A16(UInt8 *srcPtr, UInt8 *destPtr, OSInt w, OSInt h, OSInt sbpl, OSInt dbpl);
;0 retAddr
;rcx srcPtr
;rdx destPtr
;r8 w
;r9 h
;40 sbpl
;48 dbpl
	align 16
ImageUtil_ConvB8G8R8_B16G16R16A16:
	lea rax,[r8+r8*2]
	sub qword [rsp+40],rax ;sbpl
	lea rax,[r8*8]
	sub qword [rsp+48],rax ;dbpl
	
	align 16
cbgr8_bgra16lop:
	mov r10,r8
	align 16
cbgr8_bgra16lop2:
	movzx eax,byte [rcx+2]
	or eax,0xff00
	shl eax,16
	mov ax,word [rcx]
	movd xmm0,eax
	punpcklbw xmm0,xmm0
	movq [rdx],xmm0
	lea rcx,[rcx+3]
	lea rdx,[rdx+8]
	dec r10
	jnz cbgr8_bgra16lop2
	
	add rcx,qword [rsp+40] ;sbpl
	add rdx,qword [rsp+48] ;dbpl
	dec r9
	jnz cbgr8_bgra16lop
	ret
	
;void ImageUtil_ConvR8G8B8_B16G16R16A16(UInt8 *srcPtr, UInt8 *destPtr, OSInt w, OSInt h, OSInt sbpl, OSInt dbpl);
;0 retAddr
;rcx srcPtr
;rdx destPtr
;r8 w
;r9 h
;40 sbpl
;48 dbpl
	align 16
ImageUtil_ConvR8G8B8_B16G16R16A16:
	lea rax,[r8+r8*2]
	sub qword [rsp+40],rax ;sbpl
	lea rax,[r8*8]
	sub qword [rsp+48],rax ;dbpl
	
	align 16
crgb8_bgra16lop:
	mov r10,r8
	align 16
crgb8_bgra16lop2:
	movzx eax,byte [rcx]
	or eax,0xff00
	shl eax,16
	mov ah,byte [rcx+1]
	mov al,byte [rcx+2]
	movd xmm0,eax
	punpcklbw xmm0,xmm0
	movq [rdx],xmm0
	lea rcx,[rcx+3]
	lea rdx,[rdx+8]
	dec r10
	jnz crgb8_bgra16lop2
	
	add rcx,qword [rsp+40] ;sbpl
	add rdx,qword [rsp+48] ;dbpl
	dec r9
	jnz crgb8_bgra16lop
	ret
	
;void ImageUtil_ConvR8G8B8A8_B16G16R16A16(UInt8 *srcPtr, UInt8 *destPtr, OSInt w, OSInt h, OSInt sbpl, OSInt dbpl);
;0 retAddr
;rcx srcPtr
;rdx destPtr
;r8 w
;r9 h
;40 sbpl
;48 dbpl
	align 16
ImageUtil_ConvR8G8B8A8_B16G16R16A16:
	lea rax,[r8*4]
	sub qword [rsp+40],rax ;sbpl
	lea rax,[r8*8]
	sub qword [rsp+48],rax ;dbpl
	
	align 16
crgba8_bgra16lop:
	mov r10,r8
	align 16
crgba8_bgra16lop2:
	movzx eax,byte [rcx]
	mov ah,byte [rcx+3]
	shl eax,16
	mov ah,byte [rcx+1]
	mov al,byte [rcx+2]
	movd xmm0,eax
	punpcklbw xmm0,xmm0
	movq [rdx],xmm0
	lea rcx,[rcx+4]
	lea rdx,[rdx+8]
	dec r10
	jnz crgba8_bgra16lop2
	
	add rcx,qword [rsp+40] ;sbpl
	add rdx,qword [rsp+48] ;dbpl
	dec r9
	jnz crgba8_bgra16lop
	ret
	
;void ImageUtil_ConvB8G8R8A8_B16G16R16A16(UInt8 *srcPtr, UInt8 *destPtr, OSInt w, OSInt h, OSInt sbpl, OSInt dbpl);
;0 retAddr
;rcx srcPtr
;rdx destPtr
;r8 w
;r9 h
;40 sbpl
;48 dbpl
	align 16
ImageUtil_ConvB8G8R8A8_B16G16R16A16:
	lea rax,[r8*4]
	sub qword [rsp+40],rax ;sbpl
	lea rax,[r8*8]
	sub qword [rsp+48],rax ;dbpl
	
	align 16
cbgra8_bgra16lop:
	mov r10,r8
	align 16
cbgra8_bgra16lop2:
	movd xmm0,[rcx]
	punpcklbw xmm0,xmm0
	lea rcx,[rcx+4]
	movq [rdx],xmm0
	lea rdx,[rdx+8]
	dec r10
	jnz cbgra8_bgra16lop2
	
	add rcx,qword [rsp+40] ;sbpl
	add rdx,qword [rsp+48] ;dbpl
	dec r9
	jnz cbgra8_bgra16lop
	ret

;void ImageUtil_ConvB16G16R16_B16G16R16A16(UInt8 *srcPtr, UInt8 *destPtr, OSInt w, OSInt h, OSInt sbpl, OSInt dbpl);
;0 retAddr
;rcx srcPtr
;rdx destPtr
;r8 w
;r9 h
;40 sbpl
;48 dbpl
	align 16
ImageUtil_ConvB16G16R16_B16G16R16A16:
	lea rax,[r8+r8*2]
	shl rax,1
	sub qword [rsp+40],rax ;sbpl
	lea rax,[r8*8]
	sub qword [rsp+48],rax ;dbpl
	
	align 16
cbgr16_bgra16lop:
	mov r10,r8
	align 16
cbgr16_bgra16lop2:
	mov eax,dword [rcx]
	movnti dword [rdx],eax
	mov ax,word [rcx+4]
	or eax,0xffff0000
	movnti dword [rdx+4],eax
	lea rcx,[rcx+6]
	lea rdx,[rdx+8]
	dec r10
	jnz cbgr16_bgra16lop2
	
	add rcx,qword [rsp+40] ;sbpl
	add rdx,qword [rsp+48] ;dbpl
	dec r9
	jnz cbgr16_bgra16lop
	ret

;void ImageUtil_ConvR16G16B16_B16G16R16A16(UInt8 *srcPtr, UInt8 *destPtr, OSInt w, OSInt h, OSInt sbpl, OSInt dbpl);
;0 retAddr
;rcx srcPtr
;rdx destPtr
;r8 w
;r9 h
;40 sbpl
;48 dbpl
	align 16
ImageUtil_ConvR16G16B16_B16G16R16A16:
	lea rax,[r8+r8*2]
	shl rax,1
	sub qword [rsp+40],rax ;sbpl
	lea rax,[r8*8]
	sub qword [rsp+48],rax ;dbpl
	
	align 16
crgb16_bgra16lop:
	mov r10,r8
	align 16
crgb16_bgra16lop2:
	mov ax,word [rcx+2]
	shl eax,16
	mov ax,word [rcx+4]
	movnti dword [rdx],eax
	mov ax,word [rcx]
	or eax,0xffff0000
	movnti dword [rdx+4],eax
	lea rcx,[rcx+6]
	lea rdx,[rdx+8]
	dec r10
	jnz crgb16_bgra16lop2
	
	add rcx,qword [rsp+40] ;sbpl
	add rdx,qword [rsp+48] ;dbpl
	dec r9
	jnz crgb16_bgra16lop
	ret

;void ImageUtil_ConvR16G16B16A16_B16G16R16A16(UInt8 *srcPtr, UInt8 *destPtr, OSInt w, OSInt h, OSInt sbpl, OSInt dbpl);
;0 retAddr
;rcx srcPtr
;rdx destPtr
;r8 w
;r9 h
;40 sbpl
;48 dbpl
	align 16
ImageUtil_ConvR16G16B16A16_B16G16R16A16:
	lea rax,[r8+r8*2]
	shl rax,1
	sub qword [rsp+40],rax ;sbpl
	lea rax,[r8*8]
	sub qword [rsp+48],rax ;dbpl
	
	align 16
crgba16_bgra16lop:
	mov r10,r8
	align 16
crgba16_bgra16lop2:
	movq xmm0,[rcx]
	pshuflw xmm0,xmm0,0xc6
	movq [rdx],xmm0
	lea rcx,[rcx+8]
	lea rdx,[rdx+8]
	dec r10
	jnz crgba16_bgra16lop2
	
	add rcx,qword [rsp+40] ;sbpl
	add rdx,qword [rsp+48] ;dbpl
	dec r9
	jnz crgba16_bgra16lop
	ret

;void ImageUtil_ConvA2B10G10R10_B16G16R16A16(UInt8 *srcPtr, UInt8 *destPtr, OSInt w, OSInt h, OSInt sbpl, OSInt dbpl);
;0 retAddr
;rcx srcPtr
;rdx destPtr
;r8 w
;r9 h
;40 sbpl
;48 dbpl
	align 16
ImageUtil_ConvA2B10G10R10_B16G16R16A16:
	mov r11,rbx
	lea rax,[r8*4]
	sub qword [rsp+40],rax ;sbpl
	lea rax,[r8*8]
	sub qword [rsp+48],rax ;dbpl
	
	align 16
ca2b10g10r10_bgra16lop:
	mov r10,r8
	align 16
ca2b10g10r10_bgra16lop2:
	mov eax,dword [rcx]
	mov ebx,eax
	shl eax,6
	shr ebx,4
	and bx,0x3f
	or ax,bx
	mov word [rdx+4],ax
	mov ax,0
	shr eax,10
	mov bx,ax
	shr bx,10
	or ax,bx
	mov word [rdx+2],ax
	shr ebx,10
	mov ax,bx
	shr bx,10
	or ax,bx
	mov word [rdx+0],ax
	mov bx,0
	shr ebx,2
	mov eax,ebx
	shr ebx,2
	or ax,bx
	mov bx,ax
	shr bx,4
	or ax,bx
	mov al,ah
	mov word [rdx+6],ax
	lea rcx,[rcx+4]
	lea rdx,[rdx+8]
	dec r10
	jnz ca2b10g10r10_bgra16lop2
	
	add rcx,qword [rsp+40] ;sbpl
	add rdx,qword [rsp+48] ;dbpl
	dec r9
	jnz ca2b10g10r10_bgra16lop
	mov rbx,r11
	ret

;void ImageUtil_ConvFB32G32R32A32_B16G16R16A16(UInt8 *srcPtr, UInt8 *destPtr, OSInt w, OSInt h, OSInt sbpl, OSInt dbpl);
;0 retAddr
;rcx srcPtr
;rdx destPtr
;r8 w
;r9 h
;40 sbpl
;48 dbpl
	align 16
ImageUtil_ConvFB32G32R32A32_B16G16R16A16:
	lea rax,[r8*8]
	sub qword [rsp+48],rax ;dbpl
	lea rax,[rax*2]
	sub qword [rsp+40],rax ;sbpl
	mov rax,65535
	cvtsi2ss xmm1,rax
	mov rax,32768
	movd xmm2,eax
	unpcklps xmm1,xmm1
	unpcklps xmm1,xmm1
	punpckldq xmm2,xmm2
	punpckldq xmm2,xmm2
	movdqa xmm3,xmm2
	pslld xmm3,16
	por xmm3,xmm2
	
	align 16
cfbgra32_lbgra16lop:
	mov r10,r8
	align 16
cfbgra32_lbgra16lop2:
	movups xmm0,[rcx]
	mulps xmm0,xmm1
	cvtps2dq xmm0,xmm0
	psubd xmm0,xmm2
	packssdw xmm0,xmm0
	paddw xmm0,xmm3
	movq [rdx],xmm0
	lea rcx,[rcx+16]
	lea rdx,[rdx+8]
	dec r10
	jnz cfbgra32_lbgra16lop2
	
	add rcx,qword [rsp+40] ;sbpl
	add rdx,qword [rsp+48] ;dbpl
	dec r9
	jnz cfbgra32_lbgra16lop
	ret

;void ImageUtil_ConvFR32G32B32A32_B16G16R16A16(UInt8 *srcPtr, UInt8 *destPtr, OSInt w, OSInt h, OSInt sbpl, OSInt dbpl);
;0 retAddr
;rcx srcPtr
;rdx destPtr
;r8 w
;r9 h
;40 sbpl
;48 dbpl
	align 16
ImageUtil_ConvFR32G32B32A32_B16G16R16A16:
	lea rax,[r8*8]
	sub qword [rsp+48],rax ;dbpl
	lea rax,[rax*2]
	sub qword [rsp+40],rax ;sbpl
	mov rax,65535
	cvtsi2ss xmm1,rax
	mov rax,32768
	movd xmm2,eax
	unpcklps xmm1,xmm1
	unpcklps xmm1,xmm1
	punpckldq xmm2,xmm2
	punpckldq xmm2,xmm2
	movdqa xmm3,xmm2
	pslld xmm3,16
	por xmm3,xmm2
	
	align 16
cfrgba32_lbgra16lop:
	mov r10,r8
	align 16
cfrgba32_lbgra16lop2:
	movups xmm0,[rcx]
	pshufd xmm0,xmm0,0xc6
	mulps xmm0,xmm1
	cvtps2dq xmm0,xmm0
	psubd xmm0,xmm2
	packssdw xmm0,xmm0
	paddw xmm0,xmm3
	movq [rdx],xmm0
	lea rcx,[rcx+16]
	lea rdx,[rdx+8]
	dec r10
	jnz cfrgba32_lbgra16lop2
	
	add rcx,qword [rsp+40] ;sbpl
	add rdx,qword [rsp+48] ;dbpl
	dec r9
	jnz cfrgba32_lbgra16lop
	ret

;void ImageUtil_ConvFR32G32B32_B16G16R16A16(UInt8 *srcPtr, UInt8 *destPtr, OSInt w, OSInt h, OSInt sbpl, OSInt dbpl);
;0 retAddr
;rcx srcPtr
;rdx destPtr
;r8 w
;r9 h
;40 sbpl
;48 dbpl
	align 16
ImageUtil_ConvFR32G32B32_B16G16R16A16:
	lea rax,[r8*8]
	sub qword [rsp+48],rax ;dbpl
	lea rax,[r8*4+rax]
	sub qword [rsp+40],rax ;sbpl
	mov rax,65535
	cvtsi2ss xmm1,rax
	mov rax,32768
	movd xmm2,eax
	unpcklps xmm1,xmm1
	unpcklps xmm1,xmm1
	punpckldq xmm2,xmm2
	punpckldq xmm2,xmm2
	movdqa xmm3,xmm2
	pslld xmm3,16
	por xmm3,xmm2
	
	align 16
cfrgba32_bgra16lop:
	mov r10,r8
	align 16
cfrgba32_bgra16lop2:
	movq xmm0,[rcx]
	movss xmm4,[rcx+8]
	punpcklqdq xmm0,xmm4
	pshufd xmm0,xmm0,0xc6
	mulps xmm0,xmm1
	cvtps2dq xmm0,xmm0
	psubd xmm0,xmm2
	packssdw xmm0,xmm0
	paddw xmm0,xmm3
	movq [rdx],xmm0
	mov word [rdx+6],0xffff
	lea rcx,[rcx+12]
	lea rdx,[rdx+8]
	dec r10
	jnz cfrgba32_bgra16lop2
	
	add rcx,qword [rsp+40] ;sbpl
	add rdx,qword [rsp+48] ;dbpl
	dec r9
	jnz cfrgba32_bgra16lop
	ret

;void ImageUtil_ConvFB32G32R32_B16G16R16A16(UInt8 *srcPtr, UInt8 *destPtr, OSInt w, OSInt h, OSInt sbpl, OSInt dbpl);
;0 retAddr
;rcx srcPtr
;rdx destPtr
;r8 w
;r9 h
;40 sbpl
;48 dbpl
	align 16
ImageUtil_ConvFB32G32R32_B16G16R16A16:
	lea rax,[r8*8]
	sub qword [rsp+48],rax ;dbpl
	lea rax,[r8*4+rax]
	sub qword [rsp+40],rax ;sbpl
	mov rax,65535
	cvtsi2ss xmm1,rax
	mov rax,32768
	movd xmm2,eax
	unpcklps xmm1,xmm1
	unpcklps xmm1,xmm1
	punpckldq xmm2,xmm2
	punpckldq xmm2,xmm2
	movdqa xmm3,xmm2
	pslld xmm3,16
	por xmm3,xmm2
	
	align 16
cfbgra32_bgra16lop:
	mov r10,r8
	align 16
cfbgra32_bgra16lop2:
	movq xmm0,[rcx]
	movss xmm4,[rcx+8]
	punpcklqdq xmm0,xmm4
	mulps xmm0,xmm1
	cvtps2dq xmm0,xmm0
	psubd xmm0,xmm2
	packssdw xmm0,xmm0
	paddw xmm0,xmm3
	movq [rdx],xmm0
	mov word [rdx+6],0xffff
	lea rcx,[rcx+12]
	lea rdx,[rdx+8]
	dec r10
	jnz cfbgra32_bgra16lop2
	
	add rcx,qword [rsp+40] ;sbpl
	add rdx,qword [rsp+48] ;dbpl
	dec r9
	jnz cfbgra32_bgra16lop
	ret

;void ImageUtil_ConvFW32A32_B16G16R16A16(UInt8 *srcPtr, UInt8 *destPtr, OSInt w, OSInt h, OSInt sbpl, OSInt dbpl);
;0 retAddr
;rcx srcPtr
;rdx destPtr
;r8 w
;r9 h
;40 sbpl
;48 dbpl
	align 16
ImageUtil_ConvFW32A32_B16G16R16A16:
	lea rax,[r8*8]
	sub qword [rsp+48],rax ;dbpl
	sub qword [rsp+40],rax ;sbpl
	mov rax,65535
	cvtsi2ss xmm1,rax
	mov rax,32768
	movd xmm2,eax
	unpcklps xmm1,xmm1
	unpcklps xmm1,xmm1
	punpckldq xmm2,xmm2
	punpckldq xmm2,xmm2
	movdqa xmm3,xmm2
	pslld xmm3,16
	por xmm3,xmm2
	
	align 16
cfwa32_bgra16lop:
	mov r10,r8
	align 16
cfwa32_bgra16lop2:
	movq xmm0,[rcx]
	mulps xmm0,xmm1
	cvtps2dq xmm0,xmm0
	psubd xmm0,xmm2
	packssdw xmm0,xmm0
	paddw xmm0,xmm3
	movd eax,xmm0
	mov word [rdx],ax
	mov word [rdx+2],ax
	mov dword [rdx+4],eax
	lea rcx,[rcx+8]
	lea rdx,[rdx+8]
	dec r10
	jnz cfwa32_bgra16lop2
	
	add rcx,qword [rsp+40] ;sbpl
	add rdx,qword [rsp+48] ;dbpl
	dec r9
	jnz cfwa32_bgra16lop
	ret

;void ImageUtil_ConvFW32_B16G16R16A16(UInt8 *srcPtr, UInt8 *destPtr, OSInt w, OSInt h, OSInt sbpl, OSInt dbpl);
;0 retAddr
;rcx srcPtr
;rdx destPtr
;r8 w
;r9 h
;40 sbpl
;48 dbpl
	align 16
ImageUtil_ConvFW32_B16G16R16A16:
	lea rax,[r8*4]
	sub qword [rsp+40],rax ;sbpl
	lea rax,[r8*8]
	sub qword [rsp+48],rax ;dbpl
	mov rax,65535
	cvtsi2ss xmm1,rax
	mov rax,32768
	movd xmm2,eax
	unpcklps xmm1,xmm1
	unpcklps xmm1,xmm1
	punpckldq xmm2,xmm2
	punpckldq xmm2,xmm2
	movdqa xmm3,xmm2
	pslld xmm3,16
	por xmm3,xmm2
	
	align 16
cfw32_bgra16lop:
	mov r10,r8
	align 16
cfw32_bgra16lop2:
	movss xmm0,[rcx]
	mulps xmm0,xmm1
	cvtps2dq xmm0,xmm0
	psubd xmm0,xmm2
	packssdw xmm0,xmm0
	paddw xmm0,xmm3
	movd eax,xmm0
	mov word [rdx],ax
	mov word [rdx+2],ax
	mov word [rdx+4],ax
	mov word [rdx+6],0xffff
	lea rcx,[rcx+4]
	lea rdx,[rdx+8]
	dec r10
	jnz cfw32_bgra16lop2
	
	add rcx,qword [rsp+40] ;sbpl
	add rdx,qword [rsp+48] ;dbpl
	dec r9
	jnz cfw32_bgra16lop
	ret

;void ImageUtil_ConvW16_B8G8R8A8(UInt8 *srcPtr, UInt8 *destPtr, OSInt w, OSInt h, OSInt sbpl, OSInt dbpl);
;0 retAddr
;rcx srcPtr
;rdx destPtr
;r8 w
;r9 h
;40 sbpl
;48 dbpl
	align 16
ImageUtil_ConvW16_B8G8R8A8:
	mov r11,rbx
	lea rax,[r8*2]
	sub qword [rsp+40],rax ;sbpl
	lea rax,[r8*4]
	sub qword [rsp+48],rax ;dbpl
	
	align 16
cw16_bgra8lop:
	mov r10,r8
	align 16
cw16_bgra8lop2:
	movzx eax,byte [rcx+1]
	mov bl,al
	or eax,0xff00
	shl eax,16
	mov ah,bl
	mov al,bl
	movnti dword [rdx],eax
	lea rcx,[rcx+2]
	lea rdx,[rdx+4]
	dec r10
	jnz cw16_bgra8lop2
	
	add rcx,qword [rsp+40] ;sbpl
	add rdx,qword [rsp+48] ;dbpl
	dec r9
	jnz cw16_bgra8lop
	mov rbx,r11
	ret

;void ImageUtil_ConvW16A16_B8G8R8A8(UInt8 *srcPtr, UInt8 *destPtr, OSInt w, OSInt h, OSInt sbpl, OSInt dbpl);
;0 retAddr
;rcx srcPtr
;rdx destPtr
;r8 w
;r9 h
;40 sbpl
;48 dbpl
	align 16
ImageUtil_ConvW16A16_B8G8R8A8:
	mov r11,rbx
	lea rax,[r8*4]
	sub qword [rsp+40],rax ;sbpl
	lea rax,[r8*4]
	sub qword [rsp+48],rax ;dbpl
	
	align 16
cwa16_bgra8lop:
	mov r10,r8
	align 16
cwa16_bgra8lop2:
	movzx eax,byte [rcx+1]
	mov bl,al
	mov ah,byte [rcx+3]
	shl eax,16
	mov ah,bl
	mov al,bl
	movnti dword [rdx],eax
	lea rcx,[rcx+4]
	lea rdx,[rdx+4]
	dec r10
	jnz cwa16_bgra8lop2
	
	add rcx,qword [rsp+40] ;sbpl
	add rdx,qword [rsp+48] ;dbpl
	dec r9
	jnz cwa16_bgra8lop
	mov rbx,r11
	ret

;void ImageUtil_ConvW8A8_B8G8R8A8(UInt8 *srcPtr, UInt8 *destPtr, OSInt w, OSInt h, OSInt sbpl, OSInt dbpl);
;0 retAddr
;rcx srcPtr
;rdx destPtr
;r8 w
;r9 h
;40 sbpl
;48 dbpl
	align 16
ImageUtil_ConvW8A8_B8G8R8A8:
	mov r11,rbx
	lea rax,[r8*2]
	sub qword [rsp+40],rax ;sbpl
	lea rax,[r8*4]
	sub qword [rsp+48],rax ;dbpl
	
	align 16
cwa8_bgra8lop:
	mov r10,r8
	align 16
cwa8_bgra8lop2:
	movzx eax,byte [rcx+0]
	mov bl,al
	mov ah,byte [rcx+1]
	shl eax,16
	mov ah,bl
	mov al,bl
	movnti dword [rdx],eax
	lea rcx,[rcx+2]
	lea rdx,[rdx+4]
	dec r10
	jnz cwa8_bgra8lop2
	
	add rcx,qword [rsp+40] ;sbpl
	add rdx,qword [rsp+48] ;dbpl
	dec r9
	jnz cwa8_bgra8lop
	mov rbx,r11
	ret

;void ImageUtil_ConvW16_B16G16R16A16(UInt8 *srcPtr, UInt8 *destPtr, OSInt w, OSInt h, OSInt sbpl, OSInt dbpl);
;0 retAddr
;rcx srcPtr
;rdx destPtr
;r8 w
;r9 h
;40 sbpl
;48 dbpl
	align 16
ImageUtil_ConvW16_B16G16R16A16:
	mov r11,rbx
	lea rax,[r8*2]
	sub qword [rsp+40],rax ;sbpl
	lea rax,[r8*8]
	sub qword [rsp+48],rax ;dbpl
	
	align 16
cw16_bgra16lop:
	mov r10,r8
	align 16
cw16_bgra16lop2:
	mov ax,word [rcx]
	or eax,0xffff0000
	mov word [rdx],ax
	mov word [rdx+2],ax
	mov dword [rdx+4],eax
	lea rcx,[rcx+2]
	lea rdx,[rdx+8]
	dec r10
	jnz cw16_bgra16lop2
	
	add rcx,qword [rsp+40] ;sbpl
	add rdx,qword [rsp+48] ;dbpl
	dec r9
	jnz cw16_bgra16lop
	mov rbx,r11
	ret

;void ImageUtil_ConvW16A16_B16G16R16A16(UInt8 *srcPtr, UInt8 *destPtr, OSInt w, OSInt h, OSInt sbpl, OSInt dbpl);
;0 retAddr
;rcx srcPtr
;rdx destPtr
;r8 w
;r9 h
;40 sbpl
;48 dbpl
	align 16
ImageUtil_ConvW16A16_B16G16R16A16:
	mov r11,rbx
	lea rax,[r8*4]
	sub qword [rsp+40],rax ;sbpl
	lea rax,[r8*8]
	sub qword [rsp+48],rax ;dbpl
	
	align 16
cwa16_bgra16lop:
	mov r10,r8
	align 16
cwa16_bgra16lop2:
	mov eax,dword [rcx]
	mov word [rdx],ax
	mov word [rdx+2],ax
	mov dword [rdx+4],eax
	lea rcx,[rcx+4]
	lea rdx,[rdx+8]
	dec r10
	jnz cwa16_bgra16lop2
	
	add rcx,qword [rsp+40] ;sbpl
	add rdx,qword [rsp+48] ;dbpl
	dec r9
	jnz cwa16_bgra16lop
	mov rbx,r11
	ret

;void ImageUtil_ConvW8A8_B16G16R16A16(UInt8 *srcPtr, UInt8 *destPtr, OSInt w, OSInt h, OSInt sbpl, OSInt dbpl);
;0 retAddr
;rcx srcPtr
;rdx destPtr
;r8 w
;r9 h
;40 sbpl
;48 dbpl
	align 16
ImageUtil_ConvW8A8_B16G16R16A16:
	mov r11,rbx
	lea rax,[r8*2]
	sub qword [rsp+40],rax ;sbpl
	lea rax,[r8*8]
	sub qword [rsp+48],rax ;dbpl
	
	align 16
cwa8_bgra16lop:
	mov r10,r8
	align 16
cwa8_bgra16lop2:
	mov al,byte [rcx+1]
	mov ah,al
	shl eax,16
	mov al,byte [rcx+0]
	mov ah,al
	mov word [rdx],ax
	mov word [rdx+2],ax
	mov dword [rdx+4],eax
	lea rcx,[rcx+2]
	lea rdx,[rdx+8]
	dec r10
	jnz cwa8_bgra16lop2
	
	add rcx,qword [rsp+40] ;sbpl
	add rdx,qword [rsp+48] ;dbpl
	dec r9
	jnz cwa8_bgra16lop
	mov rbx,r11
	ret


;void ImageUtil_ConvP1_P8(UInt8 *srcPtr, UInt8 *destPtr, OSInt w, OSInt h, OSInt sbpl, OSInt dbpl);
;0 retAddr
;rcx srcPtr
;rdx destPtr
;r8 srcWidth
;r9 srcHeight
;40 sbpl
	align 16
ImageUtil_ConvP1_P8:
	mov r11,r8
	mov r10,[rsp+40]
	shr r8,3
	jz cp1_p8lop
	sub r10,r8 ;sbpl
	and r11,7
	align 16
cp1_p8lop2:
	push r8
	align 16
cp1_p8lop3:
	mov al,byte [rcx]
	test al,0x80
	setne byte [rdx]
	test al,0x40
	setne byte [rdx+1]
	test al,0x20
	setne byte [rdx+2]
	test al,0x10
	setne byte [rdx+3]
	test al,0x8
	setne byte [rdx+4]
	test al,0x4
	setne byte [rdx+5]
	test al,0x2
	setne byte [rdx+6]
	test al,0x1
	setne byte [rdx+7]
	lea rcx,[rcx+1]
	lea rdx,[rdx+8]
	dec r8
	jnz cp1_p8lop3
	mov r8,r11
	mov al,byte [rcx]
	align 16
cp1_p8lop3b:
	test al,0x80
	setne byte [rdx]
	lea rdx,[rdx+1]
	shl al,1
	dec r8
	jnz cp1_p8lop3b
	pop r8
	add rcx,r10
	dec r9
	jnz cp1_p8lop2
	ret

	align 16
cp1_p8lop:
	sub r10,r8 ;sbpl

	align 16
cp1_p8lop4:
	push r8
	align 16
cp1_p8lop5:
	mov al,byte [rcx]
	test al,0x80
	setne byte [rdx]
	test al,0x40
	setne byte [rdx+1]
	test al,0x20
	setne byte [rdx+2]
	test al,0x10
	setne byte [rdx+3]
	test al,0x8
	setne byte [rdx+4]
	test al,0x4
	setne byte [rdx+5]
	test al,0x2
	setne byte [rdx+6]
	test al,0x1
	setne byte [rdx+7]
	lea rcx,[rcx+1]
	lea rdx,[rdx+8]
	dec r8
	jnz cp1_p8lop5
	pop r8
	add rcx,r10
	dec r9
	jnz cp1_p8lop4
	ret

;void ImageUtil_Rotate32_CW90(UInt8 *srcPtr, UInt8 *destPtr, OSInt srcWidth, OSInt srcHeight, OSInt sbpl, OSInt dbpl);
;0 rdi
;8 rsi
;16 retAddr
;rcx srcPtr
;rdx destPtr
;r8 srcWidth
;r9 srcHeight
;56 sbpl
;64 dbpl
	align 16
ImageUtil_Rotate32_CW90:
	push rsi
	push rdi
	mov rax,qword [rsp+56] ;sbpl
	mov rdi,rdx
	mul r9 ;srcHeight
	add rcx,rax ;srcPtr
	lea rdx,[r9*4]
	sub qword [rsp+64],rdx ;dbpl

	align 16
r32cw90lop:
	mov rdx,r9 ;srcHeight
	mov rsi,rcx ;srcPtr
	
	align 16
r32cw90lop2:
	sub rsi,qword [rsp+56] ;sbpl
	mov eax,dword [rsi]
	mov dword [rdi],eax
	lea rdi,[rdi+4]
	dec rdx
	jnz r32cw90lop2
	lea rcx,[rcx+4] ;srcPtr
	add rdi,qword [rsp+64] ;dbpl
	dec r8
	jnz r32cw90lop
	pop rdi
	pop rsi
	ret

;void ImageUtil_Rotate32_CW180(UInt8 *srcPtr, UInt8 *destPtr, OSInt srcWidth, OSInt srcHeight, OSInt sbpl, OSInt dbpl);
;0 rdi
;8 rsi
;16 retAddr
;rcx srcPtr
;rdx destPtr
;r8 srcWidth
;r9 srcHeight
;56 sbpl
;64 dbpl
	align 16
ImageUtil_Rotate32_CW180:
	push rsi
	push rdi
	mov rax,qword [rsp+56] ;sbpl
	mov rdi,rdx
	mul r9 ;srcHeight
	add rcx,rax ;srcPtr
	lea rax,[r8*4]
	sub qword [rsp+56],rax ;sbpl
	sub qword [rsp+64],rax ;dbpl

	align 16
r32cw180lop:
	mov rdx,r8 ;srcWidth
	
	align 16
r32cw180lop2:
	lea rcx,[rcx-4]
	mov eax,dword [rcx]
	mov dword [rdi],eax
	lea rdi,[rdi+4]
	dec rdx
	jnz r32cw180lop2
	
	sub rcx,qword [rsp+56] ;sbpl
	add rdi,qword [rsp+64] ;dbpl
	dec r9
	jnz r32cw180lop
	
	pop rdi
	pop rsi
	ret

;void ImageUtil_Rotate32_CW270(UInt8 *srcPtr, UInt8 *destPtr, OSInt srcWidth, OSInt srcHeight, OSInt sbpl, OSInt dbpl);
;0 rdi
;8 rsi
;16 retAddr
;rcx srcPtr
;rdx destPtr
;r8 srcWidth
;r9 srcHeight
;56 sbpl
;64 dbpl
	align 16
ImageUtil_Rotate32_CW270:
	push rsi
	push rdi
	add rcx,qword [rsp+56] ;sbpl
	lea rax,[r9*4] ;srcHeight
	sub qword [rsp+64],rax ;dbpl
	
	mov rdi,rdx ;destPtr
	align 16
r32cw270lop:
	lea rcx,[rcx-4] ;srcPtr
	mov rdx,r9 ;srcHeight
	mov rsi,rcx ;srcPtr
	
	align 16
r32cw270lop2:
	mov eax,dword [rsi]
	mov dword [rdi],eax
	add rsi,qword [rsp+56] ;sbpl
	lea rdi,[rdi+4]
	dec rdx
	jnz r32cw270lop2
	
	add rdi,qword [rsp+64] ;dbpl
	dec r8
	jnz r32cw270lop
	
	pop rdi
	pop rsi
	ret

;void ImageUtil_Rotate64_CW90(UInt8 *srcPtr, UInt8 *destPtr, OSInt srcWidth, OSInt srcHeight, OSInt sbpl, OSInt dbpl);
;0 rdi
;8 rsi
;16 retAddr
;rcx srcPtr
;rdx destPtr
;r8 srcWidth
;r9 srcHeight
;56 sbpl
;64 dbpl
	align 16
ImageUtil_Rotate64_CW90:
	push rsi
	push rdi
	mov rax,qword [rsp+56] ;sbpl
	mov rdi,rdx
	mul r9 ;srcHeight
	add rcx,rax ;srcPtr
	lea rdx,[r9*8]
	sub qword [rsp+64],rdx ;dbpl

	align 16
r64cw90lop:
	mov rdx,r9 ;srcHeight
	mov rsi,rcx ;srcPtr
	
	align 16
r64cw90lop2:
	sub rsi,qword [rsp+56] ;sbpl
	mov rax,qword [rsi]
	mov qword [rdi],rax
	lea rdi,[rdi+8]
	dec rdx
	jnz r64cw90lop2
	lea rcx,[rcx+8] ;srcPtr
	add rdi,qword [rsp+64] ;dbpl
	dec r8
	jnz r64cw90lop
	pop rdi
	pop rsi
	ret

;void ImageUtil_Rotate64_CW180(UInt8 *srcPtr, UInt8 *destPtr, OSInt srcWidth, OSInt srcHeight, OSInt sbpl, OSInt dbpl);
;0 rdi
;8 rsi
;16 retAddr
;rcx srcPtr
;rdx destPtr
;r8 srcWidth
;r9 srcHeight
;56 sbpl
;64 dbpl
	align 16
ImageUtil_Rotate64_CW180:
	push rsi
	push rdi
	mov rax,qword [rsp+56] ;sbpl
	mov rdi,rdx
	mul r9 ;srcHeight
	add rcx,rax ;srcPtr
	lea rax,[r8*8]
	sub qword [rsp+56],rax ;sbpl
	sub qword [rsp+64],rax ;dbpl

	align 16
r64cw180lop:
	mov rdx,r8 ;srcWidth
	
	align 16
r64cw180lop2:
	lea rcx,[rcx-8]
	mov rax,qword [rcx]
	mov qword [rdi],rax
	lea rdi,[rdi+8]
	dec rdx
	jnz r64cw180lop2
	
	sub rcx,qword [rsp+56] ;sbpl
	add rdi,qword [rsp+64] ;dbpl
	dec r9
	jnz r64cw180lop
	
	pop rdi
	pop rsi
	ret

;void ImageUtil_Rotate64_CW270(UInt8 *srcPtr, UInt8 *destPtr, OSInt srcWidth, OSInt srcHeight, OSInt sbpl, OSInt dbpl);
;0 rdi
;8 rsi
;16 retAddr
;rcx srcPtr
;rdx destPtr
;r8 srcWidth
;r9 srcHeight
;56 sbpl
;64 dbpl
	align 16
ImageUtil_Rotate64_CW270:
	push rsi
	push rdi
	add rcx,qword [rsp+56] ;sbpl
	lea rax,[r9*8] ;srcHeight
	sub qword [rsp+64],rax ;dbpl
	
	mov rdi,rdx ;destPtr
	align 16
r64cw270lop:
	lea rcx,[rcx-8] ;srcPtr
	mov rdx,r9 ;srcHeight
	mov rsi,rcx ;srcPtr
	
	align 16
r64cw270lop2:
	mov rax,qword [rsi]
	mov qword [rdi],rax
	add rsi,qword [rsp+56] ;sbpl
	lea rdi,[rdi+8]
	dec rdx
	jnz r64cw270lop2
	
	add rdi,qword [rsp+64] ;dbpl
	dec r8
	jnz r64cw270lop
	
	pop rdi
	pop rsi
	ret


;void ImageUtil_HFlip32(UInt8 *inPt, UInt8 *outPt, UOSInt width, UOSInt height, UOSInt sbpl, UOSInt dbpl, Bool upsideDown);
;0 retAddr
;rcx rdi srcPtr
;rdx rsi destPtr
;r8 rdx srcWidth
;r9 rcx srcHeight
;40 r8 sbpl
;48 r11 r9 dbpl
;56 upsideDown

	align 16
ImageUtil_HFlip32:
	mov r10,r8
	mov r11,[rsp+48]
	cmp byte [rsp+56],0
	jz hf32lop

	neg r11
	lea rax,[r9-1]
	imul rax,r11
	add rdx,rax

	align 16
hf32lop:
	lea rax,[r10*4]
	sub r11,rax

	align 16
hf32lop2:
	mov r8,r10

	align 16
hf32lop3:
	dec r8
	mov eax,dword [rcx+r8*4]
	mov dword [rdx],eax
	lea rdx,[rdx+4]
	jnz hf32lop3
	
	add rcx,[rsp+40]
	add rdx,r11
	dec r9
	jnz hf32lop2
	ret

;void ImageUtil_HFRotate32_CW90(UInt8 *srcPtr, UInt8 *destPtr, OSInt srcWidth, OSInt srcHeight, OSInt sbpl, OSInt dbpl);
;0 rsi
;8 retAddr
;rcx rdi srcPtr
;rdx rsi destPtr
;r8 rdx srcWidth
;r9 rcx srcHeight
;48 r8 sbpl
;56 r9 dbpl
	align 16
ImageUtil_HFRotate32_CW90:
	push rsi
	mov r10,r9 ;srcHeight
	mov rax,[rsp+48] ;sbpl
	imul rax,r9 ;srcHeight
	lea rcx,[rcx+rax] ;srcPtr
	lea rcx,[rcx+r8*4] ;srcPtr += sbpl * srcHeight + srcWidth * 4
	lea rax,[r10*4]
	sub [rsp+56],rax ;dbpl

	align 16
hfr32cw90lop:
	lea rcx,[rcx-4] ;srcPtr
	mov r11,r10 ;srcHeight
	mov rsi,rcx ;srcPtr
	
	align 16
hfr32cw90lop2:
	sub rsi,[rsp+48] ;sbpl
	mov eax,dword [rsi]
	mov dword [rdx],eax
	lea rdx,[rdx+4]
	dec r11
	jnz hfr32cw90lop2
	add rdx,[rsp+56] ;dbpl
	dec r8
	jnz hfr32cw90lop
	pop rsi
	ret

;void ImageUtil_HFRotate32_CW180(UInt8 *srcPtr, UInt8 *destPtr, OSInt srcWidth, OSInt srcHeight, OSInt sbpl, OSInt dbpl);
;0 retAddr
;rcx rdi srcPtr
;rdx rsi destPtr
;r8 rdx srcWidth
;r9 rcx srcHeight
;40 r8 sbpl
;48 r9 dbpl

	align 16
ImageUtil_HFRotate32_CW180:
	mov r11,rsi
	xchg rdx,rdi
	mov rsi,rcx
	mov rax,[rsp+40] ;sbpl
	imul rax,r9 ;srcHeight
	add rsi,rax ;srcPtr
	mov r10,r9 ;srcHeight
	lea rax,[r8*4]
	add [rsp+40],rax ;sbpl
	sub [rsp+48],rax ;dbpl

	align 16
hfr32cw180lop:
	sub rsi,[rsp+40] ;sbpl
	mov rcx,r8 ;srcWidth
	rep movsd
	add rdi,[rsp+48] ;dbpl
	dec r10
	jnz hfr32cw180lop
	mov rsi,r11
	mov rdi,rdx
	ret

;void ImageUtil_HFRotate32_CW270(UInt8 *srcPtr, UInt8 *destPtr, OSInt srcWidth, OSInt srcHeight, OSInt sbpl, OSInt dbpl);
;0 rsi
;8 retAddr
;rcx rdi srcPtr
;rdx rsi destPtr
;r8 rdx srcWidth
;r9 rcx srcHeight
;48 r8 sbpl
;56 r9 dbpl
	align 16
ImageUtil_HFRotate32_CW270:
	push rsi
	mov r10,r9 ;srcHeight
	lea rax,[r9*4] ;srcHeight
	sub [rsp+56],rax ;dbpl
	
	align 16
hfr32cw270lop:
	mov r11,r10 ;srcHeight
	mov rsi,rcx ;srcPtr
	
	align 16
hfr32cw270lop2:
	mov eax,dword [rsi]
	mov dword [rdx],eax
	add rsi,r8 ;sbpl
	lea rdx,[rdx+4]
	dec r11
	jnz hfr32cw270lop2
	
	lea rcx,[rcx+4] ;srcPtr
	add rdx,[rsp+56] ;dbpl
	dec r8
	jnz hfr32cw270lop
	
	pop rsi
	ret

;void ImageUtil_HFlip64(UInt8 *inPt, UInt8 *outPt, UOSInt width, UOSInt height, UOSInt sbpl, UOSInt dbpl, Bool upsideDown);
;0 retAddr
;rcx rdi srcPtr
;rdx rsi destPtr
;r8 rdx srcWidth
;r9 rcx srcHeight
;40 r8 sbpl
;48 r9 dbpl
;56 upsideDown

	align 16
ImageUtil_HFlip64:
	mov r10,r8
	mov r11,[rsp+48]
	cmp byte [rsp+56],0
	jz hf64lop

	neg r11
	lea rax,[r9-1]
	imul rax,r11
	add rdx,rax

	align 16
hf64lop:
	lea rax,[r10*8]
	sub r11,rax

	align 16
hf64lop2:
	mov r8,r10

	align 16
hf64lop3:
	dec r8
	mov rax,qword [rcx+r8*8]
	mov qword [rdx],rax
	lea rdx,[rdx+8]
	jnz hf64lop3
	
	add rcx,[rsp+40]
	add rdx,r11
	dec r9
	jnz hf64lop2
	ret

;void ImageUtil_HFRotate64_CW90(UInt8 *srcPtr, UInt8 *destPtr, OSInt srcWidth, OSInt srcHeight, OSInt sbpl, OSInt dbpl);
;0 rsi
;8 retAddr
;rcx rdi srcPtr
;rdx rsi destPtr
;r8 rdx srcWidth
;r9 rcx srcHeight
;48 r8 sbpl
;56 r9 dbpl
	align 16
ImageUtil_HFRotate64_CW90:
	push rsi
	mov r10,r9 ;srcHeight
	mov rax,[rsp+48] ;sbpl
	imul rax,r9 ;srcHeight
	lea rcx,[rcx+rax] ;srcPtr
	lea rcx,[rcx+r8*8] ;srcPtr += sbpl * srcHeight + srcWidth * 4
	lea rax,[r10*8]
	sub [rsp+56],rax ;dbpl

	align 16
hfr64cw90lop:
	lea rcx,[rcx-8] ;srcPtr
	mov r11,r10 ;srcHeight
	mov rsi,rcx ;srcPtr
	
	align 16
hfr64cw90lop2:
	sub rsi,[rsp+48] ;sbpl
	mov rax,qword [rsi]
	mov qword [rdx],rax
	lea rdx,[rdx+8]
	dec r11
	jnz hfr64cw90lop2
	add rdx,[rsp+56] ;dbpl
	dec r8
	jnz hfr64cw90lop
	pop rsi
	ret

;void ImageUtil_HFRotate64_CW180(UInt8 *srcPtr, UInt8 *destPtr, OSInt srcWidth, OSInt srcHeight, OSInt sbpl, OSInt dbpl);
;0 retAddr
;rcx rdi srcPtr
;rdx rsi destPtr
;r8 rdx srcWidth
;r9 rcx srcHeight
;40 r8 sbpl
;48 r9 dbpl

	align 16
ImageUtil_HFRotate64_CW180:
	mov r11,rsi
	xchg rdx,rdi
	mov rsi,rcx
	mov rax,[rsp+40] ;sbpl
	imul rax,r9 ;srcHeight
	add rsi,rax ;srcPtr
	mov r10,r9 ;srcHeight
	lea rax,[r8*8]
	add [rsp+40],rax ;sbpl
	sub [rsp+48],rax ;dbpl

	align 16
hfr64cw180lop:
	sub rsi,[rsp+40] ;sbpl
	mov rcx,r8 ;srcWidth
	rep movsq
	add rdi,[rsp+48] ;dbpl
	dec r10
	jnz hfr64cw180lop
	mov rsi,r11
	mov rdi,rdx
	ret

;void ImageUtil_HFRotate64_CW270(UInt8 *srcPtr, UInt8 *destPtr, OSInt srcWidth, OSInt srcHeight, OSInt sbpl, OSInt dbpl);
;0 rsi
;8 retAddr
;rcx rdi srcPtr
;rdx rsi destPtr
;r8 rdx srcWidth
;r9 rcx srcHeight
;48 r8 sbpl
;56 r9 dbpl
	align 16
ImageUtil_HFRotate64_CW270:
	push rsi
	mov r10,r9 ;srcHeight
	lea rax,[r9*8] ;srcHeight
	sub [rsp+56],rax ;dbpl
	
	align 16
hfr64cw270lop:
	mov r11,r10 ;srcHeight
	mov rsi,rcx ;srcPtr
	
	align 16
hfr64cw270lop2:
	mov eax,dword [rsi]
	mov dword [rdx],eax
	add rsi,[rsp+48] ;sbpl
	lea rdx,[rdx+8]
	dec r11
	jnz hfr64cw270lop2
	
	lea rcx,[rcx+8] ;srcPtr
	add rdx,[rsp+56] ;dbpl
	dec r8
	jnz hfr64cw270lop
	pop rsi
	ret

;void ImageUtil_CopyShiftW(UInt8 *srcPtr, UInt8 *destPtr, OSInt byteSize, OSInt shiftCnt); //Assume aligned
;0 retAddr
;rcx srcPtr
;rdx destPtr
;r8 byteSize
;r9 shiftCnt
	align 16
ImageUtil_CopyShiftW:
	shr r8,4
	cmp r9,6
	jz cswlop6
	cmp r9,4
	jz cswlop4
	cmp r9,5
	jz cswlop5
	cmp r9,3
	jz cswlop3
	cmp r9,2
	jz cswlop2
	cmp r9,1
	jz cswlop1
	jmp cswlop0
	
	align 16
cswlop0:
	movdqa xmm0,[rcx]
	movntdq [rdx],xmm0
	lea rcx,[rcx+16]
	lea rdx,[rdx+16]
	dec r8
	jnz cswlop0
	ret
	
	align 16
cswlop6:
	movdqa xmm0,[rcx]
	psllw xmm0,6
	movntdq [rdx],xmm0
	lea rcx,[rcx+16]
	lea rdx,[rdx+16]
	dec r8
	jnz cswlop6
	ret

	align 16
cswlop5:
	movdqa xmm0,[rcx]
	psllw xmm0,5
	movntdq [rdx],xmm0
	lea rcx,[rcx+16]
	lea rdx,[rdx+16]
	dec r8
	jnz cswlop5
	ret

	align 16
cswlop4:
	movdqa xmm0,[rcx]
	psllw xmm0,4
	movntdq [rdx],xmm0
	lea rcx,[rcx+16]
	lea rdx,[rdx+16]
	dec r8
	jnz cswlop4
	ret

	align 16
cswlop3:
	movdqa xmm0,[rcx]
	psllw xmm0,3
	movntdq [rdx],xmm0
	lea rcx,[rcx+16]
	lea rdx,[rdx+16]
	dec r8
	jnz cswlop3
	ret

	align 16
cswlop2:
	movdqa xmm0,[rcx]
	psllw xmm0,2
	movntdq [rdx],xmm0
	lea rcx,[rcx+16]
	lea rdx,[rdx+16]
	dec r8
	jnz cswlop2
	ret

	align 16
cswlop1:
	movdqa xmm0,[rcx]
	psllw xmm0,1
	movntdq [rdx],xmm0
	lea rcx,[rcx+16]
	lea rdx,[rdx+16]
	dec r8
	jnz cswlop1
	ret

;void ImageUtil_UVInterleaveShiftW(UInt8 *destPtr, UInt8 *uptr, UInt8 *vptr, OSInt pixelCnt, OSInt shiftCnt);
;0 retAddr
;rcx destPtr
;rdx uptr
;r8 vptr
;r9 pixelCnt
;40 shiftCnt
	align 16
ImageUtil_UVInterleaveShiftW:
	shr r9,3
	mov rax,r8
	cmp qword [rsp+40],6
	jz uviswlop6
	cmp qword [rsp+40],4
	jz uviswlop4
	jmp uviswlop0
	
	align 16
uviswlop6:
	movdqa xmm0,[rdx]
	psllw xmm0,6
	movdqa xmm1,[rax]
	psllw xmm1,6
	movdqa xmm2,xmm0
	punpcklwd xmm0,xmm1
	movntdq [rcx],xmm0
	punpckhwd xmm2,xmm1
	movntdq [rcx+16],xmm2
	lea rcx,[rcx+32]
	lea rax,[rax+16]
	lea rdx,[rdx+16]
	dec r9
	jnz uviswlop6
	ret

	align 16
uviswlop4:
	movdqa xmm0,[rdx]
	psllw xmm0,4
	movdqa xmm1,[rax]
	psllw xmm1,4
	movdqa xmm2,xmm0
	punpcklwd xmm0,xmm1
	movntdq [rcx],xmm0
	punpckhwd xmm2,xmm1
	movntdq [rcx+16],xmm2
	lea rcx,[rcx+32]
	lea rax,[rax+16]
	lea rdx,[rdx+16]
	dec r9
	jnz uviswlop4
	ret

	align 16
uviswlop0:
	movdqa xmm0,[rdx]
	movdqa xmm2,xmm0
	movdqa xmm1,[rax]
	punpcklwd xmm0,xmm1
	movntdq [rcx],xmm0
	punpckhwd xmm2,xmm1
	movntdq [rcx+16],xmm2
	lea rcx,[rcx+32]
	lea rax,[rax+16]
	lea rdx,[rdx+16]
	dec r9
	jnz uviswlop0
	ret

;void ImageUtil_YUV_Y416ShiftW(UInt8 *destPtr, UInt8 *yptr, UInt8 *uptr, UInt8 *vptr, OSInt pixelCnt, OSInt shiftCnt);
;0 rbx
;8 rdi
;16 rsi
;24 retAddr
;rcx destPtr
;rdx yptr
;r8 uptr
;r9 vptr
;64 pixelCnt
;72 shiftCnt
	align 16
ImageUtil_YUV_Y416ShiftW:
	push rsi
	push rdi
	push rbx
	mov rax,-1
	mov rdi,rcx ;destPtr
	mov rsi,rdx ;yptr
	mov rbx,qword [rsp+64] ;pixelCnt
	mov rcx,qword [rsp+72] ;shiftCnt
	shr rbx,3
	movd xmm3,eax
	punpckldq xmm3,xmm3
	punpckldq xmm3,xmm3
	cmp rcx,6
	jz yuvy416swlop6
	jmp yuvy416swlop0
	
	align 16
yuvy416swlop6:
	movdqu xmm0,[rsi]
	movdqu xmm1,[r8]
	psllw xmm0,6
	movdqu xmm2,[r9]
	psllw xmm1,6
	psllw xmm2,6
	movdqa xmm4,xmm1
	movdqa xmm5,xmm0
	punpcklwd xmm4,xmm2
	punpckhwd xmm1,xmm2
	punpcklwd xmm5,xmm3
	punpckhwd xmm0,xmm3
	movdqa xmm2,xmm4
	punpcklwd xmm4,xmm5
	movntdq [rdi],xmm4
	punpckhwd xmm2,xmm5
	movntdq [rdi+16],xmm2
	movdqa xmm2,xmm1
	punpcklwd xmm1,xmm0
	movntdq [rdi+32],xmm1
	punpckhwd xmm2,xmm0
	movntdq [rdi+48],xmm2
	lea rdi,[rdi+64]
	lea rsi,[rsi+16]
	lea r8,[r8+16]
	lea r9,[r9+16]
	dec rbx
	jnz yuvy416swlop6
	jmp yuvy416swexit
	
	align 16
yuvy416swlop0:
	movdqu xmm0,[rsi]
	movdqu xmm1,[r8]
	movdqa xmm5,xmm0
	movdqu xmm2,[r9]
	movdqa xmm4,xmm1
	punpcklwd xmm4,xmm2
	punpckhwd xmm1,xmm2
	punpcklwd xmm5,xmm3
	punpckhwd xmm0,xmm3
	movdqa xmm2,xmm4
	punpcklwd xmm4,xmm5
	movntdq [rdi],xmm4
	punpckhwd xmm2,xmm5
	movntdq [rdi+16],xmm2
	movdqa xmm2,xmm1
	punpcklwd xmm1,xmm0
	movntdq [rdi+32],xmm1
	punpckhwd xmm2,xmm0
	movntdq [rdi+48],xmm2
	lea rdi,[rdi+64]
	lea rsi,[rsi+16]
	lea r8,[r8+16]
	lea r9,[r9+16]
	dec rbx
	jnz yuvy416swlop0
	jmp yuvy416swexit
	
	align 16
yuvy416swexit:
	pop rbx
	pop rdi
	pop rsi
	ret

