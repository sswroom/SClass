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
			virtual void PrepareSelector(NN<IO::FileSelector> selector, IO::ParserType t);
			virtual IO::ParserType GetParserType();
			virtual Optional<IO::ParsedObject> ParseFileHdr(NN<IO::StreamData> fd, Optional<IO::PackageFile> pkgFile, IO::ParserType targetType, Data::ByteArrayR hdr);

			static Optional<Crypto::Cert::X509File> ParseBuff(Data::ByteArrayR buff, NN<Text::String> fileName);
			static Optional<Crypto::Cert::X509File> ToType(NN<IO::ParsedObject> pobj, Crypto::Cert::X509File::FileType ftype);
			static Optional<Crypto::Cert::X509File> ParseBinary(Data::ByteArrayR buff);
		};
	}
}
#endif
