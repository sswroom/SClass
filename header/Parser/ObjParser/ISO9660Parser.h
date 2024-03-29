#ifndef _SM_PARSER_OBJPARSER_ISO9660PARSER
#define _SM_PARSER_OBJPARSER_ISO9660PARSER
#include "IO/ISectorData.h"
#include "IO/ObjectParser.h"
#include "IO/VirtualPackageFile.h"

namespace Parser
{
	namespace ObjParser
	{
		class ISO9660Parser : public IO::ObjectParser
		{
		public:
			ISO9660Parser();
			virtual ~ISO9660Parser();

			virtual Int32 GetName();
			virtual void PrepareSelector(NotNullPtr<IO::FileSelector> selector, IO::ParserType t);
			virtual IO::ParserType GetParserType();
			virtual IO::ParsedObject *ParseObject(NotNullPtr<IO::ParsedObject> pobj, IO::PackageFile *pkgFile, IO::ParserType targetType);

			NotNullPtr<IO::PackageFile> ParseVol(NotNullPtr<IO::ISectorData> sectorData, UInt32 sectorNum, UInt32 codePage);
			void ParseDir(NotNullPtr<IO::VirtualPackageFile> pkgFile, NotNullPtr<IO::ISectorData> sectorData, UInt32 sectorNum, UInt32 recSize, UTF8Char *fileNameBuff, UTF8Char *fileNameEnd, UInt32 codePage);
		};
	}
}
#endif
