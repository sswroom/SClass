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

const UTF8Char *IO::ProtoDec::TSProtocolDecoder::GetName()
{
	return (const UTF8Char*)"TS";
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
				hdlr(userObj, fileOfst + j, i - j, (const UTF8Char*)"Unknown Protocol");
			}
			j = i;
			cmdSize = ReadUInt16(&buff[i + 2]);
			cmdType = ReadUInt16(&buff[i + 4]);
			seqId = ReadUInt16(&buff[i + 6]);
			if (i + cmdSize > buffSize)
				return j;
			sb.ClearStr();
			sb.Append((const UTF8Char*)"0x");
			sb.AppendHex16(cmdType);
			sb.Append((const UTF8Char*)", seq=");
			sb.AppendI32(seqId);
			sb.Append((const UTF8Char*)", ");
			switch (cmdType)
			{
			case 0:
				sb.Append((const UTF8Char*)"KA (Svr->Cli)");
				break;
			case 1:
				sb.Append((const UTF8Char*)"KA Reply");
				break;
			case 2:
				sb.Append((const UTF8Char*)"Dev Log Message");
				break;
			case 3:
				sb.Append((const UTF8Char*)"Measure Circle");
				break;
			case 4:
				sb.Append((const UTF8Char*)"Cancel Working Task");
				break;
			case 5:
				sb.Append((const UTF8Char*)"Cancel Working Task Reply");
				break;
			case 6:
				sb.Append((const UTF8Char*)"Send Station Setup");
				break;
			case 7:
				sb.Append((const UTF8Char*)"Reply Station Setup");
				break;
			case 8:
				sb.Append((const UTF8Char*)"Face North");
				break;
			case 9:
				sb.Append((const UTF8Char*)"Face South");
				break;
			case 10:
				sb.Append((const UTF8Char*)"Send Task List");
				break;
			case 11:
				sb.Append((const UTF8Char*)"Send Task List Reply");
				break;
			case 12:
				sb.Append((const UTF8Char*)"Check Task Status");
				break;
			case 13:
				sb.Append((const UTF8Char*)"Task Sync Completed");
				break;
			case 14:
				sb.Append((const UTF8Char*)"Request Task Detail");
				break;
			case 15:
				sb.Append((const UTF8Char*)"Request Task Detail Reply");
				break;
			case 16:
				sb.Append((const UTF8Char*)"Request Area Detail");
				break;
			case 17:
				sb.Append((const UTF8Char*)"Request Area Detail Reply");
				break;
			case 18:
				sb.Append((const UTF8Char*)"Cancel Task");
				break;
			case 20:
				sb.Append((const UTF8Char*)"Task Status Update");
				break;
			case 21:
				sb.Append((const UTF8Char*)"Task Status Update Reply");
				break;
			case 22:
				sb.Append((const UTF8Char*)"Remove Task");
				break;
			case 23:
				sb.Append((const UTF8Char*)"Remove Task Reply");
				break;
			case 24:
				sb.Append((const UTF8Char*)"Request Task File");
				break;
			case 25:
				sb.Append((const UTF8Char*)"Task File Detail");
				break;
			case 26:
				sb.Append((const UTF8Char*)"Transfer File");
				break;
			case 27:
				sb.Append((const UTF8Char*)"End Transfer File");
				break;
			case 28:
				sb.Append((const UTF8Char*)"Request Dev Task List");
				break;
			case 29:
				sb.Append((const UTF8Char*)"Dev Task List Resp");
				break;
			case 30:
				sb.Append((const UTF8Char*)"Reopen");
				break;
			case 31:
				sb.Append((const UTF8Char*)"Move To Angle");
				break;
			case 32:
				sb.Append((const UTF8Char*)"Request To Send Dummy Data");
				break;
			case 33:
				sb.Append((const UTF8Char*)"Send Dummy Data");
				break;
			case 34:
				sb.Append((const UTF8Char*)"Get Current Angle");
				break;
			case 35:
				sb.Append((const UTF8Char*)"Get Current Angle Reply");
				break;
			case 36:
				sb.Append((const UTF8Char*)"Turn Laser");
				break;
			default:
				sb.Append((const UTF8Char*)"Unknown Protocol");
				break;
			}
			hdlr(userObj, fileOfst + j, cmdSize, sb.ToString());
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
				hdlr(userObj, fileOfst + j, i - j, (const UTF8Char*)"Unknown Protocol");
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
			sb.Append((const UTF8Char*)"0x");
			sb.AppendHex16(cmdType);
			sb.Append((const UTF8Char*)", seq=");
			sb.AppendI32(seqId);
			sb.Append((const UTF8Char*)", ");
			switch (cmdType)
			{
			case 0:
				sb.Append((const UTF8Char*)"KA (Svr->Cli)");
				break;
			case 1:
				sb.Append((const UTF8Char*)"KA Reply");
				break;
			case 2:
				sb.Append((const UTF8Char*)"Dev Log Message");
				break;
			case 3:
				sb.Append((const UTF8Char*)"Measure Circle");
				break;
			case 4:
				sb.Append((const UTF8Char*)"Cancel Working Task");
				break;
			case 5:
				sb.Append((const UTF8Char*)"Cancel Working Task Reply");
				break;
			case 6:
				sb.Append((const UTF8Char*)"Send Station Setup");
				break;
			case 7:
				sb.Append((const UTF8Char*)"Reply Station Setup");
				break;
			case 8:
				sb.Append((const UTF8Char*)"Face North");
				break;
			case 9:
				sb.Append((const UTF8Char*)"Face South");
				break;
			case 10:
				sb.Append((const UTF8Char*)"Send Task List");
				break;
			case 11:
				sb.Append((const UTF8Char*)"Send Task List Reply");
				break;
			case 12:
				sb.Append((const UTF8Char*)"Check Task Status");
				break;
			case 13:
				sb.Append((const UTF8Char*)"Task Sync Completed");
				break;
			case 14:
				sb.Append((const UTF8Char*)"Request Task Detail");
				break;
			case 15:
				sb.Append((const UTF8Char*)"Request Task Detail Reply");
				break;
			case 16:
				sb.Append((const UTF8Char*)"Request Area Detail");
				break;
			case 17:
				sb.Append((const UTF8Char*)"Request Area Detail Reply");
				break;
			case 18:
				sb.Append((const UTF8Char*)"Cancel Task");
				break;
			case 20:
				sb.Append((const UTF8Char*)"Task Status Update");
				break;
			case 21:
				sb.Append((const UTF8Char*)"Task Status Update Reply");
				break;
			case 22:
				sb.Append((const UTF8Char*)"Remove Task");
				break;
			case 23:
				sb.Append((const UTF8Char*)"Remove Task Reply");
				break;
			case 24:
				sb.Append((const UTF8Char*)"Request Task File");
				break;
			case 25:
				sb.Append((const UTF8Char*)"Task File Detail");
				break;
			case 26:
				sb.Append((const UTF8Char*)"Transfer File");
				break;
			case 27:
				sb.Append((const UTF8Char*)"End Transfer File");
				break;
			case 28:
				sb.Append((const UTF8Char*)"Request Dev Task List");
				break;
			case 29:
				sb.Append((const UTF8Char*)"Dev Task List Resp");
				break;
			case 30:
				sb.Append((const UTF8Char*)"Reopen");
				break;
			case 31:
				sb.Append((const UTF8Char*)"Move To Angle");
				break;
			case 32:
				sb.Append((const UTF8Char*)"Request To Send Dummy Data");
				break;
			case 33:
				sb.Append((const UTF8Char*)"Send Dummy Data");
				break;
			case 34:
				sb.Append((const UTF8Char*)"Get Current Angle");
				break;
			case 35:
				sb.Append((const UTF8Char*)"Get Current Angle Reply");
				break;
			case 36:
				sb.Append((const UTF8Char*)"Turn Laser");
				break;
			default:
				sb.Append((const UTF8Char*)"Unknown Protocol");
				break;
			}
			hdlr(userObj, fileOfst + j, tmpVal2 + 12, sb.ToString());
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

Bool IO::ProtoDec::TSProtocolDecoder::GetProtocolDetail(UInt8 *buff, UOSInt buffSize, Text::StringBuilderUTF *sb)
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
	sb->Append((const UTF8Char*)"Protocol Size=");
	sb->AppendU32(cmdSize);
	sb->Append((const UTF8Char*)"\r\n");
	sb->Append((const UTF8Char*)"Seq=");
	sb->AppendU32(seqId);
	sb->Append((const UTF8Char*)"\r\n");
	sb->Append((const UTF8Char*)"Protocol Type=");
	sb->AppendU32(cmdType);
	sb->Append((const UTF8Char*)", ");
	switch (cmdType)
	{
	case 0:
		{
			Data::DateTime dt;
			dt.SetTicks(ReadInt64(&buff[8]));
			sb->Append((const UTF8Char*)"KA (Svr->Cli)");
			sb->Append((const UTF8Char*)"\r\n");
			sb->Append((const UTF8Char*)"Server Time=");
			sb->AppendDate(&dt);
			sb->Append((const UTF8Char*)"\r\n");
		}
		break;
	case 1:
		{
			OSInt ofst = 14;
			sb->Append((const UTF8Char*)"KA Reply");
			sb->Append((const UTF8Char*)"\r\n");
			sb->Append((const UTF8Char*)"DevStatus=0x");
			sb->AppendHex32(ReadUInt32(&buff[8]));
			sb->Append((const UTF8Char*)"\r\n");
			if (buff[13] & 1)
			{
				Data::DateTime dt;
				sb->Append((const UTF8Char*)"Server Time=");
				dt.SetTicks(ReadInt64(&buff[ofst]));
				sb->AppendDate(&dt);
				sb->Append((const UTF8Char*)"\r\n");
				ofst += 8;
			}
			sb->Append((const UTF8Char*)"SN=");
			if (buff[12])
			{
				sb->AppendC((UTF8Char*)&buff[ofst], buff[12]);
				ofst += buff[12];
			}
			sb->Append((const UTF8Char*)"\r\n");
		}
		break;
	case 2:
		{
			sb->Append((const UTF8Char*)"Dev Log Message");
			sb->Append((const UTF8Char*)"\r\n");
			sb->AppendC((UTF8Char*)&buff[8], cmdSize - 10);
			sb->Append((const UTF8Char*)"\r\n");
		}
		break;
	case 3:
		{
			sb->Append((const UTF8Char*)"Measure Circle");
			sb->Append((const UTF8Char*)"\r\n");
			sb->Append((const UTF8Char*)"Count=");
			sb->AppendI32(ReadInt32(&buff[8]));
			sb->Append((const UTF8Char*)"\r\n");
		}
		break;
	case 4:
		{
			sb->Append((const UTF8Char*)"Cancel Working Task");
			sb->Append((const UTF8Char*)"\r\n");
		}
		break;
	case 5:
		{
			sb->Append((const UTF8Char*)"Cancel Working Task Reply");
			sb->Append((const UTF8Char*)"\r\n");
		}
		break;
	case 6:
		{
			sb->Append((const UTF8Char*)"Send Station Setup");
			sb->Append((const UTF8Char*)"\r\n");
			sb->Append((const UTF8Char*)"InstX=");
			Text::SBAppendF64(sb, ReadDouble(&buff[8]));
			sb->Append((const UTF8Char*)"\r\n");
			sb->Append((const UTF8Char*)"InstY=");
			Text::SBAppendF64(sb, ReadDouble(&buff[16]));
			sb->Append((const UTF8Char*)"\r\n");
			sb->Append((const UTF8Char*)"InstZ=");
			Text::SBAppendF64(sb, ReadDouble(&buff[24]));
			sb->Append((const UTF8Char*)"\r\n");
			sb->Append((const UTF8Char*)"From Angle=");
			Text::SBAppendF64(sb, ReadDouble(&buff[32]));
			sb->Append((const UTF8Char*)"\r\n");
			sb->Append((const UTF8Char*)"To Angle=");
			Text::SBAppendF64(sb, ReadDouble(&buff[40]));
			sb->Append((const UTF8Char*)"\r\n");
			sb->Append((const UTF8Char*)"Radius=");
			Text::SBAppendF64(sb, ReadDouble(&buff[48]));
			sb->Append((const UTF8Char*)"\r\n");
			sb->Append((const UTF8Char*)"HAngle=");
			Text::SBAppendF64(sb, ReadDouble(&buff[56]));
			sb->Append((const UTF8Char*)"\r\n");
			sb->Append((const UTF8Char*)"Azimuth=");
			Text::SBAppendF64(sb, ReadDouble(&buff[64]));
			sb->Append((const UTF8Char*)"\r\n");
		}
		break;
	case 7:
		{
			sb->Append((const UTF8Char*)"Reply Station Setup");
			sb->Append((const UTF8Char*)"\r\n");
		}
		break;
	case 8:
		{
			sb->Append((const UTF8Char*)"Face North");
			sb->Append((const UTF8Char*)"\r\n");
		}
		break;
	case 9:
		{
			sb->Append((const UTF8Char*)"Face South");
			sb->Append((const UTF8Char*)"\r\n");
		}
		break;
	case 10:
		{
			OSInt i;
			OSInt j;
			sb->Append((const UTF8Char*)"Send Task List");
			sb->Append((const UTF8Char*)"\r\n");
			sb->Append((const UTF8Char*)"Task Cnt=");
			sb->AppendI32((Int32)(j = ReadInt32(&buff[8])));
			sb->Append((const UTF8Char*)"\r\n");
			i = 0;
			while (i < j)
			{
				sb->Append((const UTF8Char*)"Task ");
				sb->AppendI32((Int32)i);
				sb->Append((const UTF8Char*)"=");
				sb->AppendI32(ReadInt32(&buff[12 + i * 4]));
				sb->Append((const UTF8Char*)"\r\n");
				i++;
			}
		}
		break;
	case 11:
		{
			sb->Append((const UTF8Char*)"Send Task List Reply");
			sb->Append((const UTF8Char*)"\r\n");
		}
		break;
	case 12:
		{
			sb->Append((const UTF8Char*)"Check Task Status");
			sb->Append((const UTF8Char*)"\r\n");
			sb->Append((const UTF8Char*)"TaskId=");
			sb->AppendI32(ReadInt32(&buff[8]));
			sb->Append((const UTF8Char*)"\r\n");
		}
		break;
	case 13:
		{
			sb->Append((const UTF8Char*)"Task Sync Completed");
			sb->Append((const UTF8Char*)"\r\n");
			sb->Append((const UTF8Char*)"TaskId=");
			sb->AppendI32(ReadInt32(&buff[8]));
			sb->Append((const UTF8Char*)"\r\n");
		}
		break;
	case 14:
		{
			sb->Append((const UTF8Char*)"Request Task Detail");
			sb->Append((const UTF8Char*)"\r\n");
			sb->Append((const UTF8Char*)"TaskId=");
			sb->AppendI32(ReadInt32(&buff[8]));
			sb->Append((const UTF8Char*)"\r\n");
		}
		break;
	case 15:
		{
			sb->Append((const UTF8Char*)"Request Task Detail Reply");
			sb->Append((const UTF8Char*)"\r\n");
			sb->Append((const UTF8Char*)"TaskId=");
			sb->AppendI32(ReadInt32(&buff[8]));
			sb->Append((const UTF8Char*)"\r\n");
			if (cmdSize >= 42)
			{
				OSInt i;
				OSInt j;
				Data::DateTime dt;
				sb->Append((const UTF8Char*)"Start Time=");
				dt.SetTicks(ReadInt64(&buff[12]));
				sb->AppendDate(&dt);
				sb->Append((const UTF8Char*)"\r\n");
				sb->Append((const UTF8Char*)"End Time=");
				dt.SetTicks(ReadInt64(&buff[20]));
				sb->AppendDate(&dt);
				sb->Append((const UTF8Char*)"\r\n");
				sb->Append((const UTF8Char*)"Grid Size=");
				dt.SetTicks(ReadInt64(&buff[28]));
				sb->AppendDate(&dt);
				sb->Append((const UTF8Char*)"\r\n");
				sb->Append((const UTF8Char*)"Area Cnt=");
				sb->AppendI32((Int32)(j = ReadInt32(&buff[36])));
				sb->Append((const UTF8Char*)"\r\n");
				i = 0;
				while (i < j)
				{
					sb->Append((const UTF8Char*)"Area ");
					sb->AppendI32((Int32)i);
					sb->Append((const UTF8Char*)"=");
					sb->AppendI32(ReadInt32(&buff[40 + i * 4]));
					sb->Append((const UTF8Char*)"\r\n");
					i++;
				}
			}
		}
		break;
	case 16:
		{
			sb->Append((const UTF8Char*)"Request Area Detail");
			sb->Append((const UTF8Char*)"\r\n");
			sb->Append((const UTF8Char*)"AreaId=");
			sb->AppendI32(ReadInt32(&buff[8]));
			sb->Append((const UTF8Char*)"\r\n");
		}
		break;
	case 17:
		{
			OSInt i;
			OSInt j;
			sb->Append((const UTF8Char*)"Request Area Detail Reply");
			sb->Append((const UTF8Char*)"\r\n");
			sb->Append((const UTF8Char*)"AreaId=");
			sb->AppendI32(ReadInt32(&buff[8]));
			sb->Append((const UTF8Char*)"\r\n");
			sb->Append((const UTF8Char*)"Area Height=");
			Text::SBAppendF64(sb, ReadDouble(&buff[12]));
			sb->Append((const UTF8Char*)"\r\n");
			sb->Append((const UTF8Char*)"Point Cnt=");
			sb->AppendI32((Int32)(j = ReadInt32(&buff[20])));
			sb->Append((const UTF8Char*)"\r\n");
			i = 0;
			while (i < j)
			{
				sb->Append((const UTF8Char*)"Point ");
				sb->AppendI32((Int32)i);
				sb->Append((const UTF8Char*)": ");
				Text::SBAppendF64(sb, ReadDouble(&buff[24 + i * 16]));
				sb->Append((const UTF8Char*)", ");
				Text::SBAppendF64(sb, ReadDouble(&buff[32 + i * 16]));
				sb->Append((const UTF8Char*)"\r\n");
				i++;
			}
		}
		break;
	case 18:
		{
			sb->Append((const UTF8Char*)"Cancel Task");
			sb->Append((const UTF8Char*)"\r\n");
			sb->Append((const UTF8Char*)"Task Id=");
			sb->AppendI32(ReadInt32(&buff[8]));
			sb->Append((const UTF8Char*)"\r\n");
		}
		break;
	case 20:
		{
			sb->Append((const UTF8Char*)"Task Status Update");
			sb->Append((const UTF8Char*)"\r\n");
			sb->Append((const UTF8Char*)"Task Id=");
			sb->AppendI32(ReadInt32(&buff[8]));
			sb->Append((const UTF8Char*)"\r\n");
			sb->Append((const UTF8Char*)"Task Status=");
			sb->AppendI32(ReadInt32(&buff[12]));
			sb->Append((const UTF8Char*)"\r\n");
			sb->Append((const UTF8Char*)"End Status=");
			sb->AppendI32(ReadInt32(&buff[16]));
			sb->Append((const UTF8Char*)"\r\n");
			if (cmdSize >= 30)
			{
				Data::DateTime dt;
				dt.SetTicks(ReadInt64(&buff[20]));
				sb->Append((const UTF8Char*)"Complete Time=");
				sb->AppendDate(&dt);
				sb->Append((const UTF8Char*)"\r\n");
			}
		}
		break;
	case 21:
		{
			sb->Append((const UTF8Char*)"Task Status Update Reply");
			sb->Append((const UTF8Char*)"\r\n");
			sb->Append((const UTF8Char*)"Task Id=");
			sb->AppendI32(ReadInt32(&buff[8]));
			sb->Append((const UTF8Char*)"\r\n");
			sb->Append((const UTF8Char*)"Task Status=");
			sb->AppendI32(ReadInt32(&buff[12]));
			sb->Append((const UTF8Char*)"\r\n");
			sb->Append((const UTF8Char*)"End Status=");
			sb->AppendI32(ReadInt32(&buff[16]));
			sb->Append((const UTF8Char*)"\r\n");
		}
		break;
	case 22:
		{
			sb->Append((const UTF8Char*)"Remove Task");
			sb->Append((const UTF8Char*)"\r\n");
			sb->Append((const UTF8Char*)"Task Id=");
			sb->AppendI32(ReadInt32(&buff[8]));
			sb->Append((const UTF8Char*)"\r\n");
		}
		break;
	case 23:
		{
			sb->Append((const UTF8Char*)"Remove Task Reply");
			sb->Append((const UTF8Char*)"\r\n");
			sb->Append((const UTF8Char*)"Task Id=");
			sb->AppendI32(ReadInt32(&buff[8]));
			sb->Append((const UTF8Char*)"\r\n");
		}
		break;
	case 24:
		{
			sb->Append((const UTF8Char*)"Request Task File");
			sb->Append((const UTF8Char*)"\r\n");
			sb->Append((const UTF8Char*)"Task Id=");
			sb->AppendI32(ReadInt32(&buff[8]));
			sb->Append((const UTF8Char*)"\r\n");
		}
		break;
	case 25:
		{
			sb->Append((const UTF8Char*)"Task File Detail");
			sb->Append((const UTF8Char*)"\r\n");
			sb->Append((const UTF8Char*)"Task Id=");
			sb->AppendI32(ReadInt32(&buff[8]));
			sb->Append((const UTF8Char*)"\r\n");
			sb->Append((const UTF8Char*)"File Size=");
			sb->AppendI32(ReadInt32(&buff[12]));
			sb->Append((const UTF8Char*)"\r\n");
			sb->Append((const UTF8Char*)"File CRC=");
			sb->AppendHex32(ReadUInt32(&buff[16]));
			sb->Append((const UTF8Char*)"\r\n");
		}
		break;
	case 26:
		{
			sb->Append((const UTF8Char*)"Transfer File");
			sb->Append((const UTF8Char*)"\r\n");
			sb->Append((const UTF8Char*)"Task Id=");
			sb->AppendI32(ReadInt32(&buff[8]));
			sb->Append((const UTF8Char*)"\r\n");
			sb->Append((const UTF8Char*)"Block Id=");
			sb->AppendI32(ReadInt32(&buff[12]));
			sb->Append((const UTF8Char*)"\r\n");
		}
		break;
	case 27:
		{
			sb->Append((const UTF8Char*)"End Transfer File");
			sb->Append((const UTF8Char*)"\r\n");
			sb->Append((const UTF8Char*)"Task Id=");
			sb->AppendI32(ReadInt32(&buff[8]));
			sb->Append((const UTF8Char*)"\r\n");
		}
		break;
	case 28:
		{
			sb->Append((const UTF8Char*)"Request Dev Task List");
			sb->Append((const UTF8Char*)"\r\n");
		}
		break;
	case 29:
		{
			OSInt i;
			OSInt j;
			sb->Append((const UTF8Char*)"Dev Task List Resp");
			sb->Append((const UTF8Char*)"\r\n");
			sb->Append((const UTF8Char*)"Cmd Id=");
			sb->AppendI32(ReadInt32(&buff[8]));
			sb->Append((const UTF8Char*)"\r\n");
			sb->Append((const UTF8Char*)"Task Cnt=");
			sb->AppendI32((Int32)(j = ReadInt32(&buff[12])));
			sb->Append((const UTF8Char*)"\r\n");
			i = 0;
			while (i < j)
			{
				sb->Append((const UTF8Char*)"Task Id=");
				sb->AppendI32(ReadInt32(&buff[16 + i * 12]));
				sb->Append((const UTF8Char*)", Task Status=");
				sb->AppendI32(ReadInt32(&buff[20 + i * 12]));
				sb->Append((const UTF8Char*)", Task Status=");
				sb->AppendI32(ReadInt32(&buff[24 + i * 12]));
				sb->Append((const UTF8Char*)"\r\n");
				
				i++;
			}
		}
		break;
	case 30:
		{
			sb->Append((const UTF8Char*)"Reopen");
			sb->Append((const UTF8Char*)"\r\n");
		}
		break;
	case 31:
		{
			sb->Append((const UTF8Char*)"Move To Angle");
			sb->Append((const UTF8Char*)"\r\n");
			sb->Append((const UTF8Char*)"H=");
			Text::SBAppendF64(sb, ReadDouble(&buff[8]));
			sb->Append((const UTF8Char*)"\r\n");
			sb->Append((const UTF8Char*)"V=");
			Text::SBAppendF64(sb, ReadDouble(&buff[16]));
			sb->Append((const UTF8Char*)"\r\n");
		}
		break;
	case 32:
		{
			sb->Append((const UTF8Char*)"Request To Send Dummy Data");
			sb->Append((const UTF8Char*)"\r\n");
			sb->Append((const UTF8Char*)"Size=");
			sb->AppendI32(ReadInt32(&buff[8]));
			sb->Append((const UTF8Char*)"\r\n");
		}
		break;
	case 33:
		{
			sb->Append((const UTF8Char*)"Send Dummy Data");
			sb->Append((const UTF8Char*)"\r\n");
		}
		break;
	case 34:
		{
			sb->Append((const UTF8Char*)"Get Current Angle");
			sb->Append((const UTF8Char*)"\r\n");
		}
		break;
	case 35:
		{
			sb->Append((const UTF8Char*)"Get Current Angle Reply");
			sb->Append((const UTF8Char*)"\r\n");
			sb->Append((const UTF8Char*)"H=");
			Text::SBAppendF64(sb, ReadDouble(&buff[8]));
			sb->Append((const UTF8Char*)"\r\n");
			sb->Append((const UTF8Char*)"V=");
			Text::SBAppendF64(sb, ReadDouble(&buff[16]));
			sb->Append((const UTF8Char*)"\r\n");
		}
		break;
	case 36:
		{
			sb->Append((const UTF8Char*)"Turn Laser");
			sb->Append((const UTF8Char*)"\r\n");
			sb->Append((const UTF8Char*)"Value=");
			sb->AppendI32(ReadInt32(&buff[8]));
			sb->Append((const UTF8Char*)"\r\n");
		}
		break;
	default:
		sb->Append((const UTF8Char*)"Unknown");
		sb->Append((const UTF8Char*)"\r\n");
		break;
	}
	return true;
}

Bool IO::ProtoDec::TSProtocolDecoder::IsValid(UInt8 *buff, UOSInt buffSize)
{
	return buff[0] == 'T' && buff[1] == 's';
}
