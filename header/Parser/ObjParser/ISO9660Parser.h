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
			virtual void PrepareSelector(NN<IO::FileSelector> selector, IO::ParserType t);
			virtual IO::ParserType GetParserType();
			virtual IO::ParsedObject *ParseObject(NN<IO::ParsedObject> pobj, IO::PackageFile *pkgFile, IO::ParserType targetType);

			NN<IO::PackageFile> ParseVol(NN<IO::ISectorData> sectorData, UInt32 sectorNum, UInt32 codePage);
			void ParseDir(NN<IO::VirtualPackageFile> pkgFile, NN<IO::ISectorData> sectorData, UInt32 sectorNum, UInt32 recSize, UTF8Char *fileNameBuff, UTF8Char *fileNameEnd, UInt32 codePage);
		};
	}
}
#endif
