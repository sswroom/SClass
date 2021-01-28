namespace Media
{
	class DDrawRenderer
	{
	private:
		void *ddObj;
		void *pSurface;
		void *bbSurface;
		void *vbSurface;
		void *clipper;

		void *hMainMenu;
		void *hwnd;
		Sync::Event *updEvt;
		Int32 scnUpdStat;

		Int64 updStart;
		Int32 *updDelayAll;
		Int32 updDelayInd;
		Int32 updDelayTot;
		Int32 updDelayAvg;


		Int32 scnWidth;
		Int32 scnHeight;
		Int32 fs;
		Int32 waitVBlank;

		Int32 ddLock; // 0: fullScn
		Int32 ddCount;
		Int32 framePresented;
		Int32 active; // 0 = inactive, 1 = active, 2 = active with items on top

		Int32 updateFlag; // 1 = surface need update, 2 = window need redraw
		Sync::Event *busyEvent;

		void DrawOsd(void *drawSurface, WChar *osd);
		Int32 CreateSurface(Int32 fullScreen);
		static UInt32 __stdcall ScnUpdater(void *lpVoid);
	public:
		DDrawRenderer(void *hwnd);
		~DDrawRenderer();

		void SetBusyEvent(Sync::Event *evt);
		Int32 IsBusy();
		Int32 IsError();
		Bool IsFullscreen();
		Int32 GetActive();
		void SetActive(Int32 active);
		void SetWaitVBlank(Int32 waitVBlank);

		void GetMaxSize(Int32 *width, Int32 *height);
		void GetCurrSize(Int32 *width, Int32 *height);
		void SetMainMenu(void *hMainMenu);


		Int32 PresentNext(FrameInfo *frameInfo, void *frameData, WChar *osd);
		void InitDisplay(Bool fullScreen, Int32 preferW, Int32 preferH);
		Int32 GetPresentDelay();
		void Redraw();
		void GetDrawingRect(void *rc);
	};
};
