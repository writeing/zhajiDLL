// ���� ifdef ���Ǵ���ʹ�� DLL �������򵥵�
// ��ı�׼�������� DLL �е������ļ��������������϶���� ZHTDLL_EXPORTS
// ���ű���ġ���ʹ�ô� DLL ��
// �κ�������Ŀ�ϲ�Ӧ����˷��š�������Դ�ļ��а������ļ����κ�������Ŀ���Ὣ
// ZHTDLL_API ������Ϊ�Ǵ� DLL ����ģ����� DLL ���ô˺궨���
// ������Ϊ�Ǳ������ġ�
#ifdef ZHTDLL_EXPORTS
#define ZHTDLL_API __declspec(dllexport)
#else
#define ZHTDLL_API __declspec(dllimport)
#endif

// �����Ǵ� zhtDll.dll ������
//class ZHTDLL_API CzhtDll {
//public:
//	CzhtDll(void);
//	// TODO: �ڴ�������ķ�����
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
