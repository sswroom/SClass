section .text

global CSI420_LRGB_do_yv12rgb

;void CSI420_LRGB_do_yv12rgb(UInt8 *yPtr, UInt8 *uPtr, UInt8 *vPtr, UInt8 *dest, OSInt width, OSInt height, OSInt dbpl, OSInt isFirst, OSInt isLast, UInt8 *csLineBuff, UInt8 *csLineBuff2, Int64 *yuv2rgb, UInt8 *rgbGammaCorr);
;0 rdi
;8 rsi
;16 rbx
;24 rbp
;32 retAddr
;rcx yPtr
;rdx uPtr
;r8 vPtr
;r9 dest
;72 width
;80 height
;88 dbpl
;96 isFirst
;104 isLast
;112 csLineBuff
;120 csLineBuff2
;128 yuv2rgb
;136 rgbGammaCorr

	align 16
CSI420_LRGB_do_yv12rgb:
	ret
