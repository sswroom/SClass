#ifndef _SM_CRYPTO_X509FILE
#define _SM_CRYPTO_X509FILE
#include "Net/ASN1Data.h"

namespace Crypto
{
	class X509File : public Net::ASN1Data
	{
	public:
		typedef enum
		{
			FT_CERT,
			FT_RSA_KEY,
			FT_CERT_REQ,
			FT_PRIV_KEY
		} FileType;
	private:
		FileType fileType;

		X509File(const UTF8Char *sourceName, const UInt8 *buff, UOSInt buffSize, FileType fileType);
	public:
		virtual ~X509File();

		virtual Net::ASN1Data::ASN1Type GetASN1Type();
		FileType GetFileType();

		static X509File *LoadFile(const UTF8Char *sourceName, const UInt8 *buff, UOSInt buffSize, FileType fileType);
	};
}
#endif
