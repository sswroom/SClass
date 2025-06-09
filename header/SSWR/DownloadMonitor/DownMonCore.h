#ifndef _SM_SSWR_DOWNLOADMONITOR_DOWNMONCORE
#define _SM_SSWR_DOWNLOADMONITOR_DOWNMONCORE
#include "AnyType.h"
#include "Handles.h"
#include "Data/FastMapNN.h"
#include "Data/FastStringMapNN.h"
#include "Media/VideoChecker.h"
#include "Parser/ParserList.h"
#include "Sync/Event.h"
#include "Sync/Mutex.h"
#include "Sync/MutexUsage.h"
#include "Sync/Thread.h"

namespace SSWR
{
	namespace DownloadMonitor
	{
		class DownMonCore
		{
		public:
			typedef enum
			{
				CS_IDLE,
				CS_CHECKING,
				CS_EXTRACTING,
				CS_VALIDATING,
				CS_MOVING,
				CS_MUXING,
				CS_DOWNLOADING
			} CheckStatus;

			typedef enum
			{
				FS_NORMAL,
				FS_DOWNLOADED,
				FS_ERROR
			} FileStatus;

			struct FileInfo
			{
				Int32 id;
				Int32 webType;
				NN<Text::String> dbName;
				NN<Text::String> fileName;
				FileStatus status;
				Sync::Mutex mut;
			};

			typedef void (CALLBACKFUNC FileEndHandler)(AnyType userObj, Int32 fileId, Int32 webType);
		private:
			NN<Net::SocketFactory> sockf;
			NN<Net::TCPClientFactory> clif;
			Optional<Net::SSLEngine> ssl;
			Sync::Thread thread;
			CheckStatus chkStatus;
			FileEndHandler fileEndHdlr;
			void *fileEndObj;

			Text::String *downPath;
			Text::String *succPath;
			Text::String *errPath;
			Text::String *ytPath;
			Text::String *ffmpegPath;
			Text::String *firefoxPath;
			Text::String *listFile;
			Parser::ParserList *parsers;
			Media::VideoChecker checker;

			Sync::Mutex fileMut;
			Data::FastMapNN<Int32, FileInfo> fileTypeMap;
			Data::FastStringMapNN<FileInfo> fileNameMap;

			Bool FFMPEGMux(const UTF8Char *videoFile, const UTF8Char *audioFile, const UTF8Char *outFile);
			Bool FFMPEGMuxAAC(const UTF8Char *videoFile, const UTF8Char *audioFile, const UTF8Char *outFile);
			Bool ExtractZIP(Text::CStringNN zipFile, Text::CStringNN mp4File);
			Bool VideoValid(Text::CStringNN fileName);
			void ProcessDir(Text::String *downPath, Text::String *succPath, Text::String *errPath);
			static void __stdcall CheckThread(NN<Sync::Thread> thread);

		public:
			DownMonCore();
			~DownMonCore();

			Bool IsError();
			NN<Net::SocketFactory> GetSocketFactory();
			NN<Net::TCPClientFactory> GetTCPClientFactory();
			Optional<Net::SSLEngine> GetSSLEngine();
			CheckStatus GetCurrStatus();
			void SetFileEndHandler(FileEndHandler hdlr, void *userObj);
			Text::String *GetListFile();

			static void __stdcall FileFree(NN<FileInfo> file);
			Bool FileAdd(Int32 id, Int32 webType, NN<Text::String> dbName);
			Optional<FileInfo> FileGet(Int32 id, Int32 webType, NN<Sync::MutexUsage> mutUsage);
			Int32 FileGetByName(Text::CStringNN fileName, OutParam<Int32> webType);
			Bool FileEnd(Int32 id, Int32 webType);
			Bool FileStart(Int32 id, Int32 webType, Optional<ControlHandle> formHand);
			Int32 FileGetMaxId(Int32 webType);
		};
	}
}
#endif