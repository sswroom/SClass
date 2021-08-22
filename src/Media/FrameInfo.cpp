#include "Stdafx.h"
#include "MyMemory.h"
#include "Media/FrameInfo.h"
#include "Text/MyStringFloat.h"

Media::FrameInfo::FrameInfo()
{
	this->Init();
}

Media::FrameInfo::~FrameInfo()
{
	this->Deinit();
}

void Media::FrameInfo::Init()
{
	NEW_CLASS(color, Media::ColorProfile());
}

void Media::FrameInfo::Deinit()
{
	SDEL_CLASS(color);
}

void Media::FrameInfo::Clear()
{
	this->storeWidth = 0;
	this->storeHeight = 0;
	this->dispWidth = 0;
	this->dispHeight = 0;
	this->fourcc = 0;
	this->storeBPP = 0;
	this->pf  = Media::PF_UNKNOWN;
	this->byteSize = 0;
	this->par2 = 1.0;
	this->hdpi = 0;
	this->vdpi = 0;
	this->ftype = Media::FT_NON_INTERLACE;
	this->atype = Media::AT_NO_ALPHA;
	this->yuvType = Media::ColorProfile::YUVT_UNKNOWN;
	this->ycOfst = Media::YCOFST_C_CENTER_LEFT;
	this->rotateType = Media::RT_NONE;
}

void Media::FrameInfo::Set(const FrameInfo *info)
{
	this->storeWidth = info->storeWidth;
	this->storeHeight = info->storeHeight;
	this->dispWidth = info->dispWidth;
	this->dispHeight = info->dispHeight;
	this->fourcc = info->fourcc;
	this->storeBPP = info->storeBPP;
	this->pf  = info->pf;
	this->byteSize = info->byteSize;
	this->par2 = info->par2;
	this->hdpi = info->hdpi;
	this->vdpi = info->vdpi;
	this->ftype = info->ftype;
	this->atype = info->atype;
	this->color->Set(info->color);
	this->rotateType = info->rotateType;
	this->yuvType = info->yuvType;
	this->ycOfst = info->ycOfst;
}

void Media::FrameInfo::ToString(Text::StringBuilderUTF *sb)
{
	sb->Append((const UTF8Char*)"FourCC = ");
	if (this->fourcc < 1024)
	{
		sb->AppendU32(this->fourcc);
	}
	else
	{
		sb->AppendC((const UTF8Char*)&this->fourcc, 4);
	}
	sb->Append((const UTF8Char*)"\r\n");
	sb->Append((const UTF8Char*)"Display Size = ");
	sb->AppendUOSInt(this->dispWidth);
	sb->Append((const UTF8Char*)" x ");
	sb->AppendUOSInt(this->dispHeight);
	sb->Append((const UTF8Char*)"\r\n");
	sb->Append((const UTF8Char*)"Store Size = ");
	sb->AppendUOSInt(this->storeWidth);
	sb->Append((const UTF8Char*)" x ");
	sb->AppendUOSInt(this->storeHeight);
	sb->Append((const UTF8Char*)"\r\n");
	sb->Append((const UTF8Char*)"Store BPP = ");
	sb->AppendU32(this->storeBPP);
	sb->Append((const UTF8Char*)"\r\n");
	sb->Append((const UTF8Char*)"Pixel Format = ");
	sb->Append(Media::PixelFormatGetName(this->pf));
	sb->Append((const UTF8Char*)"\r\n");
	sb->Append((const UTF8Char*)"Byte Size = ");
	sb->AppendUOSInt(this->byteSize);
	sb->Append((const UTF8Char*)"\r\n");
	sb->Append((const UTF8Char*)"Pixel Aspect Ratio = ");
	Text::SBAppendF64(sb, this->par2);
	sb->Append((const UTF8Char*)"\r\n");
	sb->Append((const UTF8Char*)"H-DPI = ");
	Text::SBAppendF64(sb, this->hdpi);
	sb->Append((const UTF8Char*)"\r\n");
	sb->Append((const UTF8Char*)"V-DPI = ");
	Text::SBAppendF64(sb, this->vdpi);
	sb->Append((const UTF8Char*)"\r\n");
	sb->Append((const UTF8Char*)"Pixel Format = ");
	sb->Append(Media::FrameTypeGetName(this->ftype));
	sb->Append((const UTF8Char*)"\r\n");
	sb->Append((const UTF8Char*)"Alpha Type = ");
	sb->Append(Media::AlphaTypeGetName(this->atype));
	sb->Append((const UTF8Char*)"\r\n");
	sb->Append((const UTF8Char*)"YUV Type = ");
	sb->Append(Media::ColorProfile::YUVTypeGetName(this->yuvType));
	sb->Append((const UTF8Char*)"\r\n");
	sb->Append((const UTF8Char*)"Y/C Offset = ");
	sb->Append(Media::YCOffsetGetName(this->ycOfst));
	sb->Append((const UTF8Char*)"\r\n");
	sb->Append((const UTF8Char*)"\r\nColor Profile:\r\n");
	this->color->ToString(sb);
}

const UTF8Char *Media::FrameTypeGetName(FrameType frameType)
{
	switch (frameType)
	{
	case FT_NON_INTERLACE:
		return (const UTF8Char*)"Non-Interlaced";
	case FT_FIELD_TF:
		return (const UTF8Char*)"Field (Top Field)";
	case FT_FIELD_BF:
		return (const UTF8Char*)"Field (Bottom Field)";
	case FT_INTERLACED_TFF:
		return (const UTF8Char*)"Interlaced (Top Field First)";
	case FT_INTERLACED_BFF:
		return (const UTF8Char*)"Interlaced (Bottom Field First)";
	case FT_MERGED_TF:
		return (const UTF8Char*)"Merged Field (Bottom Field)";
	case FT_MERGED_BF:
		return (const UTF8Char*)"Merged Field (Top Field)";
	case FT_DISCARD:
		return (const UTF8Char*)"Discard";
	case FT_INTERLACED_NODEINT:
		return (const UTF8Char*)"Interlaced No-deinterlace";
	default:
		return (const UTF8Char*)"Unknown";
	}
}

const UTF8Char *Media::AlphaTypeGetName(AlphaType atype)
{
	switch (atype)
	{
	case AT_NO_ALPHA:
		return (const UTF8Char*)"No Alpha";
	case AT_ALPHA:
		return (const UTF8Char*)"Alpha";
	case AT_PREMUL_ALPHA:
		return (const UTF8Char*)"Premultiply Alpha";
	default:
		return (const UTF8Char*)"Unknown";
	}
}

const UTF8Char *Media::YCOffsetGetName(YCOffset ycOfst)
{
	switch (ycOfst)
	{
	case YCOFST_C_TOP_LEFT:
		return (const UTF8Char*)"Top Left";
	case YCOFST_C_TOP_CENTER:
		return (const UTF8Char*)"Top Center";
	case YCOFST_C_CENTER_LEFT:
		return (const UTF8Char*)"Center Left";
	case YCOFST_C_CENTER_CENTER:
		return (const UTF8Char*)"Center Center";
	default:
		return (const UTF8Char*)"Unknown";
	}
}

const UTF8Char *Media::PixelFormatGetName(PixelFormat pf)
{
	switch (pf)
	{
	case PF_PAL_1:
		return (const UTF8Char*)"Indexed 1-bit";
	case PF_PAL_2:
		return (const UTF8Char*)"Indexed 2-bit";
	case PF_PAL_4:
		return (const UTF8Char*)"Indexed 4-bit";
	case PF_PAL_8:
		return (const UTF8Char*)"Indexed 8-bit";
	case PF_LE_R5G5B5:
		return (const UTF8Char*)"R5G5B5 (LE)";
	case PF_LE_R5G6B5:
		return (const UTF8Char*)"R5G6B5 (LE)";
	case PF_LE_B16G16R16A16:
		return (const UTF8Char*)"B16G16R16A16 (LE)";
	case PF_LE_B16G16R16:
		return (const UTF8Char*)"B16G16R16 (LE)";
	case PF_LE_W16:
		return (const UTF8Char*)"Grayscale 16-bit";
	case PF_LE_A2B10G10R10:
		return (const UTF8Char*)"A2B10G10R10 (LE)";
	case PF_B8G8R8A8:
		return (const UTF8Char*)"B8G8R8A8";
	case PF_B8G8R8:
		return (const UTF8Char*)"B8G8R8";
	case PF_PAL_W1:
		return (const UTF8Char*)"1-bit B/W";
	case PF_PAL_W2:
		return (const UTF8Char*)"Grayscale 2-bit";
	case PF_PAL_W4:
		return (const UTF8Char*)"Grayscale 4-bit";
	case PF_PAL_W8:
		return (const UTF8Char*)"Grayscale 8-bit";
	case PF_W8A8:
		return (const UTF8Char*)"Grayscale 8-bit w/Alpha";
	case PF_LE_W16A16:
		return (const UTF8Char*)"Grayscale 16-bit (LE) w/Alpha";
	case PF_LE_FB32G32R32A32:
		return (const UTF8Char*)"B32G32R32A32 (FLE)";
	case PF_LE_FB32G32R32:
		return (const UTF8Char*)"B32G32R32 (FLE)";
	case PF_LE_FW32A32:
		return (const UTF8Char*)"Grayscale 32-bit (FLE) w/Alpha";
	case PF_LE_FW32:
		return (const UTF8Char*)"Grayscale 32-bit (FLE)";
	case PF_R8G8B8A8:
		return (const UTF8Char*)"R8G8B8A8";
	case PF_R8G8B8:
		return (const UTF8Char*)"R8G8B8";
	case PF_PAL_1_A1:
		return (const UTF8Char*)"Indexed 1-bit + 1-bit Alpha";
	case PF_PAL_2_A1:
		return (const UTF8Char*)"Indexed 2-bit + 1-bit Alpha";
	case PF_PAL_4_A1:
		return (const UTF8Char*)"Indexed 4-bit + 1-bit Alpha";
	case PF_PAL_8_A1:
		return (const UTF8Char*)"Indexed 8-bit + 1-bit Alpha";
	case PF_B8G8R8A1:
		return (const UTF8Char*)"B8G8R8A1";
	case PF_UNKNOWN:
	default:
		return (const UTF8Char*)"Unknown";
	}
}

Media::PixelFormat Media::PixelFormatGetDef(UInt32 fourcc, UInt32 storeBPP)
{
	if (fourcc == 0 || fourcc == *(UInt32*)"DIBS")
	{
		if (storeBPP == 1)
			return PF_PAL_1;
		if (storeBPP == 2)
			return PF_PAL_2;
		if (storeBPP == 4)
			return PF_PAL_4;
		if (storeBPP == 8)
			return PF_PAL_8;
		if (storeBPP == 16)
			return PF_LE_R5G6B5;
		if (storeBPP == 24)
			return PF_B8G8R8;
		if (storeBPP == 32)
			return PF_B8G8R8A8;
		if (storeBPP == 48)
			return PF_LE_B16G16R16;
		if (storeBPP == 64)
			return PF_LE_B16G16R16A16;
		if (storeBPP == 128)
			return PF_LE_FB32G32R32A32;
	}
	return PF_UNKNOWN;
}

Media::RotateType Media::RotateTypeCalc(RotateType srcType, RotateType destType)
{
	if (srcType == destType)
	{
		return RT_NONE;
	}
	switch (srcType)
	{
	case RT_NONE:
		return destType;
	case RT_CW_90:
		switch (destType)
		{
		case RT_NONE:
			return RT_CW_270;
		case RT_CW_90:
			return RT_NONE;
		case RT_CW_180:
			return RT_CW_90;
		case RT_CW_270:
			return RT_CW_180;
		default:
			return RT_NONE;
		}
	case RT_CW_180:
		switch (destType)
		{
		case RT_NONE:
			return RT_CW_180;
		case RT_CW_90:
			return RT_CW_270;
		case RT_CW_180:
			return RT_NONE;
		case RT_CW_270:
			return RT_CW_90;
		default:
			return RT_NONE;
		}
	case RT_CW_270:
		switch (destType)
		{
		case RT_NONE:
			return RT_CW_90;
		case RT_CW_90:
			return RT_CW_180;
		case RT_CW_180:
			return RT_CW_270;
		case RT_CW_270:
			return RT_NONE;
		default:
			return RT_NONE;
		}
	default:
		return RT_NONE;
	}
}

Media::RotateType Media::RotateTypeCombine(RotateType rtype1, RotateType rtype2)
{
	switch (rtype1)
	{
	case RT_NONE:
		return rtype2;
	case RT_CW_90:
		switch (rtype2)
		{
		case RT_NONE:
			return RT_CW_90;
		case RT_CW_90:
			return RT_CW_180;
		case RT_CW_180:
			return RT_CW_270;
		case RT_CW_270:
			return RT_NONE;
		default:
			return RT_NONE;
		}
	case RT_CW_180:
		switch (rtype2)
		{
		case RT_NONE:
			return RT_CW_180;
		case RT_CW_90:
			return RT_CW_270;
		case RT_CW_180:
			return RT_NONE;
		case RT_CW_270:
			return RT_CW_90;
		default:
			return RT_NONE;
		}
	case RT_CW_270:
		switch (rtype2)
		{
		case RT_NONE:
			return RT_CW_270;
		case RT_CW_90:
			return RT_NONE;
		case RT_CW_180:
			return RT_CW_90;
		case RT_CW_270:
			return RT_CW_180;
		default:
			return RT_NONE;
		}
	default:
		return RT_NONE;
	}
}
