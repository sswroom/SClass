#ifndef _SM_CRYPTO_CERT_RSAKEY
#define _SM_CRYPTO_CERT_RSAKEY

namespace Crypto
{
	namespace Cert
	{
		class RSAKey
		{
		public:
			virtual const UInt8 *GetRSAModulus(OptOut<UOSInt> size) const = 0;
			virtual const UInt8 *GetRSAPublicExponent(OptOut<UOSInt> size) const = 0;
			virtual const UInt8 *GetRSAPrivateExponent(OptOut<UOSInt> size) const = 0;
			virtual const UInt8 *GetRSAPrime1(OptOut<UOSInt> size) const = 0;
			virtual const UInt8 *GetRSAPrime2(OptOut<UOSInt> size) const = 0;
			virtual const UInt8 *GetRSAExponent1(OptOut<UOSInt> size) const = 0;
			virtual const UInt8 *GetRSAExponent2(OptOut<UOSInt> size) const = 0;
			virtual const UInt8 *GetRSACoefficient(OptOut<UOSInt> size) const = 0;
		};
	}
}
#endif
