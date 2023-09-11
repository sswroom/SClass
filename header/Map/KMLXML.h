#ifndef _SM_MAP_KMLXML
#define _SM_MAP_KMLXML
#include "Data/ICaseStringMap.h"
#include "Map/GPSTrack.h"
#include "Map/MapDrawLayer.h"
#include "Media/SharedImage.h"
#include "Net/WebBrowser.h"
#include "Parser/ParserList.h"
#include "Text/String.h"
#include "Text/XMLReader.h"

namespace Map
{
	class KMLXML
	{
	private:
		struct KMLStyle
		{
			UInt32 lineColor;
			UInt32 lineWidth;
			Text::String *iconURL;
			Int32 iconSpotX;
			Int32 iconSpotY;
			UInt32 iconColor;
			UInt32 fillColor;
			Int32 flags;
			Media::SharedImage *img;
		};
	public:
		static Map::MapDrawLayer *ParseKMLRoot(NotNullPtr<Text::XMLReader> reader, Text::CStringNN fileName, Parser::ParserList *parsers, Net::WebBrowser *browser, IO::PackageFile *pkgFile);
	private:
		static Map::MapDrawLayer *ParseKMLContainer(NotNullPtr<Text::XMLReader> reader, Data::ICaseStringMap<KMLStyle*> *styles, Text::CStringNN sourceName, Parser::ParserList *parsers, Net::WebBrowser *browser, IO::PackageFile *basePF);
		static void ParseKMLPlacemarkTrack(NotNullPtr<Text::XMLReader> reader, Map::GPSTrack *lyr, Data::StringMap<KMLStyle*> *styles);
		static Map::MapDrawLayer *ParseKMLPlacemarkLyr(NotNullPtr<Text::XMLReader> reader, Data::StringMap<KMLStyle*> *styles, Text::CStringNN sourceName, Parser::ParserList *parsers, Net::WebBrowser *browser, IO::PackageFile *basePF);
		static void ParseCoordinates(NotNullPtr<Text::XMLReader> reader, Data::ArrayList<Double> *coordList, Data::ArrayList<Double> *altList);
	};
}
#endif
