#include "BaseInc.h"
#include "FTPState.h"


map<int, string> mpStateDesc;

string GetDescript(int state)
{
	mpStateDesc.clear();
	mpStateDesc[110] = "重启标志位，后面的提示信息标志服务器状态，提示信息固定";
	mpStateDesc[120] = "	服务器准备就绪的时间";
	mpStateDesc[125] = "数据连接已正常开启，开始传输文件";
	mpStateDesc[150] = "文件状态正常，开始开启数据连接";
	mpStateDesc[200] = "命令执行成功";
	mpStateDesc[202] = "命令尚未实现，该站点不支持该命令";
	mpStateDesc[211] = "系统状态, 或系统帮助信息";
	mpStateDesc[212] = "目录状态";
	mpStateDesc[213] = "文件状态";
	mpStateDesc[214] = "命令帮助信息";
	mpStateDesc[215] = "该系统类型";
	mpStateDesc[220] = "服务已为新连接的用户就绪";
	mpStateDesc[221] = "服务退出控制连接";
	mpStateDesc[225] = "打开数据连接，未开始数据传输";
	mpStateDesc[226] = "结束数据连接，数据传输已完成";
	mpStateDesc[227] = "进入被动模式（IP 地址、ID 端口）";
	mpStateDesc[230] = "用户登入成功";
	mpStateDesc[250] = "请求文件操作完成";
	mpStateDesc[257] = "路径名建立成功";
	mpStateDesc[331] = "用户名正确，需要用户密码";
	mpStateDesc[332] = "需要登入用户";
	mpStateDesc[350] = "请求文件的工作需要得到进一步确认";
	mpStateDesc[421] = "服务不可能，即将关闭控制连接";
	mpStateDesc[425] = "无法打开数据连接";
	mpStateDesc[426] = "连接中止，传输关闭。";
	mpStateDesc[450] = "请求文件尚不可用，文件可能被占用";
	mpStateDesc[451] = "请求操作终止，遇到本地错误";
	mpStateDesc[452] = "	请求操作终止，磁盘空间不足";
	mpStateDesc[500] = "	命令语法错误，可能包含命令过长的错误";
	mpStateDesc[501] = "命令参数错误";
	mpStateDesc[502] = "	命令尚未被执行";
	mpStateDesc[503] = "命令次序错误";
	mpStateDesc[504] = "该命令尚未被完成";
	mpStateDesc[530] = "	尚未登入服务器";
	mpStateDesc[532] = "存储文件需要帐号";
	mpStateDesc[550] = "	请求的动作尚未执行，文件不可用";
	mpStateDesc[551] = "	请求操作终止，不知道的页类型";
	mpStateDesc[552] = "	请求操作终止，超出用户配额";
	mpStateDesc[553] = "	请求操作终止，文件名不允许";
	if (mpStateDesc.find(state) != mpStateDesc.end())
	{
		return mpStateDesc[state];
	}
	return "state not found";
}
