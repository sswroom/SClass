#ifndef _SM_IO_CAMERACONTROL
#define _SM_IO_CAMERACONTROL
#include "Data/ArrayListNN.h"
#include "IO/Stream.h"
#include "Text/StringBuilderUTF8.h"

namespace IO
{
	class CameraControl
	{
	public:
		typedef enum
		{
			FT_IMAGE,
			FT_MOVIE,
			FT_GPSLOG,
			FT_SENSORLOG
		} FileType;

		typedef struct
		{
			UTF8Char fileName[20];
			UTF8Char filePath[64];
			UInt64 fileSize;
			Int64 fileTimeTicks;
			FileType fileType;
		} FileInfo;
	
	public:
		CameraControl() {};
		virtual ~CameraControl() {};

		virtual UOSInt GetInfoList(NotNullPtr<Data::ArrayListStringNN> nameList, NotNullPtr<Data::ArrayListStringNN> valueList) = 0;
		virtual void FreeInfoList(NotNullPtr<Data::ArrayListStringNN> nameList, NotNullPtr<Data::ArrayListStringNN> valueList) = 0;
		virtual UOSInt GetFileList(NotNullPtr<Data::ArrayListNN<FileInfo>> fileList) = 0;
		virtual Bool GetFile(NotNullPtr<FileInfo> file, NotNullPtr<IO::Stream> outStm) = 0;
		virtual Bool GetThumbnailFile(NotNullPtr<FileInfo> file, NotNullPtr<IO::Stream> outStm) = 0;
	};
}
#endif
