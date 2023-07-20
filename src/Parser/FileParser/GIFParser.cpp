#include "Stdafx.h"
#include "MyMemory.h"
#include "Data/ByteBuffer.h"
#include "Data/ByteBufferA.h"
#include "Data/ByteTool.h"
#include "Data/Compress/LZWDecStream.h"
#include "IO/BitReaderLSB.h"
#include "IO/StreamData.h"
#include "IO/MemoryStream.h"
#include "IO/StreamDataStream.h"
#include "Media/FrameInfo.h"
#include "Media/IImgResizer.h"
#include "Media/ImageList.h"
#include "Media/StaticImage.h"
#include "Parser/FileParser/GIFParser.h"

Parser::FileParser::GIFParser::GIFParser()
{
}

Parser::FileParser::GIFParser::~GIFParser()
{
}

Int32 Parser::FileParser::GIFParser::GetName()
{
	return *(Int32*)"GIFP";
}

void Parser::FileParser::GIFParser::PrepareSelector(IO::FileSelector *selector, IO::ParserType t)
{
	if (t == IO::ParserType::Unknown || t == IO::ParserType::ImageList)
	{
		selector->AddFilter(CSTR("*.gif"), CSTR("GIF File"));
	}
}

IO::ParserType Parser::FileParser::GIFParser::GetParserType()
{
	return IO::ParserType::ImageList;
}

IO::ParsedObject *Parser::FileParser::GIFParser::ParseFileHdr(IO::StreamData *fd, IO::PackageFile *pkgFile, IO::ParserType targetType, const UInt8 *hdr)
{
//	UInt8 ver;

	if (hdr[0] != 'G' || hdr[1] != 'I' || hdr[2] != 'F')
	{
		return 0;
	}
	if (hdr[3] == '8' && hdr[4] == '7' && hdr[5] == 'a')
	{
//		ver = 87;
	}
	else if (hdr[3] == '8' && hdr[4] == '9' && hdr[5] == 'a')
	{
//		ver = 89;
	}
	else
	{
		return 0;
	}

	UOSInt i;
	UOSInt j;
	UOSInt currOfst = 13;
	UInt32 scnWidth = ReadUInt16(&hdr[6]);
	UInt32 scnHeight = ReadUInt16(&hdr[8]);
//	Int32 bpp = ((hdr[10] >> 4) & 7) + 1;
	Int32 colorSize = (hdr[10] & 7) + 1;
//	Int32 colorIndex = -1;
	Int32 transparentIndex = -1;
	UInt32 frameDelay = 0;
	Bool isFirst = true;
	UInt8 blockType;
	UInt8 readBlock[256];
	UOSInt readSize;
	UInt8 disposalMethod = 0;
	Media::ImageList *imgList;
	;
	Data::ByteBufferA scnImg32;
	Data::ByteBuffer globalColorTable;
	Data::ByteBuffer screenColorTable;
	Int32 globalTransparentIndex = -1;
	NEW_CLASS(imgList, Media::ImageList(fd->GetFullName()));
	if (hdr[10] & 0x80)
	{
		globalColorTable.ChangeSize((UOSInt)3 << colorSize);
//		colorIndex = hdr[11];
		fd->GetRealData(currOfst, (UOSInt)3 << colorSize, globalColorTable);
		currOfst += (UOSInt)3 << colorSize;
	}
	Data::ByteBufferA scnImg(scnWidth * scnHeight);
	
	while (true)
	{
		if (fd->GetRealData(currOfst, 1, Data::ByteArray(&blockType, 1)) != 1)
			break;
		currOfst++;
		if (blockType == 0x2c)
		{
			UInt8 imgDesc[9];
			UInt32 left;
			UInt32 top;
			UInt32 imgW;
			UInt32 imgH;
			Data::ByteBuffer localColorTable;
			if (fd->GetRealData(currOfst, 9, BYTEARR(imgDesc)) != 9)
				break;
			currOfst += 9;

			left = ReadUInt16(&imgDesc[0]);
			top = ReadUInt16(&imgDesc[2]);
			imgW = ReadUInt16(&imgDesc[4]);
			imgH = ReadUInt16(&imgDesc[6]);
			if (imgDesc[8] & 0x80)
			{
				Int32 colorSize = (imgDesc[8] & 7) + 1;
				localColorTable.ChangeSize((UOSInt)3 << colorSize);
				fd->GetRealData(currOfst, (UOSInt)3 << colorSize, localColorTable);
				currOfst += (UOSInt)3 << colorSize;
			}

			while (true)
			{
				if (fd->GetRealData(currOfst, 1, Data::ByteArray(&blockType, 1)) != 1)
					break;
				currOfst++;
				if (blockType < 0x20)
				{
					if (disposalMethod == 0)
					{
					}
					else if (disposalMethod == 1)
					{
					}
					else if (disposalMethod == 2)
					{
						disposalMethod = 2;
					}
					else if (disposalMethod == 3)
					{
						disposalMethod = 3;
					}

					IO::MemoryStream mstm;
					while (true)
					{
						readSize = fd->GetRealData(currOfst, 256, BYTEARR(readBlock));
						if (readSize == 0)
							break;
						if (readBlock[0] == 0)
						{
							currOfst += 1;
							break;
						}
						mstm.Write(&readBlock[1], readBlock[0]);
						currOfst += 1 + (UOSInt)readBlock[0];
					}
					mstm.SeekFromBeginning(0);
					Data::Compress::LZWDecStream *lzw;
					Media::StaticImage *simg;
					NEW_CLASS(lzw, Data::Compress::LZWDecStream(&mstm, true, blockType, 12, 0));
					
					Data::ByteArray tmpPtr;
					Data::ByteArray tmpPtr2;
					Data::ByteArray currColorTable;
					OSInt currColorSize;
					if (isFirst)
					{
						if (!localColorTable.IsNull())
						{
							screenColorTable.ReplaceBy(localColorTable);
						}
						globalTransparentIndex = transparentIndex;
					}
					else if (scnImg32.IsNull())
					{
						if (!localColorTable.IsNull())
						{
							scnImg32.ChangeSize(scnWidth * scnHeight * 4);
							tmpPtr = scnImg;
							tmpPtr2 = scnImg32;
							i = scnWidth * scnHeight;
							if (!screenColorTable.IsNull())
							{
								while (i-- > 0)
								{
									j = (UOSInt)tmpPtr[0] * 3;
									tmpPtr2[0] = screenColorTable[j + 2];
									tmpPtr2[1] = screenColorTable[j + 1];
									tmpPtr2[2] = screenColorTable[j + 0];
									if (globalTransparentIndex == tmpPtr[0])
									{
										tmpPtr2[3] = 0;
									}
									else
									{
										tmpPtr2[3] = 0xff;
									}
									tmpPtr++;
									tmpPtr2 += 4;
								}
							}
							else if (!globalColorTable.IsNull())
							{
								while (i-- > 0)
								{
									j = (UOSInt)tmpPtr[0] * 3;
									tmpPtr2[0] = globalColorTable[j + 2];
									tmpPtr2[1] = globalColorTable[j + 1];
									tmpPtr2[2] = globalColorTable[j + 0];
									if (globalTransparentIndex == tmpPtr[0])
									{
										tmpPtr2[3] = 0;
									}
									else
									{
										tmpPtr2[3] = 0xff;
									}
									tmpPtr++;
									tmpPtr2 += 4;
								}
							}
							else
							{
								while (i-- > 0)
								{
									tmpPtr2[0] = tmpPtr[0];
									tmpPtr2[1] = tmpPtr[0];
									tmpPtr2[2] = tmpPtr[0];
									if (globalTransparentIndex == tmpPtr[0])
									{
										tmpPtr2[3] = 0;
									}
									else
									{
										tmpPtr2[3] = 0xff;
									}
									tmpPtr++;
									tmpPtr2 += 4;
								}
							}
						}
					}

					Data::ByteBuffer imgData;
					Media::ColorProfile color(Media::ColorProfile::CPT_PUNKNOWN);
					if (!scnImg32.IsNull())
					{
						NEW_CLASS(simg, Media::StaticImage(Math::Size2D<UOSInt>(scnWidth, scnHeight), 0, 32, Media::PF_B8G8R8A8, scnWidth * scnHeight * 4, &color, Media::ColorProfile::YUVT_UNKNOWN, Media::AT_ALPHA, Media::YCOFST_C_CENTER_LEFT));
						imgData.ChangeSize(imgW * imgH);
						readSize = lzw->Read(imgData);
						if (!localColorTable.IsNull())
						{
							currColorTable = localColorTable;
							currColorSize = (Int32)(1 << ((imgDesc[8] & 7) + 1));
						}
						else if (!globalColorTable.IsNull())
						{
							currColorTable = globalColorTable;
							currColorSize = (Int32)(1 << colorSize);
						}
						else
						{
							currColorTable = Data::ByteArray(0, 0);
							currColorSize = 0;
						}

						if (!currColorTable.IsNull())
						{
							if (imgDesc[8] & 0x40)
							{
								tmpPtr = imgData;
								tmpPtr2 = scnImg32 + top * scnWidth * 4 + left * 4;
								i = 0;
								while (i < imgH)
								{
									j = 0;
									while (j < imgW)
									{
										blockType = tmpPtr[j];
										if (blockType != transparentIndex && blockType < currColorSize)
										{
											tmpPtr2[j * 4 + 0] = currColorTable[blockType * 3 + 2];
											tmpPtr2[j * 4 + 1] = currColorTable[blockType * 3 + 1];
											tmpPtr2[j * 4 + 2] = currColorTable[blockType * 3 + 0];
											tmpPtr2[j * 4 + 3] = 0xff;
										}
										j++;
									}
									tmpPtr += imgW;
									tmpPtr2 += scnWidth << 5;
									i += 8;
								}

								tmpPtr2 = scnImg32 + (top + 4) * scnWidth * 4 + left * 4;
								i = 4;
								while (i < imgH)
								{
									j = 0;
									while (j < imgW)
									{
										blockType = tmpPtr[j];
										if (blockType != transparentIndex && blockType < currColorSize)
										{
											tmpPtr2[j * 4 + 0] = currColorTable[blockType * 3 + 2];
											tmpPtr2[j * 4 + 1] = currColorTable[blockType * 3 + 1];
											tmpPtr2[j * 4 + 2] = currColorTable[blockType * 3 + 0];
											tmpPtr2[j * 4 + 3] = 0xff;
										}
										j++;
									}
									tmpPtr += imgW;
									tmpPtr2 += scnWidth << 5;
									i += 8;
								}

								tmpPtr2 = scnImg32 + (top + 2) * scnWidth * 4 + left * 4;
								i = 2;
								while (i < imgH)
								{
									j = 0;
									while (j < imgW)
									{
										blockType = tmpPtr[j];
										if (blockType != transparentIndex && blockType < currColorSize)
										{
											tmpPtr2[j * 4 + 0] = currColorTable[blockType * 3 + 2];
											tmpPtr2[j * 4 + 1] = currColorTable[blockType * 3 + 1];
											tmpPtr2[j * 4 + 2] = currColorTable[blockType * 3 + 0];
											tmpPtr2[j * 4 + 3] = 0xff;
										}
										j++;
									}
									tmpPtr += imgW;
									tmpPtr2 += scnWidth << 4;
									i += 4;
								}

								tmpPtr2 = scnImg32 + (top + 1) * scnWidth * 4 + left * 4;
								i = 1;
								while (i < imgH)
								{
									j = 0;
									while (j < imgW)
									{
										blockType = tmpPtr[j];
										if (blockType != transparentIndex && blockType < currColorSize)
										{
											tmpPtr2[j * 4 + 0] = currColorTable[blockType * 3 + 2];
											tmpPtr2[j * 4 + 1] = currColorTable[blockType * 3 + 1];
											tmpPtr2[j * 4 + 2] = currColorTable[blockType * 3 + 0];
											tmpPtr2[j * 4 + 3] = 0xff;
										}
										j++;
									}
									tmpPtr += imgW;
									tmpPtr2 += scnWidth << 3;
									i += 2;
								}
							}
							else
							{
								tmpPtr = imgData;
								tmpPtr2 = scnImg32 + top * scnWidth * 4 + left * 4;
								i = 0;
								while (i < imgH)
								{
									j = 0;
									while (j < imgW)
									{
										blockType = tmpPtr[j];
										if (blockType != transparentIndex && blockType < currColorSize)
										{
											tmpPtr2[j * 4 + 0] = currColorTable[blockType * 3 + 2];
											tmpPtr2[j * 4 + 1] = currColorTable[blockType * 3 + 1];
											tmpPtr2[j * 4 + 2] = currColorTable[blockType * 3 + 0];
											tmpPtr2[j * 4 + 3] = 0xff;
										}
										j++;
									}
									tmpPtr += imgW;
									tmpPtr2 += scnWidth << 2;
									i++;
								}
							}
						}
						else
						{
							if (imgDesc[8] & 0x40)
							{
								tmpPtr = imgData;
								tmpPtr2 = scnImg32 + top * scnWidth * 4 + left * 4;
								i = 0;
								while (i < imgH)
								{
									j = 0;
									while (j < imgW)
									{
										blockType = tmpPtr[j];
										if (blockType != transparentIndex)
										{
											tmpPtr2[j * 4 + 0] = blockType;
											tmpPtr2[j * 4 + 1] = blockType;
											tmpPtr2[j * 4 + 2] = blockType;
											tmpPtr2[j * 4 + 3] = 0xff;
										}
										j++;
									}
									tmpPtr += imgW;
									tmpPtr2 += scnWidth << 5;
									i += 8;
								}

								tmpPtr2 = scnImg32 + (top + 4) * scnWidth * 4 + left * 4;
								i = 4;
								while (i < imgH)
								{
									j = 0;
									while (j < imgW)
									{
										blockType = tmpPtr[j];
										if (blockType != transparentIndex)
										{
											tmpPtr2[j * 4 + 0] = blockType;
											tmpPtr2[j * 4 + 1] = blockType;
											tmpPtr2[j * 4 + 2] = blockType;
											tmpPtr2[j * 4 + 3] = 0xff;
										}
										j++;
									}
									tmpPtr += imgW;
									tmpPtr2 += scnWidth << 5;
									i += 8;
								}

								tmpPtr2 = scnImg32 + (top + 2) * scnWidth * 4 + left * 4;
								i = 2;
								while (i < imgH)
								{
									j = 0;
									while (j < imgW)
									{
										blockType = tmpPtr[j];
										if (blockType != transparentIndex)
										{
											tmpPtr2[j * 4 + 0] = blockType;
											tmpPtr2[j * 4 + 1] = blockType;
											tmpPtr2[j * 4 + 2] = blockType;
											tmpPtr2[j * 4 + 3] = 0xff;
										}
										j++;
									}
									tmpPtr += imgW;
									tmpPtr2 += scnWidth << 4;
									i += 4;
								}

								tmpPtr2 = scnImg32 + (top + 1) * scnWidth * 4 + left * 4;
								i = 1;
								while (i < imgH)
								{
									j = 0;
									while (j < imgW)
									{
										blockType = tmpPtr[j];
										if (blockType != transparentIndex)
										{
											tmpPtr2[j * 4 + 0] = blockType;
											tmpPtr2[j * 4 + 1] = blockType;
											tmpPtr2[j * 4 + 2] = blockType;
											tmpPtr2[j * 4 + 3] = 0xff;
										}
										j++;
									}
									tmpPtr += imgW;
									tmpPtr2 += scnWidth << 3;
									i += 2;
								}
							}
							else
							{
								tmpPtr = imgData;
								tmpPtr2 = scnImg32 + top * scnWidth * 4 + left * 4;
								i = 0;
								while (i < imgH)
								{
									j = 0;
									while (j < imgW)
									{
										blockType = tmpPtr[j];
										if (blockType != transparentIndex)
										{
											tmpPtr2[j * 4 + 0] = blockType;
											tmpPtr2[j * 4 + 1] = blockType;
											tmpPtr2[j * 4 + 2] = blockType;
											tmpPtr2[j * 4 + 3] = 0xff;
										}
										j++;
									}
									tmpPtr += imgW;
									tmpPtr2 += scnWidth << 2;
									i++;
								}
							}
						}
						MemCopyANC(simg->data, scnImg32.Ptr(), scnWidth * scnHeight * 4);
					}
					else
					{
						NEW_CLASS(simg, Media::StaticImage(Math::Size2D<UOSInt>(scnWidth, scnHeight), 0, 8, Media::PF_PAL_8, scnWidth * scnHeight, &color, Media::ColorProfile::YUVT_UNKNOWN, (globalTransparentIndex == -1)?Media::AT_NO_ALPHA:Media::AT_ALPHA, Media::YCOFST_C_CENTER_LEFT));
						if (imgDesc[8] & 0x40)
						{
							imgData.ChangeSize(imgW * imgH);
							readSize = lzw->Read(imgData);
							tmpPtr = imgData;
							tmpPtr2 = scnImg + top * scnWidth + left;
							i = 0;
							while (i < imgH)
							{
								j = 0;
								while (j < imgW)
								{
									blockType = tmpPtr[j];
									if (isFirst || blockType != transparentIndex)
									{
										tmpPtr2[j] = blockType;
									}
									j++;
								}
								tmpPtr += imgW;
								tmpPtr2 += scnWidth << 3;
								i += 8;
							}

							tmpPtr2 = scnImg + (top + 4) * scnWidth + left;
							i = 4;
							while (i < imgH)
							{
								j = 0;
								while (j < imgW)
								{
									blockType = tmpPtr[j];
									if (isFirst || blockType != transparentIndex)
									{
										tmpPtr2[j] = blockType;
									}
									j++;
								}
								tmpPtr += imgW;
								tmpPtr2 += scnWidth << 3;
								i += 8;
							}

							tmpPtr2 = scnImg + (top + 2) * scnWidth + left;
							i = 2;
							while (i < imgH)
							{
								j = 0;
								while (j < imgW)
								{
									blockType = tmpPtr[j];
									if (isFirst || blockType != transparentIndex)
									{
										tmpPtr2[j] = blockType;
									}
									j++;
								}
								tmpPtr += imgW;
								tmpPtr2 += scnWidth << 2;
								i += 4;
							}

							tmpPtr2 = scnImg + (top + 1) * scnWidth + left;
							i = 1;
							while (i < imgH)
							{
								j = 0;
								while (j < imgW)
								{
									blockType = tmpPtr[j];
									if (isFirst || blockType != transparentIndex)
									{
										tmpPtr2[j] = blockType;
									}
									j++;
								}
								tmpPtr += imgW;
								tmpPtr2 += scnWidth << 1;
								i += 2;
							}
						}
						else
						{
							if (isFirst)
							{
								if (imgW == scnWidth)
								{
									readSize = lzw->Read(scnImg.WithSize(imgW * imgH));
								}
								else
								{
									tmpPtr2 = scnImg + top * scnWidth + left;
									i = 0;
									while (i < imgH)
									{
										readSize = lzw->Read(tmpPtr2.WithSize(imgW));
										tmpPtr2 += scnWidth;
										i++;
									}
								}
							}
							else
							{
								imgData.ChangeSize(imgW * imgH);
								readSize = lzw->Read(imgData);
								tmpPtr = imgData;
								tmpPtr2 = scnImg + top * scnWidth + left;
								i = 0;
								while (i < imgH)
								{
									j = 0;
									while (j < imgW)
									{
										blockType = tmpPtr[j];
										if (blockType != transparentIndex)
										{
											tmpPtr2[j] = blockType;
										}
										j++;
									}
									tmpPtr += imgW;
									tmpPtr2 += scnWidth;
									i++;
								}
							}
						}

						MemCopyANC(simg->data, scnImg.Ptr(), scnWidth * scnHeight);
						if (!localColorTable.IsNull())
						{
							readSize = (UInt32)(1 << ((imgDesc[8] & 7) + 1));
							tmpPtr = localColorTable;
						}
						else if (!screenColorTable.IsNull())
						{
							readSize = (UInt32)(1 << colorSize);
							tmpPtr = screenColorTable;
						}
						else if (!globalColorTable.IsNull())
						{
							readSize = (UInt32)(1 << colorSize);
							tmpPtr = globalColorTable;
						}
						else
						{
							tmpPtr = Data::ByteArray(0, 0);
						}
						if (!tmpPtr.IsNull())
						{
							tmpPtr2 = Data::ByteArray(simg->pal, readSize * 4);
							i = 0;
							while (i < readSize)
							{
								tmpPtr2[0] = tmpPtr[2];
								tmpPtr2[1] = tmpPtr[1];
								tmpPtr2[2] = tmpPtr[0];
								tmpPtr2[3] = 0xff;
								tmpPtr2 += 4;
								tmpPtr += 3;
								i++;
							}
							if (globalTransparentIndex >= 0)
							{
								simg->pal[globalTransparentIndex * 4 + 3] = 0;
							}
						}
					}
					imgData.Delete();
					imgList->AddImage(simg, frameDelay);
					isFirst = false;

					DEL_CLASS(lzw);
					break;
				}
				else
				{
					currOfst--;
					break;
				}
			}
			localColorTable.Delete();
		}
		else if (blockType == 0x21)
		{
			if (fd->GetRealData(currOfst, 2, BYTEARR(readBlock)) != 2)
				break;
			currOfst += 2;
			blockType = readBlock[0];
			readSize = readBlock[1];
			if (readSize == 0)
				break;

			if (blockType == 0xf9) //Graphic Control Label
			{
				if (fd->GetRealData(currOfst, readSize + 1, BYTEARR(readBlock)) != (readSize + 1))
				{
					break;
				}
				currOfst += readSize + 1;
				if (readSize >= 4)
				{
					frameDelay = (UInt32)ReadUInt16(&readBlock[1]) * 10;
					if (readBlock[0] & 1)
					{
						transparentIndex = readBlock[3];
					}
					disposalMethod = (readBlock[0] >> 2) & 7;
				}
				if (readBlock[readSize] != 0)
				{
					break;
				}
			}
			else if (blockType == 0xfe) //Comment Label
			{
				while (readSize > 0)
				{
					if (fd->GetRealData(currOfst, readSize + 1, BYTEARR(readBlock)) != (readSize + 1))
					{
						break;
					}
					currOfst += readSize + 1;
					readSize = readBlock[readSize];
				}
			}
			else if (blockType == 0x01) //Plain Text Label
			{
				while (readSize > 0)
				{
					if (fd->GetRealData(currOfst, readSize + 1, BYTEARR(readBlock)) != (readSize + 1))
					{
						break;
					}
					currOfst += readSize + 1;
					readSize = readBlock[readSize];
				}
			}
			else if (blockType == 0xff) //Application Extension Label
			{
				while (readSize > 0)
				{
					if (fd->GetRealData(currOfst, readSize + 1, BYTEARR(readBlock)) != (readSize + 1))
					{
						break;
					}
					currOfst += readSize + 1;
					readSize = readBlock[readSize];
				}
			}
			else
			{
				while (readSize > 0)
				{
					if (fd->GetRealData(currOfst, readSize + 1, BYTEARR(readBlock)) != (readSize + 1))
					{
						break;
					}
					currOfst += readSize + 1;
					readSize = readBlock[readSize];
				}
			}
		}
		else if (blockType == 0x3b) //GIF Trailer
		{
			break;
		}
		else
		{
			break;
		}
	}

	if (imgList->GetCount() == 0)
	{
		DEL_CLASS(imgList);
		imgList = 0;
	}
	return imgList;
}
