#ifndef _SM_SSWR_ORGANWEB_ORGANWEBCONTROLLER
#define _SM_SSWR_ORGANWEB_ORGANWEBCONTROLLER
#include "Data/ArrayListNN.h"
#include "IO/ConfigFile.h"
#include "IO/MemoryStream.h"
#include "IO/Writer.h"
#include "Net/WebServer/MemoryWebSessionManager.h"
#include "Net/WebServer/WebController.h"
#include "SSWR/OrganWeb/OrganWebCommon.h"
#include "Sync/RWMutexUsage.h"

namespace SSWR
{
	namespace OrganWeb
	{
		class OrganWebEnv;

		class OrganWebController : public Net::WebServer::WebController
		{
		protected:
			NN<Net::WebServer::MemoryWebSessionManager> sessMgr;
			NN<SSWR::OrganWeb::OrganWebEnv> env;
			UInt32 scnSize;

			static UOSInt GetPreviewSize();
			static UOSInt GetPerPage(Bool isMobile);
			Optional<Net::WebServer::IWebSession> ParseRequestEnv(NN<Net::WebServer::IWebRequest> req, NN<Net::WebServer::IWebResponse> resp, NN<RequestEnv> env, Bool keepSess);

			static void ResponseMstm(NN<Net::WebServer::IWebRequest> req, NN<Net::WebServer::IWebResponse> resp, NN<IO::MemoryStream> mstm, Text::CStringNN contType);

			void WriteHeaderPart1(NN<IO::Writer> writer, UnsafeArray<const UTF8Char> title, Bool isMobile);
			void WriteHeaderPart2(NN<IO::Writer> writer, Optional<WebUserInfo> user, UnsafeArrayOpt<const UTF8Char> onLoadFunc);
			void WriteHeader(NN<IO::Writer> writer, UnsafeArray<const UTF8Char> title, Optional<WebUserInfo> user, Bool isMobile);
			void WriteFooter(NN<IO::Writer> writer);
			void WriteLocator(NN<Sync::RWMutexUsage> mutUsage, NN<IO::Writer> writer, NN<GroupInfo> group, NN<CategoryInfo> cate);
			void WriteLocatorText(NN<Sync::RWMutexUsage> mutUsage, NN<IO::Writer> writer, NN<GroupInfo> group, NN<CategoryInfo> cate);
			void WriteGroupTable(NN<Sync::RWMutexUsage> mutUsage, NN<IO::Writer> writer, NN<const Data::ReadingList<Optional<GroupInfo>>> groupList, UInt32 scnWidth, Bool showSelect, Bool showAll);
			void WriteSpeciesTable(NN<Sync::RWMutexUsage> mutUsage, NN<IO::Writer> writer, NN<const Data::ReadingList<Optional<SpeciesInfo>>> spList, UInt32 scnWidth, Int32 cateId, Bool showSelect, Bool showModify);
			void WritePickObjs(NN<Sync::RWMutexUsage> mutUsage, NN<IO::Writer> writer, NN<RequestEnv> env, UnsafeArray<const UTF8Char> url, Bool allowMerge);
			void WriteDataFiles(NN<IO::Writer> writer, NN<Data::FastMapNN<Data::Timestamp, DataFileInfo>> fileMap, Int64 startTimeTicks, Int64 endTimeTicks);

			static Text::CStringNN LangGetValue(Optional<IO::ConfigFile> lang, Text::CStringNN name);
		public:
			OrganWebController(NN<Net::WebServer::MemoryWebSessionManager> sessMgr, NN<OrganWebEnv> env, UInt32 scnSize);
			virtual ~OrganWebController();
		};
	}
}
#endif
