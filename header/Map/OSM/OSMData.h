#ifndef _SM_MAP_OSM_OSMDATA
#define _SM_MAP_OSM_OSMDATA
#include "Data/ArrayListStringNN.h"
#include "Data/Comparator.hpp"
#include "IO/ParsedObject.h"
#include "Map/MapDrawLayer.h"

namespace Map
{
	namespace OSM
	{
		enum class ElementType
		{
			Unknown = 0,
			Node = 1,
			Way = 2,
			Relation = 3
		};

		enum class RoleType
		{
			Unknown = 0,
			Outer,
			Inner,
			Subarea,
			AdminCentre
		};

		enum class LayerType
		{
			Unknown = 0,
			Aeroway,
			Amenity,
			AmenityCafe,
			AmenityFastFood,
			AmenityKindergarten,
			AmenityMarketplace,
			AmenityPolice,
			AmenityPostBox,
			AmenityRestaurant,
			AmenityTaxi,
			AmenityTelephone,
			Boundary,
			Building,
			Golf,
			GolfBunker,
			GolfFairway,
			GolfHole,
			GolfRough,
			Highway,
			HighwayBusStop,
			HighwayConstruction,
			HighwayCorridor,
			HighwayCrossing,
			HighwayCycleway,
			HighwayElevator,
			HighwayEmergencyAccessPoint,
			HighwayFootway,
			HighwayGiveWay,
			HighwayMilestone,
			HighwayMotorway,
			HighwayMotorwayLink,
			HighwayPath,
			HighwayPedestrian,
			HighwayPlatform,
			HighwayPrimary,
			HighwayPrimaryLink,
			HighwayResidential,
			HighwayRestArea,
			HighwaySecondary,
			HighwaySecondaryLink,
			HighwayService,
			HighwaySpeedCamera,
			HighwaySteps,
			HighwayStreetLamp,
			HighwayTertiary,
			HighwayTrafficSignals,
			HighwayTrunk,
			HighwayTrunkLink,
			HighwayUnclassified,
			Landuse,
			LanduseIndustrial,
			LanduseResidential,
			Leisure,
			LeisurePark,
			LeisurePitch,
			Natural,
			NaturalBay,
			NaturalCape,
			NaturalGrassland,
			NaturalPeak,
			NaturalRidge,
			NaturalRock,
			NaturalSaddle,
			NaturalScrub,
			NaturalValley,
			NaturalWood,
			Place,
			PlaceHamlet,
			PlaceLocality,
			Power,
			PowerLine,
			PowerSubstation,
			Railway,
			Road,
			Transport,
			Waterway,
			WaterwayStream,

			Count
		};

		struct LayerSpec
		{
			Double minScale;
			Double maxScale;
			Bool hide;
			Bool isArea;
		};

		struct TagInfo
		{
			NN<Text::String> k;
			NN<Text::String> v;
		};
		
		struct ElementInfo
		{
			virtual ~ElementInfo() {}

			Int64 id;
			ElementType type;
			Bool hasParent;
			LayerType layerType;
			Optional<Data::ArrayListNN<TagInfo>> tags;
		};

		struct NodeInfo : public ElementInfo
		{
			virtual ~NodeInfo() {}

			Double lat;
			Double lon;
		};

		struct WayInfo : public ElementInfo
		{
			virtual ~WayInfo() {}
			
			Math::RectAreaDbl bounds;
			Data::ArrayListNN<NodeInfo> nodes;
		};

		struct RelationMember
		{
			ElementType type;
			Int64 refId;
			Optional<Text::String> role;
		};

		struct RelationInfo : public ElementInfo
		{
			virtual ~RelationInfo() {}

			Data::ArrayListNN<RelationMember> members;
		};

		struct ElementGroup
		{
			Int32 x;
			Int32 y;
			Data::ArrayListNN<ElementInfo> elements;
		};

		class ElementComparator : public Data::Comparator<NN<ElementInfo>>
		{
		public:
			virtual IntOS Compare(NN<ElementInfo> a, NN<ElementInfo> b) const;
		};

		class OSMData : public Map::MapDrawLayer
		{
		private:
			Bool nodeSorted;
			Bool waySorted;
			Bool relationSorted;
			Bool showUnknown;
			Data::ArrayListNN<ElementInfo> elements;
			Double groupDist;
			Data::Int64FastMapNN<ElementGroup> eleGroups;
			Optional<Text::String> lang;
			Math::RectAreaDbl bounds;
			Math::RectAreaDbl dataBounds;
			Data::Timestamp osmBaseTime;
			Optional<Text::String> note;
			Double currScale;
			MixedData mixedData;
			LayerSpec layerSpecs[(UIntOS)LayerType::Count];

			static void __stdcall FreeRelationMember(NN<RelationMember> member);
			static void __stdcall FreeElement(NN<ElementInfo> elem);
			void SortElements();
			LayerType CalcElementLayerType(NN<ElementInfo> elem) const;
			void Init();
		public:
			OSMData(Text::CStringNN sourceName);
			OSMData(NN<Text::String> sourceName);
			virtual ~OSMData();

			virtual void SetCurrScale(Double scale);
			NN<NodeInfo> NewNode(Int64 id, Double lat, Double lon);
			NN<WayInfo> NewWay(Int64 id);
			NN<RelationInfo> NewRelation(Int64 id);
			Optional<ElementInfo> GetElementById(Int64 id, ElementType type);
			Optional<NodeInfo> GetNodeById(Int64 id);
			Optional<NodeInfo> GetNodeByPos(Double lat, Double lon);
			Optional<WayInfo> GetWayById(Int64 id);
			Optional<RelationInfo> GetRelationById(Int64 id);
			void WayAddNode(NN<WayInfo> way, NN<NodeInfo> node);
			void ElementAddTag(NN<ElementInfo> elem, NN<Text::String> k, NN<Text::String> v);
			void RelationAddMember(NN<RelationInfo> rel, ElementType type, Int64 refId, Optional<Text::String> role);
			void SetLang(Text::CString lang);
			void SetDataBounds(Math::RectAreaDbl bounds);
			Math::RectAreaDbl GetDataBounds() const;
			void SetGroupDist(Double groupDist);
			void SetOSMBase(Data::Timestamp baseTime);
			void SetNote(Text::CStringNN note);
			void SetShowUnknown(Bool showUnknown);
			Bool IsShowUnknown() const;
			void BuildIndex();
			void SetStyleDefault();
			void SetStyleCenterline();
			NN<Math::Geometry::Vector2D> CreateVector(NN<ElementInfo> elem);
			UIntOS GetRelations(NN<Data::ArrayListNN<RelationInfo>> outArr) const;

			virtual DrawLayerType GetLayerType() const;
			virtual void SetMixedData(MixedData MixedData);
			virtual UIntOS GetAllObjectIds(NN<Data::ArrayListInt64> outArr, OptOut<Optional<NameArray>> nameArr);
			virtual UIntOS GetObjectIds(NN<Data::ArrayListInt64> outArr, OptOut<Optional<NameArray>> nameArr, Double mapRate, Math::RectArea<Int32> rect, Bool keepEmpty);
			virtual UIntOS GetObjectIdsMapXY(NN<Data::ArrayListInt64> outArr, OptOut<Optional<NameArray>> nameArr, Math::RectAreaDbl rect, Bool keepEmpty);
			virtual Int64 GetObjectIdMax() const;
			virtual UIntOS GetRecordCnt() const;
			virtual void ReleaseNameArr(Optional<NameArray> nameArr);
			virtual Bool GetString(NN<Text::StringBuilderUTF8> sb, Optional<NameArray> nameArr, Int64 id, UIntOS strIndex);
			virtual UIntOS GetColumnCnt() const;
			virtual UnsafeArrayOpt<UTF8Char> GetColumnName(UnsafeArray<UTF8Char> buff, UIntOS colIndex) const;
			virtual DB::DBUtil::ColType GetColumnType(UIntOS colIndex, OptOut<UIntOS> colSize) const;
			virtual Bool GetColumnDef(UIntOS colIndex, NN<DB::ColDef> colDef) const;
			virtual UInt32 GetCodePage() const;
			virtual Bool GetBounds(OutParam<Math::RectAreaDbl> rect) const;

			virtual NN<GetObjectSess> BeginGetObject();
			virtual void EndGetObject(NN<GetObjectSess> session);
			virtual Optional<Math::Geometry::Vector2D> GetNewVectorById(NN<GetObjectSess> session, Int64 id);

			virtual UIntOS GetGeomCol() const;

			virtual ObjectClass GetObjectClass() const;
		};
		Text::CStringNN ElementTypeGetName(ElementType type);
		ElementType ElementTypeFromString(Text::CStringNN str);
		Text::CStringNN LayerTypeGetName(LayerType layerType);
	}
}
#endif
