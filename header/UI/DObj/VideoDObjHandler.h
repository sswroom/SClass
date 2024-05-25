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
			Math::Coord2D<OSInt> videoTL;
			Math::Size2D<UOSInt> videoSize;

			UI::GUIForm *ownerFrm;
			NN<UI::GUITimer> tmr;
			NN<Text::String> videoFileName;
			Sync::Mutex frameMut;
			Optional<Media::DrawImage> frameImg;
			Parser::ParserList *parsers;
			Optional<Media::MediaFile> mf;
			Media::MediaPlayer *player;

			static void __stdcall OnTimerTick(AnyType userObj);
			static void __stdcall OnPBEnd(AnyType userObj);
		protected:
			virtual void DrawBkg(NN<Media::DrawImage> dimg);
			void DrawVideo(NN<Media::DrawImage> dimg);

			virtual void LockUpdateSize(NN<Sync::MutexUsage> mutUsage);
			virtual void DrawFromSurface(NN<Media::MonitorSurface> surface, Math::Coord2D<OSInt> destTL, Math::Size2D<UOSInt> buffSize, Bool clearScn);
		public:
			VideoDObjHandler(UI::GUIForm *ownerFrm, NN<Media::DrawEngine> deng, NN<Media::ColorManagerSess> colorSess, NN<Media::MonitorSurfaceMgr> surfaceMgr, Parser::ParserList *parsers, Text::CStringNN imageFileName, Math::Coord2D<OSInt> videoTL, Math::Size2D<UOSInt> videoSize, Text::CStringNN videoFileName);
			virtual ~VideoDObjHandler();

			void UpdateVideoArea(Math::Coord2D<OSInt> videoTL, Math::Size2D<UOSInt> videoSize);

			virtual void SetRotateType(Media::RotateType rotateType);
			virtual Media::RotateType GetRotateType() const;
		};
	}
}
#endif
