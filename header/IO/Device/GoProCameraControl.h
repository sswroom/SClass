#ifndef _SM_IO_DEVICE_GOPROCAMERACONTROL
#define _SM_IO_DEVICE_GOPROCAMERACONTROL
#include "Data/ArrayListStringNN.h"
#include "IO/CameraControl.h"
#include "Net/SocketUtil.h"
#include "Net/TCPClientFactory.h"

namespace IO
{
	namespace Device
	{
		class GoProCameraControl : public CameraControl
		{
		private:
			Net::SocketUtil::AddressInfo addr;
			NN<Net::TCPClientFactory> clif;
			Data::ArrayListNN<IO::CameraControl::FileInfo> *fileList;

			void GetMediaList();
			Bool GetInfo(NN<Data::ArrayListStringNN> nameList, NN<Data::ArrayListStringNN> valueList);			
		public:
			GoProCameraControl(NN<Net::TCPClientFactory> clif, const Net::SocketUtil::AddressInfo *addr);
			virtual ~GoProCameraControl();

			virtual UOSInt GetInfoList(NN<Data::ArrayListStringNN> nameList, NN<Data::ArrayListStringNN> valueList);
			virtual void FreeInfoList(NN<Data::ArrayListStringNN> nameList, NN<Data::ArrayListStringNN> valueList);
			virtual UOSInt GetFileList(NN<Data::ArrayListNN<FileInfo>> fileList);
			virtual Bool GetFile(NN<FileInfo> file, NN<IO::Stream> outStm);
			virtual Bool GetThumbnailFile(NN<FileInfo> file, NN<IO::Stream> outStm);

			static Optional<GoProCameraControl> CreateControl(NN<Net::TCPClientFactory> clif);
		};
	}
}
#endif
