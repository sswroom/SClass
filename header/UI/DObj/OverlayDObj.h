#ifndef _SM_UI_DOBJ_OVERLAYDOBJ
#define _SM_UI_DOBJ_OVERLAYDOBJ
#include "Manage/HiResClock.h"
#include "Media/ImageList.h"
#include "Parser/ParserList.h"
#include "UI/DObj/DirectObject.h"

namespace UI
{
	namespace DObj
	{
		class OverlayDObj : public DirectObject
		{
		private:
			NN<Media::DrawEngine> deng;
			Optional<Media::DrawImage> bmp;
			Optional<Media::ImageList> imgList;
			Bool noRelease;
			IntOS frameDelay;
			Manage::HiResClock *clk;
			Double startTime;
			IntOS lastFrameNum;

		public:
			OverlayDObj(NN<Media::DrawEngine> deng, Optional<Media::DrawImage> bmp, Math::Coord2D<IntOS> tl);
			OverlayDObj(NN<Media::DrawEngine> deng, Text::CString fileName, Math::Coord2D<IntOS> tl, NN<Parser::ParserList> parsers);
			virtual ~OverlayDObj();

			virtual Bool IsChanged();
			virtual Bool DoEvents();
			virtual void DrawObject(NN<Media::DrawImage> dimg);

			virtual Bool IsObject(Math::Coord2D<IntOS> scnPos);
			//virtual System::Windows::Forms::Cursor ^GetCursor() override;
			virtual void OnMouseDown();
			virtual void OnMouseUp();
			virtual void OnMouseClick();

			void SetFrameDelay(IntOS frameDelay);
		};
	}
}
#endif
