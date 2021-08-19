#ifndef _SM_CRYPTO_JKSFILE
#define _SM_CRYPTO_JKSFILE
#include "Crypto/X509File.h"

namespace Crypto
{
	class JKSFile : public Crypto::X509File
	{
	public:
		JKSFile(const UTF8Char *sourceName, const UInt8 *buff, UOSInt buffSize);
		virtual ~JKSFile();

		virtual FileType GetFileType();
		virtual Net::ASN1Data *Clone();
		virtual void ToString(Text::StringBuilderUTF *sb);
	};
}
#endif
