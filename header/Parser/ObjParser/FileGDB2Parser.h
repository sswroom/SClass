#ifndef _SM_PARSER_OBJPARSER_FILEGDB2PARSER
#define _SM_PARSER_OBJPARSER_FILEGDB2PARSER
#include "IO/ObjectParser.h"

namespace Parser
{
	namespace ObjParser
	{
		class FileGDB2Parser : public IO::ObjectParser
		{
		private:
			Math::ArcGISPRJParser *prjParser;
		public:
			FileGDB2Parser();
			virtual ~FileGDB2Parser();

			virtual Int32 GetName();
			virtual void SetArcGISPRJParser(Math::ArcGISPRJParser *prjParser);
			virtual void PrepareSelector(IO::FileSelector *selector, IO::ParserType t);
			virtual IO::ParserType GetParserType();
			virtual IO::ParsedObject *ParseObject(IO::ParsedObject *pobj, IO::PackageFile *pkgFile, IO::ParserType targetType);
		};
	}
}
#endif
