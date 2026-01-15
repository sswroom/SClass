section .text

global CSP010_RGB8_do_p010rgb8
global CSP010_RGB8_do_p010rgb2

;void CSP010_RGB8_do_p010rgb8(UInt8 *yPtr, UInt8 *uvPtr, UInt8 *dest, IntOS width, IntOS height, IntOS dbpl, IntOS isFirst, IntOS isLast, UInt8 *csLineBuff, UInt8 *csLineBuff2, Int64 *yuv2rgb, UInt8 *rgbGammaCorr);
	align 16
CSP010_RGB8_do_p010rgb8:
	ret

;void CSP010_RGB8_do_p010rgb2(UInt8 *yPtr, UInt8 *uvPtr, UInt8 *dest, IntOS width, IntOS height, IntOS dbpl, IntOS isFirst, IntOS isLast, UInt8 *csLineBuff, UInt8 *csLineBuff2, Int64 *yuv2rgb, UInt8 *rgbGammaCorr);
	align 16
CSP010_RGB8_do_p010rgb2:
	ret
