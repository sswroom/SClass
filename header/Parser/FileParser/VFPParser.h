#ifndef _SM_PARSER_FILEPARSER_VFPPARSER
#define _SM_PARSER_FILEPARSER_VFPPARSER
#include "IO/FileParser.h"
#include "Media/VFPManager.h"

namespace Parser
{
	namespace FileParser
	{
		class VFPParser : public IO::FileParser
		{
		private:
			Media::VFPManager *vfpMgr;
		public:
			VFPParser();
			virtual ~VFPParser();

			virtual Int32 GetName();
			virtual void PrepareSelector(NotNullPtr<IO::FileSelector> selector, IO::ParserType t);
			virtual IO::ParserType GetParserType();
			virtual IO::ParsedObject *ParseFileHdr(NotNullPtr<IO::StreamData> fd, IO::PackageFile *pkgFile, IO::ParserType targetType, const UInt8 *hdr);
		};
	}
}
#endif
