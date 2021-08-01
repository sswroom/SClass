#ifndef _SM_CRYPTO_X509FILE
#define _SM_CRYPTO_X509FILE
#include "IO/ParsedObject.h"
#include "Text/StringBuilderUTF.h"

namespace Crypto
{
	class X509File : public IO::ParsedObject
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
		UInt8 *buff;
		UOSInt buffSize;
		FileType fileType;

		X509File(const UTF8Char *sourceName, const UInt8 *buff, UOSInt buffSize, FileType fileType);
	public:
		virtual ~X509File();

		virtual IO::ParsedObject::ParserType GetParserType();

		Bool ToASN1String(Text::StringBuilderUTF *sb);
		FileType GetFileType();
		const UInt8 *GetASN1Buff();
		UOSInt GetASN1BuffSize();

		static X509File *LoadFile(const UTF8Char *sourceName, const UInt8 *buff, UOSInt buffSize, FileType fileType);
	};
}
#endif
