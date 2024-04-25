#ifndef _SM_UI_DOBJ_SIZEDOVERLAYDOBJ
#define _SM_UI_DOBJ_SIZEDOVERLAYDOBJ
#include "Manage/HiResClock.h"
#include "Math/Coord2DDbl.h"
#include "Media/ImageList.h"
#include "Media/Resizer/LanczosResizer8_C8.h"
#include "Parser/ParserList.h"
#include "UI/DObj/DirectObject.h"

namespace UI
{
	namespace DObj
	{
		class SizedOverlayDObj : public DirectObject
		{
		private:
			NN<Media::DrawEngine> deng;
			Sync::Mutex imgMut;
			Media::ImageList *imgList;
			Bool noRelease;
			OSInt frameDelay;
			Manage::HiResClock *clk;
			Double startTime;
			OSInt lastFrameNum;
			Math::Size2D<UOSInt> size;
			Media::Resizer::LanczosResizer8_C8 *resizer;
			Sync::Mutex dispMut;
			Media::StaticImage *dispImg;
			UOSInt dispFrameNum;
			Math::Coord2DDbl drawOfst;

		public:
			SizedOverlayDObj(NN<Media::DrawEngine> deng, Text::CString fileName, Math::Coord2D<OSInt> tl, Math::Size2D<UOSInt> size, Parser::ParserList *parsers, Media::Resizer::LanczosResizer8_C8 *resizer);
			virtual ~SizedOverlayDObj();

			virtual Bool IsChanged();
			virtual Bool DoEvents();
			virtual void DrawObject(NN<Media::DrawImage> dimg);

			virtual Bool IsObject(Math::Coord2D<OSInt> scnPos);
			virtual void OnMouseDown();
			virtual void OnMouseUp();
			virtual void OnMouseClick();

			void SetFrameDelay(OSInt frameDelay);
			void SetSize(Math::Size2D<UOSInt> size);
			void SetImage(Text::CStringNN fileName, Parser::ParserList *parsers);
		};
	}
}
#endif
