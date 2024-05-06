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
			UTF8Char fileName2[20];
			UOSInt fileNameLen;
			UTF8Char filePath2[64];
			UOSInt filePathLen;
			UInt64 fileSize;
			Int64 fileTimeTicks;
			FileType fileType;
		} FileInfo;
	
	public:
		CameraControl() {};
		virtual ~CameraControl() {};

		virtual UOSInt GetInfoList(NN<Data::ArrayListStringNN> nameList, NN<Data::ArrayListStringNN> valueList) = 0;
		virtual void FreeInfoList(NN<Data::ArrayListStringNN> nameList, NN<Data::ArrayListStringNN> valueList) = 0;
		virtual UOSInt GetFileList(NN<Data::ArrayListNN<FileInfo>> fileList) = 0;
		virtual Bool GetFile(NN<FileInfo> file, NN<IO::Stream> outStm) = 0;
		virtual Bool GetThumbnailFile(NN<FileInfo> file, NN<IO::Stream> outStm) = 0;
	};
}
#endif
