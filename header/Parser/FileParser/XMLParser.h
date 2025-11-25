#ifndef _SM_PARSER_FILEPARSER_XMLPARSER
#define _SM_PARSER_FILEPARSER_XMLPARSER
#include "Data/ICaseStringMap.hpp"
#include "IO/FileParser.h"
#include "Map/VectorLayer.h"
#include "Map/GPSTrack.h"
#include "Media/SharedImage.h"
#include "Text/VSProject.h"
#include "Text/XMLReader.h"

namespace Parser
{
	namespace FileParser
	{
		class XMLParser : public IO::FileParser
		{
		private:
			UInt32 codePage;
			Optional<Text::EncodingFactory> encFact;
			Optional<Parser::ParserList> parsers;
			Optional<Net::WebBrowser> browser;
		public:
			XMLParser();
			virtual ~XMLParser();

			virtual Int32 GetName();
			virtual void SetCodePage(UInt32 codePage);
			virtual void SetParserList(Optional<Parser::ParserList> parsers);
			virtual void SetWebBrowser(Optional<Net::WebBrowser> browser);
			virtual void SetEncFactory(Optional<Text::EncodingFactory> encFact);
			virtual void PrepareSelector(NN<IO::FileSelector> selector, IO::ParserType t);
			virtual IO::ParserType GetParserType();
			virtual Optional<IO::ParsedObject> ParseFileHdr(NN<IO::StreamData> fd, Optional<IO::PackageFile> pkgFile, IO::ParserType targetType, Data::ByteArrayR hdr);

			static Optional<IO::ParsedObject> ParseStream(Optional<Text::EncodingFactory> encFact, NN<IO::Stream> stm, Text::CStringNN fileName, Optional<Parser::ParserList> parsers, Optional<Net::WebBrowser> browser, Optional<IO::PackageFile> pkgFile);
		private:
			static Bool ParseGPXPoint(NN<Text::XMLReader> reader, Map::GPSTrack::GPSRecord3 *rec);
			static Bool ParseVSProjFile(NN<Text::XMLReader> reader, NN<Text::VSProjContainer> container);
			static Bool ParseVSConfFile(NN<Text::XMLReader> reader, NN<Text::CodeProject> proj);
		};
	}
}
#endif
