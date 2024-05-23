#ifndef _SM_UI_DOBJ_BUTTONDOBJ
#define _SM_UI_DOBJ_BUTTONDOBJ
#include "AnyType.h"
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
			NN<Media::DrawEngine> deng;
			Optional<Media::DrawImage> bmpUnclick;
			Optional<Media::DrawImage> bmpClicked;
			Double alpha;
			Double a;
			Data::RandomOS *rnd;
			Bool isMouseDown;
			Bool isMouseUp;
			Bool isMouseClick;
			Math::Coord2D<OSInt> dispTL;
			Double downAlpha;
			Data::DateTime downTime;
			UI::UIEvent clkHdlr;
			AnyType clkUserObj;
			Bool rectMode;
			Bool isVisible;

			static UInt32 __stdcall ClickThread(AnyType userObj);
		public:
			ButtonDObj(NN<Media::DrawEngine> deng, Text::CString fileNameUnclick, Text::CString fileNameClicked, Math::Coord2D<OSInt> tl, Bool rectMode, UI::UIEvent clkHdlr, AnyType clkUserObj);
			virtual ~ButtonDObj();

			void SetRectMode(Bool rectMode);
			void SetVisible(Bool isVisible);

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
