#ifndef _SM_MEDIA_MEDIAPLAYERWEBINTERFACE
#define _SM_MEDIA_MEDIAPLAYERWEBINTERFACE
#include "Media/MediaPlayerInterface.h"
#include "Net/WebServer/WebHandler.h"
#include "Text/String.h"

namespace Media
{
	class MediaPlayerWebInterface : public Net::WebServer::WebHandler
	{
	private:
		struct VideoFileInfo
		{
			NN<Text::String> fileName;
			UInt64 fileSize;
		};

	private:
		NN<Media::MediaPlayerInterface> iface;
		Bool autoRelease;

		static IntOS __stdcall VideoFileCompare(NN<VideoFileInfo> file1, NN<VideoFileInfo> file2);
	public:
		MediaPlayerWebInterface(NN<Media::MediaPlayerInterface> iface, Bool autoRelease);
		virtual ~MediaPlayerWebInterface();

		void BrowseRequest(NN<Net::WebServer::WebRequest> req, NN<Net::WebServer::WebResponse> resp);
		virtual void DoWebRequest(NN<Net::WebServer::WebRequest> req, NN<Net::WebServer::WebResponse> resp);
		virtual void Release();
	};
}
#endif
