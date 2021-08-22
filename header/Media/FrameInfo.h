#ifndef _SM_MEDIA_FRAMEINFO
#define _SM_MEDIA_FRAMEINFO
#include "MyMemory.h"
#include "Media/ColorProfile.h"
#include "Text/StringBuilderUTF.h"

namespace Media
{
	typedef enum
	{
		FIELD_TYPE_FRAME,
		FIELD_TYPE_INTERLACED_TOP,
		FIELD_TYPE_INTERLACED_BOTTOM
	} FieldType;

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
		AT_NO_ALPHA,
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

	enum RotateType
	{
		RT_NONE,
		RT_CW_90,
		RT_CW_180,
		RT_CW_270
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
		PF_B8G8R8A1
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
		UOSInt storeWidth;
		UOSInt storeHeight;
		UOSInt dispWidth;
		UOSInt dispHeight;
		UInt32 fourcc; // see FFMT_xxxx
		UInt32 storeBPP;
		PixelFormat pf;
		UOSInt byteSize; //valid only if bpp = 0
		Double par2; // def = 1;
		Double hdpi;
		Double vdpi;
		FrameType ftype;
		AlphaType atype;
		ColorProfile *color;
		RotateType rotateType;

		Media::ColorProfile::YUVType yuvType;
		Media::YCOffset ycOfst;

	public:
		FrameInfo();
		~FrameInfo();
		void Init();
		void Deinit();
		void Clear();
		void Set(const FrameInfo *info);
		void ToString(Text::StringBuilderUTF *sb);
	};

	const UTF8Char *FrameTypeGetName(FrameType frameType);
	const UTF8Char *AlphaTypeGetName(AlphaType atype);
	const UTF8Char *YCOffsetGetName(YCOffset ycOfst);
	const UTF8Char *PixelFormatGetName(PixelFormat pf);
	PixelFormat PixelFormatGetDef(UInt32 fourcc, UInt32 storeBPP);
	RotateType RotateTypeCalc(RotateType srcType, RotateType destType);
	RotateType RotateTypeCombine(RotateType rtype1, RotateType rtype2);
}
#endif
