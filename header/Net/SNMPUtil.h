#ifndef _SM_NET_SNMPUTIL
#define _SM_NET_SNMPUTIL
#include "Data/ArrayList.h"
#include "Text/CString.h"

namespace Net
{
	class SNMPUtil
	{
	public:
		typedef struct
		{
			UInt8 entOID[64];
			UOSInt entOIDLen;
			UInt32 agentIPv4;
			UInt32 genericTrap;
			UInt32 specificTrap;
			UInt32 timeStamp;
			UTF8Char community[64];
		} TrapInfo;
		
		typedef struct
		{
			UInt8 oid[64];
			UOSInt oidLen;
			UInt8 valType;
			UOSInt valLen;
			UInt8 *valBuff;
		} BindingItem;

		typedef enum
		{
			ES_NOERROR,
			ES_TOOBIG,
			ES_NOSUCHNAME,
			ES_BADVALUE,
			ES_READONLY,
			ES_GENERROR,
			ES_NORESP = -1,
			ES_UNKRESP = -2
		} ErrorStatus;

	public:
		static ErrorStatus PDUParseMessage(const UInt8 *pdu, UOSInt pduSize, Int32 *reqId, Data::ArrayList<BindingItem*> *itemList);
		static ErrorStatus PDUParseTrapMessage(const UInt8 *pdu, UOSInt pduSize, TrapInfo *trap, Data::ArrayList<BindingItem*> *itemList);

		static Text::CString TypeGetName(UInt8 type);
		static Bool ValueToInt32(UInt8 type, const UInt8 *pduBuff, UOSInt valLen, Int32 *outVal);
		static void FreeBindingItem(BindingItem *item);
		static Text::CString ErrorStatusToString(ErrorStatus err);
	};
}
#endif
