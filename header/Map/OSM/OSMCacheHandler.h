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
				NN<Sync::Event> evt;
				OSMCacheHandler *me;
			} ThreadStatus;

		private:
			Data::ArrayListStringNN urls;
			UOSInt urlNext;
			Sync::Mutex urlMut;

			NN<Text::String> cacheDir;
			Int32 maxLevel;
			NN<Net::TCPClientFactory> clif;
			Optional<Net::SSLEngine> ssl;
			CacheStatus status;
			Optional<Sync::Mutex> ioMut;

			Optional<IO::SeekableStream> GetTileData(Int32 lev, Int32 xTile, Int32 yTile, NN<Sync::MutexUsage> mutUsage);
		public:
			OSMCacheHandler(Text::CString url, Text::CStringNN cacheDir, Int32 maxLevel, NN<Net::TCPClientFactory> clif, Optional<Net::SSLEngine> ssl);
			virtual ~OSMCacheHandler();

			void AddAlternateURL(Text::CStringNN url);
			void GetStatus(NN<CacheStatus> status);
			void SetIOMut(Optional<Sync::Mutex> ioMut);

			virtual Bool ProcessRequest(NN<Net::WebServer::WebRequest> req, NN<Net::WebServer::WebResponse> resp, Text::CStringNN subReq);
		};
	}
}
#endif
