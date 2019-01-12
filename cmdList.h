#include <string>

// ����һ����������(cmd:arg - ð���м�û�пո�, argΪ�ձ�ʾ�޲���)
inline std::string MAKE_CMD(const char *cmd, const char *arg)
{
	char buffer[1024];
	sprintf_s(buffer, "%s%s%s", cmd, arg ? ":" : "", arg ? arg : "");
	return buffer;
}

// ����һ����������(���ز�����Ϣ)
inline std::string PARSE_CMD(const char *msg, char *out_cmd)
{
	const char *p = msg;
	while (*p && ':' != *p) ++p;
	memcpy(out_cmd, msg, p - msg);
	out_cmd[p-msg] = 0;
	return (':' == *p) ? p + 1 : "";
}

// Ĭ���������ڣ��룩
#define ALIVE_TIME 120

// ffmpeg������ʼ�˿�
#define _BASE_PORT 6666

#define RESTART			"restart"		// ��������

#define REFRESH			"refresh"		// ˢ����ʾ

#define STOP			"stop"			// ֹͣ����

#define START			"start"			// ��������

#define SHUTDOWN		"shutdown"		// �ػ�

#define REBOOT			"reboot"		// ��������

#define REGISTER		"register"		// ע��=>register:success

#define KEEPALIVE		"keepAlive"		// ����=>keeAlive:AliveTime

#define UPDATE			"update"		// ��������=>update:pos/a/+/-

#define SETTIME			"settime"		// Уʱ=>settime:��,��,��,ʱ,��,��

#define NOTICE			"notice"		// ����=>notice:Infomation

#define PAUSE			"pause"			// ��ͣ

#define ALLOW_DEBUG		"debug"			// ����Ӧ�ó��򽵼�

#define SETIP			"setip"			// �����ػ�����Զ��IP=>setip:IP

#define SETPORT			"setport"		// �����ػ�����Զ�̶˿�=>setport:PORT

#define WATCH			"watch"			// ���Ӵ˳�������״̬=>watch:Port
