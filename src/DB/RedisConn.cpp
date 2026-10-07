#include "Stdafx.h"
#include "DB/RedisConn.h"

//#define VERBOSE

struct RedisConn_ClassData
{
	NN<Net::TCPClientFactory> clif;
	Optional<Net::TCPClient> cli;
	UInt8 dataBuff[16384];
	UIntOS dataSize;
	UIntOS dataPos;
};

Text::CString RedisConn_NextLine(NN<RedisConn_ClassData> clsData, NN<Net::TCPClient> cli)
{
	if (clsData->dataPos >= clsData->dataSize)
	{
		clsData->dataSize = cli->Read(Data::ByteArray(clsData->dataBuff, sizeof(clsData->dataBuff)));
		clsData->dataPos = 0;
		if (clsData->dataSize == 0)
			return nullptr;
	}
	while (true)
	{
		UIntOS lineStart = clsData->dataPos;
		UIntOS currIndex = lineStart;
		while (currIndex < clsData->dataSize)
		{
			if (clsData->dataBuff[currIndex] == '\r' && currIndex + 1 < clsData->dataSize && clsData->dataBuff[currIndex + 1] == '\n')
			{
				clsData->dataBuff[currIndex] = 0;
				Text::CStringNN ret((const UTF8Char*)&clsData->dataBuff[lineStart], currIndex - lineStart);
				clsData->dataPos = currIndex + 2;
				return ret;
			}
			currIndex++;
		}
		if (clsData->dataPos > 0)
		{
			MemCopyO(clsData->dataBuff, &clsData->dataBuff[clsData->dataPos], clsData->dataSize - clsData->dataPos);
			clsData->dataSize -= clsData->dataPos;
			clsData->dataPos = 0;
		}
		lineStart = cli->Read(Data::ByteArray(&clsData->dataBuff[clsData->dataSize], sizeof(clsData->dataBuff) - clsData->dataSize));
		clsData->dataSize += lineStart;
		if (lineStart == 0)
			return nullptr;
	}
}

Text::CString RedisConn_NextLineSize(NN<RedisConn_ClassData> clsData, NN<Net::TCPClient> cli, UIntOS size)
{
	if (clsData->dataPos >= clsData->dataSize)
	{
		clsData->dataSize = cli->Read(Data::ByteArray(clsData->dataBuff, sizeof(clsData->dataBuff)));
		clsData->dataPos = 0;
		if (clsData->dataSize == 0)
			return nullptr;
	}
	if (size + 2 > sizeof(clsData->dataBuff))
		return nullptr;
	UIntOS lineStart;
	while (clsData->dataSize - clsData->dataPos < size + 2)
	{
		if (clsData->dataPos > 0)
		{
			MemCopyO(clsData->dataBuff, &clsData->dataBuff[clsData->dataPos], clsData->dataSize - clsData->dataPos);
			clsData->dataSize -= clsData->dataPos;
			clsData->dataPos = 0;
		}
		lineStart = cli->Read(Data::ByteArray(&clsData->dataBuff[clsData->dataSize], sizeof(clsData->dataBuff) - clsData->dataSize));
		clsData->dataSize += lineStart;
		if (lineStart == 0)
			return nullptr;
	}
	if (clsData->dataBuff[clsData->dataPos + size] == '\r' && clsData->dataBuff[clsData->dataPos + size + 1] == '\n')
	{
		Text::CStringNN ret((const UTF8Char*)&clsData->dataBuff[clsData->dataPos], size);
		clsData->dataPos += size + 2;
		return ret;
	}
	return nullptr;
}

void RedisConn_FreePartial(NN<DB::RedisConn::ReplyData> ret, UIntOS currIndex)
{
	if (ret->type == DB::RedisConn::DataType::Array)
	{
		while (currIndex-- > 0)
		{
			DB::RedisConn::FreeReplyDataInner(ret->arr.items[currIndex]);
		}
		MemFreeArr(ret->arr.items);
	}
	else if (ret->type == DB::RedisConn::DataType::String)
	{
		ret->str->Release();
	}
	ret->type = DB::RedisConn::DataType::Null;
}

Bool RedisConn_ParseData(NN<RedisConn_ClassData> clsData, NN<Net::TCPClient> cli, NN<DB::RedisConn::ReplyData> data)
{
	Text::CStringNN line;
	if (!RedisConn_NextLine(clsData, cli).SetTo(line))
		return false;
	if (line.v[0] == '$')
	{
		IntOS bulkSize;
		if (!line.Substring(1).ToIntOS(bulkSize) || bulkSize < -1)
		{
			printf("RedisConn: Unexpected bulk size: %s\r\n", line.v.Ptr());
			return false;
		}
		if (bulkSize == -1)
		{
			data->type = DB::RedisConn::DataType::Null;
			return true;
		}
		else
		{
			if (!RedisConn_NextLine(clsData, cli).SetTo(line))
			{
				printf("RedisConn: Error in reading bulk line\r\n");
				return false;
			}
			data->type = DB::RedisConn::DataType::String;
			data->str = Text::String::New(line);
			return true;
		}
	}
	else if (line.v[0] == '*')
	{
		UIntOS arraySize;
		if (!line.Substring(1).ToUIntOS(arraySize))
		{
			printf("RedisConn: Unexpected array size: %s\r\n", line.v.Ptr());
			return false;
		}
		data->type = DB::RedisConn::DataType::Array;
		data->arr.size = arraySize;
		data->arr.items = MemAllocArr(DB::RedisConn::ReplyData, arraySize);
		UIntOS i = 0;
		while (i < arraySize)
		{
			if (!RedisConn_ParseData(clsData, cli, data->arr.items[i]))
			{
				RedisConn_FreePartial(data, i);
				return false;
			}
			i++;
		}
		return true;
	}
	else if (line.v[0] == ':')
	{
		if (!line.Substring(1).ToInt64(data->integer))
		{
			printf("RedisConn: Unexpected integer value: %s\r\n", line.v.Ptr());
			return false;
		}
		data->type = DB::RedisConn::DataType::Integer;
		return true;
	}
	else if (line.Equals(CSTR("_")))
	{
		data->type = DB::RedisConn::DataType::Null;
		return true;
	}
	else if (line.v[0] == '#')
	{
		data->type = DB::RedisConn::DataType::Integer;
		data->integer = (line.v[1] == 't' || line.v[1] == 'T') ? 1 : 0;
		return true;
	}
	else if (line.v[0] == ',')
	{
		data->type = DB::RedisConn::DataType::Double;
		if (!line.Substring(1).ToDouble(data->dbl))
		{
			printf("RedisConn: Unexpected double value: %s\r\n", line.v.Ptr());
			return false;
		}
		return true;
	}
	else if (line.v[0] == '(')
	{
		data->type = DB::RedisConn::DataType::String;
		data->str = Text::String::New(line.Substring(1));
		return true;
	}
	else
	{
		printf("RedisConn: Unknown data type: %s\r\n", line.v.Ptr());
		return false;
	}
}

Optional<DB::RedisConn::ReplyInfo> DB::RedisConn::SendCommand(Int32 argc, UnsafeArray<UnsafeArray<const Char>> argv, UnsafeArray<const UIntOS> argvlen) const
{
	NN<RedisConn_ClassData> clsData = this->context.GetNN<RedisConn_ClassData>();
	NN<Net::TCPClient> cli;
	if (!clsData->cli.SetTo(cli))
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
	Text::StringBuilderUTF8 sb;
	sb.AppendUTF8Char('*');
	sb.AppendI32(argc);
	sb.AppendC(UTF8STRC("\r\n"));
	Int32 i = 0;
	while (i < argc)
	{
		sb.AppendUTF8Char('$');
		sb.AppendUIntOS(argvlen[i]);
		sb.AppendC(UTF8STRC("\r\n"));
		sb.AppendC(UnsafeArray<const UTF8Char>::ConvertFrom(argv[i]), argvlen[i]);
		sb.AppendC(UTF8STRC("\r\n"));
		i++;
	}
	clsData->dataPos = 0;
	clsData->dataSize = 0;
	if (cli->WriteCont(sb.v, sb.leng) != sb.leng)
	{
		clsData->cli.Delete();
		return nullptr;
	}
	Text::CStringNN line;
	if (!RedisConn_NextLine(clsData, cli).SetTo(line))
		return nullptr;
	NN<DB::RedisConn::ReplyInfo> ret = MemAllocNN(DB::RedisConn::ReplyInfo);
	if (line.v[0] == '+')
	{
		ret->replyType = DB::RedisConn::ReplyType::Status;
		ret->type = DB::RedisConn::DataType::String;
		ret->str = Text::String::New(line.Substring(1));
		return ret;
	}
	else if (line.v[0] == '-')
	{
		ret->replyType = DB::RedisConn::ReplyType::Error;
		ret->type = DB::RedisConn::DataType::String;
		ret->str = Text::String::New(line.Substring(1));
		return ret;
	}
	else if (line.v[0] == '*')
	{
		ret->replyType = DB::RedisConn::ReplyType::Array;
		ret->type = DB::RedisConn::DataType::Array;
		Int32 arraySize;
		if (!line.Substring(1).ToInt32(arraySize) || arraySize < 0)
		{
			MemFreeNN(ret);
			printf("RedisConn: Unexpected array size: %s\r\n", line.v.Ptr());
			return nullptr;
		}
		ret->arr.size = (UIntOS)arraySize;
		ret->arr.items = MemAllocArr(DB::RedisConn::ReplyData, (UIntOS)arraySize);
		Int32 i = 0;
		while (i < arraySize)
		{
			if (!RedisConn_ParseData(clsData, cli, ret->arr.items[(UIntOS)i]))
			{
				printf("RedisConn: Array parse error at index %d\r\n", i);
				RedisConn_FreePartial(ret, (UIntOS)i);
				MemFreeNN(ret);
				return nullptr;
			}
			i++;
		}
		return ret;
	}
	else if (line.v[0] == '$')
	{
		Int32 strLen;
		if (!line.Substring(1).ToInt32(strLen) || strLen < -1)
		{
			MemFreeNN(ret);
			printf("RedisConn: Unexpected string length: %s\r\n", line.v.Ptr());
			return nullptr;
		}
		ret->replyType = DB::RedisConn::ReplyType::String;
		if (strLen == -1)
		{
			ret->type = DB::RedisConn::DataType::Null;
		}
		else
		{
			ret->type = DB::RedisConn::DataType::String;
			if (RedisConn_NextLineSize(clsData, cli, (UIntOS)strLen).SetTo(line))
			{
				ret->str = Text::String::New(line);
			}
			else
			{
				MemFreeNN(ret);
				return nullptr;
			}
		}
		return ret;
	}
	else if (line.v[0] == ':')
	{
		ret->replyType = DB::RedisConn::ReplyType::Integer;
		ret->type = DB::RedisConn::DataType::Integer;
		if (!line.Substring(1).ToInt64(ret->integer))
		{
			MemFreeNN(ret);
			printf("RedisConn: Unexpected integer value: %s\r\n", line.v.Ptr());
			return nullptr;
		}
		return ret;
	}
	else if (line.Equals(CSTR("_")))
	{
		ret->replyType = DB::RedisConn::ReplyType::Nil;
		ret->type = DB::RedisConn::DataType::Null;
		return ret;
	}
	else if (line.v[0] == '#')
	{
		ret->replyType = DB::RedisConn::ReplyType::Bool;
		ret->type = DB::RedisConn::DataType::Integer;
		ret->integer = (line.v[1] == 't' || line.v[1] == 'T') ? 1 : 0;
		return ret;
	}
	else if (line.v[0] == ',')
	{
		ret->replyType = DB::RedisConn::ReplyType::Double;
		ret->type = DB::RedisConn::DataType::Double;
		if (!line.Substring(1).ToDouble(ret->dbl))
		{
			MemFreeNN(ret);
			printf("RedisConn: Unexpected double value: %s\r\n", line.v.Ptr());
			return nullptr;
		}
		return ret;
	}
	else if (line.v[0] == '(')
	{
		ret->replyType = DB::RedisConn::ReplyType::BigNum;
		ret->type = DB::RedisConn::DataType::String;
		ret->str = Text::String::New(line.Substring(1));
		return ret;
	}
	else
	{
		printf("RedisConn: Unexpected reply: %s\r\n", line.v.Ptr());
		MemFreeNN(ret);
		return nullptr;
	}
}

DB::RedisConn::RedisConn(NN<Net::TCPClientFactory> clif, Text::CStringNN host, UInt16 port)
{
	NN<RedisConn_ClassData> clsData = MemAllocNN(RedisConn_ClassData);
	clsData->clif = clif;
	clsData->cli = nullptr;
	this->context = clsData;
	this->host = Text::String::New(host);
	this->port = port;
	this->Reconnect();
}

DB::RedisConn::~RedisConn()
{
	NN<RedisConn_ClassData> clsData = this->context.GetNN<RedisConn_ClassData>();
	clsData->cli.Delete();
	MemFreeNN(clsData);
	this->host->Release();
}

Bool DB::RedisConn::IsConnected() const
{
	NN<RedisConn_ClassData> clsData = this->context.GetNN<RedisConn_ClassData>();
	return !clsData->cli.IsNull();
}

Bool DB::RedisConn::Reconnect()
{
	NN<RedisConn_ClassData> clsData = this->context.GetNN<RedisConn_ClassData>();
	clsData->cli.Delete();
	clsData->dataSize = 0;
	clsData->dataPos = 0;
	clsData->cli = clsData->clif->Create(this->host->ToCString(), this->port, 4000);
	NN<Net::TCPClient> cli;
	if (clsData->cli.SetTo(cli))
	{
		cli->SetNoDelay(true);
		cli->SetTimeout(4000);
		return true;
	}
	return false;
}