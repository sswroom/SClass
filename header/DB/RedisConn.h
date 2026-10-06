#ifndef _SM_DB_REDISCONN
#define _SM_DB_REDISCONN
#include "AnyType.h"
#include "Data/DataArray.hpp"
#include "Net/TCPClientFactory.h"
#include "Text/String.h"

namespace DB
{
	class RedisConn
	{
	public:
		enum class DataType
		{
			Null,
			String,
			Integer,
			Double,
			Array
		};

		enum class ReplyType
		{
			String = 1,
			Array = 2,
			Integer = 3,
			Nil = 4,
			Status = 5,
			Error = 6,
			Double = 7,
			Bool = 8,
			Map = 9,
			Set = 10,
			Attr = 11,
			Push = 12,
			BigNum = 13,
			Verb = 14
		};

		struct ReplyData
		{
			DataType type;
			union
			{
				NN<Text::String> str;
				Int64 integer;
				Double dbl;
				struct {
					UnsafeArray<ReplyData> items;
					UIntOS size;
				} arr;
			};
		};

		struct ReplyInfo : public ReplyData
		{
			ReplyType replyType;
		};
	private:
		AnyType context;
		NN<Text::String> host;
		UInt16 port;

		Optional<ReplyInfo> SendCommand(Int32 argc, UnsafeArray<UnsafeArray<const Char>> argv, UnsafeArray<const UIntOS> argvlen) const;
	public:
		RedisConn(NN<Net::TCPClientFactory> clif, Text::CStringNN host, UInt16 port);
		~RedisConn();

		Bool IsConnected() const;
		Bool Reconnect();

		Optional<ReplyInfo> SendAuth(Text::CStringNN password) const;
		Optional<ReplyInfo> SendSelect(Int32 db) const;
		Optional<ReplyInfo> SendType(Text::CStringNN key) const;
		Optional<ReplyInfo> SendGet(Text::CStringNN key) const;
		Optional<ReplyInfo> SendSet(Text::CStringNN key, Text::CStringNN value) const;
		Optional<ReplyInfo> SendDel(Text::CStringNN key) const;
		Optional<ReplyInfo> SendKeys(Text::CStringNN pattern) const;
		Optional<ReplyInfo> SendHGet(Text::CStringNN key, Text::CStringNN field) const;
		Optional<ReplyInfo> SendHSet(Text::CStringNN key, Text::CStringNN field, Text::CStringNN value) const;
		Optional<ReplyInfo> SendHDel(Text::CStringNN key, Text::CStringNN field) const;
		Optional<ReplyInfo> SendHLen(Text::CStringNN key) const;
		Optional<ReplyInfo> SendHKeys(Text::CStringNN key) const;
		Optional<ReplyInfo> SendSMembers(Text::CStringNN key) const;
		Optional<ReplyInfo> SendXLen(Text::CStringNN key) const;
		Optional<ReplyInfo> SendXRange(Text::CStringNN key, Int64 start, Int64 end, UIntOS count) const;
		static void __stdcall FreeReplyData(NN<ReplyData> replyData);
		static void __stdcall FreeReplyDataInner(NN<ReplyData> replyData);
	};
}
#endif