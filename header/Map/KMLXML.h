#ifndef _SM_MAP_KMLXML
#define _SM_MAP_KMLXML
#include "Data/ArrayListT.hpp"
#include "Data/ICaseStringMapObj.hpp"
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
		static Optional<Map::MapDrawLayer> ParseKMLRoot(NN<Text::XMLReader> reader, Text::CStringNN fileName, Optional<Parser::ParserList> parsers, Optional<Net::WebBrowser> browser, Optional<IO::PackageFile> pkgFile);
	private:
		static Optional<Map::MapDrawLayer> ParseKMLContainer(NN<Text::XMLReader> reader, Data::ICaseStringMapObj<KMLStyle*> *styles, Text::CStringNN sourceName, Optional<Parser::ParserList> parsers, Optional<Net::WebBrowser> browser, Optional<IO::PackageFile> basePF, Bool rootKml);
		static void ParseKMLPlacemarkTrack(NN<Text::XMLReader> reader, NN<Map::GPSTrack> lyr, Data::StringMapObj<KMLStyle*> *styles);
		static Optional<Map::MapDrawLayer> ParseKMLPlacemarkLyr(NN<Text::XMLReader> reader, Data::StringMapObj<KMLStyle*> *styles, Text::CStringNN sourceName, Optional<Parser::ParserList> parsers, Optional<Net::WebBrowser> browser, Optional<IO::PackageFile> basePF);
		static Optional<Math::Geometry::Vector2D> ParseKMLVector(NN<Text::XMLReader> reader, NN<Data::ArrayListStringNN> colNames, NN<Data::ArrayListStringNN> colValues, NN<Data::ArrayListT<Map::VectorLayer::ColInfo>> colInfos);
		static void ParseCoordinates(NN<Text::XMLReader> reader, NN<Data::ArrayListA<Math::Coord2DDbl>> coordList, NN<Data::ArrayListNative<Double>> altList);
	};
}
#endif
