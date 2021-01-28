#ifndef _SM_UI_DOBJ_OVERLAYDOBJ
#define _SM_UI_DOBJ_OVERLAYDOBJ
#include "Manage/HiResClock.h"
#include "Media/ImageLIst.h"
#include "Parser/ParserList.h"
#include "UI/DObj/DirectObject.h"

namespace UI
{
	namespace DObj
	{
		class OverlayDObj : public DirectObject
		{
		private:
			Media::DrawEngine *deng;
			Media::DrawImage *bmp;
			Media::ImageList *imgList;
			Bool noRelease;
			OSInt frameDelay;
			Manage::HiResClock *clk;
			Double startTime;
			OSInt lastFrameNum;

		public:
			OverlayDObj(Media::DrawEngine *deng, Media::DrawImage *bmp, OSInt left, OSInt top);
			OverlayDObj(Media::DrawEngine *deng, const UTF8Char *fileName, OSInt left, OSInt top, Parser::ParserList *parsers);
			virtual ~OverlayDObj();

			virtual Bool IsChanged();
			virtual Bool DoEvents();
			virtual void DrawObject(Media::DrawImage *dimg);

			virtual Bool IsObject(OSInt x, OSInt y);
			//virtual System::Windows::Forms::Cursor ^GetCursor() override;
			virtual void OnMouseDown();
			virtual void OnMouseUp();
			virtual void OnMouseClick();

			void SetFrameDelay(OSInt frameDelay);
		};
	}
}
#endif
