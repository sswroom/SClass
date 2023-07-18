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
			Text::EncodingFactory *encFact;
			Text::String *oiVersion;
			Text::String *oiTrackVersion;
			Data::ArrayListStringNN cmdList;
			Data::ArrayList<IO::CameraControl::FileInfo*> *fileList;

			void GetCommandList();
			void GetImageList();
			void GetGPSLogList();
			void GetSNSLogList();
		public:
			OlympusCameraControl(NotNullPtr<Net::SocketFactory> sockf, Text::EncodingFactory *encFact, const Net::SocketUtil::AddressInfo *addr);
			virtual ~OlympusCameraControl();

			virtual UOSInt GetInfoList(Data::ArrayListNN<Text::String> *nameList, Data::ArrayListNN<Text::String> *valueList);
			virtual void FreeInfoList(Data::ArrayListNN<Text::String> *nameList, Data::ArrayListNN<Text::String> *valueList);
			virtual UOSInt GetFileList(Data::ArrayList<FileInfo*> *fileList);
			virtual Bool GetFile(FileInfo *file, IO::Stream *outStm);
			virtual Bool GetThumbnailFile(FileInfo *file, IO::Stream *outStm);

			Text::String *GetOIVersion();
			Text::String *GetOITrackVersion();
			Bool GetModel(Text::StringBuilderUTF8 *sb);
			
			static OlympusCameraControl *CreateControl(NotNullPtr<Net::SocketFactory> sockf, Text::EncodingFactory *encFact);
		};
	}
}
#endif
