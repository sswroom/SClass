#ifndef _SM_UI_DOBJ_BUTTONDOBJ
#define _SM_UI_DOBJ_BUTTONDOBJ
#include "Data/RandomOS.h"
#include "UI/GUICore.h"
#include "UI/DObj/DirectObject.h"

namespace UI
{
	namespace DObj
	{
		class ButtonDObj : public DirectObject
		{
		private:
			Media::DrawEngine *deng;
			Media::DrawImage *bmpUnclick;
			Media::DrawImage *bmpClicked;
			Double alpha;
			Double a;
			Data::RandomOS *rnd;
			Bool isMouseDown;
			Bool isMouseUp;
			Bool isMouseClick;
			OSInt dispLeft;
			OSInt dispTop;
			Double downAlpha;
			Data::DateTime *downTime;
			UI::UIEvent clkHdlr;
			void *clkUserObj;
			Bool rectMode;
			Bool isVisible;

			static UInt32 __stdcall ClickThread(void *userObj);
		public:
			ButtonDObj(Media::DrawEngine *deng, const WChar *fileNameUnclick, const WChar *fileNameClicked, Int32 left, Int32 top, Bool rectMode, UI::UIEvent clkHdlr, void *clkUserObj);
			virtual ~ButtonDObj();

			void SetRectMode(Bool rectMode);
			void SetVisible(Bool isVisible);

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
