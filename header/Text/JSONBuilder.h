#ifndef _SM_TEXT_JSONBUILDER
#define _SM_TEXT_JSONBUILDER
#include "Data/ArrayList.h"
#include "Data/ArrayListA.h"
#include "Data/VariItem.h"
#include "Math/Coord2DDbl.h"
#include "Math/Vector3.h"
#include "Math/Geometry/Polygon.h"
#include "Math/Geometry/Polyline.h"
#include "Math/Geometry/Vector2D.h"
#include "Text/JSON.h"
#include "Text/StringBuilderUTF8.h"

namespace Text
{
	class JSONBuilder
	{
	public:
		typedef enum
		{
			OT_OBJECT,
			OT_ARRAY,
			OT_END
		} ObjectType;
	private:
		Text::StringBuilderUTF8 sb;
		Data::ArrayList<ObjectType> objTypes;
		ObjectType currType;
		Bool isFirst;

		void AppendStr(Text::CStringNN val);
		void AppendStrUTF8(UnsafeArray<const UTF8Char> val);
		void AppendStrW(const WChar *val);
		void AppendDouble(Double val);
		void AppendTSStr(Data::Timestamp ts);
		void AppendDateStr(Data::Date dat);
		void AppendCoord2D(Math::Coord2DDbl coord);
		void AppendCoord2DArray(const Math::Coord2DDbl *coordList, UOSInt nPoints);
		void AppendCoordPL(NN<Math::Geometry::Polyline> pl);
		void AppendCoordPG(NN<Math::Geometry::Polygon> pg);
		void AppendGeometry(NN<Math::Geometry::Vector2D> vec);
	public:
		JSONBuilder(ObjectType rootType);
		~JSONBuilder();

		Bool ArrayAddInt32(Int32 val);
		Bool ArrayAddNInt32(NInt32 val);
		Bool ArrayAddInt64(Int64 val);
		Bool ArrayAddFloat64(Double val);
		Bool ArrayAddBool(Bool val);
		Bool ArrayAddStr(Text::PString *val);
		Bool ArrayAddStr(Text::CString val);
		Bool ArrayAddStr(NN<Text::String> val);
		Bool ArrayAddStrOpt(Optional<Text::String> val);
		Bool ArrayAddStrUTF8(UnsafeArrayOpt<const UTF8Char> val);
		Bool ArrayAddNull();
		Bool ArrayAddCoord2D(Math::Coord2DDbl coord);
		Bool ArrayAddVector3(Math::Vector3 vec3);
		Bool ArrayAdd(NN<Text::JSONArray> arr);
		Bool ArrayBeginObject();
		Bool ArrayBeginArray();
		Bool ArrayEnd();

		Bool ObjectAddFloat64(Text::CStringNN name, Double val);
		Bool ObjectAddInt32(Text::CStringNN name, Int32 val);
		Bool ObjectAddNInt32(Text::CStringNN name, NInt32 val);
		Bool ObjectAddInt64(Text::CStringNN name, Int64 val);
		Bool ObjectAddUInt64(Text::CStringNN name, UInt64 val);
		Bool ObjectAddBool(Text::CStringNN name, Bool val);
		Bool ObjectAddStr(Text::CStringNN name, Text::PString *val);
		Bool ObjectAddStr(Text::CStringNN name, NN<const Text::String> val);
		Bool ObjectAddStr(Text::CStringNN name, Text::CString val);
		Bool ObjectAddStrOpt(Text::CStringNN name, Optional<Text::String> val);
		Bool ObjectAddStrUTF8(Text::CStringNN name, UnsafeArrayOpt<const UTF8Char> val);
		Bool ObjectAddStrW(Text::CStringNN name, const WChar *val);
		Bool ObjectAddTSStr(Text::CStringNN name, Data::Timestamp ts);
		Bool ObjectAddDateStr(Text::CStringNN name, Data::Date dat);
		Bool ObjectAddNull(Text::CStringNN name);
		Bool ObjectAddArrayInt32(Text::CStringNN name, Data::ArrayList<Int32> *i32Arr);
		Bool ObjectAddCoord2D(Text::CStringNN name, Math::Coord2DDbl coord);
		Bool ObjectAddArrayCoord2D(Text::CStringNN name, Data::ArrayListA<Math::Coord2DDbl> *coordArr);
		Bool ObjectAddVector3(Text::CStringNN name, Math::Vector3 vec3);
		Bool ObjectAddGeometry(Text::CStringNN name, NN<Math::Geometry::Vector2D> vec);
		Bool ObjectAddGeometryOpt(Text::CStringNN name, Optional<Math::Geometry::Vector2D> vec);
		Bool ObjectAddVarItem(Text::CStringNN name, NN<Data::VariItem> item);
		Bool ObjectAdd(NN<Text::JSONObject> obj);
		Bool ObjectBeginArray(Text::CStringNN name);
		Bool ObjectBeginObject(Text::CStringNN name);
		Bool ObjectEnd();

		Text::CStringNN Build();
	};
}
#endif
