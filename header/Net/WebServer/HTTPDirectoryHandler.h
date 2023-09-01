#ifndef _SM_NET_WEBSERVER_HTTPDIRECTORYHANDLER
#define _SM_NET_WEBSERVER_HTTPDIRECTORYHANDLER
#include "Data/BTreeUTF8Map.h"
#include "Data/StringUTF8Map.h"
#include "Data/Timestamp.h"
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
				Data::Timestamp t;
			} CacheInfo;

			typedef struct
			{
				NotNullPtr<Text::String> fileName;
				Data::Timestamp modTime;
				IO::PackageFile *packageFile;
			} PackageInfo;

			typedef struct
			{
				NotNullPtr<Text::String> reqPath;
				NotNullPtr<Text::String> statFileName;
				Data::FastStringMap<UInt32> *cntMap;
				Bool updated;
			} StatInfo;
			
		protected:
			NotNullPtr<Text::String> rootDir;
		private:
			Bool allowBrowsing;
			Bool allowUpload;
			CacheType ctype;
			Int32 expirePeriod;
			UInt64 fileCacheSize;
			Text::String *allowOrigin;
			Data::BTreeUTF8Map<CacheInfo*> fileCache;
			Sync::Mutex fileCacheMut;
			Data::FastStringMap<StatInfo*> *statMap;
			Sync::Mutex *statMut;
			Int32 fileCacheUsing;
			Sync::RWMutex *packageMut;
			Data::FastStringMap<PackageInfo*> *packageMap;

			void AddCacheHeader(NotNullPtr<Net::WebServer::IWebResponse> resp);
			void ResponsePackageFile(NotNullPtr<Net::WebServer::IWebRequest> req, NotNullPtr<Net::WebServer::IWebResponse> resp, Text::CStringNN subReq, IO::PackageFile *packageFile);
			Bool ResponsePackageFileItem(NotNullPtr<Net::WebServer::IWebRequest> req, NotNullPtr<Net::WebServer::IWebResponse> resp, IO::PackageFile *packageFile, const IO::PackFileItem *pitem);

			void StatLoad(StatInfo *stat);
			void StatSave(StatInfo *stat);
		public:
			HTTPDirectoryHandler(NotNullPtr<Text::String> rootDir, Bool allowBrowsing, UInt64 fileCacheSize, Bool allowUpload);
			HTTPDirectoryHandler(Text::CString rootDir, Bool allowBrowsing, UInt64 fileCacheSize, Bool allowUpload);

		protected:
			virtual ~HTTPDirectoryHandler();
		public:
			virtual Bool ProcessRequest(NotNullPtr<Net::WebServer::IWebRequest> req, NotNullPtr<Net::WebServer::IWebResponse> resp, Text::CStringNN subReq);

			Bool DoFileRequest(NotNullPtr<Net::WebServer::IWebRequest> req, NotNullPtr<Net::WebServer::IWebResponse> resp, Text::CStringNN subReq);
			IO::PackageFile *GetPackageFile(Text::CStringNN path, Bool *needRelease);

			void SetRootDir(Text::String *rootDir);
			void SetCacheType(CacheType ctype);
			void SetExpirePeriod(Int32 periodSec);
			void SetAllowOrigin(Text::CString origin);
			void ClearFileCache();
			void ExpandPackageFiles(Parser::ParserList *parsers, Text::CString searchPattern);
			void EnableStats();
			void SaveStats();
		};
	}
}
#endif
