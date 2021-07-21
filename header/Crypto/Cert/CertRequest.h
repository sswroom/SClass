#ifndef _SM_CRYPTO_CERT_CERTREQUEST
#define _SM_CRYPTO_CERT_CERTREQUEST

namespace Crypto
{
	namespace Cert
	{
		class CertRequest
		{
		private:
			UInt8 *reqBuff;
			UOSInt reqBuffSize;
		public:
			CertRequest(const UTF8Char *fileName);
			~CertRequest();
		};
	}
}
#endif
