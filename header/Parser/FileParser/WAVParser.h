#ifndef _SM_PARSER_FILEPARSER_WAVPARSER
#define _SM_PARSER_FILEPARSER_WAVPARSER
#include "IO/IFileParser.h"

namespace Parser
{
	namespace FileParser
	{
		class WAVParser : public IO::IFileParser
		{
		public:
			WAVParser();
			virtual ~WAVParser();

			virtual Int32 GetName();
			virtual void PrepareSelector(IO::IFileSelector *selector, IO::ParsedObject::ParserType t);
			virtual IO::ParsedObject::ParserType GetParserType();
			virtual IO::ParsedObject *ParseFile(IO::IStreamData *fd, IO::PackageFile *pkgFile, IO::ParsedObject::ParserType targetType);
		};
	};
};
#endif
