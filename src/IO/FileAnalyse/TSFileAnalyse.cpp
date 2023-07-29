#include "Stdafx.h"
#include "Data/ByteTool.h"
#include "IO/FileStream.h"
#include "IO/FileAnalyse/TSFileAnalyse.h"
#include "Sync/ThreadUtil.h"

IO::FileAnalyse::TSFileAnalyse::TSFileAnalyse(NotNullPtr<IO::StreamData> fd)
{
	UInt8 buff[256];
	this->fd = 0;
	this->packSize = 192;
	this->fileSize = 0;
	this->hasTime = false;
	fd->GetRealData(0, 256, BYTEARR(buff));
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
	this->fd = fd->GetPartialData(0, this->fileSize).Ptr();
}

IO::FileAnalyse::TSFileAnalyse::~TSFileAnalyse()
{
	SDEL_CLASS(this->fd);
}

Text::CString IO::FileAnalyse::TSFileAnalyse::GetFormatName()
{
	return CSTR("Transport Stream (TS)");
}

UOSInt IO::FileAnalyse::TSFileAnalyse::GetFrameCount()
{
	return (UOSInt)(this->fileSize / this->packSize);
}

Bool IO::FileAnalyse::TSFileAnalyse::GetFrameName(UOSInt index, NotNullPtr<Text::StringBuilderUTF8> sb)
{
	UInt64 fileOfst = index * this->packSize;
	if (fileOfst >= this->fileSize)
		return false;

	sb->AppendU64(fileOfst);
	sb->AppendC(UTF8STRC(", size="));
	sb->AppendU32(this->packSize);
	return true;
}

Bool IO::FileAnalyse::TSFileAnalyse::GetFrameDetail(UOSInt index, NotNullPtr<Text::StringBuilderUTF8> sb)
{
	UInt64 fileOfst = index * this->packSize;
	if (fileOfst >= this->fileSize)
		return false;

	UInt8 buff[192];
	fd->GetRealData(fileOfst, this->packSize, BYTEARR(buff));
	sb->AppendHexBuff(buff, this->packSize, ' ', Text::LineBreakType::CRLF);
	sb->AppendC(UTF8STRC("\r\n"));

	UOSInt currOfst;
	if (this->hasTime)
	{
		if (buff[4] == 0x47)
		{
			sb->AppendC(UTF8STRC("Time="));
			sb->AppendI32(ReadMInt32(buff));
			sb->AppendC(UTF8STRC("\r\n"));
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
		sb->AppendC(UTF8STRC("transport_error_indicator="));
		sb->AppendU16((buff[currOfst + 1] & 0x80)?1:0);
		sb->AppendC(UTF8STRC("\r\n"));

		sb->AppendC(UTF8STRC("payload_unit_start_indicator="));
		sb->AppendU16((buff[currOfst + 1] & 0x40)?1:0);
		sb->AppendC(UTF8STRC("\r\n"));

		sb->AppendC(UTF8STRC("transport_priority="));
		sb->AppendU16((buff[currOfst + 1] & 0x20)?1:0);
		sb->AppendC(UTF8STRC("\r\n"));

		sb->AppendC(UTF8STRC("PID=0x"));
		sb->AppendHex16(ReadMInt16(&buff[currOfst + 1]) & 0x1fff);
		sb->AppendC(UTF8STRC("\r\n"));

		sb->AppendC(UTF8STRC("transport_scrambling_control="));
		sb->AppendU16((UInt8)(buff[currOfst + 3] >> 6));
		sb->AppendC(UTF8STRC("\r\n"));

		adaptation_field_control = (buff[currOfst + 3] >> 4) & 3;
		sb->AppendC(UTF8STRC("adaptation_field_control="));
		sb->AppendI32(adaptation_field_control);
		sb->AppendC(UTF8STRC("\r\n"));

		sb->AppendC(UTF8STRC("continuity_counter="));
		sb->AppendU16(buff[currOfst + 3] & 15);
		sb->AppendC(UTF8STRC("\r\n"));

		currOfst += 4;
		if (adaptation_field_control == 2 || adaptation_field_control == 3)
		{
			UInt8 adaptation_field_length = buff[currOfst];
			currOfst += 1;
			if (adaptation_field_length > 0)
			{
				UInt8 flags = buff[currOfst];
				currOfst += 1;

				sb->AppendC(UTF8STRC("discontinuity_indicator="));
				sb->AppendU16((flags & 0x80)?1:0);
				sb->AppendC(UTF8STRC("\r\n"));

				sb->AppendC(UTF8STRC("random_access_indicator="));
				sb->AppendU16((flags & 0x40)?1:0);
				sb->AppendC(UTF8STRC("\r\n"));

				sb->AppendC(UTF8STRC("elementary_stream_priority_indicator="));
				sb->AppendU16((flags & 0x20)?1:0);
				sb->AppendC(UTF8STRC("\r\n"));

				sb->AppendC(UTF8STRC("PCR_flag="));
				sb->AppendU16((flags & 0x10)?1:0);
				sb->AppendC(UTF8STRC("\r\n"));

				sb->AppendC(UTF8STRC("OPCR_flag="));
				sb->AppendU16((flags & 0x8)?1:0);
				sb->AppendC(UTF8STRC("\r\n"));

				sb->AppendC(UTF8STRC("splicing_point_flag="));
				sb->AppendU16((flags & 0x4)?1:0);
				sb->AppendC(UTF8STRC("\r\n"));

				sb->AppendC(UTF8STRC("transport_private_data_flag="));
				sb->AppendU16((flags & 0x2)?1:0);
				sb->AppendC(UTF8STRC("\r\n"));

				sb->AppendC(UTF8STRC("adaptation_field_extension_flag="));
				sb->AppendU16((flags & 0x1)?1:0);
				sb->AppendC(UTF8STRC("\r\n"));

				if (flags & 0x10)
				{
					Int64 program_clock_reference = ReadMUInt32(&buff[currOfst]);
					program_clock_reference = (program_clock_reference << 1) | (buff[currOfst + 4] >> 7);
					program_clock_reference = program_clock_reference * 300 + (ReadMUInt16(&buff[currOfst + 4]) & 0x1ff);
					sb->AppendC(UTF8STRC("program_clock_reference="));
					sb->AppendI64(program_clock_reference);
					sb->AppendC(UTF8STRC("\r\n"));
					currOfst += 6;
				}
				if (flags & 0x8)
				{
					Int64 original_program_clock_reference = ReadMUInt32(&buff[currOfst]);
					original_program_clock_reference = (original_program_clock_reference << 1) | (buff[currOfst + 4] >> 7);
					original_program_clock_reference = original_program_clock_reference * 300 + (ReadMUInt16(&buff[currOfst + 4]) & 0x1ff);
					sb->AppendC(UTF8STRC("original_program_clock_reference="));
					sb->AppendI64(original_program_clock_reference);
					sb->AppendC(UTF8STRC("\r\n"));
					currOfst += 6;
				}
				if (flags & 4)
				{
					sb->AppendC(UTF8STRC("splice_countdown="));
					sb->AppendU16(buff[currOfst]);
					sb->AppendC(UTF8STRC("\r\n"));
					currOfst += 1;
				}
				if (flags & 2)
				{
					UInt8 transport_private_data_length = buff[currOfst];
					sb->AppendC(UTF8STRC("transport_private_data_length="));
					sb->AppendU16(transport_private_data_length);
					sb->AppendC(UTF8STRC("\r\n"));
					sb->AppendC(UTF8STRC("private_data="));
					sb->AppendHexBuff(&buff[currOfst], transport_private_data_length, ' ', Text::LineBreakType::None);
					sb->AppendC(UTF8STRC("\r\n"));
					currOfst += 1 + (UOSInt)transport_private_data_length;
				}
				if (flags & 1)
				{
					UInt8 adaptation_field_extension_length = buff[currOfst];
					UInt8 adflags = buff[currOfst + 1];
					UOSInt endOfst = currOfst + 1 + adaptation_field_extension_length;
					currOfst += 2;

					sb->AppendC(UTF8STRC("adaptation_field_extension_length="));
					sb->AppendU16(adaptation_field_extension_length);
					sb->AppendC(UTF8STRC("\r\n"));

					sb->AppendC(UTF8STRC("ltw_flag="));
					sb->AppendU16((adflags & 0x80)?1:0);
					sb->AppendC(UTF8STRC("\r\n"));

					sb->AppendC(UTF8STRC("piecewise_rate_flag="));
					sb->AppendU16((adflags & 0x40)?1:0);
					sb->AppendC(UTF8STRC("\r\n"));

					sb->AppendC(UTF8STRC("seamless_splice_flag="));
					sb->AppendU16((adflags & 0x20)?1:0);
					sb->AppendC(UTF8STRC("\r\n"));

					if (adflags & 0x80)
					{
						sb->AppendC(UTF8STRC("ltw_valid_flag="));
						sb->AppendU16((buff[currOfst] & 0x80)?1:0);
						sb->AppendC(UTF8STRC("\r\n"));

						sb->AppendC(UTF8STRC("ltw_offset="));
						sb->AppendU16(0x7fff & ReadMUInt16(&buff[currOfst]));
						sb->AppendC(UTF8STRC("\r\n"));

						currOfst += 2;
					}
					if (adflags & 0x40)
					{
						sb->AppendC(UTF8STRC("piecewise_rate="));
						sb->AppendU32(0x3fffff & ReadMUInt24(&buff[currOfst]));
						sb->AppendC(UTF8STRC("\r\n"));

						currOfst += 3;
					}
					if (adflags & 0x20)
					{
						Int64 DTS_next_AU;
						sb->AppendC(UTF8STRC("splice_type="));
						sb->AppendU16((UInt8)(buff[currOfst] >> 4));
						sb->AppendC(UTF8STRC("\r\n"));

						DTS_next_AU = buff[currOfst] & 0xe;
						DTS_next_AU = (DTS_next_AU << 29) | ((ReadMUInt16(&buff[currOfst + 1]) & 0xfffe) << 14) | (ReadMUInt16(&buff[currOfst + 3]) >> 1);

						sb->AppendC(UTF8STRC("DTS_next_AU="));
						sb->AppendI64(DTS_next_AU);
						sb->AppendC(UTF8STRC("\r\n"));
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

	UTF8Char sbuff[32];
	UTF8Char *sptr;
	NEW_CLASS(frame, IO::FileAnalyse::FrameDetail(fileOfst, this->packSize));
	UInt8 buff[192];
	fd->GetRealData(fileOfst, this->packSize, BYTEARR(buff));

	UOSInt currOfst;
	if (this->hasTime)
	{
		if (buff[4] == 0x47)
		{
			frame->AddInt(0, 4, CSTR("Time"), ReadMInt32(buff));
		}
		currOfst = 4;
	}
	else
	{
		currOfst = 0;
	}
	if (buff[currOfst] == 0x47)
	{
		frame->AddHex8(currOfst, CSTR("Packet Sync"), buff[currOfst]);
		UInt32 adaptation_field_control;
		frame->AddUInt(currOfst + 1, 2, CSTR("transport_error_indicator"), (buff[currOfst + 1] & 0x80) >> 7);
		frame->AddUInt(currOfst + 1, 2, CSTR("payload_unit_start_indicator"), (buff[currOfst + 1] & 0x40) >> 6);
		frame->AddUInt(currOfst + 1, 2, CSTR("transport_priority"), (buff[currOfst + 1] & 0x20) >> 5);
		frame->AddHex16(currOfst + 1, CSTR("PID"), ReadMInt16(&buff[currOfst + 1]) & 0x1fff);
		frame->AddUInt(currOfst + 3, 1, CSTR("transport_scrambling_control"), (UInt8)(buff[currOfst + 3] >> 6));
		adaptation_field_control = (buff[currOfst + 3] >> 4) & 3;
		frame->AddUInt(currOfst + 3, 1, CSTR("adaptation_field_control"), adaptation_field_control);
		frame->AddUInt(currOfst + 3, 1, CSTR("continuity_counter"), buff[currOfst + 3] & 15);

		currOfst += 4;
		if (adaptation_field_control == 2 || adaptation_field_control == 3)
		{
			UInt8 adaptation_field_length = buff[currOfst];
			frame->AddUInt(currOfst, 1, CSTR("adaptation_field_length"), adaptation_field_length	);
			currOfst += 1;
			if (adaptation_field_length > 0)
			{
				UOSInt adaptationEnd = currOfst + adaptation_field_length;
				UInt8 flags = buff[currOfst];
				frame->AddUInt(currOfst, 1, CSTR("discontinuity_indicator"), (flags & 0x80) >> 7);
				frame->AddUInt(currOfst, 1, CSTR("random_access_indicator"), (flags & 0x40) >> 6);
				frame->AddUInt(currOfst, 1, CSTR("elementary_stream_priority_indicator"), (flags & 0x20) >> 5);
				frame->AddUInt(currOfst, 1, CSTR("PCR_flag"), (flags & 0x10) >> 4);
				frame->AddUInt(currOfst, 1, CSTR("OPCR_flag"), (flags & 0x8) >> 3);
				frame->AddUInt(currOfst, 1, CSTR("splicing_point_flag"), (flags & 0x4) >> 2);
				frame->AddUInt(currOfst, 1, CSTR("transport_private_data_flag"), (flags & 0x2) >> 1);
				frame->AddUInt(currOfst, 1, CSTR("adaptation_field_extension_flag"), (flags & 0x1) >> 0);
				currOfst += 1;

				if (flags & 0x10)
				{
					UInt64 program_clock_reference = ReadMUInt32(&buff[currOfst]);
					program_clock_reference = (program_clock_reference << 1) | (UInt32)(buff[currOfst + 4] >> 7);
					program_clock_reference = program_clock_reference * 300 + (ReadMUInt16(&buff[currOfst + 4]) & 0x1ff);
					sptr = Text::StrUInt64(sbuff, program_clock_reference);
					frame->AddField(currOfst, 6, CSTR("program_clock_reference"), CSTRP(sbuff, sptr));
					currOfst += 6;
				}
				if (flags & 0x8)
				{
					UInt64 original_program_clock_reference = ReadMUInt32(&buff[currOfst]);
					original_program_clock_reference = (original_program_clock_reference << 1) | (UInt32)(buff[currOfst + 4] >> 7);
					original_program_clock_reference = original_program_clock_reference * 300 + (ReadMUInt16(&buff[currOfst + 4]) & 0x1ff);
					sptr = Text::StrUInt64(sbuff, original_program_clock_reference);
					frame->AddField(currOfst, 6, CSTR("original_program_clock_reference"), CSTRP(sbuff, sptr));
					currOfst += 6;
				}
				if (flags & 4)
				{
					frame->AddUInt(currOfst, 1, CSTR("splice_countdown"), buff[currOfst]);
					currOfst += 1;
				}
				if (flags & 2)
				{
					UInt8 transport_private_data_length = buff[currOfst];
					frame->AddUInt(currOfst, 1, CSTR("transport_private_data_length"), transport_private_data_length);
					frame->AddHexBuff(currOfst + 1, transport_private_data_length, CSTR("private_data"), &buff[currOfst], false);
					currOfst += 1 + (UOSInt)transport_private_data_length;
				}
				if (flags & 1)
				{
					UInt8 adaptation_field_extension_length = buff[currOfst];
					UInt8 adflags = buff[currOfst + 1];
					UOSInt endOfst = currOfst + 1 + adaptation_field_extension_length;
					currOfst += 2;

					frame->AddUInt(currOfst, 1, CSTR("adaptation_field_extension_length"), adaptation_field_extension_length);
					frame->AddUInt(currOfst + 1, 1, CSTR("ltw_flag"), (adflags & 0x80) >> 7);
					frame->AddUInt(currOfst + 1, 1, CSTR("piecewise_rate_flag"), (adflags & 0x40) >> 6);
					frame->AddUInt(currOfst + 1, 1, CSTR("seamless_splice_flag"), (adflags & 0x20) >> 5);

					if (adflags & 0x80)
					{
						frame->AddUInt(currOfst, 2, CSTR("ltw_valid_flag"), (buff[currOfst] & 0x80) >> 7);
						frame->AddUInt(currOfst, 2, CSTR("ltw_offset"), 0x7fff & ReadMUInt16(&buff[currOfst]));
						currOfst += 2;
					}
					if (adflags & 0x40)
					{
						frame->AddUInt(currOfst, 3, CSTR("piecewise_rate"), 0x3fffff & ReadMUInt24(&buff[currOfst]));
						currOfst += 3;
					}
					if (adflags & 0x20)
					{
						UInt64 DTS_next_AU;
						frame->AddUInt(currOfst, 5, CSTR("splice_type"), (UInt8)(buff[currOfst] >> 4));
						DTS_next_AU = buff[currOfst] & 0xe;
						DTS_next_AU = (DTS_next_AU << 29) | ((ReadMUInt16(&buff[currOfst + 1]) & 0xfffe) << 14) | (UInt32)(ReadMUInt16(&buff[currOfst + 3]) >> 1);
						sptr = Text::StrUInt64(sbuff, DTS_next_AU);
						frame->AddField(currOfst, 5, CSTR("DTS_next_AU"), CSTRP(sbuff, sptr));
						currOfst += 5;
					}

					currOfst = endOfst;
				}
				frame->AddHexBuff(currOfst, adaptationEnd - currOfst, CSTR("adaptation_field"), &buff[currOfst], true);
				currOfst = adaptationEnd;
			}
		}
		frame->AddHexBuff(currOfst, this->packSize - currOfst, CSTR("Packet Data"), &buff[currOfst], true);
	}
	else
	{
		frame->AddTextHexBuff(0, this->packSize, buff, true);
	}

	return frame;
}

Bool IO::FileAnalyse::TSFileAnalyse::IsError()
{
	return this->fd == 0;
}

Bool IO::FileAnalyse::TSFileAnalyse::IsParsing()
{
	return false;
}

Bool IO::FileAnalyse::TSFileAnalyse::TrimPadding(Text::CString outputFile)
{
	return false;
}
