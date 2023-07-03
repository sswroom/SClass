#ifndef _SM_UI_DOBJ_ROLLINGTEXTDOBJ
#define _SM_UI_DOBJ_ROLLINGTEXTDOBJ
#include "Data/DateTime.h"
#include "UI/DObj/DirectObject.h"

namespace UI
{
	namespace DObj
	{
		class RollingTextDObj : public DirectObject
		{
		private:
			Media::DrawEngine *deng;
			Text::String *txt;
			Text::String *fontName;
			Double fontSize;
			UInt32 fontColor;
			UInt32 codePage;
			Math::Size2D<UOSInt> size;
			Double lineHeight;
			Media::DrawImage *dimg;
			Double rollSpeed;
			OSInt lastRollPos;
			Data::DateTime startTime;

			void UpdateBGImg();
		public:
			RollingTextDObj(Media::DrawEngine *deng, Text::CString txt, Text::CString fontName, Double fontSize, UInt32 fontColor, UInt32 codePage, Math::Coord2D<OSInt> tl, Math::Size2D<UOSInt> size, Double rollSpeed);
			virtual ~RollingTextDObj();

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
