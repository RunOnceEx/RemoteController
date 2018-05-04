#include <string>

// ����һ����������(cmd:arg - ð���м�û�пո�, argΪ�ձ�ʾ�޲���)
inline std::string MAKE_CMD(const char *cmd, const char *arg)
{
	char buffer[64];
	sprintf_s(buffer, "%s%s%s", cmd, arg ? ":" : "", arg ? arg : "");
	return buffer;
}

// ����һ����������(���ز�����Ϣ)
inline std::string PARSE_CMD(const char *msg, char *out_cmd)
{
	const char *p = msg;
	while (*p && ':' != *p) ++p;
	memcpy(out_cmd, msg, p - msg);
	return (':' == *p) ? p + 1 : "";
}

#define RESTART			"restart"

#define REFRESH			"refresh"

#define STOP			"stop"

#define START			"start"

#define SHUTDOWN		"shutdown"

#define REBOOT			"reboot"

#define REGISTER		"register"

#define KEEPALIVE		"keepAlive"

#define UPDATE			"update"

#define SETTIME			"settime"
