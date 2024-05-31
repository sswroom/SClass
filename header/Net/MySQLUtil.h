#ifndef _SM_NET_MYSQLUTIL
#define _SM_NET_MYSQLUTIL
#include "DB/DBUtil.h"

namespace Net
{
	class MySQLUtil
	{
	public:
		typedef enum
		{
			CLIENT_LONG_PASSWORD =     0x00000001,
			CLIENT_FOUND_ROWS =        0x00000002,
			CLIENT_LONG_FLAG =         0x00000004,
			CLIENT_CONNECT_WITH_DB =   0x00000008,
			CLIENT_NO_SCHEMA =         0x00000010,
			CLIENT_COMPRESS =          0x00000020,
			CLIENT_ODBC =              0x00000040,
			CLIENT_LOCAL_FILES =       0x00000080,
			CLIENT_IGNORE_SPACE =      0x00000100,
			CLIENT_PROTOCOL_41 =       0x00000200,
			CLIENT_INTERACTIVE =       0x00000400,
			CLIENT_SSL =               0x00000800,
			CLIENT_IGNORE_SIGPIPE =    0x00001000,
			CLIENT_TRANSACTIONS =      0x00002000,
			CLIENT_RESERVED =          0x00004000,
			CLIENT_SECURE_CONNECTION = 0x00008000,
			CLIENT_MULTI_STATEMENTS =  0x00010000,
			CLIENT_MULTI_RESULTS =     0x00020000,
			CLIENT_PS_MULTI_RESULTS =  0x00040000,
			CLIENT_PLUGIN_AUTH =       0x00080000,
			CLIENT_CONNECT_ATTRS =     0x00100000,
			CLIENT_PLUGIN_AUTH_LENENC_CLIENT_DATA = 0x00200000,
			CLIENT_CAN_HANDLE_EXPIRED_PASSWORDS =  0x00400000,
			CLIENT_SESSION_TRACK =     0x00800000,
			CLIENT_DEPRECATE_EOF =     0x01000000
		} CapabilityFlags;

		typedef enum
		{
			MYSQL_TYPE_DECIMAL =     0x00,
			MYSQL_TYPE_TINY =        0x01,
			MYSQL_TYPE_SHORT =       0x02,
			MYSQL_TYPE_LONG =        0x03,
			MYSQL_TYPE_FLOAT =       0x04,
			MYSQL_TYPE_DOUBLE =      0x05,
			MYSQL_TYPE_NULL =        0x06,
			MYSQL_TYPE_TIMESTAMP =   0x07,
			MYSQL_TYPE_LONGLONG =    0x08,
			MYSQL_TYPE_INT24 =       0x09,
			MYSQL_TYPE_DATE =        0x0A,
			MYSQL_TYPE_TIME =        0x0B,
			MYSQL_TYPE_DATETIME =    0x0C,
			MYSQL_TYPE_YEAR =        0x0D,
			MYSQL_TYPE_NEWDATE =     0x0E,
			MYSQL_TYPE_VARCHAR =     0x0F,
			MYSQL_TYPE_BIT =         0x10,
			MYSQL_TYPE_TIMESTAMP2 =  0x11,
			MYSQL_TYPE_DATETIME2 =   0x12,
			MYSQL_TYPE_TIME2 =       0x13,
			MYSQL_TYPE_NEWDECIMAL =  0xF6,
			MYSQL_TYPE_ENUM =        0xF7,
			MYSQL_TYPE_SET =         0xF8,
			MYSQL_TYPE_TINY_BLOB =   0xF9,
			MYSQL_TYPE_MEDIUM_BLOB = 0xFA,
			MYSQL_TYPE_LONG_BLOB =   0xFB,
			MYSQL_TYPE_BLOB =        0xFC,
			MYSQL_TYPE_VAR_STRING =  0xFD,
			MYSQL_TYPE_STRING =      0xFE,
			MYSQL_TYPE_GEOMETRY =    0xFF
		} MySQLType;

		enum class AuthenType
		{
			MySQLNativePassword,
			CachingSHA2Password
		};
		
	public:
		static const UInt8 *ReadLenencInt(const UInt8 *buff, UInt64 *val);
		static UnsafeArray<UInt8> AppendLenencInt(UnsafeArray<UInt8> buff, UInt64 val);
		static UnsafeArray<UInt8> AppendLenencStrC(UnsafeArray<UInt8> buff, UnsafeArrayOpt<const UTF8Char> s, UOSInt len);
		static MySQLType ColType2MySQLType(DB::DBUtil::ColType colType);
		static DB::DBUtil::ColType MySQLType2ColType(MySQLType mysqlType);
		static AuthenType AuthenTypeParse(Text::CStringNN name);
		static Text::CStringNN AuthenTypeGetName(AuthenType authType);
		static UOSInt BuildAuthen(UnsafeArray<UInt8> buff, AuthenType authType, const UInt8 *nonce, UOSInt nonceSize, Text::CStringNN password);
		static Bool IsAxisAware(Text::CStringNN svrVer);
	};
}
#endif
