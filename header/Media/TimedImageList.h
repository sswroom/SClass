#ifndef _SM_MEDIA_TIMEDIMAGELIST
#define _SM_MEDIA_TIMEDIMAGELIST
#include "IO/FileStream.h"
#include "IO/MemoryStream.h"
#include "Media/ImageList.h"

namespace Media
{
	class TimedImageList
	{
	public:
		typedef enum
		{
			IF_JPG
		} ImageFormat;
	private:
		typedef struct
		{
			Int64 imageTimeTicks;
			Int32 flags;
			Int32 reserved;
			UInt64 fileOfst;
			UInt64 fileSize;
		} ImageInfo;

		IO::FileStream *fs;
		IO::MemoryStream indexStm;
		UInt64 currFileOfst;
		Int32 flags;
		Bool changed;

		void ScanFile();
	public:
		TimedImageList(Text::CString fileName);
		~TimedImageList();

		Bool IsError();

		Bool AddImage(Int64 captureTimeTicks, const UInt8 *imgBuff, UOSInt imgSize, ImageFormat imgFmt);
	};
}
#endif
