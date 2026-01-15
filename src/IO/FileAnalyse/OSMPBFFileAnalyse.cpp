#include "Stdafx.h"
#include "Core/ByteTool_C.h"
#include "Data/ByteBuffer.h"
#include "Data/Compress/Inflater.h"
#include "IO/ProtocolBuffersMessage.h"
#include "IO/ProtocolBuffersMessageFast.h"
#include "IO/FileAnalyse/OSMPBFFileAnalyse.h"
#include "IO/FileAnalyse/ProtocolBuffersFileAnalyse.h"
#include "IO/StmData/MemoryDataRef.h"
#include "Sync/MutexUsage.h"
#include "Text/StringBuilderUTF8.h"

void __stdcall IO::FileAnalyse::OSMPBFFileAnalyse::ParseThread(NN<Sync::Thread> thread)
{
	NN<IO::FileAnalyse::OSMPBFFileAnalyse> me = thread->GetUserObj().GetNN<IO::FileAnalyse::OSMPBFFileAnalyse>();
	NN<IO::StreamData> fd;
	if (!me->fd.SetTo(fd))
		return;
	Data::ByteBuffer packetBuff(256);
	UIntOS buffSize;

	ProtocolBuffersMessageFast blobHeader;
	blobHeader.AddString(true, 1); //type
	blobHeader.AddBytes(false, 2); //indexdata
	blobHeader.AddInt32(true, 3); //datasize
	UInt64 ofst;
	UInt64 dataSize;
	UInt64 thisSize;
	NN<PackInfo> pack;
	Int32 blobSize;
	NN<Text::String> dataType;
	ofst = 0;
	dataSize = fd->GetDataSize();
	while (ofst < dataSize - 4 && !thread->IsStopping())
	{
		if (fd->GetRealData(ofst, 4, packetBuff) != 4)
			break;
		pack = MemAllocNN(PackInfo);
		pack->fileOfst = ofst;
		pack->packSize = 4;
		pack->packType = PackType::Length;
		pack->dataType = nullptr;
		me->packs.Add(pack);
		ofst += 4;

		thisSize = packetBuff.ReadMU32(0);
		if (thisSize + ofst > dataSize)
		{
			break;
		}
		pack = MemAllocNN(PackInfo);
		pack->fileOfst = ofst;
		pack->packSize = thisSize;
		pack->packType = PackType::BlobHeader;
		pack->dataType = nullptr;
		me->packs.Add(pack);
		if (thisSize > packetBuff.GetSize())
		{
			buffSize = packetBuff.GetSize();
			while (buffSize < thisSize)
			{
				buffSize <<= 1;
			}
			packetBuff.ChangeSizeAndClear(buffSize);
		}
		if (fd->GetRealData(ofst, thisSize, packetBuff) != thisSize)
		{
			break;
		}
		blobHeader.ClearValues();
		if (!blobHeader.ParseMsssage(packetBuff.Arr(), (UIntOS)thisSize))
		{
			break;
		}
		ofst += thisSize;
		thisSize = blobHeader.GetInt32(3, blobSize) ? (UInt64)blobSize : 0;
		if (ofst + thisSize > dataSize)
		{
			break;
		}
		pack = MemAllocNN(PackInfo);
		pack->fileOfst = ofst;
		pack->packSize = thisSize;
		pack->packType = PackType::Blob;
		pack->dataType = nullptr;
		if (blobHeader.GetString(1, dataType))
		{
			pack->dataType = dataType->Clone();
		}
		me->packs.Add(pack);
		ofst += thisSize;
	}
}

void __stdcall IO::FileAnalyse::OSMPBFFileAnalyse::FreePackInfo(NN<PackInfo> pack)
{
	OPTSTR_DEL(pack->dataType);
	MemFreeNN(pack);
}

IO::FileAnalyse::OSMPBFFileAnalyse::OSMPBFFileAnalyse(NN<IO::StreamData> fd) : thread(ParseThread, this, CSTR("OSMPBFFA"))
{
	UInt8 buff[128];
	this->fd = nullptr;
	this->pauseParsing = false;
	if (fd->GetRealData(0, 128, BYTEARR(buff)) != 128)
	{
		return;
	}
	if (buff[4] != 0x0a || buff[5] != 9 || !Text::StrEqualsC(&buff[6], 9, UTF8STRC("OSMHeader")))
	{
		return;
	}
	this->fd = fd->GetPartialData(0, fd->GetDataSize());
	this->thread.Start();
}

IO::FileAnalyse::OSMPBFFileAnalyse::~OSMPBFFileAnalyse()
{
	this->thread.Stop();
	this->fd.Delete();
	this->packs.FreeAll(FreePackInfo);
}

Text::CStringNN IO::FileAnalyse::OSMPBFFileAnalyse::GetFormatName()
{
	return CSTR("OSMPBF");
}

UIntOS IO::FileAnalyse::OSMPBFFileAnalyse::GetFrameCount()
{
	return this->packs.GetCount();
}

Bool IO::FileAnalyse::OSMPBFFileAnalyse::GetFrameName(UIntOS index, NN<Text::StringBuilderUTF8> sb)
{
	NN<IO::StreamData> fd;
	NN<PackInfo> pack;
	if (index >= this->packs.GetCount())
	{
		return false;
	}
	if (!this->fd.SetTo(fd))
	{
		return false;
	}
	pack = this->packs.GetItemNoCheck(index);
	Data::ByteBuffer packetBuff(pack->packSize);
	fd->GetRealData(pack->fileOfst, pack->packSize, packetBuff);
	sb->AppendU64(pack->fileOfst);
	sb->AppendC(UTF8STRC(", psize="));
	sb->AppendUIntOS(pack->packSize);
	sb->AppendC(UTF8STRC(", "));
	sb->Append(PackTypeGetName(pack->packType));
	return true;
}

UIntOS IO::FileAnalyse::OSMPBFFileAnalyse::GetFrameIndex(UInt64 ofst)
{
	IntOS i = 0;
	IntOS j = (IntOS)this->packs.GetCount() - 1;
	IntOS k;
	NN<PackInfo> pack;
	while (i <= j)
	{
		k = (i + j) >> 1;
		pack = this->packs.GetItemNoCheck((UIntOS)k);
		if (ofst < pack->fileOfst)
		{
			j = k - 1;
		}
		else if (ofst >= pack->fileOfst + pack->packSize)
		{
			i = k + 1;
		}
		else
		{
			return (UIntOS)k;
		}
	}
	return INVALID_INDEX;
}

Optional<IO::FileAnalyse::FrameDetail> IO::FileAnalyse::OSMPBFFileAnalyse::GetFrameDetail(UIntOS index)
{
	NN<IO::StreamData> fd;
	if (!this->fd.SetTo(fd))
	{
		return nullptr;
	}
	NN<IO::FileAnalyse::FrameDetail> frame;
	if (index >= this->packs.GetCount())
	{
		return nullptr;
	}
	NN<PackInfo> pack = this->packs.GetItemNoCheck(index);
	NEW_CLASSNN(frame, IO::FileAnalyse::FrameDetail(pack->fileOfst, pack->packSize));
	Data::ByteBuffer packetBuff(pack->packSize);
	fd->GetRealData(pack->fileOfst, pack->packSize, packetBuff);
	switch (pack->packType)
	{
	case PackType::Length:
		frame->AddUInt(0, 4, CSTR("Length"), packetBuff.ReadMU32(0));
		break;
	case PackType::BlobHeader:
		{
			ProtocolBuffersMessage blobHeader(CSTR("BlobHeader"));
			blobHeader.AddString(true, CSTR("type"), 1, false);
			blobHeader.AddBytes(false, CSTR("indexdata"), 2, false);
			blobHeader.AddInt32(true, CSTR("datasize"), 3, false);
			blobHeader.ParseMsssage(frame, packetBuff.Arr(), 0, (UIntOS)pack->packSize);
		}
		break;
	case PackType::Blob:
		{
			ProtocolBuffersMessage blob(CSTR("Blob"));
			blob.AddBytes(false, CSTR("raw"), 1, false);
			blob.AddInt32(false, CSTR("raw_size"), 2, false);
			blob.AddBytes(false, CSTR("zlib_data"), 3, false);
			blob.AddBytes(false, CSTR("lzma_data"), 4, false);
			blob.AddBytes(false, CSTR("OBSOLUTE_bzip2_data"), 5, false);
			blob.AddBytes(false, CSTR("lz4_data"), 6, false);
			blob.AddBytes(false, CSTR("zstd_data"), 7, false);
			Int32 rawSize;
			if (blob.ParseMsssage(frame, packetBuff.Arr(), 0, (UIntOS)pack->packSize) && blob.GetInt32(2, rawSize) && rawSize > 0)
			{
				NN<Data::ByteBuffer> sourceData;
				if (blob.GetBytes(1, sourceData) && sourceData->GetSize() == (UIntOS)rawSize)
				{
					frame->SetDevrivedBuff(sourceData->SubArray(0));
				}
				else if (blob.GetBytes(3, sourceData))
				{
					Data::ByteBuffer devBuff((UIntOS)rawSize);
					UIntOS devSize;
					Bool succ = Data::Compress::Inflater::DecompressDirect(devBuff, devSize, sourceData->SubArray(0), true);
					//printf("Zlib Decompress: %s, devSize = %llu, rawSize = %d\r\n", succ?"succ":"fail", devSize, rawSize);
					if (succ && devSize == (UIntOS)rawSize)
					{
						frame->SetDevrivedBuff(devBuff);
					}
				}
				NN<Text::String> dataType;
				NN<Data::ByteBuffer> devBuff;
				if (pack->dataType.SetTo(dataType) && frame->GetDevrivedBuff().SetTo(devBuff))
				{
					NN<ProtocolBuffersMessage> msg;
					NN<ProtocolBuffersMessage> subMsg;
					NN<ProtocolBuffersMessage> subMsg2;
					NN<ProtocolBuffersMessage> msgInfo;
					NN<IO::FileAnalyse::ProtocolBuffersFileAnalyseCreator> analyseCreator;
					if (dataType->Equals(UTF8STRC("OSMData")))
					{
						NEW_CLASSNN(msg, ProtocolBuffersMessage(CSTR("OSMData")));
						NEW_CLASSNN(subMsg, ProtocolBuffersMessage(CSTR("StringTable")));
						subMsg->AddString(true, CSTR("s"), 1, false);
						msg->AddSubMessage(true, CSTR("stringtable"), 1, subMsg);
						NEW_CLASSNN(msgInfo, ProtocolBuffersMessage(CSTR("Info")));
						msgInfo->AddInt32(false, CSTR("version"), 1, false);
						msgInfo->AddInt64(false, CSTR("timestamp"), 2, false);
						msgInfo->AddInt64(false, CSTR("changeset"), 3, false);
						msgInfo->AddInt32(false, CSTR("uid"), 4, false);
						msgInfo->AddUInt32(false, CSTR("user_sid"), 5, false);
						msgInfo->AddBool(false, CSTR("visible"), 6, false);
						NEW_CLASSNN(subMsg, ProtocolBuffersMessage(CSTR("PrimitiveGroup")));
						NEW_CLASSNN(subMsg2, ProtocolBuffersMessage(CSTR("Node")));
						subMsg2->AddSInt64(true, CSTR("id"), 1, false);
						subMsg2->AddUInt32(false, CSTR("keys"), 2, true);
						subMsg2->AddUInt32(false, CSTR("vals"), 3, true);
						subMsg2->AddSubMessage(false, CSTR("info"), 4, msgInfo);
						subMsg2->AddSInt64(true, CSTR("lat"), 8, false);
						subMsg2->AddSInt64(true, CSTR("lon"), 9, false);
						subMsg->AddSubMessage(true, CSTR("nodes"), 1, subMsg2);
						NEW_CLASSNN(subMsg2, ProtocolBuffersMessage(CSTR("Way")));
						subMsg2->AddInt64(true, CSTR("id"), 1, false);
						subMsg2->AddUInt32(false, CSTR("keys"), 2, true);
						subMsg2->AddUInt32(false, CSTR("vals"), 3, true);
						subMsg2->AddSubMessage(false, CSTR("info"), 4, msgInfo->Clone());
						subMsg2->AddSInt64(false, CSTR("refs"), 8, true, true);
						subMsg2->AddSInt64(false, CSTR("lat"), 9, true, true);
						subMsg2->AddSInt64(false, CSTR("lon"), 10, true, true);
						subMsg->AddSubMessage(true, CSTR("ways"), 3, subMsg2);
						NEW_CLASSNN(subMsg2, ProtocolBuffersMessage(CSTR("Relation")));
						subMsg2->AddInt64(true, CSTR("id"), 1, false);
						subMsg2->AddUInt32(false, CSTR("keys"), 2, true);
						subMsg2->AddUInt32(false, CSTR("vals"), 3, true);
						subMsg2->AddSubMessage(false, CSTR("info"), 4, msgInfo->Clone());
						subMsg2->AddInt32(false, CSTR("roles_sid"), 8, true);
						subMsg2->AddSInt64(false, CSTR("memids"), 9, true, true);
						subMsg2->AddEnum(false, CSTR("types"), 10, true);
						subMsg->AddSubMessage(true, CSTR("relations"), 4, subMsg2);
						NEW_CLASSNN(msgInfo, ProtocolBuffersMessage(CSTR("DenseInfo")));
						msgInfo->AddInt32(false, CSTR("version"), 1, true);
						msgInfo->AddSInt64(false, CSTR("timestamp"), 2, true, true);
						msgInfo->AddSInt64(false, CSTR("changeset"), 3, true, true);
						msgInfo->AddSInt32(false, CSTR("uid"), 4, true, true);
						msgInfo->AddSInt32(false, CSTR("user_sid"), 5, true, true);
						msgInfo->AddBool(false, CSTR("visible"), 6, true);
						NEW_CLASSNN(subMsg2, ProtocolBuffersMessage(CSTR("DenseNodes")));
						subMsg2->AddSInt64(true, CSTR("id"), 1, true, true);
						subMsg2->AddSubMessage(false, CSTR("denseinfo"), 5, msgInfo);
						subMsg2->AddSInt64(false, CSTR("lat"), 8, true, true);
						subMsg2->AddSInt64(false, CSTR("lon"), 9, true, true);
						subMsg2->AddUInt32(false, CSTR("keys_vals"), 10, true);
						subMsg->AddSubMessage(false, CSTR("dense"), 2, subMsg2);
						msg->AddSubMessage(true, CSTR("primitivegroup"), 2, subMsg);
						msg->AddInt32(false, CSTR("granularity"), 17, false);
						msg->AddInt64(false, CSTR("lat_offset"), 19, false);
						msg->AddInt64(false, CSTR("lon_offset"), 20, false);
						msg->AddInt32(false, CSTR("date_granularity"), 18, false);
						NEW_CLASSNN(analyseCreator, IO::FileAnalyse::ProtocolBuffersFileAnalyseCreator(msg));
						frame->SetDevrivedAnaylse(analyseCreator);
					}
					else if (dataType->Equals(UTF8STRC("OSMHeader")))
					{
						NEW_CLASSNN(subMsg, ProtocolBuffersMessage(CSTR("HeaderBBox")));
						subMsg->AddInt64(true, CSTR("left"), 1, false);
						subMsg->AddInt64(true, CSTR("right"), 2, false);
						subMsg->AddInt64(true, CSTR("top"), 3, false);
						subMsg->AddInt64(true, CSTR("bottom"), 4, false);
						NEW_CLASSNN(msg, ProtocolBuffersMessage(CSTR("HeaderBlock")));
						msg->AddSubMessage(false, CSTR("bbox"), 1, subMsg);
						msg->AddString(false, CSTR("required_features"), 4, false);
						msg->AddString(false, CSTR("optional_features"), 5, false);
						msg->AddString(false, CSTR("writingprogram"), 16, false);
						msg->AddString(false, CSTR("source"), 17, false);
						msg->AddInt64(false, CSTR("osmosis_replication_timestamp"), 32, false);
						msg->AddInt64(false, CSTR("osmosis_replication_sequence_number"), 33, false);
						msg->AddString(false, CSTR("osmosis_replication_base_url"), 34, false);
						NEW_CLASSNN(analyseCreator, IO::FileAnalyse::ProtocolBuffersFileAnalyseCreator(msg));
						frame->SetDevrivedAnaylse(analyseCreator);
					}
					else
					{
						printf("OSMPBFFileAnalyse: Unknown OSM PBF Blob Type: %s\r\n", dataType->v.Ptr());
					}
				}
			}
		}
	}
	return frame;
}

Bool IO::FileAnalyse::OSMPBFFileAnalyse::IsError()
{
	return this->fd.IsNull();
}

Bool IO::FileAnalyse::OSMPBFFileAnalyse::IsParsing()
{
	return this->thread.IsRunning();
}

Bool IO::FileAnalyse::OSMPBFFileAnalyse::TrimPadding(Text::CStringNN outputFile)
{
	return false;
}

Text::CStringNN IO::FileAnalyse::OSMPBFFileAnalyse::PackTypeGetName(PackType packType)
{
	switch (packType)
	{
	case PackType::Length:
		return CSTR("Length");
	case PackType::BlobHeader:
		return CSTR("BlobHeader");
	case PackType::Blob:
		return CSTR("Blob");
	default:
		return CSTR("Unknown");
	}
}