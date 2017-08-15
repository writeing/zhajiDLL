// 下列 ifdef 块是创建使从 DLL 导出更简单的
// 宏的标准方法。此 DLL 中的所有文件都是用命令行上定义的 ZHTDLL_EXPORTS
// 符号编译的。在使用此 DLL 的
// 任何其他项目上不应定义此符号。这样，源文件中包含此文件的任何其他项目都会将
// ZHTDLL_API 函数视为是从 DLL 导入的，而此 DLL 则将用此宏定义的
// 符号视为是被导出的。
#ifdef ZHTDLL_EXPORTS
#define ZHTDLL_API __declspec(dllexport)
#else
#define ZHTDLL_API __declspec(dllimport)
#endif

// 此类是从 zhtDll.dll 导出的
//class ZHTDLL_API CzhtDll {
//public:
//	CzhtDll(void);
//	// TODO: 在此添加您的方法。
//};
//
//extern ZHTDLL_API int nzhtDll;
//
//ZHTDLL_API int fnzhtDll(void);
typedef void(*FUNPTR_CALLBACK)(int nID, int eventType, void* param);
ZHTDLL_API int __stdcall t_AddtoWhitelist(int hComm, int id, char *cardid);
ZHTDLL_API int __stdcall zht_ClosePort(int hComm, int id);
ZHTDLL_API int __stdcall zht_InitPort(int id, int iPort, int gPort, char* ControllerIP);
ZHTDLL_API int __stdcall zht_SetCallbackAddr(int hComm, int id, FUNPTR_CALLBACK callback, char *localIP);
ZHTDLL_API int __stdcall zht_SetTime(int hComm, int id, int nYear, int nMonth, int nDay, int nHour, int nMinute, int nSecond, int nWeekDay);
