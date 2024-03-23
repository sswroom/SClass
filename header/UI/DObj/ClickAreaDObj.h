#ifndef _SM_UI_DOBJ_CLICKAREADOBJ
#define _SM_UI_DOBJ_CLICKAREADOBJ
#include "AnyType.h"
#include "Data/RandomOS.h"
#include "UI/GUICore.h"
#include "UI/DObj/DirectObject.h"

namespace UI
{
	namespace DObj
	{
		class ClickAreaDObj : public DirectObject
		{
		private:
			NotNullPtr<Media::DrawEngine> deng;
			UI::UIEvent clkHdlr;
			AnyType clkUserObj;
			OSInt width;
			OSInt height;

		public:
			ClickAreaDObj(NotNullPtr<Media::DrawEngine> deng, Math::Coord2D<OSInt> tl, OSInt width, OSInt height, UI::UIEvent clkHdlr, AnyType clkUserObj);
			virtual ~ClickAreaDObj();

			virtual Bool IsChanged();
			virtual Bool DoEvents();
			virtual void DrawObject(NotNullPtr<Media::DrawImage> dimg);

			virtual Bool IsObject(Math::Coord2D<OSInt> scnPos);
			//virtual System::Windows::Forms::Cursor ^GetCursor() override;
			virtual void OnMouseDown();
			virtual void OnMouseUp();
			virtual void OnMouseClick();
		};
	}
}
#endif
