#include "Stdafx.h"
#include "MyMemory.h"

extern "C" void CSNV12_RGB32C_VerticalFilterLRGB(UInt8 *inYPt, UInt8 *inUVPt, UInt8 *outPt, IntOS width, IntOS height, IntOS tap, IntOS *index, Int64 *weight, IntOS isFirst, IntOS isLast, UInt8 *csLineBuff, UInt8 *csLineBuff2, IntOS ystep, IntOS dstep, Int64 *yuv2rgb, Int64 *rgbGammaCorr)
{
}

extern "C" void CSNV12_RGB32C_do_yv12rgb8(UInt8 *yPtr, UInt8 *uvPtr, UInt8 *dest, IntOS width, IntOS height, IntOS dbpl, IntOS isFirst, IntOS isLast, UInt8 *csLineBuff, UInt8 *csLineBuff2, IntOS yBpl, IntOS uvBpl, Int64 *yuv2rgb, Int64 *rgbGammaCorr)
{
}

extern "C" void CSNV12_RGB32C_do_yv12rgb2(UInt8 *yPtr, UInt8 *uvPtr, UInt8 *dest, IntOS width, IntOS height, IntOS dbpl, IntOS isFirst, IntOS isLast, UInt8 *csLineBuff, UInt8 *csLineBuff2, IntOS yBpl, IntOS uvBpl, Int64 *yuv2rgb, Int64 *rgbGammaCorr)
{
}

