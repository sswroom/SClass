#ifndef _SM_IO_DEVICE_OLYMPUSCAMERACONTROL
#define _SM_IO_DEVICE_OLYMPUSCAMERACONTROL
#include "Data/ArrayListStringNN.h"
#include "IO/CameraControl.h"
#include "IO/Stream.h"
#include "Net/SocketUtil.h"
#include "Net/TCPClientFactory.h"
#include "Text/EncodingFactory.h"
#include "Text/StringBuilderUTF8.h"

namespace IO
{
	namespace Device
	{
		class OlympusCameraControl : public CameraControl
		{
		private:
			Net::SocketUtil::AddressInfo addr;
			NN<Net::TCPClientFactory> clif;
			Optional<Text::EncodingFactory> encFact;
			Optional<Text::String> oiVersion;
			Optional<Text::String> oiTrackVersion;
			Data::ArrayListStringNN cmdList;
			Optional<Data::ArrayListNN<IO::CameraControl::FileInfo>> fileList;

			void GetCommandList();
			void GetImageList();
			void GetGPSLogList();
			void GetSNSLogList();
		public:
			OlympusCameraControl(NN<Net::TCPClientFactory> clif, Optional<Text::EncodingFactory> encFact, NN<const Net::SocketUtil::AddressInfo> addr);
			virtual ~OlympusCameraControl();

			virtual UOSInt GetInfoList(NN<Data::ArrayListStringNN> nameList, NN<Data::ArrayListStringNN> valueList);
			virtual void FreeInfoList(NN<Data::ArrayListStringNN> nameList, NN<Data::ArrayListStringNN> valueList);
			virtual UOSInt GetFileList(NN<Data::ArrayListNN<FileInfo>> fileList);
			virtual Bool GetFile(NN<FileInfo> file, NN<IO::Stream> outStm);
			virtual Bool GetThumbnailFile(NN<FileInfo> file, NN<IO::Stream> outStm);

			Optional<Text::String> GetOIVersion();
			Optional<Text::String> GetOITrackVersion();
			Bool GetModel(NN<Text::StringBuilderUTF8> sb);
			
			static Optional<OlympusCameraControl> CreateControl(NN<Net::TCPClientFactory> clif, Optional<Text::EncodingFactory> encFact);
		};
	}
}
#endif
