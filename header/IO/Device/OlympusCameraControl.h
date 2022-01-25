#ifndef _SM_IO_DEVICE_OLYMPUSCAMERACONTROL
#define _SM_IO_DEVICE_OLYMPUSCAMERACONTROL
#include "Data/ArrayListString.h"
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
			Net::SocketFactory *sockf;
			Text::EncodingFactory *encFact;
			Text::String *oiVersion;
			Text::String *oiTrackVersion;
			Data::ArrayListString *cmdList;
			Data::ArrayList<IO::CameraControl::FileInfo*> *fileList;

			void GetCommandList();
			void GetImageList();
			void GetGPSLogList();
			void GetSNSLogList();
		public:
			OlympusCameraControl(Net::SocketFactory *sockf, Text::EncodingFactory *encFact, const Net::SocketUtil::AddressInfo *addr);
			virtual ~OlympusCameraControl();

			virtual UOSInt GetInfoList(Data::ArrayList<Text::String*> *nameList, Data::ArrayList<Text::String*> *valueList);
			virtual void FreeInfoList(Data::ArrayList<Text::String*> *nameList, Data::ArrayList<Text::String*> *valueList);
			virtual UOSInt GetFileList(Data::ArrayList<FileInfo*> *fileList);
			virtual Bool GetFile(FileInfo *file, IO::Stream *outStm);
			virtual Bool GetThumbnailFile(FileInfo *file, IO::Stream *outStm);

			Text::String *GetOIVersion();
			Text::String *GetOITrackVersion();
			Bool GetModel(Text::StringBuilderUTF8 *sb);
			
			static OlympusCameraControl *CreateControl(Net::SocketFactory *sockf, Text::EncodingFactory *encFact);
		};
	}
}
#endif
