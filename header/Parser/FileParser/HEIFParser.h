#ifndef _SM_PARSER_FILEPARSER_HEIFPARSER
#define _SM_PARSER_FILEPARSER_HEIFPARSER
#include "IO/FileParser.h"
#include "Media/EXIFData.h"
#include "Media/ICCProfile.h"
#include "Text/XMLDOM.h"

namespace Parser
{
	namespace FileParser
	{
		class HEIFParser : public IO::FileParser
		{
		public:
			HEIFParser();
			virtual ~HEIFParser();

			virtual Int32 GetName();
			virtual void PrepareSelector(NotNullPtr<IO::FileSelector> selector, IO::ParserType t);
			virtual IO::ParserType GetParserType();
			virtual IO::ParsedObject *ParseFileHdr(NotNullPtr<IO::StreamData> fd, IO::PackageFile *pkgFile, IO::ParserType targetType, const UInt8 *hdr);

			static Bool ParseHeaders(NotNullPtr<IO::StreamData> fd, OutParam<Optional<Media::EXIFData>> exif, OutParam<Optional<Text::XMLDocument>> xmf, OutParam<Optional<Media::ICCProfile>> icc, OutParam<UInt32> width, OutParam<UInt32> height);
		};
	}
}
#endif
