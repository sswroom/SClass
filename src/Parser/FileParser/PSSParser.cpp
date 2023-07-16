#include "Stdafx.h"
#include "MyMemory.h"
#include "Data/ByteBuffer.h"
#include "Data/ByteTool.h"
#include "IO/StmData/BlockStreamData.h"
#include "IO/StmData/ConcatStreamData.h"
#include "IO/StmData/FileData.h"
#include "Media/AudioFixBlockSource.h"
#include "Media/FileVideoSource.h"
#include "Media/LPCMSource.h"
#include "Media/MediaFile.h"
#include "Media/MPEGVideoParser.h"
#include "Media/BlockParser/AC3BlockParser.h"
#include "Media/BlockParser/MP2BlockParser.h"
#include "Media/BlockParser/MP3BlockParser.h"
#include "Media/Decoder/MP2GDecoder.h"
#include "Parser/FileParser/PSSParser.h"

Parser::FileParser::PSSParser::PSSParser()
{
}

Parser::FileParser::PSSParser::~PSSParser()
{
}

Int32 Parser::FileParser::PSSParser::GetName()
{
	return *(Int32*)"PSSP";
}

void Parser::FileParser::PSSParser::PrepareSelector(IO::FileSelector *selector, IO::ParserType t)
{
	if (t == IO::ParserType::Unknown || t == IO::ParserType::MediaFile)
	{
		selector->AddFilter(CSTR("*.pss"), CSTR("PSS File"));
	}
}

IO::ParserType Parser::FileParser::PSSParser::GetParserType()
{
	return IO::ParserType::MediaFile;
}

IO::ParsedObject *Parser::FileParser::PSSParser::ParseFileHdr(IO::StreamData *fd, IO::PackageFile *pkgFile, IO::ParserType targetType, const UInt8 *hdr)
{
	UTF8Char sbuff[512];
	UTF8Char *sptr2;
	Bool v1;

	if (*(Int32*)&hdr[0] != (Int32)0xba010000)
		return 0;
	if ((hdr[4] & 0xc0) == 0x40)
	{
		v1 = false;
	}
	else if ((hdr[4] & 0xf0) == 0x20)
	{
		v1 = true;
	}
	else
	{
		return 0;
	}
	UTF8Char *sptr;
	Bool valid = true;
	Media::AudioFormat *formats[4];
	Int32 audDelay[4];
	Media::FileVideoSource *vstm = 0;
	IO::StmData::BlockStreamData *stmData[4];
	Int32 stmId;
	Int64 initScr = -1;
	Int64 last_scr_base = -1;
//	Int64 scr_base_diff = -1;
//	Int64 last_dts = -1;
	Int32 lastFrameTime = -1000;
	Bool resync = false;
	Int64 scr_base = 0;
//	Int32 scr_ext = 0;

	Media::FrameInfo frameInfo;
	IO::StreamData *concatFile = 0;
	UInt32 i;
	UInt64 currOfst;

	stmData[0] = 0;
	stmData[1] = 0;
	stmData[2] = 0;
	stmData[3] = 0;
	if (v1)
	{
		currOfst = 12;
	}
	else
	{
		i = (hdr[13] & 7);
		currOfst = 14 + i;
	}
	if (*(Int32*)&hdr[currOfst] != (Int32)0xbb010000)
		return 0;

	if (fd->GetFullFileName()->EndsWithICase(UTF8STRC("_1.vob")))
	{
		if (fd->IsFullFile())
		{
			IO::StmData::ConcatStreamData *data;
			stmId = 2;
			NEW_CLASS(data, IO::StmData::ConcatStreamData(fd->GetFullName()));
			data->AddData(fd->GetPartialData(0, fd->GetDataSize()));
			
			NotNullPtr<Text::String> s = fd->GetFullFileName();
			sptr = Text::StrConcatC(sbuff, s->v, s->leng - 5);
			while (true)
			{
				sptr2 = Text::StrConcatC(Text::StrInt32(sptr, stmId), UTF8STRC(".vob"));
				NEW_CLASS(concatFile, IO::StmData::FileData(CSTRP(sbuff, sptr2), false));
				if (concatFile->GetDataSize() <= 0)
				{
					DEL_CLASS(concatFile);
					break;
				}
				data->AddData(concatFile);
				stmId++;
			}
			concatFile = data;
			fd = data;
		}
		else if (pkgFile)
		{
			sptr = fd->GetShortName().ConcatTo(sbuff) - 5;
/*			IO::StmData::ConcatStreamData *data;
			stmId = 2;
			NEW_CLASS(data, IO::StmData::ConcatStreamData(fd->GetFullName()));
			data->AddData(fd->GetPartialData(0, fd->GetDataSize()));
			
			sptr = Text::StrConcat(sbuff, fd->GetFullFileName()) - 5;
			while (true)
			{
				Text::StrConcat(Text::StrInt32(sptr, stmId), L".vob");
				NEW_CLASS(concatFile, IO::StmData::FileData(sbuff, false));
				if (concatFile->GetDataSize() <= 0)
				{
					DEL_CLASS(concatFile);
					break;
				}
				data->AddData(concatFile);
				stmId++;
			}
			concatFile = data;
			fd = data;*/
		}
	}

	i = 4;
	while (i-- > 0)
	{
		NEW_CLASS(formats[i], Media::AudioFormat());
		formats[i]->formatId = 0;
		audDelay[i] = 0;
	}
	i = ReadMUInt16(&hdr[currOfst + 4]);
	currOfst += 6 + i;
	UInt8 buff[256];
	while (true)
	{
		if (fd->GetRealData(currOfst, 256, BYTEARR(buff)) < 4)
		{
//			valid = false;
			break;
		}
		if (buff[0] != 0 || buff[1] != 0 || buff[2] != 1)
		{
			Bool found = false;
			i = 0;
			while (i < 253)
			{
				if (buff[i] == 0 && buff[i + 1] == 0 && buff[i + 2] == 1)
				{
					currOfst += i;
					found = true;
					break;
				}
				i++;
			}
			if (!found)
			{
				currOfst += 256;
			}
//			valid = false;
//			break;
			continue;
		}
		if (buff[3] == 0xB9) //End Of File
			break;
		if (buff[3] == 0xba) 
		{
			if (v1)
			{
				scr_base = (((Int64)(buff[4] & 0xe)) << 29) | (buff[5] << 22) | ((buff[6] & 0xfe) << 14) | (buff[7] << 7) | ((buff[8] & 0xfe) >> 1);
//				scr_ext = 0;
				currOfst += 12;
			}
			else
			{
				scr_base = (((Int64)(buff[4] & 0x38)) << 27) | ((buff[4] & 3) << 28) | (buff[5] << 20) | ((buff[6] & 0xf8) << 12) | ((buff[6] & 3) << 13) | (buff[7] << 5) | (buff[8] >> 3);
//				scr_ext = ((buff[8] & 3) << 7) | (buff[9] >> 1);
				currOfst += (UOSInt)(14 + (UInt32)(buff[13] & 7));
			}
			if (initScr == -1)
			{
				initScr = scr_base;
			}
			if (last_scr_base < 0 || scr_base < last_scr_base || scr_base > last_scr_base + 90000)
			{
				resync = true;
			}
			else
			{
//				scr_base_diff = scr_base - last_scr_base;
			}
			last_scr_base = scr_base;
		}
		else
		{
			i = ReadMUInt16(&buff[4]);
			if (buff[3] == 0xbd) //Private stream 1
			{
				Int64 pts = 0;
				Int64 dts = 0;
				UInt8 stmHdrSize = buff[8];
				if ((buff[7] & 0xc0) == 0x80)
				{
					pts = (((Int64)(buff[9] & 0xe)) << 29) | (buff[10] << 22) | ((buff[11] & 0xfe) << 14) | (buff[12] << 7) | (buff[13] >> 1);
					dts = pts;
				}
				else if ((buff[7] & 0xc0) == 0xc0)
				{
					pts = (((Int64)(buff[9] & 0xe)) << 29) | (buff[10] << 22) | ((buff[11] & 0xfe) << 14) | (buff[12] << 7) | (buff[13] >> 1);
					dts = (((Int64)(buff[14] & 0xe)) << 29) | (buff[15] << 22) | ((buff[16] & 0xfe) << 14) | (buff[17] << 7) | (buff[18] >> 1);
				}

				UInt8 stmType = buff[9 + stmHdrSize];
				if (stmType == 0xff)
				{
					stmId = buff[0x16];
					if (*(Int32*)&buff[0x17] == 0x64685353)
					{
						if (formats[stmId]->formatId == 0)
						{
							if (buff[0x14] == 0xa0)
							{
								formats[stmId]->formatId = 1;
								formats[stmId]->frequency = ReadUInt32(&buff[0x23]);
								formats[stmId]->nChannels = (UInt16)ReadUInt32(&buff[0x27]);
								formats[stmId]->bitpersample = 16;
								formats[stmId]->bitRate = formats[stmId]->frequency * formats[stmId]->nChannels << 4;
								formats[stmId]->align = (UInt32)(formats[stmId]->nChannels << 1);
								formats[stmId]->other = ReadUInt32(&buff[0x2b]) >> 1;
								formats[stmId]->intType = Media::AudioFormat::IT_NORMAL;
								formats[stmId]->extraSize = 0;
								formats[stmId]->extra = 0;
								NEW_CLASS(stmData[stmId], IO::StmData::BlockStreamData(fd));
								stmData[stmId]->Append(currOfst + 0x3F, i - 0x39);
								audDelay[stmId] = (Int32)((dts - initScr) / 90);
							}
							else if (buff[0x14] == 0xa1)
							{
								formats[stmId]->formatId = 0x2081;
								formats[stmId]->frequency = ReadUInt32(&buff[0x23]);
								formats[stmId]->nChannels = (UInt16)ReadUInt32(&buff[0x27]);
								formats[stmId]->bitpersample = 0;
								formats[stmId]->bitRate = (formats[stmId]->frequency * formats[stmId]->nChannels * 8 / 14) << 3;
								formats[stmId]->align = formats[stmId]->nChannels * ReadUInt32(&buff[0x2b]);
								formats[stmId]->other = 0;
								formats[stmId]->intType = Media::AudioFormat::IT_NORMAL;
								formats[stmId]->extraSize = 0;
								formats[stmId]->extra = 0;
								NEW_CLASS(stmData[stmId], IO::StmData::BlockStreamData(fd));
								stmData[stmId]->Append(currOfst + 0x3F, i - 0x39);
								audDelay[stmId] = (Int32)((dts - initScr) / 90);
							}
						}

						if (resync)
						{
							resync = false;
							initScr = dts - 90 * ((Int64)(stmData[stmId]->GetDataSize() * 8000LL / formats[stmId]->bitRate) + audDelay[stmId]);
						}

					}
					else if (formats[stmId]->formatId == 1 || formats[stmId]->formatId == 0x2081)
					{
						if (resync)
						{
							resync = false;
							initScr = dts - 90 * ((Int64)(stmData[stmId]->GetDataSize() * 8000LL / formats[stmId]->bitRate) + audDelay[stmId]);
						}

						stmData[stmId]->Append(currOfst + 0x17, i - 0x11);
					}
				}
				else if (stmType == 0xa0)
				{
					if (buff[10 + stmHdrSize] > 0)
					{
						stmId = 2;
						if (formats[stmId]->formatId == 0)
						{
							formats[stmId]->formatId = 1;
							if (buff[14 + stmHdrSize] == 0x91)
							{
								formats[stmId]->frequency = 96000;
								formats[stmId]->nChannels = 2;
								formats[stmId]->bitpersample = 24;
							}
							else //1
							{
								formats[stmId]->frequency = 48000;
								formats[stmId]->nChannels = 2;
								formats[stmId]->bitpersample = 16;
							}

							formats[stmId]->bitRate = formats[stmId]->frequency * formats[stmId]->nChannels * formats[stmId]->bitpersample;
							formats[stmId]->align = formats[stmId]->nChannels * (UInt32)(formats[stmId]->bitpersample >> 3);
							formats[stmId]->other = 0;
							formats[stmId]->intType = Media::AudioFormat::IT_BIGENDIAN16;
							formats[stmId]->extraSize = 0;
							formats[stmId]->extra = 0;
							NEW_CLASS(stmData[stmId], IO::StmData::BlockStreamData(fd));
							audDelay[stmId] = (Int32)((dts - initScr) / 90);
						}

						if (resync)
						{
							resync = false;
							initScr = dts - 90 * ((Int64)(stmData[stmId]->GetDataSize() * 8000LL / formats[stmId]->bitRate) + audDelay[stmId]);
						}

						stmData[stmId]->Append(currOfst + 16 + stmHdrSize, i - 10 - stmHdrSize);
					}
					else
					{
						stmId = 1;
					}
				}
				else if (stmType == 0x80)
				{
					stmId = 2;
					if (formats[stmId]->formatId == 0)
					{
						formats[stmId]->formatId = 0x2000;
						formats[stmId]->bitRate = 0;
						NEW_CLASS(stmData[stmId], IO::StmData::BlockStreamData(fd));
						audDelay[stmId] = (Int32)((dts - initScr) / 90);

						if (resync)
						{
							resync = false;
						}
					}

					if (resync && formats[stmId]->bitRate)
					{
						resync = false;
						initScr = dts - 90 * ((Int64)(stmData[stmId]->GetDataSize() * 8000LL / formats[stmId]->bitRate) + audDelay[stmId]);
					}
					stmData[stmId]->Append(currOfst + 13 + stmHdrSize, i - 7 - stmHdrSize);
					if (formats[stmId]->bitRate == 0)
					{
						Media::BlockParser::AC3BlockParser ac3Parser;
						Data::ByteBuffer frameBuff(i - 7 - stmHdrSize);
						fd->GetRealData(currOfst + 13 + stmHdrSize, i - 7 - stmHdrSize, frameBuff);
						ac3Parser.ParseStreamFormat(frameBuff.Ptr(), i - 7 - stmHdrSize, formats[stmId]);
					}
				}
				else
				{
					stmId = 2;
				}
			}
			else if ((buff[3] & 0xe0) == 0xc0) //Audio stream
			{
				Int64 pts = 0;
				Int64 dts = 0;
				UOSInt j;
				j = 6;
				while (buff[j] & 0x80)
				{
					j++;
				}
				if ((buff[j] & 0xc0) == 0x40)
				{
					j += 2;
				}

				if ((buff[j] & 0xf0) == 0x20)
				{
					pts = (((Int64)buff[j] & 0xe) << 29) | (buff[j + 1] << 22) | ((buff[j + 2] & 0xfe) << 14) | (buff[j + 3] << 7) | (buff[j + 4] >> 1);;
					j += 5;
				}
				else if ((buff[j] & 0xf0) == 0x30)
				{
					pts = (((Int64)buff[j] & 0xe) << 29) | (buff[j + 1] << 22) | ((buff[j + 2] & 0xfe) << 14) | (buff[j + 3] << 7) | (buff[j + 4] >> 1);;
					dts = (((Int64)buff[j + 5] & 0xe) << 29) | (buff[j + 6] << 22) | ((buff[j + 7] & 0xfe) << 14) | (buff[j + 8] << 7) | (buff[j + 9] >> 1);;
					j += 10;
				}
				else if (buff[j] == 0xf)
				{
					j++;
				}
				else
				{
					valid = false;
					break;
				}

				stmId = buff[3] & 0x1f;
				if (formats[stmId]->formatId == 0)
				{
					UInt32 v = ReadMUInt32(&buff[j]);
					if ((v & 0x80000000) != 0 && (v & 0x7fffffff) <= 2048)
					{
						formats[stmId]->formatId = 0x2080;
						formats[stmId]->nChannels = buff[j + 7];
						formats[stmId]->frequency = ReadMUInt32(&buff[j + 8]);
						formats[stmId]->bitpersample = 16;
						formats[stmId]->bitRate = (formats[stmId]->frequency * formats[stmId]->nChannels * 9) >> 1;
						formats[stmId]->align = (UInt32)(18 * formats[stmId]->nChannels);
						formats[stmId]->other = 0;
						formats[stmId]->intType = Media::AudioFormat::IT_NORMAL;
						formats[stmId]->extraSize = 0;
						formats[stmId]->extra = 0;
						NEW_CLASS(stmData[stmId], IO::StmData::BlockStreamData(fd));
						stmData[stmId]->Append(currOfst + j + 4 + (v & 0x7fffffff), (UInt32)(i - j + 2 - (v & 0x7fffffff)));
						audDelay[stmId] = (Int32)((pts - initScr) / 90);

						if (resync)
							resync = false;
					}
					else if (buff[j] == 0xff && (buff[j + 1] & 0xfe) == 0xfc)
					{
						formats[stmId]->formatId = 0x50;
						formats[stmId]->bitRate = 0;
						NEW_CLASS(stmData[stmId], IO::StmData::BlockStreamData(fd));
						stmData[stmId]->Append(currOfst + j, (UInt32)(i - j + 6));
						audDelay[stmId] = (Int32)((pts - initScr) / 90);

						if (resync)
							resync = false;

						{
							Media::BlockParser::MP2BlockParser mp2Parser;
							mp2Parser.ParseStreamFormat(&buff[j], i - j + 6, formats[stmId]);
						}
					}
					else if (buff[j] == 0xff && (buff[j + 1] & 0xfe) == 0xfa)
					{
						formats[stmId]->formatId = 0x55;
						formats[stmId]->bitRate = 224000;
						NEW_CLASS(stmData[stmId], IO::StmData::BlockStreamData(fd));
						stmData[stmId]->Append(currOfst + j, (UInt32)(i - j + 6));
						audDelay[stmId] = (Int32)((pts - initScr) / 90);

						if (resync)
							resync = false;

						{
							Media::BlockParser::MP3BlockParser mp3Parser;
							mp3Parser.ParseStreamFormat(&buff[j], i - j + 6, formats[stmId]);
						}
					}
				}
				else
				{
					if (resync)
					{
						resync = false;
						initScr = dts - 90 * ((Int64)(stmData[stmId]->GetDataSize() * 8000LL / formats[stmId]->bitRate) + audDelay[stmId]);
					}

					stmData[stmId]->Append(currOfst + j, (UInt32)(i - j + 6));
				}
			}
			else if ((buff[3] & 0xf0) == 0xe0) //Video stream
			{
				Bool isFrame = false;
				Int64 pts = 0;
				Int64 dts = 0;
				UOSInt stmHdrSize;
				Bool hasDTS = false;
				if (v1)
				{
					UOSInt buffOfst = 6;
					while (buff[buffOfst] & 0x80)
					{
						buffOfst++;
					}
					if (buff[buffOfst] & 0xc0)
					{
						buffOfst += 2;
					}
					if ((buff[buffOfst] & 0xf0) == 0x20)
					{
						pts = (((Int64)(buff[buffOfst] & 0xe)) << 29) | (buff[buffOfst + 1] << 22) | ((buff[buffOfst + 2] & 0xfe) << 14) | (buff[buffOfst + 3] << 7) | (buff[buffOfst + 4] >> 1);
						dts = pts;
						buffOfst += 5;
						hasDTS = true;
					}
					else if ((buff[buffOfst] & 0xf0) == 0x30)
					{
						pts = (((Int64)(buff[buffOfst] & 0xe)) << 29) | (buff[buffOfst + 1] << 22) | ((buff[buffOfst + 2] & 0xfe) << 14) | (buff[buffOfst + 3] << 7) | (buff[buffOfst + 4] >> 1);
						dts = (((Int64)(buff[buffOfst + 5] & 0xe)) << 29) | (buff[buffOfst + 6] << 22) | ((buff[buffOfst + 7] & 0xfe) << 14) | (buff[buffOfst + 8] << 7) | (buff[buffOfst + 9] >> 1);
						buffOfst += 10;
						hasDTS = true;
					}
					else
					{
						buffOfst++;
					}
					stmHdrSize = buffOfst - 9;
				}
				else
				{
					stmHdrSize = buff[8];
					if ((buff[7] & 0xc0) == 0x80)
					{
						pts = (((Int64)(buff[9] & 0xe)) << 29) | (buff[10] << 22) | ((buff[11] & 0xfe) << 14) | (buff[12] << 7) | (buff[13] >> 1);
						dts = pts;
						hasDTS = true;
					}
					else if ((buff[7] & 0xc0) == 0xc0)
					{
						pts = (((Int64)(buff[9] & 0xe)) << 29) | (buff[10] << 22) | ((buff[11] & 0xfe) << 14) | (buff[12] << 7) | (buff[13] >> 1);
						dts = (((Int64)(buff[14] & 0xe)) << 29) | (buff[15] << 22) | ((buff[16] & 0xfe) << 14) | (buff[17] << 7) | (buff[18] >> 1);
						hasDTS = true;
					}
				}

				if (hasDTS)
				{
					if (buff[9 + stmHdrSize] == 0 && buff[10 + stmHdrSize] == 0 && buff[11 + stmHdrSize] == 1)
					{
						if (buff[12 + stmHdrSize] == 0xb3)
						{
							isFrame = true;

/*							if (resync)
							{
								if (syncAudioTime)
								{
									initScr = dts - syncAudioTime * 90LL;
									resync = false;
								}
								else if (scr_base_diff >= 0)
								{
									initScr = dts - last_dts + initScr - scr_base_diff;
									resync = false;
								}
								else
								{
									resync = false;
								}
							}*/
//							last_dts = dts;
						}
						else if (buff[12 + stmHdrSize] == 0)
						{
							Media::MPEGVideoParser::MPEGFrameProp prop;
							if (Media::MPEGVideoParser::GetFrameProp(&buff[9 + stmHdrSize], 256 - 9 - stmHdrSize, &prop) && prop.pictureCodingType == 'I')
							{
								isFrame = true;

/*								if (resync)
								{
									if (syncAudioTime)
									{
										initScr = dts - syncAudioTime * 90LL;
										resync = false;
									}
									else if (scr_base_diff >= 0)
									{
										initScr = dts - last_dts + initScr - scr_base_diff;
										resync = false;
									}
									else
									{
										resync = false;
									}
								}*/
//								last_dts = dts;
							}
						}
					}

					if (vstm && v1 && !isFrame)
					{
						Media::MPEGVideoParser::MPEGFrameProp prop;
						UOSInt currPtr;
						Int32 srchByte;
						UOSInt frameSize = i - stmHdrSize - 3;
						Data::ByteBuffer frameBuff(frameSize);
						fd->GetRealData(currOfst + 9 + stmHdrSize, frameSize, frameBuff);
						WriteMInt32((UInt8*)&srchByte, 0x00000100);
						currPtr = 0;
						while (currPtr < frameSize - 4)
						{
							if (*(Int32*)&frameBuff[currPtr] == srchByte)
							{
								if (Media::MPEGVideoParser::GetFrameProp(&frameBuff[currPtr], frameSize - currPtr, &prop) && prop.pictureCodingType == 'I')
								{
									isFrame = true;
									vstm->AddFramePart(currOfst + 9 + stmHdrSize, (UInt32)currPtr);
									stmHdrSize += currPtr;
									break;
								}
							}
							currPtr++;
						}
					}
				}

				UInt32 frameRateNorm;
				UInt32 frameRateDenorm;
				if (isFrame && vstm == 0 && Media::MPEGVideoParser::GetFrameInfo(&buff[9 + stmHdrSize], 256 - 9 - stmHdrSize, &frameInfo, &frameRateNorm, &frameRateDenorm, 0, false))
				{
					frameInfo.fourcc = *(UInt32*)"MP2G";
					NEW_CLASS(vstm, Media::FileVideoSource(fd, &frameInfo, frameRateNorm, frameRateDenorm, true));
				}

				if (vstm != 0)
				{
					if (isFrame)
					{
						Int32 frameTime = (Int32)((dts - initScr) / 90);
						if (resync)
						{
							frameTime = lastFrameTime;
						}
						if (frameTime < 0)
							frameTime = 0;
						if (lastFrameTime > frameTime)
						{
							lastFrameTime = 0;
						}
						lastFrameTime = frameTime;
						vstm->AddNewFrame(currOfst + 9 + stmHdrSize, (UInt32)(i - stmHdrSize - 3), true, (UInt32)frameTime);
					}
					else
					{
						vstm->AddFramePart(currOfst + 9 + stmHdrSize, (UInt32)(i - stmHdrSize - 3));
					}
				}

				stmId = 3;
			}
			else if (buff[3] == 0xbc) // program_stream_map
			{
			}
			else if (buff[3] == 0xbe) // padding stream
			{
			}
			else if (buff[3] == 0xbf) // private stream 2
			{
			}
			currOfst += 6 + i;
		}
	}
	if (!valid)
	{
		if (vstm)
		{
			DEL_CLASS(vstm);
		}

		i = 4;
		while (i-- > 0)
		{
			if (stmData[i])
			{
				DEL_CLASS(stmData[i]);
			}
			DEL_CLASS(formats[i]);
		}
		SDEL_CLASS(concatFile);
		return 0;
	}
	Media::MediaFile *file;
	NEW_CLASS(file, Media::MediaFile(fd->GetFullName()));
	if (vstm)
	{
		Media::Decoder::MP2GDecoder *mp2g;
		NEW_CLASS(mp2g, Media::Decoder::MP2GDecoder(vstm, true));
		file->AddSource(mp2g, 0);
	}

	i = 0;
	while (i < 4)
	{
		if (formats[i]->formatId)
		{
			if (formats[i]->formatId == 1)
			{
				Media::IAudioSource *as;
				NEW_CLASS(as, Media::LPCMSource(stmData[i], 0, stmData[i]->GetDataSize(), formats[i], fd->GetFullName()));
				file->AddSource(as, audDelay[i]);
				DEL_CLASS(stmData[i]);
			}
			else if (formats[i]->formatId == 0x50)
			{
				Media::IAudioSource *as;
				Media::BlockParser::MP2BlockParser mp2Parser;
				as = mp2Parser.ParseStreamData(stmData[i]);
				DEL_CLASS(stmData[i]);
				if (as)
				{
					file->AddSource(as, audDelay[i]);
				}
			}
			else if (formats[i]->formatId == 0x55)
			{
				Media::IAudioSource *as;
				Media::BlockParser::MP3BlockParser mp3Parser;
				as = mp3Parser.ParseStreamData(stmData[i]);
				DEL_CLASS(stmData[i]);
				if (as)
				{
					file->AddSource(as, audDelay[i]);
				}
			}
			else if (formats[i]->formatId == 0x2000)
			{
				Media::IAudioSource *as;
				Media::BlockParser::AC3BlockParser ac3Parser;
				as = ac3Parser.ParseStreamData(stmData[i]);
				DEL_CLASS(stmData[i]);
				if (as)
				{
					file->AddSource(as, audDelay[i]);
				}
			}
			else if (formats[i]->formatId == 0x2080)
			{
				Media::IAudioSource *as;
				NEW_CLASS(as, Media::AudioFixBlockSource(stmData[i], 0, stmData[i]->GetDataSize(), formats[i], fd->GetFullName()));
				file->AddSource(as, audDelay[i]);
				DEL_CLASS(stmData[i]);
			}
			else if (formats[i]->formatId == 0x2081)
			{
				Media::IAudioSource *as;
				NEW_CLASS(as, Media::AudioFixBlockSource(stmData[i], 0, stmData[i]->GetDataSize(), formats[i], fd->GetFullName()));
				file->AddSource(as, audDelay[i]);
				DEL_CLASS(stmData[i]);
			}
		}
		DEL_CLASS(formats[i]);
		i++;
	}
	SDEL_CLASS(concatFile);
	return file;
}
