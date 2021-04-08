#ifndef _SM_NET_RTPCLICHANNEL
#define _SM_NET_RTPCLICHANNEL
#include "Net/UDPServer.h"
#include "Data/Int32Map.h"
#include "Media/IAudioSource.h"
#include "Media/IMediaSource.h"
#include "Media/IVideoSource.h"
#include "Net/IRTPController.h"
#include "Sync/Mutex.h"

namespace Net
{
	class IRTPPLHandler;

	class RTPCliChannel
	{
	private:
		typedef struct
		{
			Int32 seqNum;
			Int32 payloadType;
			UInt32 ts;
			UInt8 *buff;
			OSInt dataSize;
		} PacketBuff;

		typedef struct
		{
			Int32 useCnt;

			Net::UDPServer *rtpUDP;
			Net::UDPServer *rtcpUDP;
			void *userData;
			const UTF8Char *controlURL;
			Int32 lastSSRC;
			Int32 lastSeqNumHi;
			Int32 lastSeqNumLo;
			Data::Int32Map<Net::IRTPPLHandler *> *payloadMap;
			Media::MediaType mediaType;
			Net::SocketFactory *sockf;

			OSInt threadCnt;
			OSInt buffCnt;

			Sync::Mutex *packMut;
			PacketBuff *packBuff; 
			OSInt packCnt;

			Net::IRTPController *playCtrl;

			Bool playing;
			Bool playToStop;
			Sync::Event *playEvt;
		} ChannelData;

	private:
		ChannelData *chData;

	private:
		static void __stdcall PacketHdlr(const Net::SocketUtil::AddressInfo *addr, UInt16 port, const UInt8 *buff, UOSInt dataSize, void *userData);
		static void __stdcall PacketCtrlHdlr(const Net::SocketUtil::AddressInfo *addr, UInt16 port, const UInt8 *buff, UOSInt dataSize, void *userData);
		static UInt32 __stdcall PlayThread(void *userObj);

	private:
		void SetControlURL(const UTF8Char *url);
		void SetPlayControl(Net::IRTPController *playCtrl);

		RTPCliChannel(Net::SocketFactory *sockf, UInt16 port);
		RTPCliChannel(RTPCliChannel *ch);
	public:
		~RTPCliChannel();

		UInt16 GetPort();
		UTF8Char *GetTransportDesc(UTF8Char *sbuff);
		const UTF8Char *GetControlURL();
		Media::MediaType GetMediaType();
		void SetMediaType(Media::MediaType mediaType);
		Media::IVideoSource *GetVideo(OSInt index);
		Media::IAudioSource *GetAudio(OSInt index);
		Media::IVideoSource *CreateShadowVideo(OSInt index);
		Media::IAudioSource *CreateShadowAudio(OSInt index);

		void *GetUserData();
		void SetUserData(void *userData);

		Bool StartPlay();
		Bool StopPlay();
		Bool IsRunning();

		Bool MapPayloadType(Int32 payloadType, const UTF8Char *typ, Int32 freq, Int32 nChannel);
		Bool SetPayloadFormat(Int32 paylodType, const UTF8Char *format);

		static RTPCliChannel *CreateChannel(Net::SocketFactory *sockf, Data::ArrayList<const UTF8Char *> *sdpDesc, const UTF8Char *ctrlURL, Net::IRTPController *playCtrl);
	};
};
#endif
