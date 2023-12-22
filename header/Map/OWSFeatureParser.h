#ifndef _SM_MAP_OWSFEATUREPARSER
#define _SM_MAP_OWSFEATUREPARSER
#include "Data/ArrayList.h"
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
		static Bool ParseText(Text::PString txt, UInt32 srid, Math::Coord2DDbl coord, NotNullPtr<Data::ArrayListNN<Math::Geometry::Vector2D>> vecList, Data::ArrayList<UOSInt> *valueOfstList, Data::ArrayListStringNN *nameList, Data::ArrayList<Text::String*> *valueList);
		static Bool ParseJSON(Text::CStringNN json, UInt32 srid, NotNullPtr<Data::ArrayListNN<Math::Geometry::Vector2D>> vecList, Data::ArrayList<UOSInt> *valueOfstList, Data::ArrayListStringNN *nameList, Data::ArrayList<Text::String*> *valueList);
		static Bool ParseGML(Text::CStringNN json, UInt32 srid, Bool swapXY, Text::EncodingFactory *encFact, NotNullPtr<Data::ArrayListNN<Math::Geometry::Vector2D>> vecList, Data::ArrayList<UOSInt> *valueOfstList, Data::ArrayListStringNN *nameList, Data::ArrayList<Text::String*> *valueList);
		static Bool ParseESRI_WMS_XML(Text::CStringNN xml, UInt32 srid, Bool swapXY, Text::EncodingFactory *encFact, NotNullPtr<Data::ArrayListNN<Math::Geometry::Vector2D>> vecList, Data::ArrayList<UOSInt> *valueOfstList, Data::ArrayListStringNN *nameList, Data::ArrayList<Text::String*> *valueList, Math::Coord2DDbl coord);
		static Bool ParseESRIFeatureInfoResponse(NotNullPtr<Text::XMLReader> reader, UInt32 srid, Bool swapXY, NotNullPtr<Data::ArrayListNN<Math::Geometry::Vector2D>> vecList, Data::ArrayList<UOSInt> *valueOfstList, Data::ArrayListStringNN *nameList, Data::ArrayList<Text::String*> *valueList, Math::Coord2DDbl coord);
		static Bool ParseESRIFeatureInfoCollection(NotNullPtr<Text::XMLReader> reader, UInt32 srid, Bool swapXY, NotNullPtr<Data::ArrayListNN<Math::Geometry::Vector2D>> vecList, Data::ArrayList<UOSInt> *valueOfstList, Data::ArrayListStringNN *nameList, Data::ArrayList<Text::String*> *valueList, Math::Coord2DDbl coord);
		static Bool ParseESRIFeatureInfo(NotNullPtr<Text::XMLReader> reader, UInt32 srid, Bool swapXY, NotNullPtr<Data::ArrayListNN<Math::Geometry::Vector2D>> vecList, Data::ArrayList<UOSInt> *valueOfstList, Data::ArrayListStringNN *nameList, Data::ArrayList<Text::String*> *valueList, Math::Coord2DDbl coord);
		static Optional<Math::Geometry::Vector2D> ParseESRIFieldGeometry(NotNullPtr<Text::XMLReader> reader, UInt32 srid, Bool swapXY);
	};
}
#endif
