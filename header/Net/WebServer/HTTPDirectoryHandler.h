#ifndef _SM_NET_WEBSERVER_HTTPDIRECTORYHANDLER
#define _SM_NET_WEBSERVER_HTTPDIRECTORYHANDLER
#include "Data/BTreeUTF8Map.hpp"
#include "Data/FastStringMapNative.hpp"
#include "Data/StringUTF8Map.hpp"
#include "Data/Timestamp.h"
#include "IO/VirtualPackageFile.h"
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
				NN<Text::String> fileName;
				Data::Timestamp modTime;
				NN<IO::PackageFile> packageFile;
			} PackageInfo;

			typedef struct
			{
				NN<Text::String> reqPath;
				NN<Text::String> statFileName;
				Data::FastStringMapNative<UInt32> *cntMap;
				Bool updated;
			} StatInfo;
			
		protected:
			NN<Text::String> rootDir;
		private:
			Bool allowBrowsing;
			Bool allowUpload;
			CacheType ctype;
			Int32 expirePeriod;
			UInt64 fileCacheSize;
			Data::BTreeUTF8Map<CacheInfo*> fileCache;
			Sync::Mutex fileCacheMut;
			Data::FastStringMapNN<StatInfo> *statMap;
			Sync::Mutex *statMut;
			Int32 fileCacheUsing;
			Optional<Sync::RWMutex> packageMut;
			Optional<Data::FastStringMapNN<PackageInfo>> packageMap;

			void AddCacheHeader(NN<Net::WebServer::WebResponse> resp);
			void ResponsePackageFile(NN<Net::WebServer::WebRequest> req, NN<Net::WebServer::WebResponse> resp, Text::CStringNN subReq, NN<IO::PackageFile> packageFile);
			Bool ResponsePackageFileItem(NN<Net::WebServer::WebRequest> req, NN<Net::WebServer::WebResponse> resp, NN<IO::VirtualPackageFile> packageFile, NN<const IO::PackFileItem> pitem);

			void StatLoad(NN<StatInfo> stat);
			void StatSave(NN<StatInfo> stat);
		public:
			HTTPDirectoryHandler(NN<Text::String> rootDir, Bool allowBrowsing, UInt64 fileCacheSize, Bool allowUpload);
			HTTPDirectoryHandler(Text::CStringNN rootDir, Bool allowBrowsing, UInt64 fileCacheSize, Bool allowUpload);
			virtual ~HTTPDirectoryHandler();

		protected:
			virtual Bool FileValid(Text::CStringNN subReq);
		public:
			virtual Bool ProcessRequest(NN<Net::WebServer::WebRequest> req, NN<Net::WebServer::WebResponse> resp, Text::CStringNN subReq);

			Bool DoFileRequest(NN<Net::WebServer::WebRequest> req, NN<Net::WebServer::WebResponse> resp, Text::CStringNN subReq);
			Bool DoPackageRequest(NN<Net::WebServer::WebRequest> req, NN<Net::WebServer::WebResponse> resp, Text::CStringNN subReq);
			Optional<IO::PackageFile> GetPackageFile(Text::CStringNN path, OutParam<Bool> needRelease);

			void SetRootDir(NN<Text::String> rootDir);
			void SetRootDir(Text::CStringNN rootDir);
			void SetAllowBrowsing(Bool allowBrowsing);
			void SetCacheType(CacheType ctype);
			void SetExpirePeriod(Int32 periodSec);
			void ClearFileCache();
			void ExpandPackageFiles(NN<Parser::ParserList> parsers, Text::CStringNN searchPattern);
			void EnableStats();
			void SaveStats();
			void AddPackage(Text::CStringNN path, NN<IO::PackageFile> pkgFile);
		};
	}
}
#endif
