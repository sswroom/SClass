#ifndef _SM_SSWR_ORGANWEB_ORGANWEBPHOTOCONTROLLER
#define _SM_SSWR_ORGANWEB_ORGANWEBPHOTOCONTROLLER
#include "Data/RandomOS.h"
#include "Media/ColorProfile.h"
#include "Media/LRGBLimiter.h"
#include "Media/CS/CSConverter.h"
#include "Media/Resizer/LanczosResizerLR_C32.h"
#include "Sync/Mutex.h"
#include "SSWR/OrganWeb/OrganWebController.h"

namespace SSWR
{
	namespace OrganWeb
	{
		class OrganWebPhotoController : public OrganWebController
		{
		private:
			Data::RandomOS random;
			Sync::Mutex resizerMut;
			Media::Resizer::LanczosResizerLR_C32 *resizerLR;
			Sync::Mutex csconvMut;
			Optional<Media::CS::CSConverter> csconv;
			UInt32 csconvFCC;
			UInt32 csconvBpp;
			Media::PixelFormat csconvPF;
			Media::ColorProfile csconvColor;
			Media::LRGBLimiter lrgbLimiter;

			static Bool __stdcall SvcPhoto(NN<Net::WebServer::WebRequest> req, NN<Net::WebServer::WebResponse> resp, Text::CStringNN subReq, NN<Net::WebServer::WebController> parent);
			static Bool __stdcall SvcPhotoDown(NN<Net::WebServer::WebRequest> req, NN<Net::WebServer::WebResponse> resp, Text::CStringNN subReq, NN<Net::WebServer::WebController> parent);
			static Bool __stdcall SvcFavicon(NN<Net::WebServer::WebRequest> req, NN<Net::WebServer::WebResponse> resp, Text::CStringNN subReq, NN<Net::WebServer::WebController> parent);

			void ResponsePhoto(NN<Net::WebServer::WebRequest> req, NN<Net::WebServer::WebResponse> resp, Optional<WebUserInfo> user, Bool isMobile, Int32 speciesId, Int32 cateId, UInt32 imgWidth, UInt32 imgHeight, const UTF8Char *fileName);
			void ResponsePhotoId(NN<Net::WebServer::WebRequest> req, NN<Net::WebServer::WebResponse> resp, Optional<WebUserInfo> user, Bool isMobile, Int32 speciesId, Int32 cateId, UInt32 imgWidth, UInt32 imgHeight, Int32 photoId);
			void ResponsePhotoWId(NN<Net::WebServer::WebRequest> req, NN<Net::WebServer::WebResponse> resp, Optional<WebUserInfo> user, Bool isMobile, Int32 speciesId, Int32 cateId, UInt32 imgWidth, UInt32 imgHeight, Int32 photoWId);

		public:
			OrganWebPhotoController(NN<Net::WebServer::MemoryWebSessionManager> sessMgr, NN<OrganWebEnv> env, UInt32 scnSize);
			virtual ~OrganWebPhotoController();
		};
	}
}
#endif
