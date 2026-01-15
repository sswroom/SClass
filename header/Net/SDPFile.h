#ifndef _SM_NET_SDPFILE
#define _SM_NET_SDPFILE
#include "Data/ArrayListNative.hpp"
#include "Data/ArrayListStrUTF8.h"
#include "Data/ArrayListStringNN.h"
#include "IO/Stream.h"
#include "Net/SDPMedia.h"

namespace Net
{
	class SDPFile
	{
	private:
		UInt8 *buff;
		UIntOS buffSize;

		Int32 version;
		Optional<Text::String> sessName;
		Optional<Text::String> userName;
		Optional<Text::String> sessId;
		Optional<Text::String> sessVer;
		Optional<Text::String> userAddrType;
		Optional<Text::String> userAddrHost;
		Int64 startTime;
		Int64 endTime;
		Optional<Text::String> sessTool;
		Optional<Text::String> sessType;
		Optional<Text::String> sessCharset;
		Optional<Text::String> sessControl;
		Bool sessRecv;
		Bool sessSend;
		Optional<Text::String> reqUserAgent;

		Data::ArrayListStringNN sessDesc;
		Data::ArrayListNative<Int32> sessDescType;
		Data::ArrayListObj<Net::SDPMedia *> sessMedia;

		Data::ArrayListNN<Data::ArrayListStrUTF8> mediaList;

		void InitSDP();
	public:
		SDPFile(UInt8 *buff, UIntOS buffSize);
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

		void AddBuildMedia(Net::SDPMedia *media);
		Bool BuildBuff();

		Bool WriteToStream(NN<IO::Stream> stm);
		UIntOS GetLength();

		UIntOS GetMediaCount();
		Optional<Data::ArrayListStrUTF8> GetMediaDesc(UIntOS index);
	};
}
#endif
