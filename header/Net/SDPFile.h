#ifndef _SM_NET_SDPFILE
#define _SM_NET_SDPFILE
#include "Net/ISDPMedia.h"
#include "Data/ArrayList.h"
#include "IO/Stream.h"

namespace Net
{
	class SDPFile
	{
	private:
		UInt8 *buff;
		UOSInt buffSize;

		Int32 version;
		Text::String *sessName;
		Text::String *userName;
		Text::String *sessId;
		Text::String *sessVer;
		Text::String *userAddrType;
		Text::String *userAddrHost;
		Int64 startTime;
		Int64 endTime;
		Text::String *sessTool;
		Text::String *sessType;
		Text::String *sessCharset;
		Text::String *sessControl;
		Bool sessRecv;
		Bool sessSend;
		Text::String *reqUserAgent;

		Data::ArrayList<Text::String *> *sessDesc;
		Data::ArrayList<Int32> *sessDescType;
		Data::ArrayList<Net::ISDPMedia *> *sessMedia;

		Data::ArrayList<Data::ArrayList<const UTF8Char *>*> *mediaList;

		void InitSDP();
	public:
		SDPFile(UInt8 *buff, UOSInt buffSize);
		SDPFile();
		~SDPFile();

		void SetDefaults();
		void SetVersion(Int32 version);
		void SetUserName(const UTF8Char *userName);
		void SetSessName(const UTF8Char *sessName);
		void SetSessId(const UTF8Char *sessId);
		void SetSessVer(const UTF8Char *sessVer);
		void SetUserAddrType(const UTF8Char *userAddrType);
		void SetUserAddrHost(const UTF8Char *userAddrHost);
		void SetStartTime(Int64 startTime);
		void SetEndTime(Int64 endTime);
		void SetSessTool(const UTF8Char *sessTool);
		void SetSessType(const UTF8Char *sessType);
		void SetSessCharset(const UTF8Char *sessCharset);
		void SetSessControl(const UTF8Char *sessControl);
		void SetReqUserAgent(const UTF8Char *userAgent);

		void AddBuildMedia(Net::ISDPMedia *media);
		Bool BuildBuff();

		Bool WriteToStream(IO::Stream *stm);
		UOSInt GetLength();

		UOSInt GetMediaCount();
		Data::ArrayList<const UTF8Char *> *GetMediaDesc(UOSInt index);
	};
}
#endif
