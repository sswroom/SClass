#ifndef _SM_MEDIA_COLORPROFILE
#define _SM_MEDIA_COLORPROFILE
#include "Math/Double2D.h"
#include "Math/Matrix3.h"
#include "Media/CS/TransferParam.h"
#include "Text/CString.h"
#include "Text/StringBuilderUTF8.h"

namespace Media
{
	class ColorProfile
	{
	public:
		typedef enum
		{
			YUVT_BT601,
			YUVT_BT709,
			YUVT_FCC,
			YUVT_BT470BG,
			YUVT_SMPTE170M,
			YUVT_SMPTE240M,
			YUVT_BT2020,
			YUVT_UNKNOWN = 255,
			YUVT_MASK = 255,
			YUVT_FLAG_YUV_0_255 = 65536
		} YUVType;

		typedef enum
		{
			CPT_ADOBE,
			CPT_APPLE,
			CPT_CIERGB,
			CPT_COLORMATCH,
			CPT_BT709,
			CPT_NTSC,
			CPT_PAL,
			CPT_SGI,
			CPT_SMPTE240M,
			CPT_SMPTEC,
			CPT_SRGB,
			CPT_WIDE,
			CPT_BT2020,
			CPT_BT2100,

			CPT_LAST = CPT_BT2100,
			CPT_FILE = 256,
			CPT_CUSTOM,
			CPT_PDISPLAY,
			CPT_VDISPLAY,
			CPT_OS,
			CPT_PUNKNOWN,
			CPT_VUNKNOWN,
			CPT_EDID
		} CommonProfileType;

		typedef enum
		{
			CT_PUNKNOWN,
			CT_SRGB,
			CT_BT709,
			CT_BT470M,
			CT_BT470BG,
			CT_SMPTE170M,
			CT_SMPTE240M,
			CT_GENERIC_FILM,
			CT_BT2020,
			CT_ADOBE,
			CT_APPLE,
			CT_CIERGB,
			CT_COLORMATCH,
			CT_WIDE,
			CT_SGAMUT,
			CT_SGAMUTCINE,
			CT_SMPTE_RP431_2,
			CT_ACESGAMUT,
			CT_ALEXAWIDE,
			CT_VGAMUT,
			CT_GOPRO_PROTUNE,
			CT_SMPTE_EG432_1,
			CT_EBU3213,
			CT_SMPTE428,

			CT_CUSTOM = 256,
			CT_DISPLAY = 257,
			CT_VUNKNOWN = 258,

			CT_BT2100 = CT_BT2020,
			CT_DCI_P3 = CT_SMPTE_EG432_1
		} ColorType;

		typedef enum
		{
			WPT_A,
			WPT_B,
			WPT_C,
			WPT_D50,
			WPT_D55,
			WPT_D65,
			WPT_D75,
			WPT_E,
			WPT_F1,
			WPT_F2,
			WPT_F3,
			WPT_F4,
			WPT_F5,
			WPT_F6,
			WPT_F7,
			WPT_F8,
			WPT_F9,
			WPT_F10,
			WPT_F11,
			WPT_F12,
			WPT_SMPTE431,
			WPT_SMPTE432,
			WPT_SMPTE428
		} WhitePointType;

		struct ColorCoordinate
		{
			Double X;
			Double Y;
			Double Z;

			void SetFromxyY(Double x, Double y, Double Y);
			void SetFromXYZ(Double X, Double Y, Double Z);
			Math::Double2D Getxy();
		};

		class ColorPrimaries
		{
		public:
			Media::ColorProfile::ColorType colorType;
			Math::Double2D r;
			Math::Double2D g;
			Math::Double2D b;
			Math::Double2D w;

		public:
			void SetWhiteType(WhitePointType wpType);
			void SetWhiteTemp(Double colorTemp);
		public:
			ColorPrimaries();
			~ColorPrimaries();

			void Set(NN<const ColorPrimaries> primaries);
			void SetColorType(ColorType colorType);
			Math::Double2D GetWhitexy() const;
			void GetConvMatrix(NN<Math::Matrix3> matrix) const;
			void SetConvMatrix(NN<const Math::Matrix3> matrix);

			Bool Equals(NN<const ColorPrimaries> primaries) const;

			static Math::Double2D GetWhitePointXY(WhitePointType wpType);
			static Math::Double2D GetWhitePointXY(Double colorTemp);
			static Math::Vector3 GetWhitePointXYZ(WhitePointType wpType);
			static void GetMatrixBradford(NN<Math::Matrix3> mat);
			static void GetMatrixVonKries(NN<Math::Matrix3> mat);
			static Math::Vector3 xyYToXYZ(const Math::Vector3 &xyY);
			static Math::Vector3 XYZToxyY(const Math::Vector3 &XYZ);
			static void GetAdaptationMatrix(NN<Math::Matrix3> mat, WhitePointType srcWP, WhitePointType destWP);
		};

		Media::CS::TransferParam rtransfer;
		Media::CS::TransferParam gtransfer;
		Media::CS::TransferParam btransfer;
		Media::ColorProfile::ColorPrimaries primaries;
		const UInt8 *rawICC;

	public:
		ColorProfile();
		ColorProfile(NN<const ColorProfile> profile);
		ColorProfile(CommonProfileType cpt);
		ColorProfile(const ColorProfile&);
		~ColorProfile();

		void Set(NN<const ColorProfile> profile);
		void SetCommonProfile(CommonProfileType cpt);
		void RGB32ToXYZ(Int32 rgb, OutParam<Double> X, OutParam<Double> Y, OutParam<Double> Z) const;
		Int32 XYZToRGB32(Double a, Double X, Double Y, Double Z) const;
		static void XYZWPTransform(WhitePointType srcWP, Double srcX, Double srcY, Double srcZ, WhitePointType destWP, OutParam<Double> outX, OutParam<Double> outY, OutParam<Double> outZ);
		Bool Equals(NN<const ColorProfile> profile) const;
		NN<Media::CS::TransferParam> GetRTranParam();
		NN<Media::CS::TransferParam> GetGTranParam();
		NN<Media::CS::TransferParam> GetBTranParam();
		NN<const Media::CS::TransferParam> GetRTranParamRead() const;
		NN<const Media::CS::TransferParam> GetGTranParamRead() const;
		NN<const Media::CS::TransferParam> GetBTranParamRead() const;
		NN<Media::ColorProfile::ColorPrimaries> GetPrimaries();
		NN<const Media::ColorProfile::ColorPrimaries> GetPrimariesRead() const;
		void ToString(NN<Text::StringBuilderUTF8> sb) const;

		void SetRAWICC(const UInt8 *iccData);
		const UInt8 *GetRAWICC() const;

		static Text::CStringNN YUVTypeGetName(YUVType yuvType);
		static Text::CStringNN ColorTypeGetName(ColorType colorType);
		static Text::CStringNN CommonProfileTypeGetName(CommonProfileType cpt);
		static void GetYUVConstants(YUVType yuvType, Double *kr, Double *kb);
		static void YUV2RGB(YUVType yuvType, Double y, Double u, Double v, Double *r, Double *g, Double *b);
		static void RGB2RGB(Media::ColorProfile *srcColor, Media::ColorProfile *destColor, Double srcR, Double srcG, Double srcB, Double *destR, Double *destG, Double *destB);
		static void GetConvMatrix(NN<Math::Matrix3> mat, NN<const ColorProfile::ColorPrimaries> srcColor, NN<const ColorProfile::ColorPrimaries> destColor);
	};
}
#endif
