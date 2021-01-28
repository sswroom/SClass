#ifndef _SM_MEDIA_COLORCORR
#define _SM_MEDIA_COLORCORR

namespace Media
{
	// Gamma: 0 = sRGB, 1 - 65535 = 0.0001 - 6.5535
	class ColorCorr
	{
	private:
		Double radd;
		Double gadd;
		Double badd;
		Double rmul;
		Double gmul;
		Double bmul;
		Double rpow;
		Double gpow;
		Double bpow;
		Int32 rgamma;
		Int32 ggamma;
		Int32 bgamma;

		UInt8 *corrTab32;
		Bool corrTab32Valid;
		Int32 corrTab32Gamma;
		UInt8 *uncorrTab32;
		Bool uncorrTab32Valid;
		Int32 uncorrTab32Gamma;

	public:
		ColorCorr(WChar *name);
		~ColorCorr();
		Int32 Save(WChar *name);

		Double GetRAdd();
		Double GetRMul();
		Double GetRPow();
		Double GetGAdd();
		Double GetGMul();
		Double GetGPow();
		Double GetBAdd();
		Double GetBMul();
		Double GetBPow();
		Int32 GetRGamma();
		Int32 GetGGamma();
		Int32 GetBGamma();

		void SetRVals(Double radd, Double rmul, Double rpow);
		void SetGVals(Double gadd, Double gmul, Double gpow);
		void SetBVals(Double badd, Double bmul, Double bpow);
		void SetGammas(Int32 rgamma, Int32 ggamma, Int32 bgamma);
		void CorrImage32(UInt8 *src, Int32 sbpl, UInt32 sgamma, UInt8 *dest, Int32 dbpl, Int32 width, Int32 height);
		void UncorrImage32(UInt8 *src, Int32 sbpl, UInt32 sgamma, UInt8 *dest, Int32 dbpl, Int32 width, Int32 height);
	};
};
#endif
