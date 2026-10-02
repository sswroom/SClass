#include "Stdafx.h"
#include "DB/RedisClient.h"
#include "Text/StringBuilderUTF8.h"
#include <hiredis/hiredis.h>

Bool DB::RedisClient::Connect(Text::CStringNN host, UInt16 port, Text::CString password, Int32 db)
{
	if (!this->context.IsNull())
	{
		redisFree((redisContext*)this->context.p);
		this->context = 0;
	}
	redisContext *ctx = redisConnect((const Char*)host.v.Ptr(), port);
	if (ctx == 0 || ctx->err)
	{
		if (ctx)
			redisFree(ctx);
		return false;
	}
	this->context = ctx;
	Text::CStringNN passwordNN;
	if (password.SetTo(passwordNN))
	{
		UnsafeArray<const Char> argv[2] = {"AUTH", (const Char*)passwordNN.v.Ptr()};
		UIntOS argvlen[2] = {4, passwordNN.leng};
		redisReply *reply = (redisReply*)this->SendCommand(2, argv, argvlen).p;
		if (reply == 0 || reply->type == REDIS_REPLY_ERROR)
		{
			if (reply)
				freeReplyObject(reply);
			redisFree(ctx);
			this->context = 0;
			return false;
		}
		freeReplyObject(reply);
	}
	if (db != 0)
	{
		UTF8Char dbStr[32];
		UnsafeArray<UTF8Char> dbEnd = Text::StrInt32(dbStr, db);
		UnsafeArray<const Char> argv[2] = {"SELECT", (const Char*)dbStr};
		UIntOS argvlen[2] = {6, (UIntOS)(dbEnd.Ptr() - dbStr)};
		redisReply *reply = (redisReply*)this->SendCommand(2, argv, argvlen).p;
		if (reply == 0 || reply->type == REDIS_REPLY_ERROR)
		{
			if (reply)
				freeReplyObject(reply);
			redisFree(ctx);
			this->context = 0;
			return false;
		}
		freeReplyObject(reply);
	}
	return true;
}

AnyType DB::RedisClient::SendCommand(Int32 argc, UnsafeArray<UnsafeArray<const Char>> argv, UnsafeArray<const UIntOS> argvlen) const
{
	if (this->context.IsNull() || ((redisContext*)this->context.p)->err)
		return 0;
	return redisCommandArgv((redisContext*)this->context.p, argc, (const Char**)argv.Ptr(), (const size_t*)argvlen.Ptr());
}

DB::RedisClient::RedisClient(Text::CStringNN host, UInt16 port) : IO::ConfigFile(CSTR("RedisClient"))
{
	this->context = 0;
	this->keyPrefix = Text::String::New(CSTR("SClass"));
	this->Connect(host, port, nullptr, 0);
}

DB::RedisClient::RedisClient(Text::CStringNN host, UInt16 port, Text::CString password, Int32 db, Text::CStringNN keyPrefix) : IO::ConfigFile(CSTR("RedisClient"))
{
	this->context = 0;
	this->keyPrefix = Text::String::New(keyPrefix);
	this->Connect(host, port, password, db);
}

DB::RedisClient::~RedisClient()
{
	if (!this->context.IsNull())
		redisFree((redisContext*)this->context.p);
	this->keyPrefix->Release();
}

Bool DB::RedisClient::IsConnected() const
{
	return !this->context.IsNull() && ((redisContext*)this->context.p)->err == 0;
}

Optional<Text::String> DB::RedisClient::GetCategoryKey(Text::CString category) const
{
	Text::StringBuilderUTF8 sb;
	sb.Append(this->keyPrefix);
	sb.AppendC(UTF8STRC(":category:"));
	sb.Append(category.OrEmpty());
	return Text::String::New(sb.ToString(), sb.GetLength());
}

Optional<Text::String> DB::RedisClient::GetCategoriesKey() const
{
	Text::StringBuilderUTF8 sb;
	sb.Append(this->keyPrefix);
	sb.AppendC(UTF8STRC(":categories"));
	return Text::String::New(sb.ToString(), sb.GetLength());
}

Optional<Text::String> DB::RedisClient::GetDefaultKey(Text::CStringNN name) const
{
	Text::StringBuilderUTF8 sb;
	sb.Append(this->keyPrefix);
	sb.AppendC(UTF8STRC(":default:"));
	sb.Append(name);
	return Text::String::New(sb.ToString(), sb.GetLength());
}

Optional<Text::String> DB::RedisClient::GetDefaultKeysKey() const
{
	Text::StringBuilderUTF8 sb;
	sb.Append(this->keyPrefix);
	sb.AppendC(UTF8STRC(":default:keys"));
	return Text::String::New(sb.ToString(), sb.GetLength());
}

Optional<Text::String> DB::RedisClient::GetCateValue(NN<Text::String> category, NN<Text::String> name)
{
	return this->GetCateValue(category->ToCString(), name->ToCString());
}

Optional<Text::String> DB::RedisClient::GetCateValue(Text::CStringNN category, Text::CStringNN name)
{
	if (category.leng == 0)
	{
		NN<Text::String> valueKey;
		if (!this->GetDefaultKey(name).SetTo(valueKey))
			return nullptr;
		UnsafeArray<const Char> argv[2] = {"GET", (const Char*)valueKey->v.Ptr()};
		UIntOS argvlen[2] = {3, valueKey->leng};
		redisReply *reply = (redisReply*)this->SendCommand(2, argv, argvlen).p;
		valueKey->Release();
		if (!reply)
			return nullptr;
		Optional<Text::String> ret = nullptr;
		if (reply->type == REDIS_REPLY_STRING)
			ret = Text::String::New((const UTF8Char*)reply->str, reply->len);
		freeReplyObject(reply);
		return ret;
	}
	NN<Text::String> categoryKey;
	if (!this->GetCategoryKey(category).SetTo(categoryKey))
		return nullptr;
	UnsafeArray<const Char> argv[3] = {"HGET", (const Char*)categoryKey->v.Ptr(), (const Char*)name.v.Ptr()};
	UIntOS argvlen[3] = {4, categoryKey->leng, name.leng};
	redisReply *reply = (redisReply*)this->SendCommand(3, argv, argvlen).p;
	categoryKey->Release();
	if (!reply)
		return nullptr;
	Optional<Text::String> ret = nullptr;
	if (reply->type == REDIS_REPLY_STRING)
		ret = Text::String::New((const UTF8Char*)reply->str, reply->len);
	freeReplyObject(reply);
	return ret;
}

Bool DB::RedisClient::SetValue(NN<Text::String> category, NN<Text::String> name, Optional<Text::String> value)
{
	NN<Text::String> valueNN;
	if (value.SetTo(valueNN))
		return this->SetValue(category->ToCString(), name->ToCString(), valueNN->ToCString());
	return this->SetValue(category->ToCString(), name->ToCString(), nullptr);
}

Bool DB::RedisClient::SetValue(Text::CStringNN category, Text::CStringNN name, Text::CString value)
{
	if (category.leng == 0)
	{
		NN<Text::String> valueKey;
		NN<Text::String> defaultKeysKey;
		NN<Text::String> categoriesKey;
		if (!this->GetDefaultKey(name).SetTo(valueKey) || !this->GetDefaultKeysKey().SetTo(defaultKeysKey) || !this->GetCategoriesKey().SetTo(categoriesKey))
			return false;
		Text::CStringNN valueNN;
		redisReply *reply;
		if (value.SetTo(valueNN))
		{
			UnsafeArray<const Char> argv[3] = {"SET", (const Char*)valueKey->v.Ptr(), (const Char*)valueNN.v.Ptr()};
			UIntOS argvlen[3] = {3, valueKey->leng, valueNN.leng};
			reply = (redisReply*)this->SendCommand(3, argv, argvlen).p;
		}
		else
		{
			UnsafeArray<const Char> argv[2] = {"DEL", (const Char*)valueKey->v.Ptr()};
			UIntOS argvlen[2] = {3, valueKey->leng};
			reply = (redisReply*)this->SendCommand(2, argv, argvlen).p;
		}
		valueKey->Release();
		if (!reply)
		{
			defaultKeysKey->Release();
			categoriesKey->Release();
			return false;
		}
		Bool success = reply->type != REDIS_REPLY_ERROR;
		freeReplyObject(reply);
		if (!success)
		{
			defaultKeysKey->Release();
			categoriesKey->Release();
			return false;
		}
		if (value.SetTo(valueNN))
		{
			UnsafeArray<const Char> argv[3] = {"SADD", (const Char*)defaultKeysKey->v.Ptr(), (const Char*)name.v.Ptr()};
			UIntOS argvlen[3] = {4, defaultKeysKey->leng, name.leng};
			reply = (redisReply*)this->SendCommand(3, argv, argvlen).p;
		}
		else
		{
			UnsafeArray<const Char> argv[3] = {"SREM", (const Char*)defaultKeysKey->v.Ptr(), (const Char*)name.v.Ptr()};
			UIntOS argvlen[3] = {4, defaultKeysKey->leng, name.leng};
			reply = (redisReply*)this->SendCommand(3, argv, argvlen).p;
		}
		defaultKeysKey->Release();
		if (!reply)
		{
			categoriesKey->Release();
			return false;
		}
		success = reply->type != REDIS_REPLY_ERROR;
		freeReplyObject(reply);
		if (success && value.SetTo(valueNN))
		{
			UnsafeArray<const Char> argv[3] = {"SADD", (const Char*)categoriesKey->v.Ptr(), (const Char*)category.v.Ptr()};
			UIntOS argvlen[3] = {4, categoriesKey->leng, category.leng};
			reply = (redisReply*)this->SendCommand(3, argv, argvlen).p;
			if (!reply)
				success = false;
			else
			{
				success = reply->type != REDIS_REPLY_ERROR;
				freeReplyObject(reply);
			}
		}
		categoriesKey->Release();
		return success;
	}
	NN<Text::String> categoryKey;
	NN<Text::String> categoriesKey;
	if (!this->GetCategoryKey(category).SetTo(categoryKey) || !this->GetCategoriesKey().SetTo(categoriesKey))
		return false;
	Text::CStringNN valueNN;
	redisReply *reply;
	if (value.SetTo(valueNN))
	{
		UnsafeArray<const Char> argv[4] = {"HSET", (const Char*)categoryKey->v.Ptr(), (const Char*)name.v.Ptr(), (const Char*)valueNN.v.Ptr()};
		UIntOS argvlen[4] = {4, categoryKey->leng, name.leng, valueNN.leng};
		reply = (redisReply*)this->SendCommand(4, argv, argvlen).p;
	}
	else
	{
		UnsafeArray<const Char> argv[3] = {"HDEL", (const Char*)categoryKey->v.Ptr(), (const Char*)name.v.Ptr()};
		UIntOS argvlen[3] = {4, categoryKey->leng, name.leng};
		reply = (redisReply*)this->SendCommand(3, argv, argvlen).p;
	}
	categoryKey->Release();
	if (!reply)
	{
		categoriesKey->Release();
		return false;
	}
	Bool success = reply->type != REDIS_REPLY_ERROR;
	freeReplyObject(reply);
	if (!success)
	{
		categoriesKey->Release();
		return false;
	}
	UnsafeArray<const Char> argv[3] = {"SADD", (const Char*)categoriesKey->v.Ptr(), (const Char*)category.v.Ptr()};
	UIntOS argvlen[3] = {4, categoriesKey->leng, category.leng};
	reply = (redisReply*)this->SendCommand(3, argv, argvlen).p;
	categoriesKey->Release();
	if (!reply)
		return false;
	success = reply->type != REDIS_REPLY_ERROR;
	freeReplyObject(reply);
	return success;
}

Bool DB::RedisClient::RemoveValue(Text::CString category, Text::CStringNN name)
{
	return this->SetValue(category.OrEmpty(), name, nullptr);
}

UIntOS DB::RedisClient::GetCateCount() const
{
	NN<Text::String> categoriesKey;
	if (!this->GetCategoriesKey().SetTo(categoriesKey))
		return 0;
	UnsafeArray<const Char> argv[2] = {"SCARD", (const Char*)categoriesKey->v.Ptr()};
	UIntOS argvlen[2] = {5, categoriesKey->leng};
	redisReply *reply = (redisReply*)this->SendCommand(2, argv, argvlen).p;
	categoriesKey->Release();
	if (!reply)
		return 0;
	UIntOS count = reply->type == REDIS_REPLY_INTEGER ? (UIntOS)reply->integer : 0;
	freeReplyObject(reply);
	return count;
}

UIntOS DB::RedisClient::GetCateList(NN<Data::ArrayListStringNN> cateList, Bool withEmpty)
{
	NN<Text::String> categoriesKey;
	if (!this->GetCategoriesKey().SetTo(categoriesKey))
		return 0;
	UnsafeArray<const Char> argv[2] = {"SMEMBERS", (const Char*)categoriesKey->v.Ptr()};
	UIntOS argvlen[2] = {8, categoriesKey->leng};
	redisReply *reply = (redisReply*)this->SendCommand(2, argv, argvlen).p;
	categoriesKey->Release();
	if (!reply)
		return 0;
	UIntOS count = 0;
	if (reply->type == REDIS_REPLY_ARRAY)
	{
		for (size_t i = 0; i < reply->elements; i++)
		{
			redisReply *item = reply->element[i];
			if (item->type == REDIS_REPLY_STRING && (withEmpty || item->len > 0))
			{
				cateList->Add(Text::String::New((const UTF8Char*)item->str, item->len));
				count++;
			}
		}
	}
	freeReplyObject(reply);
	return count;
}

UIntOS DB::RedisClient::GetKeys(NN<Text::String> category, NN<Data::ArrayListStringNN> keyList)
{
	return this->GetKeys(category->ToCString(), keyList);
}

UIntOS DB::RedisClient::GetKeys(Text::CStringNN category, NN<Data::ArrayListStringNN> keyList)
{
	if (category.leng == 0)
	{
		NN<Text::String> defaultKeysKey;
		if (!this->GetDefaultKeysKey().SetTo(defaultKeysKey))
			return 0;
		UnsafeArray<const Char> argv[2] = {"SMEMBERS", (const Char*)defaultKeysKey->v.Ptr()};
		UIntOS argvlen[2] = {8, defaultKeysKey->leng};
		redisReply *reply = (redisReply*)this->SendCommand(2, argv, argvlen).p;
		defaultKeysKey->Release();
		if (!reply)
			return 0;
		UIntOS count = 0;
		if (reply->type == REDIS_REPLY_ARRAY)
		{
			for (size_t i = 0; i < reply->elements; i++)
			{
				redisReply *item = reply->element[i];
				if (item->type == REDIS_REPLY_STRING)
				{
					keyList->Add(Text::String::New((const UTF8Char*)item->str, item->len));
					count++;
				}
			}
		}
		freeReplyObject(reply);
		return count;
	}
	NN<Text::String> categoryKey;
	if (!this->GetCategoryKey(category).SetTo(categoryKey))
		return 0;
	UnsafeArray<const Char> argv[2] = {"HKEYS", (const Char*)categoryKey->v.Ptr()};
	UIntOS argvlen[2] = {5, categoryKey->leng};
	redisReply *reply = (redisReply*)this->SendCommand(2, argv, argvlen).p;
	categoryKey->Release();
	if (!reply)
		return 0;
	UIntOS count = 0;
	if (reply->type == REDIS_REPLY_ARRAY)
	{
		for (size_t i = 0; i < reply->elements; i++)
		{
			redisReply *item = reply->element[i];
			if (item->type == REDIS_REPLY_STRING)
			{
				keyList->Add(Text::String::New((const UTF8Char*)item->str, item->len));
				count++;
			}
		}
	}
	freeReplyObject(reply);
	return count;
}

UIntOS DB::RedisClient::GetCount(Text::CString category) const
{
	if (category.IsNull() || category.leng == 0)
	{
		NN<Text::String> defaultKeysKey;
		if (!this->GetDefaultKeysKey().SetTo(defaultKeysKey))
			return 0;
		UnsafeArray<const Char> argv[2] = {"SCARD", (const Char*)defaultKeysKey->v.Ptr()};
		UIntOS argvlen[2] = {5, defaultKeysKey->leng};
		redisReply *reply = (redisReply*)this->SendCommand(2, argv, argvlen).p;
		defaultKeysKey->Release();
		if (!reply)
			return 0;
		UIntOS count = reply->type == REDIS_REPLY_INTEGER ? (UIntOS)reply->integer : 0;
		freeReplyObject(reply);
		return count;
	}
	NN<Text::String> categoryKey;
	if (!this->GetCategoryKey(category).SetTo(categoryKey))
		return 0;
	UnsafeArray<const Char> argv[2] = {"HLEN", (const Char*)categoryKey->v.Ptr()};
	UIntOS argvlen[2] = {4, categoryKey->leng};
	redisReply *reply = (redisReply*)this->SendCommand(2, argv, argvlen).p;
	categoryKey->Release();
	if (!reply)
		return 0;
	UIntOS count = reply->type == REDIS_REPLY_INTEGER ? (UIntOS)reply->integer : 0;
	freeReplyObject(reply);
	return count;
}

Optional<Text::String> DB::RedisClient::GetKey(Text::CString category, UIntOS index) const
{
	if (category.IsNull() || category.leng == 0)
	{
		NN<Text::String> defaultKeysKey;
		if (!this->GetDefaultKeysKey().SetTo(defaultKeysKey))
			return nullptr;
		UnsafeArray<const Char> argv[2] = {"SMEMBERS", (const Char*)defaultKeysKey->v.Ptr()};
		UIntOS argvlen[2] = {8, defaultKeysKey->leng};
		redisReply *reply = (redisReply*)this->SendCommand(2, argv, argvlen).p;
		defaultKeysKey->Release();
		if (!reply)
			return nullptr;
		Optional<Text::String> ret = nullptr;
		if (reply->type == REDIS_REPLY_ARRAY && index < reply->elements)
		{
			redisReply *item = reply->element[index];
			if (item->type == REDIS_REPLY_STRING)
				ret = Text::String::New((const UTF8Char*)item->str, item->len);
		}
		freeReplyObject(reply);
		return ret;
	}
	NN<Text::String> categoryKey;
	if (!this->GetCategoryKey(category).SetTo(categoryKey))
		return nullptr;
	UnsafeArray<const Char> argv[2] = {"HKEYS", (const Char*)categoryKey->v.Ptr()};
	UIntOS argvlen[2] = {5, categoryKey->leng};
	redisReply *reply = (redisReply*)this->SendCommand(2, argv, argvlen).p;
	categoryKey->Release();
	if (!reply)
		return nullptr;
	Optional<Text::String> ret = nullptr;
	if (reply->type == REDIS_REPLY_ARRAY && index < reply->elements)
	{
		redisReply *item = reply->element[index];
		if (item->type == REDIS_REPLY_STRING)
			ret = Text::String::New((const UTF8Char*)item->str, item->len);
	}
	freeReplyObject(reply);
	return ret;
}

Bool DB::RedisClient::HasCategory(Text::CString category) const
{
	NN<Text::String> categoriesKey;
	if (!this->GetCategoriesKey().SetTo(categoriesKey))
		return false;
	Text::CStringNN categoryNN = category.OrEmpty();
	UnsafeArray<const Char> argv[3] = {"SISMEMBER", (const Char*)categoriesKey->v.Ptr(), (const Char*)categoryNN.v.Ptr()};
	UIntOS argvlen[3] = {9, categoriesKey->leng, categoryNN.leng};
	redisReply *reply = (redisReply*)this->SendCommand(3, argv, argvlen).p;
	categoriesKey->Release();
	if (!reply)
		return false;
	Bool ret = reply->type == REDIS_REPLY_INTEGER && reply->integer != 0;
	freeReplyObject(reply);
	return ret;
}