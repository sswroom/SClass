#ifndef _SM_MEDIA_FRAMEINFO
#define _SM_MEDIA_FRAMEINFO
#include "MyMemory.h"
#include "Math/Size2D.h"
#include "Media/ColorProfile.h"
#include "Text/CString.h"
#include "Text/StringBuilderUTF8.h"

namespace Media
{
	enum class FieldType
	{
		Frame,
		InterlacedTop,
		InterlacedBottom
	};

	typedef enum
	{
		FT_NON_INTERLACE,
		FT_FIELD_TF,
		FT_FIELD_BF,
		FT_INTERLACED_TFF,
		FT_INTERLACED_BFF,
		FT_MERGED_TF,
		FT_MERGED_BF,
		FT_DISCARD,
		FT_INTERLACED_NODEINT
	} FrameType;

	typedef enum
	{
		AT_IGNORE_ALPHA,
		AT_ALPHA_ALL_FF,
		AT_ALPHA,
		AT_PREMUL_ALPHA
	} AlphaType;

	typedef enum
	{
		YCOFST_C_TOP_LEFT = 0,
		YCOFST_C_TOP_CENTER = 1,
		YCOFST_C_CENTER_LEFT = 2,
		YCOFST_C_CENTER_CENTER = 3
	} YCOffset;

	enum class RotateType
	{
		None,
		CW_90,
		CW_180,
		CW_270,
		HFLIP,
		HFLIP_CW_90,
		HFLIP_CW_180,
		HFLIP_CW_270
	};

	typedef enum
	{
		PF_UNKNOWN,
		PF_PAL_1,
		PF_PAL_2,
		PF_PAL_4,
		PF_PAL_8,
		PF_LE_R5G5B5,
		PF_LE_R5G6B5,
		PF_LE_B16G16R16A16,
		PF_LE_B16G16R16,
		PF_LE_W16,
		PF_LE_A2B10G10R10,
		PF_B8G8R8A8,
		PF_B8G8R8,
		PF_PAL_W1,
		PF_PAL_W2,
		PF_PAL_W4,
		PF_PAL_W8,
		PF_W8A8,
		PF_LE_W16A16,
		PF_LE_FB32G32R32A32,
		PF_LE_FB32G32R32,
		PF_LE_FW32A32,
		PF_LE_FW32,
		PF_R8G8B8A8,
		PF_R8G8B8,
		PF_PAL_1_A1,
		PF_PAL_2_A1,
		PF_PAL_4_A1,
		PF_PAL_8_A1,
		PF_B8G8R8A1,
		PF_LE_R16G16B16A16,
		PF_LE_R16G16B16,
		PF_LE_FR32G32B32A32,
		PF_LE_FR32G32B32
	} PixelFormat;

#define FFMT_ARGB 0
#define FFMT_YUV444P10LE	256 //3 Planes
#define FFMT_YUV420P10LE	257 //3 Planes
#define FFMT_YUV420P12LE	258 //3 Planes
#define FFMT_YUV420P8		259 //3 Planes
#define FFMT_AYUV444_10		260 //1 Planes Interleaved
#define FFMT_YUV444P10LEP	261 //1 Planes

	class FrameInfo
	{
	public:
		Math::Size2D<UIntOS> storeSize;
		Math::Size2D<UIntOS> dispSize;
		UInt32 fourcc; // see FFMT_xxxx
		UInt32 storeBPP;
		PixelFormat pf;
		UIntOS byteSize; //valid only if bpp = 0
		Double par2; // def = 1;
		Double hdpi;
		Double vdpi;
		FrameType ftype;
		AlphaType atype;
		ColorProfile color;
		RotateType rotateType;

		Media::ColorProfile::YUVType yuvType;
		Media::YCOffset ycOfst;

	public:
		FrameInfo();
		~FrameInfo();
		void InitFrameInfo();
		void DeinitFrameInfo();
		void Clear();
		void Set(NN<const FrameInfo> info);
		void ToString(NN<Text::StringBuilderUTF8> sb) const;
	};

	Text::CStringNN FrameTypeGetName(FrameType frameType);
	Text::CStringNN AlphaTypeGetName(AlphaType atype);
	Text::CStringNN YCOffsetGetName(YCOffset ycOfst);
	Text::CStringNN PixelFormatGetName(PixelFormat pf);
	Text::CString RotateTypeGetName(RotateType rotateType);
	PixelFormat PixelFormatGetDef(UInt32 fourcc, UInt32 storeBPP);
	RotateType RotateTypeCalc(RotateType srcType, RotateType destType);
	RotateType RotateTypeCombine(RotateType rtype1, RotateType rtype2);
}
#endif
