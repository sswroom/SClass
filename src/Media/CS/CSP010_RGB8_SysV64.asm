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

global CSP010_RGB8_do_p010rgb8
global _CSP010_RGB8_do_p010rgb8
global CSP010_RGB8_do_p010rgb2
global _CSP010_RGB8_do_p010rgb2

;void CSP010_RGB8_do_p010rgb8(UInt8 *yPtr, UInt8 *uvPtr, UInt8 *dest, OSInt width, OSInt height, OSInt dbpl, OSInt isFirst, OSInt isLast, UInt8 *csLineBuff, UInt8 *csLineBuff2, Int64 *yuv2rgb, UInt8 *rgbGammaCorr);
	align 16
CSP010_RGB8_do_p010rgb8:
_CSP010_RGB8_do_p010rgb8:
	ret

;void CSP010_RGB8_do_p010rgb2(UInt8 *yPtr, UInt8 *uvPtr, UInt8 *dest, OSInt width, OSInt height, OSInt dbpl, OSInt isFirst, OSInt isLast, UInt8 *csLineBuff, UInt8 *csLineBuff2, Int64 *yuv2rgb, UInt8 *rgbGammaCorr);
	align 16
CSP010_RGB8_do_p010rgb2:
_CSP010_RGB8_do_p010rgb2:
	ret
