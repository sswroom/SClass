#ifndef _SM_MEDIA_ICCPROFILE
#define _SM_MEDIA_ICCPROFILE
#include "Data/DateTime.h"
#include "Media/FrameInfo.h"
#include "Media/LUT.h"
#include "Media/CS/TransferFunc.h"
#include "Text/StringBuilderUTF8.h"

namespace Media
{
	class ICCProfile
	{
	public:
		struct CIEXYZ : public Math::Vector3
		{
		};
	private:
		static const UInt8 srgbICC[];

		UInt8 *iccBuff;

		ICCProfile(const UInt8 *iccBuff);
	public:
		~ICCProfile();

		Int32 GetCMMType();
		void GetProfileVer(UInt8 *majorVer, UInt8 *minorVer, UInt8 *bugFixVer);
		Int32 GetProfileClass();
		Int32 GetDataColorspace();
		Int32 GetPCS();
		void GetCreateTime(Data::DateTime *createTime);
		Int32 GetPrimaryPlatform();
		Int32 GetProfileFlag();
		Int32 GetDeviceManufacturer();
		Int32 GetDeviceModel();
		Int32 GetDeviceAttrib();
		Int32 GetDeviceAttrib2();
		Int32 GetRenderingIntent();
		void GetPCSIlluminant(CIEXYZ *xyz);
		Int32 GetProfileCreator();
		Int32 GetTagCount();

		Media::LUT *CreateRLUT();
		Media::LUT *CreateGLUT();
		Media::LUT *CreateBLUT();
		Bool GetRedTransferParam(Media::CS::TransferParam *param);
		Bool GetGreenTransferParam(Media::CS::TransferParam *param);
		Bool GetBlueTransferParam(Media::CS::TransferParam *param);
		Bool GetColorPrimaries(Media::ColorProfile::ColorPrimaries *color);

		void ToString(Text::StringBuilderUTF8 *sb);

		static ICCProfile *Parse(const UInt8 *buff, UOSInt buffSize);

		static void ReadDateTimeNumber(const UInt8 *buff, Data::DateTime *dt);
		static void ReadXYZNumber(const UInt8 *buff, CIEXYZ *xyz);
		static Double ReadS15Fixed16Number(const UInt8 *buff);
		static Double ReadU16Fixed16Number(const UInt8 *buff);
		static Double ReadU8Fixed8Number(const UInt8 *buff);

		static const UTF8Char *GetNameCMMType(Int32 val);
		static const UTF8Char *GetNameProfileClass(Int32 val);
		static const UTF8Char *GetNameDataColorspace(Int32 val);
		static const UTF8Char *GetNamePrimaryPlatform(Int32 val);
		static const UTF8Char *GetNameDeviceManufacturer(Int32 val);
		static const UTF8Char *GetNameDeviceModel(Int32 val);
		static const UTF8Char *GetNameTag(Int32 val);
		static const UTF8Char *GetNameStandardObserver(Int32 val);
		static const UTF8Char *GetNameStandardIlluminent(Int32 val);

		static void GetDispCIEXYZ(Text::StringBuilderUTF8 *sb, CIEXYZ *xyz);
		static void GetDispTagType(Text::StringBuilderUTF8 *sb, UInt8 *buff, UInt32 leng);

		static Media::CS::TransferType FindTransferType(UOSInt colorCount, UInt16 *curveColors, Double *gamma);
		static UTF8Char *GetProfilePath(UTF8Char *sbuff);

		static ICCProfile *NewSRGBProfile();
		static const UInt8 *GetSRGBICCData();
	};
}
#endif
