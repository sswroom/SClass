#ifndef _SM_MEDIA_MEDIAPLAYERWEBINTERFACE
#define _SM_MEDIA_MEDIAPLAYERWEBINTERFACE
#include "Media/MediaPlayerInterface.h"
#include "Net/WebServer/IWebHandler.h"
#include "Text/String.h"

namespace Media
{
	class MediaPlayerWebInterface : public Net::WebServer::IWebHandler
	{
	private:
		struct VideoFileInfo
		{
			NN<Text::String> fileName;
			UInt64 fileSize;
		};

	private:
		Media::MediaPlayerInterface *iface;
		Bool autoRelease;

		static OSInt __stdcall VideoFileCompare(VideoFileInfo *file1, VideoFileInfo *file2);
	public:
		MediaPlayerWebInterface(Media::MediaPlayerInterface *iface, Bool autoRelease);
		virtual ~MediaPlayerWebInterface();

		void BrowseRequest(NN<Net::WebServer::IWebRequest> req, NN<Net::WebServer::IWebResponse> resp);
		virtual void WebRequest(NN<Net::WebServer::IWebRequest> req, NN<Net::WebServer::IWebResponse> resp);
		virtual void Release();
	};
}
#endif
