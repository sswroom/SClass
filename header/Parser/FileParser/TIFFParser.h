#ifndef _SM_PARSER_FILEPARSER_TIFFPARSER
#define _SM_PARSER_FILEPARSER_TIFFPARSER
#include "IO/FileParser.h"
#include "Media/EXIFData.h"

namespace Parser
{
	namespace FileParser
	{
		class TIFFParser : public IO::FileParser
		{
		private:
			Parser::ParserList *parsers;
		public:
			TIFFParser();
			virtual ~TIFFParser();

			virtual Int32 GetName();
			virtual void SetParserList(Parser::ParserList *parsers);
			virtual void PrepareSelector(IO::IFileSelector *selector, IO::ParserType t);
			virtual IO::ParserType GetParserType();
			virtual IO::ParsedObject *ParseFile(IO::IStreamData *fd, IO::PackageFile *pkgFile, IO::ParserType targetType);

		private:
			static UInt32 GetUInt(Media::EXIFData *exif, Int32 id);
			static UInt32 GetUInt0(Media::EXIFData *exif, Int32 id);
			static UInt32 GetUIntSum(Media::EXIFData *exif, Int32 id, UOSInt *nChannels);
		};
	};
};
#endif
