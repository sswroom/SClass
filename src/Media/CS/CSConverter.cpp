#include "Stdafx.h"
#include "MyMemory.h"
#include "Media/CS/CSYUY2_RGB8.h"
#include "Media/CS/CSUYVY_RGB8.h"
#include "Media/CS/CSI420_RGB32C.h"
#include "Media/CS/CSBI42_RGB32C.h"
//#include "Media/CS/CSYV12_RGB8.h"
#include "Media/CS/CSYV12_RGB32C.h"
//#include "Media/CS/CSNV12_RGB8.h"
#include "Media/CS/CSNV12_RGB32C.h"
#include "Media/CS/CSYVU9_RGB8.h"
#include "Media/CS/CSAYUV_RGB8.h"
#include "Media/CS/CSP010_RGB8.h"
#include "Media/CS/CSP016_RGB32C.h"
#include "Media/CS/CSY416_RGB32C.h"
#include "Media/CS/CSYUV420P8_RGB32C.h"
#include "Media/CS/CSAYUV444_10_RGB32C.h"
#include "Media/CS/CSYUV444P10LEP_RGB32C.h"
#include "Media/CS/CSRGB8_RGB8.h"
#include "Media/CS/CSRGB16_RGB8.h"
//#include "Media/CS/CSRGB8_LRGB.h"
//#include "Media/CS/CSRGB16_LRGB.h"
//#include "Media/CS/CSYUY2_LRGB.h"
//#include "Media/CS/CSUYVY_LRGB.h"
#include "Media/CS/CSI420_LRGBC.h"
#include "Media/CS/CSBI42_LRGBC.h"
//#include "Media/CS/CSYV12_LRGB.h"
#include "Media/CS/CSNV12_LRGBC.h"
#include "Media/CS/CSP016_LRGBC.h"
#include "Media/CS/CSP216_LRGBC.h"
#include "Media/CS/CSRGB8_LRGBC.h"
#include "Media/CS/CSRGB16_LRGBC.h"
#include "Media/CS/CSRGBF_LRGBC.h"
#include "Media/CS/CSUYVY_LRGBC.h"
#include "Media/CS/CSYUY2_LRGBC.h"
#include "Media/CS/CSYV12_LRGBC.h"
#include "Media/CS/CSY416_LRGBC.h"
#include "Media/CS/CSYUV420P8_LRGBC.h"
#include "Media/CS/CSAYUV444_10_LRGBC.h"
#include "Media/CS/CSYUV444P10LEP_LRGBC.h"
#include "Media/CS/CSYUV_Y8.h"

//http://msdn.microsoft.com/en-us/library/windows/desktop/dd206750%28v=vs.85%29.aspx
//https://msdn.microsoft.com/en-us/library/windows/desktop/bb970578%28v=vs.85%29.aspx

Media::CS::CSConverter::CSConverter(Media::ColorManagerSess *colorSess)
{
	this->colorSess = colorSess;
	if (this->colorSess)
	{
		this->colorSess->AddHandler(*this);
	}
}

Media::CS::CSConverter::~CSConverter()
{
	if (this->colorSess)
	{
		this->colorSess->RemoveHandler(*this);
	}
}

void Media::CS::CSConverter::SetPalette(UInt8 *pal)
{
}

UOSInt Media::CS::CSConverter::GetSupportedCS(Data::ArrayListUInt32 *csList)
{
	/////////////////////////////////
	// YUV8 4:4:4 AYUV
	// YUV8 4:2:2 IMC1, IMC3, IMC2, IMC4, NV12
	// YUV10 Y410, Y210, P210, P010
	// YUV16 Y416, Y216, P216, P016
	// LRGB

	UOSInt i = csList->GetCount();
	csList->SortedInsert(0);
	csList->SortedInsert(FFMT_YUV420P8);
	csList->SortedInsert(FFMT_AYUV444_10);
	csList->SortedInsert(FFMT_YUV444P10LEP);
	csList->SortedInsert(*(UInt32*)"DIBS");
	csList->SortedInsert(*(UInt32*)"NV12");
	csList->SortedInsert(*(UInt32*)"YV12");
	csList->SortedInsert(*(UInt32*)"YUY2");
	csList->SortedInsert(*(UInt32*)"YUYV");
	csList->SortedInsert(*(UInt32*)"UYVY");
	csList->SortedInsert(*(UInt32*)"YVU9");
	csList->SortedInsert(*(UInt32*)"Y422");
	csList->SortedInsert(*(UInt32*)"UYNV");
	csList->SortedInsert(*(UInt32*)"HDYC");
	csList->SortedInsert(*(UInt32*)"I420");
	csList->SortedInsert(*(UInt32*)"AYUV");
	csList->SortedInsert(*(UInt32*)"P010");
	csList->SortedInsert(*(UInt32*)"P210");
	csList->SortedInsert(*(UInt32*)"P016");
	csList->SortedInsert(*(UInt32*)"P216");
	csList->SortedInsert(*(UInt32*)"Y416");
	csList->SortedInsert(*(UInt32*)"Y800");
	csList->SortedInsert(*(UInt32*)"Y8  ");
	csList->SortedInsert(*(UInt32*)"GREY");
	return csList->GetCount() - i;
}

Bool Media::CS::CSConverter::IsSupported(UInt32 fourcc)
{
	Data::ArrayListUInt32 csList;
	GetSupportedCS(&csList);
	return csList.SortedIndexOf(fourcc) >= 0;
}

Media::CS::CSConverter *Media::CS::CSConverter::NewConverter(UInt32 srcFormat, UOSInt srcNBits, Media::PixelFormat srcPF, NN<const Media::ColorProfile> srcProfile, UInt32 destFormat, UOSInt destNBits, Media::PixelFormat destPF, NN<const Media::ColorProfile> destProfile, Media::ColorProfile::YUVType yuvType, Media::ColorManagerSess *colorSess)
{
	Media::CS::CSConverter *conv;
	if (destFormat == *(UInt32*)"LRGB")
	{
		if (srcFormat == 0 && (srcPF == Media::PF_LE_FB32G32R32A32 || srcPF == Media::PF_LE_FB32G32R32 || srcPF == Media::PF_LE_FW32A32 || srcPF == Media::PF_LE_FW32))
		{
			NEW_CLASS(conv, Media::CS::CSRGBF_LRGBC(srcNBits, srcPF, false, srcProfile, destProfile, colorSess));
			return conv;
		}
		else if (srcFormat == 0 && srcNBits <= 32 && srcPF != Media::PF_LE_W16 && srcPF != Media::PF_LE_W16A16 && srcPF != Media::PF_LE_A2B10G10R10)
		{
			NEW_CLASS(conv, Media::CS::CSRGB8_LRGBC(srcNBits, srcPF, false, srcProfile, destProfile, colorSess));
			return conv;
		}
		else if (srcFormat == 0 && srcNBits <= 64)
		{
			NEW_CLASS(conv, Media::CS::CSRGB16_LRGBC(srcNBits, srcPF, false, srcProfile, destProfile, colorSess));
			return conv;
		}
		else if (srcFormat == *(UInt32*)"DIBS" && srcNBits <= 32 && srcPF != Media::PF_LE_W16 && srcPF != Media::PF_LE_W16A16 && srcPF != Media::PF_LE_A2B10G10R10)
		{
			NEW_CLASS(conv, Media::CS::CSRGB8_LRGBC(srcNBits, srcPF, true, srcProfile, destProfile, colorSess));
			return conv;
		}
		else if (srcFormat == *(UInt32*)"DIBS" && srcNBits <= 64)
		{
			NEW_CLASS(conv, Media::CS::CSRGB16_LRGBC(srcNBits, srcPF, true, srcProfile, destProfile, colorSess));
			return conv;
		}
		else if (srcFormat == *(UInt32*)"YUY2" || srcFormat == *(UInt32*)"YUYV")
		{
			NEW_CLASS(conv, Media::CS::CSYUY2_LRGBC(srcProfile, destProfile, yuvType, colorSess));
			return conv;
		}
		else if (srcFormat == *(UInt32*)"UYVY" || srcFormat == *(UInt32*)"Y422" || srcFormat == *(UInt32*)"UYNV" || srcFormat == *(UInt32*)"HDYC")
		{
			NEW_CLASS(conv, Media::CS::CSUYVY_LRGBC(srcProfile, destProfile, yuvType, colorSess));
			return conv;
		}
		else if (srcFormat == *(UInt32*)"I420")
		{
			NEW_CLASS(conv, Media::CS::CSI420_LRGBC(srcProfile, destProfile, yuvType, colorSess));
			return conv;
		}
		else if (srcFormat == *(UInt32*)"BI42") //I420 in broadcom capture
		{
			NEW_CLASS(conv, Media::CS::CSBI42_LRGBC(srcProfile, destProfile, yuvType, colorSess));
			return conv;
		}
		else if (srcFormat == *(UInt32*)"YV12")
		{
			NEW_CLASS(conv, Media::CS::CSYV12_LRGBC(srcProfile, destProfile, yuvType, colorSess));
			return conv;
		}
		else if (srcFormat == *(UInt32*)"NV12")
		{
			NEW_CLASS(conv, Media::CS::CSNV12_LRGBC(srcProfile, destProfile, yuvType, colorSess));
			return conv;
		}
		else if (srcFormat == *(UInt32*)"P010" || srcFormat == *(UInt32*)"P016")
		{
			NEW_CLASS(conv, Media::CS::CSP016_LRGBC(srcProfile, destProfile, yuvType, colorSess));
			return conv;
		}
		else if (srcFormat == *(UInt32*)"P210" || srcFormat == *(UInt32*)"P216")
		{
			NEW_CLASS(conv, Media::CS::CSP216_LRGBC(srcProfile, destProfile, yuvType, colorSess));
			return conv;
		}
		else if (srcFormat == *(UInt32*)"Y416")
		{
			NEW_CLASS(conv, Media::CS::CSY416_LRGBC(srcProfile, destProfile, yuvType, colorSess));
			return conv;
		}
		else if (srcFormat == FFMT_YUV420P8)
		{
			NEW_CLASS(conv, Media::CS::CSYUV420P8_LRGBC(srcProfile, destProfile, yuvType, colorSess));
			return conv;
		}
		else if (srcFormat == FFMT_AYUV444_10)
		{
			NEW_CLASS(conv, Media::CS::CSAYUV444_10_LRGBC(srcProfile, destProfile, yuvType, colorSess));
			return conv;
		}
		else if (srcFormat == FFMT_YUV444P10LEP)
		{
			NEW_CLASS(conv, Media::CS::CSYUV444P10LEP_LRGBC(srcProfile, destProfile, yuvType, colorSess));
			return conv;
		}
	}
	else if (destFormat == *(UInt32*)"DIBS" && destNBits == 32)
	{
		if (srcFormat == 0 && srcNBits <= 32 && srcPF != Media::PF_LE_W16 && srcPF != Media::PF_LE_W16A16 && srcPF != Media::PF_LE_A2B10G10R10)
		{
			NEW_CLASS(conv, Media::CS::CSRGB8_RGB8(srcNBits, srcPF, destNBits, destPF, true, srcProfile, destProfile, colorSess));
			return conv;
		}
		else if (srcFormat == *(UInt32*)"DIBS" && srcNBits <= 32 && srcPF != Media::PF_LE_W16 && srcPF != Media::PF_LE_W16A16 && srcPF != Media::PF_LE_A2B10G10R10)
		{
			NEW_CLASS(conv, Media::CS::CSRGB8_RGB8(srcNBits, srcPF, destNBits, destPF, false, srcProfile, destProfile, colorSess));
			return conv;
		}
		else if (srcFormat == 0 && srcNBits <= 64)
		{
			NEW_CLASS(conv, Media::CS::CSRGB16_RGB8(srcNBits, srcPF, destNBits, destPF, true, srcProfile, destProfile, colorSess));
			return conv;
		}
		else if (srcFormat == *(UInt32*)"DIBS" && srcNBits <= 64)
		{
			NEW_CLASS(conv, Media::CS::CSRGB16_RGB8(srcNBits, srcPF, destNBits, destPF, false, srcProfile, destProfile, colorSess));
			return conv;
		}
	}
	else if (destFormat == 0 && destNBits == 32)
	{
		if (srcFormat == 0 && srcNBits <= 32 && srcPF != Media::PF_LE_W16 && srcPF != Media::PF_LE_W16A16 && srcPF != Media::PF_LE_A2B10G10R10)
		{
			NEW_CLASS(conv, Media::CS::CSRGB8_RGB8(srcNBits, srcPF, destNBits, destPF, false, srcProfile, destProfile, colorSess));
			return conv;
		}
		else if (srcFormat == *(UInt32*)"DIBS" && srcNBits <= 32 && srcPF != Media::PF_LE_W16 && srcPF != Media::PF_LE_W16A16 && srcPF != Media::PF_LE_A2B10G10R10)
		{
			NEW_CLASS(conv, Media::CS::CSRGB8_RGB8(srcNBits, srcPF, destNBits, destPF, true, srcProfile, destProfile, colorSess));
			return conv;
		}
		else if (srcFormat == 0 && srcNBits <= 64)
		{
			NEW_CLASS(conv, Media::CS::CSRGB16_RGB8(srcNBits, srcPF, destNBits, destPF, false, srcProfile, destProfile, colorSess));
			return conv;
		}
		else if (srcFormat == *(UInt32*)"DIBS" && srcNBits <= 64)
		{
			NEW_CLASS(conv, Media::CS::CSRGB16_RGB8(srcNBits, srcPF, destNBits, destPF, true, srcProfile, destProfile, colorSess));
			return conv;
		}
		else if (srcFormat == *(UInt32*)"YUY2" || srcFormat == *(UInt32*)"YUYV")
		{
			NEW_CLASS(conv, Media::CS::CSYUY2_RGB8(srcProfile, destProfile, yuvType, colorSess));
			return conv;
		}
		else if (srcFormat == *(UInt32*)"UYVY" || srcFormat == *(UInt32*)"Y422" || srcFormat == *(UInt32*)"UYNV" || srcFormat == *(UInt32*)"HDYC")
		{
			NEW_CLASS(conv, Media::CS::CSUYVY_RGB8(srcProfile, destProfile, yuvType, colorSess));
			return conv;
		}
		else if (srcFormat == *(UInt32*)"I420")
		{
			NEW_CLASS(conv, Media::CS::CSI420_RGB32C(srcProfile, destProfile, yuvType, colorSess, destPF));
			return conv;
		}
		else if (srcFormat == *(UInt32*)"BI42") //I420 in broadcom capture
		{
			NEW_CLASS(conv, Media::CS::CSBI42_RGB32C(srcProfile, destProfile, yuvType, colorSess, destPF));
			return conv;
		}
		else if (srcFormat == *(UInt32*)"YV12")
		{
			NEW_CLASS(conv, Media::CS::CSYV12_RGB32C(srcProfile, destProfile, yuvType, colorSess, destPF));
//			NEW_CLASS(conv, Media::CS::CSYV12_RGB8(srcProfile, destProfile, yuvType, colorSess));
			return conv;
		}
		else if (srcFormat == *(UInt32*)"NV12")
		{
			//NEW_CLASS(conv, Media::CS::CSNV12_RGB8(srcProfile, destProfile, yuvType, colorSess));
			NEW_CLASS(conv, Media::CS::CSNV12_RGB32C(srcProfile, destProfile, yuvType, colorSess, destPF));
			return conv;
		}
		else if (srcFormat == *(UInt32*)"YVU9")
		{
			NEW_CLASS(conv, Media::CS::CSYVU9_RGB8(srcProfile, destProfile, yuvType, colorSess));
			return conv;
		}
		else if (srcFormat == *(UInt32*)"AYUV")
		{
			NEW_CLASS(conv, Media::CS::CSAYUV_RGB8(srcProfile, destProfile, yuvType, colorSess));
			return conv;
		}
		else if (srcFormat == *(UInt32*)"P010" || srcFormat == *(UInt32*)"P016")
		{
			NEW_CLASS(conv, Media::CS::CSP016_RGB32C(srcProfile, destProfile, yuvType, colorSess, destPF));
			return conv;
		}
		else if (srcFormat == *(UInt32*)"P210" || srcFormat == *(UInt32*)"P216")
		{
			NEW_CLASS(conv, Media::CS::CSP010_RGB8(srcProfile, destProfile, yuvType, colorSess));
			////////////////////////////////////////
			return conv;
		}
		else if (srcFormat == *(UInt32*)"Y416")
		{
			NEW_CLASS(conv, Media::CS::CSY416_RGB32C(srcProfile, destProfile, yuvType, colorSess, destPF));
			return conv;
		}
		else if (srcFormat == FFMT_YUV420P8)
		{
			NEW_CLASS(conv, Media::CS::CSYUV420P8_RGB32C(srcProfile, destProfile, yuvType, colorSess, destPF));
			return conv;
		}
		else if (srcFormat == FFMT_AYUV444_10)
		{
			NEW_CLASS(conv, Media::CS::CSAYUV444_10_RGB32C(srcProfile, destProfile, yuvType, colorSess, destPF));
			return conv;
		}
		else if (srcFormat == FFMT_YUV444P10LEP)
		{
			NEW_CLASS(conv, Media::CS::CSYUV444P10LEP_RGB32C(srcProfile, destProfile, yuvType, colorSess, destPF));
			return conv;
		}
	}
	else if (destFormat == *(UInt32*)"Y800" || destFormat == *(UInt32*)"Y8  " || destFormat == *(UInt32*)"GREY")
	{
		if (Media::CS::CSYUV_Y8::IsSupported(srcFormat))
		{
			NEW_CLASS(conv, Media::CS::CSYUV_Y8(srcFormat));
			return conv;
		}
	}

	return 0;
}

Text::CString Media::CS::CSConverter::GetFormatName(UInt32 format)
{
	if (format == 0)
		return CSTR("ARGB");
	else if (format == FFMT_YUV444P10LE)
		return CSTR("YUV 4:4:4 10-bit Planar");
	else if (format == FFMT_YUV420P10LE)
		return CSTR("YUV 4:2:0 10-bit Planar");
	else if (format == FFMT_YUV420P12LE)
		return CSTR("YUV 4:2:0 12-bit Planar");
	else if (format == FFMT_YUV420P8)
		return CSTR("YUV 4:2:0 8-bit Planar");
	else if (format == FFMT_AYUV444_10)
		return CSTR("AYUV 4:4:4 10-bit Interleaved");
	else if (format == FFMT_YUV444P10LEP)
		return CSTR("YUV 4:4:4 10-bit Planar Conc");
	else if (format == *(UInt32*)"DIBS")
		return CSTR("ARGB (Reverse)");
	else if (format == *(UInt32*)"LRGB")
		return CSTR("Linear RGB 16-bit (LRGB)");
	else if (format == *(UInt32*)"LR32")
		return CSTR("Linear RGB 32-bit (LR32)");
	else if (format == *(UInt32*)"YV12")
		return CSTR("YUV 4:2:0 8-bit (YV12)");
	else if (format == *(UInt32*)"YUY2")
		return CSTR("YUV 4:2:2 8-bit (YUY2)");
	else if (format == *(UInt32*)"UYVY")
		return CSTR("YUV 4:2:2 8-bit (UYVY)");
	else if (format == *(UInt32*)"AYUV")
		return CSTR("AYUV 4:4:4 8-bit (AYUV)");
	else if (format == *(UInt32*)"IMC1")
		return CSTR("YUV 4:2:2 8-bit (IMC1)");
	else if (format == *(UInt32*)"IMC3")
		return CSTR("YUV 4:2:2 8-bit (IMC3)");
	else if (format == *(UInt32*)"IMC2")
		return CSTR("YUV 4:2:2 8-bit (IMC2)");
	else if (format == *(UInt32*)"IMC4")
		return CSTR("YUV 4:2:2 8-bit (IMC4)");
	else if (format == *(UInt32*)"NV12")
		return CSTR("YUV 4:2:0 8-bit (NV12)");
	else if (format == *(UInt32*)"Y410")
		return CSTR("YUV 4:2:2 10-bit (Y410)");
	else if (format == *(UInt32*)"P010")
		return CSTR("YUV 4:2:0 10-bit (P010)");
	else if (format == *(UInt32*)"P016")
		return CSTR("YUV 4:2:0 16-bit (P016)");
	else if (format == *(UInt32*)"P210")
		return CSTR("YUV 4:2:2 10-bit (P210)");
	else if (format == *(UInt32*)"P216")
		return CSTR("YUV 4:2:2 16-bit (P216)");
	else if (format == *(UInt32*)"Y210")
		return CSTR("YUV 4:2:2 10-bit (Y210)");
	else if (format == *(UInt32*)"Y216")
		return CSTR("YUV 4:2:2 16-bit (Y216)");
	else if (format == *(UInt32*)"Y416")
		return CSTR("AYUV 4:4:4 16-bit (Y416)");
	else
		return CSTR("Unknown");
/*
	csList->Add(*(UInt32*)"YVU9");
	csList->Add(*(UInt32*)"Y422");
	csList->Add(*(UInt32*)"UYNV");
	csList->Add(*(UInt32*)"HDYC");
	csList->Add(*(UInt32*)"I420");
	csList->Add(*(UInt32*)"AYUV");
*/
}
