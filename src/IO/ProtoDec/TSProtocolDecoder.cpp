#include "Stdafx.h"
#include "MyMemory.h"
#include "Crypto/Encrypt/Base64.h"
#include "Data/ByteTool.h"
#include "IO/ProtoDec/TSProtocolDecoder.h"
#include "Text/MyStringFloat.h"
#include "Text/StringBuilderUTF8.h"

IO::ProtoDec::TSProtocolDecoder::TSProtocolDecoder()
{
	this->protoBuff = MemAlloc(UInt8, 4096);
}

IO::ProtoDec::TSProtocolDecoder::~TSProtocolDecoder()
{
	MemFree(this->protoBuff);
}

Text::CString IO::ProtoDec::TSProtocolDecoder::GetName()
{
	return CSTR("TS");
}

UOSInt IO::ProtoDec::TSProtocolDecoder::ParseProtocol(ProtocolInfo hdlr, void *userObj, UInt64 fileOfst, UInt8 *buff, UOSInt buffSize)
{
	UOSInt i;
	UOSInt j;
	UInt32 cmdSize;
	UInt16 cmdType;
	Int32 seqId;

	Text::StringBuilderUTF8 sb;
	j = 0;
	i = 0;
	while (i < buffSize - 9)
	{
		if (buff[i] == 'T' && buff[i + 1] == 's')
		{
			if (j != i)
			{
				hdlr(userObj, fileOfst + j, i - j, CSTR("Unknown Protocol"));
			}
			j = i;
			cmdSize = ReadUInt16(&buff[i + 2]);
			cmdType = ReadUInt16(&buff[i + 4]);
			seqId = ReadUInt16(&buff[i + 6]);
			if (i + cmdSize > buffSize)
				return j;
			sb.ClearStr();
			sb.AppendC(UTF8STRC("0x"));
			sb.AppendHex16(cmdType);
			sb.AppendC(UTF8STRC(", seq="));
			sb.AppendI32(seqId);
			sb.AppendC(UTF8STRC(", "));
			switch (cmdType)
			{
			case 0:
				sb.AppendC(UTF8STRC("KA (Svr->Cli)"));
				break;
			case 1:
				sb.AppendC(UTF8STRC("KA Reply"));
				break;
			case 2:
				sb.AppendC(UTF8STRC("Dev Log Message"));
				break;
			case 3:
				sb.AppendC(UTF8STRC("Measure Circle"));
				break;
			case 4:
				sb.AppendC(UTF8STRC("Cancel Working Task"));
				break;
			case 5:
				sb.AppendC(UTF8STRC("Cancel Working Task Reply"));
				break;
			case 6:
				sb.AppendC(UTF8STRC("Send Station Setup"));
				break;
			case 7:
				sb.AppendC(UTF8STRC("Reply Station Setup"));
				break;
			case 8:
				sb.AppendC(UTF8STRC("Face North"));
				break;
			case 9:
				sb.AppendC(UTF8STRC("Face South"));
				break;
			case 10:
				sb.AppendC(UTF8STRC("Send Task List"));
				break;
			case 11:
				sb.AppendC(UTF8STRC("Send Task List Reply"));
				break;
			case 12:
				sb.AppendC(UTF8STRC("Check Task Status"));
				break;
			case 13:
				sb.AppendC(UTF8STRC("Task Sync Completed"));
				break;
			case 14:
				sb.AppendC(UTF8STRC("Request Task Detail"));
				break;
			case 15:
				sb.AppendC(UTF8STRC("Request Task Detail Reply"));
				break;
			case 16:
				sb.AppendC(UTF8STRC("Request Area Detail"));
				break;
			case 17:
				sb.AppendC(UTF8STRC("Request Area Detail Reply"));
				break;
			case 18:
				sb.AppendC(UTF8STRC("Cancel Task"));
				break;
			case 20:
				sb.AppendC(UTF8STRC("Task Status Update"));
				break;
			case 21:
				sb.AppendC(UTF8STRC("Task Status Update Reply"));
				break;
			case 22:
				sb.AppendC(UTF8STRC("Remove Task"));
				break;
			case 23:
				sb.AppendC(UTF8STRC("Remove Task Reply"));
				break;
			case 24:
				sb.AppendC(UTF8STRC("Request Task File"));
				break;
			case 25:
				sb.AppendC(UTF8STRC("Task File Detail"));
				break;
			case 26:
				sb.AppendC(UTF8STRC("Transfer File"));
				break;
			case 27:
				sb.AppendC(UTF8STRC("End Transfer File"));
				break;
			case 28:
				sb.AppendC(UTF8STRC("Request Dev Task List"));
				break;
			case 29:
				sb.AppendC(UTF8STRC("Dev Task List Resp"));
				break;
			case 30:
				sb.AppendC(UTF8STRC("Reopen"));
				break;
			case 31:
				sb.AppendC(UTF8STRC("Move To Angle"));
				break;
			case 32:
				sb.AppendC(UTF8STRC("Request To Send Dummy Data"));
				break;
			case 33:
				sb.AppendC(UTF8STRC("Send Dummy Data"));
				break;
			case 34:
				sb.AppendC(UTF8STRC("Get Current Angle"));
				break;
			case 35:
				sb.AppendC(UTF8STRC("Get Current Angle Reply"));
				break;
			case 36:
				sb.AppendC(UTF8STRC("Turn Laser"));
				break;
			default:
				sb.AppendC(UTF8STRC("Unknown Protocol"));
				break;
			}
			hdlr(userObj, fileOfst + j, cmdSize, sb.ToCString());
			j += cmdSize;
			i = j;
		}
		else if (buff[i] == 'T' && buff[i + 1] == 'S')
		{
			UOSInt tmpVal1;
			UOSInt tmpVal2;
			Crypto::Encrypt::Base64 b64;

			if (j != i)
			{
				hdlr(userObj, fileOfst + j, i - j, CSTR("Unknown Protocol"));
			}
			j = i;
			b64.Decrypt(&buff[i + 2], 8, this->protoBuff, 0);
			cmdSize = ReadUInt16(&this->protoBuff[0]);
			cmdType = ReadUInt16(&this->protoBuff[2]);
			seqId = ReadUInt16(&this->protoBuff[4]);

			tmpVal1 = cmdSize / 3;
			if (tmpVal1 * 3 < cmdSize)
			{
				tmpVal2 = (tmpVal1 + 1) * 4;
			}
			else
			{
				tmpVal2 = tmpVal1 * 4;
			}
			if (i + tmpVal2 + 12 > buffSize)
				return j;
			sb.ClearStr();
			sb.AppendC(UTF8STRC("0x"));
			sb.AppendHex16(cmdType);
			sb.AppendC(UTF8STRC(", seq="));
			sb.AppendI32(seqId);
			sb.AppendC(UTF8STRC(", "));
			switch (cmdType)
			{
			case 0:
				sb.AppendC(UTF8STRC("KA (Svr->Cli)"));
				break;
			case 1:
				sb.AppendC(UTF8STRC("KA Reply"));
				break;
			case 2:
				sb.AppendC(UTF8STRC("Dev Log Message"));
				break;
			case 3:
				sb.AppendC(UTF8STRC("Measure Circle"));
				break;
			case 4:
				sb.AppendC(UTF8STRC("Cancel Working Task"));
				break;
			case 5:
				sb.AppendC(UTF8STRC("Cancel Working Task Reply"));
				break;
			case 6:
				sb.AppendC(UTF8STRC("Send Station Setup"));
				break;
			case 7:
				sb.AppendC(UTF8STRC("Reply Station Setup"));
				break;
			case 8:
				sb.AppendC(UTF8STRC("Face North"));
				break;
			case 9:
				sb.AppendC(UTF8STRC("Face South"));
				break;
			case 10:
				sb.AppendC(UTF8STRC("Send Task List"));
				break;
			case 11:
				sb.AppendC(UTF8STRC("Send Task List Reply"));
				break;
			case 12:
				sb.AppendC(UTF8STRC("Check Task Status"));
				break;
			case 13:
				sb.AppendC(UTF8STRC("Task Sync Completed"));
				break;
			case 14:
				sb.AppendC(UTF8STRC("Request Task Detail"));
				break;
			case 15:
				sb.AppendC(UTF8STRC("Request Task Detail Reply"));
				break;
			case 16:
				sb.AppendC(UTF8STRC("Request Area Detail"));
				break;
			case 17:
				sb.AppendC(UTF8STRC("Request Area Detail Reply"));
				break;
			case 18:
				sb.AppendC(UTF8STRC("Cancel Task"));
				break;
			case 20:
				sb.AppendC(UTF8STRC("Task Status Update"));
				break;
			case 21:
				sb.AppendC(UTF8STRC("Task Status Update Reply"));
				break;
			case 22:
				sb.AppendC(UTF8STRC("Remove Task"));
				break;
			case 23:
				sb.AppendC(UTF8STRC("Remove Task Reply"));
				break;
			case 24:
				sb.AppendC(UTF8STRC("Request Task File"));
				break;
			case 25:
				sb.AppendC(UTF8STRC("Task File Detail"));
				break;
			case 26:
				sb.AppendC(UTF8STRC("Transfer File"));
				break;
			case 27:
				sb.AppendC(UTF8STRC("End Transfer File"));
				break;
			case 28:
				sb.AppendC(UTF8STRC("Request Dev Task List"));
				break;
			case 29:
				sb.AppendC(UTF8STRC("Dev Task List Resp"));
				break;
			case 30:
				sb.AppendC(UTF8STRC("Reopen"));
				break;
			case 31:
				sb.AppendC(UTF8STRC("Move To Angle"));
				break;
			case 32:
				sb.AppendC(UTF8STRC("Request To Send Dummy Data"));
				break;
			case 33:
				sb.AppendC(UTF8STRC("Send Dummy Data"));
				break;
			case 34:
				sb.AppendC(UTF8STRC("Get Current Angle"));
				break;
			case 35:
				sb.AppendC(UTF8STRC("Get Current Angle Reply"));
				break;
			case 36:
				sb.AppendC(UTF8STRC("Turn Laser"));
				break;
			default:
				sb.AppendC(UTF8STRC("Unknown Protocol"));
				break;
			}
			hdlr(userObj, fileOfst + j, tmpVal2 + 12, sb.ToCString());
			j += tmpVal2 + 12;
			i = j;
		}
		else
		{
			i++;
		}
	}
	return j;
}

Bool IO::ProtoDec::TSProtocolDecoder::GetProtocolDetail(UInt8 *buff, UOSInt buffSize, NotNullPtr<Text::StringBuilderUTF8> sb)
{
	if (buffSize < 10)
		return false;
	UInt32 cmdSize = ReadUInt16(&buff[2]);
	UInt32 cmdType = ReadUInt16(&buff[4]);
	UInt32 seqId = ReadUInt16(&buff[6]);
	if (buff[0] == 'T' && buff[1] == 's')
	{
		cmdSize = ReadUInt16(&buff[2]);
		cmdType = ReadUInt16(&buff[4]);
		seqId = ReadUInt16(&buff[6]);
	}
	else if (buff[0] == 'T' && buff[1] == 'S')
	{
		UOSInt tmpVal1;
		UOSInt tmpVal2;
		Crypto::Encrypt::Base64 b64;
		b64.Decrypt(&buff[2], 8, this->protoBuff, 0);
		cmdSize = ReadUInt16(&this->protoBuff[0]);
		cmdType = ReadUInt16(&this->protoBuff[2]);
		seqId = ReadUInt16(&this->protoBuff[4]);

		tmpVal1 = cmdSize / 3;
		if (tmpVal1 * 3 < cmdSize)
		{
			tmpVal2 = (tmpVal1 + 1) * 4;
		}
		else
		{
			tmpVal2 = tmpVal1 * 4;
		}
		b64.Decrypt(&buff[10], tmpVal2, &this->protoBuff[8], 0);
		buff = this->protoBuff;
	}
	else
	{
		return false;
	}
	sb->AppendC(UTF8STRC("Protocol Size="));
	sb->AppendU32(cmdSize);
	sb->AppendC(UTF8STRC("\r\n"));
	sb->AppendC(UTF8STRC("Seq="));
	sb->AppendU32(seqId);
	sb->AppendC(UTF8STRC("\r\n"));
	sb->AppendC(UTF8STRC("Protocol Type="));
	sb->AppendU32(cmdType);
	sb->AppendC(UTF8STRC(", "));
	switch (cmdType)
	{
	case 0:
		{
			Data::DateTime dt;
			dt.SetTicks(ReadInt64(&buff[8]));
			sb->AppendC(UTF8STRC("KA (Svr->Cli)"));
			sb->AppendC(UTF8STRC("\r\n"));
			sb->AppendC(UTF8STRC("Server Time="));
			sb->AppendDate(&dt);
			sb->AppendC(UTF8STRC("\r\n"));
		}
		break;
	case 1:
		{
			OSInt ofst = 14;
			sb->AppendC(UTF8STRC("KA Reply"));
			sb->AppendC(UTF8STRC("\r\n"));
			sb->AppendC(UTF8STRC("DevStatus=0x"));
			sb->AppendHex32(ReadUInt32(&buff[8]));
			sb->AppendC(UTF8STRC("\r\n"));
			if (buff[13] & 1)
			{
				Data::DateTime dt;
				sb->AppendC(UTF8STRC("Server Time="));
				dt.SetTicks(ReadInt64(&buff[ofst]));
				sb->AppendDate(&dt);
				sb->AppendC(UTF8STRC("\r\n"));
				ofst += 8;
			}
			sb->AppendC(UTF8STRC("SN="));
			if (buff[12])
			{
				sb->AppendC((UTF8Char*)&buff[ofst], buff[12]);
				ofst += buff[12];
			}
			sb->AppendC(UTF8STRC("\r\n"));
		}
		break;
	case 2:
		{
			sb->AppendC(UTF8STRC("Dev Log Message"));
			sb->AppendC(UTF8STRC("\r\n"));
			sb->AppendC((UTF8Char*)&buff[8], cmdSize - 10);
			sb->AppendC(UTF8STRC("\r\n"));
		}
		break;
	case 3:
		{
			sb->AppendC(UTF8STRC("Measure Circle"));
			sb->AppendC(UTF8STRC("\r\n"));
			sb->AppendC(UTF8STRC("Count="));
			sb->AppendI32(ReadInt32(&buff[8]));
			sb->AppendC(UTF8STRC("\r\n"));
		}
		break;
	case 4:
		{
			sb->AppendC(UTF8STRC("Cancel Working Task"));
			sb->AppendC(UTF8STRC("\r\n"));
		}
		break;
	case 5:
		{
			sb->AppendC(UTF8STRC("Cancel Working Task Reply"));
			sb->AppendC(UTF8STRC("\r\n"));
		}
		break;
	case 6:
		{
			sb->AppendC(UTF8STRC("Send Station Setup"));
			sb->AppendC(UTF8STRC("\r\n"));
			sb->AppendC(UTF8STRC("InstX="));
			Text::SBAppendF64(sb, ReadDouble(&buff[8]));
			sb->AppendC(UTF8STRC("\r\n"));
			sb->AppendC(UTF8STRC("InstY="));
			Text::SBAppendF64(sb, ReadDouble(&buff[16]));
			sb->AppendC(UTF8STRC("\r\n"));
			sb->AppendC(UTF8STRC("InstZ="));
			Text::SBAppendF64(sb, ReadDouble(&buff[24]));
			sb->AppendC(UTF8STRC("\r\n"));
			sb->AppendC(UTF8STRC("From Angle="));
			Text::SBAppendF64(sb, ReadDouble(&buff[32]));
			sb->AppendC(UTF8STRC("\r\n"));
			sb->AppendC(UTF8STRC("To Angle="));
			Text::SBAppendF64(sb, ReadDouble(&buff[40]));
			sb->AppendC(UTF8STRC("\r\n"));
			sb->AppendC(UTF8STRC("Radius="));
			Text::SBAppendF64(sb, ReadDouble(&buff[48]));
			sb->AppendC(UTF8STRC("\r\n"));
			sb->AppendC(UTF8STRC("HAngle="));
			Text::SBAppendF64(sb, ReadDouble(&buff[56]));
			sb->AppendC(UTF8STRC("\r\n"));
			sb->AppendC(UTF8STRC("Azimuth="));
			Text::SBAppendF64(sb, ReadDouble(&buff[64]));
			sb->AppendC(UTF8STRC("\r\n"));
		}
		break;
	case 7:
		{
			sb->AppendC(UTF8STRC("Reply Station Setup"));
			sb->AppendC(UTF8STRC("\r\n"));
		}
		break;
	case 8:
		{
			sb->AppendC(UTF8STRC("Face North"));
			sb->AppendC(UTF8STRC("\r\n"));
		}
		break;
	case 9:
		{
			sb->AppendC(UTF8STRC("Face South"));
			sb->AppendC(UTF8STRC("\r\n"));
		}
		break;
	case 10:
		{
			OSInt i;
			OSInt j;
			sb->AppendC(UTF8STRC("Send Task List"));
			sb->AppendC(UTF8STRC("\r\n"));
			sb->AppendC(UTF8STRC("Task Cnt="));
			sb->AppendI32((Int32)(j = ReadInt32(&buff[8])));
			sb->AppendC(UTF8STRC("\r\n"));
			i = 0;
			while (i < j)
			{
				sb->AppendC(UTF8STRC("Task "));
				sb->AppendI32((Int32)i);
				sb->AppendC(UTF8STRC("="));
				sb->AppendI32(ReadInt32(&buff[12 + i * 4]));
				sb->AppendC(UTF8STRC("\r\n"));
				i++;
			}
		}
		break;
	case 11:
		{
			sb->AppendC(UTF8STRC("Send Task List Reply"));
			sb->AppendC(UTF8STRC("\r\n"));
		}
		break;
	case 12:
		{
			sb->AppendC(UTF8STRC("Check Task Status"));
			sb->AppendC(UTF8STRC("\r\n"));
			sb->AppendC(UTF8STRC("TaskId="));
			sb->AppendI32(ReadInt32(&buff[8]));
			sb->AppendC(UTF8STRC("\r\n"));
		}
		break;
	case 13:
		{
			sb->AppendC(UTF8STRC("Task Sync Completed"));
			sb->AppendC(UTF8STRC("\r\n"));
			sb->AppendC(UTF8STRC("TaskId="));
			sb->AppendI32(ReadInt32(&buff[8]));
			sb->AppendC(UTF8STRC("\r\n"));
		}
		break;
	case 14:
		{
			sb->AppendC(UTF8STRC("Request Task Detail"));
			sb->AppendC(UTF8STRC("\r\n"));
			sb->AppendC(UTF8STRC("TaskId="));
			sb->AppendI32(ReadInt32(&buff[8]));
			sb->AppendC(UTF8STRC("\r\n"));
		}
		break;
	case 15:
		{
			sb->AppendC(UTF8STRC("Request Task Detail Reply"));
			sb->AppendC(UTF8STRC("\r\n"));
			sb->AppendC(UTF8STRC("TaskId="));
			sb->AppendI32(ReadInt32(&buff[8]));
			sb->AppendC(UTF8STRC("\r\n"));
			if (cmdSize >= 42)
			{
				OSInt i;
				OSInt j;
				Data::DateTime dt;
				sb->AppendC(UTF8STRC("Start Time="));
				dt.SetTicks(ReadInt64(&buff[12]));
				sb->AppendDate(&dt);
				sb->AppendC(UTF8STRC("\r\n"));
				sb->AppendC(UTF8STRC("End Time="));
				dt.SetTicks(ReadInt64(&buff[20]));
				sb->AppendDate(&dt);
				sb->AppendC(UTF8STRC("\r\n"));
				sb->AppendC(UTF8STRC("Grid Size="));
				dt.SetTicks(ReadInt64(&buff[28]));
				sb->AppendDate(&dt);
				sb->AppendC(UTF8STRC("\r\n"));
				sb->AppendC(UTF8STRC("Area Cnt="));
				sb->AppendI32((Int32)(j = ReadInt32(&buff[36])));
				sb->AppendC(UTF8STRC("\r\n"));
				i = 0;
				while (i < j)
				{
					sb->AppendC(UTF8STRC("Area "));
					sb->AppendI32((Int32)i);
					sb->AppendC(UTF8STRC("="));
					sb->AppendI32(ReadInt32(&buff[40 + i * 4]));
					sb->AppendC(UTF8STRC("\r\n"));
					i++;
				}
			}
		}
		break;
	case 16:
		{
			sb->AppendC(UTF8STRC("Request Area Detail"));
			sb->AppendC(UTF8STRC("\r\n"));
			sb->AppendC(UTF8STRC("AreaId="));
			sb->AppendI32(ReadInt32(&buff[8]));
			sb->AppendC(UTF8STRC("\r\n"));
		}
		break;
	case 17:
		{
			OSInt i;
			OSInt j;
			sb->AppendC(UTF8STRC("Request Area Detail Reply"));
			sb->AppendC(UTF8STRC("\r\n"));
			sb->AppendC(UTF8STRC("AreaId="));
			sb->AppendI32(ReadInt32(&buff[8]));
			sb->AppendC(UTF8STRC("\r\n"));
			sb->AppendC(UTF8STRC("Area Height="));
			Text::SBAppendF64(sb, ReadDouble(&buff[12]));
			sb->AppendC(UTF8STRC("\r\n"));
			sb->AppendC(UTF8STRC("Point Cnt="));
			sb->AppendI32((Int32)(j = ReadInt32(&buff[20])));
			sb->AppendC(UTF8STRC("\r\n"));
			i = 0;
			while (i < j)
			{
				sb->AppendC(UTF8STRC("Point "));
				sb->AppendI32((Int32)i);
				sb->AppendC(UTF8STRC(": "));
				Text::SBAppendF64(sb, ReadDouble(&buff[24 + i * 16]));
				sb->AppendC(UTF8STRC(", "));
				Text::SBAppendF64(sb, ReadDouble(&buff[32 + i * 16]));
				sb->AppendC(UTF8STRC("\r\n"));
				i++;
			}
		}
		break;
	case 18:
		{
			sb->AppendC(UTF8STRC("Cancel Task"));
			sb->AppendC(UTF8STRC("\r\n"));
			sb->AppendC(UTF8STRC("Task Id="));
			sb->AppendI32(ReadInt32(&buff[8]));
			sb->AppendC(UTF8STRC("\r\n"));
		}
		break;
	case 20:
		{
			sb->AppendC(UTF8STRC("Task Status Update"));
			sb->AppendC(UTF8STRC("\r\n"));
			sb->AppendC(UTF8STRC("Task Id="));
			sb->AppendI32(ReadInt32(&buff[8]));
			sb->AppendC(UTF8STRC("\r\n"));
			sb->AppendC(UTF8STRC("Task Status="));
			sb->AppendI32(ReadInt32(&buff[12]));
			sb->AppendC(UTF8STRC("\r\n"));
			sb->AppendC(UTF8STRC("End Status="));
			sb->AppendI32(ReadInt32(&buff[16]));
			sb->AppendC(UTF8STRC("\r\n"));
			if (cmdSize >= 30)
			{
				Data::DateTime dt;
				dt.SetTicks(ReadInt64(&buff[20]));
				sb->AppendC(UTF8STRC("Complete Time="));
				sb->AppendDate(&dt);
				sb->AppendC(UTF8STRC("\r\n"));
			}
		}
		break;
	case 21:
		{
			sb->AppendC(UTF8STRC("Task Status Update Reply"));
			sb->AppendC(UTF8STRC("\r\n"));
			sb->AppendC(UTF8STRC("Task Id="));
			sb->AppendI32(ReadInt32(&buff[8]));
			sb->AppendC(UTF8STRC("\r\n"));
			sb->AppendC(UTF8STRC("Task Status="));
			sb->AppendI32(ReadInt32(&buff[12]));
			sb->AppendC(UTF8STRC("\r\n"));
			sb->AppendC(UTF8STRC("End Status="));
			sb->AppendI32(ReadInt32(&buff[16]));
			sb->AppendC(UTF8STRC("\r\n"));
		}
		break;
	case 22:
		{
			sb->AppendC(UTF8STRC("Remove Task"));
			sb->AppendC(UTF8STRC("\r\n"));
			sb->AppendC(UTF8STRC("Task Id="));
			sb->AppendI32(ReadInt32(&buff[8]));
			sb->AppendC(UTF8STRC("\r\n"));
		}
		break;
	case 23:
		{
			sb->AppendC(UTF8STRC("Remove Task Reply"));
			sb->AppendC(UTF8STRC("\r\n"));
			sb->AppendC(UTF8STRC("Task Id="));
			sb->AppendI32(ReadInt32(&buff[8]));
			sb->AppendC(UTF8STRC("\r\n"));
		}
		break;
	case 24:
		{
			sb->AppendC(UTF8STRC("Request Task File"));
			sb->AppendC(UTF8STRC("\r\n"));
			sb->AppendC(UTF8STRC("Task Id="));
			sb->AppendI32(ReadInt32(&buff[8]));
			sb->AppendC(UTF8STRC("\r\n"));
		}
		break;
	case 25:
		{
			sb->AppendC(UTF8STRC("Task File Detail"));
			sb->AppendC(UTF8STRC("\r\n"));
			sb->AppendC(UTF8STRC("Task Id="));
			sb->AppendI32(ReadInt32(&buff[8]));
			sb->AppendC(UTF8STRC("\r\n"));
			sb->AppendC(UTF8STRC("File Size="));
			sb->AppendI32(ReadInt32(&buff[12]));
			sb->AppendC(UTF8STRC("\r\n"));
			sb->AppendC(UTF8STRC("File CRC="));
			sb->AppendHex32(ReadUInt32(&buff[16]));
			sb->AppendC(UTF8STRC("\r\n"));
		}
		break;
	case 26:
		{
			sb->AppendC(UTF8STRC("Transfer File"));
			sb->AppendC(UTF8STRC("\r\n"));
			sb->AppendC(UTF8STRC("Task Id="));
			sb->AppendI32(ReadInt32(&buff[8]));
			sb->AppendC(UTF8STRC("\r\n"));
			sb->AppendC(UTF8STRC("Block Id="));
			sb->AppendI32(ReadInt32(&buff[12]));
			sb->AppendC(UTF8STRC("\r\n"));
		}
		break;
	case 27:
		{
			sb->AppendC(UTF8STRC("End Transfer File"));
			sb->AppendC(UTF8STRC("\r\n"));
			sb->AppendC(UTF8STRC("Task Id="));
			sb->AppendI32(ReadInt32(&buff[8]));
			sb->AppendC(UTF8STRC("\r\n"));
		}
		break;
	case 28:
		{
			sb->AppendC(UTF8STRC("Request Dev Task List"));
			sb->AppendC(UTF8STRC("\r\n"));
		}
		break;
	case 29:
		{
			OSInt i;
			OSInt j;
			sb->AppendC(UTF8STRC("Dev Task List Resp"));
			sb->AppendC(UTF8STRC("\r\n"));
			sb->AppendC(UTF8STRC("Cmd Id="));
			sb->AppendI32(ReadInt32(&buff[8]));
			sb->AppendC(UTF8STRC("\r\n"));
			sb->AppendC(UTF8STRC("Task Cnt="));
			sb->AppendI32((Int32)(j = ReadInt32(&buff[12])));
			sb->AppendC(UTF8STRC("\r\n"));
			i = 0;
			while (i < j)
			{
				sb->AppendC(UTF8STRC("Task Id="));
				sb->AppendI32(ReadInt32(&buff[16 + i * 12]));
				sb->AppendC(UTF8STRC(", Task Status="));
				sb->AppendI32(ReadInt32(&buff[20 + i * 12]));
				sb->AppendC(UTF8STRC(", Task Status="));
				sb->AppendI32(ReadInt32(&buff[24 + i * 12]));
				sb->AppendC(UTF8STRC("\r\n"));
				
				i++;
			}
		}
		break;
	case 30:
		{
			sb->AppendC(UTF8STRC("Reopen"));
			sb->AppendC(UTF8STRC("\r\n"));
		}
		break;
	case 31:
		{
			sb->AppendC(UTF8STRC("Move To Angle"));
			sb->AppendC(UTF8STRC("\r\n"));
			sb->AppendC(UTF8STRC("H="));
			Text::SBAppendF64(sb, ReadDouble(&buff[8]));
			sb->AppendC(UTF8STRC("\r\n"));
			sb->AppendC(UTF8STRC("V="));
			Text::SBAppendF64(sb, ReadDouble(&buff[16]));
			sb->AppendC(UTF8STRC("\r\n"));
		}
		break;
	case 32:
		{
			sb->AppendC(UTF8STRC("Request To Send Dummy Data"));
			sb->AppendC(UTF8STRC("\r\n"));
			sb->AppendC(UTF8STRC("Size="));
			sb->AppendI32(ReadInt32(&buff[8]));
			sb->AppendC(UTF8STRC("\r\n"));
		}
		break;
	case 33:
		{
			sb->AppendC(UTF8STRC("Send Dummy Data"));
			sb->AppendC(UTF8STRC("\r\n"));
		}
		break;
	case 34:
		{
			sb->AppendC(UTF8STRC("Get Current Angle"));
			sb->AppendC(UTF8STRC("\r\n"));
		}
		break;
	case 35:
		{
			sb->AppendC(UTF8STRC("Get Current Angle Reply"));
			sb->AppendC(UTF8STRC("\r\n"));
			sb->AppendC(UTF8STRC("H="));
			Text::SBAppendF64(sb, ReadDouble(&buff[8]));
			sb->AppendC(UTF8STRC("\r\n"));
			sb->AppendC(UTF8STRC("V="));
			Text::SBAppendF64(sb, ReadDouble(&buff[16]));
			sb->AppendC(UTF8STRC("\r\n"));
		}
		break;
	case 36:
		{
			sb->AppendC(UTF8STRC("Turn Laser"));
			sb->AppendC(UTF8STRC("\r\n"));
			sb->AppendC(UTF8STRC("Value="));
			sb->AppendI32(ReadInt32(&buff[8]));
			sb->AppendC(UTF8STRC("\r\n"));
		}
		break;
	default:
		sb->AppendC(UTF8STRC("Unknown"));
		sb->AppendC(UTF8STRC("\r\n"));
		break;
	}
	return true;
}

Bool IO::ProtoDec::TSProtocolDecoder::IsValid(UInt8 *buff, UOSInt buffSize)
{
	return buff[0] == 'T' && buff[1] == 's';
}
