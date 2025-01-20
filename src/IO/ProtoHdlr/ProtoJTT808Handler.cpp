#include "Stdafx.h"
#include "MyMemory.h"
#include "Data/ByteTool.h"
#include "IO/ProtoHdlr/ProtoJTT808Handler.h"

typedef struct
{
	UInt8 devId[6];
} JTT808StreamData;

IO::ProtoHdlr::ProtoJTT808Handler::ProtoJTT808Handler(NN<IO::ProtocolHandler::DataListener> listener, UInt64 devId)
{
	this->listener = listener;
	this->devId = devId;
}

IO::ProtoHdlr::ProtoJTT808Handler::~ProtoJTT808Handler()
{
}

AnyType IO::ProtoHdlr::ProtoJTT808Handler::CreateStreamData(NN<IO::Stream> stm)
{
	JTT808StreamData *data = MemAlloc(JTT808StreamData, 1);
	UInt64 devId = this->devId;
	data->devId[5] = Data::ByteTool::Int2BCDB((UInt32)(devId % 100));
	devId = devId / 100;
	data->devId[4] = Data::ByteTool::Int2BCDB((UInt32)(devId % 100));
	devId = devId / 100;
	data->devId[3] = Data::ByteTool::Int2BCDB((UInt32)(devId % 100));
	devId = devId / 100;
	data->devId[2] = Data::ByteTool::Int2BCDB((UInt32)(devId % 100));
	devId = devId / 100;
	data->devId[1] = Data::ByteTool::Int2BCDB((UInt32)(devId % 100));
	devId = devId / 100;
	data->devId[0] = Data::ByteTool::Int2BCDB((UInt32)(devId % 100));
	return data;
}

void IO::ProtoHdlr::ProtoJTT808Handler::DeleteStreamData(NN<IO::Stream> stm, AnyType stmData)
{
	NN<JTT808StreamData> data = stmData.GetNN<JTT808StreamData>();
	MemFree(data.Ptr());
}

UOSInt IO::ProtoHdlr::ProtoJTT808Handler::ParseProtocol(NN<IO::Stream> stm, AnyType stmObj, AnyType stmData, const Data::ByteArrayR &buff)
{
	NN<JTT808StreamData> data = stmData.GetNN<JTT808StreamData>();
	UInt8 packetBuff[1044];
	UOSInt firstIndex = (UOSInt)-1;
	UOSInt parseOfst = 0;
	UOSInt i;
	UOSInt j;
	i = 0;
	while (i < buff.GetSize())
	{
		if (buff[i] == 0x7e)
		{
			if (firstIndex == (UOSInt)-1)
			{
				firstIndex = i;
			}
			else
			{
				UInt8 chk = 0;
				UInt8 c;
				j = firstIndex + 1;
				while (j < i)
				{
					c = buff[j];
					if (c == 0x7d)
					{
						if (buff[j + 1] == 1)
						{
							j++;
						}
						else if (buff[j + 1] == 2)
						{
							c = 0x7e;
							j++;
						}
					}
					chk = chk ^ c;
					j++;
				}
				if (chk == 0)
				{
					if (parseOfst != firstIndex)
					{
						this->listener->DataSkipped(stm, stmObj, &buff[parseOfst], firstIndex - parseOfst);
					}

					j = firstIndex + 1;
					packetBuff[0] = 0x7e;
					parseOfst = 1;
					while (j < i)
					{
						c = buff[j];
						if (c == 0x7d)
						{
							if (buff[j + 1] == 1)
							{
								j++;
							}
							else if (buff[j + 1] == 2)
							{
								c = 0x7e;
								j++;
							}
						}
						packetBuff[parseOfst] = c;
						j++;
						parseOfst++;
					}
					packetBuff[parseOfst] = 0x7e;
					data->devId[0] = packetBuff[5];
					data->devId[1] = packetBuff[6];
					data->devId[2] = packetBuff[7];
					data->devId[3] = packetBuff[8];
					data->devId[4] = packetBuff[9];
					data->devId[5] = packetBuff[10];
					this->listener->DataParsed(stm, stmObj, ReadMUInt16(&packetBuff[1]), ReadMUInt16(&packetBuff[11]), packetBuff, parseOfst + 1);

					firstIndex = (UOSInt)-1;
					parseOfst = i + 1;
				}
				else
				{
					this->listener->DataSkipped(stm, stmObj, &buff[parseOfst], i - parseOfst);
					firstIndex = i;
					parseOfst = i;
				}
			}
		}
		i++;
	}
	return buff.GetSize() - parseOfst;
}

UOSInt IO::ProtoHdlr::ProtoJTT808Handler::BuildPacket(UnsafeArray<UInt8> buff, Int32 cmdType, Int32 seqId, UnsafeArray<const UInt8> cmd, UOSInt cmdSize, AnyType stmData)
{
	NN<JTT808StreamData> data;
	UInt8 hdr[12];
	if (cmdSize >= 1024)
		return 0;
	WriteMInt16(&hdr[0], cmdType);
	WriteMInt16(&hdr[2], cmdSize);
	WriteMInt16(&hdr[10], seqId);
	if (stmData.GetOpt<JTT808StreamData>().SetTo(data))
	{
		hdr[4] = data->devId[0];
		hdr[5] = data->devId[1];
		hdr[6] = data->devId[2];
		hdr[7] = data->devId[3];
		hdr[8] = data->devId[4];
		hdr[9] = data->devId[5];
	}
	else
	{
		UInt64 devId = (UInt64)this->devId;
		hdr[9] = Data::ByteTool::Int2BCDB((UInt32)(devId % 100));
		devId = devId / 100;
		hdr[8] = Data::ByteTool::Int2BCDB((UInt32)(devId % 100));
		devId = devId / 100;
		hdr[7] = Data::ByteTool::Int2BCDB((UInt32)(devId % 100));
		devId = devId / 100;
		hdr[6] = Data::ByteTool::Int2BCDB((UInt32)(devId % 100));
		devId = devId / 100;
		hdr[5] = Data::ByteTool::Int2BCDB((UInt32)(devId % 100));
		devId = devId / 100;
		hdr[4] = Data::ByteTool::Int2BCDB((UInt32)(devId % 100));
	}
	UInt8 chk = 0;
	UInt8 c;
	UOSInt i;
	UOSInt j;
	i = 1;
	buff[0] = 0x7e;
	j = 0;
	while (j < 12)
	{
		c = hdr[j];
		chk = chk ^ c;
		if (c == 0x7e)
		{
			buff[i] = 0x7d;
			buff[i + 1] = 2;
			i += 2;
		}
		else if (c == 0x7d)
		{
			buff[i] = 0x7d;
			buff[i + 1] = 1;
			i += 2;
		}
		else
		{
			buff[i] = c;
			i++;
		}
		j++;
	}
	j = 0;
	while (j < cmdSize)
	{
		c = cmd[j];
		chk = chk ^ c;
		if (c == 0x7e)
		{
			buff[i] = 0x7d;
			buff[i + 1] = 2;
			i += 2;
		}
		else if (c == 0x7d)
		{
			buff[i] = 0x7d;
			buff[i + 1] = 1;
			i += 2;
		}
		else
		{
			buff[i] = c;
			i++;
		}
		j++;
	}
	buff[i] = chk;
	buff[i + 1] = 0x7e;
	i += 2;
	return i;
}

UnsafeArray<const UInt8> IO::ProtoHdlr::ProtoJTT808Handler::GetPacketContent(UnsafeArray<const UInt8> packet, OutParam<UOSInt> contSize)
{
	UInt16 size;
	if (packet[0] != 0x7e)
	{
		contSize.Set(0);
		return packet;
	}
	size = ReadMUInt16(&packet[3]);
	contSize.Set(size & 0x3ff);
	if (size & 0x2000)
	{
		return packet + 17;
	}
	else
	{
		return packet + 13;
	}
}
