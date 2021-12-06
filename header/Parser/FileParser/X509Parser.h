#ifndef _SM_PARSER_FILEPARSER_X509PARSER
#define _SM_PARSER_FILEPARSER_X509PARSER
#include "Crypto/Cert/X509File.h"
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
			virtual void PrepareSelector(IO::IFileSelector *selector, IO::ParserType t);
			virtual IO::ParserType GetParserType();
			virtual IO::ParsedObject *ParseFile(IO::IStreamData *fd, IO::PackageFile *pkgFile, IO::ParserType targetType);

			static Crypto::Cert::X509File *ParseBuff(const UInt8 *buff, UOSInt buffSize, Text::String *fileName);
		};
	}
}
#endif
