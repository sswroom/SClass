#ifndef _SM_PARSER_FILEPARSER_X509PARSER
#define _SM_PARSER_FILEPARSER_X509PARSER
#include "Crypto/Cert/X509File.h"
#include "Data/ByteArray.h"
#include "IO/FileParser.h"

namespace Parser
{
	namespace FileParser
	{
		class X509Parser : public IO::FileParser
		{
		public:
			X509Parser();
			virtual ~X509Parser();

			virtual Int32 GetName();
			virtual void PrepareSelector(NotNullPtr<IO::FileSelector> selector, IO::ParserType t);
			virtual IO::ParserType GetParserType();
			virtual IO::ParsedObject *ParseFileHdr(NotNullPtr<IO::StreamData> fd, IO::PackageFile *pkgFile, IO::ParserType targetType, const UInt8 *hdr);

			static Crypto::Cert::X509File *ParseBuff(Data::ByteArrayR buff, NotNullPtr<Text::String> fileName);
			static Crypto::Cert::X509File *ToType(IO::ParsedObject *pobj, Crypto::Cert::X509File::FileType ftype);
			static Crypto::Cert::X509File *ParseBinary(Data::ByteArrayR buff);
		};
	}
}
#endif
