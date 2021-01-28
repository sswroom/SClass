#ifndef _SM_PARSER_FILEPARSER_XMLPARSER
#define _SM_PARSER_FILEPARSER_XMLPARSER
#include "Data/ICaseStringUTF8Map.h"
#include "IO/IFileParser.h"
#include "Map/VectorLayer.h"
#include "Map/GPSTrack.h"
#include "Text/VSProject.h"
#include "Text/XMLReader.h"

namespace Parser
{
	namespace FileParser
	{
		class XMLParser : public IO::IFileParser
		{
		private:
			typedef struct
			{
				Int32 lineColor;
				Int32 lineWidth;
				const UTF8Char *iconURL;
				Int32 iconSpotX;
				Int32 iconSpotY;
				Int32 iconColor;
				Int32 fillColor;
				Int32 flags;
			} KMLStyle;

		private:
			Int32 codePage;
			Text::EncodingFactory *encFact;
			Parser::ParserList *parsers;
			Net::WebBrowser *browser;
		public:
			XMLParser();
			virtual ~XMLParser();

			virtual Int32 GetName();
			virtual void SetCodePage(Int32 codePage);
			virtual void SetParserList(Parser::ParserList *parsers);
			virtual void SetWebBrowser(Net::WebBrowser *browser);
			virtual void SetEncFactory(Text::EncodingFactory *encFact);
			virtual void PrepareSelector(IO::IFileSelector *selector, IO::ParsedObject::ParserType t);
			virtual IO::ParsedObject::ParserType GetParserType();
			virtual IO::ParsedObject *ParseFile(IO::IStreamData *fd, IO::PackageFile *pkgFile, IO::ParsedObject::ParserType targetType);

			static IO::ParsedObject *ParseStream(Text::EncodingFactory *encFact, IO::Stream *stm, const UTF8Char *fileName, Parser::ParserList *parsers, Net::WebBrowser *browser, IO::PackageFile *pkgFile);
		private:
			static Map::IMapDrawLayer *ParseKMLContainer(Text::XMLReader *reader, Data::ICaseStringUTF8Map<KMLStyle*> *styles, const UTF8Char *sourceName, Parser::ParserList *parsers, Net::WebBrowser *browser, IO::PackageFile *basePF);
			static void ParseKMLPlacemarkTrack(Text::XMLReader *reader, Map::GPSTrack *lyr, Data::StringUTF8Map<KMLStyle*> *styles);
			static void ParseCoordinates(Text::XMLReader *reader, Data::ArrayList<Double> *coordList, Data::ArrayList<Double> *altList);
			static Map::IMapDrawLayer *ParseKMLPlacemarkLyr(Text::XMLReader *reader, Data::StringUTF8Map<KMLStyle*> *styles, const UTF8Char *sourceName, Parser::ParserList *parsers, Net::WebBrowser *browser, IO::PackageFile *basePF);
			static Bool ParseGPXPoint(Text::XMLReader *reader, Map::GPSTrack::GPSRecord *rec);
			static Bool ParseVSProjFile(Text::XMLReader *reader, Text::VSProjContainer *container);
			static Bool ParseVSConfFile(Text::XMLReader *reader, Text::CodeProject *proj);
		};
	}
}
#endif
