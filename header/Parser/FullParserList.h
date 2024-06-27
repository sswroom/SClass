#ifndef _SM_PARSER_FULLPARSERLIST
#define _SM_PARSER_FULLPARSERLIST
#include "Math/ArcGISPRJParser.h"
#include "Parser/ParserList.h"

namespace Parser
{
	class FullParserList : public Parser::ParserList
	{
	private:
		Math::ArcGISPRJParser prjParser;
	public:
		FullParserList();
		virtual ~FullParserList();

		NN<Math::ArcGISPRJParser> GetPRJParser();
	};
}
#endif
