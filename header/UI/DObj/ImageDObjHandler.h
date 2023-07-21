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
			Media::DrawImage *bmpBuff;
			UInt32 bgColor;

		protected:
			virtual void DrawBkg(Media::DrawImage *dimg);
		public:
			ImageDObjHandler(NotNullPtr<Media::DrawEngine> deng, Text::CString fileName);
			virtual ~ImageDObjHandler();

			void SetBGColor(UInt32 bgColor);
		};
	}
}
#endif
