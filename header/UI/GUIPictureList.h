#ifndef _SM_UI_GUIPICTURELIST
#define _SM_UI_GUIPICTURELIST
#include "Media/CS/CSConverter.h"
#include "Media/ImageResizer.h"
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
		Data::ArrayListNN<Media::DrawImage> imgList;
		UOSInt selectedIndex;
		Media::ImageResizer *resizer;
		Math::Size2D<UOSInt> iconSize;
		Bool hasBorder;

	public:
		GUIPictureList(NN<GUICore> ui, NN<UI::GUIClientControl> parent, NN<Media::DrawEngine> eng, Bool hasBorder, Math::Size2D<UOSInt> iconSize, Optional<Media::ColorSess> colorSess);
		virtual ~GUIPictureList();

		virtual void OnDraw(NN<Media::DrawImage> img);
		virtual void OnMouseDown(OSInt scrollY, Math::Coord2D<OSInt> pos, UI::GUIClientControl::MouseButton btn, KeyButton keys);

		UOSInt IndexFromPoint(Math::Coord2D<OSInt> pos);
		void Add(NN<Media::RasterImage> img);
		UOSInt GetCount();
		void RemoveAt(UOSInt index);
		void Clear();
		UOSInt GetSelectedIndex();
		void SetSelectedIndex(UOSInt index);
	};
}
#endif
