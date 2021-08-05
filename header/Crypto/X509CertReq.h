#ifndef _SM_CRYPTO_X509CERTREQ
#define _SM_CRYPTO_X509CERTREQ
#include "Crypto/X509File.h"

namespace Crypto
{
	class X509CertReq : public Crypto::X509File
	{
	public:
		X509CertReq(const UTF8Char *sourceName, const UInt8 *buff, UOSInt buffSize);
		virtual ~X509CertReq();

		virtual FileType GetFileType();
		virtual void ToString(Text::StringBuilderUTF *sb);
	};
}
#endif
