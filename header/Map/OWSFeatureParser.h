#ifndef _SM_MAP_OWSFEATUREPARSER
#define _SM_MAP_OWSFEATUREPARSER
#include "Data/ArrayList.h"
#include "Data/ArrayListNN.h"
#include "Math/Geometry/Vector2D.h"
#include "Text/EncodingFactory.h"
#include "Text/PString.h"
#include "Text/String.h"

namespace Map
{
	class OWSFeatureParser
	{
	public:
		static Bool ParseText(Text::PString txt, UInt32 srid, Math::Coord2DDbl coord, Data::ArrayList<Math::Geometry::Vector2D*> *vecList, Data::ArrayList<UOSInt> *valueOfstList, Data::ArrayListNN<Text::String> *nameList, Data::ArrayList<Text::String*> *valueList);
		static Bool ParseJSON(Text::CStringNN json, UInt32 srid, Data::ArrayList<Math::Geometry::Vector2D*> *vecList, Data::ArrayList<UOSInt> *valueOfstList, Data::ArrayListNN<Text::String> *nameList, Data::ArrayList<Text::String*> *valueList);
		static Bool ParseGML(Text::CStringNN json, UInt32 srid, Bool swapXY, Text::EncodingFactory *encFact, Data::ArrayList<Math::Geometry::Vector2D*> *vecList, Data::ArrayList<UOSInt> *valueOfstList, Data::ArrayListNN<Text::String> *nameList, Data::ArrayList<Text::String*> *valueList);
	};
}
#endif
