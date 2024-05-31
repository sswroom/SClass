#ifndef _SM_MEDIA_ICCPROFILE
#define _SM_MEDIA_ICCPROFILE
#include "Data/ByteArray.h"
#include "Data/ByteBuffer.h"
#include "Data/DateTime.h"
#include "IO/FileAnalyse/FrameDetailHandler.h"
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
			CIEXYZ() = default;
			CIEXYZ(Double x, Double y, Double z) : Math::Vector3(x, y, z)
			{
			}
		};
	private:
		static const UInt8 srgbICC[];

		Data::ByteBuffer iccBuff;

		ICCProfile(UnsafeArray<const UInt8> iccBuff);
	public:
		~ICCProfile();

		Int32 GetCMMType() const;
		void GetProfileVer(OutParam<UInt8> majorVer, OutParam<UInt8> minorVer, OutParam<UInt8> bugFixVer) const;
		Int32 GetProfileClass() const;
		Int32 GetDataColorspace() const;
		Int32 GetPCS() const;
		void GetCreateTime(NN<Data::DateTime> createTime) const;
		Int32 GetPrimaryPlatform() const;
		Int32 GetProfileFlag() const;
		Int32 GetDeviceManufacturer() const;
		Int32 GetDeviceModel() const;
		Int32 GetDeviceAttrib() const;
		Int32 GetDeviceAttrib2() const;
		Int32 GetRenderingIntent() const;
		CIEXYZ GetPCSIlluminant() const;
		Int32 GetProfileCreator() const;
		Int32 GetTagCount() const;

		Optional<Media::LUT> CreateRLUT() const;
		Optional<Media::LUT> CreateGLUT() const;
		Optional<Media::LUT> CreateBLUT() const;
		Bool GetRedTransferParam(NN<Media::CS::TransferParam> param) const;
		Bool GetGreenTransferParam(NN<Media::CS::TransferParam> param) const;
		Bool GetBlueTransferParam(NN<Media::CS::TransferParam> param) const;
		Bool GetColorPrimaries(NN<Media::ColorProfile::ColorPrimaries> color) const;

		Bool SetToColorProfile(NN<Media::ColorProfile> colorProfile);
		void ToString(NN<Text::StringBuilderUTF8> sb) const;

		static Optional<ICCProfile> Parse(Data::ByteArrayR buff);
		static Bool ParseFrame(NN<IO::FileAnalyse::FrameDetailHandler> frame, UOSInt ofst, UnsafeArray<const UInt8> buff, UOSInt buffSize);

		static void ReadDateTimeNumber(const UInt8 *buff, NN<Data::DateTime> dt);
		static CIEXYZ ReadXYZNumber(const UInt8 *buff);
		static Double ReadS15Fixed16Number(const UInt8 *buff);
		static Double ReadU16Fixed16Number(const UInt8 *buff);
		static Double ReadU8Fixed8Number(const UInt8 *buff);

		static Text::CStringNN GetNameCMMType(Int32 val);
		static Text::CStringNN GetNameProfileClass(Int32 val);
		static Text::CStringNN GetNameDataColorspace(Int32 val);
		static Text::CStringNN GetNamePrimaryPlatform(Int32 val);
		static Text::CStringNN GetNameDeviceManufacturer(Int32 val);
		static Text::CStringNN GetNameDeviceModel(Int32 val);
		static Text::CStringNN GetNameTag(Int32 val);
		static Text::CStringNN GetNameStandardObserver(Int32 val);
		static Text::CStringNN GetNameStandardIlluminent(Int32 val);

		static void GetDispCIEXYZ(NN<Text::StringBuilderUTF8> sb, const CIEXYZ &xyz);
		static void GetDispTagType(NN<Text::StringBuilderUTF8> sb, UInt8 *buff, UInt32 leng);

		static Media::CS::TransferType FindTransferType(UOSInt colorCount, UInt16 *curveColors, OutParam<Double> gamma);
		static UnsafeArray<UTF8Char> GetProfilePath(UnsafeArray<UTF8Char> sbuff);

		static Optional<ICCProfile> NewSRGBProfile();
		static const UInt8 *GetSRGBICCData();

		static void FrameAddXYZNumber(NN<IO::FileAnalyse::FrameDetailHandler> frame, UOSInt ofst, Text::CStringNN fieldName, const UInt8 *xyzBuff);
		static void FrameDispTagType(NN<IO::FileAnalyse::FrameDetailHandler> frame, UOSInt ofst, Text::CStringNN fieldName, const UInt8 *buff, UInt32 leng);
	};
}
#endif
