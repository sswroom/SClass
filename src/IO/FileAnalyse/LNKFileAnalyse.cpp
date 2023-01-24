#include "Stdafx.h"
#include "Data/ByteTool.h"
#include "Data/UUID.h"
#include "IO/FileAnalyse/LNKFileAnalyse.h"
#include "Sync/Thread.h"
#include "Text/MyStringW.h"
#include "Text/StringBuilderUTF8.h"

UInt32 __stdcall IO::FileAnalyse::LNKFileAnalyse::ParseThread(void* userObj)
{
	IO::FileAnalyse::LNKFileAnalyse* me = (IO::FileAnalyse::LNKFileAnalyse*)userObj;
	UInt64 ofst;
	UInt8 tagHdr[24];
	UInt32 linkFlags;
	UInt64 fileSize = me->fd->GetDataSize();
	IO::FileAnalyse::LNKFileAnalyse::TagInfo* tag;
	me->threadRunning = true;
	me->threadStarted = true;

	tag = MemAlloc(IO::FileAnalyse::LNKFileAnalyse::TagInfo, 1);
	tag->ofst = 0;
	tag->size = 0x4C;
	tag->tagType = TagType::ShellLinkHeader;
	me->tags.Add(tag);
	
	me->fd->GetRealData(0, 24, tagHdr);
	linkFlags = ReadUInt32(&tagHdr[20]);
	ofst = 0x4C;
	if (linkFlags & 1)
	{
		me->fd->GetRealData(ofst, 24, tagHdr);
		tag = MemAlloc(IO::FileAnalyse::LNKFileAnalyse::TagInfo, 1);
		tag->ofst = ofst;
		tag->size = (UOSInt)ReadUInt16(&tagHdr[0]) + 2;
		tag->tagType = TagType::LinkTargetIDList;
		me->tags.Add(tag);
		ofst += tag->size;
	}
	if (linkFlags & 2)
	{
		me->fd->GetRealData(ofst, 24, tagHdr);
		tag = MemAlloc(IO::FileAnalyse::LNKFileAnalyse::TagInfo, 1);
		tag->ofst = ofst;
		tag->size = ReadUInt32(&tagHdr[0]);
		tag->tagType = TagType::LinkInfo;
		me->tags.Add(tag);
		ofst += tag->size;		
	}
	if (linkFlags & 4)
	{
		me->fd->GetRealData(ofst, 24, tagHdr);
		tag = MemAlloc(IO::FileAnalyse::LNKFileAnalyse::TagInfo, 1);
		tag->ofst = ofst;
		tag->size = (UOSInt)ReadUInt16(&tagHdr[0]) * 2 + 2;
		tag->tagType = TagType::NameString;
		me->tags.Add(tag);
		ofst += tag->size;
	}
	if (linkFlags & 8)
	{
		me->fd->GetRealData(ofst, 24, tagHdr);
		tag = MemAlloc(IO::FileAnalyse::LNKFileAnalyse::TagInfo, 1);
		tag->ofst = ofst;
		tag->size = (UOSInt)ReadUInt16(&tagHdr[0]) * 2 + 2;
		tag->tagType = TagType::RelativePath;
		me->tags.Add(tag);
		ofst += tag->size;
	}
	if (linkFlags & 16)
	{
		me->fd->GetRealData(ofst, 24, tagHdr);
		tag = MemAlloc(IO::FileAnalyse::LNKFileAnalyse::TagInfo, 1);
		tag->ofst = ofst;
		tag->size = (UOSInt)ReadUInt16(&tagHdr[0]) * 2 + 2;
		tag->tagType = TagType::WorkingDir;
		me->tags.Add(tag);
		ofst += tag->size;
	}
	if (linkFlags & 32)
	{
		me->fd->GetRealData(ofst, 24, tagHdr);
		tag = MemAlloc(IO::FileAnalyse::LNKFileAnalyse::TagInfo, 1);
		tag->ofst = ofst;
		tag->size = (UOSInt)ReadUInt16(&tagHdr[0]) * 2 + 2;
		tag->tagType = TagType::CommandLineArguments;
		me->tags.Add(tag);
		ofst += tag->size;
	}
	if (linkFlags & 64)
	{
		me->fd->GetRealData(ofst, 24, tagHdr);
		tag = MemAlloc(IO::FileAnalyse::LNKFileAnalyse::TagInfo, 1);
		tag->ofst = ofst;
		tag->size = (UOSInt)ReadUInt16(&tagHdr[0]) * 2 + 2;
		tag->tagType = TagType::IconLocation;
		me->tags.Add(tag);
		ofst += tag->size;
	}
	while (ofst < fileSize)
	{
		me->fd->GetRealData(ofst, 24, tagHdr);
		UInt32 size = ReadUInt32(&tagHdr[0]);
		if (size < 4)
		{
			tag = MemAlloc(IO::FileAnalyse::LNKFileAnalyse::TagInfo, 1);
			tag->ofst = ofst;
			tag->size = 4;
			tag->tagType = TagType::ExtraData;
			me->tags.Add(tag);
			ofst += 4;
			break;
		}
		else
		{
			tag = MemAlloc(IO::FileAnalyse::LNKFileAnalyse::TagInfo, 1);
			tag->ofst = ofst;
			tag->size = size;
			tag->tagType = TagType::ExtraData;
			me->tags.Add(tag);
			ofst += size;
		}
	}

	me->threadRunning = false;
	return 0;
}

IO::FileAnalyse::LNKFileAnalyse::LNKFileAnalyse(IO::IStreamData* fd)
{
	UInt8 buff[40];
	this->fd = 0;
	this->threadRunning = false;
	this->pauseParsing = false;
	this->threadToStop = false;
	this->threadStarted = false;
	fd->GetRealData(0, 40, buff);
	if (ReadUInt32(&buff[0]) != 0x4C || ReadUInt32(&buff[4]) != 0x00021401 || ReadUInt32(&buff[8]) != 0 || ReadUInt32(&buff[12]) != 0xC0 || ReadUInt32(&buff[16]) != 0x46000000)
	{
		return;
	}
	this->fd = fd->GetPartialData(0, fd->GetDataSize());

	Sync::Thread::Create(ParseThread, this);
	while (!this->threadStarted)
	{
		Sync::Thread::Sleep(10);
	}
}

IO::FileAnalyse::LNKFileAnalyse::~LNKFileAnalyse()
{
	if (this->threadRunning)
	{
		this->threadToStop = true;
		while (this->threadRunning)
		{
			Sync::Thread::Sleep(10);
		}
	}
	SDEL_CLASS(this->fd);
	LIST_FREE_FUNC(&this->tags, MemFree);
}

Text::CString IO::FileAnalyse::LNKFileAnalyse::GetFormatName()
{
	return CSTR("SLNK");
}

UOSInt IO::FileAnalyse::LNKFileAnalyse::GetFrameCount()
{
	return this->tags.GetCount();
}

Bool IO::FileAnalyse::LNKFileAnalyse::GetFrameName(UOSInt index, Text::StringBuilderUTF8* sb)
{
	IO::FileAnalyse::LNKFileAnalyse::TagInfo* tag = this->tags.GetItem(index);
	if (tag == 0)
		return false;
	sb->AppendU64(tag->ofst);
	sb->AppendC(UTF8STRC(": Type="));
	sb->Append(TagTypeGetName(tag->tagType));
	sb->AppendC(UTF8STRC(", size="));
	sb->AppendUOSInt(tag->size);
	return true;
}

UOSInt IO::FileAnalyse::LNKFileAnalyse::GetFrameIndex(UInt64 ofst)
{
	OSInt i = 0;
	OSInt j = (OSInt)this->tags.GetCount() - 1;
	OSInt k;
	TagInfo* pack;
	while (i <= j)
	{
		k = (i + j) >> 1;
		pack = this->tags.GetItem((UOSInt)k);
		if (ofst < pack->ofst)
		{
			j = k - 1;
		}
		else if (ofst >= pack->ofst + pack->size)
		{
			i = k + 1;
		}
		else
		{
			return (UOSInt)k;
		}
	}
	return INVALID_INDEX;
}

IO::FileAnalyse::FrameDetail* IO::FileAnalyse::LNKFileAnalyse::GetFrameDetail(UOSInt index)
{
	IO::FileAnalyse::FrameDetail* frame;
	UTF8Char sbuff[1024];
	UTF8Char* sptr;
	UInt8* tagData;
	IO::FileAnalyse::LNKFileAnalyse::TagInfo* tag = this->tags.GetItem(index);
	if (tag == 0)
		return 0;

	NEW_CLASS(frame, IO::FileAnalyse::FrameDetail(tag->ofst, (UInt32)tag->size));
	sptr = TagTypeGetName(tag->tagType).ConcatTo(Text::StrConcatC(sbuff, UTF8STRC("Type=")));
	frame->AddHeader(CSTRP(sbuff, sptr));

	tagData = MemAlloc(UInt8, tag->size);
	this->fd->GetRealData(tag->ofst, tag->size, tagData);
	switch (tag->tagType)
	{
	case TagType::ShellLinkHeader:
	{
		frame->AddUInt(0, 4, CSTR("HeaderSize"), ReadUInt32(&tagData[0]));
		Data::UUID uuid;
		uuid.SetValue(&tagData[4]);
		sptr = uuid.ToString(sbuff);
		frame->AddField(4, 16, CSTR("Number of Valid Rows"), CSTRP(sbuff, sptr));
		UInt32 linkFlags = ReadUInt32(&tagData[20]);
		frame->AddHex32(20, CSTR("LinkFlags"), linkFlags);
		frame->AddUInt(20, 4, CSTR("HasLinkTargetIDList"), (linkFlags >> 0) & 1);
		frame->AddUInt(20, 4, CSTR("HasLinkInfo"), (linkFlags >> 1) & 1);
		frame->AddUInt(20, 4, CSTR("HasName"), (linkFlags >> 2) & 1);
		frame->AddUInt(20, 4, CSTR("HasRelativePath"), (linkFlags >> 3) & 1);
		frame->AddUInt(20, 4, CSTR("HasWorkingDir"), (linkFlags >> 4) & 1);
		frame->AddUInt(20, 4, CSTR("HasArguments"), (linkFlags >> 5) & 1);
		frame->AddUInt(20, 4, CSTR("HasIconLocation"), (linkFlags >> 6) & 1);
		frame->AddUInt(20, 4, CSTR("IsUnicode"), (linkFlags >> 7) & 1);
		frame->AddUInt(20, 4, CSTR("ForceNoLinkInfo"), (linkFlags >> 8) & 1);
		frame->AddUInt(20, 4, CSTR("HasExpString"), (linkFlags >> 9) & 1);
		frame->AddUInt(20, 4, CSTR("RunInSeparateProcess"), (linkFlags >> 10) & 1);
		frame->AddUInt(20, 4, CSTR("Unused1"), (linkFlags >> 11) & 1);
		frame->AddUInt(20, 4, CSTR("HasDarwinID"), (linkFlags >> 12) & 1);
		frame->AddUInt(20, 4, CSTR("RunAsUser"), (linkFlags >> 13) & 1);
		frame->AddUInt(20, 4, CSTR("HasExpIcon"), (linkFlags >> 14) & 1);
		frame->AddUInt(20, 4, CSTR("NoPidlAlias"), (linkFlags >> 15) & 1);
		frame->AddUInt(20, 4, CSTR("Unused2"), (linkFlags >> 16) & 1);
		frame->AddUInt(20, 4, CSTR("RunWithShimLayer"), (linkFlags >> 17) & 1);
		frame->AddUInt(20, 4, CSTR("ForceNoLinkTrack"), (linkFlags >> 18) & 1);
		frame->AddUInt(20, 4, CSTR("EnableTargetMetadata"), (linkFlags >> 19) & 1);
		frame->AddUInt(20, 4, CSTR("DisableLinkPathTracking"), (linkFlags >> 20) & 1);
		frame->AddUInt(20, 4, CSTR("DisableKnownFolderTracking"), (linkFlags >> 21) & 1);
		frame->AddUInt(20, 4, CSTR("DisableKnownFolderAlias"), (linkFlags >> 22) & 1);
		frame->AddUInt(20, 4, CSTR("AllowLinkToLink"), (linkFlags >> 23) & 1);
		frame->AddUInt(20, 4, CSTR("UnaliasOnSave"), (linkFlags >> 24) & 1);
		frame->AddUInt(20, 4, CSTR("PreferEnvironmentPath"), (linkFlags >> 25) & 1);
		frame->AddUInt(20, 4, CSTR("KeepLocalIDListForUNCTarget"), (linkFlags >> 26) & 1);
		UInt32 fileAttributes = ReadUInt32(&tagData[24]);
		frame->AddHex32(24, CSTR("FileAttributes "), fileAttributes);
		frame->AddUInt(24, 4, CSTR("FILE_ATTRIBUTE_READONLY"), (fileAttributes >> 0) & 1);
		frame->AddUInt(24, 4, CSTR("FILE_ATTRIBUTE_HIDDEN"), (fileAttributes >> 1) & 1);
		frame->AddUInt(24, 4, CSTR("FILE_ATTRIBUTE_SYSTEM"), (fileAttributes >> 2) & 1);
		frame->AddUInt(24, 4, CSTR("Reserved1"), (fileAttributes >> 3) & 1);
		frame->AddUInt(24, 4, CSTR("FILE_ATTRIBUTE_DIRECTORY"), (fileAttributes >> 4) & 1);
		frame->AddUInt(24, 4, CSTR("FILE_ATTRIBUTE_ARCHIVE"), (fileAttributes >> 5) & 1);
		frame->AddUInt(24, 4, CSTR("Reserved2"), (fileAttributes >> 6) & 1);
		frame->AddUInt(24, 4, CSTR("FILE_ATTRIBUTE_NORMAL"), (fileAttributes >> 7) & 1);
		frame->AddUInt(24, 4, CSTR("FILE_ATTRIBUTE_TEMPORARY"), (fileAttributes >> 8) & 1);
		frame->AddUInt(24, 4, CSTR("FILE_ATTRIBUTE_SPARSE_FILE"), (fileAttributes >> 9) & 1);
		frame->AddUInt(24, 4, CSTR("FILE_ATTRIBUTE_REPARSE_POINT"), (fileAttributes >> 10) & 1);
		frame->AddUInt(24, 4, CSTR("FILE_ATTRIBUTE_COMPRESSED"), (fileAttributes >> 11) & 1);
		frame->AddUInt(24, 4, CSTR("FILE_ATTRIBUTE_OFFLINE"), (fileAttributes >> 12) & 1);
		frame->AddUInt(24, 4, CSTR("FILE_ATTRIBUTE_NOT_CONTENT_INDEXED"), (fileAttributes >> 13) & 1);
		frame->AddUInt(24, 4, CSTR("FILE_ATTRIBUTE_ENCRYPTED"), (fileAttributes >> 14) & 1);
		sptr = Data::Timestamp::FromFILETIME(&tagData[28], Data::DateTimeUtil::GetLocalTzQhr()).ToString(sbuff);
		frame->AddField(28, 8, CSTR("CreationTime"), CSTRP(sbuff, sptr));
		sptr = Data::Timestamp::FromFILETIME(&tagData[36], Data::DateTimeUtil::GetLocalTzQhr()).ToString(sbuff);
		frame->AddField(36, 8, CSTR("AccessTime"), CSTRP(sbuff, sptr));
		sptr = Data::Timestamp::FromFILETIME(&tagData[44], Data::DateTimeUtil::GetLocalTzQhr()).ToString(sbuff);
		frame->AddField(44, 8, CSTR("WriteTime"), CSTRP(sbuff, sptr));
		frame->AddUInt(52, 4, CSTR("FileSize"), ReadUInt32(&tagData[52]));
		frame->AddInt(56, 4, CSTR("IconIndex"), ReadInt32(&tagData[56]));
		frame->AddUIntName(60, 4, CSTR("ShowCommand"), ReadUInt32(&tagData[60]), ShowWindowGetName(ReadUInt32(&tagData[60])));
		frame->AddHex16(64, CSTR("HotKeyFlags"), ReadUInt16(&tagData[64]));
		frame->AddUInt(66, 2, CSTR("Reserved1"), ReadUInt16(&tagData[66]));
		frame->AddUInt(68, 4, CSTR("Reserved2"), ReadUInt32(&tagData[68]));
		frame->AddUInt(72, 4, CSTR("Reserved3"), ReadUInt32(&tagData[72]));
		break;
	}
	case TagType::LinkTargetIDList:
	{
		UInt16 val;
		UOSInt ofst = 2;
		frame->AddUInt(0, 2, CSTR("IDListSize"), ReadUInt16(&tagData[0]));
		while (ofst < tag->size)
		{
			val = ReadUInt16(&tagData[ofst]);
			if (val == 0)
			{
				frame->AddUInt(ofst, 2, CSTR("TerminalID"), val);
				break;
			}
			frame->AddUInt(ofst, 2, CSTR("ItemIDSize"), val);
			ofst += val;
		}
		break;
	}
	case TagType::LinkInfo:
	{
		frame->AddUInt(0, 4, CSTR("LinkInfoSize"), ReadUInt32(&tagData[0]));
		UOSInt linkInfoHeaderSize = ReadUInt32(&tagData[4]);
		frame->AddUInt(4, 4, CSTR("LinkInfoHeaderSize"), linkInfoHeaderSize);
		UInt32 linkInfoFlags = ReadUInt32(&tagData[8]);
		frame->AddHex32(8, CSTR("LinkInfoFlags"), linkInfoFlags);
		frame->AddUInt(8, 4, CSTR("VolumeIDAndLocalBasePath"), (linkInfoFlags >> 0) & 1);
		frame->AddUInt(8, 4, CSTR("CommonNetworkRelativeLinkAndPathSuffix"), (linkInfoFlags >> 1) & 1);
		UInt32 volumeIDOffset = ReadUInt32(&tagData[12]);
		frame->AddUInt(12, 4, CSTR("VolumeIDOffset"), volumeIDOffset);
		UInt32 localBasePathOffset = ReadUInt32(&tagData[16]);
		frame->AddUInt(16, 4, CSTR("LocalBasePathOffset"), localBasePathOffset);
		UInt32 commonNetworkRelativeLinkOffset = ReadUInt32(&tagData[20]);
		frame->AddUInt(20, 4, CSTR("CommonNetworkRelativeLinkOffset"), commonNetworkRelativeLinkOffset);
		UInt32 commonPathSuffixOffset = ReadUInt32(&tagData[24]);
		frame->AddUInt(24, 4, CSTR("CommonPathSuffixOffset"), commonPathSuffixOffset);
		UInt32 localBasePathOffsetUnicode = 0;
		UInt32 commonPathSuffixOffsetUnicode = 0;
		if (linkInfoHeaderSize >= 36)
		{
			localBasePathOffsetUnicode = ReadUInt32(&tagData[28]);
			frame->AddUInt(28, 4, CSTR("LocalBasePathOffsetUnicode"), localBasePathOffsetUnicode);
			commonPathSuffixOffsetUnicode = ReadUInt32(&tagData[32]);
			frame->AddUInt(32, 4, CSTR("CommonPathSuffixOffsetUnicode"), commonPathSuffixOffsetUnicode);
		}
		if ((linkInfoFlags & 1) && volumeIDOffset != 0)
		{
			frame->AddUInt(volumeIDOffset, 4, CSTR("VolumeID.VolumeIDSize"), ReadUInt32(&tagData[volumeIDOffset]));
			frame->AddUIntName(volumeIDOffset + 4, 4, CSTR("VolumeID.DriveType"), ReadUInt32(&tagData[volumeIDOffset + 4]), DriveTypeGetName(ReadUInt32(&tagData[volumeIDOffset + 4])));
			frame->AddHex32(volumeIDOffset + 8, CSTR("VolumeID.DriveSerialNumber"), ReadUInt32(&tagData[volumeIDOffset + 8]));
			UInt32 volumeLabelOffset = ReadUInt32(&tagData[volumeIDOffset + 12]);
			frame->AddUInt(volumeIDOffset + 12, 4, CSTR("VolumeID.VolumeLabelOffset"), volumeLabelOffset);
			if (volumeLabelOffset == 0x14)
			{
				volumeLabelOffset = ReadUInt32(&tagData[volumeIDOffset + 16]);
				frame->AddUInt(volumeIDOffset + 16, 4, CSTR("VolumeID.VolumeLabelOffsetUnicode"), volumeLabelOffset);
				UOSInt strLen = Text::StrCharCnt((const UTF16Char*)&tagData[volumeIDOffset + volumeLabelOffset]);
				sptr = Text::StrUTF16_UTF8(sbuff, (const UTF16Char*)&tagData[volumeIDOffset + volumeLabelOffset]);
				frame->AddField(volumeIDOffset + volumeLabelOffset, strLen * 2 + 2, CSTR("VolumeID.VolumeLabel"), CSTRP(sbuff, sptr));
			}
			else
			{
				frame->AddStrZ(volumeIDOffset + volumeLabelOffset, CSTR("VolumeID.VolumeLabel"), &tagData[volumeIDOffset + volumeLabelOffset]);
			}
		}
		if ((linkInfoFlags & 1) && localBasePathOffset != 0)
		{
			frame->AddStrZ(localBasePathOffset, CSTR("LocalBasePath"), &tagData[localBasePathOffset]);
		}
		if ((linkInfoFlags & 2) && commonNetworkRelativeLinkOffset != 0)
		{
			//////////////////////////////////
		}
		if (commonPathSuffixOffset != 0)
		{
			frame->AddStrZ(commonPathSuffixOffset, CSTR("CommonPathSuffix"), &tagData[commonPathSuffixOffset]);
		}
		break;
	}
	case TagType::NameString:
	{
		frame->AddUInt(0, 2, CSTR("NameString.CountCharacters"), ReadUInt16(&tagData[0]));
		sptr = Text::StrUTF16_UTF8C(sbuff, (const UTF16Char*)&tagData[2], (tag->size - 2) >> 1);
		*sptr = 0;
		frame->AddField(2, tag->size - 2, CSTR("NameString.String"), CSTRP(sbuff, sptr));
		break;
	}
	case TagType::RelativePath:
	{
		frame->AddUInt(0, 2, CSTR("RelativePath.CountCharacters"), ReadUInt16(&tagData[0]));
		sptr = Text::StrUTF16_UTF8C(sbuff, (const UTF16Char*)&tagData[2], (tag->size - 2) >> 1);
		*sptr = 0;
		frame->AddField(2, tag->size - 2, CSTR("RelativePath.String"), CSTRP(sbuff, sptr));
		break;
	}
	case TagType::WorkingDir:
	{
		frame->AddUInt(0, 2, CSTR("WorkingDir.CountCharacters"), ReadUInt16(&tagData[0]));
		sptr = Text::StrUTF16_UTF8C(sbuff, (const UTF16Char*)&tagData[2], (tag->size - 2) >> 1);
		*sptr = 0;
		frame->AddField(2, tag->size - 2, CSTR("WorkingDir.String"), CSTRP(sbuff, sptr));
		break;
	}
	case TagType::CommandLineArguments:
	{
		frame->AddUInt(0, 2, CSTR("CommandLineArguments.CountCharacters"), ReadUInt16(&tagData[0]));
		sptr = Text::StrUTF16_UTF8C(sbuff, (const UTF16Char*)&tagData[2], (tag->size - 2) >> 1);
		*sptr = 0;
		frame->AddField(2, tag->size - 2, CSTR("CommandLineArguments.String"), CSTRP(sbuff, sptr));
		break;
	}
	case TagType::IconLocation:
	{
		frame->AddUInt(0, 2, CSTR("IconLocation.CountCharacters"), ReadUInt16(&tagData[0]));
		sptr = Text::StrUTF16_UTF8C(sbuff, (const UTF16Char*)&tagData[2], (tag->size - 2) >> 1);
		*sptr = 0;
		frame->AddField(2, tag->size - 2, CSTR("IconLocation.String"), CSTRP(sbuff, sptr));
		break;
	}
	case TagType::ExtraData:
	{
		UInt32 blockSize = ReadUInt32(&tagData[0]);
		frame->AddUInt(0, 4, CSTR("BlockSize"), blockSize);
		if (blockSize >= 4)
		{
			UInt32 blockSignature = ReadUInt32(&tagData[4]);
			switch (blockSignature)
			{
			case 0xA0000002:
				frame->AddHex32Name(4, CSTR("BlockSignature"), blockSignature, CSTR("ConsoleDataBlock"));
				if (blockSize == 0x000000CC)
				{
					UInt16 fillAttributes = ReadUInt16(&tagData[8]);
					frame->AddHex16(8, CSTR("FillAttributes"), fillAttributes);
					frame->AddUInt(8, 2, CSTR("FOREGROUND_BLUE"), (fillAttributes >> 0) & 1);
					frame->AddUInt(8, 2, CSTR("FOREGROUND_GREEN"), (fillAttributes >> 1) & 1);
					frame->AddUInt(8, 2, CSTR("FOREGROUND_RED"), (fillAttributes >> 2) & 1);
					frame->AddUInt(8, 2, CSTR("FOREGROUND_INTENSITY"), (fillAttributes >> 3) & 1);
					frame->AddUInt(8, 2, CSTR("BACKGROUND_BLUE"), (fillAttributes >> 4) & 1);
					frame->AddUInt(8, 2, CSTR("BACKGROUND_GREEN"), (fillAttributes >> 5) & 1);
					frame->AddUInt(8, 2, CSTR("BACKGROUND_RED"), (fillAttributes >> 6) & 1);
					frame->AddUInt(8, 2, CSTR("BACKGROUND_INTENSITY"), (fillAttributes >> 7) & 1);
					fillAttributes = ReadUInt16(&tagData[10]);
					frame->AddHex16(10, CSTR("PopupFillAttributes"), fillAttributes);
					frame->AddUInt(10, 2, CSTR("FOREGROUND_BLUE"), (fillAttributes >> 0) & 1);
					frame->AddUInt(10, 2, CSTR("FOREGROUND_GREEN"), (fillAttributes >> 1) & 1);
					frame->AddUInt(10, 2, CSTR("FOREGROUND_RED"), (fillAttributes >> 2) & 1);
					frame->AddUInt(10, 2, CSTR("FOREGROUND_INTENSITY"), (fillAttributes >> 3) & 1);
					frame->AddUInt(10, 2, CSTR("BACKGROUND_BLUE"), (fillAttributes >> 4) & 1);
					frame->AddUInt(10, 2, CSTR("BACKGROUND_GREEN"), (fillAttributes >> 5) & 1);
					frame->AddUInt(10, 2, CSTR("BACKGROUND_RED"), (fillAttributes >> 6) & 1);
					frame->AddUInt(10, 2, CSTR("BACKGROUND_INTENSITY"), (fillAttributes >> 7) & 1);
					frame->AddInt(12, 2, CSTR("ScreenBufferSizeX"), ReadInt16(&tagData[12]));
					frame->AddInt(14, 2, CSTR("ScreenBufferSizeY"), ReadInt16(&tagData[14]));
					frame->AddInt(16, 2, CSTR("WindowSizeX"), ReadInt16(&tagData[16]));
					frame->AddInt(18, 2, CSTR("WindowSizeY"), ReadInt16(&tagData[18]));
					frame->AddInt(20, 2, CSTR("WindowOriginX"), ReadInt16(&tagData[20]));
					frame->AddInt(22, 2, CSTR("WindowOriginY"), ReadInt16(&tagData[22]));
					frame->AddUInt(24, 4, CSTR("Unused1"), ReadUInt32(&tagData[24]));
					frame->AddUInt(28, 4, CSTR("Unused2"), ReadUInt32(&tagData[28]));
					frame->AddUInt(32, 2, CSTR("FontSize.width"), ReadUInt16(&tagData[32]));
					frame->AddUInt(34, 2, CSTR("FontSize.height"), ReadUInt16(&tagData[34]));
					UInt32 fontFamily = ReadUInt32(&tagData[36]);
					frame->AddHex32(36, CSTR("FontFamily"), fontFamily);
					frame->AddHex32Name(36, CSTR("FontFamily.family"), fontFamily & 0x00F0, FontFamilyGetName(fontFamily));
					frame->AddUInt(36, 4, CSTR("FontFamily.TMPF_FIXED_PITCH"), (fontFamily >> 0) & 1);
					frame->AddUInt(36, 4, CSTR("FontFamily.TMPF_VECTOR"), (fontFamily >> 1) & 1);
					frame->AddUInt(36, 4, CSTR("FontFamily.TMPF_TRUETYPE"), (fontFamily >> 2) & 1);
					frame->AddUInt(36, 4, CSTR("FontFamily.TMPF_DEVICE"), (fontFamily >> 3) & 1);
					frame->AddUInt(40, 4, CSTR("FontWeight"), ReadUInt32(&tagData[40]));
					sptr = Text::StrUTF16_UTF8C(sbuff, (const UTF16Char*)&tagData[44], 32);
					*sptr = 0;
					sptr = sbuff + Text::StrCharCnt(sbuff);
					frame->AddField(44, 64, CSTR("FaceName"), CSTRP(sbuff, sptr));
					frame->AddUInt(108, 4, CSTR("CursorSize"), ReadUInt32(&tagData[108]));
					frame->AddUInt(112, 4, CSTR("FullScreen"), ReadUInt32(&tagData[112]));
					frame->AddUInt(116, 4, CSTR("QuickEdit"), ReadUInt32(&tagData[116]));
					frame->AddUInt(120, 4, CSTR("InsertMode"), ReadUInt32(&tagData[120]));
					frame->AddUInt(124, 4, CSTR("AutoPosition"), ReadUInt32(&tagData[124]));
					frame->AddUInt(128, 4, CSTR("HistoryBufferSize"), ReadUInt32(&tagData[128]));
					frame->AddUInt(132, 4, CSTR("NumberOfHistoryBuffers"), ReadUInt32(&tagData[132]));
					frame->AddUInt(136, 4, CSTR("HistoryNoDup"), ReadUInt32(&tagData[136]));
					UOSInt i = 0;
					UOSInt j = 32;
					while (i < j)
					{
						sptr = Text::StrConcatC(sbuff, UTF8STRC("ColorTable["));
						sptr = Text::StrUOSInt(sptr, i);
						*sptr++ = ']';
						*sptr = 0;
						frame->AddHex32(140 + i * 4, CSTRP(sbuff, sptr), ReadUInt32(&tagData[140 + i * 4]));
						i++;
					}
				}
				break;
			case 0xA0000004:
				frame->AddHex32Name(4, CSTR("BlockSignature"), blockSignature, CSTR("ConsoleFEDataBlock"));
				if (blockSize == 0x0000000C)
				{
					frame->AddUInt(8, 4, CSTR("CodePage"), ReadUInt32(&tagData[8]));
				}
				break;
			case 0xA0000006:
				frame->AddHex32Name(4, CSTR("BlockSignature"), blockSignature, CSTR("DarwinDataBlock"));
				if (blockSize == 0x00000314)
				{
					frame->AddField(8, 260, CSTR("DarwinDataAnsi"), Text::CString::FromPtr(&tagData[8]));
					sptr = Text::StrUTF16_UTF8(sbuff, (const UTF16Char*)&tagData[268]);
					frame->AddField(268, 520, CSTR("DarwinDataUnicode"), CSTRP(sbuff, sptr));
				}
				break;
			case 0xA0000001:
				frame->AddHex32Name(4, CSTR("BlockSignature"), blockSignature, CSTR("EnvironmentVariableDataBlock"));
				if (blockSize == 0x00000314)
				{
					frame->AddField(8, 260, CSTR("TargetAnsi"), Text::CString::FromPtr(&tagData[8]));
					sptr = Text::StrUTF16_UTF8(sbuff, (const UTF16Char*)&tagData[268]);
					frame->AddField(268, 520, CSTR("TargetUnicode"), CSTRP(sbuff, sptr));
				}
				break;
			case 0xA0000007:
				frame->AddHex32Name(4, CSTR("BlockSignature"), blockSignature, CSTR("IconEnvironmentDataBlock"));
				if (blockSize == 0x00000314)
				{
					frame->AddField(8, 260, CSTR("TargetAnsi"), Text::CString::FromPtr(&tagData[8]));
					sptr = Text::StrUTF16_UTF8(sbuff, (const UTF16Char*)&tagData[268]);
					frame->AddField(268, 520, CSTR("TargetUnicode"), CSTRP(sbuff, sptr));
				}
				break;
			case 0xA000000B:
				frame->AddHex32Name(4, CSTR("BlockSignature"), blockSignature, CSTR("KnownFolderDataBlock"));
				if (blockSize == 0x0000001C)
				{
					Data::UUID uuid;
					uuid.SetValue(&tagData[4]);
					sptr = uuid.ToString(sbuff);
					frame->AddField(8, 16, CSTR("KnownFolderID"), CSTRP(sbuff, sptr));
					frame->AddUInt(24, 4, CSTR("Offset"), ReadUInt32(&tagData[24]));
				}
				break;
			case 0xA0000009:
				frame->AddHex32Name(4, CSTR("BlockSignature"), blockSignature, CSTR("PropertyStoreDataBlock"));
				if (blockSize >= 0x0000000C)
				{
					//////////////////////////////////
				}
				break;
			case 0xA0000008:
				frame->AddHex32Name(4, CSTR("BlockSignature"), blockSignature, CSTR("ShimDataBlock"));
				if (blockSize >= 0x00000088)
				{
					//////////////////////////////////
				}
				break;
			case 0xA0000005:
				frame->AddHex32Name(4, CSTR("BlockSignature"), blockSignature, CSTR("SpecialFolderDataBlock"));
				if (blockSize >= 0x00000010)
				{
					frame->AddUInt(8, 4, CSTR("SpecialFolderID"), ReadUInt32(&tagData[8]));
					frame->AddUInt(12, 4, CSTR("Offset"), ReadUInt32(&tagData[12]));
				}
				break;
			case 0xA0000003:
				frame->AddHex32Name(4, CSTR("BlockSignature"), blockSignature, CSTR("TrackerDataBlock"));
				if (blockSize >= 0x00000060)
				{
					Data::UUID uuid;
					frame->AddUInt(8, 4, CSTR("Length"), ReadUInt32(&tagData[8]));
					frame->AddUInt(12, 4, CSTR("Version"), ReadUInt32(&tagData[12]));
					frame->AddField(16, 16, CSTR("MachineID"), Text::CString::FromPtr(&tagData[16]));
					uuid.SetValue(&tagData[32]);
					sptr = uuid.ToString(sbuff);
					frame->AddField(32, 16, CSTR("Droid[0]"), CSTRP(sbuff, sptr));
					uuid.SetValue(&tagData[48]);
					sptr = uuid.ToString(sbuff);
					frame->AddField(48, 16, CSTR("Droid[1]"), CSTRP(sbuff, sptr));
					uuid.SetValue(&tagData[64]);
					sptr = uuid.ToString(sbuff);
					frame->AddField(64, 16, CSTR("DroidBirth[0]"), CSTRP(sbuff, sptr));
					uuid.SetValue(&tagData[80]);
					sptr = uuid.ToString(sbuff);
					frame->AddField(80, 16, CSTR("DroidBirth[1]"), CSTRP(sbuff, sptr));
				}
				break;
			default:
				frame->AddHex32Name(4, CSTR("BlockSignature"), blockSignature, CSTR("Unknown"));
				break;
			}
		}
		break;
	}
	}
	MemFree(tagData);
	return frame;
}

Bool IO::FileAnalyse::LNKFileAnalyse::IsError()
{
	return this->fd == 0;
}

Bool IO::FileAnalyse::LNKFileAnalyse::IsParsing()
{
	return this->threadRunning;
}

Bool IO::FileAnalyse::LNKFileAnalyse::TrimPadding(Text::CString outputFile)
{
	return false;
}

Text::CString IO::FileAnalyse::LNKFileAnalyse::TagTypeGetName(TagType tagType)
{
	switch (tagType)
	{
	case TagType::ShellLinkHeader:
		return CSTR("ShellLinkHeader");
	case TagType::LinkTargetIDList:
		return CSTR("LinkTargetIDList");
	case TagType::LinkInfo:
		return CSTR("LinkInfo");
	case TagType::NameString:
		return CSTR("NameString");
	case TagType::RelativePath:
		return CSTR("RelativePath");
	case TagType::WorkingDir:
		return CSTR("WorkingDir");
	case TagType::CommandLineArguments:
		return CSTR("CommandLineArguments");
	case TagType::IconLocation:
		return CSTR("IconLocation");
	case TagType::ExtraData:
		return CSTR("ExtraData");
	default:
		return CSTR("Unknown");
	}
}

Text::CString IO::FileAnalyse::LNKFileAnalyse::ShowWindowGetName(UInt32 showWindow)
{
	switch (showWindow)
	{
	case 0x00000003:
		return CSTR("SW_SHOWMAXIMIZED");
	case 0x00000007:
		return CSTR("SW_SHOWMINNOACTIVE");
	case 0x00000001:
	default:
		return CSTR("SW_SHOWNORMAL");
	}
}

Text::CString IO::FileAnalyse::LNKFileAnalyse::DriveTypeGetName(UInt32 driveType)
{
	switch (driveType)
	{
	case 0x00000001:
		return CSTR("DRIVE_NO_ROOT_DIR");
	case 0x00000002:
		return CSTR("DRIVE_REMOVABLE");
	case 0x00000003:
		return CSTR("DRIVE_FIXED");
	case 0x00000004:
		return CSTR("DRIVE_REMOTE");
	case 0x00000005:
		return CSTR("DRIVE_CDROM");
	case 0x00000006:
		return CSTR("DRIVE_RAMDISK");
	case 0x00000000:
	default:
		return CSTR("DRIVE_UNKNOWN");
	}
}

Text::CString IO::FileAnalyse::LNKFileAnalyse::FontFamilyGetName(UInt32 fontFamily)
{
	switch (fontFamily & 0xf0)
	{
	case 0x00:
		return CSTR("FF_DONTCARE");
	case 0x10:
		return CSTR("FF_ROMAN");
	case 0x20:
		return CSTR("FF_SWISS");
	case 0x30:
		return CSTR("FF_MODERN");
	case 0x40:
		return CSTR("FF_SCRIPT");
	case 0x50:
		return CSTR("FF_DECORATIVE");
	default:
		return CSTR("Unknown");
	}
}
