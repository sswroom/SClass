#ifndef _SM_UI_DOBJ_IMAGEDOBJHANDLER
#define _SM_UI_DOBJ_IMAGEDOBJHANDLER
#include "UI/DObj/DObjHandler.h"

namespace UI
{
	namespace DObj
	{
		class ImageDObjHandler : public DObjHandler
		{
		protected:
			Media::DrawImage *bmpBkg;
			UInt32 bgColor;

		protected:
			virtual void DrawBkg(Media::DrawImage *dimg);
		public:
			ImageDObjHandler(Media::DrawEngine *deng, Text::CString fileName);
			virtual ~ImageDObjHandler();

			void SetBGColor(UInt32 bgColor);
		};
	}
}
#endif
