#ifndef _SM_NET_RTPAUDIOSESSION
#define _SM_NET_RTPAUDIOSESSION
#include "Net/UDPServer.h"
#include "Media/AudioSource.h"

namespace Net
{
	class RTPAudioSession : public Media::AudioSource
	{
	public:
		typedef enum
		{
			RTPAFMT_UNKNOWN,
			RTPAFMT_PCMU
		} RTPAudioFormat;
	private:
		NN<Net::SocketFactory> sockf;
		Optional<Net::UDPServer> svr;
		NN<IO::LogTool> log;
		Media::AudioFormat *format;
		Bool started;
		Sync::Mutex readMut;
		Sync::Event *readEvt;
		UInt8 *readBuff;
		UIntOS readBuffSize;
		Sync::Event readBuffEvt;
		UIntOS sizeRead;
		UInt32 lastSSRC;

		Int32 fmtFreq;
		RTPAudioFormat afmt;

		Bool outRunning;
		Bool outToStop;
		Media::AudioSource *outAudio;
		Net::SocketUtil::AddressInfo outAddr;
		UInt32 outPort;
		Int32 outSSRC;

	private:
		static void __stdcall UDPData(NN<const Net::SocketUtil::AddressInfo> addr, UInt16 port, Data::ByteArrayR data, AnyType userData);
		static UInt32 __stdcall SendThread(AnyType userObj);
	public:
		RTPAudioSession(NN<Net::SocketFactory> sockf, const Char *ip, UInt16 port, NN<IO::LogTool> log);
		virtual ~RTPAudioSession();

		Bool IsError();
		void SetAudioFormat(RTPAudioFormat afmt, Int32 frequency);
		Bool StartSend(Media::AudioSource *audSrc, UInt32 destIP, UInt16 destPort, Int32 outSSRC);
		void StopSend();

		virtual UnsafeArrayOpt<UTF8Char> GetSourceName(UnsafeArray<UTF8Char> buff);
		virtual Bool CanSeek();
		virtual Data::Duration GetStreamTime();
		virtual void GetFormat(Media::AudioFormat *format);

		virtual Data::Duration SeekToTime(Data::Duration time);
		virtual Bool Start(Sync::Event *evt, Int32 blkSize);
		virtual void Stop();
		virtual UIntOS ReadBlock(UInt8 *buff, UIntOS blkSize); //ret actual block size
		virtual UIntOS GetMinBlockSize();
	};
};
#endif
