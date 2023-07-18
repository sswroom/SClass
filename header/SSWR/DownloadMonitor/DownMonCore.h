#ifndef _SM_SSWR_DOWNLOADMONITOR_DOWNMONCORE
#define _SM_SSWR_DOWNLOADMONITOR_DOWNMONCORE
#include "Handles.h"
#include "Data/FastMap.h"
#include "Data/FastStringMap.h"
#include "Media/VideoChecker.h"
#include "Parser/ParserList.h"
#include "Sync/Event.h"
#include "Sync/Mutex.h"
#include "Sync/MutexUsage.h"

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
				NotNullPtr<Text::String> dbName;
				NotNullPtr<Text::String> fileName;
				FileStatus status;
				Sync::Mutex mut;
			};

			typedef void (__stdcall *FileEndHandler)(void *userObj, Int32 fileId, Int32 webType);
		private:
			NotNullPtr<Net::SocketFactory> sockf;
			Net::SSLEngine *ssl;
			Bool chkRunning;
			Bool chkToStop;
			Sync::Event chkEvt;
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
			Data::FastMap<Int32, FileInfo*> fileTypeMap;
			Data::FastStringMap<FileInfo*> fileNameMap;

			Bool FFMPEGMux(const UTF8Char *videoFile, const UTF8Char *audioFile, const UTF8Char *outFile);
			Bool FFMPEGMuxAAC(const UTF8Char *videoFile, const UTF8Char *audioFile, const UTF8Char *outFile);
			Bool ExtractZIP(Text::CString zipFile, Text::CString mp4File);
			Bool VideoValid(Text::CString fileName);
			void ProcessDir(Text::String *downPath, Text::String *succPath, Text::String *errPath);
			static UInt32 __stdcall CheckThread(void *userObj);

		public:
			DownMonCore();
			~DownMonCore();

			Bool IsError();
			NotNullPtr<Net::SocketFactory> GetSocketFactory();
			Net::SSLEngine *GetSSLEngine();
			CheckStatus GetCurrStatus();
			void SetFileEndHandler(FileEndHandler hdlr, void *userObj);
			Text::String *GetListFile();

			void FileFree(FileInfo *file);
			Bool FileAdd(Int32 id, Int32 webType, NotNullPtr<Text::String> dbName);
			FileInfo *FileGet(Int32 id, Int32 webType, Sync::MutexUsage *mutUsage);
			Int32 FileGetByName(Text::CString fileName, Int32 *webType);
			Bool FileEnd(Int32 id, Int32 webType);
			Bool FileStart(Int32 id, Int32 webType, ControlHandle *formHand);
			Int32 FileGetMaxId(Int32 webType);
		};
	}
}
#endif