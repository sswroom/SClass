#ifndef _SM_NET_RTPAUDIOSESSION
#define _SM_NET_RTPAUDIOSESSION
#include "Net/UDPServer.h"
#include "Media/IAudioSource.h"

namespace Net
{
	class RTPAudioSession : public Media::IAudioSource
	{
	public:
		typedef enum
		{
			RTPAFMT_UNKNOWN,
			RTPAFMT_PCMU
		} RTPAudioFormat;
	private:
		Net::SocketFactory *sockf;
		Net::UDPServer *svr;
		IO::LogTool *log;
		Media::AudioFormat *format;
		Bool started;
		Sync::Mutex *readMut;
		Sync::Event *readEvt;
		UInt8 *readBuff;
		OSInt readBuffSize;
		Sync::Event *readBuffEvt;
		OSInt sizeRead;
		UInt32 lastSSRC;

		Int32 fmtFreq;
		RTPAudioFormat afmt;

		Bool outRunning;
		Bool outToStop;
		Media::IAudioSource *outAudio;
		UInt32 outIP;
		UInt32 outPort;
		Int32 outSSRC;

	private:
		static void __stdcall UDPData(UInt32 ip, UInt16 port, UInt8 *buff, OSInt dataSize, void *userData);
		static UInt32 __stdcall SendThread(void *userObj);
	public:
		RTPAudioSession(Net::SocketFactory *sockf, const Char *ip, UInt16 port, IO::LogTool *log);
		virtual ~RTPAudioSession();

		Bool IsError();
		void SetAudioFormat(RTPAudioFormat afmt, Int32 frequency);
		Bool StartSend(Media::IAudioSource *audSrc, UInt32 destIP, UInt16 destPort, Int32 outSSRC);
		void StopSend();

		virtual WChar *GetName(WChar *buff);
		virtual Bool CanSeek();
		virtual Int32 GetStreamTime(); //ms
		virtual void GetFormat(Media::AudioFormat *format);

		virtual Int32 SeekToTime(Int32 time); //ms, ret actual time
		virtual Bool Start(Sync::Event *evt, Int32 blkSize);
		virtual void Stop();
		virtual OSInt ReadBlock(UInt8 *buff, OSInt blkSize); //ret actual block size
		virtual OSInt GetMinBlockSize();
	};
};
#endif
