section .text

global ImageTo8Bit_CalDiff
global _ImageTo8Bit_CalDiff

;Int32 ImageTo8Bit_CalDiff(Int32 col1, Int32 col2);
;0 retAddr
;rdi col1
;rsi col2

	align 16
ImageTo8Bit_CalDiff:
_ImageTo8Bit_CalDiff:
	movd xmm0,edi
	pxor xmm3,xmm3
	movd xmm1,esi
	punpcklbw xmm0,xmm3
	punpcklbw xmm1,xmm3
	movq xmm2,xmm1
	pmaxsw xmm2,xmm0
	pminsw xmm1,xmm0
	psubw xmm2,xmm1
	pextrw rax,xmm2,0
	pextrw rdx,xmm2,1
	pextrw rcx,xmm2,2
	add rax,rdx
	add rax,rcx
	ret
