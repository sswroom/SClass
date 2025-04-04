#ifndef _SM_IO_GSMMODEMCONTROLLER
#define _SM_IO_GSMMODEMCONTROLLER
#include "Data/ArrayListNN.h"
#include "Data/DateTime.h"
#include "IO/ModemController.h"
#include "Text/SMSMessage.h"
#include "Text/String.h"

namespace IO
{
	class GSMModemController : public ModemController
	{
	public:
 		typedef enum
		{
			SIMS_UNKNOWN,
			SIMS_READY,
			SIMS_ABSENT
		} SIMStatus;

		typedef enum
		{
			RSSI_113 = 0, //dBm
			RSSI_111 = 1,
			RSSI_109 = 2,
			RSSI_107 = 3,
			RSSI_105 = 4,
			RSSI_103 = 5,
			RSSI_101 = 6,
			RSSI_99 = 7,
			RSSI_97 = 8,
			RSSI_95 = 9,
			RSSI_93 = 10,
			RSSI_91 = 11,
			RSSI_89 = 12,
			RSSI_87 = 13,
			RSSI_85 = 14,
			RSSI_83 = 15,
			RSSI_81 = 16,
			RSSI_79 = 17,
			RSSI_77 = 18,
			RSSI_75 = 19,
			RSSI_73 = 20,
			RSSI_71 = 21,
			RSSI_69 = 22,
			RSSI_67 = 23,
			RSSI_65 = 24,
			RSSI_63 = 25,
			RSSI_61 = 26,
			RSSI_59 = 27,
			RSSI_57 = 28,
			RSSI_55 = 29,
			RSSI_53 = 30,
			RSSI_51 = 31,
			RSSI_UNKNOWN = 99
		} RSSI;

		typedef enum
		{
			BER_RXQUAL_0 = 0, //       BER < 0.2%
			BER_RXQUAL_1 = 1, //0.2% < BER < 0.4%
			BER_RXQUAL_2 = 2, //0.4% < BER < 0.8%
			BER_RXQUAL_3 = 3, //0.8% < BER < 1.6%
			BER_RXQUAL_4 = 4, //1.6% < BER < 3.2%
			BER_RXQUAL_5 = 5, //3.2% < BER < 6.4%
			BER_RXQUAL_6 = 6, //6.4% < BER <12.8%
			BER_RXQUAL_7 = 7, //12.8%< BER
			BER_UNKNWON = 99
		} BER;
		
		typedef enum
		{
			OSTAT_UNKNOWN = 0,
			OSTAT_AVAILABLE = 1,
			OSTAT_CURRENT = 2,
			OSTAT_FORBIDDEN = 3
		} OperStatus;

		typedef enum
		{
			SMSS_UNREAD = 0,
			SMSS_READ = 1,
			SMSS_UNSENT = 2,
			SMSS_SENT = 3,
			SMSS_ALL = 4
		} SMSStatus;

		typedef enum
		{
			SMSF_PDU = 0,
			SMSF_TEXT = 1
		} SMSFormat;

		typedef enum
		{
			SMSSTORE_UNCHANGE = 0,
			SMSSTORE_SIM = 1,
			SMSSTORE_FLASH = 2,
			SMSSTORE_STATUSREPORT = 3,
			SMSSTORE_CBMMESSAGE = 4
		} SMSStorage;

		enum class NetworkResult
		{
			Disable = 0,
			Enable = 1,
			Enable_w_Location = 2
		};

		enum class RegisterStatus
		{
			NotRegistered = 0,
			RegisteredHomeNetwork = 1,
			NotRegisteredSearch = 2,
			RegistrationDenied = 3,
			Unknown = 4,
			RegisteredRoaming = 5
		};

		enum class AccessTech
		{
			GSM = 0,
			GSMCompact = 1,
			UTRAN = 2,
			GSM_w_EGPRS = 3,
			UTRAN_w_HSDPA = 4,
			UTRAN_w_HSUPA = 5,
			UTRAN_w_HSDPA_HSUPA = 6,
			EUTRAN = 7
		};

		typedef enum
		{
			PBSTORE_UNKNOWN,
			PBSTORE_SIM,
			PBSTORE_SIM_RESTRICTED,
			PBSTORE_SIM_OWN_NUMBERS,
			PBSTORE_EMERGENCY,
			PBSTORE_LASTNUMDIAL,
			PBSTORE_UNANSWERED,
			PBSTORE_ME,
			PBSTORE_ME_SIM,
			PBSTORE_RECEIVED_CALL,
			PBSTORE_SERVICE_DIALING_NUMBERS
		} PBStorage;

		typedef struct
		{
			NN<Text::String> longName;
			NN<Text::String> shortName;
			Int32 plmn;
			OperStatus status;
			Int32 netact;
		} Operator;

		typedef struct
		{
			Int32 index;
			SMSStatus status;
			UOSInt pduLeng;
			UnsafeArray<UInt8> pduMessage;
		} SMSMessage;

		typedef struct
		{
			SMSStorage storage;
			Int32 used;
			Int32 available;
		} SMSStorageInfo;

		typedef struct
		{
			Int32 index;
			NN<Text::String> name;
			NN<Text::String> number;
		} PBEntry;

		struct PDPContext
		{
			UInt32 cid;
			NN<Text::String> type;
			NN<Text::String> apn;
		};

		struct ActiveState
		{
			UInt32 cid;
			Bool active;

			ActiveState() = default;
			ActiveState(UInt32 cid)
			{
				this->cid = cid;
				this->active = false;
			}

			Bool operator==(ActiveState& act)
			{
				return this->cid == act.cid && this->active == act.active;
			}
		};

	protected:
		Int32 smsFormat;

	protected:
		Bool SetSMSFormat(SMSFormat smsFormat);
		Bool GetSMSFormat(OutParam<SMSFormat> smsFormat);

		static void __stdcall FreeOperator(NN<Operator> oper);

	public:
		GSMModemController(NN<IO::ATCommandChannel> channel, Bool needRelease);
		virtual ~GSMModemController();

		// GSM Commands 3GPP TS 27.007
		UnsafeArrayOpt<UTF8Char> GSMGetManufacturer(UnsafeArray<UTF8Char> manu); //AT+CGMI
		UnsafeArrayOpt<UTF8Char> GSMGetModelIdent(UnsafeArray<UTF8Char> model); //AT+CGMM
		UnsafeArrayOpt<UTF8Char> GSMGetModemVer(UnsafeArray<UTF8Char> ver); //AT+CGMR
		UnsafeArrayOpt<UTF8Char> GSMGetIMEI(UnsafeArray<UTF8Char> imei); //AT+CGSN
		UnsafeArrayOpt<UTF8Char> GSMGetTECharset(UnsafeArray<UTF8Char> cs); //AT+CSCS
		Bool GSMSetTECharset(const UnsafeArray<UTF8Char> cs); //AT+CSCS
		Bool GSMGetTECharsetsSupported(NN<Data::ArrayListStringNN> csList); //AT+CSCS
		UnsafeArrayOpt<UTF8Char> GSMGetIMSI(UnsafeArray<UTF8Char> imsi); //AT+CIMI
		UnsafeArrayOpt<UTF8Char> GSMGetCurrOperator(UnsafeArray<UTF8Char> oper); //AT+COPS
		UnsafeArrayOpt<UTF8Char> GSMGetCurrPLMN(UnsafeArray<UTF8Char> plmn); //AT+COPS
		Bool GSMConnectPLMN(Int32 plmn); //AT+COPS
		Bool GSMGetAllowedOperators(NN<Data::ArrayListNN<Operator>> operList); //AT+COPS
		void GSMFreeOperators(NN<Data::ArrayListNN<Operator>> operList);
		Int32 GSMSearchOperatorPLMN(Int32 netact);
		SIMStatus GSMGetSIMStatus(); //AT+CPIN
		Bool GSMGetSignalQuality(OutParam<RSSI> rssi, OutParam<BER> ber); //AT+CSQ
		Bool GSMSetFunctionalityMin(); //AT+CFUN
		Bool GSMSetFunctionalityFull(); //AT+CFUN
		Bool GSMSetFunctionalityReset(); //AT+CFUN
		Bool GSMGetModemTime(NN<Data::DateTime> date); //AT+CCLK
		Bool GSMSetModemTime(NN<Data::DateTime> date); //AT+CCLK
		Bool GSMGetRegisterNetwork(OutParam<NetworkResult> n, OutParam<RegisterStatus> stat, OutParam<UInt16> lac, OutParam<UInt32> ci, OutParam<AccessTech> act); //AT+CREG

		Int32 GSMGetSIMPLMN();
		
		Bool GPRSEPSReg(); //AT+CEREG
		Bool GPRSNetworkReg(); //AT+CEREG
		Bool GPRSServiceIsAttached(OutParam<Bool> attached); //AT+CGATT
		Bool GPRSServiceSetAttached(Bool attached); //AT+CGATT
		Bool GPRSSetAPN(Text::CStringNN apn); //AT+CGDCONT
		Bool GPRSSetPDPContext(UInt32 cid, Text::CStringNN type, Text::CStringNN apn); //AT+CGDCONT
		Bool GPRSGetPDPContext(NN<Data::ArrayListNN<PDPContext>> ctxList); //AT+CGDCONT
		void GPRSFreePDPContext(NN<Data::ArrayListNN<PDPContext>> ctxList);
		Bool GPRSSetPDPActive(Bool active); //AT+CGACT
		Bool GPRSSetPDPActive(Bool active, UInt32 cid); //AT+CGACT
		Bool GPRSGetPDPActive(NN<Data::ArrayList<ActiveState>> actList); //AT+CGACT

		// SMS Commands
		Bool SMSListMessages(NN<Data::ArrayListNN<SMSMessage>> msgList, SMSStatus status);
		void SMSFreeMessages(NN<Data::ArrayListNN<SMSMessage>> msgList);
		static void __stdcall SMSFreeMessage(NN<SMSMessage> msg);
		Bool SMSDeleteMessage(Int32 index);
		Bool SMSSendMessage(NN<Text::SMSMessage> msg);
		Bool SMSSetStorage(SMSStorage reading, SMSStorage writing, SMSStorage store);
		Bool SMSGetStorageInfo(Optional<SMSStorageInfo> reading, Optional<SMSStorageInfo> writing, Optional<SMSStorageInfo> store);
		UnsafeArrayOpt<UTF8Char> SMSGetSMSC(UnsafeArray<UTF8Char> buff);

		// Phonebook Commands
		UnsafeArrayOpt<UTF8Char> PBGetCharset(UnsafeArray<UTF8Char> cs);
		Bool PBSetCharset(UnsafeArray<const UTF8Char> cs, UOSInt csLen);
		Bool PBSetStorage(PBStorage storage);
		Bool PBGetStorage(OptOut<PBStorage> storage, OptOut<Int32> usedEntry, OptOut<Int32> freeEntry);
		Bool PBGetStorageStatus(OptOut<Int32> startEntry, OptOut<Int32> endEntry, OptOut<Int32> maxNumberLen, OptOut<Int32> maxTextLen);
		Bool PBReadEntries(NN<Data::ArrayListNN<PBEntry>> phoneList, Int32 startEntry, Int32 endEntry);
		Bool PBReadAllEntries(NN<Data::ArrayListNN<PBEntry>> phoneList);
		static void __stdcall PBFreeEntry(NN<PBEntry> entry);
		void PBFreeEntries(NN<Data::ArrayListNN<PBEntry>> phoneList);

	public:
		static Int32 RSSIGetdBm(RSSI rssi);
		static UnsafeArray<UTF8Char> RSSIGetName(UnsafeArray<UTF8Char> buff, RSSI rssi);
		static UnsafeArray<UTF8Char> BERGetName(UnsafeArray<UTF8Char> buff, BER ber);
		static Text::CStringNN OperStatusGetName(OperStatus operStatus);
		static Text::CStringNN SIMStatusGetName(SIMStatus simStatus);
		static Text::CStringNN NetworkResultGetName(NetworkResult n);
		static Text::CStringNN RegisterStatusGetName(RegisterStatus stat);
		static Text::CStringNN AccessTechGetName(AccessTech act);
	};
}
#endif
