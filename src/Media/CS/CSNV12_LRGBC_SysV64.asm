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

global CSNV12_LRGBC_VerticalFilterLRGB ;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
global _CSNV12_LRGBC_VerticalFilterLRGB ;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
global CSNV12_LRGBC_do_yv12rgb8 ;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
global _CSNV12_LRGBC_do_yv12rgb8 ;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
global CSNV12_LRGBC_do_yv12rgb2 ;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
global _CSNV12_LRGBC_do_yv12rgb2 ;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

;void CSNV12_LRGBC_VerticalFilterLRGB(UInt8 *inYPt, UInt8 *inUVPt, UInt8 *outPt, IntOS width, IntOS height, IntOS tap, IntOS *index, Int64 *weight, Int32 isFirst, Int32 isLast, UInt8 *csLineBuff, UInt8 *csLineBuff2, IntOS ystep, IntOS dstep, Int64 *yuv2rgb, Int64 *rgbGammaCorr)
	align 16
CSNV12_LRGBC_VerticalFilterLRGB:
_CSNV12_LRGBC_VerticalFilterLRGB:
	ret

;void CSNV12_LRGBC_do_yv12rgb8(UInt8 *yPtr, UInt8 *uvPtr, UInt8 *dest, IntOS width, IntOS height, IntOS dbpl, IntOS isFirst, IntOS isLast, UInt8 *csLineBuff, UInt8 *csLineBuff2, IntOS yBpl, IntOS uvBpl, Int64 *yuv2rgb, Int64 *rgbGammaCorr)

	align 16
CSNV12_LRGBC_do_yv12rgb8:
_CSNV12_LRGBC_do_yv12rgb8:
	ret

;void CSNV12_LRGBC_do_yv12rgb2(UInt8 *yPtr, UInt8 *uvPtr, UInt8 *dest, IntOS width, IntOS height, IntOS dbpl, IntOS isFirst, IntOS isLast, UInt8 *csLineBuff, UInt8 *csLineBuff2, IntOS yBpl, IntOS uvBpl, Int64 *yuv2rgb, Int64 *rgbGammaCorr)

	align 16
CSNV12_LRGBC_do_yv12rgb2:
_CSNV12_LRGBC_do_yv12rgb2:
	ret
