section .text

global _CSI420_LRGB_do_yv12rgb

;void CSI420_LRGB_do_yv12rgb(UInt8 *yPtr, UInt8 *uPtr, UInt8 *vPtr, UInt8 *dest, OSInt width, OSInt height, OSInt dbpl, OSInt isFirst, OSInt isLast, UInt8 *csLineBuff, UInt8 *csLineBuff2, Int64 *yuv2rgb, UInt8 *rgbGammaCorr);
;0 edi
;4 esi
;8 ebx
;12 ebp
;16 retAddr
;20 yPtr
;24 uPtr
;28 vPtr
;32 dest
;36 width
;40 height
;44 dbpl
;48 isFirst
;52 isLast
;56 csLineBuff
;60 csLineBuff2
;64 yuv2rgb
;68 rgbGammaCorr

	align 16
_CSI420_LRGB_do_yv12rgb:
	ret
