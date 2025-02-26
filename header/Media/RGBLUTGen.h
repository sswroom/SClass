#ifndef _SM_MEDIA_RGBLUTGEN
#define _SM_MEDIA_RGBLUTGEN
#include "Media/ColorSess.h"

namespace Media
{
	class RGBLUTGen
	{
	private:
		Optional<Media::ColorSess> colorSess;

	public:
		RGBLUTGen(Optional<Media::ColorSess> colorSess);
		~RGBLUTGen();

		void SetSrcTrans(NN<Media::CS::TransferParam> rTran, NN<Media::CS::TransferParam> gTran, NN<Media::CS::TransferParam> bTran, NN<const Media::ColorProfile> srcProfile);
		Media::CS::TransferType SetDestTrans(NN<Media::CS::TransferParam> rTran, NN<Media::CS::TransferParam> gTran, NN<Media::CS::TransferParam> bTran, NN<const Media::ColorProfile> destProfile);
		void SetSrcTran(NN<Media::CS::TransferParam> outTran, NN<const Media::CS::TransferParam> srcTran);
		Media::CS::TransferType SetDestTran(NN<Media::CS::TransferParam> outTran, NN<const Media::CS::TransferParam> destTran);

		void GenRGB8_LRGB(UnsafeArray<UInt16> rgbTable, NN<const Media::ColorProfile> srcProfile, Int32 nBitLRGB); //1536 bytes (RGB * 512)
		void GenRGBA8_LRGBC(UnsafeArray<Int64> rgbTable, NN<const Media::ColorProfile> srcProfile, NN<const Media::ColorProfile::ColorPrimaries> destPrimaries, Int32 nBitLRGB); //8192 bytes (rgba * 2048)
		void GenRGB16_LRGBC(UnsafeArray<Int64> rgbTable, NN<const Media::ColorProfile> srcProfile, NN<const Media::ColorProfile::ColorPrimaries> destPrimaries, Int32 nBitLRGB); //2097152 bytes
		void GenLRGB_BGRA8(UnsafeArray<UInt8> rgbTable, NN<const Media::ColorProfile> destProfile, Int32 nBitLRGB, Double srcRefLuminance); //262144 bytes
		void GenLRGB_RGB16(UnsafeArray<UInt8> rgbTable, NN<const Media::ColorProfile> destProfile, Int32 nBitLRGB, Double srcRefLuminance); //65536 * 6 bytes
		void GenLRGB_A2B10G10R10(UnsafeArray<UInt8> rgbTable, NN<const Media::ColorProfile> destProfile, Int32 nBitLRGB, Double srcRefLuminance); //65536 * 12 bytes
		void GenLARGB_A2B10G10R10(UnsafeArray<UInt8> rgbTable, NN<const Media::ColorProfile> destProfile, Int32 nBitLRGB, Double srcRefLuminance); //65536 * 16 bytes
		void GenLARGB_B8G8R8A8(UnsafeArray<UInt8> rgbTable, NN<const Media::ColorProfile> destProfile, Int32 nBitLRGB, Double srcRefLuminance); //65536 * 16 bytes
		void GenW8_L(UnsafeArray<UInt16> rgbTable, NN<const Media::CS::TransferParam> srcTran, Int32 nBitLRGB); //512 bytes
		void GenL_W8(UnsafeArray<UInt8> rgbTable, NN<const Media::CS::TransferParam> destTran, Int32 nBitLRGB, Double srcRefLuminance); //65536 bytes
	};
}
#endif
