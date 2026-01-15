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
			NN<Media::DrawEngine> deng;
			Optional<Media::DrawImage> bmp1;
			Optional<Media::DrawImage> bmp2;
			Double alpha;
			Double a;
			Data::Random *rnd;
		public:
			DynamicOverlayDObj(NN<Media::DrawEngine> deng, Text::CString fileName1, Text::CString fileName2, Math::Coord2D<IntOS> tl);
			virtual ~DynamicOverlayDObj();

			virtual Bool IsChanged();
			virtual Bool DoEvents();
			virtual void DrawObject(NN<Media::DrawImage> dimg);

			virtual Bool IsObject(Math::Coord2D<IntOS> scnPos);
			//virtual System::Windows::Forms::Cursor ^GetCursor();
			virtual void OnMouseDown();
			virtual void OnMouseUp();
			virtual void OnMouseClick();
		};
	}
}
#endif
