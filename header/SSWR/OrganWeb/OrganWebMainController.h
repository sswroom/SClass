#ifndef _SM_SSWR_ORGANWEB_ORGANWEBMAINCONTROLLER
#define _SM_SSWR_ORGANWEB_ORGANWEBMAINCONTROLLER
#include "SSWR/OrganWeb/OrganWebController.h"

namespace SSWR
{
	namespace OrganWeb
	{
		class OrganWebMainController : public OrganWebController
		{
		private:
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

			static Bool __stdcall SvcPhoto(Net::WebServer::IWebRequest *req, Net::WebServer::IWebResponse *resp, Text::CString subReq, Net::WebServer::WebController *parent);
			static Bool __stdcall SvcPhotoDown(Net::WebServer::IWebRequest *req, Net::WebServer::IWebResponse *resp, Text::CString subReq, Net::WebServer::WebController *parent);
			static Bool __stdcall SvcGroup(Net::WebServer::IWebRequest *req, Net::WebServer::IWebResponse *resp, Text::CString subReq, Net::WebServer::WebController *parent);
			static Bool __stdcall SvcGroupMod(Net::WebServer::IWebRequest *req, Net::WebServer::IWebResponse *resp, Text::CString subReq, Net::WebServer::WebController *parent);
			static Bool __stdcall SvcSpecies(Net::WebServer::IWebRequest *req, Net::WebServer::IWebResponse *resp, Text::CString subReq, Net::WebServer::WebController *parent);
			static Bool __stdcall SvcSpeciesMod(Net::WebServer::IWebRequest *req, Net::WebServer::IWebResponse *resp, Text::CString subReq, Net::WebServer::WebController *parent);
			static Bool __stdcall SvcList(Net::WebServer::IWebRequest *req, Net::WebServer::IWebResponse *resp, Text::CString subReq, Net::WebServer::WebController *parent);
			static Bool __stdcall SvcPhotoDetail(Net::WebServer::IWebRequest *req, Net::WebServer::IWebResponse *resp, Text::CString subReq, Net::WebServer::WebController *parent);
			static Bool __stdcall SvcPhotoDetailD(Net::WebServer::IWebRequest *req, Net::WebServer::IWebResponse *resp, Text::CString subReq, Net::WebServer::WebController *parent);
			static Bool __stdcall SvcPhotoYear(Net::WebServer::IWebRequest *req, Net::WebServer::IWebResponse *resp, Text::CString subReq, Net::WebServer::WebController *parent);
			static Bool __stdcall SvcPhotoDay(Net::WebServer::IWebRequest *req, Net::WebServer::IWebResponse *resp, Text::CString subReq, Net::WebServer::WebController *parent);
			static Bool __stdcall SvcPhotoUpload(Net::WebServer::IWebRequest *req, Net::WebServer::IWebResponse *resp, Text::CString subReq, Net::WebServer::WebController *parent);
			static Bool __stdcall SvcPhotoUpload2(Net::WebServer::IWebRequest *req, Net::WebServer::IWebResponse *resp, Text::CString subReq, Net::WebServer::WebController *parent);
			static Bool __stdcall SvcPhotoUploadD(Net::WebServer::IWebRequest *req, Net::WebServer::IWebResponse *resp, Text::CString subReq, Net::WebServer::WebController *parent);
			static Bool __stdcall SvcSearchInside(Net::WebServer::IWebRequest *req, Net::WebServer::IWebResponse *resp, Text::CString subReq, Net::WebServer::WebController *parent);
			static Bool __stdcall SvcSearchInsideMoreS(Net::WebServer::IWebRequest *req, Net::WebServer::IWebResponse *resp, Text::CString subReq, Net::WebServer::WebController *parent);
			static Bool __stdcall SvcSearchInsideMoreG(Net::WebServer::IWebRequest *req, Net::WebServer::IWebResponse *resp, Text::CString subReq, Net::WebServer::WebController *parent);
			static Bool __stdcall SvcBookList(Net::WebServer::IWebRequest *req, Net::WebServer::IWebResponse *resp, Text::CString subReq, Net::WebServer::WebController *parent);
			static Bool __stdcall SvcBook(Net::WebServer::IWebRequest *req, Net::WebServer::IWebResponse *resp, Text::CString subReq, Net::WebServer::WebController *parent);
			static Bool __stdcall SvcBookView(Net::WebServer::IWebRequest *req, Net::WebServer::IWebResponse *resp, Text::CString subReq, Net::WebServer::WebController *parent);
			static Bool __stdcall SvcLogin(Net::WebServer::IWebRequest *req, Net::WebServer::IWebResponse *resp, Text::CString subReq, Net::WebServer::WebController *parent);
			static Bool __stdcall SvcLogout(Net::WebServer::IWebRequest *req, Net::WebServer::IWebResponse *resp, Text::CString subReq, Net::WebServer::WebController *parent);
			static Bool __stdcall SvcReload(Net::WebServer::IWebRequest *req, Net::WebServer::IWebResponse *resp, Text::CString subReq, Net::WebServer::WebController *parent);
			static Bool __stdcall SvcRestart(Net::WebServer::IWebRequest *req, Net::WebServer::IWebResponse *resp, Text::CString subReq, Net::WebServer::WebController *parent);
			static Bool __stdcall SvcIndex(Net::WebServer::IWebRequest *req, Net::WebServer::IWebResponse *resp, Text::CString subReq, Net::WebServer::WebController *parent);
			static Bool __stdcall SvcCate(Net::WebServer::IWebRequest *req, Net::WebServer::IWebResponse *resp, Text::CString subReq, Net::WebServer::WebController *parent);
			static Bool __stdcall SvcFavicon(Net::WebServer::IWebRequest *req, Net::WebServer::IWebResponse *resp, Text::CString subReq, Net::WebServer::WebController *parent);
			static Bool __stdcall SvcPublicPOI(Net::WebServer::IWebRequest *req, Net::WebServer::IWebResponse *resp, Text::CString subReq, Net::WebServer::WebController *parent);
			static Bool __stdcall SvcGroupPOI(Net::WebServer::IWebRequest *req, Net::WebServer::IWebResponse *resp, Text::CString subReq, Net::WebServer::WebController *parent);
			static Bool __stdcall SvcSpeciesPOI(Net::WebServer::IWebRequest *req, Net::WebServer::IWebResponse *resp, Text::CString subReq, Net::WebServer::WebController *parent);
			static Bool __stdcall SvcDayPOI(Net::WebServer::IWebRequest *req, Net::WebServer::IWebResponse *resp, Text::CString subReq, Net::WebServer::WebController *parent);

			void AddGroupPOI(Sync::RWMutexUsage *mutUsage, Text::StringBuilderUTF8 *sb, GroupInfo *group, Int32 userId);
			void AddSpeciesPOI(Sync::RWMutexUsage *mutUsage, Text::StringBuilderUTF8 *sb, SpeciesInfo *species, Int32 userId, Bool publicGroup);
			void AddUserfilePOI(Text::StringBuilderUTF8 *sb, SpeciesInfo *species, UserFileInfo *file);

			void ResponsePhoto(Net::WebServer::IWebRequest *req, Net::WebServer::IWebResponse *resp, WebUserInfo *user, Bool isMobile, Int32 speciesId, Int32 cateId, UInt32 imgWidth, UInt32 imgHeight, const UTF8Char *fileName);
			void ResponsePhotoId(Net::WebServer::IWebRequest *req, Net::WebServer::IWebResponse *resp, WebUserInfo *user, Bool isMobile, Int32 speciesId, Int32 cateId, UInt32 imgWidth, UInt32 imgHeight, Int32 photoId);
			void ResponsePhotoWId(Net::WebServer::IWebRequest *req, Net::WebServer::IWebResponse *resp, WebUserInfo *user, Bool isMobile, Int32 speciesId, Int32 cateId, UInt32 imgWidth, UInt32 imgHeight, Int32 photoWId);

		public:
			OrganWebMainController(Net::WebServer::MemoryWebSessionManager *sessMgr, OrganWebEnv *env, UInt32 scnSize);
			virtual ~OrganWebMainController();
		};
	}
}
#endif
