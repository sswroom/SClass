#ifndef _SM_MAP_OSM_OSMCACHEHANDLER
#define _SM_MAP_OSM_OSMCACHEHANDLER
#include "Data/ArrayListStrUTF8.h"
#include "Data/SyncLinkedList.h"
#include "IO/FileStream.h"
#include "Net/SocketFactory.h"
#include "Net/SSLEngine.h"
#include "Net/WebServer/WebStandardHandler.h"
#include "Sync/Mutex.h"

namespace Map
{
	namespace OSM
	{
		class OSMCacheHandler : public Net::WebServer::WebStandardHandler
		{
		public:
			typedef struct
			{
				Int32 reqCnt;
				Int32 remoteSuccCnt;
				Int32 remoteErrCnt;
				Int32 localCnt;
				Int32 cacheCnt;
			} CacheStatus;

			typedef struct
			{
				Int32 lev;
				Int32 yTile;
				Int32 xTile;
			} TaskStatus;

			typedef struct
			{
				Int32 stat; //0 = not running, 1 = idle, 2 = to stop, 3 = working
				Sync::Event *evt;
				OSMCacheHandler *me;
			} ThreadStatus;

		private:
			Data::ArrayListStrUTF8 *urls;
			UOSInt urlNext;
			Sync::Mutex *urlMut;

			const UTF8Char *cacheDir;
			Int32 maxLevel;
			Net::SocketFactory *sockf;
			Net::SSLEngine *ssl;
			CacheStatus status;

			OSInt threadCnt;
			ThreadStatus *threadStat;
			Sync::Mutex *taskMut;
			Data::SyncLinkedList *taskList;

			IO::SeekableStream *GetTileData(Int32 lev, Int32 xTile, Int32 yTile);
		public:
			OSMCacheHandler(const UTF8Char *url, const UTF8Char *cacheDir, Int32 maxLevel, Net::SocketFactory *sockf, Net::SSLEngine *ssl);
			virtual ~OSMCacheHandler();

			void AddAlternateURL(const UTF8Char *url);
			void GetStatus(CacheStatus *status);

			virtual Bool ProcessRequest(Net::WebServer::IWebRequest *req, Net::WebServer::IWebResponse *resp, const UTF8Char *subReq);
		};
	}
}
#endif
