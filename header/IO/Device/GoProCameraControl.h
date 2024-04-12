#ifndef _SM_IO_DEVICE_GOPROCAMERACONTROL
#define _SM_IO_DEVICE_GOPROCAMERACONTROL
#include "Data/ArrayListStringNN.h"
#include "IO/CameraControl.h"
#include "Net/SocketFactory.h"
#include "Net/SocketUtil.h"

namespace IO
{
	namespace Device
	{
		class GoProCameraControl : public CameraControl
		{
		private:
			Net::SocketUtil::AddressInfo addr;
			NotNullPtr<Net::SocketFactory> sockf;
			Data::ArrayListNN<IO::CameraControl::FileInfo> *fileList;

			void GetMediaList();
			Bool GetInfo(NotNullPtr<Data::ArrayListStringNN> nameList, NotNullPtr<Data::ArrayListStringNN> valueList);			
		public:
			GoProCameraControl(NotNullPtr<Net::SocketFactory> sockf, const Net::SocketUtil::AddressInfo *addr);
			virtual ~GoProCameraControl();

			virtual UOSInt GetInfoList(NotNullPtr<Data::ArrayListStringNN> nameList, NotNullPtr<Data::ArrayListStringNN> valueList);
			virtual void FreeInfoList(NotNullPtr<Data::ArrayListStringNN> nameList, NotNullPtr<Data::ArrayListStringNN> valueList);
			virtual UOSInt GetFileList(NotNullPtr<Data::ArrayListNN<FileInfo>> fileList);
			virtual Bool GetFile(NotNullPtr<FileInfo> file, NotNullPtr<IO::Stream> outStm);
			virtual Bool GetThumbnailFile(NotNullPtr<FileInfo> file, NotNullPtr<IO::Stream> outStm);

			static GoProCameraControl *CreateControl(NotNullPtr<Net::SocketFactory> sockf);
		};
	}
}
#endif
