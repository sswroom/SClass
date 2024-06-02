#ifndef _SM_NET_SDPFILE
#define _SM_NET_SDPFILE
#include "Data/ArrayList.h"
#include "Data/ArrayListStrUTF8.h"
#include "Data/ArrayListStringNN.h"
#include "IO/Stream.h"
#include "Net/ISDPMedia.h"

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

		Data::ArrayListStringNN sessDesc;
		Data::ArrayList<Int32> sessDescType;
		Data::ArrayList<Net::ISDPMedia *> sessMedia;

		Data::ArrayListNN<Data::ArrayListStrUTF8> mediaList;

		void InitSDP();
	public:
		SDPFile(UInt8 *buff, UOSInt buffSize);
		SDPFile();
		~SDPFile();

		void SetDefaults();
		void SetVersion(Int32 version);
		void SetUserName(Text::CString userName);
		void SetSessName(Text::CString sessName);
		void SetSessId(Text::CString sessId);
		void SetSessVer(Text::CString sessVer);
		void SetUserAddrType(Text::CString userAddrType);
		void SetUserAddrHost(Text::CString userAddrHost);
		void SetStartTime(Int64 startTime);
		void SetEndTime(Int64 endTime);
		void SetSessTool(Text::CString sessTool);
		void SetSessType(Text::CString sessType);
		void SetSessCharset(Text::CString sessCharset);
		void SetSessControl(Text::CString sessControl);
		void SetReqUserAgent(Text::CString userAgent);

		void AddBuildMedia(Net::ISDPMedia *media);
		Bool BuildBuff();

		Bool WriteToStream(NN<IO::Stream> stm);
		UOSInt GetLength();

		UOSInt GetMediaCount();
		Optional<Data::ArrayListStrUTF8> GetMediaDesc(UOSInt index);
	};
}
#endif
