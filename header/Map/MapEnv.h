#ifndef _SM_MAP_MAPENV
#define _SM_MAP_MAPENV
#include "Data/ArrayList.h"
#include "Data/RandomOS.h"
#include "Data/StringMap.h"
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
			Map::MapDrawLayer *layer;
			Bool needRelease;
			Int32 lineType;
			UOSInt lineStyle;
			UOSInt lineThick;
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
			Data::ArrayList<MapItem*> *subitems;
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
			UOSInt thick;
			UInt8 *pattern;
			UOSInt npattern;
		} LineStyleLayer;

		class LineStyle
		{
		public:
			Data::ArrayList<LineStyleLayer*> layers;
			Text::String *name;

			LineStyle() : layers(4) { this->name = 0; }
		};

		typedef struct
		{
			Text::String *styleName;
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
		Data::ArrayList<MapItem*> mapLayers;
		Data::ArrayList<LineStyle*> lineStyles;
		Data::ArrayList<FontStyle*> fontStyles;
		Data::StringMap<ImageInfo*> images;
		Data::ArrayList<ImageInfo*> imgList;
		Data::RandomOS random;
		UOSInt nStr;
		UOSInt defLineStyle;
		UOSInt defFontStyle;
		Math::CoordinateSystem *csys;

	private:
		UInt8 GetRandomColor();
		UOSInt GetLayersInList(Data::ArrayList<Map::MapDrawLayer*> *layers, const Data::ArrayList<MapItem*> *list, Map::DrawLayerType lyrType) const;
		void AddGroupUpdatedHandler(GroupItem *group, Map::MapDrawLayer::UpdatedHandler hdlr, void *obj);
		void RemoveGroupUpdatedHandler(GroupItem *group, Map::MapDrawLayer::UpdatedHandler hdlr, void *obj);
	public:
		MapEnv(Text::CString fileName, UInt32 bgColor, Math::CoordinateSystem *csys);
		virtual ~MapEnv();

		virtual IO::ParserType GetParserType() const;

		UInt32 GetBGColor() const;
		UOSInt GetDefLineStyle() const;
		void SetDefLineStyle(UOSInt lineStyle);
		UOSInt GetDefFontStyle() const;
		void SetDefFontStyle(UOSInt fontStyle);

		UOSInt AddLineStyle();
		Bool SetLineStyleName(UOSInt index, Text::CString name);
		UTF8Char *GetLineStyleName(UOSInt index, UTF8Char *buff) const;
		Bool AddLineStyleLayer(UOSInt index, UInt32 color, UOSInt thick, const UInt8 *pattern, UOSInt npattern);
		Bool ChgLineStyleLayer(UOSInt index, UOSInt layerId, UInt32 color, UOSInt thick, const UInt8 *pattern, UOSInt npattern);
		Bool RemoveLineStyleLayer(UOSInt index, UOSInt layerId);
		Bool RemoveLineStyle(UOSInt index);
		UOSInt GetLineStyleCount() const;
		Bool GetLineStyleLayer(UOSInt index, UOSInt layerId, UInt32 *color, UOSInt *thick, UInt8 **pattern, UOSInt *npattern) const;
		UOSInt GetLineStyleLayerCnt(UOSInt index) const;

		//-1 = error
		UOSInt AddFontStyle(Text::CString styleName, Text::CString fontName, Double fontSizePt, Bool bold, UInt32 fontColor, UOSInt buffSize, UInt32 buffColor); //-1 = fail
		Bool SetFontStyleName(UOSInt index, Text::CString name);
		UTF8Char *GetFontStyleName(UOSInt index, UTF8Char *buff) const;
		Bool RemoveFontStyle(UOSInt index);
		UOSInt GetFontStyleCount() const;
		Bool GetFontStyle(UOSInt index, Text::String **fontName, Double *fontSizePt, Bool *bold, UInt32 *fontColor, UOSInt *buffSize, UInt32 *buffColor) const;
		Bool ChgFontStyle(UOSInt index, Text::String *fontName, Double fontSizePt, Bool bold, UInt32 fontColor, UOSInt buffSize, UInt32 buffColor);

		UOSInt AddLayer(GroupItem *group, Map::MapDrawLayer *layer, Bool needRelease);
		Bool ReplaceLayer(GroupItem *group, UOSInt index, Map::MapDrawLayer *layer, Bool needRelease);
		GroupItem *AddGroup(GroupItem *group, NotNullPtr<Text::String> subgroupName);
		GroupItem *AddGroup(GroupItem *group, Text::CString subgroupName);
		void RemoveItem(GroupItem *group, UOSInt index);
		void MoveItem(GroupItem *group, UOSInt fromIndex, UOSInt toIndex);
		void MoveItem(GroupItem *fromGroup, UOSInt fromIndex, GroupItem *toGroup, UOSInt toIndex);
		UOSInt GetItemCount(GroupItem *group) const;
		MapItem *GetItem(GroupItem *group, UOSInt index) const;
		NotNullPtr<Text::String> GetGroupName(GroupItem *group) const;
		void SetGroupName(GroupItem *group, Text::CString name);
		void SetGroupHide(GroupItem *group, Bool isHide);
		Bool GetGroupHide(GroupItem *group) const;
		Bool GetLayerProp(LayerItem *setting, GroupItem *group, UOSInt index) const;
		Bool SetLayerProp(LayerItem *setting, GroupItem *group, UOSInt index);
		UOSInt GetNString() const;
		void SetNString(UOSInt nStr);
//		void *AddLayerColl(GroupItem *group, Map::MapLayerCollection *layerColl, Bool releaseColl);

		UOSInt GetImageCnt() const;
		Media::StaticImage *GetImage(UOSInt index, UInt32 *imgDurMS) const;
		OSInt AddImage(Text::CString fileName, Parser::ParserList *parserList); // -1 = fail
		UOSInt AddImage(Text::CString fileName, Media::ImageList *imgList);
		UOSInt AddImageSquare(UInt32 color, UOSInt size);
		
		UOSInt GetImageFileCnt() const;
		Bool GetImageFileInfo(UOSInt index, Map::MapEnv::ImageInfo *info) const;
		UOSInt GetImageFileIndex(UOSInt index) const;

		UOSInt GetLayersOfType(Data::ArrayList<Map::MapDrawLayer *> *layers, Map::DrawLayerType lyrType) const;
		void AddUpdatedHandler(Map::MapDrawLayer::UpdatedHandler hdlr, void *obj);
		void RemoveUpdatedHandler(Map::MapDrawLayer::UpdatedHandler hdlr, void *obj);

		Int64 GetTimeEndTS(GroupItem *group) const;
		Int64 GetTimeStartTS(GroupItem *group) const;
		void SetCurrTimeTS(GroupItem *group, Int64 timeStamp);

		Map::MapDrawLayer *GetFirstLayer(GroupItem *group) const;
		UOSInt GetLayersInGroup(Map::MapEnv::GroupItem *group, Data::ArrayList<Map::MapDrawLayer *> *layers) const;
		Bool GetBounds(Map::MapEnv::GroupItem *group, Math::RectAreaDbl *bounds) const;
		Map::MapView *CreateMapView(Math::Size2DDbl scnSize) const;
		Math::CoordinateSystem *GetCoordinateSystem() const;
		UInt32 GetSRID() const;

		void BeginUse(NotNullPtr<Sync::MutexUsage> mutUsage) const;
	};
}
#endif
