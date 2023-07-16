#ifndef _SM_NET_HTTPDATA
#define _SM_NET_HTTPDATA
#include "IO/StreamData.h"
#include "IO/FileStream.h"
#include "Net/HTTPClient.h"
#include "Net/HTTPQueue.h"
#include "Sync/Mutex.h"
#include "Sync/Event.h"

namespace Net
{
	class HTTPData : public IO::StreamData
	{
	private:
		struct HTTPDATAHANDLE
		{
			IO::FileStream *file;
			UInt64 fileLength;
			UInt64 currentOffset;
			NotNullPtr<Text::String> url;
			Text::CString fileName;
			NotNullPtr<Text::String> localFile;
			UOSInt seekCnt;

			Net::HTTPClient *cli;
			Net::SocketFactory *sockf;
			Net::SSLEngine *ssl;
			Net::HTTPQueue *queue;
			UInt64 loadSize;
			Bool isLoading;
			Sync::Mutex mut;
			Sync::Event *evtTmp;
			Int32 objectCnt;
		};


	private:
		HTTPDATAHANDLE *fdh;
		UInt64 dataOffset;
		UInt64 dataLength;

		static UInt32 __stdcall LoadThread(void *userObj);
	public:
		HTTPData(const HTTPData *fd, UInt64 offset, UInt64 length);
		HTTPData(Net::SocketFactory *sockf, Net::SSLEngine *ssl, Net::HTTPQueue *queue, Text::CString url, Text::CString localFile, Bool forceReload);
		virtual ~HTTPData();

		virtual UOSInt GetRealData(UInt64 offset, UOSInt length, Data::ByteArray buffer);
		virtual NotNullPtr<Text::String> GetFullName();
		virtual Text::CString GetShortName();
		virtual void SetFullName(Text::CString fullName);
		virtual UInt64 GetDataSize();
		virtual const UInt8 *GetPointer();

		virtual IO::StreamData *GetPartialData(UInt64 offset, UInt64 length);
		virtual Bool IsFullFile();
		virtual Bool IsLoading();
		virtual UOSInt GetSeekCount();
	private:
		void Close();
	};
}
#endif
