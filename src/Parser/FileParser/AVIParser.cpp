#include "Stdafx.h"
#include "MyMemory.h"
#include "Data/ArrayListInt64.h"
#include "Data/ArrayListUInt32.h"
#include "Data/ArrayListUInt64.h"
#include "Data/ByteBuffer.h"
#include "Data/ByteTool.h"
#include "IO/StmData/BlockStreamData.h"
#include "Media/AudioFrameSource.h"
#include "Media/FileVideoSource.h"
#include "Media/LPCMSource.h"
#include "Media/MediaFile.h"
#include "Parser/FileParser/AVIParser.h"
#include "Text/Encoding.h"
#include "Text/MyStringW.h"

#if defined(_WIN32) || defined(_WIN64)
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

void Parser::FileParser::AVIParser::PrepareSelector(NotNullPtr<IO::FileSelector> selector, IO::ParserType t)
{
	if (t == IO::ParserType::Unknown || t == IO::ParserType::MediaFile)
	{
		selector->AddFilter(CSTR("*.avi"), CSTR("AVI File"));
	}
}

IO::ParserType Parser::FileParser::AVIParser::GetParserType()
{
	return IO::ParserType::MediaFile;
}

IO::ParsedObject *Parser::FileParser::AVIParser::ParseFileHdr(NotNullPtr<IO::StreamData> fd, IO::PackageFile *pkgFile, IO::ParserType targetType, const UInt8 *hdr)
{
	UTF8Char sbuff[512];
	UTF8Char *sptr;
	UInt8 chunkBuffer[12];
	UInt64 offset;
//	Int32 hdrlSize;
	UInt32 i;
	UInt32 j;
	UInt32 k;
	UInt32 l;
	UInt16 wLongsPerEntry;
	Int32 cmpTmp;
	Media::MediaFile *mf;
	NotNullPtr<Text::String> audsName;

	UInt32 rate = 30000;
	UInt32 scale = 1001;

	UInt64 base = 0;
	MyAVIHeader *avih = 0;
	AVIStream *strl = 0;
//	Int64 frameCnt;
	Data::ByteBuffer idx1;
	Data::ByteBuffer info;
	UInt8 *indx = 0;
	Data::ByteBuffer chap;

	if (*(Int32*)&hdr[0] != *(Int32*)"RIFF" || *(Int32*)&hdr[8] != *(Int32*)"AVI ")
		return 0;
	if (*(Int32*)&hdr[12] != *(Int32*)"LIST" || *(Int32*)&hdr[20] != *(Int32*)"hdrl")
	{
		return 0;
	}
//	hdrlSize = *(Int32*)&chunkBuffer[4];

	offset = 24;
	offset += fd->GetRealData(offset, 8, BYTEARR(chunkBuffer));
	if (*(Int32*)chunkBuffer != *(Int32*)"avih")
	{
		return 0;
	}

	Text::Encoding enc(this->codePage);
	avih = (MyAVIHeader*)MAlloc(i = ReadUInt32(&chunkBuffer[4]));
	offset += fd->GetRealData(offset, i, Data::ByteArray((UInt8*)avih, i));

	offset += fd->GetRealData(offset, 12, BYTEARR(chunkBuffer));
	strl = MemAlloc(AVIStream, i = avih->dwStreams);
	l = 0;
	while (*(Int32*)chunkBuffer == *(Int32*)"LIST" && *(Int32*)&chunkBuffer[8] == *(Int32*)"strl")
	{
		Data::ByteBuffer buffer(i = ReadUInt32(&chunkBuffer[4]) - 4);
		offset += fd->GetRealData(offset, i, buffer);
		if (buffer.ReadI32(0) != *(Int32*)"strh")
		{
			l += 1;
			continue;
		}
		MemCopyNO(&strl[l].strh, &buffer[8], ReadUInt32(&buffer[4]));

		j = 8 + ReadUInt32(&buffer[4]);
		if (*(Int32*)&buffer[j] != *(Int32*)"strf")
		{
			l += 1;
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

		offset += fd->GetRealData(offset, 12, BYTEARR(chunkBuffer));
		l += 1;
	}

	while (offset <= ReadUInt32(&hdr[4]) + 4)
	{
		if (*(Int32*)chunkBuffer == *(Int32*)"idx1")
		{
			offset -= 8;
			idx1.ChangeSize(i = ReadUInt32(&chunkBuffer[4]) + 4);
			offset += fd->GetRealData(offset, i, idx1);
		}
		else if (*(Int32*)chunkBuffer == *(Int32*)"LIST")
		{
			if (*(Int32*)&chunkBuffer[8] == *(Int32*)"INFO")
			{
				info.ChangeSize(i = ReadUInt32(&chunkBuffer[4]));
				WriteUInt32(&info[0], i);
				offset += fd->GetRealData(offset, i - 4, info.SubArray(4));
			}
			else if (*(Int32*)&chunkBuffer[8] == *(Int32*)"odml")
			{
				Data::ByteBuffer buffer(i = ReadUInt32(&chunkBuffer[4]) - 4);
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
			chap.ChangeSize(i = ReadUInt32(&chunkBuffer[4]));
			offset += fd->GetRealData(offset, i, chap);
			offset += offset & 1;
		}
		else
		{
			offset += ReadUInt32(&chunkBuffer[4]) - 4;
		}
		offset += (i = (UInt32)fd->GetRealData(offset, 12, BYTEARR(chunkBuffer)));
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
					sptr = enc.UTF8FromBytes(sbuff, &strl[i].others[j + 8], ReadUInt32(&strl[i].others[j + 4]), 0);
					audsName = Text::String::NewP(sbuff, sptr);
					j += *(UInt32*)&strl[i].others[j + 4] + 8;
				}
				else if (*(Int32*)&strl[i].others[j] == *(Int32*)"indx")
				{
					idx1.Delete();
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
						Data::ByteBuffer buffer(l = ReadUInt32(&indx[(k << 4) + 32]));
						if (l > fd->GetRealData(ReadUInt64(&indx[(k << 4) + 24]), l, buffer))
						{
							break;
						}

						if (ReadUInt32(&buffer[4]) != ReadUInt32(&indx[(k << 4) + 32]) - 8 && ReadUInt32(&buffer[4]) != ReadUInt32(&indx[(k << 4) + 32]))
						{
							error = true;
							break;
						}
						if (buffer[10])
						{
							error = true;
							break;
						}
						wLongsPerEntry = ReadUInt16(&buffer[8]);
						if (wLongsPerEntry != 2)
						{
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

			if (!idx1.IsNull())
			{
				k = 4;
				l = ReadUInt32(&idx1[0]);
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
				info.dispSize = Math::Size2D<UOSInt>((UInt32)bmih->biWidth, (UInt32)bmih->biHeight);
				info.storeSize = info.dispSize;
				info.fourcc = bmih->biCompression;
				info.storeBPP = bmih->biBitCount;
				info.pf = Media::PixelFormatGetDef(bmih->biCompression, bmih->biBitCount);
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
				info.color.GetRTranParam()->Set(ttype, 2.2);
				info.color.GetGTranParam()->Set(ttype, 2.2);
				info.color.GetBTranParam()->Set(ttype, 2.2);
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
				info.rotateType = Media::RotateType::None;

				Media::FileVideoSource *vstm;
				NEW_CLASS(vstm, Media::FileVideoSource(fd, info, strl[i].strh.dwRate, strl[i].strh.dwScale, false));
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
			Int32 audDelay = 0;
			Bool error;

			fmt.FromWAVEFORMATEX(strl[i].strf);

			audsName = Text::String::NewEmpty();
			error = false;
			j = 0;
			while (j < strl[i].otherSize)
			{
				if (*(Int32*)&strl[i].others[j] == *(Int32*)"strn")
				{
					sptr = enc.UTF8FromBytes(sbuff, &strl[i].others[j + 8], ReadUInt32(&strl[i].others[j + 4]), 0);
					audsName->Release();
					audsName = Text::String::NewP(sbuff, sptr);
					j += *(UInt32*)&strl[i].others[j + 4] + 8;
				}
				else if (*(Int32*)&strl[i].others[j] == *(Int32*)"indx")
				{
					idx1.Delete();

					UInt64 totalSize = 0;
					IO::StmData::BlockStreamData *blkData = 0;

					if (fmt.formatId == 1)
					{
						NEW_CLASS(blkData, IO::StmData::BlockStreamData(fd));
					}
					else
					{
						NEW_CLASS(audsData, Media::AudioFrameSource(fd, fmt, audsName));
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
						Data::ByteBuffer buffer(l = ReadUInt32(&indx[(k << 4) + 32]));
						if (l > fd->GetRealData(ReadUInt64(&indx[(k << 4) + 24]), l, buffer))
						{
							break;
						}

						if (*(Int32*)&buffer[4] != *(Int32*)&indx[(k << 4) + 32] - 8 && *(Int32*)&buffer[4] != *(Int32*)&indx[(k << 4) + 32])
						{
							error = true;
							break;
						}
						if (buffer[10])
						{
							error = true;
							break;
						}
						wLongsPerEntry = *(UInt16*)&buffer[8];
						if (wLongsPerEntry != 2)
						{
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
						k++;
					}
					NotNullPtr<IO::StreamData> fd;
					if(fmt.formatId == 1 && fd.Set(blkData))
					{
						NEW_CLASS(lpcmData, Media::LPCMSource(fd, 0, blkData->GetDataSize(), fmt, audsName));
						fd.Delete();
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
			else if (audsData == 0 && lpcmData == 0 && !idx1.IsNull())
			{
				UInt64 totalSize = 0;
				if (fmt.formatId == 1)
				{
					IO::StmData::BlockStreamData blkData(fd);
					k = 4;
					l = ReadUInt32(&idx1[0]);
					cmpTmp = *(Int32*)"00wb";
					((Char*)&cmpTmp)[1] = (Char)('0' + (i % 10));
					*(Char*)&cmpTmp = (Char)('0' + (i / 10));
					while (k < l)
					{
						if (*(Int32*)&idx1[k] == cmpTmp)
							blkData.Append(base + ReadUInt32(&idx1[k + 8]) + 4, ReadUInt32(&idx1[k + 12]));
						k += 16;
					}
					NEW_CLASS(lpcmData, Media::LPCMSource(blkData, 0,  blkData.GetDataSize(), fmt, audsName));
				}
				else
				{
					NEW_CLASS(audsData, Media::AudioFrameSource(fd, fmt, audsName));
					k = 4;
					l = ReadUInt32(&idx1[0]);
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
			audsName->Release();
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


	if (!chap.IsNull())
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
				sptr = Text::StrUTF16_UTF8(sbuff, (UTF16Char*)&chap[chapOfst + 2]);
				chapters->AddChapter(chapTime, CSTRP(sbuff, sptr), CSTR_NULL);
			}
			else
			{
				sptr = enc.UTF8FromBytes(sbuff, &chap[chapOfst], Text::StrCharCnt(&chap[chapOfst]), 0);
				chapters->AddChapter(chapTime, CSTRP(sbuff, sptr), CSTR_NULL);
			}
			i++;
		}

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
