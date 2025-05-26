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
			Optional<Media::DrawImage> bmpBkg;
			Optional<Media::DrawImage> bmpBuff;
			UInt32 bgColor;
			Optional<Media::ColorSess> colorSess;

		protected:
			virtual void DrawBkg(NN<Media::DrawImage> dimg);
		public:
			ImageDObjHandler(NN<Media::DrawEngine> deng, Text::CStringNN fileName, Optional<Media::ColorSess> colorSess);
			virtual ~ImageDObjHandler();

			void SetBGColor(UInt32 bgColor);
		};
	}
}
#endif
