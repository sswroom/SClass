#ifndef _SM_MAP_OSM_OSMCACHEHANDLER
#define _SM_MAP_OSM_OSMCACHEHANDLER
#include "Data/ArrayListStringNN.h"
#include "Data/SyncLinkedList.h"
#include "IO/FileStream.h"
#include "Net/SocketFactory.h"
#include "Net/SSLEngine.h"
#include "Net/WebServer/WebStandardHandler.h"
#include "Sync/Mutex.h"
#include "Sync/MutexUsage.h"

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
			Data::ArrayListStringNN urls;
			UOSInt urlNext;
			Sync::Mutex urlMut;

			NotNullPtr<Text::String> cacheDir;
			Int32 maxLevel;
			NotNullPtr<Net::SocketFactory> sockf;
			Net::SSLEngine *ssl;
			CacheStatus status;
			Sync::Mutex *ioMut;

			IO::SeekableStream *GetTileData(Int32 lev, Int32 xTile, Int32 yTile, Sync::MutexUsage *mutUsage);
		public:
			OSMCacheHandler(Text::CString url, Text::CString cacheDir, Int32 maxLevel, NotNullPtr<Net::SocketFactory> sockf, Net::SSLEngine *ssl);
			virtual ~OSMCacheHandler();

			void AddAlternateURL(Text::CString url);
			void GetStatus(CacheStatus *status);
			void SetIOMut(Sync::Mutex *ioMut);

			virtual Bool ProcessRequest(Net::WebServer::IWebRequest *req, Net::WebServer::IWebResponse *resp, Text::CString subReq);
		};
	}
}
#endif
