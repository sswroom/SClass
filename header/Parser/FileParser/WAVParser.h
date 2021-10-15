#ifndef _SM_PARSER_FILEPARSER_WAVPARSER
#define _SM_PARSER_FILEPARSER_WAVPARSER
#include "IO/FileParser.h"

namespace Parser
{
	namespace FileParser
	{
		class WAVParser : public IO::FileParser
		{
		public:
			WAVParser();
			virtual ~WAVParser();

			virtual Int32 GetName();
			virtual void PrepareSelector(IO::IFileSelector *selector, IO::ParserType t);
			virtual IO::ParserType GetParserType();
			virtual IO::ParsedObject *ParseFile(IO::IStreamData *fd, IO::PackageFile *pkgFile, IO::ParserType targetType);
		};
	};
};
#endif
