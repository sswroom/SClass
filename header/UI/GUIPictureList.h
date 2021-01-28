#ifndef _SM_UI_GUIPICTURELIST
#define _SM_UI_GUIPICTURELIST
#include "Media/CS/CSConverter.h"
#include "Media/IImgResizer.h"
#include "Media/ColorManager.h"
#include "Media/DrawEngine.h"
#include "Media/StaticImage.h"
#include "Parser/ParserList.h"
#include "UI/GUIControl.h"
#include "UI/GUICustomDrawVScroll.h"

namespace UI
{
	class GUIPictureList : public GUICustomDrawVScroll
	{
	private:
		static Int32 useCnt;
		Data::ArrayList<Media::DrawImage *> *imgList;
		OSInt selectedIndex;
		Media::IImgResizer *resizer;
		UOSInt iconWidth;
		UOSInt iconHeight;
		Bool hasBorder;
		Bool allowResize;

	public:
		GUIPictureList(GUICore *ui, UI::GUIClientControl *parent, Media::DrawEngine *eng, Bool hasBorder, UOSInt iconWidth, UOSInt iconHeight);
		virtual ~GUIPictureList();

		virtual void OnDraw(Media::DrawImage *img);
		virtual void OnMouseDown(OSInt scrollY, Int32 xPos, Int32 yPos, UI::GUIClientControl::MouseButton btn, KeyButton keys);

		OSInt IndexFromPoint(Int32 x, Int32 y);
		void Add(Media::Image *img);
		OSInt GetCount();
		void RemoveAt(OSInt index);
		void Clear();
		OSInt GetSelectedIndex();
		void SetSelectedIndex(OSInt index);
	};
}
#endif
