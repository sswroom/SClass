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
			NN<Media::DrawEngine> deng;
			Optional<Text::String> txt;
			NN<Text::String> fontName;
			Double fontSize;
			UInt32 fontColor;
			UInt32 codePage;
			Math::Size2D<UIntOS> size;
			Double lineHeight;
			Optional<Media::DrawImage> dimg;
			Double rollSpeed;
			IntOS lastRollPos;
			Data::DateTime startTime;

			void UpdateBGImg();
		public:
			RollingTextDObj(NN<Media::DrawEngine> deng, Text::CString txt, Text::CString fontName, Double fontSize, UInt32 fontColor, UInt32 codePage, Math::Coord2D<IntOS> tl, Math::Size2D<UIntOS> size, Double rollSpeed);
			virtual ~RollingTextDObj();

			virtual Bool IsChanged();
			virtual Bool DoEvents();
			virtual void DrawObject(NN<Media::DrawImage> dimg);

			virtual Bool IsObject(Math::Coord2D<IntOS> scnPos);
			//virtual System::Windows::Forms::Cursor ^GetCursor() override;
			virtual void OnMouseDown();
			virtual void OnMouseUp();
			virtual void OnMouseClick();
		};
	}
}
#endif
