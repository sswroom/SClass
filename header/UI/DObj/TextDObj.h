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
			Media::DrawEngine *deng;
			Text::String *txt;
			Text::String *fontName;
			Double fontSize;
			Media::DrawEngine::DrawFontStyle fontStyle;
			TextAlign talign;
			UInt32 fontColor;
			UInt32 codePage;
			UOSInt width;
			UOSInt height;
			Double lineHeight;
			UInt32 currPage;
			Bool pageChg;
			Data::ArrayList<Text::String *> lines;

		public:
			TextDObj(Media::DrawEngine *deng, Text::CString txt, Text::CString fontName, Double fontSize, Media::DrawEngine::DrawFontStyle fontStyle, UInt32 fontColor, OSInt left, OSInt top, UOSInt width, UOSInt height, UInt32 codePage);
			virtual ~TextDObj();

			virtual Bool IsChanged();
			virtual Bool DoEvents();
			virtual void DrawObject(Media::DrawImage *dimg);

			virtual Bool IsObject(OSInt x, OSInt y);
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
