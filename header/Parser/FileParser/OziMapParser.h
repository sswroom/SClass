#ifndef _SM_PARSER_FILEPARSER_OZIMAPPARSER
#define _SM_PARSER_FILEPARSER_OZIMAPPARSER
#include "IO/FileParser.h"

namespace Parser
{
	namespace FileParser
	{
		class OziMapParser : public IO::FileParser
		{
		private:
			Optional<Parser::ParserList> parsers;
			static Bool __stdcall FitCorners(UnsafeArray<Double> ptXY, UInt32 nPts, Double imgW, Double imgH, Math::Coord2DDbl &tl, Math::Coord2DDbl &tr, Math::Coord2DDbl &br, Math::Coord2DDbl &bl);
		public:
			OziMapParser();
			virtual ~OziMapParser();

			virtual Int32 GetName();
			virtual void SetParserList(Optional<Parser::ParserList> parsers);
			virtual void PrepareSelector(NN<IO::FileSelector> selector, IO::ParserType t);
			virtual IO::ParserType GetParserType();
			virtual Optional<IO::ParsedObject> ParseFileHdr(NN<IO::StreamData> fd, Optional<IO::PackageFile> pkgFile, IO::ParserType targetType, Data::ByteArrayR hdr);
		};
	}
}
#endif
