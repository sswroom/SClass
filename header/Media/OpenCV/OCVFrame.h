#ifndef _SM_MEDIA_OPENCV_OCVFRAME
#define _SM_MEDIA_OPENCV_OCVFRAME

namespace Media
{
	namespace OpenCV
	{
		class OCVFrame
		{
		private:
			void *frame;

		public:
			OCVFrame(void *frame);
			~OCVFrame();

			void *GetFrame();

			static OCVFrame *CreateYFrame(UInt8 **imgData, UOSInt dataSize, UInt32 fourcc, UOSInt dispWidth, UOSInt dispHeight, UOSInt storeWidth, UOSInt storeBPP, Media::PixelFormat pf);
		};
	}
}
#endif
