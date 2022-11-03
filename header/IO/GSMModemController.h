#ifndef _SM_IO_GSMMODEMCONTROLLER
#define _SM_IO_GSMMODEMCONTROLLER
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
			Text::String *longName;
			Text::String *shortName;
			Int32 plmn;
			OperStatus status;
			Int32 netact;
		} Operator;

		typedef struct
		{
			Int32 index;
			SMSStatus status;
			UOSInt pduLeng;
			UInt8 *pduMessage;
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
			Text::String *name;
			Text::String *number;
		} PBEntry;

		struct PDPContext
		{
			UInt32 cid;
			Text::String *type;
			Text::String *apn;
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
		Bool GetSMSFormat(SMSFormat *smsFormat);

		void FreeOperator(Operator *oper);

	public:
		GSMModemController(IO::ATCommandChannel *channel, Bool needRelease);
		virtual ~GSMModemController();

		// GSM Commands 3GPP TS 27.007
		UTF8Char *GSMGetManufacturer(UTF8Char *manu); //AT+CGMI
		UTF8Char *GSMGetModelIdent(UTF8Char *model); //AT+CGMM
		UTF8Char *GSMGetModemVer(UTF8Char *ver); //AT+CGMR
		UTF8Char *GSMGetIMEI(UTF8Char *imei); //AT+CGSN
		UTF8Char *GSMGetTECharset(UTF8Char *cs); //AT+CSCS
		Bool GSMSetTECharset(const UTF8Char *cs); //AT+CSCS
		Bool GSMGetTECharsetsSupported(Data::ArrayList<Text::String*> *csList); //AT+CSCS
		UTF8Char *GSMGetIMSI(UTF8Char *imsi); //AT+CIMI
		UTF8Char *GSMGetCurrOperator(UTF8Char *oper); //AT+COPS
		UTF8Char *GSMGetCurrPLMN(UTF8Char *plmn); //AT+COPS
		Bool GSMConnectPLMN(Int32 plmn); //AT+COPS
		Bool GSMGetAllowedOperators(Data::ArrayList<Operator*> *operList); //AT+COPS
		void GSMFreeOperators(Data::ArrayList<Operator*> *operList);
		Int32 GSMSearchOperatorPLMN(Int32 netact);
		SIMStatus GSMGetSIMStatus(); //AT+CPIN
		Bool GSMGetSignalQuality(RSSI *rssi, BER *ber); //AT+CSQ
		Bool GSMSetFunctionalityMin(); //AT+CFUN
		Bool GSMSetFunctionalityFull(); //AT+CFUN
		Bool GSMSetFunctionalityReset(); //AT+CFUN
		Bool GSMGetModemTime(Data::DateTime *date); //AT+CCLK
		Bool GSMSetModemTime(Data::DateTime *date); //AT+CCLK
		Bool GSMGetRegisterNetwork(NetworkResult *n, RegisterStatus *stat, UInt16 *lac, UInt32 *ci, AccessTech *act); //AT+CREG

		Int32 GSMGetSIMPLMN();
		
		Bool GPRSEPSReg(); //AT+CEREG
		Bool GPRSNetworkReg(); //AT+CEREG
		Bool GPRSServiceIsAttached(Bool *attached); //AT+CGATT
		Bool GPRSServiceSetAttached(Bool attached); //AT+CGATT
		Bool GPRSSetAPN(Text::CString apn); //AT+CGDCONT
		Bool GPRSSetPDPContext(UInt32 cid, Text::CString type, Text::CString apn); //AT+CGDCONT
		Bool GPRSGetPDPContext(Data::ArrayList<PDPContext*> *ctxList); //AT+CGDCONT
		void GPRSFreePDPContext(Data::ArrayList<PDPContext*> *ctxList);
		Bool GPRSSetPDPActive(Bool active); //AT+CGACT
		Bool GPRSSetPDPActive(Bool active, UInt32 cid); //AT+CGACT
		Bool GPRSGetPDPActive(Data::ArrayList<ActiveState> *actList); //AT+CGACT

		// SMS Commands
		Bool SMSListMessages(Data::ArrayList<SMSMessage*> *msgList, SMSStatus status);
		void SMSFreeMessages(Data::ArrayList<SMSMessage*> *msgList);
		void SMSFreeMessage(SMSMessage *msg);
		Bool SMSDeleteMessage(Int32 index);
		Bool SMSSendMessage(Text::SMSMessage *msg);
		Bool SMSSetStorage(SMSStorage reading, SMSStorage writing, SMSStorage store);
		Bool SMSGetStorageInfo(SMSStorageInfo *reading, SMSStorageInfo *writing, SMSStorageInfo *store);
		UTF8Char *SMSGetSMSC(UTF8Char *buff);

		// Phonebook Commands
		UTF8Char *PBGetCharset(UTF8Char *cs);
		Bool PBSetCharset(const UTF8Char *cs, UOSInt csLen);
		Bool PBSetStorage(PBStorage storage);
		Bool PBGetStorage(PBStorage *storage, Int32 *usedEntry, Int32 *freeEntry);
		Bool PBGetStorageStatus(Int32 *startEntry, Int32 *endEntry, Int32 *maxNumberLen, Int32 *maxTextLen);
		Bool PBReadEntries(Data::ArrayList<PBEntry*> *phoneList, Int32 startEntry, Int32 endEntry);
		Bool PBReadAllEntries(Data::ArrayList<PBEntry*> *phoneList);
		void PBFreeEntry(PBEntry *entry);
		void PBFreeEntries(Data::ArrayList<PBEntry*> *phoneList);

	public:
		static Int32 RSSIGetdBm(RSSI rssi);
		static UTF8Char *RSSIGetName(UTF8Char *buff, RSSI rssi);
		static UTF8Char *BERGetName(UTF8Char *buff, BER ber);
		static Text::CString OperStatusGetName(OperStatus operStatus);
		static Text::CString SIMStatusGetName(SIMStatus simStatus);
		static Text::CString NetworkResultGetName(NetworkResult n);
		static Text::CString RegisterStatusGetName(RegisterStatus stat);
		static Text::CString AccessTechGetName(AccessTech act);
	};
}
#endif
