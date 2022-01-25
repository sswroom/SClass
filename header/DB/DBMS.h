#ifndef _SM_DB_DBMS
#define _SM_DB_DBMS
#include "Crypto/Hash/SHA1.h"
#include "Data/Int32Map.h"
#include "Data/StringUTF8Map.h"
#include "DB/DBReader.h"
#include "IO/LogTool.h"
#include "Net/SocketUtil.h"
#include "Sync/Mutex.h"
#include "Text/String.h"
#include "Text/StringBuilderUTF8.h"

namespace DB
{
	class DBMSReader;
	class DBMS
	{
	public:
		typedef struct
		{
			UInt32 clientMaxPacketSize;
		} SessionParam;

		typedef enum
		{
			SQLM_ALLOW_INVALID_DATES = 1,
			SQLM_ANSI_QUOTES = 2,
			SQLM_ERROR_FOR_DIVISION_BY_ZERO = 4,
			SQLM_HIGH_NOT_PRECEDENCE = 8,
			SQLM_IGNORE_SPACE = 0x10,
			SQLM_NO_AUTO_VALUE_ON_ZERO = 0x20,
			SQLM_NO_BACKSLASH_ESCAPES = 0x40,
			SQLM_NO_DIR_IN_CREATE = 0x80,
			SQLM_NO_ENGINE_SUBSTITUTION = 0x100,
			SQLM_NO_UNSIGNED_SUBTRACTION = 0x200,
			SQLM_NO_ZERO_DATE = 0x400,
			SQLM_NO_ZERO_IN_DATE = 0x800,
			SQLM_ONLY_FULL_GROUP_BY = 0x1000,
			SQLM_PAD_CHAR_TO_FULL_LENGTH = 0x2000,
			SQLM_PIPES_AS_CONCAT = 0x4000,
			SQLM_REAL_AS_FLOAT = 0x8000,
			SQLM_STRICT_ALL_TABLES = 0x10000,
			SQLM_STRICT_TRANS_TABLES = 0x20000,
			SQLM_TIME_TRUNCATE_FRACTIONAL = 0x40000,

			SQLM_ANSI = SQLM_REAL_AS_FLOAT
					| SQLM_PIPES_AS_CONCAT
					| SQLM_ANSI_QUOTES
		 			| SQLM_IGNORE_SPACE
					| SQLM_ONLY_FULL_GROUP_BY,
			SQLM_TRADITIONAL = SQLM_STRICT_TRANS_TABLES
					| SQLM_STRICT_ALL_TABLES
					| SQLM_NO_ZERO_IN_DATE
					| SQLM_NO_ZERO_DATE
					| SQLM_ERROR_FOR_DIVISION_BY_ZERO
					| SQLM_NO_ENGINE_SUBSTITUTION
		} SQLMODE;
		
	private:
		typedef struct
		{
			Int32 userId;
			UTF8Char host[32];
			UInt8 pwdSha1[20];
		} UserInfo;
		
		typedef struct
		{
			const UTF8Char *login;
			Data::ArrayList<UserInfo*> *userList;
		} LoginInfo;
		
		typedef struct
		{
			Int32 sessId;
			UserInfo *user;
			const UTF8Char *lastError;

			Bool autoCommit;
			Int32 autoIncInc;
			SQLMODE sqlModes;
			SessionParam params;
			Text::String *database;
			Data::StringUTF8Map<Text::String*> *userVars;
		} SessionInfo;

		typedef struct
		{
			const UTF8Char *name;
			const UTF8Char *sqlPtr;
			Text::String *asName;
		} SQLColumn;
		
		typedef enum
		{
			AT_READ,
			AT_SET_SESSION,
			AT_SET_GLOBAL
		} AccessType;
		
	private:
		static const Char *sysVarList[];

		const UTF8Char *versionStr;
		IO::LogTool *log;
		Sync::Mutex *loginMut;
		Crypto::Hash::SHA1 *loginSHA1;
		Data::StringUTF8Map<LoginInfo*> *loginMap;
		Sync::Mutex *sessMut;
		Data::Int32Map<SessionInfo*> *sessMap;

		static const UTF8Char *SQLParseName(UTF8Char *nameBuff, const UTF8Char *sql);
		static Bool StrLike(const UTF8Char *val, const UTF8Char *likeStr);

		Bool SysVarExist(SessionInfo *sess, const UTF8Char *varName, AccessType atype);
		const UTF8Char *SysVarGet(Text::StringBuilderUTF8 *sb, SessionInfo *sess, const UTF8Char *varName, UOSInt nameLen);
		void SysVarColumn(DB::DBMSReader *reader, UOSInt colIndex, const UTF8Char *varName, const UTF8Char *colName);
		Bool SysVarSet(SessionInfo *sess, Bool isGlobal, const UTF8Char *varName, Text::String *val);

		const UTF8Char *UserVarGet(Text::StringBuilderUTF8 *sb, SessionInfo *sess, const UTF8Char *varName);
		void UserVarColumn(DB::DBMSReader *reader, UOSInt colIndex, const UTF8Char *varName, const UTF8Char *colName);
		Bool UserVarSet(SessionInfo *sess, const UTF8Char *varName, Text::String *val);

		Text::String *Evals(const UTF8Char **valPtr, SessionInfo *sess, DB::DBMSReader *reader, UOSInt colIndex, const UTF8Char *colName, Bool *valid);
	public:
		DBMS(const UTF8Char *versionStr, IO::LogTool *log);
		virtual ~DBMS();

		const UTF8Char *GetVersion();
		IO::LogTool *GetLogTool();

		Bool UserAdd(Int32 userId, const UTF8Char *userName, const UTF8Char *password, const UTF8Char *host);
		Int32 UserLoginMySQL(Int32 sessId, const UTF8Char *userName, const UInt8 *randomData, const UInt8 *passHash, const Net::SocketUtil::AddressInfo *addr, const SessionParam *param, const UTF8Char *database);

		DB::DBReader *ExecuteReader(Int32 sessId, const UTF8Char *sql, UOSInt sqlLen);
		void CloseReader(DB::DBReader *r);
		UTF8Char *GetErrMessage(Int32 sessId, UTF8Char *msgBuff);

		SessionInfo *SessGet(Int32 sessId);
		void SessEnd(Int32 sessId);
	private:
		void SessDelete(SessionInfo *sess);
	};
}
#endif
