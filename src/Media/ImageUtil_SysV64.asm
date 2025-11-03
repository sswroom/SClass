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

global ImageUtil_SwapRGB
global _ImageUtil_SwapRGB
global ImageUtil_ColorReplace32
global _ImageUtil_ColorReplace32
global ImageUtil_ColorReplace32A
global _ImageUtil_ColorReplace32A
global ImageUtil_ColorReplace32A2
global _ImageUtil_ColorReplace32A2
global ImageUtil_ColorFill32
global _ImageUtil_ColorFill32
global ImageUtil_ImageColorReplace32
global _ImageUtil_ImageColorReplace32
global ImageUtil_ImageMaskABlend32
global _ImageUtil_ImageMaskABlend32
global ImageUtil_ImageMask2ABlend32
global _ImageUtil_ImageMask2ABlend32
global ImageUtil_ImageColorBuffer32
global _ImageUtil_ImageColorBuffer32
global ImageUtil_ImageColorFill32
global _ImageUtil_ImageColorFill32
global ImageUtil_ImageColorBlend32
global _ImageUtil_ImageColorBlend32
global ImageUtil_ImageFillAlpha32
global _ImageUtil_ImageFillAlpha32
global ImageUtil_ImageAlphaMul32
global _ImageUtil_ImageAlphaMul32
global ImageUtil_ImageColorMul32
global _ImageUtil_ImageColorMul32
global ImageUtil_DrawRectNA32
global _ImageUtil_DrawRectNA32
global ImageUtil_ConvP1_B8G8R8A8
global _ImageUtil_ConvP1_B8G8R8A8
global ImageUtil_ConvP2_B8G8R8A8
global _ImageUtil_ConvP2_B8G8R8A8
global ImageUtil_ConvP4_B8G8R8A8
global _ImageUtil_ConvP4_B8G8R8A8
global ImageUtil_ConvP8_B8G8R8A8
global _ImageUtil_ConvP8_B8G8R8A8
global ImageUtil_ConvP1_A1_B8G8R8A8
global _ImageUtil_ConvP1_A1_B8G8R8A8
global ImageUtil_ConvP2_A1_B8G8R8A8
global _ImageUtil_ConvP2_A1_B8G8R8A8
global ImageUtil_ConvP4_A1_B8G8R8A8
global _ImageUtil_ConvP4_A1_B8G8R8A8
global ImageUtil_ConvP8_A1_B8G8R8A8
global _ImageUtil_ConvP8_A1_B8G8R8A8
global ImageUtil_ConvB5G5R5_B8G8R8A8
global _ImageUtil_ConvB5G5R5_B8G8R8A8
global ImageUtil_ConvB5G6R5_B8G8R8A8
global _ImageUtil_ConvB5G6R5_B8G8R8A8
global ImageUtil_ConvB8G8R8_B8G8R8A8
global _ImageUtil_ConvB8G8R8_B8G8R8A8
global ImageUtil_ConvR8G8B8_B8G8R8A8
global _ImageUtil_ConvR8G8B8_B8G8R8A8
global ImageUtil_ConvB8G8R8A8_B8G8R8
global _ImageUtil_ConvB8G8R8A8_B8G8R8
global ImageUtil_ConvR8G8B8A8_B8G8R8A8
global _ImageUtil_ConvR8G8B8A8_B8G8R8A8
global ImageUtil_ConvR8G8B8N8_B8G8R8A8
global _ImageUtil_ConvR8G8B8N8_B8G8R8A8
global ImageUtil_ConvB16G16R16_B8G8R8A8
global _ImageUtil_ConvB16G16R16_B8G8R8A8
global ImageUtil_ConvR16G16B16_B8G8R8A8
global _ImageUtil_ConvR16G16B16_B8G8R8A8
global ImageUtil_ConvB16G16R16A16_B8G8R8A8
global _ImageUtil_ConvB16G16R16A16_B8G8R8A8
global ImageUtil_ConvR16G16B16A16_B8G8R8A8
global _ImageUtil_ConvR16G16B16A16_B8G8R8A8
global ImageUtil_ConvA2B10G10R10_B8G8R8A8
global _ImageUtil_ConvA2B10G10R10_B8G8R8A8
global ImageUtil_ConvFB32G32R32A32_B8G8R8A8
global _ImageUtil_ConvFB32G32R32A32_B8G8R8A8
global ImageUtil_ConvFR32G32B32A32_B8G8R8A8
global _ImageUtil_ConvFR32G32B32A32_B8G8R8A8
global ImageUtil_ConvFB32G32R32_B8G8R8A8
global _ImageUtil_ConvFB32G32R32_B8G8R8A8
global ImageUtil_ConvFR32G32B32_B8G8R8A8
global _ImageUtil_ConvFR32G32B32_B8G8R8A8
global ImageUtil_ConvFW32A32_B8G8R8A8
global _ImageUtil_ConvFW32A32_B8G8R8A8
global ImageUtil_ConvFW32_B8G8R8A8
global _ImageUtil_ConvFW32_B8G8R8A8
global ImageUtil_ConvP1_B16G16R16A16
global _ImageUtil_ConvP1_B16G16R16A16
global ImageUtil_ConvP2_B16G16R16A16
global _ImageUtil_ConvP2_B16G16R16A16
global ImageUtil_ConvP4_B16G16R16A16
global _ImageUtil_ConvP4_B16G16R16A16
global ImageUtil_ConvP8_B16G16R16A16
global _ImageUtil_ConvP8_B16G16R16A16
global ImageUtil_ConvP1_A1_B16G16R16A16
global _ImageUtil_ConvP1_A1_B16G16R16A16
global ImageUtil_ConvP2_A1_B16G16R16A16
global _ImageUtil_ConvP2_A1_B16G16R16A16
global ImageUtil_ConvP4_A1_B16G16R16A16
global _ImageUtil_ConvP4_A1_B16G16R16A16
global ImageUtil_ConvP8_A1_B16G16R16A16
global _ImageUtil_ConvP8_A1_B16G16R16A16
global ImageUtil_ConvB5G5R5_B16G16R16A16
global _ImageUtil_ConvB5G5R5_B16G16R16A16
global ImageUtil_ConvB5G6R5_B16G16R16A16
global _ImageUtil_ConvB5G6R5_B16G16R16A16
global ImageUtil_ConvB8G8R8_B16G16R16A16
global _ImageUtil_ConvB8G8R8_B16G16R16A16
global ImageUtil_ConvR8G8B8_B16G16R16A16
global _ImageUtil_ConvR8G8B8_B16G16R16A16
global ImageUtil_ConvR8G8B8A8_B16G16R16A16
global _ImageUtil_ConvR8G8B8A8_B16G16R16A16
global ImageUtil_ConvB8G8R8A8_B16G16R16A16
global _ImageUtil_ConvB8G8R8A8_B16G16R16A16
global ImageUtil_ConvR8G8B8A8_B16G16R16A16
global _ImageUtil_ConvR8G8B8A8_B16G16R16A16
global ImageUtil_ConvB16G16R16_B16G16R16A16
global _ImageUtil_ConvB16G16R16_B16G16R16A16
global ImageUtil_ConvR16G16B16_B16G16R16A16
global _ImageUtil_ConvR16G16B16_B16G16R16A16
global ImageUtil_ConvR16G16B16A16_B16G16R16A16
global _ImageUtil_ConvR16G16B16A16_B16G16R16A16
global ImageUtil_ConvA2B10G10R10_B16G16R16A16
global _ImageUtil_ConvA2B10G10R10_B16G16R16A16
global ImageUtil_ConvFB32G32R32A32_B16G16R16A16
global _ImageUtil_ConvFB32G32R32A32_B16G16R16A16
global ImageUtil_ConvFR32G32B32A32_B16G16R16A16
global _ImageUtil_ConvFR32G32B32A32_B16G16R16A16
global ImageUtil_ConvFB32G32R32_B16G16R16A16
global _ImageUtil_ConvFB32G32R32_B16G16R16A16
global ImageUtil_ConvFR32G32B32_B16G16R16A16
global _ImageUtil_ConvFR32G32B32_B16G16R16A16
global ImageUtil_ConvFW32A32_B16G16R16A16
global _ImageUtil_ConvFW32A32_B16G16R16A16
global ImageUtil_ConvFW32_B16G16R16A16
global _ImageUtil_ConvFW32_B16G16R16A16
global ImageUtil_ConvW16_B8G8R8A8
global _ImageUtil_ConvW16_B8G8R8A8
global ImageUtil_ConvW16A16_B8G8R8A8
global _ImageUtil_ConvW16A16_B8G8R8A8
global ImageUtil_ConvW8A8_B8G8R8A8
global _ImageUtil_ConvW8A8_B8G8R8A8
global ImageUtil_ConvW16_B16G16R16A16
global _ImageUtil_ConvW16_B16G16R16A16
global ImageUtil_ConvW16A16_B16G16R16A16
global _ImageUtil_ConvW16A16_B16G16R16A16
global ImageUtil_ConvW8A8_B16G16R16A16
global _ImageUtil_ConvW8A8_B16G16R16A16
global ImageUtil_ConvP1_P8
global _ImageUtil_ConvP1_P8
global ImageUtil_Rotate32_CW90
global _ImageUtil_Rotate32_CW90
global ImageUtil_Rotate32_CW180
global _ImageUtil_Rotate32_CW180
global ImageUtil_Rotate32_CW270
global _ImageUtil_Rotate32_CW270
global ImageUtil_Rotate64_CW90
global _ImageUtil_Rotate64_CW90
global ImageUtil_Rotate64_CW180
global _ImageUtil_Rotate64_CW180
global ImageUtil_Rotate64_CW270
global _ImageUtil_Rotate64_CW270
global ImageUtil_HFlip32
global _ImageUtil_HFlip32
global ImageUtil_HFRotate32_CW90
global _ImageUtil_HFRotate32_CW90
global ImageUtil_HFRotate32_CW180
global _ImageUtil_HFRotate32_CW180
global ImageUtil_HFRotate32_CW270
global _ImageUtil_HFRotate32_CW270
global ImageUtil_HFlip64
global _ImageUtil_HFlip64
global ImageUtil_HFRotate64_CW90
global _ImageUtil_HFRotate64_CW90
global ImageUtil_HFRotate64_CW180
global _ImageUtil_HFRotate64_CW180
global ImageUtil_HFRotate64_CW270
global _ImageUtil_HFRotate64_CW270
global ImageUtil_CopyShiftW
global _ImageUtil_CopyShiftW
global ImageUtil_UVInterleaveShiftW
global _ImageUtil_UVInterleaveShiftW
global ImageUtil_YUV_Y416ShiftW
global _ImageUtil_YUV_Y416ShiftW

extern _UseSSE42

;void ImageUtil_SwapRGB(UInt8 *imgPtr, OSInt pixelCnt, OSInt bpp)
;0 retAddr
;rdi imgPtr
;rsi pixelCnt
;rdx bpp
	align 16
ImageUtil_SwapRGB:
_ImageUtil_SwapRGB:
	cmp rdx,48
	jz srgb48start
	cmp rdx,64
	jz srgb64start
	cmp rdx,24
	jz srgb24start
	cmp rdx,32
	jz srgb32start
	jmp srgbexit

	align 16
srgb48start:
	mov ax,word [rdi]
	mov dx,word [rdi+4]
	mov word [rdi],dx
	mov word [rdi+4],ax
	lea rdi,[rdi+6]
	dec rsi
	jnz srgb48start
	jmp srgbexit
	
	align 16
srgb64start:
	mov ax,word [rdi]
	mov dx,word [rdi+4]
	mov word [rdi],dx
	mov word [rdi+4],ax
	lea rdi,[rdi+8]
	dec rsi
	jnz srgb64start
	jmp srgbexit
	
	align 16
srgb24start:
	mov al,byte [rdi]
	mov dl,byte [rdi+2]
	mov byte [rdi],dl
	mov byte [rdi+2],al
	lea rdi,[rdi+3]
	dec rsi
	jnz srgb24start
	jmp srgbexit

	align 16
srgb32start:
	mov al,byte [rdi]
	mov dl,byte [rdi+2]
	mov byte [rdi],dl
	mov byte [rdi+2],al
	lea rdi,[rdi+4]
	dec rsi
	jnz srgb32start
	jmp srgbexit

	align 16
srgbexit:
	ret
	
;void ImageUtil_ColorReplace32(UInt8 *pixelPtr, OSInt w, OSInt h, Int32 col);
;0 retAddr
;rdi pixelPtr
;rsi w
;rdx h
;rcx col

	align 16
ImageUtil_ColorReplace32:
_ImageUtil_ColorReplace32:
	mov rax,rsi ;w
	mul rdx ;h
	mov rdx,rax
	shr rdx,4
	jz cr32lop1
	movd xmm1,ecx
	punpckldq xmm1,xmm1
	punpckldq xmm1,xmm1
	pcmpeqd xmm2,xmm2
	pxor xmm3,xmm3
	align 16
cr32lop2:
	movups xmm0,[rdi]
	pcmpeqd xmm0,xmm3
	pxor xmm0,xmm2
	pand xmm0,xmm1
	movups [rdi],xmm0
	movups xmm0,[rdi+16]
	pcmpeqd xmm0,xmm3
	pxor xmm0,xmm2
	pand xmm0,xmm1
	movups [rdi+16],xmm0
	movups xmm0,[rdi+32]
	pcmpeqd xmm0,xmm3
	pxor xmm0,xmm2
	pand xmm0,xmm1
	movups [rdi+32],xmm0
	movups xmm0,[rdi+48]
	pcmpeqd xmm0,xmm3
	pxor xmm0,xmm2
	pand xmm0,xmm1
	movups [rdi+48],xmm0
	add rdi,64
	dec rdx
	jnz cr32lop2
	and rax,15
	jnz cr32lop1
	ret

	align 16
cr32lop1:
	mov rdx,-1
	align 16
clop:
	test dword [rdi], edx
	jz clop2
	mov dword [rdi],ecx
	align 16
clop2:
	add rdi,4
	dec rax
	jnz clop
	ret

;void ImageUtil_ColorReplace32A(UInt8 *pixelPtr, OSInt w, OSInt h, Int32 col);
;0 retAddr
;rdi pixelPtr
;rsi w
;rdx h
;rcx col
	align 16
ImageUtil_ColorReplace32A:
_ImageUtil_ColorReplace32A:
	movd xmm4,ecx
	mov rax,rsi ;w
	mul rdx ;h
	mov rdx,rax
	pxor xmm5,xmm5
	punpckldq xmm4,xmm4
	punpcklbw xmm4,xmm4
	shr rcx,1
	jnb dslop1
	movd xmm0,dword [rdi]
	punpcklbw xmm0,xmm0
	punpcklbw xmm0,xmm5
	pmulhuw xmm0,xmm4
	packuswb xmm0,xmm5
	movd dword [rdi],xmm0
	lea rdi,[rdi+4]
	align 16
dslop1:
	movq xmm0,[rdi]
	punpcklbw xmm0,xmm5
	pmulhuw xmm0,xmm4
	packuswb xmm0,xmm5
	movq [rdi],xmm0
	lea rdi,[rdi+8]
	dec rdx
	jnz dslop1
	ret

;void ImageUtil_ColorReplace32A2(UInt8 *pixelPtr, OSInt w, OSInt h, Int32 col);
;0 retAddr
;rdi pixelPtr
;rsi w
;rdx h
;rcx col
	align 16
ImageUtil_ColorReplace32A2:
_ImageUtil_ColorReplace32A2:
	movd xmm2,ecx ;col
	mov rax,rsi
	mul rdx
	pxor xmm1,xmm1

	mov rcx,rax ;pxCnt
	punpcklbw xmm2,xmm1
	align 16
cr32a2lop2a:
	mov eax,dword [rdi]
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
	movd [rdi],xmm0

	align 16
cr32a2lop2b:
	lea rdi,[rdi+4]
	dec rcx
	jnz cr32a2lop2a
	ret

;void ImageUtil_ColorFill32(UInt8 *pixelPtr, OSInt pixelCnt, Int32 color);
;0 retAddr
;rdi pixelPtr
;rsi pixelCnt
;rdx color
	align 16
ImageUtil_ColorFill32:
_ImageUtil_ColorFill32:
	mov rcx,rsi
	shr rcx,4
	jz cf32lop2
	movd xmm0,edx
	punpckldq xmm0,xmm0
	punpckldq xmm0,xmm0
	align 16
cf32lop:
	movups [rdi],xmm0
	movups [rdi+16],xmm0
	movups [rdi+32],xmm0
	movups [rdi+48],xmm0
	add rdi,64
	dec rcx
	jnz cf32lop
	and rsi,15
	jnz cf32lop2
	ret

	align 16
cf32lop2:
	mov rax,rdx ;color
	cld	
	mov rcx,rsi ;pixelCnt
	and rcx,63
	rep stosd
	ret

;void ImageUtil_ImageColorReplace32(UInt8 *srcPtr, UInt8 *destPtr, OSInt w, OSInt h, OSInt sbpl, OSInt dbpl, Int32 col)
;0 retAddr
;rdi srcPtr
;rsi destPtr
;rdx w
;rcx h
;r8 sbpl
;r9 dbpl
;8 col
	align 16
ImageUtil_ImageColorReplace32:
_ImageUtil_ImageColorReplace32:
	lea rax,[rdx * 4] ;w
	mov r10,rcx ;h
	sub r8,rax ;sbpl
	sub r9,rax ;dbpl

	mov eax,dword [rsp+8] ;col
	align 16
icr32lop10:
	mov rcx,rdx ;w
	align 16
icr32lop11:
	cmp dword [rdi],0
	jz icr32lop12
	mov dword [rsi],eax
icr32lop12:
	lea rsi,[rsi+4]
	lea rdi,[rdi+4]
	dec rcx
	jnz icr32lop11

	add rdi,r8 ;sbpl
	add rsi,r9 ;dbpl
	dec r10 ;h
	jnz icr32lop10
	ret
	
;void ImageUtil_ImageMaskABlend32(UInt8 *maskPtr, UInt8 *destPtr, OSInt w, OSInt h, OSInt sbpl, OSInt dbpl, Int32 col);
;0 retAddr
;rdi maskPtr
;rsi destPtr
;rdx w
;rcx h
;r8 sbpl
;r9 dbpl
;8 col
	align 16
ImageUtil_ImageMaskABlend32:
_ImageUtil_ImageMaskABlend32:
	lea rax,[rdx*4] ;w
	mov r10,rcx ;h
	mov r11,rdx ;w
	sub r8,rax ;sbpl
	sub r9,rax ;dbpl

	mov eax,dword [rsp+8] ;col
	mov edx,eax
	mov ecx,0xff
	shr edx,24
	sub ecx,edx
	and eax,0xffffff
	movd xmm4,edx
	movd xmm5,ecx
	movd xmm2,eax
	mov eax,0xff000000
	and eax,dword [rsp+8] ;col
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
	mov rdx,r11 ;w
	align 16
imab32lop11a:
	cmp dword [rdi],0
	jz imab32lop12a
	movd xmm0,dword [rsi]
	punpcklbw xmm0,xmm0
	pmulhuw xmm0,xmm5
	paddusw xmm0,xmm4
	psrlw xmm0,8
	packuswb xmm0,xmm3
	movd dword [rsi],xmm0
	align 16
imab32lop12a:
	lea rsi,[rsi+4]
	lea rdi,[rdi+4]
	dec rdx
	jnz imab32lop11a

	add rdi,r8 ;sbpl
	add rsi,r9 ;dbpl
	dec r10 ;h
	jnz imab32lop10a
	ret

;void ImageUtil_ImageMask2ABlend32(UInt8 *maskPtr, UInt8 *destPtr, OSInt w, OSInt h, OSInt sbpl, OSInt dbpl, Int32 col1, Int32 col2)
;0 retAddr
;rdi maskPtr
;rsi destPtr
;rdx w
;rcx h
;r8 sbpl
;r9 dbpl
;8 col1
;16 col2
	align 16
ImageUtil_ImageMask2ABlend32:
_ImageUtil_ImageMask2ABlend32:
	xchg rsi,rdi
	lea rax,[rdx*4] ;w
	mov r10,rdx ;w
	mov r11,rcx ;h
	sub r8,rax ;sbpl
	sub r9,rax ;dbpl

	mov edx,0xff000000
	mov ecx,0xffffffff
	movd xmm3,edx
	movd xmm4,ecx
	punpcklbw xmm3,xmm3 ;mortmp
	punpcklbw xmm4,xmm4 ;mtmp

	movzx eax,byte [rsp+11] ;col1[3]
	movd xmm1,eax
	punpcklbw xmm1,xmm1
	punpcklbw xmm1,xmm1
	punpcklbw xmm1,xmm1
		
	movq xmm2,xmm1
	pxor xmm1,xmm4 ;mtmp
	movq xmm6,xmm1 ;cmul1
	por xmm2,xmm3 ;mortmp

	movd xmm7,[rsp+8] ;col1
	punpcklbw xmm7,xmm7
	pmulhuw xmm7,xmm2 ;cadd1

	movzx eax,byte [rsp+19] ;col2[3]
	movd xmm1,eax
	punpcklbw xmm1,xmm1
	punpcklbw xmm1,xmm1
	punpcklbw xmm1,xmm1
	
	movq xmm2,xmm1
	pxor xmm4,xmm1 ;mtmp cmul2
	por xmm2,xmm3 ;mortmp
	
	movd xmm5,[rsp+16] ;col2
	punpcklbw xmm5,xmm5
	pmulhuw xmm5,xmm2 ;cadd2
	
	cld

	align 16
im2ab32lop3:
	mov rcx,r10 ;w
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
	lea rdi,[rdi+4]
	dec rcx
	jnz im2ab32lop
	
	add rsi,r8 ;sbpl
	add rdi,r9 ;dbpl
	dec r11 ;h
	jnz im2ab32lop3
	
	ret
	
;void ImageUtil_ImageColorBuffer32(UInt8 *pixelPtr, OSInt w, OSInt h, OSInt bpl, OSInt buffSize)
;0 bpl
;8 rbx
;16 rbp
;24 retAddr
;rdi pixelPtr
;rsi w r11
;rdx h
;rcx bpl
;r8 buffSize
	align 16
ImageUtil_ImageColorBuffer32:
_ImageUtil_ImageColorBuffer32:
	push rbp
	push rbx
	push rcx
	mov r11,rsi
	mov rbx,rcx ;bpl
	mov r10,rcx ;bpl
	lea rax,[r11*4] ;w
	mov r9,rdx ;h
	sub rbx,rax ;bpl
	mov r10,rbx ;bpl - w * 4
	mov rbx,r8 ;buffSize
	
	align 16
icb32lop:
	mov rcx,r11 ;w
	
	align 16
icb32lop2:
	test dword [rdi],0x808080
	jz icb32lop3
	mov rbp,rdi
	mov rsi,rdi
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
	cmp dword [rsi+rdx*4],0
	jnz icb32lop6c
	mov dword [rsi+rdx*4],0x7f7f7f
icb32lop6c:
	neg rdx
	cmp dword [rbp+rdx*4],0
	jnz icb32lop6b
	mov dword [rbp+rdx*4],0x7f7f7f
icb32lop6b:
	cmp dword [rsi+rdx*4],0
	jnz icb32lop6d
	mov dword [rsi+rdx*4],0x7f7f7f
icb32lop6d:
	neg rdx
	jnb icb32lop5
	dec rdx
	jmp icb32lop7
	
	align 16
icb32lop5:
	inc rax
	add rbp,qword [rsp] ;bpl
	sub rsi,qword [rsp] ;bpl
	cmp rax,rbx ;buffSize
	jbe icb32lop4

	align 16
icb32lop3:
	lea rdi,[rdi+4]
	dec rcx
	jnz icb32lop2
	add rdi,r10 ;bpl - w * 4
	dec r9 ;h
	jnz icb32lop
	
	pop rcx
	pop rbx
	pop rbp
	ret

;void ImageUtil_ImageColorFill32(UInt8 *pixelPtr, OSInt w, OSInt h, OSInt bpl, Int32 col)
;0 retAddr
;rdi pixelPtr
;rsi w
;rdx h
;rcx bpl
;r8 col
	align 16
ImageUtil_ImageColorFill32:
_ImageUtil_ImageColorFill32:
	cld
	lea rax,[rsi*4] ;w
	sub rcx,rax ;bpl
	mov eax,r8d ;col
	mov r9,rcx ;bpl
	align 16
icf32lop4:
	mov rcx,rsi
	rep stosd
	add rdi,r9
	dec rdx
	jnz icf32lop4
	ret

;void ImageUtil_ImageColorBlend32(UInt8 *pixelPtr, OSInt w, OSInt h, OSInt bpl, Int32 col)
;0 retAddr
;rdi pixelPtr
;rsi w
;rdx h
;rcx bpl
;r8 col
	align 16
ImageUtil_ImageColorBlend32:
_ImageUtil_ImageColorBlend32:
	mov r10,rdx ;h
	mov r11,rcx ;bpl
	mov eax,r8d ;col
	shr eax,24
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

	movd xmm0,r8d
	punpcklbw xmm0,xmm0
	pmulhuw xmm0,xmm2
	movq xmm6,xmm0 ;cadd
	pxor xmm3,xmm3

	lea rax,[rsi*4] ;w
	sub r11,rax ;bpl
	mov rdx,rdi ;initOfst
	align 16
icbl32lop:
	mov rcx,rsi ;w2
	shr rcx,1
	align 16
icbl32lop2:
	movd xmm0,[rdx]
	movd xmm1,[rdx+4]
	punpcklbw xmm0,xmm0
	punpcklbw xmm1,xmm1

	pmulhuw xmm0,xmm5 ;cimul
	paddusw xmm0,xmm6 ;cadd
	psrlw xmm0,8
	pmulhuw xmm1,xmm5 ;cimul
	paddusw xmm1,xmm6 ;cadd
	psrlw xmm1,8
	packuswb xmm0,xmm1
	movq [rdx],xmm0

	lea rdx,[rdx+8]
	dec rcx
	jnz icbl32lop2

	test rsi,1 ;w
	jz icbl32lop3

	movd xmm0,[rdx]
	punpcklbw xmm0,xmm0

	pmulhuw xmm0,xmm5 ;cimul
	paddusw xmm0,xmm6 ;cadd
	psrlw xmm0,8
	packuswb xmm0,xmm3
	movd [rdx],xmm0
	
	align 16
icbl32lop3:
	add rdx,r11 ;bpl
	dec r10 ;hleft
	jnz icbl32lop
	ret
	
;void ImageUtil_ImageFillAlpha32(UInt8 *pixelPtr, OSInt w, OSInt h, OSInt bpl, UInt8 a);
;0 retAddr
;rdi pixelPtr
;rsi w
;rdx h
;rcx bpl
;r8 a

	align 16
ImageUtil_ImageFillAlpha32:
_ImageUtil_ImageFillAlpha32:
	lea rax,[rsi*4] ;w
	mov r9,rcx ;bpl
	sub r9,rax ;bpl
	mov al,r8b ;a
;	mov ecx,0xff000000
;	movd xmm1,ecx
;	mov cl,al
;	shl cl,24
;	movd xmm0,ecx
;	punpckldq xmm0,xmm0
;	punpckldq xmm0,xmm0
;	punpckldq xmm1,xmm1
;	punpckldq xmm1,xmm1
	test rsi,7
	jnz ifa32lop
	shr rsi,3
	align 16
ifa32lop4:
	mov rcx,rsi ;w >> 2
	align 16
ifa32lop4b:
	mov byte [rdi+3],al
	mov byte [rdi+7],al
	mov byte [rdi+11],al
	mov byte [rdi+15],al
	mov byte [rdi+19],al
	mov byte [rdi+23],al
	mov byte [rdi+27],al
	mov byte [rdi+31],al
	add rdi,32
;	maskmovdqu xmm0,xmm1
;	add rdi,16
;	maskmovdqu xmm0,xmm1
;	add rdi,16
	dec rcx
	jnz ifa32lop4b
	add rdi,r9 ;bpl
	dec rdx ;h
	jnz ifa32lop4
	ret

	align 16
ifa32lop:
	mov rcx,rsi ;w
	shr rcx,3
	jz ifa32lop2b
	align 16
ifa32lop2:
	mov byte [rdi+3],al
	mov byte [rdi+7],al
	mov byte [rdi+11],al
	mov byte [rdi+15],al
	mov byte [rdi+19],al
	mov byte [rdi+23],al
	mov byte [rdi+27],al
	mov byte [rdi+31],al
	add rdi,32
;	maskmovdqu xmm0,xmm1
;	add rdi,16
;	maskmovdqu xmm0,xmm1
;	add rdi,16
	dec rcx
	jnz ifa32lop2
	align 16
ifa32lop2b:
	mov rcx,rsi
	and rcx,7
	align 16
ifa32lop2c:
	mov	byte[rdi+3],al
	add rdi,4
	dec rcx
	jnz ifa32lop2c

	add rdi,r9 ;bpl
	dec rdx ;h
	jnz ifa32lop
	ret


;void ImageUtil_ImageAlphaMul32(UInt8 *pixelPtr, OSInt w, OSInt h, OSInt bpl, UInt32 a);
;0 retAddr
;rdi pixelPtr
;rsi w
;rdx h
;rcx bpl
;r8 a

	align 16
ImageUtil_ImageAlphaMul32:
_ImageUtil_ImageAlphaMul32:
	lea rax,[rsi*4] ;w
	mov r9,rcx ;bpl
	mov r10,rdx ;h
	sub r9,rax ;bpl
	align 16
iam32lop2:
	mov rcx,rsi ;w
	
	align 16
iam32lop:
	movzx eax,byte [rdi+3]
	mul r8d ;v
	shr eax,16
	mov byte [rdi+3],al
	add rdi,4
	dec rcx
	jnz iam32lop
	add rdi,r9 ;bpl
	dec r10 ;h
	jnz iam32lop2
	ret

;void ImageUtil_ImageColorMul32(UInt8 *pixelPtr, OSInt w, OSInt h, OSInt bpl, UInt32 c);
;0 retAddr
;rdi pixelPtr
;rsi w
;rdx h
;rcx bpl
;r8 c

	align 16
ImageUtil_ImageColorMul32:
_ImageUtil_ImageColorMul32:
	lea rax,[rsi*4] ;w
	mov r9,rcx ;bpl
	mov r10,rdx ;h
	sub r9,rax ;bpl
	movd xmm1,r8d ;c
	punpcklbw xmm1,xmm1
	pxor xmm2,xmm2
	align 16
icm32lop2:
	mov rcx,rsi ;w
	
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
	dec r10 ;h
	jnz icm32lop2
	ret

;void ImageUtil_DrawRectNA32(UInt8 *pixelPtr, OSInt w, OSInt h, OSInt bpl, UInt32 col)
;0 retAddr
;rdi pixelPtr
;rsi w
;rdx h
;rcx bpl
;r8 col

	align 16
ImageUtil_DrawRectNA32:
_ImageUtil_DrawRectNA32:
	mov r10,rcx ;bpl
	lea rax,[rsi*4]
	sub r10,rax
	sub rdx,2
	jb drna32exit
	jz drna32lop
	
	mov rax,rsi
	align 16
drna32lop2:
	mov dword [rdi],r8d
	lea rdi,[rdi+4]
	dec rax
	jnz drna32lop2
	lea rdi,[rdi+r10]
	
	align 16
drna32lop3:
	mov dword [rdi],r8d
	mov dword [rdi+rsi*4-4],r8d
	lea rdi,[rdi+rcx]
	dec rdx
	jnz drna32lop3
	
	mov rax,rsi
	align 16
drna32lop4:
	mov dword [rdi],r8d
	lea rdi,[rdi+4]
	dec rax
	jnz drna32lop4
	jmp drna32exit

	align 16
drna32lop:	
	mov rax,rsi
	align 16
drna32lop5:
	mov dword [rdi],r8d
	lea rdi,[rdi+4]
	dec rax
	jnz drna32lop5
	lea rdi,[rdi+r10]

	mov rax,rsi
	align 16
drna32lop6:
	mov dword [rdi],r8d
	lea rdi,[rdi+4]
	dec rax
	jnz drna32lop6
	
	align 16
drna32exit:
	ret

;void ImageUtil_ConvP1_B8G8R8A8(UInt8 *srcPtr, UInt8 *destPtr, OSInt w, OSInt h, OSInt sbpl, OSInt dbpl, UInt8 *pal);
;0 retAddr
;rdi srcPtr
;rsi destPtr
;rdx w
;rcx h
;r8 sbpl
;r9 dbpl
;8 pal
	align 16
ImageUtil_ConvP1_B8G8R8A8:
_ImageUtil_ConvP1_B8G8R8A8:
	mov r10,rcx
	mov rcx,qword [rsp+8] ;pal
	push rbx
	push rbp
	mov ebx,[rcx]
	mov ebp,[rcx+4]
	lea rax,[rdx*4]
	test rdx,7
	jnz cp1_bgra8lop
	
	shr rdx,3
	sub r8,rdx ;sbpl
	sub r9,rax ;dbpl
	align 16
cp1_bgra8lop2:
	mov r11,rdx
	align 16
cp1_bgra8lop3:
	mov al,byte [rdi]
	mov ecx,ebx
	shl al,1
	cmovc ecx,ebp
	movnti dword [rsi],ecx
	mov ecx,ebx
	shl al,1
	cmovc ecx,ebp
	movnti dword [rsi+4],ecx
	mov ecx,ebx
	shl al,1
	cmovc ecx,ebp
	movnti dword [rsi+8],ecx
	mov ecx,ebx
	shl al,1
	cmovc ecx,ebp
	movnti dword [rsi+12],ecx
	mov ecx,ebx
	shl al,1
	cmovc ecx,ebp
	movnti dword [rsi+16],ecx
	mov ecx,ebx
	shl al,1
	cmovc ecx,ebp
	movnti dword [rsi+20],ecx
	mov ecx,ebx
	shl al,1
	cmovc ecx,ebp
	movnti dword [rsi+24],ecx
	mov ecx,ebx
	shl al,1
	cmovc ecx,ebp
	movnti dword [rsi+28],ecx
	add rdi,1
	add rsi,32
	dec r11
	jnz cp1_bgra8lop3
	
	add rdi,r8 ;sbpl
	add rsi,r9 ;dbpl
	dec r10
	jnz cp1_bgra8lop2
	jmp cp1_bgra8exit
	
	align 16
cp1_bgra8lop:
	push r12
	mov r12,rdx
	shr rdx,3
	sub r8,rdx ;sbpl
	sub r9,rax ;dbpl
	mov rdx,rcx
	align 16
cp1_bgra8lop4:
	mov r11,r12
	shr r11,3
	align 16
cp1_bgra8lop5:
	movzx rax,byte [rdi]
	mov ecx,ebx
	shl al,1
	cmovc ecx,ebp
	movnti dword [rsi],ecx
	mov ecx,ebx
	shl al,1
	cmovc ecx,ebp
	movnti dword [rsi+4],ecx
	mov ecx,ebx
	shl al,1
	cmovc ecx,ebp
	movnti dword [rsi+8],ecx
	mov ecx,ebx
	shl al,1
	cmovc ecx,ebp
	movnti dword [rsi+12],ecx
	mov ecx,ebx
	shl al,1
	cmovc ecx,ebp
	movnti dword [rsi+16],ecx
	mov ecx,ebx
	shl al,1
	cmovc ecx,ebp
	movnti dword [rsi+20],ecx
	mov ecx,ebx
	shl al,1
	cmovc ecx,ebp
	movnti dword [rsi+24],ecx
	mov ecx,ebx
	shl al,1
	cmovc ecx,ebp
	movnti dword [rsi+28],ecx
	add rdi,1
	add rsi,32
	dec r11
	jnz cp1_bgra8lop5

	mov r11,r12
	and r11,7
	movzx rax,byte [rdi]
	align 16
cp1_bgra8lop6:
	mov ecx,ebx
	shl al,1
	cmovc ecx,ebp
	movnti dword [rsi],ecx
	add rsi,4
	dec r11
	jnz cp1_bgra8lop6
	
	add rdi,r8 ;sbpl
	add rsi,r9 ;dbpl
	dec r10
	jnz cp1_bgra8lop4
	pop r12

	align 16
cp1_bgra8exit:
	pop rbp
	pop rbx
	ret

;void ImageUtil_ConvP2_B8G8R8A8(UInt8 *srcPtr, UInt8 *destPtr, OSInt w, OSInt h, OSInt sbpl, OSInt dbpl, UInt8 *pal);
;0 retAddr
;rdi srcPtr
;rsi destPtr
;rdx w
;rcx h
;r8 sbpl
;r9 dbpl
;8 pal rsi
	align 16
ImageUtil_ConvP2_B8G8R8A8:
_ImageUtil_ConvP2_B8G8R8A8:
	mov r10,rcx ;h
	mov rcx,rdx
	mov rdx,qword [rsp+8] ;pal
	lea rax,[rcx*4]
	test rcx,3
	jnz cp2_bgra8lop
	
	shr rcx,2
	sub r8,rcx ;sbpl
	sub r9,rax ;dbpl
	align 16
cp2_bgra8lop2:
	mov r11,rcx
	align 16
cp2_bgra8lop3:
	movzx rax,byte [rdi]
	shr rax,6
	and rax,3
	mov eax,dword [rdx+rax*4]
	movnti dword [rsi],eax
	movzx rax,byte [rdi]
	shr rax,4
	and rax,3
	mov eax,dword [rdx+rax*4]
	movnti dword [rsi+4],eax
	movzx rax,byte [rdi]
	shr rax,2
	and rax,3
	mov eax,dword [rdx+rax*4]
	movnti dword [rsi+8],eax
	movzx rax,byte [rdi]
	and rax,3
	mov eax,dword [rdx+rax*4]
	movnti dword [rsi+12],eax
	lea rdi,[rdi+1]
	lea rsi,[rsi+16]
	dec r11
	jnz cp2_bgra8lop3
	
	add rdi,r8 ;sbpl
	add rsi,r9 ;dbpl
	dec r10
	jnz cp2_bgra8lop2
	jmp cp2_bgra8exit
	
	align 16
cp2_bgra8lop:
	push r12
	mov r12,rcx
	shr rcx,2
	sub r8,rcx ;sbpl
	sub r9,rax ;dbpl
	align 16
cp2_bgra8lop4:
	mov r11,r12
	shr r11,2
	align 16
cp2_bgra8lop5:
	movzx rax,byte [rdi]
	rol al,2
	mov rcx,rax
	and rcx,3
	mov ecx,dword [rdx+rcx*4]
	movnti dword [rsi],ecx
	rol al,2
	mov rcx,rax
	and rcx,3
	mov ecx,dword [rdx+rcx*4]
	movnti dword [rsi+4],ecx
	rol al,2
	mov rcx,rax
	and rcx,3
	mov ecx,dword [rdx+rcx*4]
	movnti dword [rsi+8],ecx
	rol al,2
	mov rcx,rax
	and rcx,3
	mov ecx,dword [rdx+rcx*4]
	movnti dword [rsi+12],ecx
	lea rdi,[rdi+1]
	lea rsi,[rsi+16]
	dec r11
	jnz cp2_bgra8lop5

	mov r11,r12
	and r11,3
	movzx rax,byte [rdi]
	align 16
cp2_bgra8lop6:
	rol al,2
	mov rcx,rax
	and rcx,3
	mov ecx,dword [rdx+rcx*4]
	movnti dword [rsi],ecx
	lea rsi,[rsi+4]
	dec r11
	jnz cp2_bgra8lop6
	
	add rdi,r8 ;sbpl
	add rsi,r9 ;dbpl
	dec r10
	jnz cp2_bgra8lop4
	pop r12

	align 16
cp2_bgra8exit:
	ret

;void ImageUtil_ConvP4_B8G8R8A8(UInt8 *srcPtr, UInt8 *destPtr, OSInt w, OSInt h, OSInt sbpl, OSInt dbpl, UInt8 *pal);
;0 retAddr
;rdi srcPtr
;rsi destPtr
;rdx w
;rcx h
;r8 sbpl
;r9 dbpl
;8 pal rsi

	align 16
ImageUtil_ConvP4_B8G8R8A8:
_ImageUtil_ConvP4_B8G8R8A8:
	mov r10,rcx ;h
	mov rcx,rdx
	mov rdx,qword [rsp+8] ;pal
	lea rax,[rcx*4]
	shr rcx,1
	jb cp4_bgra8lop
	
	sub r8,rcx ;sbpl
	sub r9,rax ;dbpl
	align 16
cp4_bgra8lop2:
	mov r11,rcx
	align 16
cp4_bgra8lop3:
	movzx rax,byte [rdi]
	shr rax,4
	mov eax,dword [rdx+rax*4]
	movnti dword [rsi],eax
	movzx rax,byte [rdi]
	and rax,15
	mov eax,dword [rdx+rax*4]
	lea rdi,[rdi+1]
	movnti dword [rsi+4],eax
	lea rsi,[rsi+8]
	dec r11
	jnz cp4_bgra8lop3
	
	add rdi,r8 ;sbpl
	add rsi,r9 ;dbpl
	dec r10
	jnz cp4_bgra8lop2
	jmp cp4_bgra8exit
	
	align 16
cp4_bgra8lop:
	sub r8,rcx ;sbpl
	sub r9,rax ;dbpl
	align 16
cp4_bgra8lop4:
	mov r11,rcx
	align 16
cp4_bgra8lop5:
	movzx rax,byte [rdi]
	shr rax,4
	mov eax,dword [rdx+rax*4]
	movnti dword [rsi],eax
	movzx rax,byte [rdi]
	and rax,15
	mov eax,dword [rdx+rax*4]
	lea rdi,[rdi+1]
	movnti dword [rsi+4],eax
	lea rsi,[rsi+8]
	dec r11
	jnz cp4_bgra8lop5

	movzx rax,byte [rdi]
	shr rax,4
	mov eax,dword [rdx+rax*4]
	movnti dword [rsi],eax
	lea rsi,[rsi+4]
	
	add rdi,r8 ;sbpl
	add rsi,r9 ;dbpl
	dec r10
	jnz cp4_bgra8lop4

	align 16
cp4_bgra8exit:
	ret

;void ImageUtil_ConvP8_B8G8R8A8(UInt8 *srcPtr, UInt8 *destPtr, OSInt w, OSInt h, OSInt sbpl, OSInt dbpl, UInt8 *pal);
;0 retAddr
;rdi srcPtr
;rsi destPtr
;rdx w
;rcx h
;r8 sbpl
;r9 dbpl
;8 pal

	align 16
ImageUtil_ConvP8_B8G8R8A8:
_ImageUtil_ConvP8_B8G8R8A8:
	mov r10,rcx ;h
	mov rcx,rdx
	mov rdx,qword [rsp+8] ;pal
	
	lea rax,[rcx*4]
	sub r8,rcx ;sbpl
	sub r9,rax ;dbpl
	align 16
cp8_bgra8lop:
	mov r11,rcx
	align 16
cp8_bgra8lop2:
	movzx rax,byte [rdi]
	mov eax,dword [rdx+rax*4]
	lea rdi,[rdi+1]
	movnti dword [rsi],eax
	lea rsi,[rsi+4]
	dec r11
	jnz cp8_bgra8lop2
	
	add rdi,r8 ;sbpl
	add rsi,r9 ;dbpl
	dec r10
	jnz cp8_bgra8lop
	
	ret

;void ImageUtil_ConvP1_A1_B8G8R8A8(UInt8 *srcPtr, UInt8 *destPtr, OSInt w, OSInt h, OSInt sbpl, OSInt dbpl, UInt8 *pal);
;0 retAddr
;rdi srcPtr
;rsi destPtr
;rdx w
;rcx h
;r8 sbpl
;r9 dbpl
;8 pal
	align 16
ImageUtil_ConvP1_A1_B8G8R8A8:
_ImageUtil_ConvP1_A1_B8G8R8A8:
	mov r10,rcx
	mov r8,rsi
	mov rcx,rdx
	mov rdx,qword [rsp+8] ;pal

	align 16
cp1_a1_bgra8lop2:
	mov r11,rcx
	mov rsi,r8
	shr r11,3
	align 16
cp1_a1_bgra8lop3:
	movzx rax,byte [rdi]
	shr rax,7
	mov eax,dword [rdx+rax*4]
	movnti dword [rsi],eax
	movzx rax,byte [rdi]
	shr rax,6
	and rax,1
	mov eax,dword [rdx+rax*4]
	movnti dword [rsi+4],eax
	movzx rax,byte [rdi]
	shr rax,5
	and rax,1
	mov eax,dword [rdx+rax*4]
	movnti dword [rsi+8],eax
	movzx rax,byte [rdi]
	shr rax,4
	and rax,1
	mov eax,dword [rdx+rax*4]
	movnti dword [rsi+12],eax
	movzx rax,byte [rdi]
	shr rax,3
	and rax,1
	mov eax,dword [rdx+rax*4]
	movnti dword [rsi+16],eax
	movzx rax,byte [rdi]
	shr rax,2
	and rax,1
	mov eax,dword [rdx+rax*4]
	movnti dword [rsi+20],eax
	movzx rax,byte [rdi]
	shr rax,1
	and rax,1
	mov eax,dword [rdx+rax*4]
	movnti dword [rsi+24],eax
	movzx rax,byte [rdi]
	and rax,1
	mov eax,dword [rdx+rax*4]
	lea rdi,[rdi+1]
	movnti dword [rsi+28],eax
	lea rsi,[rsi+32]
	dec r11
	jnz cp1_a1_bgra8lop3
	
	mov r11,rcx
	and r11,7
	jz cp1_a1_bgra8lop3b
	movq xmm3,rbx
	movzx rbx,byte [rdi]
	lea rdi,[rdi+1]
	align 16
cp1_a1_bgra8lop3a:
	rol bl,1
	movzx rax,bl
	and rax,1
	mov eax,dword [rdx+rax*4]
	movnti dword [rsi],eax
	lea rsi,[rsi+8]
	dec r11
	jnz cp1_a1_bgra8lop3a
	movq rbx,xmm3
	align 16
cp1_a1_bgra8lop3b:
	
	mov r11,rcx
	mov rsi,r8
	shr r11,3
	align 16
cp1_a1_bgra8lop4:
	movzx rax,byte [rdi]
	shr rax,7
	neg al
	mov byte [rsi+3],al
	movzx rax,byte [rdi]
	shr rax,6
	and rax,1
	neg al
	mov byte [rsi+7],al
	movzx rax,byte [rdi]
	shr rax,5
	and rax,1
	neg al
	mov byte [rsi+11],al
	movzx rax,byte [rdi]
	shr rax,4
	and rax,1
	neg al
	mov byte [rsi+15],al
	movzx rax,byte [rdi]
	shr rax,3
	and rax,1
	neg al
	mov byte [rsi+19],al
	movzx rax,byte [rdi]
	shr rax,2
	and rax,1
	neg al
	mov byte [rsi+23],al
	movzx rax,byte [rdi]
	shr rax,1
	and rax,1
	neg al
	mov byte [rsi+27],al
	movzx rax,byte [rdi]
	and rax,1
	neg al
	mov byte [rsi+31],al
	lea rdi,[rdi+1]
	lea rsi,[rsi+32]
	dec r11
	jnz cp1_a1_bgra8lop4
	
	mov r11,rcx
	and r11,3
	jz cp1_a1_bgra8lop4b
	movq xmm3,rbx
	movzx rbx,byte [rdi]
	lea rdi,[rdi+1]
	align 16
cp1_a1_bgra8lop4a:
	rol bl,1
	mov al,bl
	and al,1
	neg al
	mov byte [rsi+3],al

	lea rsi,[rsi+4]
	dec r11
	jnz cp1_a1_bgra8lop4a
	movq rbx,xmm3
	align 16
cp1_a1_bgra8lop4b:
	
	add r8,r9 ;dbpl
	dec r10
	jnz cp1_a1_bgra8lop2

	ret

;void ImageUtil_ConvP2_A1_B8G8R8A8(UInt8 *srcPtr, UInt8 *destPtr, OSInt w, OSInt h, OSInt sbpl, OSInt dbpl, UInt8 *pal);
;0 retAddr
;rdi srcPtr
;rsi destPtr
;rdx w
;rcx h
;r8 sbpl
;r9 dbpl
;8 pal rsi
	align 16
ImageUtil_ConvP2_A1_B8G8R8A8:
_ImageUtil_ConvP2_A1_B8G8R8A8:
	mov r10,rcx ;h
	mov r8,rsi
	mov rcx,rdx
	mov rdx,qword [rsp+8] ;pal

	align 16
cp2_a1_bgra8lop2:
	mov r11,rcx
	mov rsi,r8
	shr r11,2
	align 16
cp2_a1_bgra8lop3:
	movzx rax,byte [rdi]
	shr rax,6
	and rax,3
	mov eax,dword [rdx+rax*4]
	movnti dword [rsi],eax
	movzx rax,byte [rdi]
	shr rax,4
	and rax,3
	mov eax,dword [rdx+rax*4]
	movnti dword [rsi+4],eax
	movzx rax,byte [rdi]
	shr rax,2
	and rax,3
	mov eax,dword [rdx+rax*4]
	movnti dword [rsi+8],eax
	movzx rax,byte [rdi]
	and rax,3
	mov eax,dword [rdx+rax*4]
	movnti dword [rsi+12],eax
	lea rdi,[rdi+1]
	lea rsi,[rsi+16]
	dec r11
	jnz cp2_a1_bgra8lop3
	
	mov r11,rcx
	and r11,3
	jz cp2_a1_bgra8lop3b
	movq xmm3,rbx
	movzx rbx,byte [rdi]
	lea rdi,[rdi+1]
	align 16
cp2_a1_bgra8lop3a:
	rol bl,2
	movzx rax,bl
	and rax,3
	mov eax,dword [rdx+rax*4]
	mov dword [rsi],eax
	lea rsi,[rsi+4]
	dec r11
	jnz cp2_a1_bgra8lop3a
	movq rbx,xmm3
	align 16
cp2_a1_bgra8lop3b:
	
	mov r11,rcx
	mov rsi,r8
	shr r11,3
	align 16
cp2_a1_bgra8lop4:
	movzx rax,byte [rdi]
	shr rax,7
	neg al
	mov byte [rsi+3],al
	movzx rax,byte [rdi]
	shr rax,6
	and rax,1
	neg al
	mov byte [rsi+7],al
	movzx rax,byte [rdi]
	shr rax,5
	and rax,1
	neg al
	mov byte [rsi+11],al
	movzx rax,byte [rdi]
	shr rax,4
	and rax,1
	neg al
	mov byte [rsi+15],al
	movzx rax,byte [rdi]
	shr rax,3
	and rax,1
	neg al
	mov byte [rsi+19],al
	movzx rax,byte [rdi]
	shr rax,2
	and rax,1
	neg al
	mov byte [rsi+23],al
	movzx rax,byte [rdi]
	shr rax,1
	and rax,1
	neg al
	mov byte [rsi+27],al
	movzx rax,byte [rdi]
	and rax,1
	neg al
	mov byte [rsi+31],al
	lea rdi,[rdi+1]
	lea rsi,[rsi+32]
	dec r11
	jnz cp2_a1_bgra8lop4
	
	mov r11,rcx
	and r11,3
	jz cp2_a1_bgra8lop4b
	movq xmm3,rbx
	movzx rbx,byte [rdi]
	lea rdi,[rdi+1]
	align 16
cp2_a1_bgra8lop4a:
	rol bl,1
	mov al,bl
	and al,1
	neg al
	mov byte [rsi+3],al

	lea rsi,[rsi+4]
	dec r11
	jnz cp2_a1_bgra8lop4a
	movq rbx,xmm3
	align 16
cp2_a1_bgra8lop4b:
	
	add r8,r9 ;dbpl
	dec r10
	jnz cp2_a1_bgra8lop2

	ret

;void ImageUtil_ConvP4_A1_B8G8R8A8(UInt8 *srcPtr, UInt8 *destPtr, OSInt w, OSInt h, OSInt sbpl, OSInt dbpl, UInt8 *pal);
;0 retAddr
;rdi srcPtr
;rsi destPtr
;rdx w
;rcx h
;r8 sbpl
;r9 dbpl
;8 pal rsi

	align 16
ImageUtil_ConvP4_A1_B8G8R8A8:
_ImageUtil_ConvP4_A1_B8G8R8A8:
	mov r10,rcx ;h
	mov r8,rsi
	mov rcx,rdx
	mov rdx,qword [rsp+8] ;pal

	align 16
cp4_a1_bgra8lop2:
	mov r11,rcx
	mov rsi,r8
	shr r11,1
	align 16
cp4_a1_bgra8lop3:
	movzx rax,byte [rdi]
	shr rax,4
	mov eax,dword [rdx+rax*4]
	movnti dword [rsi],eax
	movzx rax,byte [rdi]
	and rax,15
	mov eax,dword [rdx+rax*4]
	lea rdx,[rdx+1]
	movnti dword [rsi+4],eax
	lea rsi,[rsi+8]
	dec r11
	jnz cp4_a1_bgra8lop3

	test rcx,1
	jz cp4_a1_bgra8lop3b
	movzx rax,byte [rdi]
	shr rax,4
	mov eax,dword [rdx+rax*4]
	movnti dword [rsi],eax
	lea rdx,[rdx+1]
	lea rsi,[rsi+4]
	align 16
cp4_a1_bgra8lop3b:

	mov r11,rcx
	mov rsi,r8
	shr r11,3
	align 16
cp4_a1_bgra8lop4:
	movzx rax,byte [rdi]
	shr rax,7
	neg al
	mov byte [rsi+3],al
	movzx rax,byte [rdi]
	shr rax,6
	and rax,1
	neg al
	mov byte [rsi+7],al
	movzx rax,byte [rdi]
	shr rax,5
	and rax,1
	neg al
	mov byte [rsi+11],al
	movzx rax,byte [rdi]
	shr rax,4
	and rax,1
	neg al
	mov byte [rsi+15],al
	movzx rax,byte [rdi]
	shr rax,3
	and rax,1
	neg al
	mov byte [rsi+19],al
	movzx rax,byte [rdi]
	shr rax,2
	and rax,1
	neg al
	mov byte [rsi+23],al
	movzx rax,byte [rdi]
	shr rax,1
	and rax,1
	neg al
	mov byte [rsi+27],al
	movzx rax,byte [rdi]
	and rax,1
	neg al
	mov byte [rsi+31],al
	lea rdi,[rdi+1]
	lea rsi,[rsi+32]
	dec r11
	jnz cp4_a1_bgra8lop4
	
	mov r11,rcx
	and r11,3
	jz cp4_a1_bgra8lop4b
	movq xmm3,rbx
	movzx rbx,byte [rdi]
	lea rdi,[rdi+1]
	align 16
cp4_a1_bgra8lop4a:
	rol bl,1
	mov al,bl
	and al,1
	neg al
	mov byte [rsi+3],al

	lea rsi,[rsi+4]
	dec r11
	jnz cp4_a1_bgra8lop4a
	movq rbx,xmm3
	align 16
cp4_a1_bgra8lop4b:
	
	add r8,r9 ;dbpl
	dec r10
	jnz cp4_a1_bgra8lop2

	ret

;void ImageUtil_ConvP8_A1_B8G8R8A8(UInt8 *srcPtr, UInt8 *destPtr, OSInt w, OSInt h, OSInt sbpl, OSInt dbpl, UInt8 *pal);
;0 retAddr
;rdi srcPtr
;rsi destPtr
;rdx w
;rcx h
;r8 sbpl
;r9 dbpl
;8 pal

	align 16
ImageUtil_ConvP8_A1_B8G8R8A8:
_ImageUtil_ConvP8_A1_B8G8R8A8:
	mov r10,rcx ;h
	mov r8,rsi
	mov rcx,rdx
	mov rdx,qword [rsp+8] ;pal

	align 16
cp8_a1_bgra8lop:
	mov r11,rcx
	mov rsi,r8
	align 16
cp8_a1_bgra8lop2:
	movzx rax,byte [rdi]
	mov eax,dword [rdx+rax*4]
	lea rdi,[rdi+1]
	movnti dword [rsi],eax
	lea rsi,[rsi+4]
	dec r11
	jnz cp8_a1_bgra8lop2
	
	mov r11,rcx
	mov rsi,r8
	shr r11,3
	align 16
cp8_a1_bgra8lop4:
	movzx rax,byte [rdi]
	shr rax,7
	neg al
	mov byte [rsi+3],al
	movzx rax,byte [rdi]
	shr rax,6
	and rax,1
	neg al
	mov byte [rsi+7],al
	movzx rax,byte [rdi]
	shr rax,5
	and rax,1
	neg al
	mov byte [rsi+11],al
	movzx rax,byte [rdi]
	shr rax,4
	and rax,1
	neg al
	mov byte [rsi+15],al
	movzx rax,byte [rdi]
	shr rax,3
	and rax,1
	neg al
	mov byte [rsi+19],al
	movzx rax,byte [rdi]
	shr rax,2
	and rax,1
	neg al
	mov byte [rsi+23],al
	movzx rax,byte [rdi]
	shr rax,1
	and rax,1
	neg al
	mov byte [rsi+27],al
	movzx rax,byte [rdi]
	and rax,1
	neg al
	mov byte [rsi+31],al
	lea rdi,[rdi+1]
	lea rsi,[rsi+32]
	dec r11
	jnz cp8_a1_bgra8lop4
	
	mov r11,rcx
	and r11,3
	jz cp8_a1_bgra8lop4b
	movq xmm3,rbx
	movzx rbx,byte [rdi]
	lea rdi,[rdi+1]
	align 16
cp8_a1_bgra8lop4a:
	rol bl,1
	mov al,bl
	and al,1
	neg al
	mov byte [rsi+3],al

	lea rsi,[rsi+4]
	dec r11
	jnz cp8_a1_bgra8lop4a
	movq rbx,xmm3
	align 16
cp8_a1_bgra8lop4b:
	
	add r8,r9 ;dbpl
	dec r10
	jnz cp8_a1_bgra8lop
	
	ret

;void ImageUtil_ConvB5G5R5_B8G8R8A8(UInt8 *srcPtr, UInt8 *destPtr, OSInt w, OSInt h, OSInt sbpl, OSInt dbpl);
;0 retAddr
;rdi srcPtr
;rsi destPtr
;rdx w
;rcx h
;r8 sbpl
;r9 dbpl

	align 16
ImageUtil_ConvB5G5R5_B8G8R8A8:
_ImageUtil_ConvB5G5R5_B8G8R8A8:
	lea rax,[rdx*2]
	sub r8,rax ;sbpl
	lea rax,[rdx*4]
	sub r9,rax ;dbpl
	mov eax,0x00840084
	movd xmm2,eax
	pxor xmm1,xmm1
	punpckldq xmm2,xmm2

	align 16
cb5g5r5_bgra8lop:
	mov r10,rdx
	
	align 16
cb5g5r5_bgra8lop2:
	movzx eax,word [edi]
	shl eax,6
	shr ax,3
	shr al,3
	movd xmm0, eax
	punpcklbw xmm0,xmm1
	pmullw xmm0, xmm2
	psrlw xmm0,4
	packuswb xmm0,xmm1
	movd dword [rsi],xmm0
	mov byte [rsi+3],0xff

	lea rdi,[rdi+2]
	lea rsi,[rsi+4]
	dec r10
	jnz cb5g5r5_bgra8lop2
	
	add rdi,r8 ;sbpl
	add rsi,r9 ;dbpl
	dec rcx
	jnz cb5g5r5_bgra8lop
	ret

;void ImageUtil_ConvB5G6R5_B8G8R8A8(UInt8 *srcPtr, UInt8 *destPtr, OSInt w, OSInt h, OSInt sbpl, OSInt dbpl);
;0 retAddr
;rdi srcPtr
;rsi destPtr
;rdx w
;rcx h
;r8 sbpl
;r9 dbpl

	align 16
ImageUtil_ConvB5G6R5_B8G8R8A8:
_ImageUtil_ConvB5G6R5_B8G8R8A8:
	lea rax,[rdx*2]
	sub r8,rax ;sbpl
	lea rax,[rdx*4]
	sub r9,rax ;dbpl
	mov eax,0x00410084
	movd xmm2,eax
	pxor xmm1,xmm1
	punpckldq xmm2,xmm2

	align 16
cb5g6r5_bgra8lop:
	mov r10,rdx
	
	align 16
cb5g6r5_bgra8lop2:
	movzx eax,word [edi]
	shl eax,5
	shr ax,2
	shr al,3
	movd xmm0, eax
	punpcklbw xmm0,xmm1
	pmullw xmm0, xmm2
	psrlw xmm0,4
	packuswb xmm0,xmm1
	movd dword [rsi],xmm0
	mov byte [rsi+3],0xff

	lea rdi,[rdi+2]
	lea rsi,[rsi+4]
	dec r10
	jnz cb5g6r5_bgra8lop2
	
	add rdi,r8 ;sbpl
	add rsi,r9 ;dbpl
	dec rcx
	jnz cb5g6r5_bgra8lop
	ret

;void ImageUtil_ConvB8G8R8_B8G8R8A8(UInt8 *srcPtr, UInt8 *destPtr, OSInt w, OSInt h, OSInt sbpl, OSInt dbpl);
;0 retAddr
;rdi srcPtr
;rsi destPtr
;rdx w
;rcx h
;r8 sbpl
;r9 dbpl

	align 16
ImageUtil_ConvB8G8R8_B8G8R8A8:
_ImageUtil_ConvB8G8R8_B8G8R8A8:
	lea rax,[rdx+rdx*2]
	sub r8,rax ;sbpl
	lea rax,[rdx*4]
	sub r9,rax ;dbpl
	
	align 16
cbgr8_bgra8lop:
	mov r10,rdx
	align 16
cbgr8_bgra8lop2:
	movzx eax,byte [rdi+2]
	or eax,0xff00
	shl eax,16
	mov ax,word [rdi]
	lea rdi,[rdi+3]
	movnti dword [rsi],eax
	lea rsi,[rsi+4]
	dec r10
	jnz cbgr8_bgra8lop2
	
	add rdi,r8 ;sbpl
	add rsi,r9 ;dbpl
	dec rcx
	jnz cbgr8_bgra8lop
	ret
	
;void ImageUtil_ConvR8G8B8_B8G8R8A8(UInt8 *srcPtr, UInt8 *destPtr, OSInt w, OSInt h, OSInt sbpl, OSInt dbpl);
;0 retAddr
;rdi srcPtr
;rsi destPtr
;rdx w
;rcx h
;r8 sbpl
;r9 dbpl

	align 16
ImageUtil_ConvR8G8B8_B8G8R8A8:
_ImageUtil_ConvR8G8B8_B8G8R8A8:
	cmp dword [rel _UseSSE42],0
	jnz crgb8_bgra8simd
	lea rax,[rdx+rdx*2]
	sub r8,rax ;sbpl
	lea rax,[rdx*4]
	sub r9,rax ;dbpl
	align 16
crgb8_bgra8lop:
	mov r10,rdx
	align 16
crgb8_bgra8lop2:
	movzx eax,byte [rdi]
	or eax,0xff00
	shl eax,16
	mov ah,byte [rdi+1]
	mov al,byte [rdi+2]
	lea rdi,[rdi+3]
	movnti dword [rsi],eax
	lea rsi,[rsi+4]
	dec r10
	jnz crgb8_bgra8lop2
	
	add rdi,r8 ;sbpl
	add rsi,r9 ;dbpl
	dec rcx
	jnz crgb8_bgra8lop
	ret

	align 16
crgb8_bgra8simd_addr:
	db 2,1,0,0xff
	db 5,4,3,0xff
	db 8,7,6,0xff
	db 11,10,9,0xff
crgb8_bgra8simd_mask:
	db 0,0,0,0xff
	db 0,0,0,0xff
	db 0,0,0,0xff
	db 0,0,0,0xff
	align 16
crgb8_bgra8simd:
	test r9,15
	jnz crgb8_bgra8simdna
	lea rax,[rdx+rdx*2]
	sub r8,rax ;sbpl
	lea rax,[rdx*4]
	sub r9,rax ;dbpl
	movdqa xmm1,[rel crgb8_bgra8simd_addr]
	movdqa xmm2,[rel crgb8_bgra8simd_mask]

	align 16
crgb8_bgra8simd1:
	mov r10,rdx
	shr r10,2
	jz crgb8_bgra8simd2
	align 16
crgb8_bgra8simd1a:
	movq xmm0,[rdi]
	pinsrd xmm0,[rdi+8],2
	pshufb xmm0,xmm1
	por xmm0,xmm2
	movntdq [rsi],xmm0
	lea rdi,[rdi+12]
	lea rsi,[rsi+16]
	dec r10
	jnz crgb8_bgra8simd1a

	align 16
crgb8_bgra8simd2:
	mov r10,rdx
	and r10,3
	jz crgb8_bgra8simd3

	align 16
crgb8_bgra8simd2a:
	movzx eax,byte [rdi]
	or eax,0xff00
	shl eax,16
	mov ah,byte [rdi+1]
	mov al,byte [rdi+2]
	lea rdi,[rdi+3]
	movnti dword [rsi],eax
	lea rsi,[rsi+4]
	dec r10
	jnz crgb8_bgra8simd2a

	align 16
crgb8_bgra8simd3:
	add rdi,r8 ;sbpl
	add rsi,r9 ;dbpl
	dec rcx
	jnz crgb8_bgra8simd1
	ret

	align 16
crgb8_bgra8simdna:
	lea rax,[rdx+rdx*2]
	sub r8,rax ;sbpl
	lea rax,[rdx*4]
	sub r9,rax ;dbpl
	movdqa xmm1,[rel crgb8_bgra8simd_addr]
	movdqa xmm2,[rel crgb8_bgra8simd_mask]

	align 16
crgb8_bgra8simdna1:
	mov r10,rdx
	shr r10,2
	jz crgb8_bgra8simdna2
	align 16
crgb8_bgra8simdna1a:
	movq xmm0,[rdi]
	pinsrd xmm0,[rdi+8],2
	pshufb xmm0,xmm1
	por xmm0,xmm2
	movdqu [rsi],xmm0
	lea rdi,[rdi+12]
	lea rsi,[rsi+16]
	dec r10
	jnz crgb8_bgra8simdna1a

	align 16
crgb8_bgra8simdna2:
	mov r10,rdx
	and r10,3
	jz crgb8_bgra8simdna3

	align 16
crgb8_bgra8simdna2a:
	movzx eax,byte [rdi]
	or eax,0xff00
	shl eax,16
	mov ah,byte [rdi+1]
	mov al,byte [rdi+2]
	lea rdi,[rdi+3]
	movnti dword [rsi],eax
	lea rsi,[rsi+4]
	dec r10
	jnz crgb8_bgra8simdna2a

	align 16
crgb8_bgra8simdna3:
	add rdi,r8 ;sbpl
	add rsi,r9 ;dbpl
	dec rcx
	jnz crgb8_bgra8simdna1
	ret


;void ImageUtil_ConvB8G8R8A8_B8G8R8(UInt8 *srcPtr, UInt8 *destPtr, OSInt w, OSInt h, OSInt sbpl, OSInt dbpl);
;0 retAddr
;rdi srcPtr
;rsi destPtr
;rdx w
;rcx h
;r8 sbpl
;r9 dbpl

	align 16
ImageUtil_ConvB8G8R8A8_B8G8R8:
_ImageUtil_ConvB8G8R8A8_B8G8R8:
	lea rax,[rdx+rdx*2]
	sub r9,rax ;dbpl
	lea rax,[rdx*4]
	sub r8,rax ;sbpl
	
	align 16
cbgra8_bgr8lop:
	mov r10,rdx
	align 16
cbgra8_bgr8lop2:
	mov ax,word [rdi]
	mov word [rsi],ax
	mov al,byte [rdi+2]
	mov byte [rsi+2],al
	lea rdi,[rdi+4]
	lea rsi,[rsi+3]
	dec r10
	jnz cbgra8_bgr8lop2
	
	add rdi,r8 ;sbpl
	add rsi,r9 ;dbpl
	dec rcx
	jnz cbgra8_bgr8lop
	ret
	
;void ImageUtil_ConvR8G8B8A8_B8G8R8A8(UInt8 *srcPtr, UInt8 *destPtr, OSInt w, OSInt h, OSInt sbpl, OSInt dbpl);
;0 retAddr
;rdi srcPtr
;rsi destPtr
;rdx w
;rcx h
;r8 sbpl
;r9 dbpl

	align 16
ImageUtil_ConvR8G8B8A8_B8G8R8A8:
_ImageUtil_ConvR8G8B8A8_B8G8R8A8:
	lea rax,[rdx*4]
	sub r8,rax ;sbpl
	sub r9,rax ;dbpl
	
	align 16
crgba8_bgra8lop:
	mov r10,rdx
	align 16
crgba8_bgra8lop2:
	movzx eax,byte [rdi]
	mov ah,byte [rdi+3]
	shl eax,16
	mov ah,byte [rdi+1]
	mov al,byte [rdi+2]
	lea rdi,[rdi+4]
	movnti dword [rsi],eax
	lea rsi,[rsi+4]
	dec r10
	jnz crgba8_bgra8lop2
	
	add rdi,r8 ;sbpl
	add rsi,r9 ;dbpl
	dec rcx
	jnz crgba8_bgra8lop
	ret


;void ImageUtil_ConvR8G8B8N8_B8G8R8A8(UInt8 *srcPtr, UInt8 *destPtr, OSInt w, OSInt h, OSInt sbpl, OSInt dbpl);
;0 retAddr
;rdi srcPtr
;rsi destPtr
;rdx w
;rcx h
;r8 sbpl
;r9 dbpl

	align 16
crgbn8_bgra8_and dd 0xff000000,0xff000000,0xff000000,0xff000000
crgbn8_bgra8_shuf dd 0xff000102,0xff040506,0xff08090a,0xff0c0d0e
ImageUtil_ConvR8G8B8N8_B8G8R8A8:
_ImageUtil_ConvR8G8B8N8_B8G8R8A8:
	lea rax,[rdx*4]
	sub r8,rax ;sbpl
	sub r9,rax ;dbpl
	test rdx,7
	jnz crgbn8_bgra8lop
	cmp dword [rel _UseSSE42],0
	jz crgbn8_bgra8lop

	shr rdx,3
	movdqa xmm1,[rel crgbn8_bgra8_shuf]
	movdqa xmm2,[rel crgbn8_bgra8_and]
	align 16
crgbn8_bgra8lop_ssse3:
	mov r10,rdx
	align 16
crgbn8_bgra8lop2_ssse3:
	movdqa xmm0,[rdi]
	pshufb xmm0,xmm1
	movdqa xmm3,[rdi+16]
	por xmm0,xmm2
	pshufb xmm3,xmm1
	por xmm3,xmm2
	movntdq [rsi],xmm0
	movntdq [rsi+16],xmm3
	add rdi,32
	add rsi,32
	dec r10
	jnz crgbn8_bgra8lop2_ssse3
	
	add rdi,r8 ;sbpl
	add rsi,r9 ;dbpl
	dec rcx
	jnz crgbn8_bgra8lop_ssse3
	ret

	align 16
crgbn8_bgra8lop:
	mov r10,rdx
	align 16
crgbn8_bgra8lop2:
	movzx eax,byte [rdi]
	mov ah,0xff
	shl eax,16
	mov ah,byte [rdi+1]
	mov al,byte [rdi+2]
	lea rdi,[rdi+4]
	movnti dword [rsi],eax
	lea rsi,[rsi+4]
	dec r10
	jnz crgbn8_bgra8lop2
	
	add rdi,r8 ;sbpl
	add rsi,r9 ;dbpl
	dec rcx
	jnz crgbn8_bgra8lop
	ret

;void ImageUtil_ConvB16G16R16_B8G8R8A8(UInt8 *srcPtr, UInt8 *destPtr, OSInt w, OSInt h, OSInt sbpl, OSInt dbpl);
;0 retAddr
;rdi srcPtr
;rsi destPtr
;rdx w
;rcx h
;r8 sbpl
;r9 dbpl

	align 16
ImageUtil_ConvB16G16R16_B8G8R8A8:
_ImageUtil_ConvB16G16R16_B8G8R8A8:
	lea rax,[rdx+rdx*2]
	shl rax,1
	sub r8,rax ;sbpl
	lea rax,[rdx*4]
	sub r9,rax ;dbpl
	shr rdx,1
	jb cbgr16_bgra8lopb
	
	align 16
cbgr16_bgra8lop:
	mov r10,rdx
	align 16
cbgr16_bgra8lop2:
	movzx rax,byte [rdi+11]
	or rax,0xff00
	shl rax,16
	mov ah,byte [rdi+9]
	mov al,byte [rdi+7]
	shl rax,16
	mov al,byte [rdi+5]
	mov ah,0xff
	shl rax,16
	mov ah,byte [rdi+3]
	mov al,byte [rdi+1]

	add rdi,12
	movnti qword [rsi],rax
	add rsi,8
	dec r10
	jnz cbgr16_bgra8lop2
	
	add rdi,r8 ;sbpl
	add rsi,r9 ;dbpl
	dec rcx
	jnz cbgr16_bgra8lop
	ret

	
	align 16
cbgr16_bgra8lopb:
	mov r10,rdx
	align 16
cbgr16_bgra8lop2b:
	movzx rax,byte [rdi+11]
	or rax,0xff00
	shl rax,16
	mov ah,byte [rdi+9]
	mov al,byte [rdi+7]
	shl rax,16
	mov al,byte [rdi+5]
	mov ah,0xff
	shl rax,16
	mov ah,byte [rdi+3]
	mov al,byte [rdi+1]

	add rdi,12
	movnti qword [rsi],rax
	add rsi,8
	dec r10
	jnz cbgr16_bgra8lop2b

	movzx eax,byte [rdi+5]
	mov ah,0xff
	shl rax,16
	mov ah,byte [rdi+3]
	mov al,byte [rdi+1]

	add rdi,6
	movnti dword [rsi],eax
	add rsi,4
	
	add rdi,r8 ;sbpl
	add rsi,r9 ;dbpl
	dec rcx
	jnz cbgr16_bgra8lopb
	ret

;void ImageUtil_ConvR16G16B16_B8G8R8A8(UInt8 *srcPtr, UInt8 *destPtr, OSInt w, OSInt h, OSInt sbpl, OSInt dbpl);
;0 retAddr
;rdi srcPtr
;rsi destPtr
;rdx w
;rcx h
;r8 sbpl
;r9 dbpl

	align 16
ImageUtil_ConvR16G16B16_B8G8R8A8:
_ImageUtil_ConvR16G16B16_B8G8R8A8:
	lea rax,[rdx+rdx*2]
	shl rax,1
	sub r8,rax ;sbpl
	lea rax,[rdx*4]
	sub r9,rax ;dbpl
	shr rdx,1
	jb crgb16_bgra8lopb
	
	align 16
crgb16_bgra8lop:
	mov r10,rdx
	align 16
crgb16_bgra8lop2:
	movzx rax,byte [rdi+7]
	or rax,0xff00
	shl rax,16
	mov ah,byte [rdi+9]
	mov al,byte [rdi+11]
	shl rax,16
	mov al,byte [rdi+1]
	mov ah,0xff
	shl rax,16
	mov ah,byte [rdi+3]
	mov al,byte [rdi+5]

	add rdi,12
	movnti qword [rsi],rax
	add rsi,8
	dec r10
	jnz crgb16_bgra8lop2
	
	add rdi,r8 ;sbpl
	add rsi,r9 ;dbpl
	dec rcx
	jnz crgb16_bgra8lop
	ret
	
	align 16
crgb16_bgra8lopb:
	mov r10,rdx
	align 16
crgb16_bgra8lop2b:
	movzx rax,byte [rdi+7]
	or rax,0xff00
	shl rax,16
	mov ah,byte [rdi+9]
	mov al,byte [rdi+11]
	shl rax,16
	mov al,byte [rdi+1]
	mov ah,0xff
	shl rax,16
	mov ah,byte [rdi+3]
	mov al,byte [rdi+5]

	add rdi,12
	movnti qword [rsi],rax
	add rsi,8
	dec r10
	jnz crgb16_bgra8lop2b

	movzx eax,byte [rdi+1]
	mov ah,0xff
	shl rax,16
	mov ah,byte [rdi+3]
	mov al,byte [rdi+5]

	add rdi,6
	movnti dword [rsi],eax
	add rsi,4
	
	add rdi,r8 ;sbpl
	add rsi,r9 ;dbpl
	dec rcx
	jnz crgb16_bgra8lopb
	ret

;void ImageUtil_ConvB16G16R16A16_B8G8R8A8(UInt8 *srcPtr, UInt8 *destPtr, OSInt w, OSInt h, OSInt sbpl, OSInt dbpl);
;0 retAddr
;rdi srcPtr
;rsi destPtr
;rdx w
;rcx h
;r8 sbpl
;r9 dbpl

	align 16
ImageUtil_ConvB16G16R16A16_B8G8R8A8:
_ImageUtil_ConvB16G16R16A16_B8G8R8A8:
	lea rax,[rdx*8]
	sub r8,rax ;sbpl
	lea rax,[rdx*4]
	sub r9,rax ;dbpl
	
	align 16
cbgra16_bgra8lop:
	mov r10,rdx
	align 16
cbgra16_bgra8lop2:
	movq xmm0,[rdi]
	psrlw xmm0,8
	lea rdi,[rdi+8]
	packuswb xmm0,xmm0
	movd dword [rsi],xmm0
	lea rsi,[rsi+4]
	dec r10
	jnz cbgra16_bgra8lop2
	
	add rdi,r8 ;sbpl
	add rsi,r9 ;dbpl
	dec rcx
	jnz cbgra16_bgra8lop
	ret

;void ImageUtil_ConvR16G16B16A16_B8G8R8A8(UInt8 *srcPtr, UInt8 *destPtr, OSInt w, OSInt h, OSInt sbpl, OSInt dbpl);
;0 retAddr
;rdi srcPtr
;rsi destPtr
;rdx w
;rcx h
;r8 sbpl
;r9 dbpl

	align 16
ImageUtil_ConvR16G16B16A16_B8G8R8A8:
_ImageUtil_ConvR16G16B16A16_B8G8R8A8:
	lea rax,[rdx*8]
	sub r8,rax ;sbpl
	lea rax,[rdx*4]
	sub r9,rax ;dbpl
	
	align 16
crgba16_bgra8lop:
	mov r10,rdx
	align 16
crgba16_bgra8lop2:
	mov ah,[rdi+7]
	mov al,[rdi+1]
	shl eax,16
	mov ah,[rdi+3]
	mov al,[rdi+5]
	movnti dword [rsi],eax
	lea rsi,[rsi+4]
	lea rdi,[rdi+8]
	dec r10
	jnz crgba16_bgra8lop2
	
	add rdi,r8 ;sbpl
	add rsi,r9 ;dbpl
	dec rcx
	jnz crgba16_bgra8lop
	ret

;void ImageUtil_ConvA2B10G10R10_B8G8R8A8(UInt8 *srcPtr, UInt8 *destPtr, OSInt w, OSInt h, OSInt sbpl, OSInt dbpl);
;0 retAddr
;rdi srcPtr
;rsi destPtr
;rdx w
;rcx h
;r8 sbpl
;r9 dbpl
	align 16
ImageUtil_ConvA2B10G10R10_B8G8R8A8:
_ImageUtil_ConvA2B10G10R10_B8G8R8A8:
	lea rax,[rdx*4]
	sub r8,rax ;sbpl
	lea rax,[rdx*4]
	sub r9,rax ;dbpl
	
	align 16
ca2b10g10r10_bgra8lop:
	mov r10,rdx
	align 16
ca2b10g10r10_bgra8lop2:
	mov eax,dword [rdi]
	shr eax,2
	mov byte [rsi+2],al
	shr eax,10
	mov byte [rsi+1],al
	shr eax,10
	mov byte [rsi+0],al
	mov al,ah
	shl al,2
	or al,ah
	mov ah,al
	shl al,4
	or al,ah
	mov byte [rsi+3],al
	lea rdi,[rdi+4]
	lea rsi,[rsi+4]
	dec r10
	jnz ca2b10g10r10_bgra8lop2
	
	add rdi,r8 ;sbpl
	add rsi,r9 ;dbpl
	dec rcx
	jnz ca2b10g10r10_bgra8lop
	ret

;void ImageUtil_ConvFB32G32R32A32_B8G8R8A8(UInt8 *srcPtr, UInt8 *destPtr, OSInt w, OSInt h, OSInt sbpl, OSInt dbpl);
;0 retAddr
;rdi srcPtr
;rsi destPtr
;rdx w
;rcx h
;r8 sbpl
;r9 dbpl
	align 16
ImageUtil_ConvFB32G32R32A32_B8G8R8A8:
_ImageUtil_ConvFB32G32R32A32_B8G8R8A8:
	lea rax,[rdx*4]
	sub r9,rax ;dbpl
	lea rax,[rax*4]
	sub r8,rax ;sbpl
	mov rax,255
	cvtsi2ss xmm1,rax
	unpcklps xmm1,xmm1
	unpcklps xmm1,xmm1
	
	align 16
cfbgra32_bgra8lop:
	mov r10,rdx
	align 16
cfbgra32_bgra8lop2:
	movups xmm0,[rdi]
	mulps xmm0,xmm1
	cvtps2dq xmm0,xmm0
	packssdw xmm0,xmm0
	packuswb xmm0,xmm0
	movd [rsi],xmm0
	lea rdi,[rdi+16]
	lea rsi,[rsi+4]
	dec r10
	jnz cfbgra32_bgra8lop2
	
	add rdi,r8 ;sbpl
	add rsi,r9 ;dbpl
	dec rcx
	jnz cfbgra32_bgra8lop
	ret


;void ImageUtil_ConvFR32G32B32A32_B8G8R8A8(UInt8 *srcPtr, UInt8 *destPtr, OSInt w, OSInt h, OSInt sbpl, OSInt dbpl);
;0 retAddr
;rdi srcPtr
;rsi destPtr
;rdx w
;rcx h
;r8 sbpl
;r9 dbpl
	align 16
ImageUtil_ConvFR32G32B32A32_B8G8R8A8:
_ImageUtil_ConvFR32G32B32A32_B8G8R8A8:
	lea rax,[rdx*4]
	sub r9,rax ;dbpl
	lea rax,[rax*4]
	sub r8,rax ;sbpl
	mov rax,255
	cvtsi2ss xmm1,rax
	unpcklps xmm1,xmm1
	unpcklps xmm1,xmm1
	
	align 16
cfrgba32_bgra8lop:
	mov r10,rdx
	align 16
cfrgba32_bgra8lop2:
	movups xmm0,[rdi]
	pshufd xmm0, xmm0, 0xc6
	mulps xmm0,xmm1
	cvtps2dq xmm0,xmm0
	packssdw xmm0,xmm0
	packuswb xmm0,xmm0
	movd [rsi],xmm0
	lea rdi,[rdi+16]
	lea rsi,[rsi+4]
	dec r10
	jnz cfrgba32_bgra8lop2
	
	add rdi,r8 ;sbpl
	add rsi,r9 ;dbpl
	dec rcx
	jnz cfrgba32_bgra8lop
	ret

;void ImageUtil_ConvFB32G32R32_B8G8R8A8(UInt8 *srcPtr, UInt8 *destPtr, OSInt w, OSInt h, OSInt sbpl, OSInt dbpl);
;0 retAddr
;rdi srcPtr
;rsi destPtr
;rdx w
;rcx h
;r8 sbpl
;r9 dbpl
	align 16
ImageUtil_ConvFB32G32R32_B8G8R8A8:
_ImageUtil_ConvFB32G32R32_B8G8R8A8:
	lea rax,[rdx*4]
	sub r9,rax ;dbpl
	lea rax,[rax*2+rax]
	sub r8,rax ;sbpl
	mov rax,255
	cvtsi2ss xmm1,rax
	unpcklps xmm1,xmm1
	unpcklps xmm1,xmm1
	
	align 16
cfbgr32_bgra8lop:
	mov r10,rdx
	align 16
cfbgr32_bgra8lop2:
	movq xmm0,[rdi]
	movss xmm2,[rdi+8]
	punpcklqdq xmm0,xmm2
	mulps xmm0,xmm1
	cvtps2dq xmm0,xmm0
	packssdw xmm0,xmm0
	packuswb xmm0,xmm0
	movd [rsi],xmm0
	mov byte [rsi+3],0xff
	lea rdi,[rdi+12]
	lea rsi,[rsi+4]
	dec r10
	jnz cfbgr32_bgra8lop2
	
	add rdi,r8 ;sbpl
	add rsi,r9 ;dbpl
	dec rcx
	jnz cfbgr32_bgra8lop
	ret

;void ImageUtil_ConvFR32G32B32_B8G8R8A8(UInt8 *srcPtr, UInt8 *destPtr, OSInt w, OSInt h, OSInt sbpl, OSInt dbpl);
;0 retAddr
;rdi srcPtr
;rsi destPtr
;rdx w
;rcx h
;r8 sbpl
;r9 dbpl
	align 16
ImageUtil_ConvFR32G32B32_B8G8R8A8:
_ImageUtil_ConvFR32G32B32_B8G8R8A8:
	lea rax,[rdx*4]
	sub r9,rax ;dbpl
	lea rax,[rax*2+rax]
	sub r8,rax ;sbpl
	mov rax,255
	cvtsi2ss xmm1,rax
	unpcklps xmm1,xmm1
	unpcklps xmm1,xmm1
	
	align 16
cfrgb32_bgra8lop:
	mov r10,rdx
	align 16
cfrgb32_bgra8lop2:
	movq xmm0,[rdi]
	movss xmm2,[rdi+8]
	punpcklqdq xmm0,xmm2
	pshufd xmm0, xmm0, 0xc6
	mulps xmm0,xmm1
	cvtps2dq xmm0,xmm0
	packssdw xmm0,xmm0
	packuswb xmm0,xmm0
	movd [rsi],xmm0
	mov byte [rsi+3],0xff
	lea rdi,[rdi+12]
	lea rsi,[rsi+4]
	dec r10
	jnz cfrgb32_bgra8lop2
	
	add rdi,r8 ;sbpl
	add rsi,r9 ;dbpl
	dec rcx
	jnz cfrgb32_bgra8lop
	ret

;void ImageUtil_ConvFW32A32_B8G8R8A8(UInt8 *srcPtr, UInt8 *destPtr, OSInt w, OSInt h, OSInt sbpl, OSInt dbpl);
;0 retAddr
;rdi srcPtr
;rsi destPtr
;rdx w
;rcx h
;r8 sbpl
;r9 dbpl
	align 16
ImageUtil_ConvFW32A32_B8G8R8A8:
_ImageUtil_ConvFW32A32_B8G8R8A8:
	lea rax,[rdx*4]
	sub r9,rax ;dbpl
	lea rax,[rax*2]
	sub r8,rax ;sbpl
	mov rax,255
	cvtsi2ss xmm1,rax
	unpcklps xmm1,xmm1
	unpcklps xmm1,xmm1
	
	align 16
cfwa32_bgra8lop:
	mov r10,rdx
	align 16
cfwa32_bgra8lop2:
	movq xmm0,[rdi]
	mulps xmm0,xmm1
	cvtps2dq xmm0,xmm0
	packssdw xmm0,xmm0
	packuswb xmm0,xmm0
	movd eax,xmm0
	mov byte [rsi],al
	mov byte [rsi+1],al
	mov word [rsi+2],ax
	lea rdi,[rdi+8]
	lea rsi,[rsi+4]
	dec r10
	jnz cfwa32_bgra8lop2
	
	add rdi,r8 ;sbpl
	add rsi,r9 ;dbpl
	dec rcx
	jnz cfwa32_bgra8lop
	ret

;void ImageUtil_ConvFW32_B8G8R8A8(UInt8 *srcPtr, UInt8 *destPtr, OSInt w, OSInt h, OSInt sbpl, OSInt dbpl);
;0 retAddr
;rdi srcPtr
;rsi destPtr
;rdx w
;rcx h
;r8 sbpl
;r9 dbpl
	align 16
ImageUtil_ConvFW32_B8G8R8A8:
_ImageUtil_ConvFW32_B8G8R8A8:
	lea rax,[rdx*4]
	sub r8,rax ;dbpl
	sub r9,rax ;sbpl
	mov rax,255
	cvtsi2ss xmm1,rax
	unpcklps xmm1,xmm1
	unpcklps xmm1,xmm1
	
	align 16
cfw32_bgra8lop:
	mov r10,rdx
	align 16
cfw32_bgra8lop2:
	movss xmm0,[rdi]
	mulss xmm0,xmm1
	cvtps2dq xmm0,xmm0
	packssdw xmm0,xmm0
	packuswb xmm0,xmm0
	movd eax,xmm0
	mov byte [rsi],al
	mov byte [rsi+1],al
	mov byte [rsi+2],al
	mov byte [rsi+3],0xff
	lea rdi,[rdi+4]
	lea rsi,[rsi+4]
	dec r10
	jnz cfw32_bgra8lop2
	
	add rdi,r8 ;sbpl
	add rsi,r9 ;dbpl
	dec rcx
	jnz cfw32_bgra8lop
	ret

;void ImageUtil_ConvP1_B16G16R16A16(UInt8 *srcPtr, UInt8 *destPtr, OSInt w, OSInt h, OSInt sbpl, OSInt dbpl, UInt8 *pal);
;0 retAddr
;rdi srcPtr
;rsi destPtr
;rdx w
;rcx h
;r8 sbpl
;r9 dbpl
;8 pal
	align 16
ImageUtil_ConvP1_B16G16R16A16:
_ImageUtil_ConvP1_B16G16R16A16:
	mov r10,rcx
	mov rcx,rdx
	mov rdx,qword [rsp+8] ;pal
	lea rax,[rcx*8]
	test rcx,7
	jnz cp1_bgra16lop
	
	shr rcx,3
	sub r8,rcx ;sbpl
	sub r9,rax ;dbpl
	align 16
cp1_bgra16lop2:
	mov r11,rcx
	align 16
cp1_bgra16lop3:
	movzx rax,byte [rdi]
	shr rax,7
	movd xmm0,dword [rdx+rax*4]
	punpcklbw xmm0,xmm0
	movq [rsi],xmm0
	movzx rax,byte [rdi]
	shr rax,6
	and rax,1
	movd xmm0,dword [rdx+rax*4]
	punpcklbw xmm0,xmm0
	movq [rsi+8],xmm0
	movzx rax,byte [rdi]
	shr rax,5
	and rax,1
	movd xmm0,dword [rdx+rax*4]
	punpcklbw xmm0,xmm0
	movq [rsi+16],xmm0
	movzx rax,byte [rdi]
	shr rax,4
	and rax,1
	movd xmm0,dword [rdx+rax*4]
	punpcklbw xmm0,xmm0
	movq [rsi+24],xmm0
	movzx rax,byte [rdi]
	shr rax,3
	and rax,1
	movd xmm0,dword [rdx+rax*4]
	punpcklbw xmm0,xmm0
	movq [rsi+32],xmm0
	movzx rax,byte [rdi]
	shr rax,2
	and rax,1
	movd xmm0,dword [rdx+rax*4]
	punpcklbw xmm0,xmm0
	movq [rsi+40],xmm0
	movzx rax,byte [rdi]
	shr rax,1
	and rax,1
	movd xmm0,dword [rdx+rax*4]
	punpcklbw xmm0,xmm0
	movq [rsi+48],xmm0
	movzx rax,byte [rdi]
	and rax,1
	movd xmm0,dword [rdx+rax*4]
	punpcklbw xmm0,xmm0
	lea rdi,[rdi+1]
	movq [rsi+56],xmm0
	lea rsi,[rsi+64]
	dec r11
	jnz cp1_bgra16lop3
	
	add rdi,r8 ;sbpl
	add rsi,r9 ;dbpl
	dec r10
	jnz cp1_bgra16lop2
	jmp cp1_bgra16exit
	
	align 16
cp1_bgra16lop:
	push r12
	mov r12,rcx
	shr rcx,3
	sub r8,rcx ;sbpl
	sub r9,rax ;dbpl
	align 16
cp1_bgra16lop4:
	mov r11,r12
	shr r11,3
	align 16
cp1_bgra16lop5:
	movzx rax,byte [rdi]
	xor rcx,rcx
	shl al,1
	adc rcx,0
	movd xmm0,dword [rdx+rcx*4]
	punpcklbw xmm0,xmm0
	movq [rsi],xmm0
	xor rdi,rdi
	shl al,1
	adc rcx,0
	movd xmm0,dword [rdx+rcx*4]
	punpcklbw xmm0,xmm0
	movq [rsi+8],xmm0
	xor rcx,rcx
	shl al,1
	adc rcx,0
	movd xmm0,dword [rdx+rcx*4]
	punpcklbw xmm0,xmm0
	movq [rsi+16],xmm0
	xor rcx,rcx
	shl al,1
	adc rcx,0
	movd xmm0,dword [rdx+rcx*4]
	punpcklbw xmm0,xmm0
	movq [rsi+24],xmm0
	xor rcx,rcx
	shl al,1
	adc rcx,0
	movd xmm0,dword [rdx+rcx*4]
	punpcklbw xmm0,xmm0
	movq [rsi+32],xmm0
	xor rcx,rcx
	shl al,1
	adc rcx,0
	movd xmm0,dword [rdx+rcx*4]
	punpcklbw xmm0,xmm0
	movq [rsi+40],xmm0
	xor rcx,rcx
	shl al,1
	adc rcx,0
	movd xmm0,dword [rdx+rcx*4]
	punpcklbw xmm0,xmm0
	movq [rsi+48],xmm0
	xor rcx,rcx
	shl al,1
	adc rcx,0
	movd xmm0,dword [rdx+rcx*4]
	punpcklbw xmm0,xmm0
	movq [rsi+56],xmm0
	lea rdi,[rdi+1]
	lea rsi,[rsi+64]
	dec r11
	jnz cp1_bgra16lop5

	mov r11,r12
	and r11,7
	movzx rax,byte [rdi]
	align 16
cp1_bgra16lop6:
	xor rcx,rcx
	shl al,1
	adc rcx,0
	movd xmm0,dword [rdx+rcx*4]
	punpcklbw xmm0,xmm0
	movq [rsi],xmm0
	lea rsi,[rsi+8]
	dec r11
	jnz cp1_bgra16lop6
	
	add rdi,r8 ;sbpl
	add rsi,r9 ;dbpl
	dec r10
	jnz cp1_bgra16lop4
	pop r12

	align 16
cp1_bgra16exit:
	ret

;void ImageUtil_ConvP2_B16G16R16A16(UInt8 *srcPtr, UInt8 *destPtr, OSInt w, OSInt h, OSInt sbpl, OSInt dbpl, UInt8 *pal);
;0 retAddr
;rdi srcPtr
;rsi destPtr
;rdx w
;rcx h
;r8 sbpl
;r9 dbpl
;8 pal rsi
	align 16
ImageUtil_ConvP2_B16G16R16A16:
_ImageUtil_ConvP2_B16G16R16A16:
	mov r10,rcx
	mov rcx,rdx
	mov rdx,qword [rsp+8] ;pal
	lea rax,[rcx*8]
	test rcx,3
	jnz cp2_bgra16lop
	
	shr rcx,2
	sub r8,rcx ;sbpl
	sub r9,rax ;dbpl
	align 16
cp2_bgra16lop2:
	mov r11,rcx
	align 16
cp2_bgra16lop3:
	movzx rax,byte [rdi]
	shr rax,6
	and rax,3
	movd xmm0,dword [rdx+rax*4]
	punpcklbw xmm0,xmm0
	movq [rsi],xmm0
	movzx rax,byte [rdi]
	shr rax,4
	and rax,3
	movd xmm0,dword [rdx+rax*4]
	punpcklbw xmm0,xmm0
	movq [rsi+8],xmm0
	movzx rax,byte [rdi]
	shr rax,2
	and rax,3
	movd xmm0,dword [rdx+rax*4]
	punpcklbw xmm0,xmm0
	movq [rsi+16],xmm0
	movzx rax,byte [rdi]
	and rax,3
	movd xmm0,dword [rdx+rax*4]
	punpcklbw xmm0,xmm0
	movq [rsi+24],xmm0
	lea rdi,[rdi+1]
	lea rsi,[rsi+32]
	dec r11
	jnz cp2_bgra16lop3
	
	add rdi,r8 ;sbpl
	add rsi,r9 ;dbpl
	dec r10
	jnz cp2_bgra16lop2
	jmp cp2_bgra16exit
	
	align 16
cp2_bgra16lop:
	push r12
	mov r12,rcx
	shr rcx,2
	sub r8,rcx ;sbpl
	sub r9,rax ;dbpl
	align 16
cp2_bgra16lop4:
	mov r11,r12
	shr r11,2
	align 16
cp2_bgra16lop5:
	movzx rax,byte [rdi]
	rol al,2
	mov rcx,rax
	and rcx,3
	movd xmm0,dword [rdx+rcx*4]
	punpcklbw xmm0,xmm0
	movq [rsi],xmm0
	rol al,2
	mov rcx,rax
	and rcx,3
	movd xmm0,dword [rdx+rcx*4]
	punpcklbw xmm0,xmm0
	movq [rsi+8],xmm0
	rol al,2
	mov rcx,rax
	and rcx,3
	movd xmm0,dword [rdx+rcx*4]
	punpcklbw xmm0,xmm0
	movq [rsi+16],xmm0
	rol al,2
	mov rcx,rax
	and rcx,3
	movd xmm0,dword [rdx+rcx*4]
	punpcklbw xmm0,xmm0
	movq [rsi+24],xmm0
	lea rdi,[rdi+1]
	lea rsi,[rsi+32]
	dec r11
	jnz cp2_bgra16lop5

	mov r11,r12
	and r11,3
	movzx rax,byte [rdi]
	align 16
cp2_bgra16lop6:
	rol al,2
	mov rcx,rax
	and rcx,3
	movd xmm0,dword [rdx+rcx*4]
	punpcklbw xmm0,xmm0
	movq [rsi],xmm0
	lea rsi,[rsi+8]
	dec r11
	jnz cp2_bgra16lop6
	
	add rdi,r8 ;sbpl
	add rsi,r9 ;dbpl
	dec r10
	jnz cp2_bgra16lop4
	pop r12

	align 16
cp2_bgra16exit:
	ret

;void ImageUtil_ConvP4_B16G16R16A16(UInt8 *srcPtr, UInt8 *destPtr, OSInt w, OSInt h, OSInt sbpl, OSInt dbpl, UInt8 *pal);
;0 retAddr
;rdi srcPtr
;rsi destPtr
;rdx w
;rcx h
;r8 sbpl
;r9 dbpl
;8 pal rsi

	align 16
ImageUtil_ConvP4_B16G16R16A16:
_ImageUtil_ConvP4_B16G16R16A16:
	mov r10,rcx
	mov rcx,rdx
	mov rdx,qword [rsp+8] ;pal
	lea rax,[rcx*8]
	shr rcx,1
	jb cp4_bgra16lop
	
	sub r8,rcx ;sbpl
	sub r9,rax ;dbpl
	align 16
cp4_bgra16lop2:
	mov r11,rcx
	align 16
cp4_bgra16lop3:
	movzx rax,byte [rdi]
	shr rax,4
	movd xmm0,dword [rdx+rax*4]
	punpcklbw xmm0,xmm0
	movq [rsi],xmm0
	movzx rax,byte [rdi]
	and rax,15
	movd xmm0,dword [rdx+rax*4]
	lea rdi,[rdi+1]
	punpcklbw xmm0,xmm0
	movq [rsi+8],xmm0
	lea rsi,[rsi+16]
	dec r11
	jnz cp4_bgra16lop3
	
	add rdi,r8 ;sbpl
	add rsi,r9 ;dbpl
	dec r10
	jnz cp4_bgra16lop2
	jmp cp4_bgra16exit
	
	align 16
cp4_bgra16lop:
	sub r8,rcx ;sbpl
	sub r9,rax ;dbpl
	align 16
cp4_bgra16lop4:
	mov r11,rcx
	align 16
cp4_bgra16lop5:
	movzx rax,byte [rdi]
	shr rax,4
	movd xmm0,dword [rdx+rax*4]
	punpcklbw xmm0,xmm0
	movq [rsi],xmm0
	movzx rax,byte [rdi]
	and rax,15
	movd xmm0,dword [rdx+rax*4]
	lea rdi,[rdi+1]
	punpcklbw xmm0,xmm0
	movq [rsi+8],xmm0
	lea rsi,[rsi+16]
	dec r11
	jnz cp4_bgra16lop5

	movzx rax,byte [rdi]
	shr rax,4
	movd xmm0,dword [rdx+rax*4]
	punpcklbw xmm0,xmm0
	movq [rsi],xmm0
	lea rsi,[rsi+8]
	
	add rdi,r8 ;sbpl
	add rsi,r9 ;dbpl
	dec r10
	jnz cp4_bgra16lop4

	align 16
cp4_bgra16exit:
	ret

;void ImageUtil_ConvP8_B16G16R16A16(UInt8 *srcPtr, UInt8 *destPtr, OSInt w, OSInt h, OSInt sbpl, OSInt dbpl, UInt8 *pal);
;0 retAddr
;rdi srcPtr
;rsi destPtr
;rdx w
;rcx h
;r8 sbpl
;r9 dbpl
;8 pal

	align 16
ImageUtil_ConvP8_B16G16R16A16:
_ImageUtil_ConvP8_B16G16R16A16:
	mov r10,rcx
	mov rcx,rdx
	mov rdx,qword [rsp+8] ;pal
	
	lea rax,[rcx*8]
	sub r8,rcx ;sbpl
	sub r9,rax ;dbpl
	align 16
cp8_bgra16lop:
	mov r11,rcx
	align 16
cp8_bgra16lop2:
	movzx rax,byte [rdi]
	movd xmm0,dword [rdx+rax*4]
	lea rdi,[rdi+1]
	punpcklbw xmm0,xmm0
	movq [rsi],xmm0
	lea rsi,[rsi+4]
	dec r11
	jnz cp8_bgra16lop2
	
	add rdi,r8 ;sbpl
	add rsi,r9 ;dbpl
	dec r10
	jnz cp8_bgra16lop
	
	ret

;void ImageUtil_ConvP1_A1_B16G16R16A16(UInt8 *srcPtr, UInt8 *destPtr, OSInt w, OSInt h, OSInt sbpl, OSInt dbpl, UInt8 *pal);
;0 retAddr
;rdi srcPtr
;rsi destPtr
;rdx w
;rcx h
;r8 sbpl
;r9 dbpl
;8 pal
	align 16
ImageUtil_ConvP1_A1_B16G16R16A16:
_ImageUtil_ConvP1_A1_B16G16R16A16:
	mov r10,rcx
	mov r8,rsi
	mov rcx,rdx
	mov rdx,qword [rsp+8] ;pal

	align 16
cp1_a1_bgra16lop2:
	mov r11,rcx
	mov rsi,r8
	shr r11,3
	align 16
cp1_a1_bgra16lop3:
	movzx rax,byte [rdi]
	shr rax,7
	movd xmm0,dword [rdx+rax*4]
	punpcklbw xmm0,xmm0
	movq [rsi],xmm0
	movzx rax,byte [rdi]
	shr rax,6
	and rax,1
	movd xmm0,dword [rdx+rax*4]
	punpcklbw xmm0,xmm0
	movq [rsi+8],xmm0
	movzx rax,byte [rdi]
	shr rax,5
	and rax,1
	movd xmm0,dword [rdx+rax*4]
	punpcklbw xmm0,xmm0
	movq [rsi+16],xmm0
	movzx rax,byte [rdi]
	shr rax,4
	and rax,1
	movd xmm0,dword [rdx+rax*4]
	punpcklbw xmm0,xmm0
	movq [rsi+24],xmm0
	movzx rax,byte [rdi]
	shr rax,3
	and rax,1
	movd xmm0,dword [rdx+rax*4]
	punpcklbw xmm0,xmm0
	movq [rsi+32],xmm0
	movzx rax,byte [rdi]
	shr rax,2
	and rax,1
	movd xmm0,dword [rdx+rax*4]
	punpcklbw xmm0,xmm0
	movq [rsi+40],xmm0
	movzx rax,byte [rdi]
	shr rax,1
	and rax,1
	movd xmm0,dword [rdx+rax*4]
	punpcklbw xmm0,xmm0
	movq [rsi+48],xmm0
	movzx rax,byte [rdi]
	and rax,1
	movd xmm0,dword [rdx+rax*4]
	punpcklbw xmm0,xmm0
	lea rdi,[rdi+1]
	movq [rsi+56],xmm0
	lea rsi,[rsi+64]
	dec r11
	jnz cp1_a1_bgra16lop3
	
	mov r11,rcx
	and r11,7
	jz cp1_a1_bgra16lop3b
	movq xmm3,rbx
	movzx rbx,byte [rdi]
	lea rdi,[rdi+1]
	align 16
cp1_a1_bgra16lop3a:
	rol bl,1
	movzx rax,bl
	and rax,1
	movd xmm0,dword [rdx+rax*4]
	punpcklbw xmm0,xmm0
	movq [rsi],xmm0
	lea rsi,[rsi+8]
	dec r11
	jnz cp1_a1_bgra16lop3a
	movq rbx,xmm3
	align 16
cp1_a1_bgra16lop3b:
	
	mov r11,rcx
	mov rsi,r8
	shr r11,3
	align 16
cp1_a1_bgra16lop4:
	movzx rax,byte [rdi]
	shr rax,7
	neg ax
	mov word [rsi+6],ax
	movzx rax,byte [rdi]
	shr rax,6
	and rax,1
	neg ax
	mov word [rsi+14],ax
	movzx rax,byte [rdi]
	shr rax,5
	and rax,1
	neg ax
	mov word [rsi+22],ax
	movzx rax,byte [rdi]
	shr rax,4
	and rax,1
	neg ax
	mov word [rsi+30],ax
	movzx rax,byte [rdi]
	shr rax,3
	and rax,1
	neg ax
	mov word [rsi+38],ax
	movzx rax,byte [rdi]
	shr rax,2
	and rax,1
	neg ax
	mov word [rsi+46],ax
	movzx rax,byte [rdi]
	shr rax,1
	and rax,1
	neg ax
	mov word [rsi+54],ax
	movzx rax,byte [rdi]
	and rax,1
	neg ax
	mov word [rsi+62],ax
	lea rdi,[rdi+1]
	lea rsi,[rsi+64]
	dec r11
	jnz cp1_a1_bgra16lop4
	
	mov r11,rcx
	and r11,7
	jz cp1_a1_bgra16lop4b
	movq xmm3,rbx
	movzx rbx,byte [rdi]
	lea rdi,[rdi+1]
	align 16
cp1_a1_bgra16lop4a:
	rol bl,1
	movzx rax,bl
	and rax,1
	neg ax
	mov word [rsi+6],ax
	movzx rax,byte [rdi]
	lea rsi,[rsi+8]
	dec r11
	jnz cp1_a1_bgra16lop4a
	movq rbx,xmm3
	align 16
cp1_a1_bgra16lop4b:

	add r8,r9 ;dbpl
	dec r10
	jnz cp1_a1_bgra16lop2

	ret

;void ImageUtil_ConvP2_A1_B16G16R16A16(UInt8 *srcPtr, UInt8 *destPtr, OSInt w, OSInt h, OSInt sbpl, OSInt dbpl, UInt8 *pal);
;0 retAddr
;rdi srcPtr
;rsi destPtr
;rdx w
;rcx h
;r8 sbpl
;r9 dbpl
;8 pal rsi
	align 16
ImageUtil_ConvP2_A1_B16G16R16A16:
_ImageUtil_ConvP2_A1_B16G16R16A16:
	mov r10,rcx
	mov r8,rsi
	mov rcx,rdx
	mov rdx,qword [rsp+8] ;pal

	align 16
cp2_a1_bgra16lop2:
	mov r11,rcx
	mov rsi,r8
	shr r11,2
	align 16
cp2_a1_bgra16lop3:
	movzx rax,byte [rdi]
	shr rax,6
	and rax,3
	movd xmm0,dword [rdx+rax*4]
	punpcklbw xmm0,xmm0
	movq [rsi],xmm0
	movzx rax,byte [rdi]
	shr rax,4
	and rax,3
	movd xmm0,dword [rdx+rax*4]
	punpcklbw xmm0,xmm0
	movq [rsi+8],xmm0
	movzx rax,byte [rdi]
	shr rax,2
	and rax,3
	movd xmm0,dword [rdx+rax*4]
	punpcklbw xmm0,xmm0
	movq [rsi+16],xmm0
	movzx rax,byte [rdi]
	and rax,3
	movd xmm0,dword [rdx+rax*4]
	punpcklbw xmm0,xmm0
	movq [rsi+24],xmm0
	lea rdi,[rdi+1]
	lea rsi,[rsi+32]
	dec r11
	jnz cp2_a1_bgra16lop3
	
	mov r11,rcx
	and r11,3
	jz cp2_a1_bgra16lop3b
	movq xmm3,rbx
	movzx rbx,byte [rdi]
	lea rdi,[rdi+1]
	align 16
cp2_a1_bgra16lop3a:
	rol bl,2
	movzx rax,bl
	and rax,3
	movd xmm0,dword [rdx+rax*4]
	punpcklbw xmm0,xmm0
	movq [rsi],xmm0
	lea rsi,[rsi+8]
	dec r11
	jnz cp2_a1_bgra16lop3a
	movq rbx,xmm3
	align 16
cp2_a1_bgra16lop3b:

	mov r11,rcx
	mov rsi,r8
	shr r11,3
	align 16
cp2_a1_bgra16lop4:
	movzx rax,byte [rdi]
	shr rax,7
	neg ax
	mov word [rsi+6],ax
	movzx rax,byte [rdi]
	shr rax,6
	and rax,1
	neg ax
	mov word [rsi+14],ax
	movzx rax,byte [rdi]
	shr rax,5
	and rax,1
	neg ax
	mov word [rsi+22],ax
	movzx rax,byte [rdi]
	shr rax,4
	and rax,1
	neg ax
	mov word [rsi+30],ax
	movzx rax,byte [rdi]
	shr rax,3
	and rax,1
	neg ax
	mov word [rsi+38],ax
	movzx rax,byte [rdi]
	shr rax,2
	and rax,1
	neg ax
	mov word [rsi+46],ax
	movzx rax,byte [rdi]
	shr rax,1
	and rax,1
	neg ax
	mov word [rsi+54],ax
	movzx rax,byte [rdi]
	and rax,1
	neg ax
	mov word [rsi+62],ax
	lea rdi,[rdi+1]
	lea rsi,[rsi+64]
	dec r11
	jnz cp2_a1_bgra16lop4

	mov r11,rcx
	and r11,7
	jz cp2_a1_bgra16lop4b
	movq xmm3,rbx
	movzx rbx,byte [rdi]
	lea rdi,[rdi+1]
	align 16
cp2_a1_bgra16lop4a:
	rol bl,1
	movzx rax,bl
	and rax,1
	neg ax
	mov word [rsi+6],ax
	movzx rax,byte [rdi]
	lea rsi,[rsi+8]
	dec r11
	jnz cp2_a1_bgra16lop4a
	movq rbx,xmm3
	align 16
cp2_a1_bgra16lop4b:

	
	add r8,r9 ;dbpl
	dec r10
	jnz cp2_a1_bgra16lop2

	ret

;void ImageUtil_ConvP4_A1_B16G16R16A16(UInt8 *srcPtr, UInt8 *destPtr, OSInt w, OSInt h, OSInt sbpl, OSInt dbpl, UInt8 *pal);
;0 retAddr
;rdi srcPtr
;rsi destPtr
;rdx w
;rcx h
;r8 sbpl
;r9 dbpl
;8 pal rsi

	align 16
ImageUtil_ConvP4_A1_B16G16R16A16:
_ImageUtil_ConvP4_A1_B16G16R16A16:
	mov r10,rcx
	mov r8,rsi
	mov rcx,rdx
	mov rdx,qword [rsp+8] ;pal

	align 16
cp4_a1_bgra16lop2:
	mov r11,rcx
	mov rsi,r8
	shr r11,1
	align 16
cp4_a1_bgra16lop3:
	movzx rax,byte [rdi]
	shr rax,4
	movd xmm0,dword [rdx+rax*4]
	punpcklbw xmm0,xmm0
	movq [rsi],xmm0
	movzx rax,byte [rdi]
	and rax,15
	movd xmm0,dword [rdx+rax*4]
	lea rdi,[rdi+1]
	punpcklbw xmm0,xmm0
	movq [rsi+8],xmm0
	lea rsi,[rsi+16]
	dec r11
	jnz cp4_a1_bgra16lop3
	
	test rcx,1
	jz cp4_a1_bgra16lop3b
	movzx rax,byte [rdi]
	shr rax,4
	movd xmm0,dword [rdx+rax*4]
	punpcklbw xmm0,xmm0
	movq [rsi],xmm0
	lea rdi,[rdi+1]
	lea rsi,[rsi+8]
	align 16
cp4_a1_bgra16lop3b:
	
	mov r11,rcx
	mov rsi,r8
	shr r11,3
	align 16
cp4_a1_bgra16lop4:
	movzx rax,byte [rdi]
	shr rax,7
	neg ax
	mov word [rsi+6],ax
	movzx rax,byte [rdi]
	shr rax,6
	and rax,1
	neg ax
	mov word [rsi+14],ax
	movzx rax,byte [rdi]
	shr rax,5
	and rax,1
	neg ax
	mov word [rsi+22],ax
	movzx rax,byte [rdi]
	shr rax,4
	and rax,1
	neg ax
	mov word [rsi+30],ax
	movzx rax,byte [rdi]
	shr rax,3
	and rax,1
	neg ax
	mov word [rsi+38],ax
	movzx rax,byte [rdi]
	shr rax,2
	and rax,1
	neg ax
	mov word [rsi+46],ax
	movzx rax,byte [rdi]
	shr rax,1
	and rax,1
	neg ax
	mov word [rsi+54],ax
	movzx rax,byte [rdi]
	and rax,1
	neg ax
	mov word [rsi+62],ax
	lea rdi,[rdi+1]
	lea rsi,[rsi+64]
	dec r11
	jnz cp4_a1_bgra16lop4
	
	mov r11,rcx
	and r11,7
	jz cp4_a1_bgra16lop4b
	movq xmm3,rbx
	movzx rbx,byte [rdi]
	lea rdi,[rdi+1]
	align 16
cp4_a1_bgra16lop4a:
	rol bl,1
	movzx rax,bl
	and rax,1
	neg ax
	mov word [rsi+6],ax
	movzx rax,byte [rdi]
	lea rsi,[rsi+8]
	dec r11
	jnz cp4_a1_bgra16lop4a
	movq rbx,xmm3
	align 16
cp4_a1_bgra16lop4b:

	add r8,r9 ;dbpl
	dec r10
	jnz cp4_a1_bgra16lop2

	ret

;void ImageUtil_ConvARGB8_A1_B16G16R16A16(UInt8 *srcPtr, UInt8 *destPtr, OSInt w, OSInt h, OSInt sbpl, OSInt dbpl, UInt8 *pal);
;0 retAddr
;rdi srcPtr
;rsi destPtr
;rdx w
;rcx h
;r8 sbpl
;r9 dbpl
;8 pal

	align 16
ImageUtil_ConvP8_A1_B16G16R16A16:
_ImageUtil_ConvP8_A1_B16G16R16A16:
	mov r10,rcx
	mov r8,rsi
	mov rcx,rdx
	mov rdx,qword [rsp+8] ;pal
	
	align 16
cp8_a1_bgra16lop:
	mov r11,rcx
	mov rsi,r8
	align 16
cp8_a1_bgra16lop2:
	movzx rax,byte [rdi]
	movd xmm0,dword [rdx+rax*4]
	lea rdi,[rdi+1]
	punpcklbw xmm0,xmm0
	movq [rsi],xmm0
	lea rsi,[rsi+4]
	dec r11
	jnz cp8_a1_bgra16lop2
	
	mov r11,rcx
	mov rsi,r8
	shr r11,3
	align 16
cp8_a1_bgra16lop4:
	movzx rax,byte [rdi]
	shr rax,7
	neg ax
	mov word [rsi+6],ax
	movzx rax,byte [rdi]
	shr rax,6
	and rax,1
	neg ax
	mov word [rsi+14],ax
	movzx rax,byte [rdi]
	shr rax,5
	and rax,1
	neg ax
	mov word [rsi+22],ax
	movzx rax,byte [rdi]
	shr rax,4
	and rax,1
	neg ax
	mov word [rsi+30],ax
	movzx rax,byte [rdi]
	shr rax,3
	and rax,1
	neg ax
	mov word [rsi+38],ax
	movzx rax,byte [rdi]
	shr rax,2
	and rax,1
	neg ax
	mov word [rsi+46],ax
	movzx rax,byte [rdi]
	shr rax,1
	and rax,1
	neg ax
	mov word [rsi+54],ax
	movzx rax,byte [rdi]
	and rax,1
	neg ax
	mov word [rsi+62],ax
	lea rdi,[rdi+1]
	lea rsi,[rsi+64]
	dec r11
	jnz cp8_a1_bgra16lop4
	
	mov r11,rcx
	and r11,7
	jz cp8_a1_bgra16lop4b
	movq xmm3,rbx
	movzx rbx,byte [rdi]
	lea rdi,[rdi+1]
	align 16
cp8_a1_bgra16lop4a:
	rol bl,1
	movzx rax,bl
	and rax,1
	neg ax
	mov word [rsi+6],ax
	movzx rax,byte [rdi]
	lea rsi,[rsi+8]
	dec r11
	jnz cp8_a1_bgra16lop4a
	movq rbx,xmm3
	align 16
cp8_a1_bgra16lop4b:

	add r8,r9 ;dbpl
	dec r10
	jnz cp8_a1_bgra16lop
	
	ret

;void ImageUtil_ConvB5G5R5_B16G16R16A16(UInt8 *srcPtr, UInt8 *destPtr, OSInt w, OSInt h, OSInt sbpl, OSInt dbpl);
;0 retAddr
;rdi srcPtr
;rsi destPtr
;rdx w
;rcx h
;r8 sbpl
;r9 dbpl

	align 16
ImageUtil_ConvB5G5R5_B16G16R16A16:
_ImageUtil_ConvB5G5R5_B16G16R16A16:
	lea rax,[rdx*2]
	sub r8,rax ;sbpl
	lea rax,[rdx*8]
	sub r9,rax ;dbpl
	mov eax,0x08420842
	movd xmm2,eax
	pxor xmm1,xmm1
	punpckldq xmm2,xmm2

	align 16
cb5g5r5_bgra16lop:
	mov r10,rdx
	
	align 16
cb5g5r5_bgra16lop2:
	movzx eax,word [edi]
	and eax,0x7fff
	shl eax,6
	shr ax,3
	shr al,3
	movd xmm0, eax
	punpcklbw xmm0,xmm1
	pmullw xmm0, xmm2
	movq [rsi],xmm0
	mov word [rsi+6],0xffff

	lea rdi,[rdi+2]
	lea rsi,[rsi+8]
	dec r10
	jnz cb5g5r5_bgra16lop2
	
	add rdi,r8 ;sbpl
	add rsi,r9 ;dbpl
	dec rcx
	jnz cb5g5r5_bgra16lop
	ret

;void ImageUtil_ConvB5G6R5_B16G16R16A16(UInt8 *srcPtr, UInt8 *destPtr, OSInt w, OSInt h, OSInt sbpl, OSInt dbpl);
;0 retAddr
;rdi srcPtr
;rsi destPtr
;rdx w
;rcx h
;r8 sbpl
;r9 dbpl

	align 16
ImageUtil_ConvB5G6R5_B16G16R16A16:
_ImageUtil_ConvB5G6R5_B16G16R16A16:
	lea rax,[rdx*2]
	sub r8,rax ;sbpl
	lea rax,[rdx*8]
	sub r9,rax ;dbpl
	mov eax,0x04100842
	movd xmm2,eax
	pxor xmm1,xmm1
	punpckldq xmm2,xmm2

	align 16
cb5g6r5_bgra16lop:
	mov r10,rdx
	
	align 16
cb5g6r5_bgra16lop2:
	movzx eax,word [rdi]
	shl eax,5
	shr ax,2
	shr al,3
	movd xmm0, eax
	punpcklbw xmm0,xmm1
	pmullw xmm0, xmm2
	movq [rsi],xmm0
	mov word [rsi+6],0xffff

	lea rdi,[rdi+2]
	lea rsi,[rsi+8]
	dec r10
	jnz cb5g6r5_bgra16lop2
	
	add rdi,r8 ;sbpl
	add rsi,r9 ;dbpl
	dec rcx
	jnz cb5g6r5_bgra16lop
	ret

;void ImageUtil_ConvB8G8R8_B16G16R16A16(UInt8 *srcPtr, UInt8 *destPtr, OSInt w, OSInt h, OSInt sbpl, OSInt dbpl);
;0 retAddr
;rdi srcPtr
;rsi destPtr
;rdx w
;rcx h
;r8 sbpl
;r9 dbpl

	align 16
ImageUtil_ConvB8G8R8_B16G16R16A16:
_ImageUtil_ConvB8G8R8_B16G16R16A16:
	lea rax,[rdx+rdx*2]
	sub r8,rax ;sbpl
	lea rax,[rdx*8]
	sub r9,rax ;dbpl
	
	align 16
cbgr8_bgra16lop:
	mov r10,rdx
	align 16
cbgr8_bgra16lop2:
	movzx eax,byte [rdi+2]
	or eax,0xff00
	shl eax,16
	mov ax,word [rdi]
	movd xmm0,eax
	punpcklbw xmm0,xmm0
	movq [rsi],xmm0
	lea rdi,[rdi+3]
	lea rsi,[rsi+8]
	dec r10
	jnz cbgr8_bgra16lop2
	
	add rdi,r8 ;sbpl
	add rsi,r9 ;dbpl
	dec rcx
	jnz cbgr8_bgra16lop
	ret
	
;void ImageUtil_ConvR8G8B8_B16G16R16A16(UInt8 *srcPtr, UInt8 *destPtr, OSInt w, OSInt h, OSInt sbpl, OSInt dbpl);
;0 retAddr
;rdi srcPtr
;rsi destPtr
;rdx w
;rcx h
;r8 sbpl
;r9 dbpl

	align 16
ImageUtil_ConvR8G8B8_B16G16R16A16:
_ImageUtil_ConvR8G8B8_B16G16R16A16:
	lea rax,[rdx+rdx*2]
	sub r8,rax ;sbpl
	lea rax,[rdx*8]
	sub r9,rax ;dbpl
	
	align 16
crgb8_bgra16lop:
	mov r10,rdx
	align 16
crgb8_bgra16lop2:
	movzx eax,byte [rdi]
	or eax,0xff00
	shl eax,16
	mov ah,byte [rdi+1]
	mov al,byte [rdi+2]
	movd xmm0,eax
	punpcklbw xmm0,xmm0
	movq [rsi],xmm0
	lea rdi,[rdi+3]
	lea rsi,[rsi+8]
	dec r10
	jnz crgb8_bgra16lop2
	
	add rdi,r8 ;sbpl
	add rsi,r9 ;dbpl
	dec rcx
	jnz crgb8_bgra16lop
	ret
	
;void ImageUtil_ConvR8G8B8A8_B16G16R16A16(UInt8 *srcPtr, UInt8 *destPtr, OSInt w, OSInt h, OSInt sbpl, OSInt dbpl);
;0 retAddr
;rdi srcPtr
;rsi destPtr
;rdx w
;rcx h
;r8 sbpl
;r9 dbpl

	align 16
ImageUtil_ConvR8G8B8A8_B16G16R16A16:
_ImageUtil_ConvR8G8B8A8_B16G16R16A16:
	lea rax,[rdx*4]
	sub r8,rax ;sbpl
	lea rax,[rdx*8]
	sub r9,rax ;dbpl
	
	align 16
crgba8_bgra16lop:
	mov r10,rdx
	align 16
crgba8_bgra16lop2:
	movzx eax,byte [rdi]
	mov ah,byte [rdi+3]
	shl eax,16
	mov ah,byte [rdi+1]
	mov al,byte [rdi+2]
	movd xmm0,eax
	punpcklbw xmm0,xmm0
	movq [rsi],xmm0
	lea rdi,[rdi+4]
	lea rsi,[rsi+8]
	dec r10
	jnz crgba8_bgra16lop2
	
	add rdi,r8 ;sbpl
	add rsi,r9 ;dbpl
	dec rcx
	jnz crgba8_bgra16lop
	ret

;void ImageUtil_ConvB8G8R8A8_B16G16R16A16(UInt8 *srcPtr, UInt8 *destPtr, OSInt w, OSInt h, OSInt sbpl, OSInt dbpl);
;0 retAddr
;rdi srcPtr
;rsi destPtr
;rdx w
;rcx h
;r8 sbpl
;r9 dbpl

	align 16
ImageUtil_ConvB8G8R8A8_B16G16R16A16:
_ImageUtil_ConvB8G8R8A8_B16G16R16A16:
	lea rax,[rdx*4]
	sub r8,rax ;sbpl
	lea rax,[rdx*8]
	sub r9,rax ;dbpl
	
	align 16
cbgra8_bgra16lop:
	mov r10,rdx
	align 16
cbgra8_bgra16lop2:
	movd xmm0,[rdi]
	punpcklbw xmm0,xmm0
	lea rdi,[rdi+4]
	movq [rsi],xmm0
	lea rsi,[rsi+8]
	dec r10
	jnz cbgra8_bgra16lop2
	
	add rdi,r8 ;sbpl
	add rsi,r9 ;dbpl
	dec rcx
	jnz cbgra8_bgra16lop
	ret

;void ImageUtil_ConvB16G16R16_B16G16R16A16(UInt8 *srcPtr, UInt8 *destPtr, OSInt w, OSInt h, OSInt sbpl, OSInt dbpl);
;0 retAddr
;rdi srcPtr
;rsi destPtr
;rdx w
;rcx h
;r8 sbpl
;r9 dbpl

	align 16
ImageUtil_ConvB16G16R16_B16G16R16A16:
_ImageUtil_ConvB16G16R16_B16G16R16A16:
	lea rax,[rdx+rdx*2]
	shl rax,1
	sub r8,rax ;sbpl
	lea rax,[rdx*8]
	sub r9,rax ;dbpl
	
	align 16
cbgr16_bgra16lop:
	mov r10,rdx
	align 16
cbgr16_bgra16lop2:
	mov eax,dword [rdi]
	movnti dword [rsi],eax
	mov ax,word [rdi+4]
	or eax,0xffff0000
	movnti dword [rsi+4],eax
	lea rdi,[rdi+6]
	lea rsi,[rsi+8]
	dec r10
	jnz cbgr16_bgra16lop2
	
	add rdi,r8 ;sbpl
	add rsi,r9 ;dbpl
	dec rcx
	jnz cbgr16_bgra16lop
	ret

;void ImageUtil_ConvR16G16B16_B16G16R16A16(UInt8 *srcPtr, UInt8 *destPtr, OSInt w, OSInt h, OSInt sbpl, OSInt dbpl);
;0 retAddr
;rdi srcPtr
;rsi destPtr
;rdx w
;rcx h
;r8 sbpl
;r9 dbpl

	align 16
ImageUtil_ConvR16G16B16_B16G16R16A16:
_ImageUtil_ConvR16G16B16_B16G16R16A16:
	lea rax,[rdx+rdx*2]
	shl rax,1
	sub r8,rax ;sbpl
	lea rax,[rdx*8]
	sub r9,rax ;dbpl
	
	align 16
crgb16_bgra16lop:
	mov r10,rdx
	align 16
crgb16_bgra16lop2:
	mov ax,word [rdi+2]
	shl eax,16
	mov ax,word [rdi+4]
	movnti dword [rsi],eax
	mov ax,word [rdi]
	or eax,0xffff0000
	movnti dword [rsi+4],eax
	lea rdi,[rdi+6]
	lea rsi,[rsi+8]
	dec r10
	jnz crgb16_bgra16lop2
	
	add rdi,r8 ;sbpl
	add rsi,r9 ;dbpl
	dec rcx
	jnz crgb16_bgra16lop
	ret

;void ImageUtil_ConvR16G16B16A16_B16G16R16A16(UInt8 *srcPtr, UInt8 *destPtr, OSInt w, OSInt h, OSInt sbpl, OSInt dbpl);
;0 retAddr
;rdi srcPtr
;rsi destPtr
;rdx w
;rcx h
;r8 sbpl
;r9 dbpl

	align 16
ImageUtil_ConvR16G16B16A16_B16G16R16A16:
_ImageUtil_ConvR16G16B16A16_B16G16R16A16:
	lea rax,[rdx*8]
	sub r8,rax ;sbpl
	sub r9,rax ;dbpl
	
	align 16
crgba16_bgra16lop:
	mov r10,rdx
	align 16
crgba16_bgra16lop2:
	movq xmm0,[rdi]
	pshuflw xmm0,xmm0,0xc6
	movq [rsi],xmm0
	lea rdi,[rdi+8]
	lea rsi,[rsi+8]
	dec r10
	jnz crgba16_bgra16lop2
	
	add rdi,r8 ;sbpl
	add rsi,r9 ;dbpl
	dec rcx
	jnz crgba16_bgra16lop
	ret

;void ImageUtil_ConvA2B10G10R10_B16G16R16A16(UInt8 *srcPtr, UInt8 *destPtr, OSInt w, OSInt h, OSInt sbpl, OSInt dbpl);
;0 retAddr
;rdi srcPtr
;rsi destPtr
;rdx w
;rcx h
;r8 sbpl
;r9 dbpl
	align 16
ImageUtil_ConvA2B10G10R10_B16G16R16A16:
_ImageUtil_ConvA2B10G10R10_B16G16R16A16:
	mov r11,rbx
	lea rax,[rdx*4]
	sub r8,rax ;sbpl
	lea rax,[rdx*8]
	sub r9,rax ;dbpl
	
	align 16
ca2b10g10r10_bgra16lop:
	mov r10,rdx
	align 16
ca2b10g10r10_bgra16lop2:
	mov eax,dword [rdi]
	mov ebx,eax
	shl eax,6
	shr ebx,4
	and bx,0x3f
	or ax,bx
	mov word [rsi+4],ax
	mov ax,0
	shr eax,10
	mov bx,ax
	shr bx,10
	or ax,bx
	mov word [rsi+2],ax
	shr ebx,10
	mov ax,bx
	shr bx,10
	or ax,bx
	mov word [rsi+0],ax
	mov bx,0
	shr ebx,2
	mov eax,ebx
	shr ebx,2
	or ax,bx
	mov bx,ax
	shr bx,4
	or ax,bx
	mov al,ah
	mov word [rsi+6],ax
	lea rdi,[rdi+4]
	lea rsi,[rdi+8]
	dec r10
	jnz ca2b10g10r10_bgra16lop2
	
	add rdi,r8 ;sbpl
	add rsi,r9 ;dbpl
	dec r9
	jnz ca2b10g10r10_bgra16lop
	mov rbx,r11
	ret

;void ImageUtil_ConvFB32G32R32A32_B16G16R16A16(UInt8 *srcPtr, UInt8 *destPtr, OSInt w, OSInt h, OSInt sbpl, OSInt dbpl);
;0 retAddr
;rdi srcPtr
;rsi destPtr
;rdx w
;rcx h
;r8 sbpl
;r9 dbpl
	align 16
ImageUtil_ConvFB32G32R32A32_B16G16R16A16:
_ImageUtil_ConvFB32G32R32A32_B16G16R16A16:
	lea rax,[rdx*8]
	sub r9,rax ;dbpl
	lea rax,[rax*2]
	sub r8,rax ;sbpl
	mov rax,65535
	cvtsi2ss xmm1,rax
	mov eax,32768
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
	mov r10,rdx
	align 16
cfbgra32_bgra16lop2:
	movups xmm0,[rdi]
	mulps xmm0,xmm1
	cvtps2dq xmm0,xmm0
	psubd xmm0,xmm2
	packssdw xmm0,xmm0
	paddw xmm0,xmm3
	movq [rsi],xmm0
	lea rdi,[rdi+16]
	lea rsi,[rsi+8]
	dec r10
	jnz cfbgra32_bgra16lop2
	
	add rdi,r8 ;sbpl
	add rsi,r9 ;dbpl
	dec rcx
	jnz cfbgra32_bgra16lop
	ret

;void ImageUtil_ConvFR32G32B32A32_B16G16R16A16(UInt8 *srcPtr, UInt8 *destPtr, OSInt w, OSInt h, OSInt sbpl, OSInt dbpl);
;0 retAddr
;rdi srcPtr
;rsi destPtr
;rdx w
;rcx h
;r8 sbpl
;r9 dbpl
	align 16
ImageUtil_ConvFR32G32B32A32_B16G16R16A16:
_ImageUtil_ConvFR32G32B32A32_B16G16R16A16:
	lea rax,[rdx*8]
	sub r9,rax ;dbpl
	lea rax,[rax*2]
	sub r8,rax ;sbpl
	mov rax,65535
	cvtsi2ss xmm1,rax
	mov eax,32768
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
	mov r10,rdx
	align 16
cfrgba32_bgra16lop2:
	movups xmm0,[rdi]
	pshufd xmm0, xmm0, 0xc6
	mulps xmm0,xmm1
	cvtps2dq xmm0,xmm0
	psubd xmm0,xmm2
	packssdw xmm0,xmm0
	paddw xmm0,xmm3
	movq [rsi],xmm0
	lea rdi,[rdi+16]
	lea rsi,[rsi+8]
	dec r10
	jnz cfrgba32_bgra16lop2
	
	add rdi,r8 ;sbpl
	add rsi,r9 ;dbpl
	dec rcx
	jnz cfrgba32_bgra16lop
	ret

;void ImageUtil_ConvFB32G32R32_B16G16R16A16(UInt8 *srcPtr, UInt8 *destPtr, OSInt w, OSInt h, OSInt sbpl, OSInt dbpl);
;0 retAddr
;rdi srcPtr
;rsi destPtr
;rdx w
;rcx h
;r8 sbpl
;r9 dbpl
	align 16
ImageUtil_ConvFB32G32R32_B16G16R16A16:
_ImageUtil_ConvFB32G32R32_B16G16R16A16:
	lea rax,[rdx*8]
	sub r9,rax ;dbpl
	lea rax,[rcx*4+rax]
	sub r8,rax ;sbpl
	mov rax,65535
	cvtsi2ss xmm1,rax
	mov eax,32768
	movd xmm2,eax
	unpcklps xmm1,xmm1
	unpcklps xmm1,xmm1
	punpckldq xmm2,xmm2
	punpckldq xmm2,xmm2
	movdqa xmm3,xmm2
	pslld xmm3,16
	por xmm3,xmm2
	
	align 16
cfbgr32_bgra16lop:
	mov r10,rdx
	align 16
cfbgr32_bgra16lop2:
	movq xmm0,[rdi]
	movss xmm4,[rdi+8]
	punpcklqdq xmm0,xmm4
	mulps xmm0,xmm1
	cvtps2dq xmm0,xmm0
	psubd xmm0,xmm2
	packssdw xmm0,xmm0
	paddw xmm0,xmm3
	movq [rsi],xmm0
	mov word [rsi+6],0xffff
	lea rdi,[rdi+12]
	lea rsi,[rsi+8]
	dec r10
	jnz cfbgr32_bgra16lop2
	
	add rdi,r8 ;sbpl
	add rsi,r9 ;dbpl
	dec rcx
	jnz cfbgr32_bgra16lop
	ret

;void ImageUtil_ConvFR32G32B32_B16G16R16A16(UInt8 *srcPtr, UInt8 *destPtr, OSInt w, OSInt h, OSInt sbpl, OSInt dbpl);
;0 retAddr
;rdi srcPtr
;rsi destPtr
;rdx w
;rcx h
;r8 sbpl
;r9 dbpl
	align 16
ImageUtil_ConvFR32G32B32_B16G16R16A16:
_ImageUtil_ConvFR32G32B32_B16G16R16A16:
	lea rax,[rdx*8]
	sub r9,rax ;dbpl
	lea rax,[rcx*4+rax]
	sub r8,rax ;sbpl
	mov rax,65535
	cvtsi2ss xmm1,rax
	mov eax,32768
	movd xmm2,eax
	unpcklps xmm1,xmm1
	unpcklps xmm1,xmm1
	punpckldq xmm2,xmm2
	punpckldq xmm2,xmm2
	movdqa xmm3,xmm2
	pslld xmm3,16
	por xmm3,xmm2
	
	align 16
cfrgb32_bgra16lop:
	mov r10,rdx
	align 16
cfrgb32_bgra16lop2:
	movq xmm0,[rdi]
	movss xmm4,[rdi+8]
	punpcklqdq xmm0,xmm4
	pshufd xmm0, xmm0, 0xc6
	mulps xmm0,xmm1
	cvtps2dq xmm0,xmm0
	psubd xmm0,xmm2
	packssdw xmm0,xmm0
	paddw xmm0,xmm3
	movq [rsi],xmm0
	mov word [rsi+6],0xffff
	lea rdi,[rdi+12]
	lea rsi,[rsi+8]
	dec r10
	jnz cfrgb32_bgra16lop2
	
	add rdi,r8 ;sbpl
	add rsi,r9 ;dbpl
	dec rcx
	jnz cfrgb32_bgra16lop
	ret

;void ImageUtil_ConvFW32A32_B16G16R16A16(UInt8 *srcPtr, UInt8 *destPtr, OSInt w, OSInt h, OSInt sbpl, OSInt dbpl);
;0 retAddr
;rdi srcPtr
;rsi destPtr
;rdx w
;rcx h
;r8 sbpl
;r9 dbpl
	align 16
ImageUtil_ConvFW32A32_B16G16R16A16:
_ImageUtil_ConvFW32A32_B16G16R16A16:
	lea rax,[rdx*8]
	sub r9,rax ;dbpl
	sub r8,rax ;sbpl
	mov rax,65535
	cvtsi2ss xmm1,rax
	mov eax,32768
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
	mov r10,rdx
	align 16
cfwa32_bgra16lop2:
	movq xmm0,[rdi]
	mulps xmm0,xmm1
	cvtps2dq xmm0,xmm0
	psubd xmm0,xmm2
	packssdw xmm0,xmm0
	paddw xmm0,xmm3
	movd eax,xmm0
	mov word [rsi],ax
	mov word [rsi+2],ax
	mov dword [rsi+4],eax
	lea rdi,[rdi+8]
	lea rsi,[rsi+8]
	dec r10
	jnz cfwa32_bgra16lop2
	
	add rdi,r8 ;sbpl
	add rsi,r9 ;dbpl
	dec rcx
	jnz cfwa32_bgra16lop
	ret

;void ImageUtil_ConvFW32_B16G16R16A16(UInt8 *srcPtr, UInt8 *destPtr, OSInt w, OSInt h, OSInt sbpl, OSInt dbpl);
;0 retAddr
;rdi srcPtr
;rsi destPtr
;rdx w
;rcx h
;r8 sbpl
;r9 dbpl
	align 16
ImageUtil_ConvFW32_B16G16R16A16:
_ImageUtil_ConvFW32_B16G16R16A16:
	lea rax,[rdx*4]
	sub r8,rax ;sbpl
	lea rax,[rdx*8]
	sub r9,rax ;dbpl
	mov rax,65535
	cvtsi2ss xmm1,rax
	mov eax,32768
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
	mov r10,rdx
	align 16
cfw32_bgra16lop2:
	movss xmm0,[rdi]
	mulps xmm0,xmm1
	cvtps2dq xmm0,xmm0
	psubd xmm0,xmm2
	packssdw xmm0,xmm0
	paddw xmm0,xmm3
	movd eax,xmm0
	mov word [rsi],ax
	mov word [rsi+2],ax
	mov word [rsi+4],ax
	mov word [rsi+6],0xffff
	lea rdi,[rdi+4]
	lea rsi,[rsi+8]
	dec r10
	jnz cfw32_bgra16lop2
	
	add rdi,r8 ;sbpl
	add rsi,r9 ;dbpl
	dec rcx
	jnz cfw32_bgra16lop
	ret

;void ImageUtil_ConvW16_B8G8R8A8(UInt8 *srcPtr, UInt8 *destPtr, OSInt w, OSInt h, OSInt sbpl, OSInt dbpl);
;0 retAddr
;rdi srcPtr
;rsi destPtr
;rdx w
;rcx h
;r8 sbpl
;r9 dbpl
	align 16
ImageUtil_ConvW16_B8G8R8A8:
_ImageUtil_ConvW16_B8G8R8A8:
	mov r11,rbx
	lea rax,[rdx*2]
	sub r8,rax ;sbpl
	lea rax,[rcx*4]
	sub r9,rax ;dbpl
	
	align 16
cw16_bgra8lop:
	mov r10,rdx
	align 16
cw16_bgra8lop2:
	movzx eax,byte [rdi+1]
	mov bl,al
	or eax,0xff00
	shl eax,16
	mov ah,bl
	mov al,bl
	movnti dword [rsi],eax
	lea rdi,[rdi+2]
	lea rsi,[rsi+4]
	dec r10
	jnz cw16_bgra8lop2
	
	add rdi,r8 ;sbpl
	add rsi,r9 ;dbpl
	dec rcx
	jnz cw16_bgra8lop
	mov rbx,r11
	ret

;void ImageUtil_ConvW16A16_B8G8R8A8(UInt8 *srcPtr, UInt8 *destPtr, OSInt w, OSInt h, OSInt sbpl, OSInt dbpl);
;0 retAddr
;rdi srcPtr
;rsi destPtr
;rdx w
;rcx h
;r8 sbpl
;r9 dbpl
	align 16
ImageUtil_ConvW16A16_B8G8R8A8:
_ImageUtil_ConvW16A16_B8G8R8A8:
	mov r11,rbx
	lea rax,[rdx*4]
	sub r8,rax ;sbpl
	lea rax,[rdx*4]
	sub r9,rax ;dbpl
	
	align 16
cwa16_bgra8lop:
	mov r10,rdx
	align 16
cwa16_bgra8lop2:
	movzx eax,byte [rdi+1]
	mov bl,al
	mov ah,byte [rdi+3]
	shl eax,16
	mov ah,bl
	mov al,bl
	movnti dword [rsi],eax
	lea rdi,[rdi+4]
	lea rsi,[rsi+4]
	dec r10
	jnz cwa16_bgra8lop2
	
	add rdi,r8 ;sbpl
	add rsi,r9 ;dbpl
	dec rcx
	jnz cwa16_bgra8lop
	mov rbx,r11
	ret

;void ImageUtil_ConvW8A8_B8G8R8A8(UInt8 *srcPtr, UInt8 *destPtr, OSInt w, OSInt h, OSInt sbpl, OSInt dbpl);
;0 retAddr
;rdi srcPtr
;rsi destPtr
;rdx w
;rcx h
;r8 sbpl
;r9 dbpl
	align 16
ImageUtil_ConvW8A8_B8G8R8A8:
_ImageUtil_ConvW8A8_B8G8R8A8:
	mov r11,rbx
	lea rax,[rdx*2]
	sub r8,rax ;sbpl
	lea rax,[rdx*4]
	sub r9,rax ;dbpl
	
	align 16
cwa8_bgra8lop:
	mov r10,rdx
	align 16
cwa8_bgra8lop2:
	movzx eax,byte [rdi+0]
	mov bl,al
	mov ah,byte [rdi+1]
	shl eax,16
	mov ah,bl
	mov al,bl
	movnti dword [rsi],eax
	lea rdi,[rdi+2]
	lea rsi,[rsi+4]
	dec r10
	jnz cwa8_bgra8lop2
	
	add rdi,r8 ;sbpl
	add rsi,r9 ;dbpl
	dec rcx
	jnz cwa8_bgra8lop
	mov rbx,r11
	ret

;void ImageUtil_ConvW16_B16G16R16A16(UInt8 *srcPtr, UInt8 *destPtr, OSInt w, OSInt h, OSInt sbpl, OSInt dbpl);
;0 retAddr
;rdi srcPtr
;rsi destPtr
;rdx w
;rcx h
;r8 sbpl
;r9 dbpl
	align 16
ImageUtil_ConvW16_B16G16R16A16:
_ImageUtil_ConvW16_B16G16R16A16:
	lea rax,[rdx*2]
	sub r8,rax ;sbpl
	lea rax,[rdx*8]
	sub r9,rax ;dbpl
	
	align 16
cw16_bgra16lop:
	mov r10,rdx
	align 16
cw16_bgra16lop2:
	mov ax,word [rdi]
	or eax,0xffff0000
	mov word [rsi],ax
	mov word [rsi+2],ax
	mov dword [rsi+4],eax
	lea rdi,[rdi+2]
	lea rsi,[rsi+8]
	dec r10
	jnz cw16_bgra16lop2
	
	add rdi,r8 ;sbpl
	add rsi,r9 ;dbpl
	dec rcx
	jnz cw16_bgra16lop
	ret

;void ImageUtil_ConvW16A16_B16G16R16A16(UInt8 *srcPtr, UInt8 *destPtr, OSInt w, OSInt h, OSInt sbpl, OSInt dbpl);
;0 retAddr
;rdi srcPtr
;rsi destPtr
;rdx w
;rcx h
;r8 sbpl
;r9 dbpl
	align 16
ImageUtil_ConvW16A16_B16G16R16A16:
_ImageUtil_ConvW16A16_B16G16R16A16:
	lea rax,[rdx*4]
	sub r8,rax ;sbpl
	lea rax,[rdx*8]
	sub r9,rax ;dbpl
	
	align 16
cwa16_bgra16lop:
	mov r10,rdx
	align 16
cwa16_bgra16lop2:
	mov eax,dword [rdi]
	mov word [rsi],ax
	mov word [rsi+2],ax
	mov dword [rsi+4],eax
	lea rdi,[rdi+4]
	lea rsi,[rsi+8]
	dec r10
	jnz cwa16_bgra16lop2
	
	add rdi,r8 ;sbpl
	add rsi,r9 ;dbpl
	dec rcx
	jnz cwa16_bgra16lop
	ret

;void ImageUtil_ConvW8A8_B16G16R16A16(UInt8 *srcPtr, UInt8 *destPtr, OSInt w, OSInt h, OSInt sbpl, OSInt dbpl);
;0 retAddr
;rdi srcPtr
;rsi destPtr
;rdx w
;rcx h
;r8 sbpl
;r9 dbpl
	align 16
ImageUtil_ConvW8A8_B16G16R16A16:
_ImageUtil_ConvW8A8_B16G16R16A16:
	lea rax,[rdx*2]
	sub r8,rax ;sbpl
	lea rax,[rdx*8]
	sub r9,rax ;dbpl
	
	align 16
cwa8_bgra16lop:
	mov r10,rdx
	align 16
cwa8_bgra16lop2:
	mov al,byte [rdi+1]
	mov ah,al
	shl eax,16
	mov al,byte [rdi+0]
	mov ah,al
	mov word [rsi],ax
	mov word [rsi+2],ax
	mov dword [rsi+4],eax
	lea rdi,[rdi+2]
	lea rsi,[rsi+8]
	dec r10
	jnz cwa8_bgra16lop2
	
	add rdi,r8 ;sbpl
	add rsi,r9 ;dbpl
	dec rcx
	jnz cwa8_bgra16lop
	ret

;void ImageUtil_ConvP1_P8(UInt8 *srcPtr, UInt8 *destPtr, OSInt w, OSInt h, OSInt sbpl, OSInt dbpl);
;0 retAddr
;rdi srcPtr
;rsi destPtr
;rdx w
;rcx h
;r8 sbpl
	align 16
ImageUtil_ConvP1_P8:
_ImageUtil_ConvP1_P8:
	mov r9,rdx
	shr rdx,3
	jz cp1_p8lop
	sub r8,rdx ;sbpl
	and r9,7
	align 16
cp1_p8lop2:
	push rdx
	align 16
cp1_p8lop3:
	mov al,byte [rdi]
	test al,0x80
	setne byte [rsi]
	test al,0x40
	setne byte [rsi+1]
	test al,0x20
	setne byte [rsi+2]
	test al,0x10
	setne byte [rsi+3]
	test al,0x8
	setne byte [rsi+4]
	test al,0x4
	setne byte [rsi+5]
	test al,0x2
	setne byte [rsi+6]
	test al,0x1
	setne byte [rsi+7]
	lea rdi,[rdi+1]
	lea rsi,[rsi+8]
	dec rdx
	jnz cp1_p8lop3
	mov rdx,r9
	mov al,byte [rdi]
	align 16
cp1_p8lop3b:
	test al,0x80
	setne byte [rsi]
	lea rsi,[rsi+1]
	shl al,1
	dec rdx
	jnz cp1_p8lop3b
	pop rdx
	add rdi,r8
	dec rcx
	jnz cp1_p8lop2
	ret

	align 16
cp1_p8lop:
	sub r8,rdx ;sbpl

	align 16
cp1_p8lop4:
	push rdx
	align 16
cp1_p8lop5:
	mov al,byte [rdi]
	test al,0x80
	setne byte [rsi]
	test al,0x40
	setne byte [rsi+1]
	test al,0x20
	setne byte [rsi+2]
	test al,0x10
	setne byte [rsi+3]
	test al,0x8
	setne byte [rsi+4]
	test al,0x4
	setne byte [rsi+5]
	test al,0x2
	setne byte [rsi+6]
	test al,0x1
	setne byte [rsi+7]
	lea rdi,[rdi+1]
	lea rsi,[rsi+8]
	dec rdx
	jnz cp1_p8lop5
	pop rdx
	add rdi,r8
	dec rcx
	jnz cp1_p8lop4
	ret

;void ImageUtil_Rotate32_CW90(UInt8 *srcPtr, UInt8 *destPtr, OSInt srcWidth, OSInt srcHeight, OSInt sbpl, OSInt dbpl);
;0 retAddr
;rdi srcPtr
;rsi destPtr
;rdx srcWidth
;rcx srcHeight
;r8 sbpl
;r9 dbpl
	align 16
ImageUtil_Rotate32_CW90:
_ImageUtil_Rotate32_CW90:
	mov r11,rdx ;srcWidth
	mov r10,rcx ;srcHeight
	mov rax,r8 ;sbpl
	mul rcx ;srcHeight
	add rdi,rax ;srcPtr
	lea rcx,[r10*4]
	sub r9,rcx ;dbpl
	mov rdx,r11

	align 16
r32cw90lop:
	mov r11,r10 ;srcHeight
	mov rcx,rdi ;srcPtr
	
	align 16
r32cw90lop2:
	sub rcx,r8 ;sbpl
	mov eax,dword [rcx]
	mov dword [rsi],eax
	lea rsi,[rsi+4]
	dec r11
	jnz r32cw90lop2
	lea rdi,[rdi+4] ;srcPtr
	add rsi,r9 ;dbpl
	dec rdx
	jnz r32cw90lop
	ret

;void ImageUtil_Rotate32_CW180(UInt8 *srcPtr, UInt8 *destPtr, OSInt srcWidth, OSInt srcHeight, OSInt sbpl, OSInt dbpl);
;0 retAddr
;rdi srcPtr
;rsi destPtr
;rdx srcWidth
;rcx srcHeight
;r8 sbpl
;r9 dbpl

	align 16
ImageUtil_Rotate32_CW180:
_ImageUtil_Rotate32_CW180:
	mov rax,r8 ;sbpl
	imul rax,rcx ;srcHeight
	add rdi,rax ;srcPtr
	lea rax,[rdx*4]
	sub r8,rax ;sbpl
	sub r9,rax ;dbpl

	align 16
r32cw180lop:
	mov r10,rdx ;srcWidth
	
	align 16
r32cw180lop2:
	lea rdi,[rdi-4]
	mov eax,dword [rdi]
	mov dword [rsi],eax
	lea rsi,[rsi+4]
	dec r10
	jnz r32cw180lop2
	
	sub rdi,r8 ;sbpl
	add rsi,r9 ;dbpl
	dec rcx
	jnz r32cw180lop
	
	ret

;void ImageUtil_Rotate32_CW270(UInt8 *srcPtr, UInt8 *destPtr, OSInt srcWidth, OSInt srcHeight, OSInt sbpl, OSInt dbpl);
;0 retAddr
;rdi srcPtr
;rsi destPtr
;rdx srcWidth
;rcx srcHeight
;r8 sbpl
;r9 dbpl
	align 16
ImageUtil_Rotate32_CW270:
_ImageUtil_Rotate32_CW270:
	mov r10,rcx ;srcHeight
	add rdi,r8 ;srcPtr += sbpl
	lea rax,[rcx*4] ;srcHeight
	sub r9,rax ;dbpl
	
	align 16
r32cw270lop:
	lea rdi,[rdi-4] ;srcPtr
	mov r11,r10 ;srcHeight
	mov rcx,rdi ;srcPtr
	
	align 16
r32cw270lop2:
	mov eax,dword [rcx]
	mov dword [rsi],eax
	add rcx,r8 ;sbpl
	lea rsi,[rsi+4]
	dec r11
	jnz r32cw270lop2
	
	add rsi,r9 ;dbpl
	dec rdx
	jnz r32cw270lop
	
	ret

;void ImageUtil_Rotate64_CW90(UInt8 *srcPtr, UInt8 *destPtr, OSInt srcWidth, OSInt srcHeight, OSInt sbpl, OSInt dbpl);
;0 retAddr
;rdi srcPtr
;rsi destPtr
;rdx srcWidth
;rcx srcHeight
;r8 sbpl
;r9 dbpl

	align 16
ImageUtil_Rotate64_CW90:
_ImageUtil_Rotate64_CW90:
	mov r11,rdx ;srcWidth
	mov r10,rcx ;srcHeight
	mov rax,r8 ;sbpl
	mul rcx ;srcHeight
	add rdi,rax ;srcPtr
	lea rcx,[r10*8]
	sub r9,rcx ;dbpl
	mov rdx,r11 ;srcWidth

	align 16
r64cw90lop:
	mov r11,r10 ;srcHeight
	mov rcx,rdi ;srcPtr
	
	align 16
r64cw90lop2:
	sub rcx,r8 ;sbpl
	mov rax,qword [rcx]
	mov qword [rsi],rax
	lea rsi,[rsi+8]
	dec r11
	jnz r64cw90lop2
	lea rdi,[rdi+8] ;srcPtr
	add rsi,r9 ;dbpl
	dec rdx
	jnz r64cw90lop
	ret

;void ImageUtil_Rotate64_CW180(UInt8 *srcPtr, UInt8 *destPtr, OSInt srcWidth, OSInt srcHeight, OSInt sbpl, OSInt dbpl);
;0 retAddr
;rdi srcPtr
;rsi destPtr
;rdx srcWidth
;rcx srcHeight
;r8 sbpl
;r9 dbpl

	align 16
ImageUtil_Rotate64_CW180:
_ImageUtil_Rotate64_CW180:
	mov rax,r8 ;sbpl
	imul rax,rcx ;srcHeight
	add rdi,rax ;srcPtr
	lea rax,[rdx*8]
	sub r8,rax ;sbpl
	sub r9,rax ;dbpl

	align 16
r64cw180lop:
	mov r10,rdx ;srcWidth
	
	align 16
r64cw180lop2:
	lea rdi,[rdi-8]
	mov rax,qword [rdi]
	mov qword [rsi],rax
	lea rsi,[rsi+8]
	dec r10
	jnz r64cw180lop2
	
	sub rdi,r8 ;sbpl
	add rsi,r9 ;dbpl
	dec rcx
	jnz r64cw180lop
	
	ret

;void ImageUtil_Rotate64_CW270(UInt8 *srcPtr, UInt8 *destPtr, OSInt srcWidth, OSInt srcHeight, OSInt sbpl, OSInt dbpl);
;0 retAddr
;rdi srcPtr
;rsi destPtr
;rdx srcWidth
;rcx srcHeight
;r8 sbpl
;r9 dbpl

	align 16
ImageUtil_Rotate64_CW270:
_ImageUtil_Rotate64_CW270:
	mov r10,rcx ;srcHeight
	add rdi,r8 ;sbpl
	lea rax,[rcx*8] ;srcHeight
	sub r9,rax ;dbpl
	
	align 16
r64cw270lop:
	lea rdi,[rdi-8] ;srcPtr
	mov r11,r10 ;srcHeight
	mov rcx,rdi ;srcPtr
	
	align 16
r64cw270lop2:
	mov rax,qword [rcx]
	mov qword [rsi],rax
	add rcx,r8 ;sbpl
	lea rsi,[rsi+8]
	dec r11
	jnz r64cw270lop2
	
	add rsi,r9 ;dbpl
	dec rdx
	jnz r64cw270lop
	
	ret

;void ImageUtil_HFlip32(UInt8 *inPt, UInt8 *outPt, UOSInt width, UOSInt height, UOSInt sbpl, UOSInt dbpl, Bool upsideDown);
;0 retAddr
;rdi srcPtr
;rsi destPtr
;rdx srcWidth
;rcx srcHeight
;r8 sbpl
;r9 dbpl
;8 upsideDown

	align 16
ImageUtil_HFlip32:
_ImageUtil_HFlip32:
	mov r10,rdx
	cmp byte [rsp+8],0
	jz hf32lop

	neg r9
	lea rax,[rcx-1]
	imul rax,r9
	add rsi,rax

	align 16
hf32lop:
	lea rax,[r10*4]
	sub r9,rax

	align 16
hf32lop2:
	mov rdx,r10

	align 16
hf32lop3:
	dec rdx
	mov eax,dword [rdi+rdx*4]
	mov dword [rsi],eax
	lea rsi,[rsi+4]
	jnz hf32lop3
	
	add rdi,r8
	add rsi,r9
	dec rcx
	jnz hf32lop2
	ret

;void ImageUtil_HFRotate32_CW90(UInt8 *srcPtr, UInt8 *destPtr, OSInt srcWidth, OSInt srcHeight, OSInt sbpl, OSInt dbpl);
;0 retAddr
;rdi srcPtr
;rsi destPtr
;rdx srcWidth
;rcx srcHeight
;r8 sbpl
;r9 dbpl
	align 16
ImageUtil_HFRotate32_CW90:
_ImageUtil_HFRotate32_CW90:
	mov r10,rcx ;srcHeight
	mov rax,r8 ;sbpl
	imul rax,rcx ;srcHeight
	lea rdi,[rdi+rax] ;srcPtr
	lea rdi,[rdi+rdx*4] ;srcPtr += sbpl * srcHeight + srcWidth * 4
	lea rcx,[r10*4]
	sub r9,rcx ;dbpl

	align 16
hfr32cw90lop:
	lea rdi,[rdi-4] ;srcPtr
	mov r11,r10 ;srcHeight
	mov rcx,rdi ;srcPtr
	
	align 16
hfr32cw90lop2:
	sub rcx,r8 ;sbpl
	mov eax,dword [rcx]
	mov dword [rsi],eax
	lea rsi,[rsi+4]
	dec r11
	jnz hfr32cw90lop2
	add rsi,r9 ;dbpl
	dec rdx
	jnz hfr32cw90lop
	ret

;void ImageUtil_HFRotate32_CW180(UInt8 *srcPtr, UInt8 *destPtr, OSInt srcWidth, OSInt srcHeight, OSInt sbpl, OSInt dbpl);
;0 retAddr
;rdi srcPtr
;rsi destPtr
;rdx srcWidth
;rcx srcHeight
;r8 sbpl
;r9 dbpl

	align 16
ImageUtil_HFRotate32_CW180:
_ImageUtil_HFRotate32_CW180:
	xchg rsi,rdi
	mov rax,r8 ;sbpl
	imul rax,rcx ;srcHeight
	add rsi,rax ;srcPtr
	mov r10,rcx ;srcHeight
	lea rax,[rdx*4]
	sub r9,rax ;dbpl
	add r8,rax ;sbpl
	add rsi,rax

	align 16
hfr32cw180lop:
	sub rsi,r8 ;sbpl
	mov rcx,rdx ;srcWidth
	rep movsd
	add rdi,r9 ;dbpl
	dec r10
	jnz hfr32cw180lop
	ret

;void ImageUtil_HFRotate32_CW270(UInt8 *srcPtr, UInt8 *destPtr, OSInt srcWidth, OSInt srcHeight, OSInt sbpl, OSInt dbpl);
;0 retAddr
;rdi srcPtr
;rsi destPtr
;rdx srcWidth
;rcx srcHeight
;r8 sbpl
;r9 dbpl
	align 16
ImageUtil_HFRotate32_CW270:
_ImageUtil_HFRotate32_CW270:
	mov r10,rcx ;srcHeight
	lea rax,[rcx*4] ;srcHeight
	sub r9,rax ;dbpl
	
	align 16
hfr32cw270lop:
	mov r11,r10 ;srcHeight
	mov rcx,rdi ;srcPtr
	
	align 16
hfr32cw270lop2:
	mov eax,dword [rcx]
	mov dword [rsi],eax
	add rcx,r8 ;sbpl
	lea rsi,[rsi+4]
	dec r11
	jnz hfr32cw270lop2
	
	lea rdi,[rdi+4] ;srcPtr
	add rsi,r9 ;dbpl
	dec rdx
	jnz hfr32cw270lop
	
	ret

;void ImageUtil_HFlip64(UInt8 *inPt, UInt8 *outPt, UOSInt width, UOSInt height, UOSInt sbpl, UOSInt dbpl, Bool upsideDown);
;0 retAddr
;rdi srcPtr
;rsi destPtr
;rdx srcWidth
;rcx srcHeight
;r8 sbpl
;r9 dbpl
;8 upsideDown

	align 16
ImageUtil_HFlip64:
_ImageUtil_HFlip64:
	mov r10,rdx
	cmp byte [rsp+8],0
	jz hf64lop

	neg r9
	lea rax,[rcx-1]
	imul rax,r9
	add rsi,rax

	align 16
hf64lop:
	lea rax,[r10*8]
	sub r9,rax

	align 16
hf64lop2:
	mov rdx,r10

	align 16
hf64lop3:
	dec rdx
	mov rax,qword [rdi+rdx*8]
	mov qword [rsi],rax
	lea rsi,[rsi+8]
	jnz hf64lop3
	
	add rdi,r8
	add rsi,r9
	dec rcx
	jnz hf64lop2
	ret

;void ImageUtil_HFRotate64_CW90(UInt8 *srcPtr, UInt8 *destPtr, OSInt srcWidth, OSInt srcHeight, OSInt sbpl, OSInt dbpl);
;0 retAddr
;rdi srcPtr
;rsi destPtr
;rdx srcWidth
;rcx srcHeight
;r8 sbpl
;r9 dbpl
	align 16
ImageUtil_HFRotate64_CW90:
_ImageUtil_HFRotate64_CW90:
	mov r10,rcx ;srcHeight
	mov rax,r8 ;sbpl
	imul rax,rcx ;srcHeight
	lea rdi,[rdi+rax] ;srcPtr
	lea rdi,[rdi+rdx*8] ;srcPtr += sbpl * srcHeight + srcWidth * 4
	lea rcx,[r10*8]
	sub r9,rcx ;dbpl

	align 16
hfr64cw90lop:
	lea rdi,[rdi-8] ;srcPtr
	mov r11,r10 ;srcHeight
	mov rcx,rdi ;srcPtr
	
	align 16
hfr64cw90lop2:
	sub rcx,r8 ;sbpl
	mov rax,qword [rcx]
	mov qword [rsi],rax
	lea rsi,[rsi+8]
	dec r11
	jnz hfr64cw90lop2
	add rsi,r9 ;dbpl
	dec rdx
	jnz hfr64cw90lop
	ret

;void ImageUtil_HFRotate64_CW180(UInt8 *srcPtr, UInt8 *destPtr, OSInt srcWidth, OSInt srcHeight, OSInt sbpl, OSInt dbpl);
;0 retAddr
;rdi srcPtr
;rsi destPtr
;rdx srcWidth
;rcx srcHeight
;r8 sbpl
;r9 dbpl

	align 16
ImageUtil_HFRotate64_CW180:
_ImageUtil_HFRotate64_CW180:
	xchg rsi,rdi
	mov rax,r8 ;sbpl
	imul rax,rcx ;srcHeight
	add rsi,rax ;srcPtr
	mov r10,rcx ;srcHeight
	lea rax,[rdx*8]
	add r8,rax ;sbpl
	sub r9,rax ;dbpl

	align 16
hfr64cw180lop:
	sub rsi,r8 ;sbpl
	mov rcx,rdx ;srcWidth
	rep movsq
	add rdi,r9 ;dbpl
	dec r10
	jnz hfr64cw180lop
	ret

;void ImageUtil_HFRotate64_CW270(UInt8 *srcPtr, UInt8 *destPtr, OSInt srcWidth, OSInt srcHeight, OSInt sbpl, OSInt dbpl);
;0 retAddr
;rdi srcPtr
;rsi destPtr
;rdx srcWidth
;rcx srcHeight
;r8 sbpl
;r9 dbpl
	align 16
ImageUtil_HFRotate64_CW270:
_ImageUtil_HFRotate64_CW270:
	mov r10,rcx ;srcHeight
	lea rax,[rcx*8] ;srcHeight
	sub r9,rax ;dbpl
	
	align 16
hfr64cw270lop:
	mov r11,r10 ;srcHeight
	mov rcx,rdi ;srcPtr
	
	align 16
hfr64cw270lop2:
	mov eax,dword [rcx]
	mov dword [rsi],eax
	add rcx,r8 ;sbpl
	lea rsi,[rsi+8]
	dec r11
	jnz hfr64cw270lop2
	
	lea rdi,[rdi+8] ;srcPtr
	add rsi,r9 ;dbpl
	dec rdx
	jnz hfr64cw270lop
	
	ret

;void ImageUtil_CopyShiftW(UInt8 *srcPtr, UInt8 *destPtr, OSInt byteSize, OSInt shiftCnt); //Assume aligned
;0 retAddr
;rdi srcPtr
;rsi destPtr
;rdx byteSize
;rcx shiftCnt
	align 16
ImageUtil_CopyShiftW:
_ImageUtil_CopyShiftW:
	shr rdx,4
	cmp rcx,6
	jz cswlop6
	cmp rcx,4
	jz cswlop4
	cmp rcx,5
	jz cswlop5
	cmp rcx,3
	jz cswlop3
	cmp rcx,2
	jz cswlop2
	cmp rcx,1
	jz cswlop1
	jmp cswlop0
	
	align 16
cswlop0:
	movdqa xmm0,[rdi]
	movntdq [rsi],xmm0
	lea rdi,[rdi+16]
	lea rsi,[rsi+16]
	dec rdx
	jnz cswlop0
	ret
	
	align 16
cswlop6:
	movdqa xmm0,[rdi]
	psllw xmm0,6
	movntdq [rsi],xmm0
	lea rdi,[rdi+16]
	lea rsi,[rsi+16]
	dec rdx
	jnz cswlop6
	ret

	align 16
cswlop5:
	movdqa xmm0,[rdi]
	psllw xmm0,5
	movntdq [rsi],xmm0
	lea rdi,[rdi+16]
	lea rsi,[rsi+16]
	dec rdx
	jnz cswlop5
	ret

	align 16
cswlop4:
	movdqa xmm0,[rdi]
	psllw xmm0,4
	movntdq [rsi],xmm0
	lea rdi,[rdi+16]
	lea rsi,[rsi+16]
	dec rdx
	jnz cswlop4
	ret

	align 16
cswlop3:
	movdqa xmm0,[rdi]
	psllw xmm0,3
	movntdq [rsi],xmm0
	lea rdi,[rdi+16]
	lea rsi,[rsi+16]
	dec rdx
	jnz cswlop3
	ret

	align 16
cswlop2:
	movdqa xmm0,[rdi]
	psllw xmm0,2
	movntdq [rsi],xmm0
	lea rdi,[rdi+16]
	lea rsi,[rsi+16]
	dec rdx
	jnz cswlop2
	ret

	align 16
cswlop1:
	movdqa xmm0,[rdi]
	psllw xmm0,1
	movntdq [rsi],xmm0
	lea rdi,[rdi+16]
	lea rsi,[rsi+16]
	dec rdx
	jnz cswlop1
	ret

;void ImageUtil_UVInterleaveShiftW(UInt8 *destPtr, UInt8 *uptr, UInt8 *vptr, OSInt pixelCnt, OSInt shiftCnt);
;0 retAddr
;rdi destPtr
;rsi uptr
;rdx vptr
;rcx pixelCnt
;r8 shiftCnt
	align 16
ImageUtil_UVInterleaveShiftW:
_ImageUtil_UVInterleaveShiftW:
	shr rcx,3
	cmp r8,6
	jz uviswlop6
	cmp r8,4
	jz uviswlop4
	jmp uviswlop0
	
	align 16
uviswlop6:
	movdqa xmm0,[rsi]
	psllw xmm0,6
	movdqa xmm1,[rdx]
	psllw xmm1,6
	movdqa xmm2,xmm0
	punpcklwd xmm0,xmm1
	movntdq [rdi],xmm0
	punpckhwd xmm2,xmm1
	movntdq [rdi+16],xmm2
	lea rdi,[rdi+32]
	lea rdx,[rdx+16]
	lea rsi,[rsi+16]
	dec rcx
	jnz uviswlop6
	ret

	align 16
uviswlop4:
	movdqa xmm0,[rsi]
	psllw xmm0,4
	movdqa xmm1,[rdx]
	psllw xmm1,4
	movdqa xmm2,xmm0
	punpcklwd xmm0,xmm1
	movntdq [rdi],xmm0
	punpckhwd xmm2,xmm1
	movntdq [rdi+16],xmm2
	lea rdi,[rdi+32]
	lea rdx,[rdx+16]
	lea rsi,[rsi+16]
	dec rcx
	jnz uviswlop4
	ret

	align 16
uviswlop0:
	movdqa xmm0,[rdx]
	movdqa xmm2,xmm0
	movdqa xmm1,[rdx]
	punpcklwd xmm0,xmm1
	movntdq [rcx],xmm0
	punpckhwd xmm2,xmm1
	movntdq [rcx+16],xmm2
	lea rcx,[rcx+32]
	lea rdx,[rdx+16]
	lea rdx,[rdx+16]
	dec rcx
	jnz uviswlop0
	ret

;void ImageUtil_YUV_Y416ShiftW(UInt8 *destPtr, UInt8 *yptr, UInt8 *uptr, UInt8 *vptr, OSInt pixelCnt, OSInt shiftCnt);
;0 retAddr
;rdi destPtr
;rsi yptr
;rdx uptr
;rcx vptr
;r8 pixelCnt
;r9 shiftCnt
	align 16
ImageUtil_YUV_Y416ShiftW:
_ImageUtil_YUV_Y416ShiftW:
	mov rax,-1
	shr r8,3
	movd xmm3,eax
	punpckldq xmm3,xmm3
	punpckldq xmm3,xmm3
	cmp r9,6
	jz yuvy416swlop6
	jmp yuvy416swlop0
	
	align 16
yuvy416swlop6:
	movdqu xmm0,[rsi]
	movdqu xmm1,[rdx]
	psllw xmm0,6
	movdqu xmm2,[rcx]
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
	lea rdx,[rdx+16]
	lea rcx,[rcx+16]
	dec r8
	jnz yuvy416swlop6
	jmp yuvy416swexit
	
	align 16
yuvy416swlop0:
	movdqu xmm0,[rsi]
	movdqu xmm1,[rdx]
	movdqa xmm5,xmm0
	movdqu xmm2,[rcx]
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
	lea rdx,[rdx+16]
	lea rcx,[rcx+16]
	dec r8
	jnz yuvy416swlop0
	jmp yuvy416swexit
	
	align 16
yuvy416swexit:
	ret

