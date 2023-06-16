#include "Stdafx.h"
#include "MyMemory.h"
#include "Data/ByteTool.h"
#include "Data/DateTime.h"
#include "IO/Device/QQZMSerialCamera.h"
#include "Sync/SimpleThread.h"
#include "Sync/ThreadUtil.h"
#include "Text/MyString.h"

#define PACKETSIZE 512
#define IMAGESIZE 3

UInt32 __stdcall IO::Device::QQZMSerialCamera::RecvThread(void *userObj)
{
	IO::Device::QQZMSerialCamera *me = (IO::Device::QQZMSerialCamera*)userObj;
	Data::DateTime *dt;
	UInt8 cmdBuff[10];
	UInt8 buff[2048];
	UOSInt buffSize = 0;
	UOSInt recvSize;
	UOSInt i;
	me->threadRunning = true;
	NEW_CLASS(dt, Data::DateTime());
	while (!me->threadToStop)
	{
		recvSize = me->stm->Read(&buff[buffSize], 2048 - buffSize);
		if (recvSize < 0)
		{
			if (!me->threadToStop)
			{
				Sync::SimpleThread::Sleep(10);
			}
		}
		else
		{
			buffSize += recvSize;
			i = 0;
			while (i < buffSize)
			{
				if (buff[i] == 0x55)
				{
					if ((buffSize - i) < 4)
						break;
					if (buff[i + 1] == '?' && buff[i + 3] == '#')
					{
//						printf("NAK\r\n");
						i += 4;
					}
					else if (buff[i + 1] == 'H' && buff[i + 3] == '#')
					{
//						printf("Capture ACK\r\n");
						i += 4;
					}
					else if (buff[i + 1] == 'E' && buff[i + 3] == '#')
					{
//						printf("Get Packet ACK\r\n");
						i += 4;
					}
					else if (buff[i + 1] == 'R')
					{
						if ((buffSize - i) < 10)
						{
							break;
						}
						else if (buff[i + 9] != '#')
						{
							i++;
						}
						else
						{
							me->imgSize = ReadUInt32(&buff[i + 3]);
							me->imgPackets = ReadUInt16(&buff[i + 7]);
							i += 10;

//							printf("Picture Info: Size = %d, nPackets = %d\r\n", me->imgSize, me->imgPackets);

							me->imgNextOfst = 0;
							me->imgNextPacket = 1;
							if (me->imgBuff)
							{
								MemFree(me->imgBuff);
							}
							if (me->imgSize > 0)
							{
								dt->SetCurrTimeUTC();
								me->imgLastUpdateTime = dt->ToTicks();

								me->imgBuff = MemAlloc(UInt8, me->imgSize);

								cmdBuff[0] = 'U';
								cmdBuff[1] = 'E';
								cmdBuff[2] = me->cameraId;
								WriteInt16(&cmdBuff[3], 1);
								cmdBuff[5] = '#';
								me->stm->Write(cmdBuff, 6);
							}
							else
							{
								me->imgBuff = 0;
							}
						}
					}
					else if (buff[i + 1] == 'F')
					{
						if ((buffSize - i) < 7)
						{
							break;
						}
						Int32 packNum = ReadUInt16(&buff[i + 3]);
						UInt32 packSize = ReadUInt16(&buff[i + 5]);
						if (packSize > 1024 || packSize <= 0)
						{
							i++;
						}
						else if ((buffSize - i) < 9 + packSize)
						{
							break;
						}
						else
						{
//							printf("Image Packet %d Received\r\n", packNum);
							if (me->imgNextPacket == packNum && (me->imgNextOfst + packSize) <= me->imgSize)
							{
								MemCopyNO(&me->imgBuff[me->imgNextOfst], &buff[i + 7], packSize);
								me->imgNextOfst += packSize;
								me->imgNextPacket++;
								if (me->imgNextPacket <= me->imgPackets)
								{
									dt->SetCurrTimeUTC();
									me->imgLastUpdateTime = dt->ToTicks();

									cmdBuff[0] = 'U';
									cmdBuff[1] = 'E';
									cmdBuff[2] = me->cameraId;
									WriteInt16(&cmdBuff[3], me->imgNextPacket);
									cmdBuff[5] = '#';
									me->stm->Write(cmdBuff, 6);
								}
								else if (me->imgNextOfst == me->imgSize)
								{
//									printf("Complete Image Received, size = %d\r\n", me->imgSize);
									me->imgEnd = true;
								}
							}
							i += 9 + packSize;
						}
					}
					else
					{
						i++;
					}
				}
				else
				{
					i++;
				}
			}
			if (i >= buffSize)
			{
				buffSize = 0;
			}
			else if (i > 0)
			{
				MemCopyO(buff, &buff[i], buffSize - i);
				buffSize -= i;
			}
		}
	}
	DEL_CLASS(dt);
	me->threadRunning = false;
	return 0;
}

IO::Device::QQZMSerialCamera::QQZMSerialCamera(IO::Stream *stm, UInt8 cameraId, Bool toRelease)
{
	this->stm = stm;
	this->cameraId = cameraId;
	this->toRelease = toRelease;

	this->imgBuff = 0;
	this->imgNextPacket = 0;
	this->imgSize = 0;

	this->threadToStop = false;
	this->threadRunning = false;
	Sync::ThreadUtil::Create(RecvThread, this);
	while (!this->threadRunning)
	{
		Sync::SimpleThread::Sleep(10);
	}
}

IO::Device::QQZMSerialCamera::~QQZMSerialCamera()
{
	this->threadToStop = true;
	this->stm->Close();
	while (this->threadRunning)
	{
		Sync::SimpleThread::Sleep(10);
	}
	if (this->imgBuff)
	{
		MemFree(this->imgBuff);
		this->imgBuff = 0;
	}
	if (this->toRelease)
	{
		DEL_CLASS(this->stm);
	}
}

Bool IO::Device::QQZMSerialCamera::CapturePhoto(IO::Stream *outStm)
{
	Data::DateTime dt;
	UInt8 cmdBuff[7];
	Int64 currTime;
	cmdBuff[0] = 'U';
	cmdBuff[1] = 'H';
	cmdBuff[2] = this->cameraId;
	cmdBuff[3] = 48 + IMAGESIZE;
	WriteInt16(&cmdBuff[4], PACKETSIZE); //package size
	cmdBuff[6] = '#';
	dt.SetCurrTimeUTC();
	currTime = dt.ToTicks();
	this->imgEnd = false;
	this->imgLastUpdateTime = currTime;
//	printf("Start CapturePhoto\r\n");
	this->stm->Write(cmdBuff, 7);
	while (!this->imgEnd)
	{
		dt.SetCurrTimeUTC();
		currTime = dt.ToTicks();
		if ((currTime - this->imgLastUpdateTime) > 2000)
		{
			break;
		}
		Sync::SimpleThread::Sleep(100);
	}
	if (this->imgEnd && this->imgBuff)
	{
//		printf("CapturePhoto Success\r\n");
		this->imgNextPacket = -1;
		outStm->Write(this->imgBuff, this->imgSize);
		MemFree(this->imgBuff);
		this->imgBuff = 0;
		return true;
	}
	else
	{
//		printf("CapturePhoto Failed\r\n");
		return false;
	}
	
}
