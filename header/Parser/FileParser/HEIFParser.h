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
			virtual void PrepareSelector(IO::FileSelector *selector, IO::ParserType t);
			virtual IO::ParserType GetParserType();
			virtual IO::ParsedObject *ParseFileHdr(IO::StreamData *fd, IO::PackageFile *pkgFile, IO::ParserType targetType, const UInt8 *hdr);

			static Bool ParseHeaders(IO::StreamData *fd, Media::EXIFData **exif, Text::XMLDocument **xmf, Media::ICCProfile **icc, UInt32 *width, UInt32 *height);
		};
	}
}
#endif
