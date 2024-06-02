#ifndef _SM_MEDIA_OPENCV_OCVUTIL
#define _SM_MEDIA_OPENCV_OCVUTIL

namespace Media
{
	namespace OpenCV
	{
		class OCVUtil
		{
		public:
			static UnsafeArrayOpt<UTF8Char> GetDataPath(UnsafeArray<UTF8Char> sbuff, UnsafeArray<const UTF8Char> dataFile);
		};
	}
};
#endif
