section .text

global _ImageTo8Bit_CalDiff
global ImageTo8Bit_CalDiff

;Int32 ImageTo8Bit_CalDiff(Int32 col1, Int32 col2);
;0 retAddr
;4 col1
;8 col2

	align 16
_ImageTo8Bit_CalDiff:
ImageTo8Bit_CalDiff:
	movd xmm0,[esp+4]
	pxor xmm3,xmm3
	movd xmm1,[esp+8]
	punpcklbw xmm0,xmm3
	punpcklbw xmm1,xmm3
	movq xmm2,xmm1
	pmaxsw xmm2,xmm0
	pminsw xmm1,xmm0
	psubw xmm2,xmm1
	pextrw eax,xmm2,0
	pextrw edx,xmm2,1
	pextrw ecx,xmm2,2
	add eax,edx
	add eax,ecx
	ret