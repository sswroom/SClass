#ifndef _SM_NET_SNMPUTIL
#define _SM_NET_SNMPUTIL
#include "Data/ArrayListNN.h"
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
		static ErrorStatus PDUParseMessage(Data::ByteArrayR pdu, OutParam<Int32> reqId, NN<Data::ArrayListNN<BindingItem>> itemList);
		static ErrorStatus PDUParseTrapMessage(Data::ByteArrayR pdu, NN<TrapInfo> trap, NN<Data::ArrayListNN<BindingItem>> itemList);

		static Text::CStringNN TypeGetName(UInt8 type);
		static Bool ValueToInt32(UInt8 type, const UInt8 *pduBuff, UOSInt valLen, OutParam<Int32> outVal);
		static void FreeBindingItem(NN<BindingItem> item);
		static Text::CStringNN ErrorStatusToString(ErrorStatus err);
	};
}
#endif
