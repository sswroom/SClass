#ifndef _SM_MAP_MAPENV
#define _SM_MAP_MAPENV
#include "Data/ArrayList.h"
#include "Data/RandomOS.h"
#include "Data/StringUTF8Map.h"
#include "Map/IMapDrawLayer.h"
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

		struct MapItem
		{
			ItemType itemType;
		};

		struct LayerItem : public MapItem
		{
			Map::IMapDrawLayer *layer;
			Bool needRelease;
			Int32 lineType;
			UOSInt lineStyle;
			UOSInt lineThick;
			UInt32 lineColor;
			UInt32 fillStyle;
			Int32 fontType;
			UOSInt fontStyle;
			const UTF8Char *fontName;
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
			const UTF8Char *groupName;
			Bool groupHide;
		};

		typedef struct
		{
			const UTF8Char *fileName;
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

		typedef struct
		{
			Data::ArrayList<LineStyleLayer*> *layers;
			const UTF8Char *name;
		} LineStyle;

		typedef struct
		{
			const UTF8Char *styleName;
			const UTF8Char *fontName;
			Double fontSizePt;
			Bool bold;
			UInt32 fontColor;
			UOSInt buffSize;
			UInt32 buffColor;
		} FontStyle;
		
	private:
		Sync::Mutex *mut;
		UInt32 bgColor;
		Data::ArrayList<MapItem*> *mapLayers;
		Data::ArrayList<LineStyle*> *lineStyles;
		Data::ArrayList<FontStyle*> *fontStyles;
		Data::StringUTF8Map<ImageInfo*> *images;
		Data::ArrayList<ImageInfo*> *imgList;
		Data::RandomOS *random;
		UOSInt nStr;
		UOSInt defLineStyle;
		UOSInt defFontStyle;
		Math::CoordinateSystem *csys;

	private:
		UInt8 GetRandomColor();
		UOSInt GetLayersInList(Data::ArrayList<Map::IMapDrawLayer*> *layers, Data::ArrayList<MapItem*> *list, Map::DrawLayerType lyrType);
		void AddGroupUpdatedHandler(GroupItem *group, Map::IMapDrawLayer::UpdatedHandler hdlr, void *obj);
		void RemoveGroupUpdatedHandler(GroupItem *group, Map::IMapDrawLayer::UpdatedHandler hdlr, void *obj);
	public:
		MapEnv(const UTF8Char *fileName, UInt32 bgColor, Math::CoordinateSystem *csys);
		virtual ~MapEnv();

		virtual IO::ParsedObject::ParserType GetParserType();

		UInt32 GetBGColor();
		UOSInt GetDefLineStyle();
		void SetDefLineStyle(UOSInt lineStyle);
		UOSInt GetDefFontStyle();
		void SetDefFontStyle(UOSInt fontStyle);

		UOSInt AddLineStyle();
		Bool SetLineStyleName(UOSInt index, const UTF8Char *name);
		UTF8Char *GetLineStyleName(UOSInt index, UTF8Char *buff);
		Bool AddLineStyleLayer(UOSInt index, UInt32 color, UOSInt thick, const UInt8 *pattern, UOSInt npattern);
		Bool ChgLineStyleLayer(UOSInt index, UOSInt layerId, UInt32 color, UOSInt thick, const UInt8 *pattern, UOSInt npattern);
		Bool RemoveLineStyleLayer(UOSInt index, UOSInt layerId);
		Bool RemoveLineStyle(UOSInt index);
		UOSInt GetLineStyleCount();
		Bool GetLineStyleLayer(UOSInt index, UOSInt layerId, UInt32 *color, UOSInt *thick, UInt8 **pattern, UOSInt *npattern);
		UOSInt GetLineStyleLayerCnt(UOSInt index);

		//-1 = error
		UOSInt AddFontStyle(const UTF8Char *styleName, const UTF8Char *fontName, Double fontSizePt, Bool bold, UInt32 fontColor, UOSInt buffSize, UInt32 buffColor); //-1 = fail
		Bool SetFontStyleName(UOSInt index, const UTF8Char *name);
		UTF8Char *GetFontStyleName(UOSInt index, UTF8Char *buff);
		Bool RemoveFontStyle(UOSInt index);
		UOSInt GetFontStyleCount();
		Bool GetFontStyle(UOSInt index, const UTF8Char **fontName, Double *fontSizePt, Bool *bold, UInt32 *fontColor, UOSInt *buffSize, UInt32 *buffColor);
		Bool ChgFontStyle(UOSInt index, const UTF8Char *fontName, Double fontSizePt, Bool bold, UInt32 fontColor, UOSInt buffSize, UInt32 buffColor);

		UOSInt AddLayer(GroupItem *group, Map::IMapDrawLayer *layer, Bool needRelease);
		Bool ReplaceLayer(GroupItem *group, UOSInt index, Map::IMapDrawLayer *layer, Bool needRelease);
		GroupItem *AddGroup(GroupItem *group, const UTF8Char *subgroupName);
		void RemoveItem(GroupItem *group, UOSInt index);
		void MoveItem(GroupItem *group, UOSInt fromIndex, UOSInt toIndex);
		void MoveItem(GroupItem *fromGroup, UOSInt fromIndex, GroupItem *toGroup, UOSInt toIndex);
		UOSInt GetItemCount(GroupItem *group);
		MapItem *GetItem(GroupItem *group, UOSInt index);
		const UTF8Char *GetGroupName(GroupItem *group);
		void SetGroupName(GroupItem *group, const UTF8Char *name);
		void SetGroupHide(GroupItem *group, Bool isHide);
		Bool GetGroupHide(GroupItem *group);
		Bool GetLayerProp(LayerItem *setting, GroupItem *group, UOSInt index);
		Bool SetLayerProp(LayerItem *setting, GroupItem *group, UOSInt index);
		UOSInt GetNString();
		void SetNString(UOSInt nStr);
//		void *AddLayerColl(GroupItem *group, Map::MapLayerCollection *layerColl, Bool releaseColl);

		UOSInt GetImageCnt();
		Media::StaticImage *GetImage(UOSInt index, UInt32 *imgDurMS);
		OSInt AddImage(const UTF8Char *fileName, Parser::ParserList *parserList); // -1 = fail
		UOSInt AddImage(const UTF8Char *fileName, Media::ImageList *imgList);
		
		UOSInt GetImageFileCnt();
		Bool GetImageFileInfo(UOSInt index, Map::MapEnv::ImageInfo *info);
		UOSInt GetImageFileIndex(UOSInt index);

		UOSInt GetLayersOfType(Data::ArrayList<Map::IMapDrawLayer *> *layers, Map::DrawLayerType lyrType);
		void AddUpdatedHandler(Map::IMapDrawLayer::UpdatedHandler hdlr, void *obj);
		void RemoveUpdatedHandler(Map::IMapDrawLayer::UpdatedHandler hdlr, void *obj);

		Int64 GetTimeEndTS(GroupItem *group);
		Int64 GetTimeStartTS(GroupItem *group);
		void SetCurrTimeTS(GroupItem *group, Int64 timeStamp);

		Map::IMapDrawLayer *GetFirstLayer(GroupItem *group);
		UOSInt GetLayersInGroup(Map::MapEnv::GroupItem *group, Data::ArrayList<Map::IMapDrawLayer *> *layers);
		Bool GetBoundsDbl(Map::MapEnv::GroupItem *group, Double *minX, Double *minY, Double *maxX, Double *maxY);
		Map::MapView *CreateMapView(UOSInt width, UOSInt height);
		Math::CoordinateSystem *GetCoordinateSystem();
		UInt32 GetSRID();

		void BeginUse(Sync::MutexUsage *mutUsage);
	};
}
#endif
