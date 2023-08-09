#ifndef _SM_IO_DEVICE_SIM7000
#define _SM_IO_DEVICE_SIM7000
#include "IO/GSMModemController.h"
#include "Net/SocketUtil.h"
#include "Text/StringBuilderUTF8.h"

namespace IO
{
	namespace Device
	{
		class SIM7000 : public IO::GSMModemController
		{
		public:
			typedef void (__stdcall *ReceiveHandler)(void *userObj, UOSInt index, UInt32 ip, UInt16 port, const UInt8 *buff, UOSInt buffSize);
		private:
			Sync::Event respEvt;
			Sync::Mutex dnsMut;
			Text::CString dnsReq;
			Net::SocketUtil::AddressInfo *dnsResp;
			Bool dnsResult;

			UOSInt connInd;
			Int32 connResult;
			Bool nextReceive;
			UOSInt recvIndex;
			UOSInt recvSize;
			UInt32 recvIP;
			UInt16 recvPort;
			ReceiveHandler recvHdlr;
			void *recvHdlrObj;

			static Bool __stdcall CheckATCommand(void *userObj, const UTF8Char *cmd, UOSInt cmdLen);
		public:
			SIM7000(IO::ATCommandChannel *channel, Bool needRelease);
			virtual ~SIM7000();

			void SetReceiveHandler(ReceiveHandler recvHdlr, void *userObj);

			Bool SIMCOMPowerDown(); //AT+CPOWD
			Bool SIMCOMReadADC(OutParam<Int32> adc); //AT+CADC
			//AT+CFGRI
			//AT+CLTS
			//AT+CBAND
			//AT+CNBP
			//AT+CNSMOD
			//AT+CSCLK
			UTF8Char *SIMCOMGetICCID(UTF8Char *ccid); //AT+CCID
			Bool SIMCOMGetFlashDeviceType(NotNullPtr<Text::StringBuilderUTF8> sb); //AT+CDEVICE
			Bool SIMCOMGetDeviceProductID(NotNullPtr<Text::StringBuilderUTF8> sb); //AT+GSV
			//AT+SGPIO
			//AT+SLEDS
			//AT+CNETLIGHT
			//AT+CSGS
			//AT+CGPIO
			//AT+CBATCHK
			//AT+CNMP
			//AT+CMNB
			//AT+CPSMS
			//AT+CEDRXS
			UTF8Char *SIMCOMGetUESysInfo(UTF8Char *buff); //AT+CPSI
			UTF8Char *SIMCOMGetNetworkAPN(UTF8Char *apn); //AT+CGNAPN
			//AT+CSDP
			//AT+MCELLLOCK
			//AT+NCELLLOCK
			//AT+NBSC
			//AT+CAPNMODE
			//AT+CRRCSTATE
			//AT+CBANDCFG
			//AT+CNACT
			//AT+CEDUMP
			//AT+CNBS
			//AT+CNDS
			//AT+CENG

			Bool NetSetMultiIP(Bool multiIP); //AT+CIPMUX
			Bool NetIPStartTCP(UOSInt index, UInt32 ip, UInt16 port); //AT+CIPSTART
			Bool NetIPStartUDP(UOSInt index, UInt32 ip, UInt16 port); //AT+CIPSTART
			Bool NetIPSend(UOSInt index, const UInt8 *buff, UOSInt buffSize); //AT+CIPSEND
			//AT+CIPQSEND
			//AT+CIPACK
			Bool NetCloseSocket(UOSInt index); //AT+CIPCLOSE
			//AT+CIPSHUT
			Bool NetSetLocalPortTCP(UOSInt index, UInt16 port); //AT+CLPORT
			Bool NetSetLocalPortUDP(UOSInt index, UInt16 port); //AT+CLPORT
			Bool NetSetAPN(Text::CString apn); //AT+CSTT
			Bool NetDataStart(); //AT+CIICR
			UTF8Char *NetGetIFAddr(UTF8Char *addr); //AT+CIFSR
			//AT+CIFSREX
			//AT+CIPSTATUS
			Bool NetGetDNSList(Data::ArrayList<UInt32> *dnsList); //AT+CDNSCFG
			Bool NetDNSResolveIP(Text::CString domain, Net::SocketUtil::AddressInfo *addr); //AT+CDNSGIP //////////////////////////////
			//AT+CIPHEAD
			//AT+CIPATS
			//AT+CIPSPRT
			//AT+CIPSERVER
			//AT+CIPCSGP
			Bool NetShowRemoteIP(Bool show); //AT+CIPSRIP
			//AT+CIPDPDP
			//AT+CIPMODE
			//AT+CIPCCFG
			//AT+CIPSHOWTP
			//AT+CIPUDPMODE
			//AT+CIPRXGET
			//AT+CIPRDTIMER
			//AT+CIPSGTXT
			Bool NetSetSendHex(Bool hexSend);//AT+CIPSENDHEX
			Bool NetSetOutputHex(Bool hexOutput);//AT+CIPHEXS
			//AT+CIPTKA
			Bool NetSetDisableNagle(Bool disable); //AT+CIPOPTION

			Bool NetPing(const UTF8Char *addr, UInt32 *respTime, UInt32 *ttl);//AT+CIPPING
			//AT+CIPCTL
			//AT+CIPFLT
		};		
	}
}
#endif
