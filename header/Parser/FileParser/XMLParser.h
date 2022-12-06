#ifndef _SM_PARSER_FILEPARSER_XMLPARSER
#define _SM_PARSER_FILEPARSER_XMLPARSER
#include "Data/ICaseStringMap.h"
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
			Text::EncodingFactory *encFact;
			Parser::ParserList *parsers;
			Net::WebBrowser *browser;
		public:
			XMLParser();
			virtual ~XMLParser();

			virtual Int32 GetName();
			virtual void SetCodePage(UInt32 codePage);
			virtual void SetParserList(Parser::ParserList *parsers);
			virtual void SetWebBrowser(Net::WebBrowser *browser);
			virtual void SetEncFactory(Text::EncodingFactory *encFact);
			virtual void PrepareSelector(IO::IFileSelector *selector, IO::ParserType t);
			virtual IO::ParserType GetParserType();
			virtual IO::ParsedObject *ParseFileHdr(IO::IStreamData *fd, IO::PackageFile *pkgFile, IO::ParserType targetType, const UInt8 *hdr);

			static IO::ParsedObject *ParseStream(Text::EncodingFactory *encFact, IO::Stream *stm, Text::CString fileName, Parser::ParserList *parsers, Net::WebBrowser *browser, IO::PackageFile *pkgFile);
		private:
			static Bool ParseGPXPoint(Text::XMLReader *reader, Map::GPSTrack::GPSRecord3 *rec);
			static Bool ParseVSProjFile(Text::XMLReader *reader, Text::VSProjContainer *container);
			static Bool ParseVSConfFile(Text::XMLReader *reader, Text::CodeProject *proj);
		};
	}
}
#endif
