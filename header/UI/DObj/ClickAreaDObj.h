#ifndef _SM_UI_DOBJ_CLICKAREADOBJ
#define _SM_UI_DOBJ_CLICKAREADOBJ
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
			Media::DrawEngine *deng;
			UI::UIEvent clkHdlr;
			void *clkUserObj;
			OSInt width;
			OSInt height;

		public:
			ClickAreaDObj(Media::DrawEngine *deng, Math::Coord2D<OSInt> tl, OSInt width, OSInt height, UI::UIEvent clkHdlr, void *clkUserObj);
			virtual ~ClickAreaDObj();

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
