#ifndef _SM_NET_RTPCLICHANNEL
#define _SM_NET_RTPCLICHANNEL
#include "AnyType.h"
#include "Data/FastMap.h"
#include "Media/IAudioSource.h"
#include "Media/IMediaSource.h"
#include "Media/IVideoSource.h"
#include "Net/IRTPController.h"
#include "Net/UDPServer.h"
#include "Sync/Mutex.h"

namespace Net
{
	class IRTPPLHandler;

	class RTPCliChannel
	{
	private:
		typedef struct
		{
			UInt32 seqNum;
			Int32 payloadType;
			UInt32 ts;
			UInt8 *buff;
			UOSInt dataSize;
		} PacketBuff;

		struct ChannelData
		{
			Int32 useCnt;

			Net::UDPServer *rtpUDP;
			Net::UDPServer *rtcpUDP;
			AnyType userData;
			Text::String *controlURL;
			UInt32 lastSSRC;
			UInt32 lastSeqNumHi;
			UInt32 lastSeqNumLo;
			Data::FastMap<Int32, Net::IRTPPLHandler *> payloadMap;
			Media::MediaType mediaType;
			NN<Net::SocketFactory> sockf;

			UOSInt threadCnt;
			UOSInt buffCnt;

			Sync::Mutex packMut;
			PacketBuff *packBuff; 
			UOSInt packCnt;

			Net::IRTPController *playCtrl;

			Bool playing;
			Bool playToStop;
			Sync::Event playEvt;
		};

	private:
		ChannelData *chData;

	private:
		static void __stdcall PacketHdlr(NN<const Net::SocketUtil::AddressInfo> addr, UInt16 port, const UInt8 *buff, UOSInt dataSize, AnyType userData);
		static void __stdcall PacketCtrlHdlr(NN<const Net::SocketUtil::AddressInfo> addr, UInt16 port, const UInt8 *buff, UOSInt dataSize, AnyType userData);
		static UInt32 __stdcall PlayThread(AnyType userObj);

	private:
		void SetControlURL(Text::CString url);
		void SetPlayControl(Net::IRTPController *playCtrl);

		RTPCliChannel(NN<Net::SocketFactory> sockf, UInt16 port, NN<IO::LogTool> log);
		RTPCliChannel(RTPCliChannel *ch);
	public:
		~RTPCliChannel();

		UInt16 GetPort();
		UTF8Char *GetTransportDesc(UTF8Char *sbuff);
		Text::String *GetControlURL();
		Media::MediaType GetMediaType();
		void SetMediaType(Media::MediaType mediaType);
		Media::IVideoSource *GetVideo(UOSInt index);
		Media::IAudioSource *GetAudio(UOSInt index);
		Media::IVideoSource *CreateShadowVideo(UOSInt index);
		Media::IAudioSource *CreateShadowAudio(UOSInt index);

		AnyType GetUserData();
		void SetUserData(AnyType userData);

		Bool StartPlay();
		Bool StopPlay();
		Bool IsRunning();

		Bool MapPayloadType(Int32 payloadType, Text::CString typ, UInt32 freq, UInt32 nChannel);
		Bool SetPayloadFormat(Int32 paylodType, const UTF8Char *format);

		static RTPCliChannel *CreateChannel(NN<Net::SocketFactory> sockf, Data::ArrayList<const UTF8Char *> *sdpDesc, Text::CString ctrlURL, Net::IRTPController *playCtrl, NN<IO::LogTool> log);
	};
}
#endif
