#ifndef _SM_UI_DOBJ_DOWNBUTTONDOBJ
#define _SM_UI_DOBJ_DOWNBUTTONDOBJ
#include "UI/GUICore.h"
#include "UI/DObj/DirectObject.h"

namespace UI
{
	namespace DObj
	{
		class DownButtonDObj : public DirectObject
		{
		private:
			NotNullPtr<Media::DrawEngine> deng;
			Media::DrawImage *bmpUnclick;
			Media::DrawImage *bmpClicked;
			Bool isMouseDown;
			Math::Coord2D<OSInt> dispTL;
			UI::UIEvent clkHdlr;
			void *clkUserObj;
			Bool updated;

		public:
			DownButtonDObj(NotNullPtr<Media::DrawEngine> deng, Text::CString fileNameUnclick, Text::CString fileNameClicked, Math::Coord2D<OSInt> tl, UI::UIEvent clkHdlr, void *clkUserObj);
			virtual ~DownButtonDObj();

			virtual Bool IsChanged();
			virtual Bool DoEvents();
			virtual void DrawObject(Media::DrawImage *dimg);

			virtual Bool IsObject(Math::Coord2D<OSInt> scnPos);
			//virtual System::Windows::Forms::Cursor ^GetCursor() override;
			virtual void OnMouseDown();
			virtual void OnMouseUp();
			virtual void OnMouseClick();
		};
	}
}
#endif
