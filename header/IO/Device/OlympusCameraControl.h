#ifndef _SM_IO_DEVICE_OLYMPUSCAMERACONTROL
#define _SM_IO_DEVICE_OLYMPUSCAMERACONTROL
#include "Data/ArrayListStrUTF8.h"
#include "IO/CameraControl.h"
#include "IO/Stream.h"
#include "Net/SocketFactory.h"
#include "Net/SocketUtil.h"
#include "Text/EncodingFactory.h"
#include "Text/StringBuilderUTF.h"

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
			const UTF8Char *oiVersion;
			const UTF8Char *oiTrackVersion;
			Data::ArrayListStrUTF8 *cmdList;
			Data::ArrayList<IO::CameraControl::FileInfo*> *fileList;

			void GetCommandList();
			void GetImageList();
			void GetGPSLogList();
			void GetSNSLogList();
		public:
			OlympusCameraControl(Net::SocketFactory *sockf, Text::EncodingFactory *encFact, const Net::SocketUtil::AddressInfo *addr);
			virtual ~OlympusCameraControl();

			virtual OSInt GetInfoList(Data::ArrayList<const UTF8Char*> *nameList, Data::ArrayList<const UTF8Char*> *valueList);
			virtual void FreeInfoList(Data::ArrayList<const UTF8Char*> *nameList, Data::ArrayList<const UTF8Char*> *valueList);
			virtual OSInt GetFileList(Data::ArrayList<FileInfo*> *fileList);
			virtual Bool GetFile(FileInfo *file, IO::Stream *outStm);
			virtual Bool GetThumbnailFile(FileInfo *file, IO::Stream *outStm);

			const UTF8Char *GetOIVersion();
			const UTF8Char *GetOITrackVersion();
			Bool GetModel(Text::StringBuilderUTF *sb);
			
			static OlympusCameraControl *CreateControl(Net::SocketFactory *sockf, Text::EncodingFactory *encFact);
		};
	}
}
#endif
