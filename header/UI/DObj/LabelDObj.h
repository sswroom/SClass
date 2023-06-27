#ifndef _SM_UI_DOBJ_LABELDOBJ
#define _SM_UI_DOBJ_LABELDOBJ
#include "Sync/Mutex.h"
#include "UI/DObj/DirectObject.h"

namespace UI
{
	namespace DObj
	{
		class LabelDObj : public DirectObject
		{
		private:
			Media::DrawEngine *deng;
			Text::String *txt;
			Sync::Mutex txtMut;
			Bool txtChg;
			Text::String *fontName;
			Double fontSizePx;
			Media::DrawEngine::DrawFontStyle fontStyle;
			UInt32 fontColor;
			UInt32 codePage;

		public:
			LabelDObj(Media::DrawEngine *deng, Text::CString txt, Text::CString fontName, Double fontSizePx, Media::DrawEngine::DrawFontStyle fontStyle, UInt32 fontColor, Math::Coord2D<OSInt> tl, UInt32 codePage);
			virtual ~LabelDObj();

			virtual Bool IsChanged();
			virtual Bool DoEvents();
			virtual void DrawObject(Media::DrawImage *dimg);

			virtual Bool IsObject(OSInt x, OSInt y);
			virtual void OnMouseDown();
			virtual void OnMouseUp();
			virtual void OnMouseClick();

			void SetFont(Text::CString fontName, Double fontSizePx);
			void SetText(Text::CString txt);
		};
	}
}
#endif
