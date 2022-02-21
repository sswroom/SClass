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
			Media::DrawEngine *deng;
			Media::DrawImage *bmpUnclick;
			Media::DrawImage *bmpClicked;
			Bool isMouseDown;
			OSInt dispLeft;
			OSInt dispTop;
			UI::UIEvent clkHdlr;
			void *clkUserObj;
			Bool updated;

		public:
			DownButtonDObj(Media::DrawEngine *deng, Text::CString fileNameUnclick, Text::CString fileNameClicked, OSInt left, OSInt top, UI::UIEvent clkHdlr, void *clkUserObj);
			virtual ~DownButtonDObj();

			virtual Bool IsChanged();
			virtual Bool DoEvents();
			virtual void DrawObject(Media::DrawImage *dimg);

			virtual Bool IsObject(OSInt x, OSInt y);
			//virtual System::Windows::Forms::Cursor ^GetCursor() override;
			virtual void OnMouseDown();
			virtual void OnMouseUp();
			virtual void OnMouseClick();
		};
	}
}
#endif
