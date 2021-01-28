#ifndef _SM_DOTNET_PICTUREBOXRENDERER
#define _SM_DOTNET_PICTUREBOXRENDERER
#include "Media/IVideoRenderer.h"
#include "Media/IImgResizer.h"
#include "Media/CS/CSConverter.h"
#include "Media/DDrawRenderer2.h"
#include "Media/DDrawRendererLR.h"
#include "Media/ColorManager.h"

namespace DotNet
{
	typedef struct
	{
		UInt8 *frameData;
		Media::FrameType frameType;
		Int32 frameTime;
	} VideoFrame;

	__gc class PictureBoxRenderer// : public Media::IVideoRenderer
	{
	private:
		System::Windows::Forms::Control *formCtrl;
		System::EventHandler *evtHdlr;

		Media::IVideoSource *vidsrc;
		Media::CS::CSConverter *csconv;
		Bool playing;
		Bool stopPlay;
		Media::RefClock *playClk;
		Media::FrameInfo *info;
		VideoFrame *frames;
		Media::ColorManager *colorMgr;

//		Media::DDrawRenderer2 *renderer;
		Media::DDrawRendererLR *renderer;

		Bool captureFrame;
		Media::Image *captureResult;
		Sync::Mutex *captureMut;
		Sync::Event *captureEvt;

		void SizeChg(System::Object *sender, System::EventArgs *e);
		void PlayThread();
	public:
		PictureBoxRenderer(System::Windows::Forms::Control *formCtrl, Media::ColorManager *colorMgr);
		virtual ~PictureBoxRenderer();

		virtual Bool BindVideo(Media::IVideoSource *vidsrc);
		virtual Media::IVideoRenderer::RendererResult Start(Media::RefClock *clk);
		virtual void Stop();
		virtual Bool IsPlaying();
		virtual Media::Image *CaptureFrame();
	};
};
#endif
