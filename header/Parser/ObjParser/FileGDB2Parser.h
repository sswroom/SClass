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
			Optional<Math::ArcGISPRJParser> prjParser;
		public:
			FileGDB2Parser();
			virtual ~FileGDB2Parser();

			virtual Int32 GetName();
			virtual void SetArcGISPRJParser(Optional<Math::ArcGISPRJParser> prjParser);
			virtual void PrepareSelector(NN<IO::FileSelector> selector, IO::ParserType t);
			virtual IO::ParserType GetParserType();
			virtual Optional<IO::ParsedObject> ParseObject(NN<IO::ParsedObject> pobj, Optional<IO::PackageFile> pkgFile, IO::ParserType targetType);
		};
	}
}
#endif
