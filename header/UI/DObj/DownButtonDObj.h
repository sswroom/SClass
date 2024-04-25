#ifndef _SM_UI_DOBJ_DOWNBUTTONDOBJ
#define _SM_UI_DOBJ_DOWNBUTTONDOBJ
#include "AnyType.h"
#include "UI/GUICore.h"
#include "UI/DObj/DirectObject.h"

namespace UI
{
	namespace DObj
	{
		class DownButtonDObj : public DirectObject
		{
		private:
			NN<Media::DrawEngine> deng;
			Media::DrawImage *bmpUnclick;
			Media::DrawImage *bmpClicked;
			Bool isMouseDown;
			Math::Coord2D<OSInt> dispTL;
			UI::UIEvent clkHdlr;
			AnyType clkUserObj;
			Bool updated;

		public:
			DownButtonDObj(NN<Media::DrawEngine> deng, Text::CString fileNameUnclick, Text::CString fileNameClicked, Math::Coord2D<OSInt> tl, UI::UIEvent clkHdlr, AnyType clkUserObj);
			virtual ~DownButtonDObj();

			virtual Bool IsChanged();
			virtual Bool DoEvents();
			virtual void DrawObject(NN<Media::DrawImage> dimg);

			virtual Bool IsObject(Math::Coord2D<OSInt> scnPos);
			//virtual System::Windows::Forms::Cursor ^GetCursor() override;
			virtual void OnMouseDown();
			virtual void OnMouseUp();
			virtual void OnMouseClick();
		};
	}
}
#endif
