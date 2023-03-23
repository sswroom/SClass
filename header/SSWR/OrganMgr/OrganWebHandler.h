#ifndef _SM_SSWR_ORGANMGR_ORGANWEBHANDLER
#define _SM_SSWR_ORGANMGR_ORGANWEBHANDLER
#include "Data/RandomOS.h"
#include "IO/MemoryStream.h"
#include "Media/LRGBLimiter.h"
#include "Media/CS/CSConverter.h"
#include "Media/Resizer/LanczosResizerLR_C32.h"
#include "Net/WebServer/MemoryWebSessionManager.h"
#include "Net/WebServer/WebServiceHandler.h"
#include "SSWR/OrganMgr/OrganWebCommon.h"
#include "Sync/RWMutexUsage.h"

namespace SSWR
{
	namespace OrganMgr
	{
		class OrganWebEnv;

		class OrganWebHandler : public Net::WebServer::WebServiceHandler
		{
		private:
			Net::WebServer::MemoryWebSessionManager *sessMgr;
			SSWR::OrganMgr::OrganWebEnv *env;
			UInt32 scnSize;

			Data::RandomOS random;
			Sync::Mutex resizerMut;
			Media::Resizer::LanczosResizerLR_C32 *resizerLR;
			Sync::Mutex csconvMut;
			Media::CS::CSConverter *csconv;
			UInt32 csconvFCC;
			UInt32 csconvBpp;
			Media::PixelFormat csconvPF;
			Media::ColorProfile csconvColor;
			Media::LRGBLimiter lrgbLimiter;

			Net::WebServer::IWebSession *ParseRequestEnv(Net::WebServer::IWebRequest *req, Net::WebServer::IWebResponse *resp, RequestEnv *env, Bool keepSess);
			static Bool __stdcall SvcPhoto(Net::WebServer::IWebRequest *req, Net::WebServer::IWebResponse *resp, Text::CString subReq, Net::WebServer::WebServiceHandler *parent);
			static Bool __stdcall SvcPhotoDown(Net::WebServer::IWebRequest *req, Net::WebServer::IWebResponse *resp, Text::CString subReq, Net::WebServer::WebServiceHandler *parent);
			static Bool __stdcall SvcGroup(Net::WebServer::IWebRequest *req, Net::WebServer::IWebResponse *resp, Text::CString subReq, Net::WebServer::WebServiceHandler *parent);
			static Bool __stdcall SvcGroupMod(Net::WebServer::IWebRequest *req, Net::WebServer::IWebResponse *resp, Text::CString subReq, Net::WebServer::WebServiceHandler *parent);
			static Bool __stdcall SvcSpecies(Net::WebServer::IWebRequest *req, Net::WebServer::IWebResponse *resp, Text::CString subReq, Net::WebServer::WebServiceHandler *parent);
			static Bool __stdcall SvcSpeciesMod(Net::WebServer::IWebRequest *req, Net::WebServer::IWebResponse *resp, Text::CString subReq, Net::WebServer::WebServiceHandler *parent);
			static Bool __stdcall SvcList(Net::WebServer::IWebRequest *req, Net::WebServer::IWebResponse *resp, Text::CString subReq, Net::WebServer::WebServiceHandler *parent);
			static Bool __stdcall SvcPhotoDetail(Net::WebServer::IWebRequest *req, Net::WebServer::IWebResponse *resp, Text::CString subReq, Net::WebServer::WebServiceHandler *parent);
			static Bool __stdcall SvcPhotoDetailD(Net::WebServer::IWebRequest *req, Net::WebServer::IWebResponse *resp, Text::CString subReq, Net::WebServer::WebServiceHandler *parent);
			static Bool __stdcall SvcPhotoYear(Net::WebServer::IWebRequest *req, Net::WebServer::IWebResponse *resp, Text::CString subReq, Net::WebServer::WebServiceHandler *parent);
			static Bool __stdcall SvcPhotoDay(Net::WebServer::IWebRequest *req, Net::WebServer::IWebResponse *resp, Text::CString subReq, Net::WebServer::WebServiceHandler *parent);
			static Bool __stdcall SvcPhotoUpload(Net::WebServer::IWebRequest *req, Net::WebServer::IWebResponse *resp, Text::CString subReq, Net::WebServer::WebServiceHandler *parent);
			static Bool __stdcall SvcPhotoUpload2(Net::WebServer::IWebRequest *req, Net::WebServer::IWebResponse *resp, Text::CString subReq, Net::WebServer::WebServiceHandler *parent);
			static Bool __stdcall SvcPhotoUploadD(Net::WebServer::IWebRequest *req, Net::WebServer::IWebResponse *resp, Text::CString subReq, Net::WebServer::WebServiceHandler *parent);
			static Bool __stdcall SvcSearchInside(Net::WebServer::IWebRequest *req, Net::WebServer::IWebResponse *resp, Text::CString subReq, Net::WebServer::WebServiceHandler *parent);
			static Bool __stdcall SvcSearchInsideMoreS(Net::WebServer::IWebRequest *req, Net::WebServer::IWebResponse *resp, Text::CString subReq, Net::WebServer::WebServiceHandler *parent);
			static Bool __stdcall SvcSearchInsideMoreG(Net::WebServer::IWebRequest *req, Net::WebServer::IWebResponse *resp, Text::CString subReq, Net::WebServer::WebServiceHandler *parent);
			static Bool __stdcall SvcBookList(Net::WebServer::IWebRequest *req, Net::WebServer::IWebResponse *resp, Text::CString subReq, Net::WebServer::WebServiceHandler *parent);
			static Bool __stdcall SvcBook(Net::WebServer::IWebRequest *req, Net::WebServer::IWebResponse *resp, Text::CString subReq, Net::WebServer::WebServiceHandler *parent);
			static Bool __stdcall SvcBookView(Net::WebServer::IWebRequest *req, Net::WebServer::IWebResponse *resp, Text::CString subReq, Net::WebServer::WebServiceHandler *parent);
			static Bool __stdcall SvcLogin(Net::WebServer::IWebRequest *req, Net::WebServer::IWebResponse *resp, Text::CString subReq, Net::WebServer::WebServiceHandler *parent);
			static Bool __stdcall SvcLogout(Net::WebServer::IWebRequest *req, Net::WebServer::IWebResponse *resp, Text::CString subReq, Net::WebServer::WebServiceHandler *parent);
			static Bool __stdcall SvcReload(Net::WebServer::IWebRequest *req, Net::WebServer::IWebResponse *resp, Text::CString subReq, Net::WebServer::WebServiceHandler *parent);
			static Bool __stdcall SvcRestart(Net::WebServer::IWebRequest *req, Net::WebServer::IWebResponse *resp, Text::CString subReq, Net::WebServer::WebServiceHandler *parent);
			static Bool __stdcall SvcIndex(Net::WebServer::IWebRequest *req, Net::WebServer::IWebResponse *resp, Text::CString subReq, Net::WebServer::WebServiceHandler *parent);
			static Bool __stdcall SvcCate(Net::WebServer::IWebRequest *req, Net::WebServer::IWebResponse *resp, Text::CString subReq, Net::WebServer::WebServiceHandler *parent);
			static Bool __stdcall SvcFavicon(Net::WebServer::IWebRequest *req, Net::WebServer::IWebResponse *resp, Text::CString subReq, Net::WebServer::WebServiceHandler *parent);
			static Bool __stdcall SvcPublicPOI(Net::WebServer::IWebRequest *req, Net::WebServer::IWebResponse *resp, Text::CString subReq, Net::WebServer::WebServiceHandler *parent);
			static Bool __stdcall SvcGroupPOI(Net::WebServer::IWebRequest *req, Net::WebServer::IWebResponse *resp, Text::CString subReq, Net::WebServer::WebServiceHandler *parent);
			static Bool __stdcall SvcSpeciesPOI(Net::WebServer::IWebRequest *req, Net::WebServer::IWebResponse *resp, Text::CString subReq, Net::WebServer::WebServiceHandler *parent);

			void AddGroupPOI(Sync::RWMutexUsage *mutUsage, Text::StringBuilderUTF8 *sb, GroupInfo *group, Int32 userId);
			void AddSpeciesPOI(Sync::RWMutexUsage *mutUsage, Text::StringBuilderUTF8 *sb, SpeciesInfo *species, Int32 userId, Bool publicGroup);

			void ResponsePhoto(Net::WebServer::IWebRequest *req, Net::WebServer::IWebResponse *resp, WebUserInfo *user, Bool isMobile, Int32 speciesId, Int32 cateId, UInt32 imgWidth, UInt32 imgHeight, const UTF8Char *fileName);
			void ResponsePhotoId(Net::WebServer::IWebRequest *req, Net::WebServer::IWebResponse *resp, WebUserInfo *user, Bool isMobile, Int32 speciesId, Int32 cateId, UInt32 imgWidth, UInt32 imgHeight, Int32 photoId);
			void ResponsePhotoWId(Net::WebServer::IWebRequest *req, Net::WebServer::IWebResponse *resp, WebUserInfo *user, Bool isMobile, Int32 speciesId, Int32 cateId, UInt32 imgWidth, UInt32 imgHeight, Int32 photoWId);
			static void ResponseMstm(Net::WebServer::IWebRequest *req, Net::WebServer::IWebResponse *resp, IO::MemoryStream *mstm, Text::CString contType);

			void WriteHeaderPart1(IO::Writer *writer, const UTF8Char *title, Bool isMobile);
			void WriteHeaderPart2(IO::Writer *writer, WebUserInfo *user, const UTF8Char *onLoadFunc);
			void WriteHeader(IO::Writer *writer, const UTF8Char *title, WebUserInfo *user, Bool isMobile);
			void WriteFooter(IO::Writer *writer);
			void WriteLocator(Sync::RWMutexUsage *mutUsage, IO::Writer *writer, GroupInfo *group, CategoryInfo *cate);
			void WriteLocatorText(Sync::RWMutexUsage *mutUsage, IO::Writer *writer, GroupInfo *group, CategoryInfo *cate);
			void WriteGroupTable(Sync::RWMutexUsage *mutUsage, IO::Writer *writer, const Data::ReadingList<GroupInfo *> *groupList, UInt32 scnWidth, Bool showSelect);
			void WriteSpeciesTable(Sync::RWMutexUsage *mutUsage, IO::Writer *writer, const Data::ArrayList<SpeciesInfo *> *spList, UInt32 scnWidth, Int32 cateId, Bool showSelect);
			void WritePickObjs(Sync::RWMutexUsage *mutUsage, IO::Writer *writer, RequestEnv *env, const UTF8Char *url, Bool allowMerge);

			static Bool __stdcall OnSessionDel(Net::WebServer::IWebSession* sess, void *userObj);
			static Bool __stdcall OnSessionCheck(Net::WebServer::IWebSession* sess, void *userObj);

			static Text::CString LangGetValue(IO::ConfigFile *lang, const UTF8Char *name, UOSInt nameLen);
		public:
			OrganWebHandler(OrganWebEnv *env, UInt32 scnSize);
			virtual ~OrganWebHandler();

//			virtual void WebRequest(Net::WebServer::IWebRequest *req, Net::WebServer::IWebResponse *resp);
		};
	}
}
#endif
