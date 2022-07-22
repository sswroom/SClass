#ifndef _SM_MEDIA_ICCPROFILE
#define _SM_MEDIA_ICCPROFILE
#include "Data/DateTime.h"
#include "Media/FrameInfo.h"
#include "Media/LUT.h"
#include "Media/CS/TransferFunc.h"
#include "Text/CString.h"
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

		Int32 GetCMMType() const;
		void GetProfileVer(UInt8 *majorVer, UInt8 *minorVer, UInt8 *bugFixVer) const;
		Int32 GetProfileClass() const;
		Int32 GetDataColorspace() const;
		Int32 GetPCS() const;
		void GetCreateTime(Data::DateTime *createTime) const;
		Int32 GetPrimaryPlatform() const;
		Int32 GetProfileFlag() const;
		Int32 GetDeviceManufacturer() const;
		Int32 GetDeviceModel() const;
		Int32 GetDeviceAttrib() const;
		Int32 GetDeviceAttrib2() const;
		Int32 GetRenderingIntent() const;
		void GetPCSIlluminant(CIEXYZ *xyz) const;
		Int32 GetProfileCreator() const;
		Int32 GetTagCount() const;

		Media::LUT *CreateRLUT() const;
		Media::LUT *CreateGLUT() const;
		Media::LUT *CreateBLUT() const;
		Bool GetRedTransferParam(Media::CS::TransferParam *param) const;
		Bool GetGreenTransferParam(Media::CS::TransferParam *param) const;
		Bool GetBlueTransferParam(Media::CS::TransferParam *param) const;
		Bool GetColorPrimaries(Media::ColorProfile::ColorPrimaries *color) const;

		Bool SetToColorProfile(Media::ColorProfile *colorProfile);
		void ToString(Text::StringBuilderUTF8 *sb) const;

		static ICCProfile *Parse(const UInt8 *buff, UOSInt buffSize);

		static void ReadDateTimeNumber(const UInt8 *buff, Data::DateTime *dt);
		static void ReadXYZNumber(const UInt8 *buff, CIEXYZ *xyz);
		static Double ReadS15Fixed16Number(const UInt8 *buff);
		static Double ReadU16Fixed16Number(const UInt8 *buff);
		static Double ReadU8Fixed8Number(const UInt8 *buff);

		static Text::CString GetNameCMMType(Int32 val);
		static Text::CString GetNameProfileClass(Int32 val);
		static Text::CString GetNameDataColorspace(Int32 val);
		static Text::CString GetNamePrimaryPlatform(Int32 val);
		static Text::CString GetNameDeviceManufacturer(Int32 val);
		static Text::CString GetNameDeviceModel(Int32 val);
		static Text::CString GetNameTag(Int32 val);
		static Text::CString GetNameStandardObserver(Int32 val);
		static Text::CString GetNameStandardIlluminent(Int32 val);

		static void GetDispCIEXYZ(Text::StringBuilderUTF8 *sb, CIEXYZ *xyz);
		static void GetDispTagType(Text::StringBuilderUTF8 *sb, UInt8 *buff, UInt32 leng);

		static Media::CS::TransferType FindTransferType(UOSInt colorCount, UInt16 *curveColors, Double *gamma);
		static UTF8Char *GetProfilePath(UTF8Char *sbuff);

		static ICCProfile *NewSRGBProfile();
		static const UInt8 *GetSRGBICCData();
	};
}
#endif
