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
		Data::ArrayList<Media::DrawImage *> imgList;
		UOSInt selectedIndex;
		Media::IImgResizer *resizer;
		Math::Size2D<UOSInt> iconSize;
		Bool hasBorder;

	public:
		GUIPictureList(NotNullPtr<GUICore> ui, UI::GUIClientControl *parent, NotNullPtr<Media::DrawEngine> eng, Bool hasBorder, Math::Size2D<UOSInt> iconSize);
		virtual ~GUIPictureList();

		virtual void OnDraw(NotNullPtr<Media::DrawImage> img);
		virtual void OnMouseDown(OSInt scrollY, Math::Coord2D<OSInt> pos, UI::GUIClientControl::MouseButton btn, KeyButton keys);

		UOSInt IndexFromPoint(Math::Coord2D<OSInt> pos);
		void Add(Media::Image *img);
		UOSInt GetCount();
		void RemoveAt(UOSInt index);
		void Clear();
		UOSInt GetSelectedIndex();
		void SetSelectedIndex(UOSInt index);
	};
}
#endif
