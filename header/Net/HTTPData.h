#ifndef _SM_NET_HTTPDATA
#define _SM_NET_HTTPDATA
#include "AnyType.h"
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
			NN<Text::String> url;
			Text::CString fileName;
			NN<Text::String> localFile;
			UIntOS seekCnt;

			Optional<Net::HTTPClient> cli;
			NN<Net::TCPClientFactory> clif;
			Optional<Net::SSLEngine> ssl;
			Optional<Net::HTTPQueue> queue;
			UInt64 loadSize;
			Bool isLoading;
			Sync::Mutex mut;
			Sync::Event *evtTmp;
			Int32 objectCnt;
		};


	private:
		Optional<HTTPDATAHANDLE> fdh;
		UInt64 dataOffset;
		UInt64 dataLength;

		static UInt32 __stdcall LoadThread(AnyType userObj);
	public:
		HTTPData(NN<const HTTPData> fd, UInt64 offset, UInt64 length);
		HTTPData(NN<Net::TCPClientFactory> clif, Optional<Net::SSLEngine> ssl, Optional<Net::HTTPQueue> queue, Text::CStringNN url, Text::CStringNN localFile, Bool forceReload);
		virtual ~HTTPData();

		virtual UIntOS GetRealData(UInt64 offset, UIntOS length, Data::ByteArray buffer);
		virtual NN<Text::String> GetFullName() const;
		virtual Text::CString GetShortName() const;
		virtual void SetFullName(Text::CStringNN fullName);
		virtual UInt64 GetDataSize() const;
		virtual UnsafeArrayOpt<const UInt8> GetPointer() const;

		virtual NN<IO::StreamData> GetPartialData(UInt64 offset, UInt64 length);
		virtual Bool IsFullFile() const;
		virtual Bool IsLoading() const;
		virtual UIntOS GetSeekCount() const;
	private:
		void Close();
	};
}
#endif
