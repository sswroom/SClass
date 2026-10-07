#include "Stdafx.h"
#include "DB/RedisConn.h"
#include <hiredis/hiredis.h>

//#define VERBOSE

void RedisConn_ParseReplyData(redisReply *reply, NN<DB::RedisConn::ReplyData> data)
{
	switch (reply->type)
	{
	case REDIS_REPLY_STRING:
	case REDIS_REPLY_STATUS:
	case REDIS_REPLY_ERROR:
	case REDIS_REPLY_VERB:
	case REDIS_REPLY_BIGNUM:
		data->type = DB::RedisConn::DataType::String;
		data->str = Text::String::New((const UTF8Char*)reply->str, reply->len);
		break;
	case REDIS_REPLY_INTEGER:
		data->type = DB::RedisConn::DataType::Integer;
		data->integer = reply->integer;
		break;
	case REDIS_REPLY_DOUBLE:
		data->type = DB::RedisConn::DataType::Double;
		data->dbl = reply->dval;
		break;
	case REDIS_REPLY_ARRAY:
		data->type = DB::RedisConn::DataType::Array;
		{
			UIntOS i = 0;
			UIntOS j = (UIntOS)reply->elements;
			data->arr.items = MemAllocArr(DB::RedisConn::ReplyData, j);
			data->arr.size = j;
			while (i < j)
			{
				RedisConn_ParseReplyData(reply->element[i], data->arr.items[i]);
				i++;
			}
		}
		break;
	default:
		printf("RedisConn: Unknown reply type %d\r\n", reply->type);
		data->type = DB::RedisConn::DataType::Null;
		break;
	}
}

Optional<DB::RedisConn::ReplyInfo> DB::RedisConn::SendCommand(Int32 argc, UnsafeArray<UnsafeArray<const Char>> argv, UnsafeArray<const UIntOS> argvlen) const
{
	if (this->context.IsNull() || ((redisContext*)this->context.p)->err)
		return nullptr;
#ifdef VERBOSE
	printf("RedisConn: Sending command");
	Int32 i = 0;
	while (i < argc)
	{
		printf(" %s", argv[i].Ptr());
		i++;
	}
	printf("\r\n");
#endif
	redisReply *reply = (redisReply*)redisCommandArgv((redisContext*)this->context.p, argc, (const Char**)argv.Ptr(), (const size_t*)argvlen.Ptr());
	if (!reply)
		return nullptr;
	NN<DB::RedisConn::ReplyInfo> ret = MemAllocNN(DB::RedisConn::ReplyInfo);
	ret->replyType = (DB::RedisConn::ReplyType)reply->type;
	RedisConn_ParseReplyData(reply, ret);
	freeReplyObject(reply);
	return ret;

}

DB::RedisConn::RedisConn(NN<Net::TCPClientFactory> clif, Text::CStringNN host, UInt16 port)
{
	this->context = nullptr;
	this->host = Text::String::New(host);
	this->port = port;
	this->Reconnect();
}

DB::RedisConn::~RedisConn()
{
	if (!this->context.IsNull())
	{
		redisFree((redisContext*)this->context.p);
		this->context = 0;
	}
	this->host->Release();
}

Bool DB::RedisConn::IsConnected() const
{
	Bool ret = !this->context.IsNull() && ((redisContext*)this->context.p)->err == 0;
#ifdef VERBOSE
	printf("RedisConn: IsConnected = %b\r\n", ret);
#endif
	return ret;
}

Bool DB::RedisConn::Reconnect()
{
	if (!this->context.IsNull())
	{
		redisFree((redisContext*)this->context.p);
		this->context = 0;
	}
	redisContext *ctx = redisConnect((const Char*)this->host->v.Ptr(), port);
	if (ctx == 0 || ctx->err)
	{
		printf("RedisConn: Failed to connect to %s:%d\r\n", (const Char*)this->host->v.Ptr(), this->port);
		if (ctx)
			redisFree(ctx);
		return false;
	}
	this->context = ctx;
	return true;
}