#ifndef _SM_IO_DEVICE_OLYMPUSCAMERACONTROL
#define _SM_IO_DEVICE_OLYMPUSCAMERACONTROL
#include "Data/ArrayListStringNN.h"
#include "IO/CameraControl.h"
#include "IO/Stream.h"
#include "Net/SocketFactory.h"
#include "Net/SocketUtil.h"
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
			NN<Net::SocketFactory> sockf;
			Optional<Text::EncodingFactory> encFact;
			Text::String *oiVersion;
			Text::String *oiTrackVersion;
			Data::ArrayListStringNN cmdList;
			Data::ArrayListNN<IO::CameraControl::FileInfo> *fileList;

			void GetCommandList();
			void GetImageList();
			void GetGPSLogList();
			void GetSNSLogList();
		public:
			OlympusCameraControl(NN<Net::SocketFactory> sockf, Optional<Text::EncodingFactory> encFact, const Net::SocketUtil::AddressInfo *addr);
			virtual ~OlympusCameraControl();

			virtual UOSInt GetInfoList(NN<Data::ArrayListStringNN> nameList, NN<Data::ArrayListStringNN> valueList);
			virtual void FreeInfoList(NN<Data::ArrayListStringNN> nameList, NN<Data::ArrayListStringNN> valueList);
			virtual UOSInt GetFileList(NN<Data::ArrayListNN<FileInfo>> fileList);
			virtual Bool GetFile(NN<FileInfo> file, NN<IO::Stream> outStm);
			virtual Bool GetThumbnailFile(NN<FileInfo> file, NN<IO::Stream> outStm);

			Text::String *GetOIVersion();
			Text::String *GetOITrackVersion();
			Bool GetModel(NN<Text::StringBuilderUTF8> sb);
			
			static OlympusCameraControl *CreateControl(NN<Net::SocketFactory> sockf, Optional<Text::EncodingFactory> encFact);
		};
	}
}
#endif
