#ifndef _SM_CRYPTO_JKSFILE
#define _SM_CRYPTO_JKSFILE
#include "Crypto/Cert/X509File.h"

namespace Crypto
{
	class JKSFile : public Crypto::Cert::X509File
	{
	public:
		JKSFile(Text::String *sourceName, const UInt8 *buff, UOSInt buffSize);
		JKSFile(Text::CString sourceName, const UInt8 *buff, UOSInt buffSize);
		virtual ~JKSFile();

		virtual FileType GetFileType();
		virtual void ToShortName(Text::StringBuilderUTF8 *sb);
	
		virtual Net::ASN1Data *Clone();
		virtual void ToString(Text::StringBuilderUTF8 *sb);
	};
}
#endif
