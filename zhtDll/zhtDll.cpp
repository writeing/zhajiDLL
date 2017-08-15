// zhtDll.cpp : ���� DLL Ӧ�ó���ĵ���������
//

#include "stdafx.h"
#include "zhtDll.h"
#include "ace/INET_Addr.h"
#include "ace/SOCK_Dgram.h"
#include "ace/Time_Value.h"
#include "ace/SOCK_CODgram.h"
#include "ace/SOCK_Dgram_Bcast.h"
#include <time.h>   
#include <stdio.h>
#include <fstream>
#include<iostream>

using namespace std;
//// ���ǵ���������һ��ʾ��
//ZHTDLL_API int nzhtDll=0;
//
//// ���ǵ���������һ��ʾ����
//ZHTDLL_API int fnzhtDll(void)
//{
//	return 42;
//}
//
//// �����ѵ�����Ĺ��캯����
//// �й��ඨ�����Ϣ������� zhtDll.h
//CzhtDll::CzhtDll()
//{
//	return;
//}

class WGPacketShort {				//�̱���Э��
public:
	const static unsigned int	 WGPacketSize = 64;			    //���ĳ���
	//2015-04-29 22:22:41 const static unsigned char	 Type = 0x19;					//����
	const static unsigned char	 Type = 0x17;		//2015-04-29 22:22:50			//����
	
	const static unsigned int    SpecialFlag =0x55AAAA55;       //�����ʶ ��ֹ�����

	unsigned char	 functionID;		    //���ܺ�
	unsigned int	 iDevSn;                //�豸���к� 4�ֽ�
	unsigned char    data[56];              //56�ֽڵ����� [����ˮ��]
	unsigned int     ControllerPort;        //�������˿�
	unsigned char    recv[WGPacketSize];    //���յ�������
	WGPacketShort(void)
	{
		Reset();
	}
	void Reset()  //���ݸ�λ
	{
		memset(data,0,sizeof(data));
	}
	void toByte(char* buff, size_t buflen) //����64�ֽ�ָ���
	{
		if (buflen == WGPacketSize)
		{
			memset(buff,0,sizeof(buff));
			buff[0] = Type;
			buff[1] = functionID;
			memcpy(&(buff[4]),&(iDevSn), 4);
			memcpy(&(buff[8]),data,sizeof(data));
		}
	}
	int run(ACE_SOCK_CODgram udp)  //ͨ��ָ����UDP����ָ�� ���շ�����Ϣ
	{
		unsigned char buff[WGPacketSize];
		int errcnt =0;
		WGPacketShort::sequenceId++;
		memset(buff,0,sizeof(buff));
		buff[0] = Type;
		buff[1] = functionID;
		memcpy(&(buff[4]),&(iDevSn), 4);
		memcpy(&(buff[8]),data,sizeof(data));
		unsigned int currentSequenceId = WGPacketShort::sequenceId;
		memcpy(&(buff[40]),&(currentSequenceId), 4);
		int tries =3;
		do 
		{
			if (-1 == udp.send (buff, WGPacketSize))
			{
				return -1;
			}
			else 
			{
				ACE_INET_Addr your_addr;
				ACE_Time_Value recvTimeout(0, 400*1000);
				size_t recv_cnt = udp.recv(recv, WGPacketSize, &recvTimeout); 
				if (recv_cnt == WGPacketSize)
				{
					//��ˮ��
					unsigned int  sequenceIdReceived=0;
					memcpy(&sequenceIdReceived, &(recv[40]),4);

					if ((recv[0]== Type) //����һ��
						&& (recv[1]== functionID) //���ܺ�һ��
						&& (sequenceIdReceived == currentSequenceId) )  //���кŶ�Ӧ
					{
						return 1;
					}
					else
					{
						errcnt++;
					}
				}
			}
		} while(tries-- >0); //��������

		return -1;
	}
	static unsigned int sequenceIdSent()// ��󷢳���ˮ��
	{
		return sequenceId; // ��󷢳���ˮ��
	}
private:
	static  unsigned int     sequenceId;     //���к�	
};
unsigned int WGPacketShort::sequenceId = 0;  //��ˮ��ֵ

unsigned char GetHex(int val) //��ȡHexֵ, ��Ҫ��������ʱ���ʽ
{
	return ((val % 10) + (((val -(val % 10)) / 10)%10) *16);
}

class ControlInfo
{
public:
	int ID;
	int SN;
	char controlIP[16];
	int controlPort;
	int localPort;
	WGPacketShort pkt;
	int ConnectFlag;   // 1 connect 0 close
	FUNPTR_CALLBACK fbackcall;
	char localIP[16];
	ACE_SOCK_CODgram udp;

	ACE_SOCK_Dgram_Bcast serUdp;//(server_addr);
	void setAddr()
	{
		ACE_INET_Addr controller_addr(controlPort,controlIP);

		//cout << controlIP << endl;
		//cout << controlPort << endl;

		if (0 != udp.open(controller_addr))
		{	
			//return -1;
	/*		cout << "��������ЧIP" << endl;*/
		}
	}
	void setSerUdp(ACE_SOCK_Dgram_Bcast udp)
	{
		serUdp = udp;
	}
};

ControlInfo MC[50];
void log(char *info)  //��־��Ϣ
{
	//cout << info << data << endl;
}
void log(char *info,int data)  //��־��Ϣ
{
	//cout << info << data << endl;
}
int byteToLong(unsigned char *buff, int start, int len)
{
	int val = 0;
	for (int i = 0; i < len && i < 4; i++)
	{
		int lng = buff[i + start];
		val += (lng << (8 * i));
	}
	return val;
}
long getSnForControl(WGPacketShort pkt,int id)
{
	int ret = 0;
	int controllerSN = 0;

	pkt.Reset();
	pkt.functionID = 0x94;
	ret = pkt.run(MC[id].udp);
	log("��ȡ�豸��...");
	if (ret > 0)
	{		
		controllerSN = (int)byteToLong(pkt.recv, 4, 4);		
		log("1.1 ��ȡ�豸��.....controllerSN = ", controllerSN);
		//get sn
	}
	return controllerSN;

}

ZHTDLL_API int __stdcall zht_InitPort(int id, int iPort, int gPort, char* ControllerIP)
{
	int controllerSN = 0;
	int ret = 0;
	int success = 0;  //0 ʧ��, 1��ʾ�ɹ�    
	if (id > 10 || id < 0)
	{
		return 0;
	}
	MC[id].pkt.ControllerPort = gPort;
	MC[id].controlPort = gPort;
	cout << gPort << endl;
	memcpy(MC[id].controlIP, ControllerIP, strlen(ControllerIP));
	MC[id].setAddr();
	controllerSN = getSnForControl(MC[id].pkt,id);//223209404
	if (controllerSN == 0)
	{
		return 0;
	}
	MC[id].pkt.iDevSn = controllerSN;

	MC[id].SN = controllerSN;
	MC[id].ID = id;	
	MC[id].controlPort = gPort;
	MC[id].localPort = iPort;
	MC[id].ConnectFlag = 1;
	return controllerSN;
}
ZHTDLL_API int __stdcall zht_ClosePort(int hComm, int id)
{
	if (id > 10 || id < 0)
	{
		return -1;
	}
	if (MC[id].SN != hComm)
	{
		return -1;
	}
	MC[id].ConnectFlag = 0;  //close	
	int aa = MC[id].udp.close();	
	int bb = MC[id].serUdp.close();	
	if(aa !=0 || bb != 0)
	{
		return -1;
	}
	return 0;
}
ZHTDLL_API int __stdcall zht_SetTime(int hComm, int id, int nYear, int nMonth, int nDay, int nHour, int nMinute, int nSecond, int nWeekDay)
{
	int ret = 0;
	int success = 0;  //0 ʧ��, 1��ʾ�ɹ�  
	if ((id > 10 || id < 0) || MC[id].ConnectFlag == 0 || MC[id].SN != hComm)
	{
		return -1;
	}
	MC[id].pkt.Reset();
	MC[id].pkt.functionID = 0x30;
	nYear += 2000;
	MC[id].pkt.data[0] = GetHex((nYear - nYear % 100) / 100);
	MC[id].pkt.data[1] = GetHex((int)((nYear) % 100)); //st.GetMonth()); 
	MC[id].pkt.data[2] = GetHex(nMonth);
	MC[id].pkt.data[3] = GetHex(nDay);
	MC[id].pkt.data[4] = GetHex(nHour);
	MC[id].pkt.data[5] = GetHex(nMinute);
	MC[id].pkt.data[6] = GetHex(nSecond);
	ret = MC[id].pkt.run(MC[id].udp);
	success = 0;
	log("��������ʱ��...");
	if (ret > 0)
	{
		bool bSame = true;
		for (int i = 0; i < 7; i++)
		{
			if (MC[id].pkt.data[i] != MC[id].pkt.recv[8 + i])
			{
				bSame = false;
				break;
			}
		}
		if (bSame)
		{
			log("1.6 ��������ʱ�� �ɹ�...");
			success = 1;
		}
		else
		{
			log("1.6 ��������ʱ�� ʧ��...");
			success = 0;
		}
	}
	if (success == 1)
		return 0;
	else
		return -1;
}


int setRevIpandRevPort(WGPacketShort pkt , int id)  //���շ��������� -- ����
{
	int ret = 0;
	int success = 0;  //0 ʧ��, 1��ʾ�ɹ�

	//1.18	���ý��շ�������IP�Ͷ˿� [���ܺ�: 0x90] **********************************************************************************
	//	���շ�������IP: 192.168.168.101  [��ǰ����IP]
	//(��������ÿ�������������, ֻҪ�����շ�������IP��Ϊ0.0.0.0 ������)
	//���շ������Ķ˿�: 61005
	//ÿ��5�뷢��һ��: 05
	pkt.Reset();
	pkt.functionID = 0x90;
	pkt.iDevSn = MC[id].SN;

	//������IP: 192.168.168.101
	//pkt.data[0] = 192; 
	//pkt.data[1] = 168; 
	//pkt.data[2] = 168; 
	//pkt.data[3] = 101; 
	int watchServerPort = MC[id].localPort;
	char *watchServerIP = MC[id].localIP;

	ACE_INET_Addr watchServer_addr(watchServerPort, watchServerIP); //�˿�  IP��ַ
	unsigned int iwatchServerIPInfo = watchServer_addr.get_ip_address();
	pkt.data[0] = (iwatchServerIPInfo >> 24) & 0xff;
	pkt.data[1] = (iwatchServerIPInfo >> 16) & 0xff;
	pkt.data[2] = (iwatchServerIPInfo >> 8) & 0xff;
	pkt.data[3] = iwatchServerIPInfo & 0xff;


	//���շ������Ķ˿�: 61005
	pkt.data[4] = (watchServerPort & 0xff);
	pkt.data[5] = (watchServerPort >> 8) & 0xff;
	unsigned char lPort = pkt.data[4];
	unsigned char hPort = pkt.data[5];
	//ÿ��5�뷢��һ��: 05 (��ʱ�ϴ���Ϣ������Ϊ5�� [��������ʱÿ��5�뷢��һ��  ��ˢ��ʱ��������])
	pkt.data[6] = 5;

	ret = pkt.run(MC[id].udp);
	success = 0;
	if (ret >0)
	{
		if (pkt.recv[8] == 1)
		{
			//cout << ("1.18 ���ý��շ�������IP�Ͷ˿� 	 �ɹ�...") << endl;
			success = 1;
		}
		else
		{
			return -1;
		}
	}
	//1.19	��ȡ���շ�������IP�Ͷ˿� [���ܺ�: 0x92] **********************************************************************************
	pkt.Reset();
	pkt.functionID = 0x92;
	pkt.iDevSn = MC[id].SN;

	ret = pkt.run(MC[id].udp);
	success = 0;
	//cout << ("1.19 ��ȡ���շ�������IP�Ͷ˿�") << endl;
	if (ret >0)
	{
		if (((iwatchServerIPInfo >> 24) & 0xff) == pkt.recv[8] && ((iwatchServerIPInfo >> 16) & 0xff) == pkt.recv[9] && ((iwatchServerIPInfo >> 8) & 0xff) == pkt.recv[10] && ((iwatchServerIPInfo) & 0xff) == pkt.recv[11])
		{
			if (pkt.recv[12] == lPort && pkt.recv[13] == hPort)
			{
				//cout <<("1.19 ��ȡ���շ�������IP�Ͷ˿� 	 �ɹ�...") << endl;
				success = 1;
				return 0;
			}
		}
		//cout <<("1.19 ��ȡ���շ�������IP�Ͷ˿� 	 ʧ��...") << endl;
		success = 1;
	}
	return -1;
}
void toJson(char *root, char *key, char *value, int flag)
{
	char json[100];
	char *begin = "[{";
	char *temp = "}]";
	char* temp_1 = "\"";
	char* temp_2 = ":";
	char* temp_3 = ",";
	if (strlen(root) == 0)
	{
		strcat(root, begin);
	}
	//���һ��dict
	memset(json,'\0',100);
	strcat_s(json, temp_1);  //"
	strcat_s(json, key);  //key
	strcat_s(json, temp_1);  //"
	strcat_s(json, temp_2);  //:
	strcat_s(json, temp_1);  //"
	strcat_s(json, value);  //value
	strcat_s(json, temp_1);  //"
	if (flag == 0)
	{
		strcat_s(json, temp_3);  //,
	}
	else
	{
		strcat(json, temp);
	}
	strcat(root, json);  //"

}


int zht_WatchingServerRuning(char *watchServerIP, int watchServerPort,int id)
{
	//ע�����ǽ Ҫ����˶˿ڵ����а��������
	ACE_INET_Addr server_addr(static_cast<u_short> (watchServerPort), watchServerIP);
	ACE_SOCK_Dgram_Bcast udp(server_addr);
	MC[id].setSerUdp(udp);
	unsigned char buff[WGPacketShort::WGPacketSize];
	size_t buflen = sizeof(buff);
	ssize_t recv_cnt;
	//cout << ("������շ��������״̬....") << endl;	
	unsigned int recordIndex = 0;
	char root[200] = { 0 };
	memset(root, '\0', sizeof(root));
	char temp[10];
	while (true)
	{
		ACE_INET_Addr any_addr;
		recv_cnt = udp.recv(buff, buflen, any_addr);
		if (recv_cnt > 0)
		{
			if (recv_cnt == WGPacketShort::WGPacketSize)
			{
				if (buff[1] == 0x20) //
				{
					unsigned int sn;
//					unsigned int recordIndexGet;
					memcpy(&sn, &(buff[4]), 4);
					//printf("���յ����Կ�����SN = %d �����ݰ�..\r\n", sn);

					/*memcpy(&recordIndexGet, &(buff[8]), 4);
					if (recordIndex < recordIndexGet)
					{
						recordIndex = recordIndexGet;
						displayRecordInformation(buff);
					}
					*/
					//���յ����ݣ�����IDֵ���з���
					memset(root, '\0', 200);
					//TIME
					char controllerTime[] = "2000.01.01 00:00:00"; //��������ǰʱ��
					sprintf_s(controllerTime, "20%02X.%02X.%02X %02X:%02X:%02X",
						buff[51], buff[52], buff[53], buff[37], buff[38], buff[39]);
					toJson(root, "Time", controllerTime,0);					
					//ID
					memset(temp, '\0', 10);
					_itoa_s(id, temp, 10);
					//sprintf(temp, "%d", id);
					toJson(root, "nID", temp, 0);					
					//14	�ź�(1,2,3,4)	1	
					int recordDoorNO = buff[14];
					memset(temp, '\0', 10);
					_itoa_s(recordDoorNO, temp, 10);
					//sprintf(temp, "%d", id);
					toJson(root, "ChannelID", temp, 0);					
					//16-19	����(������ˢ����¼ʱ)
					//����(�������ͼ�¼)	4	
					int recordCardNO = 0;
					memcpy(&recordCardNO, &(buff[16]), 4);					
					memset(temp, '\0', 10);					
					_itoa(recordCardNO, temp, 10);										
					toJson(root, "UID", temp, 0);					

					//15	����/����(1��ʾ����, 2��ʾ����)	1	0x01
					int recordInOrOut = buff[15];
					memset(temp, '\0', 10);					
					if (recordInOrOut == 1)
					{//����
						memcpy(temp, "IN", 2);
					}
					else
					{
						memcpy(temp, "OUT", 3);
					}
					toJson(root, "direction", temp, 0);
					//13	��Ч��(0 ��ʾ��ͨ��, 1��ʾͨ��)	1	
					int recordValid = buff[13];
					memset(temp, '\0', 10);
					if (recordValid == 1)
					{//ͨ��
						toJson(root, "Count", "2", 1); //����
					}
					else
					{//δͨ��
						toJson(root, "Count", "1", 1); //����
					}
					//json��װ���
					MC[id].fbackcall(id, 1, root);
				}
			}
		}
	}
	udp.close();
	return 0;
}
int *arrayIndex;
void getIDCardIndex(char *data)
{
	unsigned int i = 0;
	unsigned int j = 0;		
	arrayIndex[i++] = -1;
	while(j < strlen(data))
	{
		if (data[j] == ';')
		{			
			arrayIndex[i++] = j;
			//log("whitet list index = ", j);
		}
		j++;
	}	
	//arrayIndex[i] = 0;
}
int getIDforCard(char *data, int index)
{
	char temp[12] = {0};
	int dataIndex = arrayIndex[index];
	int len = 10;
	if(arrayIndex[index+1] == 0)
		return 0;
		//len = arrayIndex[index + 1] - arrayIndex[index] - 1;	
	//memset(temp,'\0',10);
	strncat_s(temp, &data[dataIndex+1], len);
	//log("whitet num = ", atoi(temp));
	return atoi(temp);
}
ZHTDLL_API int __stdcall zht_AddtoWhitelist(int hComm, int id, char *cardid)
{
	int ret = 0;
	int success = 0;  //0 ʧ��, 1��ʾ�ɹ�  
	if ((id > 10 || id < 0) || MC[id].ConnectFlag == 0 || MC[id].SN != hComm || strlen(cardid) < 10)
	{
		return -1;
	}
	WGPacketShort pkt = MC[id].pkt;

	int cardNOOfPrivilege = 0;
	int index = 0;
	arrayIndex = new int[int(strlen(cardid) / 11) + 1 ];	
	memset(arrayIndex, 0, sizeof(int)*(strlen(cardid) / 11) + 1 );
	getIDCardIndex(cardid);

	while(1)
	{
		cardNOOfPrivilege = getIDforCard(cardid, index++);
		//log("1.11 Ȩ����ӻ��޸� = ", cardNOOfPrivilege);
		if (cardNOOfPrivilege == 0)
			break;
		success = 0;
		pkt.Reset();
		pkt.functionID = 0x50;
		pkt.iDevSn = hComm;
		//0D D7 37 00 Ҫ��ӻ��޸ĵ�Ȩ���еĿ��� = 0x0037D70D = 3659533 (ʮ����)
		//int cardNOOfPrivilege = 0x0037D70D;
		memcpy(&(pkt.data[0]), &cardNOOfPrivilege, 4);
		//20 10 01 01 ��ʼ����:  2010��01��01��   (�������2001��)
		pkt.data[4] = 0x20;
		pkt.data[5] = 0x10;
		pkt.data[6] = 0x01;
		pkt.data[7] = 0x01;
		//20 29 12 31 ��ֹ����:  2029��12��31��
		pkt.data[8] = 0x20;
		pkt.data[9] = 0x29;
		pkt.data[10] = 0x12;
		pkt.data[11] = 0x31;
		//01 ����ͨ�� һ���� [�Ե���, ˫��, ���ſ�������Ч] 
		pkt.data[12] = 0x01;
		//01 ����ͨ�� ������ [��˫��, ���ſ�������Ч]
		pkt.data[13] = 0x01;  //�����ֹ2����, ��ֻҪ��Ϊ 0x00
							  //01 ����ͨ�� ������ [�����ſ�������Ч]
		pkt.data[14] = 0x01;
		//01 ����ͨ�� �ĺ��� [�����ſ�������Ч]
		pkt.data[15] = 0x01;

		ret = pkt.run(MC[id].udp);
		success = 0;
		log("1.11 Ȩ����ӻ��޸�");
		if (ret >0)
		{
			if (pkt.recv[8] == 1)
			{
				//��ʱ ˢ����Ϊ= 0x0037D70D = 3659533 (ʮ����)�Ŀ�, 1���ż̵�������.
				log("1.11 Ȩ����ӻ��޸�	 �ɹ�...");
				success = 1;
			}
		}
		if(success == 0)
		{
			break;
		}
	}
	delete[] arrayIndex;
	if(success == 1)
	{
		return 0;
	}
	else
	{
		return -1;
	}
}

ZHTDLL_API int __stdcall zht_SetCallbackAddr(int hComm, int id, FUNPTR_CALLBACK callback , char *localIP)
{
	int ret = 0;
	int success = 0;  //0 ʧ��, 1��ʾ�ɹ�  
	if ((id > 10 || id < 0) || MC[id].ConnectFlag == 0 || MC[id].SN != hComm)
	{
		return -1;
	}
	//cout << "begin server" << endl;
	memcpy(MC[id].localIP, localIP,strlen(localIP));
	MC[id].fbackcall = callback;

	if (setRevIpandRevPort(MC[id].pkt, id) == -1)
	{
		return -1;
	}
	zht_WatchingServerRuning(MC[id].localIP, MC[id].localPort, id);
	return 0;
}