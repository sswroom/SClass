#ifndef _SM_MAP_OSM_OSMDATA
#define _SM_MAP_OSM_OSMDATA
#include "Data/ArrayListStringNN.h"
#include "IO/ParsedObject.h"
#include "Map/MapDrawLayer.h"

namespace Map
{
	namespace OSM
	{
		enum class ElementType
		{
			Node = 0,
			Way = 1,
			Relation = 2
		};

		enum class RoleType
		{
			Unknown = 0,
			Outer,
			Inner,
			Subarea,
			AdminCentre
		};

		struct TagInfo
		{
			NN<Text::String> k;
			NN<Text::String> v;
		};
		
		struct ElementInfo
		{
			Int64 id;
			ElementType type;
			Optional<Data::ArrayListNN<TagInfo>> tags;
		};

		struct NodeInfo : public ElementInfo
		{
			Double lat;
			Double lon;
		};

		struct WayInfo : public ElementInfo
		{
			Data::ArrayListNN<NodeInfo> nodes;
		};

		struct RelationMember
		{
			ElementType type;
			Int64 refId;
		};

		struct RelationInfo : public ElementInfo
		{
			Data::ArrayListNN<RelationMember> members;
		};

		class OSMData : public Map::MapDrawLayer
		{
		private:
			Data::Int64FastMapNN<ElementInfo> elements;

			static void FreeElement(NN<ElementInfo> elem);
		public:
			OSMData(Text::CStringNN sourceName);
			virtual ~OSMData();

			virtual IO::ParserType GetParserType() const;
			NN<NodeInfo> NewNode(Int64 id, Double lat, Double lon);
			NN<WayInfo> NewWay(Int64 id);
			NN<RelationInfo> NewRelation(Int64 id);

			virtual DrawLayerType GetLayerType() const;
			virtual UOSInt GetAllObjectIds(NN<Data::ArrayListInt64> outArr, OptOut<Optional<NameArray>> nameArr);
			virtual UOSInt GetObjectIds(NN<Data::ArrayListInt64> outArr, OptOut<Optional<NameArray>> nameArr, Double mapRate, Math::RectArea<Int32> rect, Bool keepEmpty);
			virtual UOSInt GetObjectIdsMapXY(NN<Data::ArrayListInt64> outArr, OptOut<Optional<NameArray>> nameArr, Math::RectAreaDbl rect, Bool keepEmpty);
			virtual Int64 GetObjectIdMax() const;
			virtual UOSInt GetRecordCnt() const;
			virtual void ReleaseNameArr(Optional<NameArray> nameArr);
			virtual Bool GetString(NN<Text::StringBuilderUTF8> sb, Optional<NameArray> nameArr, Int64 id, UOSInt strIndex);
			virtual UOSInt GetColumnCnt() const;
			virtual UnsafeArrayOpt<UTF8Char> GetColumnName(UnsafeArray<UTF8Char> buff, UOSInt colIndex) const;
			virtual DB::DBUtil::ColType GetColumnType(UOSInt colIndex, OptOut<UOSInt> colSize) const;
			virtual Bool GetColumnDef(UOSInt colIndex, NN<DB::ColDef> colDef) const;
			virtual UInt32 GetCodePage() const;
			virtual Bool GetBounds(OutParam<Math::RectAreaDbl> rect) const;

			virtual NN<GetObjectSess> BeginGetObject();
			virtual void EndGetObject(NN<GetObjectSess> session);
			virtual Optional<Math::Geometry::Vector2D> GetNewVectorById(NN<GetObjectSess> session, Int64 id);

			virtual UOSInt GetGeomCol() const;

			virtual ObjectClass GetObjectClass() const;
		};
	}
}
#endif
