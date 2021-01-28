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

		protected:
			virtual void DrawBkg(Media::DrawImage *dimg);
		public:
			ImageDObjHandler(Media::DrawEngine *deng, const WChar *fileName);
			virtual ~ImageDObjHandler();
		};
	}
}
#endif
