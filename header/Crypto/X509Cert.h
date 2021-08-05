#ifndef _SM_CRYPTO_X509CERT
#define _SM_CRYPTO_X509CERT
#include "Crypto/X509File.h"

namespace Crypto
{
	class X509Cert : public Crypto::X509File
	{
	public:
		X509Cert(const UTF8Char *sourceName, const UInt8 *buff, UOSInt buffSize);
		virtual ~X509Cert();

		virtual FileType GetFileType();
		virtual void ToString(Text::StringBuilderUTF *sb);
	};
}
#endif
