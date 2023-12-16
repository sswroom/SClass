#ifndef _SM_IO_CAMERACONTROL
#define _SM_IO_CAMERACONTROL
#include "Data/ArrayList.h"
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

		virtual UOSInt GetInfoList(Data::ArrayListStringNN *nameList, Data::ArrayListStringNN *valueList) = 0;
		virtual void FreeInfoList(Data::ArrayListStringNN *nameList, Data::ArrayListStringNN *valueList) = 0;
		virtual UOSInt GetFileList(Data::ArrayList<FileInfo*> *fileList) = 0;
		virtual Bool GetFile(FileInfo *file, IO::Stream *outStm) = 0;
		virtual Bool GetThumbnailFile(FileInfo *file, IO::Stream *outStm) = 0;
	};
}
#endif
