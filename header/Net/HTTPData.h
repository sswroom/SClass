#ifndef _SM_NET_HTTPDATA
#define _SM_NET_HTTPDATA
#include "IO/IStreamData.h"
#include "IO/FileStream.h"
#include "Net/HTTPClient.h"
#include "Net/HTTPQueue.h"
#include "Sync/Mutex.h"
#include "Sync/Event.h"

namespace Net
{
	class HTTPData : public IO::IStreamData
	{
	private:
		typedef struct
		{
			IO::FileStream *file;
			UInt64 fileLength;
			UInt64 currentOffset;
			Text::String *url;
			const UTF8Char *fileName;
			Text::String *localFile;
			UOSInt seekCnt;

			Net::HTTPClient *cli;
			Net::SocketFactory *sockf;
			Net::SSLEngine *ssl;
			Net::HTTPQueue *queue;
			UInt64 loadSize;
			Bool isLoading;
			Sync::Mutex *mut;
			Sync::Event *evtTmp;
			Int32 objectCnt;
		} HTTPDATAHANDLE;


	private:
		HTTPDATAHANDLE *fdh;
		UInt64 dataOffset;
		UInt64 dataLength;

		static UInt32 __stdcall LoadThread(void *userObj);
	public:
		HTTPData(const HTTPData *fd, UInt64 offset, UInt64 length);
		HTTPData(Net::SocketFactory *sockf, Net::SSLEngine *ssl, Net::HTTPQueue *queue, const UTF8Char *url, const UTF8Char *localFile, Bool forceReload);
		virtual ~HTTPData();

		virtual UOSInt GetRealData(UInt64 offset, UOSInt length, UInt8 *buffer);
		virtual Text::String *GetFullName();
		virtual const UTF8Char *GetShortName();
		virtual void SetFullName(const UTF8Char *fullName);
		virtual UInt64 GetDataSize();
		virtual const UInt8 *GetPointer();

		virtual IO::IStreamData *GetPartialData(UInt64 offset, UInt64 length);
		virtual Bool IsFullFile();
		virtual Bool IsLoading();
		virtual UOSInt GetSeekCount();
	private:
		void Close();
	};
}
#endif
