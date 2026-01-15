#ifndef _SM_MAP_MAPENV
#define _SM_MAP_MAPENV
#include "Data/ArrayListNN.hpp"
#include "Data/RandomOS.h"
#include "Data/StringMapNN.hpp"
#include "Map/MapDrawLayer.h"
#include "Map/MapLayerCollection.h"
#include "Map/MapView.h"
#include "Media/ImageList.h"
#include "Media/StaticImage.h"
#include "Parser/ParserList.h"
#include "Sync/Mutex.h"
#include "Sync/MutexUsage.h"

namespace Map
{
	class MapEnv : public IO::ParsedObject
	{
	public:
		typedef enum
		{
			IT_UNKNOWN,
			IT_LAYER,
			IT_GROUP
		} ItemType;
		
		typedef enum
		{
			SFLG_NONE = 0,
			SFLG_SHOWLABEL = 1,
			SFLG_SMART = 2,
			SFLG_ALIGN = 4,
			SFLG_ROTATE = 8,
			SFLG_TRIM = 16,
			SFLG_CAPITAL = 32,
			SFLG_HIDESHAPE = 64
		} SettingFlag;

		enum class FontType
		{
			GlobalStyle,
			LayerStyle
		};

		struct MapItem
		{
			ItemType itemType;
		};

		struct LayerItem : public MapItem
		{
			NN<Map::MapDrawLayer> layer;
			Bool needRelease;
			Int32 lineType;
			UIntOS lineStyle;
			Double lineThick;
			UInt32 lineColor;
			UInt32 fillStyle;
			FontType fontType;
			UIntOS fontStyle;
			Optional<Text::String> fontName;
			Double fontSizePt;
			UInt32 fontColor;
			Double maxScale;
			Double minScale;
			UIntOS labelCol;
			Int32 flags;
			UIntOS imgIndex;
			Int32 priority;
		};

		struct GroupItem : public MapItem
		{
			Data::ArrayListNN<MapItem> subitems;
			NN<Text::String> groupName;
			Bool groupHide;
		};

		typedef struct
		{
			NN<Text::String> fileName;
			UIntOS index;
			UIntOS cnt;
			Bool isAni;
			UIntOS aniIndex;
			Int64 aniLastTimeTick;
			NN<Media::ImageList> imgs;
		} ImageInfo;
	private:

		typedef struct
		{
			UInt32 color;
			Double thick;
			UnsafeArrayOpt<UInt8> pattern;
			UIntOS npattern;
		} LineStyleLayer;

		class LineStyle
		{
		public:
			Data::ArrayListNN<LineStyleLayer> layers;
			Optional<Text::String> name;

			LineStyle() : layers(4) { this->name = nullptr; }
		};

		typedef struct
		{
			Optional<Text::String> styleName;
			NN<Text::String> fontName;
			Double fontSizePt;
			Bool bold;
			UInt32 fontColor;
			UIntOS buffSize;
			UInt32 buffColor;
		} FontStyle;
		
	private:
		Sync::Mutex mut;
		UInt32 bgColor;
		Data::ArrayListNN<MapItem> mapLayers;
		Data::ArrayListNN<LineStyle> lineStyles;
		Data::ArrayListNN<FontStyle> fontStyles;
		Data::StringMapNN<ImageInfo> images;
		Data::ArrayListNN<ImageInfo> imgList;
		Data::RandomOS random;
		UIntOS nStr;
		UIntOS defLineStyle;
		UIntOS defFontStyle;
		NN<Math::CoordinateSystem> csys;
		Double maxScale;
		Double minScale;

	private:
		UInt8 GetRandomColor();
		UIntOS GetLayersInList(NN<Data::ArrayListNN<Map::MapDrawLayer>> layers, NN<const Data::ArrayListNN<MapItem>> list, Map::DrawLayerType lyrType) const;
		void AddGroupUpdatedHandler(Optional<GroupItem> group, Map::MapDrawLayer::UpdatedHandler hdlr, AnyType obj);
		void RemoveGroupUpdatedHandler(Optional<GroupItem> group, Map::MapDrawLayer::UpdatedHandler hdlr, AnyType obj);
	public:
		MapEnv(Text::CStringNN fileName, UInt32 bgColor, NN<Math::CoordinateSystem> csys);
		virtual ~MapEnv();

		virtual IO::ParserType GetParserType() const;

		UInt32 GetBGColor() const;
		UIntOS GetDefLineStyle() const;
		void SetDefLineStyle(UIntOS lineStyle);
		UIntOS GetDefFontStyle() const;
		void SetDefFontStyle(UIntOS fontStyle);
		Double GetMaxScale() const;
		Double GetMinScale() const;

		UIntOS AddLineStyle();
		Bool SetLineStyleName(UIntOS index, Text::CString name);
		UnsafeArrayOpt<UTF8Char> GetLineStyleName(UIntOS index, UnsafeArray<UTF8Char> buff) const;
		Bool AddLineStyleLayer(UIntOS index, UInt32 color, Double thick, UnsafeArrayOpt<const UInt8> pattern, UIntOS npattern);
		Bool ChgLineStyleLayer(UIntOS index, UIntOS layerId, UInt32 color, Double thick, UnsafeArrayOpt<const UInt8> pattern, UIntOS npattern);
		Bool RemoveLineStyleLayer(UIntOS index, UIntOS layerId);
		Bool RemoveLineStyle(UIntOS index);
		UIntOS GetLineStyleCount() const;
		Bool GetLineStyleLayer(UIntOS index, UIntOS layerId, OutParam<UInt32> color, OutParam<Double> thick, OutParam<UnsafeArrayOpt<UInt8>> pattern, OutParam<UIntOS> npattern) const;
		UIntOS GetLineStyleLayerCnt(UIntOS index) const;

		//-1 = error
		UIntOS AddFontStyle(Text::CStringNN styleName, Text::CStringNN fontName, Double fontSizePt, Bool bold, UInt32 fontColor, UIntOS buffSize, UInt32 buffColor); //-1 = fail
		Bool SetFontStyleName(UIntOS index, Text::CString name);
		UnsafeArrayOpt<UTF8Char> GetFontStyleName(UIntOS index, UnsafeArray<UTF8Char> buff) const;
		Bool RemoveFontStyle(UIntOS index);
		UIntOS GetFontStyleCount() const;
		Bool GetFontStyle(UIntOS index, OutParam<NN<Text::String>> fontName, OutParam<Double> fontSizePt, OutParam<Bool> bold, OutParam<UInt32> fontColor, OutParam<UIntOS> buffSize, OutParam<UInt32> buffColor) const;
		Bool ChgFontStyle(UIntOS index, NN<Text::String> fontName, Double fontSizePt, Bool bold, UInt32 fontColor, UIntOS buffSize, UInt32 buffColor);

		UIntOS AddLayer(Optional<GroupItem> group, NN<Map::MapDrawLayer> layer, Bool needRelease);
		Bool ReplaceLayer(Optional<GroupItem> group, UIntOS index, NN<Map::MapDrawLayer> layer, Bool needRelease);
		NN<GroupItem> AddGroup(Optional<GroupItem> group, NN<Text::String> subgroupName);
		NN<GroupItem> AddGroup(Optional<GroupItem> group, Text::CStringNN subgroupName);
		void RemoveItem(Optional<GroupItem> group, UIntOS index);
		void MoveItem(Optional<GroupItem> group, UIntOS fromIndex, UIntOS toIndex);
		void MoveItem(Optional<GroupItem> fromGroup, UIntOS fromIndex, Optional<GroupItem> toGroup, UIntOS toIndex);
		UIntOS GetItemCount(Optional<GroupItem> group) const;
		Optional<MapItem> GetItem(Optional<GroupItem> group, UIntOS index) const;
		NN<Text::String> GetGroupName(NN<GroupItem> group) const;
		void SetGroupName(NN<GroupItem> group, Text::CStringNN name);
		void SetGroupHide(NN<GroupItem> group, Bool isHide);
		Bool GetGroupHide(NN<GroupItem> group) const;
		Bool GetLayerProp(NN<LayerItem> setting, Optional<GroupItem> group, UIntOS index) const;
		Bool SetLayerProp(NN<LayerItem> setting, Optional<GroupItem> group, UIntOS index);
		UIntOS GetNString() const;
		void SetNString(UIntOS nStr);
//		void *AddLayerColl(GroupItem *group, Map::MapLayerCollection *layerColl, Bool releaseColl);

		UIntOS GetImageCnt() const;
		Optional<Media::StaticImage> GetImage(UIntOS index, OptOut<UInt32> imgDurMS) const;
		IntOS AddImage(Text::CStringNN fileName, NN<Parser::ParserList> parserList); // -1 = fail
		UIntOS AddImage(Text::CStringNN fileName, NN<Media::ImageList> imgList);
		UIntOS AddImageSquare(UInt32 color, UIntOS size);
		
		UIntOS GetImageFileCnt() const;
		Bool GetImageFileInfo(UIntOS index, NN<Map::MapEnv::ImageInfo> info) const;
		UIntOS GetImageFileIndex(UIntOS index) const;

		UIntOS GetLayersOfType(NN<Data::ArrayListNN<Map::MapDrawLayer>> layers, Map::DrawLayerType lyrType) const;
		void AddUpdatedHandler(Map::MapDrawLayer::UpdatedHandler hdlr, AnyType obj);
		void RemoveUpdatedHandler(Map::MapDrawLayer::UpdatedHandler hdlr, AnyType obj);

		Int64 GetTimeEndTS(Optional<GroupItem> group) const;
		Int64 GetTimeStartTS(Optional<GroupItem> group) const;
		void SetCurrTimeTS(Optional<GroupItem> group, Int64 timeStamp);

		Optional<Map::MapDrawLayer> GetFirstLayer(Optional<GroupItem> group) const;
		UIntOS GetLayersInGroup(Optional<Map::MapEnv::GroupItem> group, NN<Data::ArrayListNN<Map::MapDrawLayer>> layers) const;
		Bool GetBounds(Optional<Map::MapEnv::GroupItem> group, OutParam<Math::RectAreaDbl> bounds) const;
		Bool GetLayerBounds(Optional<Map::MapEnv::GroupItem> group, UIntOS index, OutParam<Math::RectAreaDbl> bounds) const;
		NN<Map::MapView> CreateMapView(Math::Size2DDbl scnSize) const;
		NN<Math::CoordinateSystem> GetCoordinateSystem() const;
		UInt32 GetSRID() const;

		void BeginUse(NN<Sync::MutexUsage> mutUsage) const;
	};
}
#endif
