#ifndef _SM_MEDIA_RGBLUTGEN
#define _SM_MEDIA_RGBLUTGEN
#include "Media/ColorSess.h"

namespace Media
{
	class RGBLUTGen
	{
	private:
		Media::ColorSess *colorSess;

	public:
		RGBLUTGen(Media::ColorSess *colorSess);
		~RGBLUTGen();

		void SetSrcTrans(NotNullPtr<Media::CS::TransferParam> rTran, NotNullPtr<Media::CS::TransferParam> gTran, NotNullPtr<Media::CS::TransferParam> bTran, NotNullPtr<const Media::ColorProfile> srcProfile);
		Media::CS::TransferType SetDestTrans(NotNullPtr<Media::CS::TransferParam> rTran, NotNullPtr<Media::CS::TransferParam> gTran, NotNullPtr<Media::CS::TransferParam> bTran, NotNullPtr<const Media::ColorProfile> destProfile);

		void GenRGB8_LRGB(UInt16 *rgbTable, NotNullPtr<const Media::ColorProfile> srcProfile, Int32 nBitLRGB); //1536 bytes (RGB * 512)
		void GenRGBA8_LRGBC(Int64 *rgbTable, NotNullPtr<const Media::ColorProfile> srcProfile, NotNullPtr<const Media::ColorProfile::ColorPrimaries> destPrimaries, Int32 nBitLRGB); //8192 bytes (rgba * 2048)
		void GenRGB16_LRGBC(Int64 *rgbTable, NotNullPtr<const Media::ColorProfile> srcProfile, NotNullPtr<const Media::ColorProfile::ColorPrimaries> destPrimaries, Int32 nBitLRGB); //2097152 bytes
		void GenLRGB_BGRA8(UInt8 *rgbTable, NotNullPtr<const Media::ColorProfile> destProfile, Int32 nBitLRGB, Double srcRefLuminance); //262144 bytes
		void GenLRGB_RGB16(UInt8 *rgbTable, NotNullPtr<const Media::ColorProfile> destProfile, Int32 nBitLRGB, Double srcRefLuminance); //65536 * 6 bytes
		void GenLRGB_A2B10G10R10(UInt8 *rgbTable, NotNullPtr<const Media::ColorProfile> destProfile, Int32 nBitLRGB, Double srcRefLuminance); //65536 * 12 bytes
		void GenLARGB_A2B10G10R10(UInt8 *rgbTable, NotNullPtr<const Media::ColorProfile> destProfile, Int32 nBitLRGB, Double srcRefLuminance); //65536 * 16 bytes
		void GenLARGB_B8G8R8A8(UInt8 *rgbTable, NotNullPtr<const Media::ColorProfile> destProfile, Int32 nBitLRGB, Double srcRefLuminance); //65536 * 16 bytes
	};
};
#endif
