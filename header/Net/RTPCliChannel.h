#ifndef _SM_NET_RTPCLICHANNEL
#define _SM_NET_RTPCLICHANNEL
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
			void *userData;
			Text::String *controlURL;
			UInt32 lastSSRC;
			UInt32 lastSeqNumHi;
			UInt32 lastSeqNumLo;
			Data::FastMap<Int32, Net::IRTPPLHandler *> payloadMap;
			Media::MediaType mediaType;
			NotNullPtr<Net::SocketFactory> sockf;

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
		static void __stdcall PacketHdlr(const Net::SocketUtil::AddressInfo *addr, UInt16 port, const UInt8 *buff, UOSInt dataSize, void *userData);
		static void __stdcall PacketCtrlHdlr(const Net::SocketUtil::AddressInfo *addr, UInt16 port, const UInt8 *buff, UOSInt dataSize, void *userData);
		static UInt32 __stdcall PlayThread(void *userObj);

	private:
		void SetControlURL(Text::CString url);
		void SetPlayControl(Net::IRTPController *playCtrl);

		RTPCliChannel(NotNullPtr<Net::SocketFactory> sockf, UInt16 port);
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

		void *GetUserData();
		void SetUserData(void *userData);

		Bool StartPlay();
		Bool StopPlay();
		Bool IsRunning();

		Bool MapPayloadType(Int32 payloadType, Text::CString typ, UInt32 freq, UInt32 nChannel);
		Bool SetPayloadFormat(Int32 paylodType, const UTF8Char *format);

		static RTPCliChannel *CreateChannel(NotNullPtr<Net::SocketFactory> sockf, Data::ArrayList<const UTF8Char *> *sdpDesc, Text::CString ctrlURL, Net::IRTPController *playCtrl);
	};
}
#endif
