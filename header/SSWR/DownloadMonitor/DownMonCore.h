#ifndef _SM_SSWR_DOWNLOADMONITOR_DOWNMONCORE
#define _SM_SSWR_DOWNLOADMONITOR_DOWNMONCORE
#include "Data/Integer32Map.h"
#include "Data/StringUTF8Map.h"
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

			typedef struct
			{
				Int32 id;
				Int32 webType;
				const UTF8Char *dbName;
				const UTF8Char *fileName;
				FileStatus status;
				Sync::Mutex *mut;
			} FileInfo;

			typedef void (__stdcall *FileEndHandler)(void *userObj, Int32 fileId, Int32 webType);
		private:
			Net::SocketFactory *sockf;
			Bool chkRunning;
			Bool chkToStop;
			Sync::Event *chkEvt;
			CheckStatus chkStatus;
			FileEndHandler fileEndHdlr;
			void *fileEndObj;

			const UTF8Char *downPath;
			const UTF8Char *succPath;
			const UTF8Char *errPath;
			const UTF8Char *ytPath;
			const UTF8Char *ffmpegPath;
			const UTF8Char *firefoxPath;
			const UTF8Char *listFile;
			Parser::ParserList *parsers;
			Media::VideoChecker *checker;

			Sync::Mutex *fileMut;
			Data::Integer32Map<FileInfo*> *fileTypeMap;
			Data::StringUTF8Map<FileInfo*> *fileNameMap;

			Bool FFMPEGMux(const UTF8Char *videoFile, const UTF8Char *audioFile, const UTF8Char *outFile);
			Bool FFMPEGMuxAAC(const UTF8Char *videoFile, const UTF8Char *audioFile, const UTF8Char *outFile);
			Bool ExtractZIP(const UTF8Char *zipFile, const UTF8Char *mp4File);
			Bool VideoValid(const UTF8Char *fileName);
			void ProcessDir(const UTF8Char *downPath, const UTF8Char *succPath, const UTF8Char *errPath);
			static UInt32 __stdcall CheckThread(void *userObj);

		public:
			DownMonCore();
			~DownMonCore();

			Bool IsError();
			Net::SocketFactory *GetSocketFactory();
			CheckStatus GetCurrStatus();
			void SetFileEndHandler(FileEndHandler hdlr, void *userObj);
			const UTF8Char *GetListFile();

			void FileFree(FileInfo *file);
			Bool FileAdd(Int32 id, Int32 webType, const UTF8Char *dbName);
			FileInfo *FileGet(Int32 id, Int32 webType, Sync::MutexUsage *mutUsage);
			Int32 FileGetByName(const UTF8Char *fileName, Int32 *webType);
			Bool FileEnd(Int32 id, Int32 webType);
			Bool FileStart(Int32 id, Int32 webType, void *formHand);
			Int32 FileGetMaxId(Int32 webType);
		};
	}
}
#endif