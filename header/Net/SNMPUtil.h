#ifndef _SM_NET_SNMPUTIL
#define _SM_NET_SNMPUTIL
#include "Data/ArrayList.h"
#include "Text/StringBuilderUTF.h"

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

		static OSInt OIDCompare(const UInt8 *oid1, UOSInt oid1Len, const UInt8 *oid2, UOSInt oid2Len);
		static Bool OIDStartsWith(const UInt8 *oid1, UOSInt oid1Len, const UInt8 *oid2, UOSInt oid2Len);
		static void OIDToString(const UInt8 *pdu, UOSInt pduSize, Text::StringBuilderUTF *sb);
		static UOSInt OIDCalcPDUSize(const UTF8Char *oid);
		static UOSInt OIDText2PDU(const UTF8Char *oid, UInt8 *pduBuff);
		static const UTF8Char *TypeGetName(UInt8 type);
		static Bool ValueToInt32(UInt8 type, const UInt8 *pduBuff, UOSInt valLen, Int32 *outVal);
		static void FreeBindingItem(BindingItem *item);
		static const UTF8Char *ErrorStatusToString(ErrorStatus err);
	};
}
#endif
