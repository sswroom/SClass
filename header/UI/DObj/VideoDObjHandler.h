#ifndef _SM_UI_DOBJ_VIDEODOBJHANDLER
#define _SM_UI_DOBJ_VIDEODOBJHANDLER
#include "Media/MediaFile.h"
#include "Media/MediaPlayer.h"
#include "Media/VideoRenderer.h"
#include "Parser/ParserList.h"
#include "UI/GUIForm.h"
#include "UI/DObj/ImageDObjHandler.h"

namespace UI
{
	namespace DObj
	{
		class VideoDObjHandler : public ImageDObjHandler, public Media::VideoRenderer
		{
		protected:
			Int32 videoX;
			Int32 videoY;
			UInt32 videoW;
			UInt32 videoH;

			UI::GUIForm *ownerFrm;
			UI::GUITimer *tmr;
			Text::String *videoFileName;
			Sync::Mutex frameMut;
			Media::DrawImage *frameImg;
			Parser::ParserList *parsers;
			Media::MediaFile *mf;
			Media::MediaPlayer *player;

			static void __stdcall OnTimerTick(void *userObj);
			static void __stdcall OnPBEnd(void *userObj);
		protected:
			virtual void DrawBkg(Media::DrawImage *dimg);
			void DrawVideo(Media::DrawImage *dimg);

			virtual void LockUpdateSize(Sync::MutexUsage *mutUsage);
			virtual void DrawFromSurface(Media::MonitorSurface *surface, OSInt destX, OSInt destY, UOSInt buffWidth, UOSInt buffHeight, Bool clearScn);
		public:
			VideoDObjHandler(UI::GUIForm *ownerFrm, Media::DrawEngine *deng, Media::ColorManagerSess *colorSess, Media::MonitorSurfaceMgr *surfaceMgr, Parser::ParserList *parsers, Text::CString imageFileName, Int32 videoX, Int32 videoY, UInt32 videoW, UInt32 videoH, Text::CString videoFileName);
			virtual ~VideoDObjHandler();

			void UpdateVideoArea(Int32 videoX, Int32 videoY, UInt32 videoW, UInt32 videoH);
		};
	}
}
#endif
