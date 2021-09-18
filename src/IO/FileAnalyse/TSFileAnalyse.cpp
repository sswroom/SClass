#include "Stdafx.h"
#include "Data/ByteTool.h"
#include "IO/FileStream.h"
#include "IO/FileAnalyse/TSFileAnalyse.h"
#include "Sync/Thread.h"

IO::FileAnalyse::TSFileAnalyse::TSFileAnalyse(IO::IStreamData *fd)
{
	UInt8 buff[256];
	this->fd = 0;
	this->packSize = 192;
	this->fileSize = 0;
	this->hasTime = false;
	fd->GetRealData(0, 256, buff);
	if (buff[0] == 0x47 && buff[188] == 0x47)
	{
		this->packSize = 188;
		this->hasTime = false;
	}
	else if (buff[4] == 0x47 && buff[196] == 0x47)
	{
		this->packSize = 192;
		this->hasTime = true;
	}
	else
	{
		return;
	}
	this->fileSize = fd->GetDataSize();
	this->fd = fd->GetPartialData(0, this->fileSize);
}

IO::FileAnalyse::TSFileAnalyse::~TSFileAnalyse()
{
	SDEL_CLASS(this->fd);
}

const UTF8Char *IO::FileAnalyse::TSFileAnalyse::GetFormatName()
{
	return (const UTF8Char*)"Transport Stream (TS)";
}

UOSInt IO::FileAnalyse::TSFileAnalyse::GetFrameCount()
{
	return (UOSInt)(this->fileSize / this->packSize);
}

Bool IO::FileAnalyse::TSFileAnalyse::GetFrameName(UOSInt index, Text::StringBuilderUTF *sb)
{
	UInt64 fileOfst = index * this->packSize;
	if (fileOfst >= this->fileSize)
		return false;

	sb->AppendU64(fileOfst);
	sb->Append((const UTF8Char*)", size=");
	sb->AppendU32(this->packSize);
	return true;
}

Bool IO::FileAnalyse::TSFileAnalyse::GetFrameDetail(UOSInt index, Text::StringBuilderUTF *sb)
{
	UInt64 fileOfst = index * this->packSize;
	if (fileOfst >= this->fileSize)
		return false;

	UInt8 buff[192];
	fd->GetRealData(fileOfst, this->packSize, buff);
	sb->AppendHexBuff(buff, this->packSize, ' ', Text::LBT_CRLF);
	sb->Append((const UTF8Char*)"\r\n");

	UOSInt currOfst;
	if (this->hasTime)
	{
		if (buff[4] == 0x47)
		{
			sb->Append((const UTF8Char*)"Time=");
			sb->AppendI32(ReadMInt32(buff));
			sb->Append((const UTF8Char*)"\r\n");
		}
		currOfst = 4;
	}
	else
	{
		currOfst = 0;
	}
	if (buff[currOfst] == 0x47)
	{
		Int32 adaptation_field_control;
		sb->Append((const UTF8Char*)"transport_error_indicator=");
		sb->AppendU16((buff[currOfst + 1] & 0x80)?1:0);
		sb->Append((const UTF8Char*)"\r\n");

		sb->Append((const UTF8Char*)"payload_unit_start_indicator=");
		sb->AppendU16((buff[currOfst + 1] & 0x40)?1:0);
		sb->Append((const UTF8Char*)"\r\n");

		sb->Append((const UTF8Char*)"transport_priority=");
		sb->AppendU16((buff[currOfst + 1] & 0x20)?1:0);
		sb->Append((const UTF8Char*)"\r\n");

		sb->Append((const UTF8Char*)"PID=0x");
		sb->AppendHex16(ReadMInt16(&buff[currOfst + 1]) & 0x1fff);
		sb->Append((const UTF8Char*)"\r\n");

		sb->Append((const UTF8Char*)"transport_scrambling_control=");
		sb->AppendU16((UInt8)(buff[currOfst + 3] >> 6));
		sb->Append((const UTF8Char*)"\r\n");

		adaptation_field_control = (buff[currOfst + 3] >> 4) & 3;
		sb->Append((const UTF8Char*)"adaptation_field_control=");
		sb->AppendI32(adaptation_field_control);
		sb->Append((const UTF8Char*)"\r\n");

		sb->Append((const UTF8Char*)"continuity_counter=");
		sb->AppendU16(buff[currOfst + 3] & 15);
		sb->Append((const UTF8Char*)"\r\n");

		currOfst += 4;
		if (adaptation_field_control == 2 || adaptation_field_control == 3)
		{
			UInt8 adaptation_field_length = buff[currOfst];
			currOfst += 1;
			if (adaptation_field_length > 0)
			{
				UInt8 flags = buff[currOfst];
				currOfst += 1;

				sb->Append((const UTF8Char*)"discontinuity_indicator=");
				sb->AppendU16((flags & 0x80)?1:0);
				sb->Append((const UTF8Char*)"\r\n");

				sb->Append((const UTF8Char*)"random_access_indicator=");
				sb->AppendU16((flags & 0x40)?1:0);
				sb->Append((const UTF8Char*)"\r\n");

				sb->Append((const UTF8Char*)"elementary_stream_priority_indicator=");
				sb->AppendU16((flags & 0x20)?1:0);
				sb->Append((const UTF8Char*)"\r\n");

				sb->Append((const UTF8Char*)"PCR_flag=");
				sb->AppendU16((flags & 0x10)?1:0);
				sb->Append((const UTF8Char*)"\r\n");

				sb->Append((const UTF8Char*)"OPCR_flag=");
				sb->AppendU16((flags & 0x8)?1:0);
				sb->Append((const UTF8Char*)"\r\n");

				sb->Append((const UTF8Char*)"splicing_point_flag=");
				sb->AppendU16((flags & 0x4)?1:0);
				sb->Append((const UTF8Char*)"\r\n");

				sb->Append((const UTF8Char*)"transport_private_data_flag=");
				sb->AppendU16((flags & 0x2)?1:0);
				sb->Append((const UTF8Char*)"\r\n");

				sb->Append((const UTF8Char*)"adaptation_field_extension_flag=");
				sb->AppendU16((flags & 0x1)?1:0);
				sb->Append((const UTF8Char*)"\r\n");

				if (flags & 0x10)
				{
					Int64 program_clock_reference = ReadMUInt32(&buff[currOfst]);
					program_clock_reference = (program_clock_reference << 1) | (buff[currOfst + 4] >> 7);
					program_clock_reference = program_clock_reference * 300 + (ReadMUInt16(&buff[currOfst + 4]) & 0x1ff);
					sb->Append((const UTF8Char*)"program_clock_reference=");
					sb->AppendI64(program_clock_reference);
					sb->Append((const UTF8Char*)"\r\n");
					currOfst += 6;
				}
				if (flags & 0x8)
				{
					Int64 original_program_clock_reference = ReadMUInt32(&buff[currOfst]);
					original_program_clock_reference = (original_program_clock_reference << 1) | (buff[currOfst + 4] >> 7);
					original_program_clock_reference = original_program_clock_reference * 300 + (ReadMUInt16(&buff[currOfst + 4]) & 0x1ff);
					sb->Append((const UTF8Char*)"original_program_clock_reference=");
					sb->AppendI64(original_program_clock_reference);
					sb->Append((const UTF8Char*)"\r\n");
					currOfst += 6;
				}
				if (flags & 4)
				{
					sb->Append((const UTF8Char*)"splice_countdown=");
					sb->AppendU16(buff[currOfst]);
					sb->Append((const UTF8Char*)"\r\n");
					currOfst += 1;
				}
				if (flags & 2)
				{
					UInt8 transport_private_data_length = buff[currOfst];
					sb->Append((const UTF8Char*)"transport_private_data_length=");
					sb->AppendU16(transport_private_data_length);
					sb->Append((const UTF8Char*)"\r\n");
					sb->Append((const UTF8Char*)"private_data=");
					sb->AppendHexBuff(&buff[currOfst], transport_private_data_length, ' ', Text::LBT_NONE);
					sb->Append((const UTF8Char*)"\r\n");
					currOfst += 1 + (UOSInt)transport_private_data_length;
				}
				if (flags & 1)
				{
					UInt8 adaptation_field_extension_length = buff[currOfst];
					UInt8 adflags = buff[currOfst + 1];
					UOSInt endOfst = currOfst + 1 + adaptation_field_extension_length;
					currOfst += 2;

					sb->Append((const UTF8Char*)"adaptation_field_extension_length=");
					sb->AppendU16(adaptation_field_extension_length);
					sb->Append((const UTF8Char*)"\r\n");

					sb->Append((const UTF8Char*)"ltw_flag=");
					sb->AppendU16((adflags & 0x80)?1:0);
					sb->Append((const UTF8Char*)"\r\n");

					sb->Append((const UTF8Char*)"piecewise_rate_flag=");
					sb->AppendU16((adflags & 0x40)?1:0);
					sb->Append((const UTF8Char*)"\r\n");

					sb->Append((const UTF8Char*)"seamless_splice_flag=");
					sb->AppendU16((adflags & 0x20)?1:0);
					sb->Append((const UTF8Char*)"\r\n");

					if (adflags & 0x80)
					{
						sb->Append((const UTF8Char*)"ltw_valid_flag=");
						sb->AppendU16((buff[currOfst] & 0x80)?1:0);
						sb->Append((const UTF8Char*)"\r\n");

						sb->Append((const UTF8Char*)"ltw_offset=");
						sb->AppendU16(0x7fff & ReadMUInt16(&buff[currOfst]));
						sb->Append((const UTF8Char*)"\r\n");

						currOfst += 2;
					}
					if (adflags & 0x40)
					{
						sb->Append((const UTF8Char*)"piecewise_rate=");
						sb->AppendU32(0x3fffff & ReadMUInt24(&buff[currOfst]));
						sb->Append((const UTF8Char*)"\r\n");

						currOfst += 3;
					}
					if (adflags & 0x20)
					{
						Int64 DTS_next_AU;
						sb->Append((const UTF8Char*)"splice_type=");
						sb->AppendU16((UInt8)(buff[currOfst] >> 4));
						sb->Append((const UTF8Char*)"\r\n");

						DTS_next_AU = buff[currOfst] & 0xe;
						DTS_next_AU = (DTS_next_AU << 29) | ((ReadMUInt16(&buff[currOfst + 1]) & 0xfffe) << 14) | (ReadMUInt16(&buff[currOfst + 3]) >> 1);

						sb->Append((const UTF8Char*)"DTS_next_AU=");
						sb->AppendI64(DTS_next_AU);
						sb->Append((const UTF8Char*)"\r\n");
						currOfst += 5;
					}

					currOfst = endOfst;
				}
			}
		}
	}

	return false;

}

UOSInt IO::FileAnalyse::TSFileAnalyse::GetFrameIndex(UInt64 ofst)
{
	if (ofst >= this->fileSize)
	{
		return INVALID_INDEX;
	}
	return (UOSInt)(ofst / this->packSize);
}

IO::FileAnalyse::FrameDetail *IO::FileAnalyse::TSFileAnalyse::GetFrameDetail(UOSInt index)
{
	IO::FileAnalyse::FrameDetail *frame;
	UInt64 fileOfst = index * this->packSize;
	if (fileOfst >= this->fileSize)
		return 0;

	NEW_CLASS(frame, IO::FileAnalyse::FrameDetail(fileOfst, this->packSize));
	return frame;
/*	UInt8 buff[192];
	fd->GetRealData(fileOfst, this->packSize, buff);
	sb->AppendHexBuff(buff, this->packSize, ' ', Text::LBT_CRLF);
	sb->Append((const UTF8Char*)"\r\n");

	UOSInt currOfst;
	if (this->hasTime)
	{
		if (buff[4] == 0x47)
		{
			sb->Append((const UTF8Char*)"Time=");
			sb->AppendI32(ReadMInt32(buff));
			sb->Append((const UTF8Char*)"\r\n");
		}
		currOfst = 4;
	}
	else
	{
		currOfst = 0;
	}
	if (buff[currOfst] == 0x47)
	{
		Int32 adaptation_field_control;
		sb->Append((const UTF8Char*)"transport_error_indicator=");
		sb->AppendU16((buff[currOfst + 1] & 0x80)?1:0);
		sb->Append((const UTF8Char*)"\r\n");

		sb->Append((const UTF8Char*)"payload_unit_start_indicator=");
		sb->AppendU16((buff[currOfst + 1] & 0x40)?1:0);
		sb->Append((const UTF8Char*)"\r\n");

		sb->Append((const UTF8Char*)"transport_priority=");
		sb->AppendU16((buff[currOfst + 1] & 0x20)?1:0);
		sb->Append((const UTF8Char*)"\r\n");

		sb->Append((const UTF8Char*)"PID=0x");
		sb->AppendHex16(ReadMInt16(&buff[currOfst + 1]) & 0x1fff);
		sb->Append((const UTF8Char*)"\r\n");

		sb->Append((const UTF8Char*)"transport_scrambling_control=");
		sb->AppendU16((UInt8)(buff[currOfst + 3] >> 6));
		sb->Append((const UTF8Char*)"\r\n");

		adaptation_field_control = (buff[currOfst + 3] >> 4) & 3;
		sb->Append((const UTF8Char*)"adaptation_field_control=");
		sb->AppendI32(adaptation_field_control);
		sb->Append((const UTF8Char*)"\r\n");

		sb->Append((const UTF8Char*)"continuity_counter=");
		sb->AppendU16(buff[currOfst + 3] & 15);
		sb->Append((const UTF8Char*)"\r\n");

		currOfst += 4;
		if (adaptation_field_control == 2 || adaptation_field_control == 3)
		{
			UInt8 adaptation_field_length = buff[currOfst];
			currOfst += 1;
			if (adaptation_field_length > 0)
			{
				UInt8 flags = buff[currOfst];
				currOfst += 1;

				sb->Append((const UTF8Char*)"discontinuity_indicator=");
				sb->AppendU16((flags & 0x80)?1:0);
				sb->Append((const UTF8Char*)"\r\n");

				sb->Append((const UTF8Char*)"random_access_indicator=");
				sb->AppendU16((flags & 0x40)?1:0);
				sb->Append((const UTF8Char*)"\r\n");

				sb->Append((const UTF8Char*)"elementary_stream_priority_indicator=");
				sb->AppendU16((flags & 0x20)?1:0);
				sb->Append((const UTF8Char*)"\r\n");

				sb->Append((const UTF8Char*)"PCR_flag=");
				sb->AppendU16((flags & 0x10)?1:0);
				sb->Append((const UTF8Char*)"\r\n");

				sb->Append((const UTF8Char*)"OPCR_flag=");
				sb->AppendU16((flags & 0x8)?1:0);
				sb->Append((const UTF8Char*)"\r\n");

				sb->Append((const UTF8Char*)"splicing_point_flag=");
				sb->AppendU16((flags & 0x4)?1:0);
				sb->Append((const UTF8Char*)"\r\n");

				sb->Append((const UTF8Char*)"transport_private_data_flag=");
				sb->AppendU16((flags & 0x2)?1:0);
				sb->Append((const UTF8Char*)"\r\n");

				sb->Append((const UTF8Char*)"adaptation_field_extension_flag=");
				sb->AppendU16((flags & 0x1)?1:0);
				sb->Append((const UTF8Char*)"\r\n");

				if (flags & 0x10)
				{
					Int64 program_clock_reference = ReadMUInt32(&buff[currOfst]);
					program_clock_reference = (program_clock_reference << 1) | (buff[currOfst + 4] >> 7);
					program_clock_reference = program_clock_reference * 300 + (ReadMUInt16(&buff[currOfst + 4]) & 0x1ff);
					sb->Append((const UTF8Char*)"program_clock_reference=");
					sb->AppendI64(program_clock_reference);
					sb->Append((const UTF8Char*)"\r\n");
					currOfst += 6;
				}
				if (flags & 0x8)
				{
					Int64 original_program_clock_reference = ReadMUInt32(&buff[currOfst]);
					original_program_clock_reference = (original_program_clock_reference << 1) | (buff[currOfst + 4] >> 7);
					original_program_clock_reference = original_program_clock_reference * 300 + (ReadMUInt16(&buff[currOfst + 4]) & 0x1ff);
					sb->Append((const UTF8Char*)"original_program_clock_reference=");
					sb->AppendI64(original_program_clock_reference);
					sb->Append((const UTF8Char*)"\r\n");
					currOfst += 6;
				}
				if (flags & 4)
				{
					sb->Append((const UTF8Char*)"splice_countdown=");
					sb->AppendU16(buff[currOfst]);
					sb->Append((const UTF8Char*)"\r\n");
					currOfst += 1;
				}
				if (flags & 2)
				{
					UInt8 transport_private_data_length = buff[currOfst];
					sb->Append((const UTF8Char*)"transport_private_data_length=");
					sb->AppendU16(transport_private_data_length);
					sb->Append((const UTF8Char*)"\r\n");
					sb->Append((const UTF8Char*)"private_data=");
					sb->AppendHexBuff(&buff[currOfst], transport_private_data_length, ' ', Text::LBT_NONE);
					sb->Append((const UTF8Char*)"\r\n");
					currOfst += 1 + (UOSInt)transport_private_data_length;
				}
				if (flags & 1)
				{
					UInt8 adaptation_field_extension_length = buff[currOfst];
					UInt8 adflags = buff[currOfst + 1];
					UOSInt endOfst = currOfst + 1 + adaptation_field_extension_length;
					currOfst += 2;

					sb->Append((const UTF8Char*)"adaptation_field_extension_length=");
					sb->AppendU16(adaptation_field_extension_length);
					sb->Append((const UTF8Char*)"\r\n");

					sb->Append((const UTF8Char*)"ltw_flag=");
					sb->AppendU16((adflags & 0x80)?1:0);
					sb->Append((const UTF8Char*)"\r\n");

					sb->Append((const UTF8Char*)"piecewise_rate_flag=");
					sb->AppendU16((adflags & 0x40)?1:0);
					sb->Append((const UTF8Char*)"\r\n");

					sb->Append((const UTF8Char*)"seamless_splice_flag=");
					sb->AppendU16((adflags & 0x20)?1:0);
					sb->Append((const UTF8Char*)"\r\n");

					if (adflags & 0x80)
					{
						sb->Append((const UTF8Char*)"ltw_valid_flag=");
						sb->AppendU16((buff[currOfst] & 0x80)?1:0);
						sb->Append((const UTF8Char*)"\r\n");

						sb->Append((const UTF8Char*)"ltw_offset=");
						sb->AppendU16(0x7fff & ReadMUInt16(&buff[currOfst]));
						sb->Append((const UTF8Char*)"\r\n");

						currOfst += 2;
					}
					if (adflags & 0x40)
					{
						sb->Append((const UTF8Char*)"piecewise_rate=");
						sb->AppendU32(0x3fffff & ReadMUInt24(&buff[currOfst]));
						sb->Append((const UTF8Char*)"\r\n");

						currOfst += 3;
					}
					if (adflags & 0x20)
					{
						Int64 DTS_next_AU;
						sb->Append((const UTF8Char*)"splice_type=");
						sb->AppendU16((UInt8)(buff[currOfst] >> 4));
						sb->Append((const UTF8Char*)"\r\n");

						DTS_next_AU = buff[currOfst] & 0xe;
						DTS_next_AU = (DTS_next_AU << 29) | ((ReadMUInt16(&buff[currOfst + 1]) & 0xfffe) << 14) | (ReadMUInt16(&buff[currOfst + 3]) >> 1);

						sb->Append((const UTF8Char*)"DTS_next_AU=");
						sb->AppendI64(DTS_next_AU);
						sb->Append((const UTF8Char*)"\r\n");
						currOfst += 5;
					}

					currOfst = endOfst;
				}
			}
		}
	}

	return false;*/
}

Bool IO::FileAnalyse::TSFileAnalyse::IsError()
{
	return this->fd == 0;
}

Bool IO::FileAnalyse::TSFileAnalyse::IsParsing()
{
	return false;
}

Bool IO::FileAnalyse::TSFileAnalyse::TrimPadding(const UTF8Char *outputFile)
{
	return false;
}
