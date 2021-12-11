#ifndef _SM_IO_DEVICE_GOPROCAMERACONTROL
#define _SM_IO_DEVICE_GOPROCAMERACONTROL
#include "IO/CameraControl.h"
#include "Net/SocketFactory.h"
#include "Net/SocketUtil.h"
#include "Text/StringBuilderUTF.h"

namespace IO
{
	namespace Device
	{
		class GoProCameraControl : public CameraControl
		{
		private:
			Net::SocketUtil::AddressInfo addr;
			Net::SocketFactory *sockf;
			Data::ArrayList<IO::CameraControl::FileInfo*> *fileList;

			void GetMediaList();
			Bool GetInfo(Data::ArrayList<Text::String*> *nameList, Data::ArrayList<Text::String*> *valueList);			
		public:
			GoProCameraControl(Net::SocketFactory *sockf, const Net::SocketUtil::AddressInfo *addr);
			virtual ~GoProCameraControl();

			virtual UOSInt GetInfoList(Data::ArrayList<Text::String*> *nameList, Data::ArrayList<Text::String*> *valueList);
			virtual void FreeInfoList(Data::ArrayList<Text::String*> *nameList, Data::ArrayList<Text::String*> *valueList);
			virtual UOSInt GetFileList(Data::ArrayList<FileInfo*> *fileList);
			virtual Bool GetFile(FileInfo *file, IO::Stream *outStm);
			virtual Bool GetThumbnailFile(FileInfo *file, IO::Stream *outStm);

			static GoProCameraControl *CreateControl(Net::SocketFactory *sockf);
		};
	}
}
#endif
