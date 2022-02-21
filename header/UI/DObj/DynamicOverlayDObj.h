#ifndef _SM_UI_DOBJ_DYNAMICOVERLAYDOBJ
#define _SM_UI_DOBJ_DYNAMICOVERLAYDOBJ
#include "Data/Random.h"
#include "UI/DObj/DirectObject.h"

namespace UI
{
	namespace DObj
	{
		class DynamicOverlayDObj : public DirectObject
		{
		private:
			Media::DrawEngine *deng;
			Media::DrawImage *bmp1;
			Media::DrawImage *bmp2;
			Double alpha;
			Double a;
			Data::Random *rnd;
		public:
			DynamicOverlayDObj(Media::DrawEngine *deng, Text::CString fileName1, Text::CString fileName2, Int32 left, Int32 top);
			virtual ~DynamicOverlayDObj();

			virtual Bool IsChanged();
			virtual Bool DoEvents();
			virtual void DrawObject(Media::DrawImage *dimg);

			virtual Bool IsObject(Int32 x, Int32 y);
			//virtual System::Windows::Forms::Cursor ^GetCursor();
			virtual void OnMouseDown();
			virtual void OnMouseUp();
			virtual void OnMouseClick();
		};
	}
}
#endif
