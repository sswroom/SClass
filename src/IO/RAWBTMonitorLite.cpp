#include "Stdafx.h"
#include "MyMemory.h"
#include "Data/ByteTool.h"
#include "IO/RAWBTMonitor.h"
#include <unistd.h>
#include <sys/ioctl.h>
#include <sys/socket.h>
//#include <bluetooth/bluetooth.h>
//#include <bluetooth/hci.h>

#include <stdio.h>

typedef UInt32 uint32_t;
typedef UInt16 uint16_t;

#define HCI_MAX_DEV	16
#define BTPROTO_HCI	1
#define SOL_HCI		0
#define HCI_DATA_DIR	1
#define HCI_FILTER	2
#define HCI_TIME_STAMP	3
#define HCI_CMSG_DIR	0x0001
#define HCI_CMSG_TSTAMP	0x0002
#define HCIGETDEVLIST	_IOR('H', 210, int)

struct sockaddr_hci {
	sa_family_t	hci_family;
	unsigned short	hci_dev;
	unsigned short  hci_channel;
};

struct hci_filter {
	uint32_t type_mask;
	uint32_t event_mask[2];
	uint16_t opcode;
};

struct hci_dev_req {
	uint16_t dev_id;
	uint32_t dev_opt;
};

struct hci_dev_list_req {
	uint16_t dev_num;
	struct hci_dev_req dev_req[0];	/* hci_dev_req structures */
};

#define MAX_PACKET_SIZE 2048

IO::RAWBTMonitor::RAWBTMonitor(UOSInt devNum)
{
	this->fd = socket(AF_BLUETOOTH, SOCK_RAW, BTPROTO_HCI);
	if (this->fd < 0) return;

	int opt = 1;
	if (setsockopt(this->fd, SOL_HCI, HCI_DATA_DIR, &opt, sizeof(opt)) < 0)
	{
		this->Close();
		return;
	}

	opt = 1;
	if (setsockopt(this->fd, SOL_HCI, HCI_TIME_STAMP, &opt, sizeof(opt)) < 0)
	{
		this->Close();
		return;
	}

	struct hci_filter flt;
	memset(&flt, 0, sizeof(flt));
	memset((void *) &flt.type_mask, 0xff, sizeof(flt.type_mask));
	memset((void *) &flt.event_mask, 0xff, sizeof(flt.event_mask));
	if (setsockopt(this->fd, SOL_HCI, HCI_FILTER, &flt, sizeof(flt)) < 0)
	{
		this->Close();
		return;
	}

	struct sockaddr_hci addr;
	addr.hci_family = AF_BLUETOOTH;
	addr.hci_dev = (UInt16)devNum;
#ifdef HAVE_STRUCT_SOCKADDR_HCI_HCI_CHANNEL
	addr.hci_channel = HCI_CHANNEL_RAW;
#endif
	if (bind(this->fd, (struct sockaddr *) &addr, sizeof(addr)) < 0)
	{
		this->Close();
		return;
	}
}

IO::RAWBTMonitor::~RAWBTMonitor()
{
	this->Close();
}

IO::RAWMonitor::LinkType IO::RAWBTMonitor::GetLinkType()
{
	return IO::RAWMonitor::LinkType::Bluetooth;
}

Bool IO::RAWBTMonitor::IsError()
{
	return this->fd < 0;
}

void IO::RAWBTMonitor::Close()
{
	if (this->fd >= 0)
	{
		close(this->fd);
		this->fd = -1;
	}
}

UOSInt IO::RAWBTMonitor::GetMTU()
{
	return MAX_PACKET_SIZE;
}

UOSInt IO::RAWBTMonitor::NextPacket(UInt8 *buff, Int64 *timeTicks)
{
	UInt8 ctrlBuff[128];
	struct cmsghdr *cmsg;
	struct msghdr msg;
	struct iovec  iv;
	ssize_t ret;
	int in = 0;
	struct timeval ts;
	Int64 packetTime = 0;

	iv.iov_base = buff + 4;
	iv.iov_len = MAX_PACKET_SIZE - 4;

	MemClear(&msg, sizeof(msg));
	msg.msg_iov = &iv;
	msg.msg_iovlen = 1;
	msg.msg_control = ctrlBuff;
	msg.msg_controllen = sizeof(ctrlBuff);

	ret = recvmsg(this->fd, &msg, 0);
	if (ret <= 0)
	{
		return 0;
	}

	cmsg = CMSG_FIRSTHDR(&msg);
	while (cmsg)
	{
		switch (cmsg->cmsg_type)
		{
		case HCI_CMSG_DIR:
			MemCopyNO(&in, CMSG_DATA(cmsg), sizeof(in));
			break;
		case HCI_CMSG_TSTAMP:
			MemCopyNO(&ts, CMSG_DATA(cmsg), sizeof(ts));
			packetTime = (Int64)ts.tv_sec * 1000 + ts.tv_usec / 1000;
			break;
		}
		cmsg = CMSG_NXTHDR(&msg, cmsg);
	}
	WriteMUInt32(buff, (in != 0));
	if (timeTicks) *timeTicks = packetTime;
	return (UOSInt)ret + 4;
}

UOSInt IO::RAWBTMonitor::GetDevCount()
{
	struct hci_dev_list_req *dev_list;
	struct hci_dev_req *dev_req;
	int sock;

	printf("GetDevCount\r\n");
	sock  = socket(AF_BLUETOOTH, SOCK_RAW, BTPROTO_HCI);
	printf("sock = %d\r\n", sock);
	if (sock < 0)
	{
		return 0;
	}

	dev_list = (struct hci_dev_list_req*)MemAlloc(UInt8, HCI_MAX_DEV * sizeof(*dev_req) + sizeof(*dev_list));
	if (dev_list == 0)
	{
		close(sock);
		return 0;
	}

	dev_list->dev_num = HCI_MAX_DEV;

	if (ioctl(sock, HCIGETDEVLIST, (void *) dev_list) < 0)
	{
		printf("HCIGETDEVLIST failed\r\n", sock);
		MemFree(dev_list);
		close(sock);
		return 0;
	}

	UOSInt cnt = dev_list->dev_num;
	printf("devcnt = %d\r\n", cnt);
	MemFree(dev_list);
	close(sock);
	return cnt;
}
