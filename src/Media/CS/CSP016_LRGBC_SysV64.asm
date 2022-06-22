section .text

global CSP016_LRGBC_VerticalFilterLRGB ;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
global _CSP016_LRGBC_VerticalFilterLRGB ;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
global CSP016_LRGBC_do_yv12rgb8 ;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
global _CSP016_LRGBC_do_yv12rgb8 ;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
global CSP016_LRGBC_do_yv12rgb2 ;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
global _CSP016_LRGBC_do_yv12rgb2 ;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

;void CSP016_LRGBC_VerticalFilterLRGB(UInt8 *inYPt, UInt8 *inUVPt, UInt8 *outPt, OSInt width, OSInt height, OSInt tap, OSInt *index, Int64 *weight, Int32 isFirst, Int32 isLast, UInt8 *csLineBuff, UInt8 *csLineBuff2, OSInt ystep, OSInt dstep, Int64 *yuv2rgb, Int64 *rgbGammaCorr)

	align 16
CSP016_LRGBC_VerticalFilterLRGB:
_CSP016_LRGBC_VerticalFilterLRGB:
	ret


;void CSP016_LRGBC_do_yv12rgb8(UInt8 *yPtr, UInt8 *uvPtr, UInt8 *dest, OSInt width, OSInt height, OSInt dbpl, OSInt isFirst, OSInt isLast, UInt8 *csLineBuff, UInt8 *csLineBuff2, OSInt yBpl, OSInt uvBpl, Int64 *yuv2rgb, Int64 *rgbGammaCorr)

	align 16
CSP016_LRGBC_do_yv12rgb8:
_CSP016_LRGBC_do_yv12rgb8:
	ret

;void CSP016_LRGBC_do_yv12rgb2(UInt8 *yPtr, UInt8 *uvPtr, UInt8 *dest, OSInt width, OSInt height, OSInt dbpl, OSInt isFirst, OSInt isLast, UInt8 *csLineBuff, UInt8 *csLineBuff2, OSInt yBpl, OSInt uvBpl, Int64 *yuv2rgb, Int64 *rgbGammaCorr)

	align 16
CSP016_LRGBC_do_yv12rgb2:
_CSP016_LRGBC_do_yv12rgb2:
	ret

