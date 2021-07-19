#include "Stdafx.h"
#include "MyMemory.h"
#include "Data/ArrayListInt64.h"
#include "Data/ArrayListUInt32.h"
#include "Data/ArrayListUInt64.h"
#include "Data/ByteTool.h"
#include "IO/StmData/BlockStreamData.h"
#include "Media/AudioFrameSource.h"
#include "Media/FileVideoSource.h"
#include "Media/LPCMSource.h"
#include "Media/MediaFile.h"
#include "Parser/FileParser/AVIParser.h"
#include "Text/Encoding.h"
#include "Text/MyStringW.h"

#if defined(_WIN32)
#include <windows.h>
#else
typedef struct
{
	UInt32 biSize;
	Int32 biWidth;
	Int32 biHeight;
	UInt16 biPlanes;
	UInt16 biBitCount;
	UInt32 biCompression;
	UInt32 biSizeImage;
	Int32 biXPelsPerMeter;
	Int32 biYPelsPerMeter;
	UInt32 biClrUsed;
	UInt32 biClrImportant;
} BITMAPINFOHEADER;
#endif


typedef struct
{
	UInt16 left;
	UInt16 top;
	UInt16 right;
	UInt16 bottom;
} FRAMERECT;

typedef struct
{
	Int32 fccType;
	Int32 fccHandler;
	UInt32 dwFlags;
	UInt32 dwPriority;
	UInt32 dwInitialFrames;
	UInt32 dwScale;
	UInt32 dwRate;
	UInt32 dwStart;
	UInt32 dwLength;
	UInt32 dwSuggestedBufferSize;
	UInt32 dwQuality;
	UInt32 dwSampleSize;
	FRAMERECT rcFrame;
} STRH;

typedef struct
{
	UInt32 dwMicroSecPerFrame;
	UInt32 dwMaxBytesPerSec;
    UInt32 dwPaddingGranularity;
    UInt32 dwFlags;
    UInt32 dwTotalFrames;
    UInt32 dwInitialFrames;
    UInt32 dwStreams;
    UInt32 dwSuggestedBufferSize;
    UInt32 dwWidth;
    UInt32 dwHeight;
    UInt32 dwReserved[4];
} MyAVIHeader;

typedef struct
{
	STRH strh;
	UInt32 strfSize;
	UInt8 *strf;
	UInt32 otherSize;
	UInt8 *others;
} AVIStream;

Parser::FileParser::AVIParser::AVIParser()
{
	this->codePage = 0;
}

Parser::FileParser::AVIParser::~AVIParser()
{
}

Int32 Parser::FileParser::AVIParser::GetName()
{
	return *(Int32*)"AVIP";
}

void Parser::FileParser::AVIParser::SetCodePage(UInt32 codePage)
{
	this->codePage = codePage;
}

void Parser::FileParser::AVIParser::PrepareSelector(IO::IFileSelector *selector, IO::ParsedObject::ParserType t)
{
	if (t == IO::ParsedObject::PT_UNKNOWN || t == IO::ParsedObject::PT_VIDEO_PARSER)
	{
		selector->AddFilter((const UTF8Char*)"*.avi", (const UTF8Char*)"AVI File");
	}
}

IO::ParsedObject::ParserType Parser::FileParser::AVIParser::GetParserType()
{
	return IO::ParsedObject::PT_VIDEO_PARSER;
}

IO::ParsedObject *Parser::FileParser::AVIParser::ParseFile(IO::IStreamData *fd, IO::PackageFile *pkgFile, IO::ParsedObject::ParserType targetType)
{
	Text::Encoding enc(this->codePage);
	UTF8Char sbuff[512];
	UInt8 headBuffer[12];
	UInt8 chunkBuffer[12];
	UInt64 offset;
//	Int32 hdrlSize;
	UInt8 *buffer;
	UInt32 i;
	UInt32 j;
	UInt32 k;
	UInt32 l;
	UInt16 wLongsPerEntry;
	Int32 cmpTmp;
	Media::MediaFile *mf;
	const UTF8Char *audsName;

	UInt32 rate = 30000;
	UInt32 scale = 1001;

	UInt64 base = 0;
	MyAVIHeader *avih = 0;
	AVIStream *strl = 0;
//	Int64 frameCnt;
	UInt8 *idx1 = 0;
	UInt8 *info = 0;
	UInt8 *indx = 0;
	UInt8 *chap = 0;

	fd->GetRealData(0, 12, headBuffer);
	if (*(Int32*)&headBuffer[0] != *(Int32*)"RIFF" || *(Int32*)&headBuffer[8] != *(Int32*)"AVI ")
		return 0;

	fd->GetRealData(12, 12, chunkBuffer);
	offset = 24;
	if (*(Int32*)chunkBuffer != *(Int32*)"LIST" || *(Int32*)&chunkBuffer[8] != *(Int32*)"hdrl")
	{
		return 0;
	}
//	hdrlSize = *(Int32*)&chunkBuffer[4];

	offset += fd->GetRealData(offset, 8, chunkBuffer);
	if (*(Int32*)chunkBuffer != *(Int32*)"avih")
	{
		return 0;
	}

	avih = (MyAVIHeader*)MAlloc(i = ReadUInt32(&chunkBuffer[4]));
	offset += fd->GetRealData(offset, i, (UInt8*)avih);

	offset += fd->GetRealData(offset, 12, chunkBuffer);
	strl = MemAlloc(AVIStream, i = avih->dwStreams);
	l = 0;
	while (*(Int32*)chunkBuffer == *(Int32*)"LIST" && *(Int32*)&chunkBuffer[8] == *(Int32*)"strl")
	{
		buffer = MemAlloc(UInt8, i = ReadUInt32(&chunkBuffer[4]) - 4);
		offset += fd->GetRealData(offset, i, buffer);
		if (*(Int32*)buffer != *(Int32*)"strh")
		{
			l += 1;
			MemFree(buffer);
			continue;
		}
		MemCopyNO(&strl[l].strh, &buffer[8], ReadUInt32(&buffer[4]));

		j = 8 + ReadUInt32(&buffer[4]);
		if (*(Int32*)&buffer[j] != *(Int32*)"strf")
		{
			l += 1;
			MemFree(buffer);
			continue;
		}

		strl[l].strf = MemAlloc(UInt8, strl[l].strfSize = ReadUInt32(&buffer[j + 4]));
		MemCopyNO(strl[l].strf, &buffer[j + 8], strl[l].strfSize);
		j += 8 + strl[l].strfSize;
		if (ReadUInt32(&chunkBuffer[4]) - 4 > j)
			if (*(Int32*)&buffer[j] == *(Int32*)"JUNK")
				j += ReadUInt32(&buffer[j + 4]) + 8;
		if (ReadUInt32(&chunkBuffer[4]) - 4 > j)
		{
			strl[l].otherSize = ReadUInt32(&chunkBuffer[4]) - j - 4;
			MemCopyNO(strl[l].others = MemAlloc(UInt8, strl[l].otherSize), &buffer[j], strl[l].otherSize);
		}
		else
		{
			strl[l].others = 0;
			strl[l].otherSize = 0;
		}

		MemFree(buffer);
		offset += fd->GetRealData(offset, 12, chunkBuffer);
		l += 1;
	}

	while (offset <= *(UInt32*)&headBuffer[4] + 4)
	{
		if (*(Int32*)chunkBuffer == *(Int32*)"idx1")
		{
			offset -= 8;
			idx1 = MemAlloc(UInt8, i = ReadUInt32(&chunkBuffer[4]) + 4);
			offset += fd->GetRealData(offset, i, idx1);
		}
		else if (*(Int32*)chunkBuffer == *(Int32*)"LIST")
		{
			if (*(Int32*)&chunkBuffer[8] == *(Int32*)"INFO")
			{
				if (info)
					MemFree(info);
				info = MemAlloc(UInt8, i = ReadUInt32(&chunkBuffer[4]));
				WriteUInt32(info, i);
				offset += fd->GetRealData(offset, i - 4, &info[4]);
			}
			else if (*(Int32*)&chunkBuffer[8] == *(Int32*)"odml")
			{
				buffer = MemAlloc(UInt8, i = ReadUInt32(&chunkBuffer[4]) - 4);
				offset += fd->GetRealData(offset, i, buffer);
				l = 0;
				while (l < i)
				{
					if (*(Int32*)&buffer[l] == *(Int32*)"dmlh")
					{
//						frameCnt = *(Int64*)&buffer[l + 8];
						break;
					}
					l += ReadUInt32(&buffer[l + 4]) + 8;
				}
				MemFree(buffer);
			}
			else if (*(Int32*)&chunkBuffer[8] == *(Int32*)"movi")
			{
				base = offset;
				offset += ReadUInt32(&chunkBuffer[4]) - 4;
			}
			else
			{
				offset += ReadUInt32(&chunkBuffer[4]) - 4;
			}
		}
		else if (*(Int32*)chunkBuffer == *(Int32*)"JUNK")
		{
			offset += ReadUInt32(&chunkBuffer[4]) - 4;
		}
		else if (*(Int32*)chunkBuffer == *(Int32*)"SMCH")
		{
			offset -= 4;
			chap = MemAlloc(UInt8, i = ReadUInt32(&chunkBuffer[4]));
			offset += fd->GetRealData(offset, i, chap);
			offset += offset & 1;
		}
		else
		{
			offset += ReadUInt32(&chunkBuffer[4]) - 4;
		}
		offset += (i = (UInt32)fd->GetRealData(offset, 12, chunkBuffer));
		if (i == 0)
			break;
	}

	NEW_CLASS(mf, Media::MediaFile(fd->GetFullName()));
	i = 0;
	while (i < avih->dwStreams)
	{
		if (strl[i].strh.fccType == *(Int32*)"vids")
		{
			Int32 arARH = 1;
			Int32 arARV = 1;
			UInt32 arLeft = 0;
			UInt32 arTop = 0;
			UInt32 arRight = 0;
			UInt32 arBottom = 0;
//			Int32 ccCoord = 0;
			Int32 ccYUV = 0;
			Int32 ccTransfer = 0;
			Int32 ccFrameOrder = 0;
			Bool error = false;

			rate = strl[i].strh.dwRate;
			scale = strl[i].strh.dwScale;

			Data::ArrayListUInt64 *ofsts;
			Data::ArrayListUInt32 *sizes;
			NEW_CLASS(ofsts, Data::ArrayListUInt64());
			NEW_CLASS(sizes, Data::ArrayListUInt32());

			//vs = new DataSegment(stmdata, true);
			j = 0;
			while (j < strl[i].otherSize)
			{
				if (*(Int32*)&strl[i].others[j] == *(Int32*)"strn")
				{
					enc.UTF8FromBytes(sbuff, &strl[i].others[j + 8], ReadUInt32(&strl[i].others[j + 4]), 0);
					audsName = Text::StrCopyNew(sbuff);
					j += *(UInt32*)&strl[i].others[j + 4] + 8;
				}
				else if (*(Int32*)&strl[i].others[j] == *(Int32*)"indx")
				{
					if (idx1)
						MemFree(idx1);
					idx1 = 0;
					indx = (UInt8*)&strl[i].others[j + 8];
					wLongsPerEntry = ReadUInt16(indx);
					if (wLongsPerEntry != 4 || indx[3] != 0)
					{
						error = true;
						break;
					}
					k = 0;
					while (k < *(UInt32*)&indx[4])
					{
						buffer = MemAlloc(UInt8, l = ReadUInt32(&indx[(k << 4) + 32]));
						if (l > fd->GetRealData(ReadUInt64(&indx[(k << 4) + 24]), l, buffer))
						{
							MemFree(buffer);
							break;
						}

						if (ReadUInt32(&buffer[4]) != ReadUInt32(&indx[(k << 4) + 32]) - 8 && ReadUInt32(&buffer[4]) != ReadUInt32(&indx[(k << 4) + 32]))
						{
							MemFree(buffer);
							error = true;
							break;
						}
						if (buffer[10])
						{
							MemFree(buffer);
							error = true;
							break;
						}
						wLongsPerEntry = ReadUInt16(&buffer[8]);
						if (wLongsPerEntry != 2)
						{
							MemFree(buffer);
							error = true;
							break;
						}

						base = ReadUInt64(&buffer[20]);
						l = 32;
						while (l < ReadUInt32(&indx[(k << 4) + 32]))
						{
							if (ReadUInt32(&buffer[l]) || ReadUInt32(&buffer[l + 4]))
							{
								ofsts->Add(base + ReadUInt32(&buffer[l]));
								sizes->Add(ReadUInt32(&buffer[l + 4]));
							}
							l += 8;
						}
						MemFree(buffer);
						k++;
					}

					j += ReadUInt32(&strl[i].others[j + 4]) + 8;
				}
				else if (*(Int32*)&strl[i].others[j] == *(Int32*)"strc")
				{
					ccYUV = strl[i].others[j + 8];
					ccTransfer = strl[i].others[j + 9];
//					ccCoord = strl[i].others[j + 10];
					if (*(Int32*)&strl[i].others[j + 4] >= 4)
						ccFrameOrder = strl[i].others[j + 11];
					j += ReadUInt32(&strl[i].others[j + 4]) + 8;
				}
				else if (*(Int32*)&strl[i].others[j] == *(Int32*)"vpar")
				{
					arTop = ReadUInt16(&strl[i].others[j + 8]);
					arBottom = ReadUInt16(&strl[i].others[j + 10]);
					arLeft = ReadUInt16(&strl[i].others[j + 12]);
					arRight = ReadUInt16(&strl[i].others[j + 14]);
					arARH = ReadUInt16(&strl[i].others[j + 16]);
					arARV = ReadUInt16(&strl[i].others[j + 18]);
					j += ReadUInt32(&strl[i].others[j + 4]) + 8;
				}
				else
				{
					j += ReadUInt32(&strl[i].others[j + 4]) + 8;
				}
				if (j & 1) j++;
			}

			if (idx1)
			{
				k = 4;
				l = ReadUInt32(idx1);
				while (k < l)
				{
					if (*(Int16*)&idx1[k + 2] == *(Int16*)"dc" || *(Int16*)&idx1[k + 2] == *(Int16*)"db")
					{
						ofsts->Add(base + *(UInt32*)&idx1[k + 8] + 4);
						sizes->Add(*(UInt32*)&idx1[k + 12] | (((*(UInt32*)&idx1[k + 12] && (*(UInt32*)&idx1[k + 4] & 16)) || sizes->GetCount() == 0)?0:0x80000000));
					}
					k += 16;
				}
			}

			if (!error)
			{
				BITMAPINFOHEADER *bmih = (BITMAPINFOHEADER*)strl[i].strf;
				Media::FrameInfo info;
				info.dispWidth = (UInt32)bmih->biWidth;
				info.dispHeight = (UInt32)bmih->biHeight;
				info.storeWidth = info.dispWidth;
				info.storeHeight = info.dispHeight;
				info.fourcc = bmih->biCompression;
				info.storeBPP = bmih->biBitCount;
				info.pf = Media::FrameInfo::GetDefPixelFormat(bmih->biCompression, bmih->biBitCount);
				info.byteSize = bmih->biSizeImage;
				info.par2 = arARV / (Double)arARH;
				info.hdpi = 96;
				switch (ccFrameOrder)
				{
				case 0:
				default:
					info.ftype = Media::FT_NON_INTERLACE;
					break;
				case 1:
					info.ftype = Media::FT_FIELD_TF;
					break;
				case 2:
					info.ftype = Media::FT_FIELD_BF;
					break;
				case 3:
					info.ftype = Media::FT_INTERLACED_TFF;
					break;
				case 4:
					info.ftype = Media::FT_INTERLACED_BFF;
					break;
				}
				info.atype = Media::AT_NO_ALPHA;
				Media::CS::TransferType ttype;
				switch (ccTransfer)
				{
				case 1:
					ttype = Media::CS::TRANT_sRGB;
					break;
				case 2:
					ttype = Media::CS::TRANT_GAMMA;
					break;
				case 3:
					ttype = Media::CS::TRANT_BT709;
					break;
				case 4:
					ttype = Media::CS::TRANT_SMPTE240;
					break;
				default:
					ttype = Media::CS::TRANT_VUNKNOWN;
					break;
				}
				info.color->GetRTranParam()->Set(ttype, 2.2);
				info.color->GetGTranParam()->Set(ttype, 2.2);
				info.color->GetBTranParam()->Set(ttype, 2.2);
				switch (ccYUV)
				{
				case 1:
					info.yuvType = Media::ColorProfile::YUVT_BT601;
					break;
				case 2:
					info.yuvType = Media::ColorProfile::YUVT_BT709;
					break;
				case 3:
					info.yuvType = Media::ColorProfile::YUVT_SMPTE240M;
					break;
				default:
					info.yuvType = Media::ColorProfile::YUVT_UNKNOWN;
					break;
				}
				info.ycOfst = Media::YCOFST_C_CENTER_LEFT;

				Media::FileVideoSource *vstm;
				NEW_CLASS(vstm, Media::FileVideoSource(fd, &info, strl[i].strh.dwRate, strl[i].strh.dwScale, false));
				UInt32 scale = strl[i].strh.dwScale;
				UInt32 rate = strl[i].strh.dwRate;
				k = (UInt32)ofsts->GetCount();
				j = 0;
				while (j < k)
				{
					UInt32 size = sizes->GetItem(j);
					vstm->AddNewFrame(ofsts->GetItem(j), size & 0x7fffffff, (size & 0x80000000) == 0, MulDivU32(j, scale * 1000, rate));
					j++;
				}

				vstm->SetBorderCrop(arLeft, arTop, arRight, arBottom);
				mf->AddSource(vstm, 0);
			}

			DEL_CLASS(ofsts);
			DEL_CLASS(sizes);
		}
		else if (strl[i].strh.fccType == *(Int32*)"auds")
		{
			Media::AudioFormat fmt;
			Media::AudioFrameSource *audsData = 0;
			Media::LPCMSource *lpcmData = 0;
			IO::StmData::BlockStreamData *blkData = 0;
			Int32 audDelay = 0;
			Bool error;

			fmt.FromWAVEFORMATEX(strl[i].strf);

			audsName = 0;
			error = false;
			j = 0;
			while (j < strl[i].otherSize)
			{
				if (*(Int32*)&strl[i].others[j] == *(Int32*)"strn")
				{
					enc.UTF8FromBytes(sbuff, &strl[i].others[j + 8], ReadUInt32(&strl[i].others[j + 4]), 0);
					audsName = Text::StrCopyNew(sbuff);
					j += *(UInt32*)&strl[i].others[j + 4] + 8;
				}
				else if (*(Int32*)&strl[i].others[j] == *(Int32*)"indx")
				{
					if (idx1)
						MemFree(idx1);
					idx1 = 0;

					UInt64 totalSize = 0;

					if (fmt.formatId == 1)
					{
						NEW_CLASS(blkData, IO::StmData::BlockStreamData(fd));
					}
					else
					{
						NEW_CLASS(audsData, Media::AudioFrameSource(fd, &fmt, audsName));
					}

					indx = (UInt8*)&strl[i].others[j + 8];
					wLongsPerEntry = *(UInt16*)indx;
					if (wLongsPerEntry != 4 || indx[3] != 0)
					{
						error = true;
						break;
					}
					k = 0;
					while (k < *(UInt32*)&indx[4])
					{
						buffer = MemAlloc(UInt8, l = ReadUInt32(&indx[(k << 4) + 32]));
						if (l > fd->GetRealData(ReadUInt64(&indx[(k << 4) + 24]), l, buffer))
						{
							MemFree(buffer);
							break;
						}

						if (*(Int32*)&buffer[4] != *(Int32*)&indx[(k << 4) + 32] - 8 && *(Int32*)&buffer[4] != *(Int32*)&indx[(k << 4) + 32])
						{
							MemFree(buffer);
							error = true;
							break;
						}
						if (buffer[10])
						{
							MemFree(buffer);
							error = true;
							break;
						}
						wLongsPerEntry = *(UInt16*)&buffer[8];
						if (wLongsPerEntry != 2)
						{
							MemFree(buffer);
							error = true;
							break;;
						}

						base = ReadUInt64(&buffer[20]);
						if (fmt.formatId == 1)
						{
							l = 32;
							while (l < *(UInt32*)&indx[(k << 4) + 32])
							{
								if (*(Int32*)&buffer[l + 4] || *(Int32*)&buffer[l])
								{
									Int32 dwSize = *(Int32*)&buffer[l + 4];
									blkData->Append(base + *(UInt32*)&buffer[l], dwSize & 0x7fffffff);
								}
								l += 8;
							}
						}
						else
						{
							l = 32;
							while (l < *(UInt32*)&indx[(k << 4) + 32])
							{
								if (*(Int32*)&buffer[l + 4] || *(Int32*)&buffer[l])
								{
									Int32 dwSize = *(Int32*)&buffer[l + 4];
									totalSize += dwSize & 0x7fffffff;
									audsData->AddBlock(base + *(UInt32*)&buffer[l], dwSize & 0x7fffffff, EstimateDecodeSize(&fmt, totalSize, dwSize & 0x7fffffff));
								}
								l += 8;
							}
						}
						MemFree(buffer);
						k++;
					}
					if(fmt.formatId == 1)
					{
						NEW_CLASS(lpcmData, Media::LPCMSource(blkData, 0, blkData->GetDataSize(), &fmt, audsName));
						DEL_CLASS(blkData);
					}

					j += (*(UInt32*)&strl[i].others[j + 4]) + 8;
				}
				else if (*(Int32*)&strl[i].others[j] == *(Int32*)"strd")
				{
					audDelay = *(Int32*)&strl[i].others[j + 8];
					j += *(UInt32*)&strl[i].others[j + 4] + 8;
				}
				else
				{
					j += (*(UInt32*)&strl[i].others[j + 4]) + 8;
				}
			}
			if (error)
			{
				DEL_CLASS(audsData);
				audsData = 0;
			}
			else if (audsData == 0 && lpcmData == 0 && idx1)
			{
				UInt64 totalSize = 0;
				if (fmt.formatId == 1)
				{
					NEW_CLASS(blkData, IO::StmData::BlockStreamData(fd));
					k = 4;
					l = ReadUInt32(idx1);
					cmpTmp = *(Int32*)"00wb";
					((Char*)&cmpTmp)[1] = (Char)('0' + (i % 10));
					*(Char*)&cmpTmp = (Char)('0' + (i / 10));
					while (k < l)
					{
						if (*(Int32*)&idx1[k] == cmpTmp)
							blkData->Append(base + ReadUInt32(&idx1[k + 8]) + 4, ReadUInt32(&idx1[k + 12]));
						k += 16;
					}
					NEW_CLASS(lpcmData, Media::LPCMSource(blkData, 0,  blkData->GetDataSize(), &fmt, audsName));
					DEL_CLASS(blkData);
				}
				else
				{
					NEW_CLASS(audsData, Media::AudioFrameSource(fd, &fmt, audsName));
					k = 4;
					l = ReadUInt32(idx1);
					cmpTmp = *(Int32*)"00wb";
					((Char*)&cmpTmp)[1] = (Char)('0' + (i % 10));
					*(Char*)&cmpTmp = (Char)('0' + (i / 10));
					while (k < l)
					{
						if (*(Int32*)&idx1[k] == cmpTmp)
						{
							totalSize += ReadUInt32(&idx1[k + 12]);
							audsData->AddBlock(base + ReadUInt32(&idx1[k + 8]) + 4, ReadUInt32(&idx1[k + 12]), EstimateDecodeSize(&fmt, totalSize, ReadUInt32(&idx1[k + 12])));
						}
						k += 16;
					}
				}
			}

/*			Data::ArrayList<DataSegment*> *dsList = new Data::ArrayList<DataSegment*>();
			dsList->Add(audsData);
			audioList->Add(new AudioStream((WAVEFORMATEX*)strl[i].strf, dsList, audsName, audDelay));*/
			/////////////////////////////////
			if (audsName)
			{
				Text::StrDelNew(audsName);
				audsName = 0;
			}
			if (audsData)
			{
				mf->AddSource(audsData, audDelay);
			}
			else if (lpcmData)
			{
				mf->AddSource(lpcmData, audDelay);
			}
		}
		else
		{
		}
		i++;
	}

	if (idx1)
	{
		MemFree(idx1);
	}
	if (info)
	{
		MemFree(info);
	}
	if (chap)
	{
		Media::ChapterInfo *chapters;
		NEW_CLASS(chapters, Media::ChapterInfo());
		UInt32 dataCnt = *(UInt32*)&chap[0];
		UInt32 frameNum;
		UInt32 chapOfst;
		UInt32 chapTime;
		i = 0;
		while (i < dataCnt)
		{
			frameNum = ReadUInt32(&chap[i * 8 + 4]);
			chapOfst = ReadUInt32(&chap[i * 8 + 8]);
			
			chapTime = MulDivU32(frameNum, scale * 1000, rate);
			if (chap[chapOfst] == 0xff && chap[chapOfst + 1] == 0xfe)
			{
				Text::StrUTF16_UTF8(sbuff, (UTF16Char*)&chap[chapOfst + 2]);
				chapters->AddChapter(chapTime, sbuff, 0);
			}
			else
			{
				enc.UTF8FromBytes(sbuff, &chap[chapOfst], Text::StrCharCnt(&chap[chapOfst]), 0);
				chapters->AddChapter(chapTime, sbuff, 0);
			}
			i++;
		}

		MemFree(chap);
		mf->SetChapterInfo(chapters, true);
	}

	i = avih->dwStreams;
	while (i-- > 0)
	{
		if (strl[i].strf)
		{
			MemFree(strl[i].strf);
		}
		if (strl[i].others)
		{
			MemFree(strl[i].others);
		}
	}
	MemFree(avih);
	MemFree(strl);
	return mf;
}

UInt32 Parser::FileParser::AVIParser::EstimateDecodeSize(Media::AudioFormat *fmt, UInt64 totalSize, UOSInt frameSize)
{
	if (fmt->formatId == 1)
	{
		return (UInt32)(frameSize / fmt->align);
	}
	if (fmt->formatId == 255)
	{
		return 1024;
	}
	UInt32 lastSize = (UInt32)((totalSize - frameSize) * fmt->frequency / (fmt->bitRate >> 3));
	return (UInt32)(totalSize * fmt->frequency / (fmt->bitRate >> 3)) - lastSize;
}
