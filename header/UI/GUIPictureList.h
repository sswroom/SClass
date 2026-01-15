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
		UIntOS selectedIndex;
		Media::ImageResizer *resizer;
		Math::Size2D<UIntOS> iconSize;
		Bool hasBorder;

	public:
		GUIPictureList(NN<GUICore> ui, NN<UI::GUIClientControl> parent, NN<Media::DrawEngine> eng, Bool hasBorder, Math::Size2D<UIntOS> iconSize, Optional<Media::ColorSess> colorSess);
		virtual ~GUIPictureList();

		virtual void OnDraw(NN<Media::DrawImage> img);
		virtual void OnMouseDown(IntOS scrollY, Math::Coord2D<IntOS> pos, UI::GUIClientControl::MouseButton btn, KeyButton keys);

		UIntOS IndexFromPoint(Math::Coord2D<IntOS> pos);
		void Add(NN<Media::RasterImage> img);
		UIntOS GetCount();
		void RemoveAt(UIntOS index);
		void Clear();
		UIntOS GetSelectedIndex();
		void SetSelectedIndex(UIntOS index);
	};
}
#endif
