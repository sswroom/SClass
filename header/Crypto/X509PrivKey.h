#ifndef _SM_CRYPTO_X509PRIVKEY
#define _SM_CRYPTO_X509PRIVKEY
#include "Crypto/X509File.h"

namespace Crypto
{
	class X509PrivKey : public Crypto::X509File
	{
	public:
		X509PrivKey(const UTF8Char *sourceName, const UInt8 *buff, UOSInt buffSize);
		virtual ~X509PrivKey();

		virtual FileType GetFileType();
		virtual void ToString(Text::StringBuilderUTF *sb);

		static X509PrivKey *CreateFromRSAKey(const UInt8 *buff, UOSInt buffSize);
	};
}
#endif
