#ifndef _SM_MAP_WEBIMAGELAYER
#define _SM_MAP_WEBIMAGELAYER
#include "Data/ArrayList.h"
#include "Data/IComparable.h"
#include "Map/IMapDrawLayer.h"
#include "Media/SharedImage.h"
#include "Net/WebBrowser.h"
#include "Parser/ParserList.h"
#include "Sync/Mutex.h"
#include "Sync/RWMutex.h"

namespace Map
{
	class WebImageLayer : public Map::IMapDrawLayer
	{
	private:
		class ImageStat : public Data::IComparable
		{
		public:
			Int32 id;
			const UTF8Char *url;
			Media::SharedImage *simg;
			IO::IStreamData *data;
			const UTF8Char *name;
			Int64 timeStart;
			Int64 timeEnd;
			Int32 zIndex;
			Double x1;
			Double y1;
			Double x2;
			Double y2;
			Double sizeX;
			Double sizeY;
			Bool isScreen;
			Double alpha;
			Bool hasAltitude;
			Double altitude;

		public:
			virtual ~ImageStat();
			virtual OSInt CompareTo(Data::IComparable *obj);
		};
	private:
		Net::WebBrowser *browser;
		Parser::ParserList *parsers;
		Data::ArrayList<ImageStat *> *pendingList;
		Data::ArrayList<ImageStat *> *loadingList;
		Data::ArrayList<ImageStat *> *loadedList;
		Sync::Mutex *loadingMut;
		Sync::RWMutex *loadedMut;
		Sync::Event *loadEvt;
		Int32 nextId;
		Bool boundsExists;
		Double minX;
		Double minY;
		Double maxX;
		Double maxY;
		Int64 minTime;
		Int64 maxTime;
		Int64 currTime;
		Sync::Mutex *updMut;
		Data::ArrayList<UpdatedHandler> *updHdlrs;
		Data::ArrayList<void *> *updObjs;

		Bool threadRunning;
		Bool threadToStop;

		OSInt GetImageStatIndex(Int32 id);
		ImageStat *GetImageStat(Int32 id);

		void LoadImage(ImageStat *stat);
		static UInt32 __stdcall LoadThread(void *userObj);
	public:
		WebImageLayer(Net::WebBrowser *browser, Parser::ParserList *parsers, const UTF8Char *sourceName, Math::CoordinateSystem *csys, const UTF8Char *layerName);
		virtual ~WebImageLayer();

		virtual void SetCurrTimeTS(Int64 timeStamp);
		virtual Int64 GetTimeStartTS();
		virtual Int64 GetTimeEndTS();

		virtual DrawLayerType GetLayerType();
		virtual UOSInt GetAllObjectIds(Data::ArrayListInt64 *outArr, void **nameArr);
		virtual UOSInt GetObjectIds(Data::ArrayListInt64 *outArr, void **nameArr, Double mapRate, Int32 x1, Int32 y1, Int32 x2, Int32 y2, Bool keepEmpty);
		virtual UOSInt GetObjectIdsMapXY(Data::ArrayListInt64 *outArr, void **nameArr, Double x1, Double y1, Double x2, Double y2, Bool keepEmpty);
		virtual Int64 GetObjectIdMax();
		virtual void ReleaseNameArr(void *nameArr);
		virtual UTF8Char *GetString(UTF8Char *buff, UOSInt buffSize, void *nameArr, Int64 id, UOSInt colIndex);
		virtual UOSInt GetColumnCnt();
		virtual UTF8Char *GetColumnName(UTF8Char *buff, UOSInt colIndex);
		virtual DB::DBUtil::ColType GetColumnType(UOSInt colIndex, UOSInt *colSize);
		virtual Bool GetColumnDef(UOSInt colIndex, DB::ColDef *colDef);
		virtual UInt32 GetCodePage();
		virtual Bool GetBoundsDbl(Double *minX, Double *minY, Double *maxX, Double *maxY);

		virtual void *BeginGetObject();
		virtual void EndGetObject(void *session);
		virtual DrawObjectL *GetObjectByIdD(void *session, Int64 id);
		virtual Math::Vector2D *GetVectorById(void *session, Int64 id);
		virtual void ReleaseObject(void *session, DrawObjectL *obj);

		virtual ObjectClass GetObjectClass();

		virtual void AddUpdatedHandler(UpdatedHandler hdlr, void *obj);
		virtual void RemoveUpdatedHandler(UpdatedHandler hdlr, void *obj);

		void AddImage(const UTF8Char *name, const UTF8Char *url, Int32 zIndex, Double x1, Double y1, Double x2, Double y2, Double sizeX, Double sizeY, Bool isScreen, Int64 timeStart, Int64 timeEnd, Double alpha, Bool hasAltitude, Double altitude);
	};
}
#endif
