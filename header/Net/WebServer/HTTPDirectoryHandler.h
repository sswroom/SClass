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
				const UTF8Char *fileName;
				Int64 modTime;
				IO::PackageFile *packageFile;
			} PackageInfo;

			typedef struct
			{
				const UTF8Char *reqPath;
				const UTF8Char *statFileName;
				Data::StringUTF8Map<UInt32> *cntMap;
				Bool updated;
			} StatInfo;
			
		private:
			Text::String *rootDir;
			Bool allowBrowsing;
			Bool allowUpload;
			CacheType ctype;
			Int32 expirePeriod;
			UInt64 fileCacheSize;
			const UTF8Char *allowOrigin;
			Data::BTreeUTF8Map<CacheInfo*> *fileCache;
			Sync::Mutex *fileCacheMut;
			Data::StringUTF8Map<StatInfo*> *statMap;
			Sync::Mutex *statMut;
			Int32 fileCacheUsing;
			Sync::RWMutex *packageMut;
			Data::StringUTF8Map<PackageInfo*> *packageMap;

			void AddCacheHeader(Net::WebServer::IWebResponse *resp);
			Bool MIMEToCompress(const UTF8Char *mime);
			void ResponsePackageFile(Net::WebServer::IWebRequest *req, Net::WebServer::IWebResponse *resp, const UTF8Char *subReq, IO::PackageFile *packageFile);

			void StatLoad(StatInfo *stat);
			void StatSave(StatInfo *stat);
		public:
			HTTPDirectoryHandler(Text::String *rootDir, Bool allowBrowsing, UInt64 fileCacheSize, Bool allowUpload);
			HTTPDirectoryHandler(const UTF8Char *rootDir, Bool allowBrowsing, UInt64 fileCacheSize, Bool allowUpload);

		protected:
			virtual ~HTTPDirectoryHandler();
		public:
			virtual Bool ProcessRequest(Net::WebServer::IWebRequest *req, Net::WebServer::IWebResponse *resp, const UTF8Char *subReq);
			void SetCacheType(CacheType ctype);
			void SetExpirePeriod(Int32 periodSec);
			void SetAllowOrigin(const UTF8Char *origin);
			void ClearFileCache();
			void ExpandPackageFiles(Parser::ParserList *parsers);
			void EnableStats();
			void SaveStats();
		};
	}
}
#endif
