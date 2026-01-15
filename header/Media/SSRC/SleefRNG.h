#ifndef _SM_MEDIA_SSRC_SLEEFRNG
#define _SM_MEDIA_SSRC_SLEEFRNG
// PRNG for SSRC, written by Naoki Shibata ( shibatch@users.sourceforge.net )
// Modified by sswroom
// This PRNG passes Big Crush TestU01. Period length = 2^128

namespace Media
{
	namespace SSRC
	{
		class SleefRNG
		{
		private:
			struct U128
			{
				UInt64 l;
				UInt64 u;
			};

			U128 state;

			UInt64 remain;
			UIntOS remainSize;

			UInt64 Xrandom64();
			static UInt32 Add64c(UInt64 *ret, UInt64 *u, UInt64 *v);
			static void Add128(U128 *r, U128 *u, U128 *v);
			static U128 NextState(U128 state);
		public:
			SleefRNG(UInt64 seed);
			~SleefRNG();

			UInt64 Next(UInt32 nbits);
			UInt64 Next64();
			void NextBytes(UInt8 *ptr, UIntOS z);
			Double NextDouble();
			Double NextRectangularDouble(Double min, Double max);
			void FillRectangularDouble(Double *ptr, UIntOS z, Double min, Double max);
			Double NextTriangularDouble(Double peak);
			void FillTriangularDouble(Double *ptr, UIntOS z, Double peak);
			Double NextTwoLevelDouble(Double peak);
			void FillTwoLevelDouble(Double *ptr, UIntOS z, Double peak);
		};
	}
}
#endif
