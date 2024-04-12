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
			NotNullPtr<Net::SocketFactory> sockf;
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
			OlympusCameraControl(NotNullPtr<Net::SocketFactory> sockf, Optional<Text::EncodingFactory> encFact, const Net::SocketUtil::AddressInfo *addr);
			virtual ~OlympusCameraControl();

			virtual UOSInt GetInfoList(NotNullPtr<Data::ArrayListStringNN> nameList, NotNullPtr<Data::ArrayListStringNN> valueList);
			virtual void FreeInfoList(NotNullPtr<Data::ArrayListStringNN> nameList, NotNullPtr<Data::ArrayListStringNN> valueList);
			virtual UOSInt GetFileList(NotNullPtr<Data::ArrayListNN<FileInfo>> fileList);
			virtual Bool GetFile(NotNullPtr<FileInfo> file, NotNullPtr<IO::Stream> outStm);
			virtual Bool GetThumbnailFile(NotNullPtr<FileInfo> file, NotNullPtr<IO::Stream> outStm);

			Text::String *GetOIVersion();
			Text::String *GetOITrackVersion();
			Bool GetModel(NotNullPtr<Text::StringBuilderUTF8> sb);
			
			static OlympusCameraControl *CreateControl(NotNullPtr<Net::SocketFactory> sockf, Optional<Text::EncodingFactory> encFact);
		};
	}
}
#endif
