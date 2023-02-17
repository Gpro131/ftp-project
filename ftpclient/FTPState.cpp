#include "BaseInc.h"
#include "FTPState.h"


map<int, string> mpStateDesc;

string GetDescript(int state)
{
	mpStateDesc.clear();
	mpStateDesc[110] = "������־λ���������ʾ��Ϣ��־������״̬����ʾ��Ϣ�̶�";
	mpStateDesc[120] = "	������׼��������ʱ��";
	mpStateDesc[125] = "����������������������ʼ�����ļ�";
	mpStateDesc[150] = "�ļ�״̬��������ʼ������������";
	mpStateDesc[200] = "����ִ�гɹ�";
	mpStateDesc[202] = "������δʵ�֣���վ�㲻֧�ָ�����";
	mpStateDesc[211] = "ϵͳ״̬, ��ϵͳ������Ϣ";
	mpStateDesc[212] = "Ŀ¼״̬";
	mpStateDesc[213] = "�ļ�״̬";
	mpStateDesc[214] = "���������Ϣ";
	mpStateDesc[215] = "��ϵͳ����";
	mpStateDesc[220] = "������Ϊ�����ӵ��û�����";
	mpStateDesc[221] = "�����˳���������";
	mpStateDesc[225] = "���������ӣ�δ��ʼ���ݴ���";
	mpStateDesc[226] = "�����������ӣ����ݴ��������";
	mpStateDesc[227] = "���뱻��ģʽ��IP ��ַ��ID �˿ڣ�";
	mpStateDesc[230] = "�û�����ɹ�";
	mpStateDesc[250] = "�����ļ��������";
	mpStateDesc[257] = "·���������ɹ�";
	mpStateDesc[331] = "�û�����ȷ����Ҫ�û�����";
	mpStateDesc[332] = "��Ҫ�����û�";
	mpStateDesc[350] = "�����ļ��Ĺ�����Ҫ�õ���һ��ȷ��";
	mpStateDesc[421] = "���񲻿��ܣ������رտ�������";
	mpStateDesc[425] = "�޷�����������";
	mpStateDesc[426] = "������ֹ������رա�";
	mpStateDesc[450] = "�����ļ��в����ã��ļ����ܱ�ռ��";
	mpStateDesc[451] = "���������ֹ���������ش���";
	mpStateDesc[452] = "	���������ֹ�����̿ռ䲻��";
	mpStateDesc[500] = "	�����﷨���󣬿��ܰ�����������Ĵ���";
	mpStateDesc[501] = "�����������";
	mpStateDesc[502] = "	������δ��ִ��";
	mpStateDesc[503] = "����������";
	mpStateDesc[504] = "��������δ�����";
	mpStateDesc[530] = "	��δ���������";
	mpStateDesc[532] = "�洢�ļ���Ҫ�ʺ�";
	mpStateDesc[550] = "	����Ķ�����δִ�У��ļ�������";
	mpStateDesc[551] = "	���������ֹ����֪����ҳ����";
	mpStateDesc[552] = "	���������ֹ�������û����";
	mpStateDesc[553] = "	���������ֹ���ļ���������";
	if (mpStateDesc.find(state) != mpStateDesc.end())
	{
		return mpStateDesc[state];
	}
	return "state not found";
}
