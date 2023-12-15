#ifndef _SM_MAP_KMLXML
#define _SM_MAP_KMLXML
#include "Data/ICaseStringMap.h"
#include "Map/GPSTrack.h"
#include "Map/MapDrawLayer.h"
#include "Map/VectorLayer.h"
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
			Double lineWidth;
			Text::String *iconURL;
			Int32 iconSpotX;
			Int32 iconSpotY;
			UInt32 iconColor;
			UInt32 fillColor;
			Int32 flags;
			Media::SharedImage *img;
		};
	public:
		static Optional<Map::MapDrawLayer> ParseKMLRoot(NotNullPtr<Text::XMLReader> reader, Text::CStringNN fileName, Parser::ParserList *parsers, Net::WebBrowser *browser, IO::PackageFile *pkgFile);
	private:
		static Optional<Map::MapDrawLayer> ParseKMLContainer(NotNullPtr<Text::XMLReader> reader, Data::ICaseStringMap<KMLStyle*> *styles, Text::CStringNN sourceName, Parser::ParserList *parsers, Net::WebBrowser *browser, IO::PackageFile *basePF, Bool rootKml);
		static void ParseKMLPlacemarkTrack(NotNullPtr<Text::XMLReader> reader, NotNullPtr<Map::GPSTrack> lyr, Data::StringMap<KMLStyle*> *styles);
		static Optional<Map::MapDrawLayer> ParseKMLPlacemarkLyr(NotNullPtr<Text::XMLReader> reader, Data::StringMap<KMLStyle*> *styles, Text::CStringNN sourceName, Parser::ParserList *parsers, Net::WebBrowser *browser, IO::PackageFile *basePF);
		static Optional<Math::Geometry::Vector2D> ParseKMLVector(NotNullPtr<Text::XMLReader> reader, NotNullPtr<Data::ArrayListNN<Text::String>> colNames, NotNullPtr<Data::ArrayListNN<Text::String>> colValues, NotNullPtr<Data::ArrayList<Map::VectorLayer::ColInfo>> colInfos);
		static void ParseCoordinates(NotNullPtr<Text::XMLReader> reader, NotNullPtr<Data::ArrayListA<Math::Coord2DDbl>> coordList, NotNullPtr<Data::ArrayList<Double>> altList);
	};
}
#endif
