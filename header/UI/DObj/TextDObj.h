#ifndef _SM_UI_DOBJ_TEXTDOBJ
#define _SM_UI_DOBJ_TEXTDOBJ
#include "UI/DObj/DirectObject.h"

namespace UI
{
	namespace DObj
	{
		class TextDObj : public DirectObject
		{
		public:
			typedef enum
			{
				TA_LEFT,
				TA_CENTER,
				TA_RIGHT
			} TextAlign;
		private:
			NN<Media::DrawEngine> deng;
			Text::String *txt;
			NN<Text::String> fontName;
			Double fontSize;
			Media::DrawEngine::DrawFontStyle fontStyle;
			TextAlign talign;
			UInt32 fontColor;
			UInt32 codePage;
			Math::Size2D<UOSInt> size;
			Double lineHeight;
			UInt32 currPage;
			Bool pageChg;
			Data::ArrayListStringNN lines;

		public:
			TextDObj(NN<Media::DrawEngine> deng, Text::CString txt, Text::CString fontName, Double fontSize, Media::DrawEngine::DrawFontStyle fontStyle, UInt32 fontColor, UInt32 codePage, Math::Coord2D<OSInt> tl, Math::Size2D<UOSInt> size);
			virtual ~TextDObj();

			virtual Bool IsChanged();
			virtual Bool DoEvents();
			virtual void DrawObject(NN<Media::DrawImage> dimg);

			virtual Bool IsObject(Math::Coord2D<OSInt> scnPos);
			//virtual System::Windows::Forms::Cursor ^GetCursor() override;
			virtual void OnMouseDown();
			virtual void OnMouseUp();
			virtual void OnMouseClick();

			void SetTextAlign(TextAlign talign);
			UOSInt GetPageCount();
			UInt32 GetCurrPage();
			void SetCurrPage(UInt32 currPage);
		};
	}
}
#endif
