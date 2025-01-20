#include "Stdafx.h"
#include "MyMemory.h"
#include "IO/Path.h"
#include "Manage/HiResClock.h"
#include "Math/Math.h"
#include "Media/V4LVideoCapture.h"
#include "Text/MyString.h"
#include <errno.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <linux/videodev2.h>

#include <stdio.h>

void __stdcall Media::V4LVideoCapture::PlayThread(NN<Sync::Thread> thread)
{
	NN<Media::V4LVideoCapture> me = thread->GetUserObj().GetNN<Media::V4LVideoCapture>();
	Sync::ThreadUtil::SetName(CSTR("V4LVideoCap"));
	struct timeval tv;
	fd_set fds;
	UInt32 frameNum;
	Double t;
	enum v4l2_buf_type type;
	struct v4l2_buffer buf;
	{
		Manage::HiResClock clk;
		Text::StringBuilderUTF8 debugSb;
		frameNum = 0;
		while (!thread->IsStopping())
		{
			tv.tv_sec = 10;
			tv.tv_usec = 0;

			FD_ZERO(&fds);
			FD_SET(me->fd, &fds);
			int r = select(me->fd + 1, &fds, 0, 0, &tv);
			t = clk.GetTimeDiff();
			if (r < 0 && errno != EINTR)
			{
				printf("Error in select: %d\r\n", errno);
				break;
			}
			if (r == 0)
			{
				printf("Error in select: timeout\r\n");
				break;
			}
			
			MemClear(&buf, sizeof(buf));
			buf.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
			buf.memory = V4L2_MEMORY_USERPTR;
			r = ioctl(me->fd, VIDIOC_DQBUF, &buf);
			if (r == 0)
			{
				if (buf.bytesused == 4 && ReadMUInt32((const UInt8*)buf.m.userptr) == 0xFFD8FFD9)
				{

				}
				else if (me->cb)
				{
					me->cb((UInt32)Double2Int32(t * 1000), frameNum, UnsafeArray<UnsafeArray<UInt8>>::ConvertFrom(UnsafeArray<UInt8*>((UInt8**)&buf.m.userptr)), buf.bytesused, Media::VideoSource::FS_I, me->userData, me->frameInfo.ftype, Media::VideoSource::FF_REALTIME, me->frameInfo.ycOfst);
				}
				frameNum++;
				ioctl(me->fd, VIDIOC_QBUF, &buf);
			}
			else
			{
				if (errno == ENODEV)
				{
					printf("Error in dqbuf: %d\r\n", errno);
				}
				else if (errno != EAGAIN && errno != EINTR)
				{
					printf("Error in dqbuf: %d\r\n", errno);
					break;
				}
			}
		}
	}
	type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
	ioctl(me->fd, VIDIOC_STREAMOFF, &type);
}

Media::V4LVideoCapture::V4LVideoCapture(UOSInt devId) : thread(PlayThread, this, CSTR("V4LVideoCap"))
{
	Char cbuff[64];
	Text::StrUOSInt(Text::StrConcat(cbuff, "/dev/video"), devId);
	this->fd = open(cbuff, O_RDONLY | O_NONBLOCK);
	this->devId = devId;

	this->frameBuffs[0] = 0;
	this->frameBuffs[1] = 0;
	this->frameBuffs[2] = 0;
	this->frameBuffs[3] = 0;
	this->frameBuffSize = 0;
	this->cb = 0;
	this->fcCb = 0;
	this->userData = 0;
}

Media::V4LVideoCapture::~V4LVideoCapture()
{
	this->Stop();
	if (this->fd != -1)
	{
		close(this->fd);
	}
	if (this->frameBuffs[0])
	{
		MemFreeA(this->frameBuffs[0]);
		MemFreeA(this->frameBuffs[1]);
		MemFreeA(this->frameBuffs[2]);
		MemFreeA(this->frameBuffs[3]);
		this->frameBuffs[0] = 0;
		this->frameBuffs[1] = 0;
		this->frameBuffs[2] = 0;
		this->frameBuffs[3] = 0;
	}
}

UnsafeArrayOpt<UTF8Char> Media::V4LVideoCapture::GetSourceName(UnsafeArray<UTF8Char> buff)
{
	struct v4l2_capability video_cap;

	if (ioctl(fd, VIDIOC_QUERYCAP, &video_cap) != -1)
	{
		return Text::StrConcat(buff, (const UTF8Char*)video_cap.card);;
	}
	else
	{
		*buff = 0;
		return 0;
	}
}

Text::CStringNN Media::V4LVideoCapture::GetFilterName()
{
	return CSTR("V4LVideoCapture");
}

Bool Media::V4LVideoCapture::GetVideoInfo(NN<Media::FrameInfo> info, OutParam<UInt32> frameRateNorm, OutParam<UInt32> frameRateDenorm, OutParam<UOSInt> maxFrameSize)
{
	struct v4l2_format fmt;
	MemClear(&fmt, sizeof(fmt));
	fmt.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
	if (ioctl(this->fd, VIDIOC_G_FMT, &fmt) != 0)
		return false;
	if (fmt.type != V4L2_BUF_TYPE_VIDEO_CAPTURE)
		return false;
	info->dispSize.x = fmt.fmt.pix.width;
	info->dispSize.y = fmt.fmt.pix.height;
	info->storeSize = info->dispSize;
	info->fourcc = fmt.fmt.pix.pixelformat;
	info->storeBPP = 16;
	info->pf = Media::PF_UNKNOWN;
	info->byteSize = 0;
	info->rotateType = Media::RotateType::None;
	info->par2 = 1;
	info->hdpi = 72;
	info->vdpi = 72;
	switch (fmt.fmt.pix.field)
	{
	case V4L2_FIELD_ANY:
	case V4L2_FIELD_NONE:
		info->ftype = Media::FT_NON_INTERLACE;
		break;
	case V4L2_FIELD_TOP:
		info->ftype = Media::FT_FIELD_TF;
		break;
	case V4L2_FIELD_BOTTOM:
		info->ftype = Media::FT_FIELD_TF;
		break;
	case V4L2_FIELD_INTERLACED:
		info->ftype = Media::FT_INTERLACED_TFF;
		break;
	case V4L2_FIELD_SEQ_TB:
		info->ftype = Media::FT_NON_INTERLACE;
		break;
	case V4L2_FIELD_SEQ_BT:
		info->ftype = Media::FT_NON_INTERLACE;
		break;
	case V4L2_FIELD_ALTERNATE:
		info->ftype = Media::FT_FIELD_TF;
		break;
	case V4L2_FIELD_INTERLACED_TB:
		info->ftype = Media::FT_INTERLACED_TFF;
		break;
	case V4L2_FIELD_INTERLACED_BT:
		info->ftype = Media::FT_INTERLACED_BFF;
		break;
	default:
		info->ftype = Media::FT_NON_INTERLACE;
		break;
	}
	info->atype = Media::AT_NO_ALPHA;
	switch (fmt.fmt.pix.colorspace)
	{
	case V4L2_COLORSPACE_SMPTE170M:
	case V4L2_COLORSPACE_SMPTE240M:
	case V4L2_COLORSPACE_REC709:
	case V4L2_COLORSPACE_BT878:
	case V4L2_COLORSPACE_470_SYSTEM_M:
	case V4L2_COLORSPACE_470_SYSTEM_BG:
	case V4L2_COLORSPACE_JPEG:
	case V4L2_COLORSPACE_SRGB:
	default:
		info->color.SetCommonProfile(Media::ColorProfile::CPT_BT709);
		break;
	}
	info->yuvType = Media::ColorProfile::YUVT_BT601;
	info->ycOfst = Media::YCOFST_C_CENTER_LEFT;
	maxFrameSize.Set(fmt.fmt.pix.sizeimage);
	
	v4l2_streamparm param;
	MemClear(&param, sizeof(param));
	if (ioctl(this->fd, VIDIOC_G_PARM, &param) == 0)
	{
		frameRateNorm.Set(param.parm.capture.timeperframe.denominator);
		frameRateDenorm.Set(param.parm.capture.timeperframe.numerator);
	}
	else
	{
		frameRateNorm.Set(30);
		frameRateDenorm.Set(1);
	}
	return true;
}

Bool Media::V4LVideoCapture::Init(FrameCallback cb, FrameChangeCallback fcCb, AnyType userData)
{
	this->cb = cb;
	this->fcCb = fcCb;
	this->userData = userData;
	return true;
}

Bool Media::V4LVideoCapture::Start()
{
	if (this->thread.IsRunning())
		return true;
	if (this->cb == 0)
		return false;

	if (this->ReadFrameBegin())
	{
		this->thread.Start();
		return true;
	}
	else
	{
		printf("Error in streamon %d\r\n", errno);
		return false;
	}
}

void Media::V4LVideoCapture::Stop()
{
	this->thread.Stop();
}

Bool Media::V4LVideoCapture::IsRunning()
{
	return this->thread.IsRunning();
}

void Media::V4LVideoCapture::SetPreferSize(Math::Size2D<UOSInt> size, UInt32 fourcc, UInt32 bpp, UInt32 frameRateNumer, UInt32 frameRateDenom)
{
	struct v4l2_format fmt;
	struct v4l2_streamparm parm;
	MemClear(&fmt, sizeof(fmt));
	fmt.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
	fmt.fmt.pix.width = (UInt32)size.x;
	fmt.fmt.pix.height = (UInt32)size.y;
	fmt.fmt.pix.pixelformat = fourcc;
	fmt.fmt.pix.field = V4L2_FIELD_NONE;
	fmt.fmt.pix.bytesperline = 0;
	fmt.fmt.pix.sizeimage = 0;
	fmt.fmt.pix.colorspace = V4L2_COLORSPACE_REC709;
	fmt.fmt.pix.priv = 0;
	if (ioctl(this->fd, VIDIOC_S_FMT, &fmt) != 0)
	{
		printf("Error in s_fmt %d\r\n", errno);
	}

	MemClear(&parm, sizeof(parm));
	parm.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
	parm.parm.capture.capability = 0;
	parm.parm.capture.capturemode = V4L2_MODE_HIGHQUALITY;
	parm.parm.capture.timeperframe.numerator = (UInt32)frameRateDenom;
	parm.parm.capture.timeperframe.denominator = (UInt32)frameRateNumer;
	parm.parm.capture.extendedmode = 0;
	parm.parm.capture.readbuffers = 4;
	if (ioctl(this->fd, VIDIOC_S_PARM, &parm) != 0)
	{
		printf("Error in s_parm %d\r\n", errno);
	}
}

UOSInt Media::V4LVideoCapture::GetSupportedFormats(VideoFormat *fmtArr, UOSInt maxCnt)
{
	UOSInt ret = 0;
	struct v4l2_fmtdesc fmt;
	struct v4l2_frmsizeenum fsize;
	struct v4l2_frmivalenum frmival;
	MemClear(&fsize, sizeof(fsize));
	MemClear(&frmival, sizeof(frmival));
	fmt.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
	fmt.index = 0;
	while (ret < maxCnt && ioctl(this->fd, VIDIOC_ENUM_FMT, &fmt) == 0)
	{
		fsize.index = 0;
		fsize.pixel_format = fmt.pixelformat;
		while (ret < maxCnt && ioctl(this->fd, VIDIOC_ENUM_FRAMESIZES, &fsize) == 0)
		{
			frmival.index = 0;
			frmival.pixel_format = fmt.pixelformat;
			frmival.width = fsize.discrete.width;
			frmival.height = fsize.discrete.height;
			while (ret < maxCnt && ioctl(this->fd, VIDIOC_ENUM_FRAMEINTERVALS, &frmival) == 0)
			{
				fmtArr[ret].info.dispSize.x = fsize.discrete.width;
				fmtArr[ret].info.dispSize.y = fsize.discrete.height;
				fmtArr[ret].info.storeSize = fmtArr[ret].info.dispSize;
				fmtArr[ret].info.fourcc = fmt.pixelformat;
				fmtArr[ret].info.storeBPP = 16;
				fmtArr[ret].info.pf = Media::PF_UNKNOWN;
				fmtArr[ret].info.byteSize = 0;
				fmtArr[ret].info.par2 = 1;
				fmtArr[ret].info.hdpi = 72;
				fmtArr[ret].info.vdpi = 72;
				fmtArr[ret].info.ftype = Media::FT_NON_INTERLACE;
				fmtArr[ret].info.atype = Media::AT_NO_ALPHA;
				fmtArr[ret].info.color.SetCommonProfile(Media::ColorProfile::CPT_BT709);
				fmtArr[ret].info.yuvType = Media::ColorProfile::YUVT_BT601;
				fmtArr[ret].info.ycOfst = Media::YCOFST_C_CENTER_LEFT;
				fmtArr[ret].frameRateNorm = frmival.discrete.denominator;
				fmtArr[ret].frameRateDenorm = frmival.discrete.numerator;
				ret++;
				frmival.index++;
			}
			fsize.index++;
		}
		fmt.index++;
	}
	if (ret >= maxCnt)
	{
		printf("Max Count reached: %d\r\n", (Int32)maxCnt);
	}
	return ret;
}

void Media::V4LVideoCapture::GetInfo(NN<Text::StringBuilderUTF8> sb)
{
	struct v4l2_capability video_cap;
	sb->AppendC(UTF8STRC("DevPath: /dev/video"));
	sb->AppendUOSInt(this->devId);
	sb->AppendC(UTF8STRC("\r\n"));

	if (ioctl(this->fd, VIDIOC_QUERYCAP, &video_cap) != -1)
	{
		sb->AppendC(UTF8STRC("Driver: "));
		sb->AppendSlow((const UTF8Char*)video_cap.driver);
		sb->AppendC(UTF8STRC("\r\n"));

		sb->AppendC(UTF8STRC("Card: "));
		sb->AppendSlow((const UTF8Char*)video_cap.card);
		sb->AppendC(UTF8STRC("\r\n"));

		sb->AppendC(UTF8STRC("BusInfo: "));
		sb->AppendSlow((const UTF8Char*)video_cap.bus_info);
		sb->AppendC(UTF8STRC("\r\n"));
	}				
}

UOSInt Media::V4LVideoCapture::GetDataSeekCount()
{
	return 0;
}

UOSInt Media::V4LVideoCapture::ReadFrame(UOSInt frameIndex, UnsafeArray<UInt8> buff)
{
	struct timeval tv;
	fd_set fds;
//	Double t;
//	enum v4l2_buf_type type;
	struct v4l2_buffer buf;
	tv.tv_sec = 3;
	tv.tv_usec = 0;

	FD_ZERO(&fds);
	FD_SET(this->fd, &fds);
	int r = select(this->fd + 1, &fds, 0, 0, &tv);
	if (r < 0 && errno != EINTR)
	{
		printf("Error in select: %d\r\n", errno);
		return 0;
	}
	if (r == 0)
	{
		printf("Error in select: timeout\r\n");
		return 0;
	}

	MemClear(&buf, sizeof(buf));
	buf.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
	buf.memory = V4L2_MEMORY_USERPTR;
	r = ioctl(this->fd, VIDIOC_DQBUF, &buf);
	if (r == 0)
	{
		UOSInt size = buf.bytesused;
		MemCopyNO(buff.Ptr(), (void*)buf.m.userptr, size);
		ioctl(this->fd, VIDIOC_QBUF, &buf);
		return size;
	}
	else
	{
		if (errno != EAGAIN && errno != EINTR)
		{
			printf("Error in dqbuf: %d\r\n", errno);
		}
	}
	return 0;
}

Bool Media::V4LVideoCapture::ReadFrameBegin()
{
	UOSInt i;
	UOSInt frameSize;
	UInt32 frameRateNumer;
	UInt32 frameRateDenomin;
	if (!this->GetVideoInfo(this->frameInfo, frameRateNumer, frameRateDenomin, frameSize))
	{
		return false;
	}
	if (this->frameBuffs[0])
	{
		if (this->frameBuffSize >= frameSize)
		{
			this->frameBuffSize = frameSize;
		}
		else
		{
			MemFreeA(this->frameBuffs[0]);
			MemFreeA(this->frameBuffs[1]);
			MemFreeA(this->frameBuffs[2]);
			MemFreeA(this->frameBuffs[3]);
			this->frameBuffs[0] = MemAllocA(UInt8, frameSize);
			this->frameBuffs[1] = MemAllocA(UInt8, frameSize);
			this->frameBuffs[2] = MemAllocA(UInt8, frameSize);
			this->frameBuffs[3] = MemAllocA(UInt8, frameSize);
			this->frameBuffSize = frameSize;
		}
	}
	else
	{
		this->frameBuffs[0] = MemAllocA(UInt8, frameSize);
		this->frameBuffs[1] = MemAllocA(UInt8, frameSize);
		this->frameBuffs[2] = MemAllocA(UInt8, frameSize);
		this->frameBuffs[3] = MemAllocA(UInt8, frameSize);
		this->frameBuffSize = frameSize;
	}
	int r;
	struct v4l2_requestbuffers req;
	MemClear(&req, sizeof(req));
	req.count = 4;
	req.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
	req.memory = V4L2_MEMORY_USERPTR;
	r = ioctl(this->fd, VIDIOC_REQBUFS, &req);
	if (r != 0)
	{
		printf("Error in reqbufs %d\r\n", errno);
	}

	struct v4l2_buffer buf;
	i = 0;
	while (i < 4)
	{
		MemClear(&buf, sizeof(buf));
		buf.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
		buf.memory = V4L2_MEMORY_USERPTR;
		buf.index = (UInt32)i;
		buf.m.userptr = (UOSInt)this->frameBuffs[i];
		buf.length = (UInt32)this->frameBuffSize;
		r = ioctl(fd, VIDIOC_QBUF, &buf);
		if (r != 0)
		{
			printf("Error in qbufs%d %d\r\n", (Int32)i, errno);
		}
		i++;
	}

	enum v4l2_buf_type type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
	return ioctl(this->fd, VIDIOC_STREAMON, &type) == 0;
}

Bool Media::V4LVideoCapture::ReadFrameEnd()
{
	enum v4l2_buf_type type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
	ioctl(this->fd, VIDIOC_STREAMOFF, &type);
	return true;
}

Media::V4LVideoCaptureMgr::V4LVideoCaptureMgr()
{
}

Media::V4LVideoCaptureMgr::~V4LVideoCaptureMgr()
{
}

UOSInt Media::V4LVideoCaptureMgr::GetDeviceList(Data::ArrayList<UInt32> *devList)
{
	UTF8Char sbuff[512];
	UnsafeArray<UTF8Char> sptr = Text::StrConcatC(sbuff, UTF8STRC("/dev/"));
	UnsafeArray<UTF8Char> sptr2;
	UOSInt ret = 0;
	UInt32 devId;
	IO::Path::PathType pt;
	sptr2 = Text::StrConcatC(sptr, UTF8STRC("video*"));
	NN<IO::Path::FindFileSession> sess;
	if (IO::Path::FindFile(CSTRP(sbuff, sptr2)).SetTo(sess))
	{
		while (IO::Path::FindNextFile(sptr, sess, 0, pt, 0).NotNull())
		{
			devId = Text::StrToUInt32(&sptr[5]);
			devList->Add(devId);
			ret++;
		}
		IO::Path::FindFileClose(sess);
	}
	else
	{
		Char cbuff[64];
		UnsafeArray<Char> csptr = Text::StrConcat(cbuff, "/dev/video");
		int fd;
		while (true)
		{
			Text::StrUOSInt(csptr, ret);
			fd = open(cbuff, O_RDONLY);
			if (fd == -1)
			{
				break;
			}
			close(fd);
			devList->Add((UInt32)ret);
			ret++;
		}
	}
	return ret;
}

UnsafeArrayOpt<UTF8Char> Media::V4LVideoCaptureMgr::GetDeviceName(UnsafeArray<UTF8Char> buff, UOSInt devId)
{
	int fd;
	struct v4l2_capability video_cap;
	Char cbuff[64];
	*buff = 0;

	Text::StrUOSInt(Text::StrConcat(cbuff, "/dev/video"), devId);
	fd = open(cbuff, O_RDONLY);
	if (fd != -1)
	{
		if (ioctl(fd, VIDIOC_QUERYCAP, &video_cap) != -1)
		{
			buff = Text::StrConcat(buff, video_cap.card);
		}
		close(fd);
	}

	return buff;
}

NN<Media::VideoCapturer> Media::V4LVideoCaptureMgr::CreateDevice(UOSInt devId)
{
	NN<Media::V4LVideoCapture> capture;
	NEW_CLASSNN(capture, Media::V4LVideoCapture(devId));
	return capture;
}

