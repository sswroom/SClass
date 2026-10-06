#include "Stdafx.h"
#include "DB/RedisConn.h"

Optional<DB::RedisConn::ReplyInfo> DB::RedisConn::SendAuth(Text::CStringNN password) const
{
	UnsafeArray<const Char> argv[2] = {"AUTH", (const Char*)password.v.Ptr()};
	UIntOS argvlen[2] = {4, password.leng};
	return this->SendCommand(2, argv, argvlen);
}

Optional<DB::RedisConn::ReplyInfo> DB::RedisConn::SendSelect(Int32 db) const
{
	UTF8Char dbStr[32];
	UnsafeArray<UTF8Char> dbEnd = Text::StrInt32(dbStr, db);
	UnsafeArray<const Char> argv[2] = {"SELECT", (const Char*)dbStr};
	UIntOS argvlen[2] = {6, (UIntOS)(dbEnd.Ptr() - dbStr)};
	return this->SendCommand(2, argv, argvlen);
}

Optional<DB::RedisConn::ReplyInfo> DB::RedisConn::SendType(Text::CStringNN key) const
{
	UnsafeArray<const Char> argv[2] = {"TYPE", (const Char*)key.v.Ptr()};
	UIntOS argvlen[2] = {4, key.leng};
	return this->SendCommand(2, argv, argvlen);
}

Optional<DB::RedisConn::ReplyInfo> DB::RedisConn::SendGet(Text::CStringNN key) const
{
	UnsafeArray<const Char> argv[2] = {"GET", (const Char*)key.v.Ptr()};
	UIntOS argvlen[2] = {3, key.leng};
	return this->SendCommand(2, argv, argvlen);
}

Optional<DB::RedisConn::ReplyInfo> DB::RedisConn::SendSet(Text::CStringNN key, Text::CStringNN value) const
{
	UnsafeArray<const Char> argv[3] = {"SET", (const Char*)key.v.Ptr(), (const Char*)value.v.Ptr()};
	UIntOS argvlen[3] = {3, key.leng, value.leng};
	return this->SendCommand(3, argv, argvlen);
}

Optional<DB::RedisConn::ReplyInfo> DB::RedisConn::SendDel(Text::CStringNN key) const
{
	UnsafeArray<const Char> argv[2] = {"DEL", (const Char*)key.v.Ptr()};
	UIntOS argvlen[2] = {3, key.leng};
	return this->SendCommand(2, argv, argvlen);
}

Optional<DB::RedisConn::ReplyInfo> DB::RedisConn::SendKeys(Text::CStringNN pattern) const
{
	UnsafeArray<const Char> argv[2] = {"KEYS", (const Char*)pattern.v.Ptr()};
	UIntOS argvlen[2] = {4, pattern.leng};
	return this->SendCommand(2, argv, argvlen);
}

Optional<DB::RedisConn::ReplyInfo> DB::RedisConn::SendHGet(Text::CStringNN key, Text::CStringNN field) const
{
	UnsafeArray<const Char> argv[3] = {"HGET", (const Char*)key.v.Ptr(), (const Char*)field.v.Ptr()};
	UIntOS argvlen[3] = {4, key.leng, field.leng};
	return this->SendCommand(3, argv, argvlen);
}

Optional<DB::RedisConn::ReplyInfo> DB::RedisConn::SendHSet(Text::CStringNN key, Text::CStringNN field, Text::CStringNN value) const
{
	UnsafeArray<const Char> argv[4] = {"HSET", (const Char*)key.v.Ptr(), (const Char*)field.v.Ptr(), (const Char*)value.v.Ptr()};
	UIntOS argvlen[4] = {4, key.leng, field.leng, value.leng};
	return this->SendCommand(4, argv, argvlen);
}

Optional<DB::RedisConn::ReplyInfo> DB::RedisConn::SendHDel(Text::CStringNN key, Text::CStringNN field) const
{
	UnsafeArray<const Char> argv[3] = {"HDEL", (const Char*)key.v.Ptr(), (const Char*)field.v.Ptr()};
	UIntOS argvlen[3] = {4, key.leng, field.leng};
	return this->SendCommand(3, argv, argvlen);
}

Optional<DB::RedisConn::ReplyInfo> DB::RedisConn::SendHLen(Text::CStringNN key) const
{
	UnsafeArray<const Char> argv[2] = {"HLEN", (const Char*)key.v.Ptr()};
	UIntOS argvlen[2] = {4, key.leng};
	return this->SendCommand(2, argv, argvlen);
}

Optional<DB::RedisConn::ReplyInfo> DB::RedisConn::SendHKeys(Text::CStringNN key) const
{
	UnsafeArray<const Char> argv[2] = {"HKEYS", (const Char*)key.v.Ptr()};
	UIntOS argvlen[2] = {5, key.leng};
	return this->SendCommand(2, argv, argvlen);
}

Optional<DB::RedisConn::ReplyInfo> DB::RedisConn::SendSMembers(Text::CStringNN key) const
{
	UnsafeArray<const Char> argv[2] = {"SMEMBERS", (const Char*)key.v.Ptr()};
	UIntOS argvlen[2] = {8, key.leng};
	return this->SendCommand(2, argv, argvlen);
}

Optional<DB::RedisConn::ReplyInfo> DB::RedisConn::SendXLen(Text::CStringNN key) const
{
	UnsafeArray<const Char> argv[2] = {"XLEN", (const Char*)key.v.Ptr()};
	UIntOS argvlen[2] = {4, key.leng};
	return this->SendCommand(2, argv, argvlen);
}

Optional<DB::RedisConn::ReplyInfo> DB::RedisConn::SendXRange(Text::CStringNN key, Int64 start, Int64 end, UIntOS count) const
{
	Char sbuff[128];
	UnsafeArray<Char> sptr = sbuff;
	UnsafeArray<Char> sptr2;
	UnsafeArray<const Char> argv[6] = {"XRANGE", (const Char*)key.v.Ptr(), "-", "+", "COUNT", "1"};
	UIntOS argvlen[6] = {6, key.leng, 1, 1, 5, 1};
	if (start != 0)
	{
		sptr2 = Text::StrInt64(sptr, start);
		argv[2] = (const Char*)sptr.Ptr();
		argvlen[2] = (UIntOS)(sptr2 - sptr);
		sptr = sptr2 + 1;
	}
	if (end != 0)
	{
		sptr2 = Text::StrInt64(sptr, end);
		argv[3] = (const Char*)sptr.Ptr();
		argvlen[3] = (UIntOS)(sptr2 - sptr);
		sptr = sptr2 + 1;
	}
	if (count != 0)
	{
		sptr2 = Text::StrUIntOS(sptr, count);
		argv[5] = (const Char*)sptr.Ptr();
		argvlen[5] = (UIntOS)(sptr2 - sptr);
		sptr = sptr2 + 1;
		return this->SendCommand(6, argv, argvlen);
	}
	else
	{
		return this->SendCommand(4, argv, argvlen);
	}
}

void __stdcall DB::RedisConn::FreeReplyData(NN<ReplyData> replyData)
{
	FreeReplyDataInner(replyData);
	MemFreeNN(replyData);
}

void __stdcall DB::RedisConn::FreeReplyDataInner(NN<ReplyData> replyData)
{
	if (replyData->type == DB::RedisConn::DataType::String)
	{
		replyData->str->Release();
	}
	else if (replyData->type == DB::RedisConn::DataType::Array)
	{
		UIntOS i = 0;
		UIntOS cnt = replyData->arr.size;
		while (i < cnt)
		{
			FreeReplyDataInner(replyData->arr.items[i]);
			i++;
		}
		MemFreeArr(replyData->arr.items);
	}
}
