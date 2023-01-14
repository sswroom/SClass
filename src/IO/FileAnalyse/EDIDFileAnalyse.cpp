#include "Stdafx.h"
#include "Data/ByteTool.h"
#include "IO/FileAnalyse/EDIDFileAnalyse.h"
#include "Text/MyStringFloat.h"
#include "Text/StringBuilderUTF8.h"

//https://en.wikipedia.org/wiki/Extended_Display_Identification_Data

void IO::FileAnalyse::EDIDFileAnalyse::ParseDescriptor(FrameDetail *frame, const UInt8 *buff, UOSInt ofst)
{
	UTF8Char sbuff[16];
	UTF8Char *sptr;
	if (buff[ofst] == 0 && buff[ofst + 1] == 0 && buff[ofst + 2] == 0 && buff[ofst + 4] == 0)
	{
		UInt8 type = buff[ofst + 3];
		frame->AddUIntName(ofst, 2, CSTR("Pixel clock"), 0, CSTR("Display Descriptor"));
		frame->AddUInt(ofst + 2, 1, CSTR("Reserved"), 0);
		frame->AddHex8Name(ofst + 3, CSTR("Descriptor type"), type, DescriptorTypeGetName(type));
		frame->AddUInt(ofst + 4, 1, CSTR("Reserved"), 0);
		if (type == 0xFF)
		{
			sptr = Text::StrConcatC(sbuff, &buff[ofst + 5], 13);
			sptr = Text::StrTrimC(sbuff, (UOSInt)(sptr - sbuff));
			frame->AddField(ofst + 5, 13, CSTR("Display serial number"), CSTRP(sbuff, sptr));
		}
		else if (type == 0xFE)
		{
			sptr = Text::StrConcatC(sbuff, &buff[ofst + 5], 13);
			sptr = Text::StrTrimC(sbuff, (UOSInt)(sptr - sbuff));
			frame->AddField(ofst + 5, 13, CSTR("Unspecified text"), CSTRP(sbuff, sptr));
		}
		else if (type == 0xFC)
		{
			sptr = Text::StrConcatC(sbuff, &buff[ofst + 5], 13);
			sptr = Text::StrTrimC(sbuff, (UOSInt)(sptr - sbuff));
			frame->AddField(ofst + 5, 13, CSTR("Display name"), CSTRP(sbuff, sptr));
		}
		else if (type < 16)
		{
			frame->AddHexBuff(ofst + 5, 13, CSTR("Manufacturer reserved descriptors"), &buff[ofst + 5], false);
		}
		//////////////////////////////////
	}
	else
	{
		frame->AddFloat(ofst, 2, CSTR("Pixel clock (MHz)"), ReadUInt16(&buff[ofst]) * 0.01);
		frame->AddUInt(ofst + 2, 1, CSTR("Horizontal active pixels"), buff[ofst + 2] + ((UOSInt)(buff[ofst + 4] & 0xF0)) * 16);
		frame->AddUInt(ofst + 3, 1, CSTR("Horizontal blanking pixels"), buff[ofst + 3] + ((UOSInt)(buff[ofst + 4] & 0xF)) * 256);
		frame->AddHex8(ofst + 4, CSTR("Horizontal pixels msbits"), buff[ofst + 4]);
		frame->AddUInt(ofst + 5, 1, CSTR("Vertical active lines"), buff[ofst + 5] + ((UOSInt)(buff[ofst + 7] & 0xF0)) * 16);
		frame->AddUInt(ofst + 6, 1, CSTR("Vertical blanking lines"), buff[ofst + 6] + ((UOSInt)(buff[ofst + 7] & 0xF)) * 256);
		frame->AddHex8(ofst + 7, CSTR("Vertical lines msbits"), buff[ofst + 7]);
		frame->AddUInt(ofst + 8, 1, CSTR("Horizontal front porch pixels"), buff[ofst + 8] + ((UOSInt)(buff[ofst + 11] & 0xC0)) * 4);
		frame->AddUInt(ofst + 9, 1, CSTR("Horizontal sync pulse width pixels"), buff[ofst + 9] + ((UOSInt)(buff[ofst + 11] & 0x30)) * 16);
		frame->AddUInt(ofst + 10, 1, CSTR("Vertical front porch pixels"), (buff[ofst + 10] >> 4) + ((UOSInt)(buff[ofst + 11] & 0xC)) * 4);
		frame->AddUInt(ofst + 10, 1, CSTR("Vertical sync pulse width pixels"), (buff[ofst + 10] & 15) + ((UOSInt)(buff[ofst + 11] & 0x3)) * 16);
		frame->AddHex8(ofst + 11, CSTR("Front porch and sync pulse width msbits"), buff[ofst + 11]);
		frame->AddUInt(ofst + 12, 1, CSTR("Horizontal image size (mm)"), buff[ofst + 12] + ((UOSInt)(buff[ofst + 14] & 0xF0)) * 16);
		frame->AddUInt(ofst + 13, 1, CSTR("Vertical image size (mm)"), buff[ofst + 13] + ((UOSInt)(buff[ofst + 14] & 0xF)) * 256);
		frame->AddHex8(ofst + 14, CSTR("Image size msbits"), buff[ofst + 14]);
		frame->AddUInt(ofst + 15, 1, CSTR("Horizontal border pixels"), buff[ofst + 15]);
		frame->AddUInt(ofst + 16, 1, CSTR("Vertical border pixels"), buff[ofst + 16]);
		frame->AddUIntName(ofst + 17, 1, CSTR("Signal Interface Type"), (buff[ofst + 17] >> 7) & 1, (buff[ofst + 17] & 0x80)?CSTR("interlaced"):CSTR("non-interlaced"));
		switch ((buff[ofst + 17] >> 5) & 3)
		{
		case 0:
			frame->AddField(ofst + 17, 1, CSTR("Stereo mode"), CSTR("none"));
			break;
		case 1:
			if (buff[ofst + 17] & 1)
			{
				frame->AddField(ofst + 17, 1, CSTR("Stereo mode"), CSTR("2-way interleaved, right image on even lines"));
			}
			else
			{
				frame->AddField(ofst + 17, 1, CSTR("Stereo mode"), CSTR("field sequential, right during stereo sync"));
			}
			break;
		case 2:
			if (buff[ofst + 17] & 1)
			{
				frame->AddField(ofst + 17, 1, CSTR("Stereo mode"), CSTR("2-way interleaved, left image on even lines"));
			}
			else
			{
				frame->AddField(ofst + 17, 1, CSTR("Stereo mode"), CSTR("field sequential, left during stereo sync"));
			}
			break;
		case 3:
			if (buff[ofst + 17] & 1)
			{
				frame->AddField(ofst + 17, 1, CSTR("Stereo mode"), CSTR("side-by-side interleaved"));
			}
			else
			{
				frame->AddField(ofst + 17, 1, CSTR("Stereo mode"), CSTR("4-way interleaved"));
			}
			break;
		}
		if (buff[ofst + 17] & 0x10)
		{
			frame->AddUIntName(ofst + 17, 1, CSTR("A/D type"), 1, CSTR("digital sync"));
			if (buff[ofst + 17] & 8)
			{
				frame->AddUIntName(ofst + 17, 1, CSTR("Sync type"), 1, CSTR("composite"));
				frame->AddUIntName(ofst + 17, 1, CSTR("Vertical sync polarity"), (buff[ofst + 17] >> 2) & 1, (buff[ofst + 17] & 4)?CSTR("positive"):CSTR("negative"));
				frame->AddUIntName(ofst + 17, 1, CSTR("Horizontal sync polarity"), (buff[ofst + 17] >> 1) & 1, (buff[ofst + 17] & 2)?CSTR("positive"):CSTR("negative"));
			}
			else
			{
				frame->AddUIntName(ofst + 17, 1, CSTR("Sync type"), 1, CSTR("separate"));
				frame->AddUIntName(ofst + 17, 1, CSTR("Serration"), (buff[ofst + 17] >> 2) & 1, (buff[ofst + 17] & 4)?CSTR("with serrations"):CSTR("without serrations"));
				frame->AddUIntName(ofst + 17, 1, CSTR("Horizontal sync polarity"), (buff[ofst + 17] >> 1) & 1, (buff[ofst + 17] & 2)?CSTR("positive"):CSTR("negative"));
			}
		}
		else
		{
			frame->AddUIntName(ofst + 17, 1, CSTR("A/D type"), 0, CSTR("analog sync"));
			frame->AddUIntName(ofst + 17, 1, CSTR("Sync type"), (buff[ofst + 17] >> 3) & 1, (buff[ofst + 17] & 8)?CSTR("bipolar analog composite"):CSTR("analog composite"));
			frame->AddUIntName(ofst + 17, 1, CSTR("Serration"), (buff[ofst + 17] >> 2) & 1, (buff[ofst + 17] & 4)?CSTR("with serrations"):CSTR("without serrations"));
			frame->AddUIntName(ofst + 17, 1, CSTR("Sync on red and blue lines additionally to green"), (buff[ofst + 17] >> 1) & 1, (buff[ofst + 17] & 2)?CSTR("sync on all three (RGB) video signals"):CSTR("sync on green signal only"));
		}
	}
}

IO::FileAnalyse::EDIDFileAnalyse::EDIDFileAnalyse(IO::IStreamData *fd)
{
	UInt8 buff[128];
	if (fd->GetRealData(0, 128, buff) != 128 || ReadMInt32(buff) != 0xFFFFFF || ReadUInt32(&buff[4]) != 0xFFFFFF || (((UOSInt)buff[126] + 1) << 7) != fd->GetDataSize())
	{
		this->fd = 0;
		this->blockCnt = 0;
		return;
	}
	this->fd = fd->GetPartialData(0, fd->GetDataSize());
	this->blockCnt = buff[126] + 1;
}

IO::FileAnalyse::EDIDFileAnalyse::~EDIDFileAnalyse()
{
	SDEL_CLASS(this->fd);
}

Text::CString IO::FileAnalyse::EDIDFileAnalyse::GetFormatName()
{
	return CSTR("EDID");
}

UOSInt IO::FileAnalyse::EDIDFileAnalyse::GetFrameCount()
{
	return this->blockCnt;
}

Bool IO::FileAnalyse::EDIDFileAnalyse::GetFrameName(UOSInt index, Text::StringBuilderUTF8 *sb)
{
	if (index >= this->blockCnt)
		return false;
	sb->AppendUOSInt(index * 128);
	sb->AppendC(UTF8STRC(": Type="));
	if (index > 0)
	{
		sb->AppendC(UTF8STRC("Extension"));
	}
	else
	{
		sb->AppendC(UTF8STRC("EDID"));
	}
	sb->AppendC(UTF8STRC(", size=128"));
	return true;
}

UOSInt IO::FileAnalyse::EDIDFileAnalyse::GetFrameIndex(UInt64 ofst)
{
	UOSInt blockId = (UOSInt)(ofst >> 7);
	if (blockId >= this->blockCnt)
		return INVALID_INDEX;
	return blockId;
}

IO::FileAnalyse::FrameDetail *IO::FileAnalyse::EDIDFileAnalyse::GetFrameDetail(UOSInt index)
{
	if (index >= this->blockCnt)
		return 0;

	UInt8 buff[128];
	UTF8Char sbuff[128];
	UTF8Char *sptr;
	IO::FileAnalyse::FrameDetail *frame;
	if (this->fd->GetRealData(index << 7, 128, buff) != 128)
		return 0;
	NEW_CLASS(frame, IO::FileAnalyse::FrameDetail(index << 7, 128));
	if (index == 0)
	{
		frame->AddHex64(0, CSTR("Sync Word"), ReadNUInt64(buff));
		sbuff[0] = (UTF8Char)(0x40 + ((buff[8] >> 2) & 0x1f));
		sbuff[1] = (UTF8Char)(0x40 + ((buff[9] >> 5) | ((buff[8] << 3) & 0x1f)));
		sbuff[2] = (UTF8Char)(0x40 + (buff[9] & 0x1f));
		sbuff[3] = 0;
		frame->AddField(8, 2, CSTR("Manufacturer ID"), Text::CString(sbuff, 3));
		frame->AddHex16(10, CSTR("Manufacturer product code"), ReadUInt16(&buff[10]));
		frame->AddUInt(12, 4, CSTR("Serial number"), ReadUInt32(&buff[12]));
		frame->AddUInt(16, 1, CSTR("Week of manufacture"), buff[16]);
		frame->AddUInt(17, 1, CSTR("Year of manufacture"), buff[17] + 1990);
		frame->AddUInt(18, 1, CSTR("EDID version"), buff[18]);
		frame->AddUInt(19, 1, CSTR("EDID revision"), buff[19]);
		if (buff[20] & 0x80)
		{
			frame->AddUIntName(20, 1, CSTR("Video input"), 1, CSTR("Digital input"));
			frame->AddUIntName(20, 1, CSTR("Bit depth"), (buff[20] >> 4) & 7, DigitalBitDepthGetName((buff[20] >> 4) & 7));
			frame->AddUIntName(20, 1, CSTR("Video interface"), buff[20] & 15, DigitalVideoInterfaceGetName(buff[20] & 15));
		}
		else
		{
			frame->AddUIntName(20, 1, CSTR("Video input"), 0, CSTR("Analog input"));
			frame->AddUIntName(20, 1, CSTR("Video white and sync levels, relative to blank"), (buff[20] >> 5) & 3, AnalogLevelGetName((buff[20] >> 5) & 3));
			frame->AddUInt(20, 1, CSTR("Blank-to-black setup (pedestal) expected"), (buff[20] >> 4) & 1);
			frame->AddUInt(20, 1, CSTR("Separate sync supported"), (buff[20] >> 3) & 1);
			frame->AddUInt(20, 1, CSTR("Composite sync (on HSync) supported"), (buff[20] >> 2) & 1);
			frame->AddUInt(20, 1, CSTR("Sync on green supported"), (buff[20] >> 1) & 1);
			frame->AddUInt(20, 1, CSTR("VSync pulse must be serrated when composite or sync-on-green is used"), (buff[20] >> 0) & 1);
		}
		if (buff[21] == 0 && buff[22] == 0)
		{
			frame->AddField(21, 2, CSTR("Screen size and aspect ratio"), CSTR("undefined"));
		}
		else if (buff[21] == 0)
		{
			frame->AddFloat(21, 2, CSTR("Portrait aspect ratio"), 100.0 / (buff[22] + 99));
		}
		else if (buff[22] == 0)
		{
			frame->AddFloat(21, 2, CSTR("Landscape aspect ratio"), (buff[21] + 99) / 100.0);
		}
		else
		{
			frame->AddUInt(21, 1, CSTR("Screen Width (cm)"), buff[21]);
			frame->AddUInt(22, 1, CSTR("Screen Height (cm)"), buff[22]);
		}
		frame->AddFloat(23, 1, CSTR("Display gamma"), (buff[23] + 100) / 100.0);
		frame->AddUInt(24, 1, CSTR("DPMS standby supported"), (buff[24] >> 7) & 1);
		frame->AddUInt(24, 1, CSTR("DPMS suspend supported"), (buff[24] >> 6) & 1);
		frame->AddUInt(24, 1, CSTR("DPMS active-off supported"), (buff[24] >> 5) & 1);
		if (buff[20] & 0x80)
		{
			frame->AddUIntName(24, 1, CSTR("Digital Display type"), (buff[24] >> 3) & 3, DigitalDisplayTypeGetName((buff[24] >> 3) & 3));
		}
		else
		{
			frame->AddUIntName(24, 1, CSTR("Analog Display type"), (buff[24] >> 3) & 3, AnalogDisplayTypeGetName((buff[24] >> 3) & 3));
		}
		frame->AddUInt(24, 1, CSTR("Standard sRGB colour space"), (buff[24] >> 2) & 1);
		frame->AddUInt(24, 1, CSTR("Preferred timing mode specified in descriptor block 1"), (buff[24] >> 1) & 1);
		frame->AddUInt(24, 1, CSTR("Continuous timings with GTF or CVT"), (buff[24] >> 0) & 1);
		frame->AddHex8(25, CSTR("Red and green least-significant bits"), buff[25]);
		frame->AddHex8(26, CSTR("Blue and white least-significant bits"), buff[26]);
		frame->AddFloat(27, 1, CSTR("Red x"), ((buff[27] << 2) | (buff[25] >> 6)) / 1024.0);
		frame->AddFloat(28, 1, CSTR("Red y"), ((buff[28] << 2) | ((buff[25] >> 4) & 3)) / 1024.0);
		frame->AddFloat(29, 1, CSTR("Green x"), ((buff[29] << 2) | ((buff[25] >> 2) & 3)) / 1024.0);
		frame->AddFloat(30, 1, CSTR("Green y"), ((buff[30] << 2) | ((buff[25] >> 0) & 3)) / 1024.0);
		frame->AddFloat(31, 1, CSTR("Blue x"), ((buff[31] << 2) | (buff[26] >> 6)) / 1024.0);
		frame->AddFloat(32, 1, CSTR("Blue y"), ((buff[32] << 2) | ((buff[26] >> 4) & 3)) / 1024.0);
		frame->AddFloat(33, 1, CSTR("White x"), ((buff[33] << 2) | ((buff[26] >> 2) & 3)) / 1024.0);
		frame->AddFloat(34, 1, CSTR("White y"), ((buff[34] << 2) | ((buff[26] >> 0) & 3)) / 1024.0);
		frame->AddUInt(35, 1, CSTR("Support 720x400 @ 70 Hz"), (buff[35] >> 7) & 1);
		frame->AddUInt(35, 1, CSTR("Support 720x400 @ 88 Hz"), (buff[35] >> 6) & 1);
		frame->AddUInt(35, 1, CSTR("Support 640x480 @ 60 Hz"), (buff[35] >> 5) & 1);
		frame->AddUInt(35, 1, CSTR("Support 640x480 @ 67 Hz"), (buff[35] >> 4) & 1);
		frame->AddUInt(35, 1, CSTR("Support 640x480 @ 72 Hz"), (buff[35] >> 3) & 1);
		frame->AddUInt(35, 1, CSTR("Support 640x480 @ 75 Hz"), (buff[35] >> 2) & 1);
		frame->AddUInt(35, 1, CSTR("Support 800x600 @ 56 Hz"), (buff[35] >> 1) & 1);
		frame->AddUInt(35, 1, CSTR("Support 800x600 @ 60 Hz"), (buff[35] >> 0) & 1);
		frame->AddUInt(36, 1, CSTR("Support 800x600 @ 72 Hz"), (buff[36] >> 7) & 1);
		frame->AddUInt(36, 1, CSTR("Support 800x600 @ 75 Hz"), (buff[36] >> 6) & 1);
		frame->AddUInt(36, 1, CSTR("Support 832x624 @ 75 Hz"), (buff[36] >> 5) & 1);
		frame->AddUInt(36, 1, CSTR("Support 1024x768 @ 87 Hz, interlaced"), (buff[36] >> 4) & 1);
		frame->AddUInt(36, 1, CSTR("Support 1024x768 @ 60 Hz"), (buff[36] >> 3) & 1);
		frame->AddUInt(36, 1, CSTR("Support 1024x768 @ 70 Hz"), (buff[36] >> 2) & 1);
		frame->AddUInt(36, 1, CSTR("Support 1024x768 @ 75 Hz"), (buff[36] >> 1) & 1);
		frame->AddUInt(36, 1, CSTR("Support 1280x1024 @ 75 Hz"), (buff[36] >> 0) & 1);
		frame->AddUInt(37, 1, CSTR("Support 1152x870 @ 75 Hz"), (buff[37] >> 7) & 1);
		frame->AddHex8(37, CSTR("Other manufacturer-specific display modes"), buff[37] & 0x7f);
		UOSInt i = 0;
		UOSInt j = 8;
		while (i < j)
		{
			if (buff[38 + i * 2] == 1 && buff[38 + i * 2 + 1] == 1)
			{
				sptr = Text::StrUOSInt(Text::StrConcatC(sbuff, UTF8STRC("Standard timing information ")), i);
				frame->AddField(38 + i * 2, 2, CSTRP(sbuff, sptr), CSTR("Unused"));
			}
			else
			{
				sptr = Text::StrConcatC(Text::StrUOSInt(Text::StrConcatC(sbuff, UTF8STRC("Standard timing information ")), i), UTF8STRC(" X resolution"));
				if (buff[38 + i * 2] == 0)
				{
					frame->AddField(38 + i * 2, 1, CSTRP(sbuff, sptr), CSTR("reserved"));
				}
				else
				{
					frame->AddUInt(38 + i * 2, 1, CSTRP(sbuff, sptr), (31 + (UOSInt)buff[38 + i * 2]) * 8);
				}
				sptr = Text::StrConcatC(Text::StrUOSInt(Text::StrConcatC(sbuff, UTF8STRC("Standard timing information ")), i), UTF8STRC(" Image aspect ratio"));
				frame->AddUIntName(38 + i * 2 + 1, 1, CSTRP(sbuff, sptr), buff[38 + i * 2 + 1] >> 6, ImageAspectRatioGetName(buff[38 + i * 2 + 1] >> 6));
				sptr = Text::StrConcatC(Text::StrUOSInt(Text::StrConcatC(sbuff, UTF8STRC("Standard timing information ")), i), UTF8STRC(" Vertical frequency"));
				frame->AddUInt(38 + i * 2 + 1, 1, CSTRP(sbuff, sptr), 60 + (buff[38 + i * 2 + 1] & 0x3F));
			}
			i++;
		}
		ParseDescriptor(frame, buff, 54);
		ParseDescriptor(frame, buff, 72);
		ParseDescriptor(frame, buff, 90);
		ParseDescriptor(frame, buff, 108);
		frame->AddUInt(126, 1, CSTR("Number of extensions to follow"), buff[126]);
		frame->AddHex8(127, CSTR("Checksum"), buff[127]);
	}
	else
	{
		frame->AddUIntName(0, 1, CSTR("Extension tag"), buff[0], ExtensionTagGetName(buff[0]));
		if (buff[0] == 2)
		{
			frame->AddUInt(1, 1, CSTR("Revision number"), buff[1]);
			frame->AddUInt(2, 1, CSTR("DTD Offset"), buff[2]);
			
		}
		frame->AddHex8(127, CSTR("Checksum"), buff[127]);
	}
	return frame;
}

Bool IO::FileAnalyse::EDIDFileAnalyse::IsError()
{
	return this->fd == 0;
}

Bool IO::FileAnalyse::EDIDFileAnalyse::IsParsing()
{
	return false;
}

Bool IO::FileAnalyse::EDIDFileAnalyse::TrimPadding(Text::CString outputFile)
{
	return false;
}

Text::CString IO::FileAnalyse::EDIDFileAnalyse::DigitalBitDepthGetName(UOSInt val)
{
	switch (val)
	{
	case 0:
		return CSTR("undefined");
	case 1:
		return CSTR("6 bits per color");
	case 2:
		return CSTR("8 bits per color");
	case 3:
		return CSTR("10 bits per color");
	case 4:
		return CSTR("12 bits per color");
	case 5:
		return CSTR("14 bits per color");
	case 6:
		return CSTR("16 bits per color");
	case 7:
		return CSTR("reserved");
	default:
		return CSTR("Unknown");
	}
}

Text::CString IO::FileAnalyse::EDIDFileAnalyse::DigitalVideoInterfaceGetName(UOSInt val)
{
	switch (val)
	{
	case 0:
		return CSTR("undefined");
	case 1:
		return CSTR("DVI");
	case 2:
		return CSTR("HDMIa");
	case 3:
		return CSTR("HDMIb");
	case 4:
		return CSTR("MDDI");
	case 5:
		return CSTR("DisplayPort");
	default:
		return CSTR("Unknown");
	}
}

Text::CString IO::FileAnalyse::EDIDFileAnalyse::DigitalDisplayTypeGetName(UOSInt val)
{
	switch (val)
	{
	case 0:
		return CSTR("RGB 4:4:4");
	case 1:
		return CSTR("RGB 4:4:4 + YCrCb 4:4:4");
	case 2:
		return CSTR("RGB 4:4:4 + YCrCb 4:2:2");
	case 3:
		return CSTR("RGB 4:4:4 + YCrCb 4:4:4 + YCrCb 4:2:2");
	default:
		return CSTR("Unknown");
	}
}

Text::CString IO::FileAnalyse::EDIDFileAnalyse::AnalogLevelGetName(UOSInt val)
{
	switch (val)
	{
	case 0:
		return CSTR("+0.7/-0.3 V");
	case 1:
		return CSTR("+0.714/-0.286 V ");
	case 2:
		return CSTR("+1.0/-0.4 V");
	case 3:
		return CSTR("+0.7/0 V");
	default:
		return CSTR("Unknown");
	}
}

Text::CString IO::FileAnalyse::EDIDFileAnalyse::AnalogDisplayTypeGetName(UOSInt val)
{
	switch (val)
	{
	case 0:
		return CSTR("monochrome or grayscale");
	case 1:
		return CSTR("RGB color");
	case 2:
		return CSTR("non-RGB color");
	case 3:
		return CSTR("undefined");
	default:
		return CSTR("Unknown");
	}
}

Text::CString IO::FileAnalyse::EDIDFileAnalyse::ImageAspectRatioGetName(UOSInt val)
{
	switch (val)
	{
	case 0:
		return CSTR("16:10");
	case 1:
		return CSTR("4:3");
	case 2:
		return CSTR("5:4");
	case 3:
		return CSTR("16:9");
	default:
		return CSTR("Unknown");
	}
}

Text::CString IO::FileAnalyse::EDIDFileAnalyse::DescriptorTypeGetName(UInt8 val)
{
	switch (val)
	{
	case 0x00:
	case 0x01:
	case 0x02:
	case 0x03:
	case 0x04:
	case 0x05:
	case 0x06:
	case 0x07:
	case 0x08:
	case 0x09:
	case 0x0A:
	case 0x0B:
	case 0x0C:
	case 0x0D:
	case 0x0E:
	case 0x0F:
		return CSTR("Manufacturer reserved descriptors");
	case 0x10:
		return CSTR("Dummy identifier");
	case 0xF7:
		return CSTR("Additional standard timing 3");
	case 0xF8:
		return CSTR("CVT 3-Byte Timing Codes");
	case 0xF9:
		return CSTR("Display Color Management");
	case 0xFA:
		return CSTR("Additional standard timing identifiers");
	case 0xFB:
		return CSTR("Additional white point data");
	case 0xFC:
		return CSTR("Display name");
	case 0xFD:
		return CSTR("Display range limits");
	case 0xFE:
		return CSTR("Unspecified text");
	case 0xFF:
		return CSTR("Display serial number");
	default:
		return CSTR("Unknown");
	}
}

Text::CString IO::FileAnalyse::EDIDFileAnalyse::ExtensionTagGetName(UInt8 val)
{
	switch (val)
	{
	case 2:
		return CSTR("CEA EDID");
	default:
		return CSTR("Unknown");
	}
}
