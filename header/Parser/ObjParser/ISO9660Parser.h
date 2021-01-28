#ifndef _SM_PARSER_OBJPARSER_ISO9660PARSER
#define _SM_PARSER_OBJPARSER_ISO9660PARSER
#include "IO/IObjectParser.h"
#include "IO/ISectorData.h"
#include "IO/PackageFile.h"

namespace Parser
{
	namespace ObjParser
	{
		class ISO9660Parser : public IO::IObjectParser
		{
		public:
			ISO9660Parser();
			virtual ~ISO9660Parser();

			virtual Int32 GetName();
			virtual void PrepareSelector(IO::IFileSelector *selector, IO::ParsedObject::ParserType t);
			virtual IO::ParsedObject::ParserType GetParserType();
			virtual IO::ParsedObject *ParseObject(IO::ParsedObject *pobj, IO::PackageFile *pkgFile, IO::ParsedObject::ParserType targetType);

			IO::PackageFile *ParseVol(IO::ISectorData *sectorData, UInt32 sectorNum, Int32 codePage);
			void ParseDir(IO::PackageFile *pkgFile, IO::ISectorData *sectorData, UInt32 sectorNum, UInt32 recSize, UTF8Char *fileNameBuff, UTF8Char *fileNameEnd, Int32 codePage);
		};
	};
};
#endif
