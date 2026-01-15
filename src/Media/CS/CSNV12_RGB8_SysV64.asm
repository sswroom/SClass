section .text

global CSNV12_RGB8_do_nv12rgb8
global _CSNV12_RGB8_do_nv12rgb8
global CSNV12_RGB8_do_nv12rgb2
global _CSNV12_RGB8_do_nv12rgb2

;void CSNV12_RGB8_do_nv12rgb8(UInt8 *yPtr, UInt8 *uvPtr, UInt8 *dest, IntOS width, IntOS height, IntOS dbpl, IntOS isFirst, IntOS isLast, UInt8 *csLineBuff, UInt8 *csLineBuff2, Int64 *yuv2rgb, UInt8 *rgbGammaCorr);
;0 rdi
;8 rsi
;16 rbx
;24 rbp
;32 retAddr
;rcx yPtr
;rdx uvPtr
;r8 dest
;r9 width
;72 height
;80 dbpl
;88 isFirst
;96 isLast
;104 csLineBuff
;112 csLineBuff2
;120 yuv2rgb
;128 rgbGammaCorr

	align 16
CSNV12_RGB8_do_nv12rgb8:
_CSNV12_RGB8_do_nv12rgb8:
	ret

;void CSNV12_RGB8_do_nv12rgb2(UInt8 *yPtr, UInt8 *uvPtr, UInt8 *dest, IntOS width, IntOS height, IntOS dbpl, IntOS isFirst, IntOS isLast, UInt8 *csLineBuff, UInt8 *csLineBuff2, Int64 *yuv2rgb, UInt8 *rgbGammaCorr);
;0 rdi
;8 rsi
;16 rbx
;24 rbp
;32 retAddr
;rcx yPtr
;rdx uvPtr
;r8 dest
;r9 width
;72 height
;80 dbpl
;88 isFirst
;96 isLast
;104 csLineBuff
;112 csLineBuff2
;120 yuv2rgb
;128 rgbGammaCorr

	align 16
CSNV12_RGB8_do_nv12rgb2:
_CSNV12_RGB8_do_nv12rgb2:
	ret
