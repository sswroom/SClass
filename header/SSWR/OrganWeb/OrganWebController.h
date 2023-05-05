#ifndef _SM_SSWR_ORGANWEB_ORGANWEBCONTROLLER
#define _SM_SSWR_ORGANWEB_ORGANWEBCONTROLLER
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
			Net::WebServer::MemoryWebSessionManager *sessMgr;
			SSWR::OrganWeb::OrganWebEnv *env;
			UInt32 scnSize;

			static UOSInt GetPreviewSize();
			Net::WebServer::IWebSession *ParseRequestEnv(Net::WebServer::IWebRequest *req, Net::WebServer::IWebResponse *resp, RequestEnv *env, Bool keepSess);

			static void ResponseMstm(Net::WebServer::IWebRequest *req, Net::WebServer::IWebResponse *resp, IO::MemoryStream *mstm, Text::CString contType);

			void WriteHeaderPart1(IO::Writer *writer, const UTF8Char *title, Bool isMobile);
			void WriteHeaderPart2(IO::Writer *writer, WebUserInfo *user, const UTF8Char *onLoadFunc);
			void WriteHeader(IO::Writer *writer, const UTF8Char *title, WebUserInfo *user, Bool isMobile);
			void WriteFooter(IO::Writer *writer);
			void WriteLocator(Sync::RWMutexUsage *mutUsage, IO::Writer *writer, GroupInfo *group, CategoryInfo *cate);
			void WriteLocatorText(Sync::RWMutexUsage *mutUsage, IO::Writer *writer, GroupInfo *group, CategoryInfo *cate);
			void WriteGroupTable(Sync::RWMutexUsage *mutUsage, IO::Writer *writer, const Data::ReadingList<GroupInfo *> *groupList, UInt32 scnWidth, Bool showSelect);
			void WriteSpeciesTable(Sync::RWMutexUsage *mutUsage, IO::Writer *writer, const Data::ArrayList<SpeciesInfo *> *spList, UInt32 scnWidth, Int32 cateId, Bool showSelect, Bool showModify);
			void WritePickObjs(Sync::RWMutexUsage *mutUsage, IO::Writer *writer, RequestEnv *env, const UTF8Char *url, Bool allowMerge);
			void WriteDataFiles(IO::Writer *writer, Data::FastMap<Data::Timestamp, DataFileInfo*> *fileMap, Int64 startTimeTicks, Int64 endTimeTicks);

			static Text::CString LangGetValue(IO::ConfigFile *lang, const UTF8Char *name, UOSInt nameLen);
		public:
			OrganWebController(Net::WebServer::MemoryWebSessionManager *sessMgr, OrganWebEnv *env, UInt32 scnSize);
			virtual ~OrganWebController();
		};
	}
}
#endif
