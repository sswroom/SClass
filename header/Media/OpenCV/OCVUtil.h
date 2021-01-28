#ifndef _SM_MEDIA_OPENCV_OCVUTIL
#define _SM_MEDIA_OPENCV_OCVUTIL

namespace Media
{
	namespace OpenCV
	{
		class OCVUtil
		{
		public:
			static UTF8Char *GetDataPath(UTF8Char *sbuff, const UTF8Char *dataFile);
		};
	}
};
#endif
