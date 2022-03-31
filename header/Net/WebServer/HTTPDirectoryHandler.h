#ifndef _SM_NET_WEBSERVER_HTTPDIRECTORYHANDLER
#define _SM_NET_WEBSERVER_HTTPDIRECTORYHANDLER
#include "Data/BTreeUTF8Map.h"
#include "Data/StringUTF8Map.h"
#include "IO/PackageFile.h"
#include "Parser/ParserList.h"
#include "Net/WebServer/WebStandardHandler.h"
#include "Sync/Mutex.h"
#include "Sync/RWMutex.h"

namespace Net
{
	namespace WebServer
	{
		class HTTPDirectoryHandler : public Net::WebServer::WebStandardHandler
		{
		public:
			typedef enum
			{
				CT_DEFAULT,
				CT_PUBLIC,
				CT_PRIVATE,
				CT_NO_CACHE
			} CacheType;

			typedef struct
			{
				UInt8 *buff;
				UOSInt buffSize;
				Int64 t;
			} CacheInfo;

			typedef struct
			{
				Text::String *fileName;
				Int64 modTime;
				IO::PackageFile *packageFile;
			} PackageInfo;

			typedef struct
			{
				Text::String *reqPath;
				Text::String *statFileName;
				Data::FastStringMap<UInt32> *cntMap;
				Bool updated;
			} StatInfo;
			
		private:
			Text::String *rootDir;
			Bool allowBrowsing;
			Bool allowUpload;
			CacheType ctype;
			Int32 expirePeriod;
			UInt64 fileCacheSize;
			Text::String *allowOrigin;
			Data::BTreeUTF8Map<CacheInfo*> *fileCache;
			Sync::Mutex *fileCacheMut;
			Data::FastStringMap<StatInfo*> *statMap;
			Sync::Mutex *statMut;
			Int32 fileCacheUsing;
			Sync::RWMutex *packageMut;
			Data::FastStringMap<PackageInfo*> *packageMap;

			void AddCacheHeader(Net::WebServer::IWebResponse *resp);
			void ResponsePackageFile(Net::WebServer::IWebRequest *req, Net::WebServer::IWebResponse *resp, Text::CString subReq, IO::PackageFile *packageFile);

			void StatLoad(StatInfo *stat);
			void StatSave(StatInfo *stat);
		public:
			HTTPDirectoryHandler(Text::String *rootDir, Bool allowBrowsing, UInt64 fileCacheSize, Bool allowUpload);
			HTTPDirectoryHandler(Text::CString rootDir, Bool allowBrowsing, UInt64 fileCacheSize, Bool allowUpload);

		protected:
			virtual ~HTTPDirectoryHandler();
		public:
			virtual Bool ProcessRequest(Net::WebServer::IWebRequest *req, Net::WebServer::IWebResponse *resp, Text::CString subReq);
			void SetCacheType(CacheType ctype);
			void SetExpirePeriod(Int32 periodSec);
			void SetAllowOrigin(Text::CString origin);
			void ClearFileCache();
			void ExpandPackageFiles(Parser::ParserList *parsers);
			void EnableStats();
			void SaveStats();
		};
	}
}
#endif
