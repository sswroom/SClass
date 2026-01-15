#ifndef _SM_MAP_OWSFEATUREPARSER
#define _SM_MAP_OWSFEATUREPARSER
#include "Data/ArrayListNative.hpp"
#include "Data/ArrayListStringNN.h"
#include "Math/Geometry/Vector2D.h"
#include "Text/EncodingFactory.h"
#include "Text/PString.h"
#include "Text/String.h"
#include "Text/XMLReader.h"

namespace Map
{
	class OWSFeatureParser
	{
	public:
		static Bool ParseText(Text::PString txt, UInt32 srid, Math::Coord2DDbl coord, NN<Data::ArrayListNN<Math::Geometry::Vector2D>> vecList, NN<Data::ArrayListNative<UIntOS>> valueOfstList, NN<Data::ArrayListStringNN> nameList, NN<Data::ArrayListNN<Text::String>> valueList);
		static Bool ParseJSON(Text::CStringNN json, UInt32 srid, NN<Data::ArrayListNN<Math::Geometry::Vector2D>> vecList, NN<Data::ArrayListNative<UIntOS>> valueOfstList, NN<Data::ArrayListStringNN> nameList, NN<Data::ArrayListNN<Text::String>> valueList, Math::Coord2DDbl coord);
		static Bool ParseGML(Text::CStringNN json, UInt32 srid, Bool swapXY, Optional<Text::EncodingFactory> encFact, NN<Data::ArrayListNN<Math::Geometry::Vector2D>> vecList, NN<Data::ArrayListNative<UIntOS>> valueOfstList, NN<Data::ArrayListStringNN> nameList, NN<Data::ArrayListNN<Text::String>> valueList);
		static Bool ParseESRI_WMS_XML(Text::CStringNN xml, UInt32 srid, Bool swapXY, Optional<Text::EncodingFactory> encFact, NN<Data::ArrayListNN<Math::Geometry::Vector2D>> vecList, NN<Data::ArrayListNative<UIntOS>> valueOfstList, NN<Data::ArrayListStringNN> nameList, NN<Data::ArrayListNN<Text::String>> valueList, Math::Coord2DDbl coord);
		static Bool ParseESRIFeatureInfoResponse(NN<Text::XMLReader> reader, UInt32 srid, Bool swapXY, NN<Data::ArrayListNN<Math::Geometry::Vector2D>> vecList, NN<Data::ArrayListNative<UIntOS>> valueOfstList, NN<Data::ArrayListStringNN> nameList, NN<Data::ArrayListNN<Text::String>> valueList, Math::Coord2DDbl coord);
		static Bool ParseESRIFeatureInfoCollection(NN<Text::XMLReader> reader, UInt32 srid, Bool swapXY, NN<Data::ArrayListNN<Math::Geometry::Vector2D>> vecList, NN<Data::ArrayListNative<UIntOS>> valueOfstList, NN<Data::ArrayListStringNN> nameList, NN<Data::ArrayListNN<Text::String>> valueList, Math::Coord2DDbl coord);
		static Bool ParseESRIFeatureInfo(NN<Text::XMLReader> reader, UInt32 srid, Bool swapXY, NN<Data::ArrayListNN<Math::Geometry::Vector2D>> vecList, NN<Data::ArrayListNative<UIntOS>> valueOfstList, NN<Data::ArrayListStringNN> nameList, NN<Data::ArrayListNN<Text::String>> valueList, Math::Coord2DDbl coord);
		static Optional<Math::Geometry::Vector2D> ParseESRIFieldGeometry(NN<Text::XMLReader> reader, UInt32 srid, Bool swapXY);
		static Bool ParseOGC_WMS_XML(Text::CStringNN xml, UInt32 srid, Math::Coord2DDbl coord, Optional<Text::EncodingFactory> encFact, NN<Data::ArrayListNN<Math::Geometry::Vector2D>> vecList, NN<Data::ArrayListNative<UIntOS>> valueOfstList, NN<Data::ArrayListStringNN> nameList, NN<Data::ArrayListNN<Text::String>> valueList);
	};
}
#endif
