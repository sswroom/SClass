#ifndef _SM_DOTNET_MYGRP
#define _SM_DOTNET_MYGRP
#include "Media/Resizer/LanczosResizer8_C8.h"
#include "Media/Resizer/LanczosResizerH13_8.h"
#include "Parser/ParserList.h"

namespace DotNet
{
	void GrpCropPtr32(void *pSrc, long srcX, long srcY, long srcLineW, void *pDest, long destW, long destH, long destLineW);

	public __gc class MyGrp
	{
	private:
		static Media::ColorManager *colorMgr;
		static Media::Resizer::LanczosResizer8_C8 *resizer8;
//		static Media::Resizer::LanczosResizerH13_8 *resizer16;
		static Parser::ParserList *parsers;

	private:
		static void GrpAdjustColor64(void *pSrc, void *pDest, long width, long height, long lineW, long bright, long cont);
		static void GrpCopyImageData(void *img, void *imgData);
		static void GrpDelImage(void *img);
		static void *GrpLoadImage32(WChar *fileName, Int32 *width, Int32 *height);
		static void GrpResizePtr32_32(void *pSrc, long srcW, long srcH, void *pDest, long destW, long destH);

	private:
		static int GetEncoderClsid(const WChar* format, void* pClsid);
		static void *GDIPImageFromNet(System::Drawing::Bitmap *bmp);

	public:
		static void Init(Int32 nTap);
		static void Deinit();
		static System::Drawing::Bitmap *ResizeBitmap(System::Drawing::Bitmap *bmp, System::Int32 maxWidth, System::Int32 maxHeight, System::Boolean copyExif);
		static System::Drawing::Bitmap *CropBitmap(System::Drawing::Bitmap *bmp, System::Int32 x, System::Int32 y, System::Int32 width, System::Int32 height);
		static System::Drawing::Bitmap *AdjustBmp(System::Drawing::Bitmap *bmp, System::Int32 bright, System::Int32 cont);
		static System::Boolean SaveJPGQuality(System::Drawing::Bitmap *bmp, System::String *fileName, System::Int32 quality); //true = error
		static System::Boolean SaveJPGSize(System::Drawing::Bitmap *bmp, System::String *fileName, System::Int32 targetSize); //true = error;
		static System::Boolean SaveTIFF(System::Drawing::Bitmap *bmp, System::String *fileName, System::Boolean compressed); //true = error;
		static System::Drawing::Bitmap *LoadImage(System::String *fileName);
	};
};
#endif
