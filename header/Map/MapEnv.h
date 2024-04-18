#ifndef _SM_MAP_MAPENV
#define _SM_MAP_MAPENV
#include "Data/ArrayListNN.h"
#include "Data/RandomOS.h"
#include "Data/StringMapNN.h"
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
			NotNullPtr<Map::MapDrawLayer> layer;
			Bool needRelease;
			Int32 lineType;
			UOSInt lineStyle;
			Double lineThick;
			UInt32 lineColor;
			UInt32 fillStyle;
			FontType fontType;
			UOSInt fontStyle;
			Text::String *fontName;
			Double fontSizePt;
			UInt32 fontColor;
			Double maxScale;
			Double minScale;
			UOSInt labelCol;
			Int32 flags;
			UOSInt imgIndex;
			Int32 priority;
		};

		struct GroupItem : public MapItem
		{
			Data::ArrayListNN<MapItem> subitems;
			NotNullPtr<Text::String> groupName;
			Bool groupHide;
		};

		typedef struct
		{
			NotNullPtr<Text::String> fileName;
			UOSInt index;
			UOSInt cnt;
			Bool isAni;
			UOSInt aniIndex;
			Int64 aniLastTimeTick;
			Media::ImageList *imgs;
		} ImageInfo;
	private:

		typedef struct
		{
			UInt32 color;
			Double thick;
			UInt8 *pattern;
			UOSInt npattern;
		} LineStyleLayer;

		class LineStyle
		{
		public:
			Data::ArrayListNN<LineStyleLayer> layers;
			Optional<Text::String> name;

			LineStyle() : layers(4) { this->name = 0; }
		};

		typedef struct
		{
			Optional<Text::String> styleName;
			NotNullPtr<Text::String> fontName;
			Double fontSizePt;
			Bool bold;
			UInt32 fontColor;
			UOSInt buffSize;
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
		UOSInt nStr;
		UOSInt defLineStyle;
		UOSInt defFontStyle;
		NotNullPtr<Math::CoordinateSystem> csys;
		Double maxScale;
		Double minScale;

	private:
		UInt8 GetRandomColor();
		UOSInt GetLayersInList(NotNullPtr<Data::ArrayListNN<Map::MapDrawLayer>> layers, NotNullPtr<const Data::ArrayListNN<MapItem>> list, Map::DrawLayerType lyrType) const;
		void AddGroupUpdatedHandler(Optional<GroupItem> group, Map::MapDrawLayer::UpdatedHandler hdlr, AnyType obj);
		void RemoveGroupUpdatedHandler(Optional<GroupItem> group, Map::MapDrawLayer::UpdatedHandler hdlr, AnyType obj);
	public:
		MapEnv(Text::CStringNN fileName, UInt32 bgColor, NotNullPtr<Math::CoordinateSystem> csys);
		virtual ~MapEnv();

		virtual IO::ParserType GetParserType() const;

		UInt32 GetBGColor() const;
		UOSInt GetDefLineStyle() const;
		void SetDefLineStyle(UOSInt lineStyle);
		UOSInt GetDefFontStyle() const;
		void SetDefFontStyle(UOSInt fontStyle);
		Double GetMaxScale() const;
		Double GetMinScale() const;

		UOSInt AddLineStyle();
		Bool SetLineStyleName(UOSInt index, Text::CString name);
		UTF8Char *GetLineStyleName(UOSInt index, UTF8Char *buff) const;
		Bool AddLineStyleLayer(UOSInt index, UInt32 color, Double thick, const UInt8 *pattern, UOSInt npattern);
		Bool ChgLineStyleLayer(UOSInt index, UOSInt layerId, UInt32 color, Double thick, const UInt8 *pattern, UOSInt npattern);
		Bool RemoveLineStyleLayer(UOSInt index, UOSInt layerId);
		Bool RemoveLineStyle(UOSInt index);
		UOSInt GetLineStyleCount() const;
		Bool GetLineStyleLayer(UOSInt index, UOSInt layerId, OutParam<UInt32> color, OutParam<Double> thick, OutParam<UInt8*> pattern, OutParam<UOSInt> npattern) const;
		UOSInt GetLineStyleLayerCnt(UOSInt index) const;

		//-1 = error
		UOSInt AddFontStyle(Text::CStringNN styleName, Text::CString fontName, Double fontSizePt, Bool bold, UInt32 fontColor, UOSInt buffSize, UInt32 buffColor); //-1 = fail
		Bool SetFontStyleName(UOSInt index, Text::CString name);
		UTF8Char *GetFontStyleName(UOSInt index, UTF8Char *buff) const;
		Bool RemoveFontStyle(UOSInt index);
		UOSInt GetFontStyleCount() const;
		Bool GetFontStyle(UOSInt index, OutParam<NotNullPtr<Text::String>> fontName, OutParam<Double> fontSizePt, OutParam<Bool> bold, OutParam<UInt32> fontColor, OutParam<UOSInt> buffSize, OutParam<UInt32> buffColor) const;
		Bool ChgFontStyle(UOSInt index, NotNullPtr<Text::String> fontName, Double fontSizePt, Bool bold, UInt32 fontColor, UOSInt buffSize, UInt32 buffColor);

		UOSInt AddLayer(Optional<GroupItem> group, NotNullPtr<Map::MapDrawLayer> layer, Bool needRelease);
		Bool ReplaceLayer(Optional<GroupItem> group, UOSInt index, NotNullPtr<Map::MapDrawLayer> layer, Bool needRelease);
		NotNullPtr<GroupItem> AddGroup(Optional<GroupItem> group, NotNullPtr<Text::String> subgroupName);
		NotNullPtr<GroupItem> AddGroup(Optional<GroupItem> group, Text::CString subgroupName);
		void RemoveItem(Optional<GroupItem> group, UOSInt index);
		void MoveItem(Optional<GroupItem> group, UOSInt fromIndex, UOSInt toIndex);
		void MoveItem(Optional<GroupItem> fromGroup, UOSInt fromIndex, Optional<GroupItem> toGroup, UOSInt toIndex);
		UOSInt GetItemCount(Optional<GroupItem> group) const;
		Optional<MapItem> GetItem(Optional<GroupItem> group, UOSInt index) const;
		NotNullPtr<Text::String> GetGroupName(NotNullPtr<GroupItem> group) const;
		void SetGroupName(NotNullPtr<GroupItem> group, Text::CString name);
		void SetGroupHide(NotNullPtr<GroupItem> group, Bool isHide);
		Bool GetGroupHide(NotNullPtr<GroupItem> group) const;
		Bool GetLayerProp(NotNullPtr<LayerItem> setting, Optional<GroupItem> group, UOSInt index) const;
		Bool SetLayerProp(NotNullPtr<LayerItem> setting, Optional<GroupItem> group, UOSInt index);
		UOSInt GetNString() const;
		void SetNString(UOSInt nStr);
//		void *AddLayerColl(GroupItem *group, Map::MapLayerCollection *layerColl, Bool releaseColl);

		UOSInt GetImageCnt() const;
		Media::StaticImage *GetImage(UOSInt index, OptOut<UInt32> imgDurMS) const;
		OSInt AddImage(Text::CStringNN fileName, NotNullPtr<Parser::ParserList> parserList); // -1 = fail
		UOSInt AddImage(Text::CStringNN fileName, Media::ImageList *imgList);
		UOSInt AddImageSquare(UInt32 color, UOSInt size);
		
		UOSInt GetImageFileCnt() const;
		Bool GetImageFileInfo(UOSInt index, NotNullPtr<Map::MapEnv::ImageInfo> info) const;
		UOSInt GetImageFileIndex(UOSInt index) const;

		UOSInt GetLayersOfType(NotNullPtr<Data::ArrayListNN<Map::MapDrawLayer>> layers, Map::DrawLayerType lyrType) const;
		void AddUpdatedHandler(Map::MapDrawLayer::UpdatedHandler hdlr, AnyType obj);
		void RemoveUpdatedHandler(Map::MapDrawLayer::UpdatedHandler hdlr, AnyType obj);

		Int64 GetTimeEndTS(Optional<GroupItem> group) const;
		Int64 GetTimeStartTS(Optional<GroupItem> group) const;
		void SetCurrTimeTS(Optional<GroupItem> group, Int64 timeStamp);

		Optional<Map::MapDrawLayer> GetFirstLayer(Optional<GroupItem> group) const;
		UOSInt GetLayersInGroup(Optional<Map::MapEnv::GroupItem> group, NotNullPtr<Data::ArrayListNN<Map::MapDrawLayer>> layers) const;
		Bool GetBounds(Optional<Map::MapEnv::GroupItem> group, OutParam<Math::RectAreaDbl> bounds) const;
		NotNullPtr<Map::MapView> CreateMapView(Math::Size2DDbl scnSize) const;
		NotNullPtr<Math::CoordinateSystem> GetCoordinateSystem() const;
		UInt32 GetSRID() const;

		void BeginUse(NotNullPtr<Sync::MutexUsage> mutUsage) const;
	};
}
#endif
