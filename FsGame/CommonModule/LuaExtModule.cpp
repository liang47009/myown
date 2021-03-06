//--------------------------------------------------------------------
// 文件名:		LuaExtModule.h
// 内  容:		定义LUA脚本的扩展函数
// 说  明:		
// 创建日期:	2014年06月10日
// 整理日期:	2014年06月10日
// 创建人:		  ( )
// 修改人:     
//    :	    
//--------------------------------------------------------------------

#include "LuaExtModule.h"
#include "FsGame/Define/ServerCustomDefine.h"
#include "public/VarList.h"
#include "utils/custom_func.h"
#include "utils/util_func.h"
#include "utils/extend_func.h"
#include "utils/string_util.h"
#include "FsGame/Define/ViewDefine.h"
#include "FsGame/CommonModule/LuaScriptModule.h"
#include "FsGame/CommonModule/AsynCtrlModule.h"
#include "FsGame/ItemModule/ItemBaseModule.h"
//#include "FsGame/SceneBaseModule/SecretSceneModule.h"
#include "FsGame/Define/CommandDefine.h"
#include "FsGame/Define/PubDefine.h"
#include "FsGame/Define/SnsDefine.h"
#include <windows.h>
#include <time.h>
#include "FsGame/NpcBaseModule/AI/AIFunction.h"
//#include "FsGame/ArenaModule/ArenaModule.h"
#include "FsGame/Define/SceneListDefine.h"

AsynCtrlModule * g_pAsynCtrlModule = NULL;

// 原型：int nx_msgbox(string info)
// 功能：显示一个Windows信息对话框
int nx_msgbox(void * state)
{
	// 获得核心指针
	IKernel * pKernel = ILuaExtModule::GetKernel(state);

	// 检查参数数量
	CHECK_ARG_NUM(state, nx_msgbox, 1);

	// 检查参数类型
	CHECK_ARG_STRING(state, nx_msgbox, 1);

	// 获取参数
	const char * msg = pKernel->LuaToString(state, 1);
#ifndef NDEBUG
	::MessageBox(NULL, msg, "lua extension", MB_OK);
#else

	pKernel->Trace(msg);

#endif
	// 填入返回值
	pKernel->LuaPushInt(state, 1);
	// 表示返回值数量为1
	return 1;
}

// 原型：void nx_trace(string info)
// 功能：输出一段信息到Trace Log文件
int nx_trace(void * state)
{
	// 获得核心指针
	IKernel * pKernel = ILuaExtModule::GetKernel(state);

	// 检查参数数量
	CHECK_ARG_NUM(state, nx_trace, 1);
	// 检查参数类型
	CHECK_ARG_STRING(state, nx_trace, 1);
	// 获取参数
	const char * msg = pKernel->LuaToString(state, 1);

	pKernel->Trace(msg);
	// 表示返回值数量为0
	return 0;
}

// 原型：void nx_echo(string info)
// 功能：输出一段信息到Echo窗口
int nx_echo(void * state)
{
	// 获得核心指针
	IKernel * pKernel = ILuaExtModule::GetKernel(state);

	// 检查参数数量
	CHECK_ARG_NUM(state, nx_echo, 1);
	// 检查参数类型
	CHECK_ARG_STRING(state, nx_echo, 1);
	// 获取参数
	const char * msg = pKernel->LuaToString(state, 1);

	extend_warning(pKernel,msg);
	// 表示返回值数量为0
	return 0;
}

// 原型：bool nx_findmodule(string name)
// 功能：查找指定名称的功能模块是否已被加载
int nx_findmodule(void * state)
{
	// 获得核心指针
	IKernel * pKernel = ILuaExtModule::GetKernel(state);

	// 检查参数数量
	CHECK_ARG_NUM(state, nx_findmodule, 1);
	// 检查参数类型
	CHECK_ARG_STRING(state, nx_findmodule, 1);
	// 获取参数
	const char * name = pKernel->LuaToString(state, 1);

	pKernel->LuaPushBool(state, pKernel->GetLogicModule(name) != NULL);
	// 表示返回值数量为1
	return 1;
}

#ifndef FSROOMLOGIC_EXPORTS
// 原型：int nx_get_district_id()
// 功能：获得当前分区号
int nx_get_district_id(void * state)
{
	// 获得核心指针
	IKernel * pKernel = ILuaExtModule::GetKernel(state);

	pKernel->LuaPushInt(state, pKernel->GetDistrictId());
	// 表示返回值数量为1
	return 1;
}

// 原型：int nx_get_server_id()
// 功能：获得当前服务器编号
int nx_get_server_id(void * state)
{
	// 获得核心指针
	IKernel * pKernel = ILuaExtModule::GetKernel(state);

	pKernel->LuaPushInt(state, pKernel->GetServerId());
	// 表示返回值数量为1
	return 1;
}
#endif // FSROOMLOGIC_EXPORTS

//////////////////////////////////////////////////////////////////////
// 原型：bool nx_sysinfo(object obj, int type, fast_string fast_stringid, ...);
// 功能：发送系统信息，使用服务端的字符串ID
int nx_sysinfo(void * state)
{
	IKernel * pKernel = ILuaExtModule::GetKernel(state);

	// 检查参数数量
	const int MIN_ARGS = 2;

	int arg_num = pKernel->LuaGetArgCount(state);
	if (arg_num < MIN_ARGS)
	{
		pKernel->LuaErrorHandler(state, "nx_sysinfo argument number less than 3");
		return 1;
	}

	// 检查参数类型
	CHECK_ARG_OBJECT(state, nx_sysinfo, 1);
	CHECK_ARG_INT(state, nx_sysinfo, 2);

	// 获取参数
	PERSISTID obj = pKernel->LuaToObject(state, 1);
	int info_id = pKernel->LuaToInt(state, 2);
/*
#ifndef _DEBUG
    //#ifdef _DEBUG
    //    TIPSTYPE_DEBUGINFO = 100,   // 仅用于开发调用的信息输出，防止在脚本调试输出后忘记屏蔽
    //#endif
    // 如果在正式环境下使用了这个类型，忽略它
  //  if ( TIPSTYPE_DEBUGINFO == type )
    //    return 0;
#endif*/

	CVarList msg;
	if (arg_num > MIN_ARGS)
		ILuaExtModule::GetVarList(state, MIN_ARGS + 1, arg_num, msg);

	pKernel->LuaPushBool(state, ::CustomSysInfo(pKernel, obj, info_id, msg));

	return 1;
}

// 原型：bool nx_sysinfo_byname(wstring name, int type, fast_string fast_stringid, ...);
// 功能：发送系统信息，使用服务端的字符串ID
int nx_sysinfo_byname(void * state)
{
	IKernel * pKernel = ILuaExtModule::GetKernel(state);

	// 检查参数数量
	const int MIN_ARGS = 2;
	int arg_num = pKernel->LuaGetArgCount(state);
	if (arg_num < MIN_ARGS)
	{
		pKernel->LuaErrorHandler(state, "nx_sysinfo_byname argument number less than 3");
		return 1;
	}

	// 检查参数类型
	CHECK_ARG_WIDESTR(state, nx_sysinfo_byname, 1);
	CHECK_ARG_INT(state, nx_sysinfo_byname, 2);

	// 获取参数
	const wchar_t * name = pKernel->LuaToWideStr(state, 1);
	int info_id = pKernel->LuaToInt(state, 2);

	CVarList msg;
	if (arg_num > MIN_ARGS)
		ILuaExtModule::GetVarList(state, MIN_ARGS + 1, arg_num, msg);

	pKernel->LuaPushBool(state, ::CustomSysInfoByName(pKernel, name, info_id, msg));
	return 1;
}

// 原型：bool nx_sysinfo_bychannel(int channel, int type, fast_string fast_stringid, ...);
// 功能：发送系统信息，使用服务端的字符串ID
int nx_sysinfo_bychannel(void * state)
{
	IKernel * pKernel = ILuaExtModule::GetKernel(state);

	// 检查参数数量
	const int MIN_ARGS = 2;
	int arg_num = pKernel->LuaGetArgCount(state);
	if (arg_num < MIN_ARGS)
	{
		pKernel->LuaErrorHandler(state, "nx_sysinfo argument number less than 3");
		return 1;
	}

	// 检查参数类型
	CHECK_ARG_INT(state, nx_sysinfo_bychannel, 1);
	CHECK_ARG_INT(state, nx_sysinfo_bychannel, 2);

	// 获取参数
	int channel = pKernel->LuaToInt(state, 1);
	int info_id = pKernel->LuaToInt(state, 2);
	const char * fast_stringid = pKernel->LuaToString(state, 3);

	CVarList msg;
	if (arg_num > MIN_ARGS)
		ILuaExtModule::GetVarList(state, MIN_ARGS + 1, arg_num, msg);

	pKernel->LuaPushBool(state, ::CustomSysInfoByChannel(pKernel, channel, info_id, msg));
	return 1;
}

// 原型：bool nx_sysinfo_byscene(int sceneid, int type, fast_string fast_stringid, ...);
// 功能：发送系统信息，使用服务端的字符串ID
int nx_sysinfo_byscene(void * state)
{
	IKernel * pKernel = ILuaExtModule::GetKernel(state);

	// 检查参数数量
	const int MIN_ARGS = 3;
	int arg_num = pKernel->LuaGetArgCount(state);
	if (arg_num < MIN_ARGS)
	{
		pKernel->LuaErrorHandler(state, "nx_sysinfo_byscene argument number less than 3");
		return 1;
	}

	// 检查参数类型
	CHECK_ARG_INT(state, nx_sysinfo_byscene, 1);
	CHECK_ARG_INT(state, nx_sysinfo_byscene, 2);

	// 获取参数
	int sceneid = pKernel->LuaToInt(state, 1);
	int info_id = pKernel->LuaToInt(state, 2);

	CVarList msg;
	if (arg_num > MIN_ARGS)
		ILuaExtModule::GetVarList(state, MIN_ARGS + 1, arg_num, msg);

	pKernel->LuaPushBool(state, ::CustomSysInfoByScene(pKernel, sceneid, info_id, msg));
	return 1;
}

// 原型：bool nx_sysinfo_bygroup(int groupid, int type, fast_string fast_stringid, ...);
// 功能：发送系统信息，使用服务端的字符串ID
int nx_sysinfo_bygroup(void * state)
{
	IKernel * pKernel = ILuaExtModule::GetKernel(state);

	// 检查参数数量
	const int MIN_ARGS = 2;
	int arg_num = pKernel->LuaGetArgCount(state);
	if (arg_num < MIN_ARGS)
	{
		pKernel->LuaErrorHandler(state, "nx_sysinfo_bygroup argument number less than 3");
		return 1;
	}

	// 检查参数类型
	CHECK_ARG_INT(state, nx_sysinfo_bygroup, 1);
	CHECK_ARG_INT(state, nx_sysinfo_bygroup, 2);

	// 获取参数
	int groupid = pKernel->LuaToInt(state, 1);
	int info_id = pKernel->LuaToInt(state, 2);


	CVarList msg;
	if (arg_num > MIN_ARGS)
		ILuaExtModule::GetVarList(state, MIN_ARGS + 1, arg_num, msg);

	pKernel->LuaPushBool(state, ::CustomSysInfoByGroup(pKernel, groupid, info_id, msg));
	return 1;
}

// 原型：bool nx_sysinfo_broadcast(int type, fast_string fast_stringid, ...);
// 功能：发送系统信息，使用服务端的字符串ID
int nx_sysinfo_broadcast(void * state)
{
	IKernel * pKernel = ILuaExtModule::GetKernel(state);

	// 检查参数数量
	const int MIN_ARGS = 2;
	int arg_num = pKernel->LuaGetArgCount(state);
	if (arg_num < MIN_ARGS)
	{
		pKernel->LuaErrorHandler(state, "nx_sysinfo_broadcast argument number less than 3");
		return 1;
	}

	// 检查参数类型
	CHECK_ARG_INT(state, nx_sysinfo_broadcast, 1);

	// 获取参数
	int info_id = pKernel->LuaToInt(state, 1);

	CVarList msg;
	if (arg_num > MIN_ARGS)
		ILuaExtModule::GetVarList(state, MIN_ARGS + 1, arg_num, msg);

	pKernel->LuaPushBool(state, ::CustomSysInfoBroadcast(pKernel, info_id, msg));
	return 1;
}

// 原型：int nx_custom(object obj, ...)
// 功能：发自定义消息到obj的客户端，target是自定义消息的目标对象
// 注意：在传CVarList可变长常数参数时，必须用nx_float来明确的表示浮点数
// nx_custom(obj, target, 1, 0.1) -- 错误，0.1会被认为是0
// nx_custom(obj, target, 1, nx_float(0.1)) -- 正确，在CVarList里是浮点数了
int nx_custom(void * state)
{
	IKernel * pKernel = ILuaExtModule::GetKernel(state);

	int arg_num = pKernel->LuaGetArgCount(state);

	if (arg_num < 2)
	{
		pKernel->LuaErrorHandler(state, "nx_custom argument less then 2");
		return 1;
	}

	CHECK_ARG_OBJECT(state, nx_custom, 1);

	PERSISTID obj = pKernel->LuaToObject(state, 1);

	CVarList msg;

	if (!ILuaExtModule::GetVarList(state, 2, arg_num, msg))
	{
		pKernel->LuaErrorHandler(state, "nx_custom args error");
		return 1;
	}

	pKernel->LuaPushBool(state, pKernel->Custom(obj, msg));

	return 1;
}

// 原型：bool nx_custom_byname(wstring player, ...)
// 功能：通过玩家名称向客户端发送自定义消息
int nx_custom_byname(void * state)
{
	IKernel * pKernel = ILuaExtModule::GetKernel(state);

	int arg_num = pKernel->LuaGetArgCount(state);
	if (arg_num < 2)
	{
		pKernel->LuaErrorHandler(state, "nx_command argument number less than 2");
		return 1;
	}

	// 检查参数类型
	CHECK_ARG_WIDESTR(state, nx_custom_byname, 1);

	const wchar_t* name = pKernel->LuaToWideStr(state, 1);

	CVarList msg;
	if (!ILuaExtModule::GetVarList(state, 2, arg_num, msg))
	{
		pKernel->LuaErrorHandler(state, "nx_custom_byname args error");
		return 1;
	}

	pKernel->LuaPushBool(state, pKernel->CustomByName(name, msg));
	return 1;
}

// 原型：bool nx_custom_byken(object self, ...)
// 功能：通过玩家名称向客户端发送自定义消息
int nx_custom_byken(void * state)
{
	IKernel * pKernel = ILuaExtModule::GetKernel(state);

	int arg_num = pKernel->LuaGetArgCount(state);
	if (arg_num < 2)
	{
		pKernel->LuaErrorHandler(state, "nx_custom_byken argument number less than 2");
		return 1;
	}

	// 检查参数类型
	CHECK_ARG_OBJECT(state, nx_custom_byken, 1);

	PERSISTID self = pKernel->LuaToObject(state, 1);

	CVarList msg;
	if (! ILuaExtModule::GetVarList(state, 2, arg_num, msg))
	{
		pKernel->LuaErrorHandler(state, "nx_custom_byname args error");
		return 1;
	}

	pKernel->LuaPushBool(state, pKernel->CustomByKen(self, msg));
	return 1;
}

#ifndef FSROOMLOGIC_EXPORTS
// 原型：bool nx_custom_byscene(int sceneid, ...)
// 功能：向指定场景的客户端发送自定义消息
int nx_custom_byscene(void * state)
{
	IKernel * pKernel = ILuaExtModule::GetKernel(state);

	int arg_num = pKernel->LuaGetArgCount(state);
	if (arg_num < 2)
	{
		pKernel->LuaErrorHandler(state, "nx_custom_byscene argument number less than 2");
		return 1;
	}

	// 检查参数类型
	CHECK_ARG_INT(state, nx_custom_byscene, 1);

	int sceneid = pKernel->LuaToInt(state, 1);

	CVarList msg;
	if (!ILuaExtModule::GetVarList(state, 2, arg_num, msg))
	{
		pKernel->LuaErrorHandler(state, "nx_custom_byscene args error");
		return 1;
	}

	pKernel->LuaPushBool(state, pKernel->CustomByScene(sceneid, msg));
	return 1;
}
#endif // FSROOMLOGIC_EXPORTS

// 原型：bool nx_custom_bygroup(int groupid,  ...)
// 功能：向指定分组ID的客户端发送自定义消息
int nx_custom_bygroup(void * state)
{
	IKernel * pKernel = ILuaExtModule::GetKernel(state);

	int arg_num = pKernel->LuaGetArgCount(state);
	if (arg_num < 2)
	{
		pKernel->LuaErrorHandler(state, "nx_custom_bygroup argument number less than 2");
		return 1;
	}

	// 检查参数类型
	CHECK_ARG_INT(state, nx_custom_byscene, 1);

	int groupid = pKernel->LuaToInt(state, 1);

	CVarList msg;
	if (!ILuaExtModule::GetVarList(state, 2, arg_num, msg))
	{
		pKernel->LuaErrorHandler(state, "nx_custom_bygroup args error");
		return 1;
	}

	pKernel->LuaPushBool(state, pKernel->CustomByGroup(groupid, msg));
	return 1;
}

#ifndef FSROOMLOGIC_EXPORTS
// 原型：bool nx_custom_byworld(...)
// 功能：通过玩家名称向客户端发送自定义消息
int nx_custom_byworld(void * state)
{
	IKernel * pKernel = ILuaExtModule::GetKernel(state);

	int arg_num = pKernel->LuaGetArgCount(state);
	if (arg_num < 1)
	{
		pKernel->LuaErrorHandler(state, "nx_custom_byworld argument number less than 1");
		return 1;
	}

	CVarList msg;
	if (!ILuaExtModule::GetVarList(state, 1, arg_num, msg))
	{
		pKernel->LuaErrorHandler(state, "nx_custom_byworld args error");
		return 1;
	}

	pKernel->LuaPushBool(state, pKernel->CustomByWorld(msg));
	return 1;
}

// 原型：bool nx_custom_bychannel(int channel, ...)
// 功能：通过玩家名称向客户端发送自定义消息
int nx_custom_bychannel(void * state)
{
	IKernel * pKernel = ILuaExtModule::GetKernel(state);

	int arg_num = pKernel->LuaGetArgCount(state);
	if (arg_num < 2)
	{
		pKernel->LuaErrorHandler(state, "nx_custom_bychannel argument number less than 2");
		return 1;
	}

	// 检查参数类型
	CHECK_ARG_INT(state, nx_custom_bychannel, 1);

	int channel = pKernel->LuaToInt(state, 1);

	CVarList msg;
	if (!ILuaExtModule::GetVarList(state, 2, arg_num, msg))
	{
		pKernel->LuaErrorHandler(state, "nx_custom_bychannel args error");
		return 1;
	}

	pKernel->LuaPushBool(state, pKernel->CustomByChannel(channel, msg));
	return 1;
}
#endif // FSROOMLOGIC_EXPORTS

// 原型：int nx_command(object obj, object target, ...)
// 功能：由对象obj向对对象target发送命令
int nx_command(void * state)
{
	IKernel * pKernel = ILuaExtModule::GetKernel(state);

	int arg_num = pKernel->LuaGetArgCount(state);

	if (arg_num < 2)
	{
		pKernel->LuaErrorHandler(state, "nx_command argument number less than 3");
		return 1;
	}

	CHECK_ARG_OBJECT(state, nx_command, 1);
	CHECK_ARG_OBJECT(state, nx_command, 2);

	PERSISTID obj = pKernel->LuaToObject(state, 1);
	PERSISTID target = pKernel->LuaToObject(state, 2);

	CVarList msg;

	if (! ILuaExtModule::GetVarList(state, 3, arg_num, msg))
	{
		pKernel->LuaErrorHandler(state, "nx_command args error");
		return 1;
	}

	pKernel->LuaPushInt(state, pKernel->Command(obj, target, msg));

	return 1;
}

// 原型：int nx_command_byname(wstring name, ...)
// 功能：向名为name的玩家发送命令
int nx_command_byname(void * state)
{
	IKernel * pKernel = ILuaExtModule::GetKernel(state);

	int arg_num = pKernel->LuaGetArgCount(state);

	if (arg_num < 1)
	{
		pKernel->LuaErrorHandler(state, 
			"nx_command_byname argument less then 2");
		return 1;
	}

	CHECK_ARG_WIDESTR(state, nx_command_byname, 1);

	const wchar_t * name = pKernel->LuaToWideStr(state, 1);

	CVarList msg;

	if (! ILuaExtModule::GetVarList(state, 2, arg_num, msg))
	{
		pKernel->LuaErrorHandler(state, "nx_command_byname args error");
		return 1;
	}

	pKernel->LuaPushInt(state,
		pKernel->CommandByName(name, msg));

	return 1;
}

// 原型：int nx_command_byscene(int scene_id, ...)
// 功能：向场景号为scene_id的场景的所有玩家发送命令
int nx_command_byscene(void * state)
{
	IKernel * pKernel = ILuaExtModule::GetKernel(state);

	int arg_num = pKernel->LuaGetArgCount(state);

	if (arg_num < 1)
	{
		pKernel->LuaErrorHandler(state, 
			"nx_command_byscene argument less then 2");
		return 1;
	}

	CHECK_ARG_INT(state, nx_command_byscene, 1);

	int scene = pKernel->LuaToInt(state, 1);

	CVarList msg;

	if (! ILuaExtModule::GetVarList(state, 2, arg_num, msg))
	{
		pKernel->LuaErrorHandler(state, "nx_command_byscene args error");
		return 1;
	}

	pKernel->LuaPushInt(state,
		pKernel->CommandByScene(scene, msg));

	return 1;
}

// 原型：int nx_command_bygroup(int groupid, ...)
// 功能：向组号为groupid的所有玩家发送命令
int nx_command_bygroup(void * state)
{
	IKernel * pKernel = ILuaExtModule::GetKernel(state);

	int arg_num = pKernel->LuaGetArgCount(state);

	if (arg_num < 1)
	{
		pKernel->LuaErrorHandler(state, 
			"nx_command_bygroup argument less then 2");
		return 1;
	}

	CHECK_ARG_INT(state, nx_command_bygroup, 1);

	int group = pKernel->LuaToInt(state, 1);

	CVarList msg;

	if (! ILuaExtModule::GetVarList(state, 2, arg_num, msg))
	{
		pKernel->LuaErrorHandler(state, "nx_command_bygroup args error");
		return 1;
	}

	pKernel->LuaPushInt(state,
		pKernel->CommandByGroup(group, msg));

	return 1;
}

#ifndef FSROOMLOGIC_EXPORTS
// 原型：int nx_command_byworld(...)
// 功能：向游戏世界内的所有玩家发送命令
int nx_command_byworld(void * state)
{
	IKernel * pKernel = ILuaExtModule::GetKernel(state);
	int arg_num = pKernel->LuaGetArgCount(state);
	if (arg_num < 1)
	{
		pKernel->LuaErrorHandler(state,"nx_command_byworld argument 0");
		return 1;
	}

	CVarList msg;
	if (!ILuaExtModule::GetVarList(state, 1, arg_num, msg))
	{
		pKernel->LuaErrorHandler(state, "nx_command_byworld args error");
		return 1;
	}

	pKernel->LuaPushInt(state,pKernel->CommandByWorld(msg));
	return 1;
}
#endif // FSROOMLOGIC_EXPORTS

// 原型：int nx_command_toscene(int scene_id, ...)
// 功能：向场景号为scene_id的场景发送命令
int nx_command_toscene(void * state)
{
	IKernel * pKernel = ILuaExtModule::GetKernel(state);

	int arg_num = pKernel->LuaGetArgCount(state);

	if (arg_num < 1)
	{
		pKernel->LuaErrorHandler(state, 
			"nx_command_toscene argument less then 2");
		return 1;
	}

	CHECK_ARG_INT(state, nx_command_toscene, 1);

	int scene = pKernel->LuaToInt(state, 1);

	CVarList msg;

	if (! ILuaExtModule::GetVarList(state, 2, arg_num, msg))
	{
		pKernel->LuaErrorHandler(state, "nx_command_toscene args error");
		return 1;
	}

	pKernel->LuaPushInt(state,
		pKernel->CommandToScene(scene, msg));

	return 1;
}

#ifndef FSROOMLOGIC_EXPORTS
// 原型：int nx_command_toallscene(...)
// 功能：向所有场景发送命令
int nx_command_toallscene(void * state)
{
	IKernel * pKernel = ILuaExtModule::GetKernel(state);
	int arg_num = pKernel->LuaGetArgCount(state);
	if (arg_num < 1)
	{
		pKernel->LuaErrorHandler(state,"nx_command_toallscene argument 0");
		return 1;
	}

	CVarList msg;
	if (!ILuaExtModule::GetVarList(state, 1, arg_num, msg))
	{
		pKernel->LuaErrorHandler(state, "nx_command_toallscene args error");
		return 1;
	}
	pKernel->LuaPushInt(state,pKernel->CommandToAllScene(msg));
	return 1;
}
#endif // FSROOMLOGIC_EXPORTS

// 原型：int nx_command_toscenegroup(int scene_id, int group, ...)
// 功能：向场景号为scene_id的场景发送命令
int nx_command_toscenegroup(void * state)
{
	IKernel * pKernel = ILuaExtModule::GetKernel(state);

	int arg_num = pKernel->LuaGetArgCount(state);

	if (arg_num < 2)
	{
		pKernel->LuaErrorHandler(state, 
			"nx_command_toscene argument less then 2");
		return 1;
	}

	CHECK_ARG_INT(state, nx_command_toscenegroup, 1);
	CHECK_ARG_INT(state, nx_command_toscenegroup, 2);

	int scene = pKernel->LuaToInt(state, 1);
	int group = pKernel->LuaToInt(state, 2);

	CVarList msg;

	if (! ILuaExtModule::GetVarList(state, 3, arg_num, msg))
	{
		pKernel->LuaErrorHandler(state, "nx_command_toscene args error");
		return 1;
	}

	pKernel->LuaPushInt(state,
		pKernel->CommandBySceneGroup(scene, group, msg));

	return 1;
}

// 原型：bool nx_find_property(object obj, fast_string propname)
// 功能：查询对象是否存在指定的属性名
int nx_find_property(void * state)
{
	IKernel * pKernel = ILuaExtModule::GetKernel(state);

	// 检查参数数量
	CHECK_ARG_NUM(state, nx_find_property, 2);
	// 检查参数类型
	CHECK_ARG_OBJECT(state, nx_find_property, 1);
	CHECK_ARG_STRING(state, nx_find_property, 2);

	// 获取参数
	PERSISTID obj = pKernel->LuaToObject(state, 1);
	const char * propname = pKernel->LuaToString(state, 2);

	IGameObj* pObj = pKernel->GetGameObj(obj);
	if (pObj != NULL)
	{
		pKernel->LuaPushBool(state, pObj->FindAttr(propname));
	}
	
	return 1;
}

//原型：table nx_get_property_list(object obj)
//功能：获得对象的所有属性名称
int nx_get_property_list(void * state)
{
	IKernel * pKernel = ILuaExtModule::GetKernel(state);
	CHECK_ARG_NUM(state, nx_get_property_list, 1);
	CHECK_ARG_OBJECT(state, nx_get_property_list, 1);
	PERSISTID obj = pKernel->LuaToObject( state, 1 );

	CVarList result;
	IGameObj* pObj = pKernel->GetGameObj(obj);
	if (pObj != NULL)
	{
		pObj->GetAttrList(result);
	}

	pKernel->LuaPushTable( state, result);
	return 1;
}

// 原型：var nx_get_property_type(object obj, fast_string propname)
// 功能：查询对象属性的类型
int nx_get_property_type(void * state)
{
	IKernel * pKernel = ILuaExtModule::GetKernel(state);

	// 检查参数数量
	CHECK_ARG_NUM(state, nx_get_property_type, 2);
	// 检查参数类型
	CHECK_ARG_OBJECT(state, nx_get_property_type, 1);
	CHECK_ARG_STRING(state, nx_get_property_type, 2);

	// 获取参数
	PERSISTID obj = pKernel->LuaToObject(state, 1);
	const char * propname = pKernel->LuaToString(state, 2);

	IGameObj* pObj = pKernel->GetGameObj(obj);
	if (pObj != NULL)
	{
		pKernel->LuaPushInt(state, pObj->GetAttrType(propname));
	}

	return 1;
}

// 原型：var nx_query_property(object obj, fast_string propname)
// 功能：查询对象属性的值
int nx_query_property(void * state)
{
	IKernel * pKernel = ILuaExtModule::GetKernel(state);

	// 检查参数数量
	CHECK_ARG_NUM(state, nx_query_property, 2);
	// 检查参数类型
	CHECK_ARG_OBJECT(state, nx_query_property, 1);
	CHECK_ARG_STRING(state, nx_query_property, 2);

	// 获取参数
	PERSISTID obj = pKernel->LuaToObject(state, 1);

	IGameObj* pObj = pKernel->GetGameObj(obj);
	if (pObj == NULL)
	{
		return 1;
	}

	const char * propname = pKernel->LuaToString(state, 2);

	switch (pObj->GetAttrType(propname))
	{
	case VTYPE_INT:
		pKernel->LuaPushInt(state, pObj->QueryInt(propname));
		break;
	case VTYPE_INT64:
		pKernel->LuaPushInt64(state, pObj->QueryInt64(propname));
		break;
	case VTYPE_FLOAT:
		pKernel->LuaPushFloat(state, pObj->QueryFloat(propname));
		break;
	case VTYPE_DOUBLE:
		pKernel->LuaPushDouble(state, pObj->QueryDouble(propname));
		break;
	case VTYPE_STRING:
		pKernel->LuaPushString(state, pObj->QueryString(propname));
		break;
	case VTYPE_WIDESTR:
		pKernel->LuaPushWideStr(state, pObj->QueryWideStr(propname));
		break;
	case VTYPE_OBJECT:
		pKernel->LuaPushObject(state, pObj->QueryObject(propname));
		break;
	}

	return 1;
}

// 原型：bool nx_set_property(object obj, fast_string propname, value)
// 功能：设置对象属性的值
int nx_set_property(void * state)
{
	IKernel * pKernel = ILuaExtModule::GetKernel(state);

	// 检查参数数量
	CHECK_ARG_NUM(state, nx_set_property, 3);
	// 检查参数类型
	CHECK_ARG_OBJECT(state, nx_set_property, 1);
	CHECK_ARG_STRING(state, nx_set_property, 2);

	// 获取参数
	PERSISTID obj = pKernel->LuaToObject(state, 1);
	IGameObj* pObj = pKernel->GetGameObj(obj);
	if (pObj == NULL)
	{
		return 1;
	}

	const char * propname = pKernel->LuaToString(state, 2);

	// 金钱不允许修改
	if (std::string(propname).find("CapitalType") != std::string::npos)
	{
		pKernel->LuaPushBool(state, false);

		return 1;
	}

	if (pKernel->LuaIsInt(state, 3))
	{
		pKernel->LuaPushBool(state, pObj->SetInt(propname, pKernel->LuaToInt(state, 3)));
	}
	else if (pKernel->LuaIsInt64(state, 3))
	{
		pKernel->LuaPushBool(state, pObj->SetInt64(propname, pKernel->LuaToInt64(state, 3)));
	}
	else if (pKernel->LuaIsFloat(state, 3))
	{
		pKernel->LuaPushBool(state, pObj->SetFloat(propname, pKernel->LuaToFloat(state, 3)));
	}
	else if (pKernel->LuaIsDouble(state, 3))
	{
		pKernel->LuaPushBool(state, pObj->SetDouble(propname, pKernel->LuaToDouble(state, 3)));
	}
	else if (pKernel->LuaIsString(state, 3))
	{
		pKernel->LuaPushBool(state, pObj->SetString(propname, pKernel->LuaToString(state, 3)));
	}
	else if (pKernel->LuaIsWideStr(state, 3))
	{
		pKernel->LuaPushBool(state, pObj->SetWideStr(propname, pKernel->LuaToWideStr(state, 3)));
	}
	else if (pKernel->LuaIsObject(state, 3))
	{
		pKernel->LuaPushBool(state, pObj->SetObject(propname, pKernel->LuaToObject(state, 3)));
	}

	return 1;
}

// 原型：bool nx_inc_property(object obj, fast_string propname, value)
// 功能：增加对象属性的值
int nx_inc_property(void * state)
{
	IKernel * pKernel = ILuaExtModule::GetKernel(state);

	// 检查参数数量
	CHECK_ARG_NUM(state, nx_inc_property, 3);
	// 检查参数类型
	CHECK_ARG_OBJECT(state, nx_inc_property, 1);
	CHECK_ARG_STRING(state, nx_inc_property, 2);

	// 获取参数
	PERSISTID obj = pKernel->LuaToObject(state, 1);
	IGameObj* pObj = pKernel->GetGameObj(obj);
	if (pObj == NULL)
	{
		return 1;
	}

	const char * propname = pKernel->LuaToString(state, 2);

	if (pKernel->LuaIsInt(state, 3))
	{
		int ivalue = pObj->QueryInt(propname);
		pKernel->LuaPushBool(state, pObj->SetInt(propname, ivalue + pKernel->LuaToInt(state, 3)));
	}
	else if (pKernel->LuaIsFloat(state, 3))
	{
		float fvalue = pObj->QueryFloat(propname);
		pKernel->LuaPushBool(state, pObj->SetFloat(propname, fvalue + pKernel->LuaToFloat(state, 3)));
	}
	else 
	{
		pKernel->LuaPushBool(state, false);
	}

	return 1;
}

// 原型：bool nx_set_property_flag(object obj, fast_string propname, int 标志位)
// 功能：用属性实现位标志操作（目的是节约存储，属性类型必须为整数）
int nx_set_property_flag(void * state)
{
	IKernel * pKernel = ILuaExtModule::GetKernel(state);

	// 检查参数数量
	CHECK_ARG_NUM(state, nx_set_property_flag, 3);
	// 检查参数类型
	CHECK_ARG_OBJECT(state, nx_set_property_flag, 1);
	CHECK_ARG_STRING(state, nx_set_property_flag, 2);
	CHECK_ARG_INT(state, nx_set_property_flag, 3);

	// 获取参数
	PERSISTID obj = pKernel->LuaToObject(state, 1);
	IGameObj* pObj = pKernel->GetGameObj(obj);
	if (pObj == NULL)
	{
		return 1;
	}

	const char * prop = pKernel->LuaToString(state, 2);
	int pos = pKernel->LuaToInt(state, 3);

	pKernel->LuaPushBool(state, pObj->SetAttrFlag(prop, pos));
	return 1;
}

// 原型：bool nx_clear_property_flag(object obj, fast_string propname, int 标志位)
// 功能：清除标志(0-31)
int nx_clear_property_flag(void * state)
{
	IKernel * pKernel = ILuaExtModule::GetKernel(state);

	// 检查参数数量
	CHECK_ARG_NUM(state, nx_clear_property_flag, 3);
	// 检查参数类型
	CHECK_ARG_OBJECT(state, nx_clear_property_flag, 1);
	CHECK_ARG_STRING(state, nx_clear_property_flag, 2);
	CHECK_ARG_INT(state, nx_clear_property_flag, 3);

	// 获取参数
	PERSISTID obj = pKernel->LuaToObject(state, 1);
	IGameObj* pObj = pKernel->GetGameObj(obj);
	if (pObj == NULL)
	{
		return 1;
	}

	const char * prop = pKernel->LuaToString(state, 2);
	int pos = pKernel->LuaToInt(state, 3);

	pKernel->LuaPushBool(state, pObj->ClearAttrFlag(prop, pos));
	return 1;
}

// 原型：bool nx_test_property_flag(object obj, fast_string propname, int 标志位)
// 功能：测试标志(0-31)
int nx_test_property_flag(void * state)
{
	IKernel * pKernel = ILuaExtModule::GetKernel(state);

	// 检查参数数量
	CHECK_ARG_NUM(state, nx_test_property_flag, 3);
	// 检查参数类型
	CHECK_ARG_OBJECT(state, nx_test_property_flag, 1);
	CHECK_ARG_STRING(state, nx_test_property_flag, 2);
	CHECK_ARG_INT(state, nx_test_property_flag, 3);

	// 获取参数
	PERSISTID obj = pKernel->LuaToObject(state, 1);
	IGameObj* pObj = pKernel->GetGameObj(obj);
	if (pObj == NULL)
	{
		return 1;
	}

	const char * prop = pKernel->LuaToString(state, 2);
	int pos = pKernel->LuaToInt(state, 3);

	pKernel->LuaPushBool(state, pObj->TestAttrFlag(prop, pos));
	return 1;
}

// 原型：bool nx_find_data(object obj, fast_string propname)
// 功能：查询对象是否存在指定的属性名
int nx_find_data(void * state)
{
	IKernel * pKernel = ILuaExtModule::GetKernel(state);

	// 检查参数数量
	CHECK_ARG_NUM(state, nx_find_data, 2);
	// 检查参数类型
	CHECK_ARG_OBJECT(state, nx_find_data, 1);
	CHECK_ARG_STRING(state, nx_find_data, 2);

	// 获取参数
	PERSISTID obj = pKernel->LuaToObject(state, 1);
	IGameObj* pObj = pKernel->GetGameObj(obj);
	if (pObj == NULL)
	{
		return 1;
	}

	const char * propname = pKernel->LuaToString(state, 2);

	pKernel->LuaPushBool(state, pObj->FindData(propname));
	return 1;
}


// 原型：var nx_get_data_type(object obj, fast_string propname)
// 功能：查询对象临时数据的类型
int nx_get_data_type(void * state)
{
	IKernel * pKernel = ILuaExtModule::GetKernel(state);

	// 检查参数数量
	CHECK_ARG_NUM(state, nx_get_data_type, 2);
	// 检查参数类型
	CHECK_ARG_OBJECT(state, nx_get_data_type, 1);
	CHECK_ARG_STRING(state, nx_get_data_type, 2);

	// 获取参数
	PERSISTID obj = pKernel->LuaToObject(state, 1);
	IGameObj* pObj = pKernel->GetGameObj(obj);
	if (pObj == NULL)
	{
		return 1;
	}

	const char * propname = pKernel->LuaToString(state, 2);

	pKernel->LuaPushInt(state, pObj->GetDataType(propname));
	return 1;
}

// 原型：bool nx_add_data(object obj, fast_string propname, int type)
// 功能：添加临时属性
int nx_add_data(void * state)
{
	IKernel * pKernel = ILuaExtModule::GetKernel(state);

	// 检查参数数量
	CHECK_ARG_NUM(state, nx_add_data, 3);
	// 检查参数类型
	CHECK_ARG_OBJECT(state, nx_add_data, 1);
	CHECK_ARG_STRING(state, nx_add_data, 2);
	CHECK_ARG_INT(state, nx_add_data, 3);

	// 获取参数
	PERSISTID obj = pKernel->LuaToObject(state, 1);
	IGameObj* pObj = pKernel->GetGameObj(obj);
	if (pObj == NULL)
	{
		return 1;
	}

	const char * propname = pKernel->LuaToString(state, 2);
	int type = pKernel->LuaToInt(state, 3);
	switch (type)
	{
	case VTYPE_INT:
		pKernel->LuaPushBool(state, pObj->AddDataInt(propname, 0));
		break;
	case VTYPE_INT64:
		pKernel->LuaPushBool(state, pObj->AddDataInt64(propname, 0));
		break;
	case VTYPE_FLOAT:
		pKernel->LuaPushBool(state, pObj->AddDataFloat(propname, 0.0f));
		break;
	case VTYPE_DOUBLE:
		pKernel->LuaPushBool(state, pObj->AddDataDouble(propname, 0.0f));
		break;
	case VTYPE_STRING:
		pKernel->LuaPushBool(state, pObj->AddDataString(propname, ""));
		break;
	case VTYPE_WIDESTR:
		pKernel->LuaPushBool(state, pObj->AddDataWideStr(propname, L""));
		break;
	case VTYPE_OBJECT:
		pKernel->LuaPushBool(state, pObj->AddDataObject(propname, PERSISTID()));
		break;
	default:
		break;
	}

	return 1;
}


// 原型：bool nx_add_data(object obj, fast_string propname)
int nx_remove_data(void* state)
{
	IKernel * pKernel = ILuaExtModule::GetKernel(state);

	// 检查参数数量
	CHECK_ARG_NUM(state, nx_remove_data, 2);
	// 检查参数类型
	CHECK_ARG_OBJECT(state, nx_remove_data, 1);
	CHECK_ARG_STRING(state, nx_remove_data, 2);

	PERSISTID obj = pKernel->LuaToObject( state, 1 );
	IGameObj* pObj = pKernel->GetGameObj(obj);
	if (pObj == NULL)
	{
		return 1;
	}

	const char* name = pKernel->LuaToString( state, 2 );
	pKernel->LuaPushBool( state, pObj->RemoveData( name ) );
	return 1;
}

// 原型：var nx_query_data(object obj, fast_string propname)
// 功能：查询对象临时数据的值
int nx_query_data(void * state)
{
	IKernel * pKernel = ILuaExtModule::GetKernel(state);

	// 检查参数数量
	CHECK_ARG_NUM(state, nx_query_data, 2);
	// 检查参数类型
	CHECK_ARG_OBJECT(state, nx_query_data, 1);
	CHECK_ARG_STRING(state, nx_query_data, 2);

	// 获取参数
	PERSISTID obj = pKernel->LuaToObject(state, 1);
	IGameObj* pObj = pKernel->GetGameObj(obj);
	if (pObj == NULL)
	{
		return 1;
	}

	const char * propname = pKernel->LuaToString(state, 2);

	switch (pObj->GetDataType(propname))
	{
	case VTYPE_INT:
		pKernel->LuaPushInt(state, pObj->QueryDataInt(propname));
		break;
	case VTYPE_INT64:
		pKernel->LuaPushInt64(state, pObj->QueryDataInt64(propname));
		break;
	case VTYPE_FLOAT:
		pKernel->LuaPushFloat(state, pObj->QueryDataFloat(propname));
		break;
	case VTYPE_DOUBLE:
		pKernel->LuaPushDouble(state, pObj->QueryDataDouble(propname));
		break;
	case VTYPE_STRING:
		pKernel->LuaPushString(state, pObj->QueryDataString(propname));
		break;
	case VTYPE_WIDESTR:
		pKernel->LuaPushWideStr(state, pObj->QueryDataWideStr(propname));
		break;
	case VTYPE_OBJECT:
		pKernel->LuaPushObject(state, pObj->QueryDataObject(propname));
		break;
	}

	return 1;
}

// 原型：bool nx_set_data(object obj, fast_string propname, value)
// 功能：设置对象临时数据的值
int nx_set_data(void * state)
{
	IKernel * pKernel = ILuaExtModule::GetKernel(state);

	// 检查参数数量
	CHECK_ARG_NUM(state, nx_set_data, 3);
	// 检查参数类型
	CHECK_ARG_OBJECT(state, nx_set_data, 1);
	CHECK_ARG_STRING(state, nx_set_data, 2);

	// 获取参数
	PERSISTID obj = pKernel->LuaToObject(state, 1);
	IGameObj* pObj = pKernel->GetGameObj(obj);
	if (pObj == NULL)
	{
		return 1;
	}

	const char * propname = pKernel->LuaToString(state, 2);

	switch (pObj->GetDataType(propname))
	{
	case VTYPE_INT:
		pKernel->LuaPushBool(state, pObj->SetDataInt(propname, pKernel->LuaToInt(state, 3)));
		break;
	case VTYPE_INT64:
		pKernel->LuaPushBool(state, pObj->SetDataInt64(propname, pKernel->LuaToInt64(state, 3)));
		break;
	case VTYPE_FLOAT:
		pKernel->LuaPushBool(state, pObj->SetDataFloat(propname, pKernel->LuaToFloat(state, 3)));
		break;
	case VTYPE_DOUBLE:
		pKernel->LuaPushBool(state, pObj->SetDataDouble(propname, pKernel->LuaToDouble(state, 3)));
		break;
	case VTYPE_STRING:
		pKernel->LuaPushBool(state, pObj->SetDataString(propname, pKernel->LuaToString(state, 3)));
		break;
	case VTYPE_WIDESTR:
		pKernel->LuaPushBool(state, pObj->SetDataWideStr(propname, pKernel->LuaToWideStr(state, 3)));
		break;
	case VTYPE_OBJECT:
		pKernel->LuaPushBool(state, pObj->SetDataObject(propname, pKernel->LuaToObject(state, 3)));
		break;
	}

	return 1;
}

// 原型：bool nx_find_record(object obj, fast_string recname)
// 功能：在查询对象身上是否存在指定表格
int nx_find_record(void * state)
{
	IKernel * pKernel = ILuaExtModule::GetKernel(state);

	// 检查参数数量
	CHECK_ARG_NUM(state, nx_find_record, 2);
	// 检查参数类型
	CHECK_ARG_OBJECT(state, nx_find_record, 1);
	CHECK_ARG_STRING(state, nx_find_record, 2);

	// 获取参数
	PERSISTID obj = pKernel->LuaToObject(state, 1);
	IGameObj* pObj = pKernel->GetGameObj(obj);
	if (pObj == NULL)
	{
		return 1;
	}

	const char * recname = pKernel->LuaToString(state, 2);

	pKernel->LuaPushBool(state, pObj->GetRecord(recname) != NULL);
	return 1;
}

//原型：table nx_get_record_list(object obj)
//功能：获得对象的所有表格名称
int nx_get_record_list(void * state)
{
	IKernel * pKernel = ILuaExtModule::GetKernel(state);
	CHECK_ARG_NUM(state, nx_get_record_list, 1);
	CHECK_ARG_OBJECT(state, nx_get_record_list, 1);
	PERSISTID obj = pKernel->LuaToObject( state, 1 );
	IGameObj* pObj = pKernel->GetGameObj(obj);
	if (pObj == NULL)
	{
		return 1;
	}

	CVarList result;
	pObj->GetRecordList(result);

	pKernel->LuaPushTable( state, result);
	return 1;
}

// 原型：int nx_get_record_rows(object obj, fast_string recname)
// 功能：获得指定表格的行数
int nx_get_record_rows(void * state)
{
	IKernel * pKernel = ILuaExtModule::GetKernel(state);

	// 检查参数数量
	CHECK_ARG_NUM(state, nx_get_record_rows, 2);
	// 检查参数类型
	CHECK_ARG_OBJECT(state, nx_get_record_rows, 1);
	CHECK_ARG_STRING(state, nx_get_record_rows, 2);

	// 获取参数
	PERSISTID obj = pKernel->LuaToObject(state, 1);
	IGameObj* pObj = pKernel->GetGameObj(obj);
	if (pObj == NULL)
	{
		return 1;
	}

	const char * recname = pKernel->LuaToString(state, 2);
	IRecord* pRecord = pObj->GetRecord(recname);
	if (pRecord == NULL)
	{
		return 1;
	}

	pKernel->LuaPushInt(state, pRecord->GetRows());
	return 1;
}

// 原型：int nx_get_record_max_rows(object obj, fast_string recname)
// 功能：获得指定表格的行数
int nx_get_record_max_rows(void * state)
{
	IKernel * pKernel = ILuaExtModule::GetKernel(state);

	// 检查参数数量
	CHECK_ARG_NUM(state, nx_get_record_max_rows, 2);
	// 检查参数类型
	CHECK_ARG_OBJECT(state, nx_get_record_max_rows, 1);
	CHECK_ARG_STRING(state, nx_get_record_max_rows, 2);

	// 获取参数
	PERSISTID obj = pKernel->LuaToObject(state, 1);
	IGameObj* pObj = pKernel->GetGameObj(obj);
	if (pObj == NULL)
	{
		return 1;
	}

	const char * recname = pKernel->LuaToString(state, 2);
	IRecord* pRecord = pObj->GetRecord(recname);
	if (pRecord == NULL)
	{
		return 1;
	}

	pKernel->LuaPushInt(state, pRecord->GetRowMax());
	return 1;
}


// 原型：int nx_get_record_cols(object obj, fast_string recname)
// 功能：获得指定表格的列数
int nx_get_record_cols(void * state)
{
	IKernel * pKernel = ILuaExtModule::GetKernel(state);

	// 检查参数数量
	CHECK_ARG_NUM(state, nx_get_record_cols, 2);
	// 检查参数类型
	CHECK_ARG_OBJECT(state, nx_get_record_cols, 1);
	CHECK_ARG_STRING(state, nx_get_record_cols, 2);

	// 获取参数
	PERSISTID obj = pKernel->LuaToObject(state, 1);
	IGameObj* pObj = pKernel->GetGameObj(obj);
	if (pObj == NULL)
	{
		return 1;
	}

	const char * recname = pKernel->LuaToString(state, 2);
	IRecord* pRecord = pObj->GetRecord(recname);
	if (pRecord == NULL)
	{
		return 1;
	}

	pKernel->LuaPushInt(state, pRecord->GetCols());
	return 1;
}

// 原型：int nx_get_record_coltype(object obj, fast_string recname, int col)
// 功能：获得指定表格的指定列的数据类型
int nx_get_record_coltype(void * state)
{
	IKernel * pKernel = ILuaExtModule::GetKernel(state);

	// 检查参数数量
	CHECK_ARG_NUM(state, nx_get_record_coltype, 3);
	// 检查参数类型
	CHECK_ARG_OBJECT(state, nx_get_record_coltype, 1);
	CHECK_ARG_STRING(state, nx_get_record_coltype, 2);
	CHECK_ARG_INT(state, nx_get_record_coltype, 3);

	// 获取参数
	PERSISTID obj = pKernel->LuaToObject(state, 1);
	IGameObj* pObj = pKernel->GetGameObj(obj);
	if (pObj == NULL)
	{
		return 1;
	}

	const char * recname = pKernel->LuaToString(state, 2);
	IRecord* pRecord = pObj->GetRecord(recname);
	if (pRecord == NULL)
	{
		return 1;
	}

	int col = pKernel->LuaToInt(state, 3);

	pKernel->LuaPushInt(state, pRecord->GetColType(col));
	return 1;
}

// 原型：int nx_add_record_row(object obj, fast_string recname)
// 功能：在表格的最后插入一行，返回行号
int nx_add_record_row(void * state)
{
	IKernel * pKernel = ILuaExtModule::GetKernel(state);

	// 检查参数数量
	CHECK_ARG_NUM(state, nx_add_record_row, 2);
	// 检查参数类型
	CHECK_ARG_OBJECT(state, nx_add_record_row, 1);
	CHECK_ARG_STRING(state, nx_add_record_row, 2);

	// 获取参数
	PERSISTID obj = pKernel->LuaToObject(state, 1);
	IGameObj* pObj = pKernel->GetGameObj(obj);
	if (pObj == NULL)
	{
		return 1;
	}

	const char * recname = pKernel->LuaToString(state, 2);
	IRecord* pRecord = pObj->GetRecord(recname);
	if (pRecord == NULL)
	{
		return 1;
	}

	pKernel->LuaPushInt(state, pRecord->AddRow(-1));
	return 1;
}

// 原型：int nx_add_record_row_values(object obj, fast_string recname, ...)
// 功能：在表格的最后插入一行数据，返回行号
int nx_add_record_row_values(void * state)
{
	IKernel * pKernel = ILuaExtModule::GetKernel(state);

	int arg_num = pKernel->LuaGetArgCount(state);
	if (arg_num < 2)
	{
		pKernel->LuaErrorHandler(state, "nx_add_record_row_values argument number less than 2");
		return 1;
	}

	// 检查参数类型
	CHECK_ARG_OBJECT(state, nx_add_record_row_values, 1);
	CHECK_ARG_STRING(state, nx_add_record_row_values, 2);

	// 获取参数
	PERSISTID obj = pKernel->LuaToObject(state, 1);
	IGameObj* pObj = pKernel->GetGameObj(obj);
	if (pObj == NULL)
	{
		return 1;
	}

	const char * recname = pKernel->LuaToString(state, 2);
	IRecord* pRecord = pObj->GetRecord(recname);
	if (pRecord == NULL)
	{
		return 1;
	}

	if (arg_num < 2 + pRecord->GetCols())
	{
		pKernel->LuaErrorHandler(state, "nx_add_record_row_values argument number too less");
		return 1;
	}

	CVarList values;
	ILuaExtModule::GetVarList(state, 3, arg_num, values);

	pKernel->LuaPushInt(state, pRecord->AddRowValue(-1, values));

	return 1;
}

// 原型：int nx_find_record_value(object obj, fast_string recname, int col, value)
// 功能：在表格中的指定列寻找是否存在指定类型的值
int nx_find_record_value(void * state)
{
	IKernel * pKernel = ILuaExtModule::GetKernel(state);

	// 检查参数数量
	CHECK_ARG_NUM(state, nx_find_record_value, 4);
	// 检查参数类型
	CHECK_ARG_OBJECT(state, nx_find_record_value, 1);
	CHECK_ARG_STRING(state, nx_find_record_value, 2);
	CHECK_ARG_INT(state, nx_find_record_value, 3);

	// 获取参数
	PERSISTID obj = pKernel->LuaToObject(state, 1);
	IGameObj* pObj = pKernel->GetGameObj(obj);
	if (pObj == NULL)
	{
		return 1;
	}

	const char * recname = pKernel->LuaToString(state, 2);
	IRecord* pRecord = pObj->GetRecord(recname);
	if (pRecord == NULL)
	{
		return 1;
	}

	int col = pKernel->LuaToInt(state, 3);

	int row = -1;
	if (pKernel->LuaIsInt(state, 4))
	{
		int value = pKernel->LuaToInt(state, 4);
		row = pRecord->FindInt(col, value);
	}
	else if (pKernel->LuaIsInt64(state, 4))
	{
		__int64 value = pKernel->LuaToInt64(state, 4);
		row = pRecord->FindInt64(col, value);
	}
	else if (pKernel->LuaIsFloat(state, 4))
	{
		float value = pKernel->LuaToFloat(state, 4);
		row = pRecord->FindFloat(col, value);
	}
	else if (pKernel->LuaIsDouble(state, 4))
	{
		double value = pKernel->LuaToDouble(state, 4);
		row = pRecord->FindDouble(col, value);
	}
	else if (pKernel->LuaIsString(state, 4))
	{
		const char * value = pKernel->LuaToString(state, 4);
		row = pRecord->FindString(col, value);
	}
	else if (pKernel->LuaIsWideStr(state, 4))
	{
		const wchar_t* value = pKernel->LuaToWideStr(state, 4);
		row = pRecord->FindWideStr(col, value);
	}
	else if (pKernel->LuaIsObject(state, 4))
	{
		PERSISTID value = pKernel->LuaToObject(state, 4);
		row = pRecord->FindObject(col, value);
	}

	pKernel->LuaPushInt(state, row);
	return 1;
}

// 原型：var nx_query_record_value(object obj, fast_string recname, int row, int col)
// 功能：查询表格指定行，列的数据值
int nx_query_record_value(void * state)
{
	IKernel * pKernel = ILuaExtModule::GetKernel(state);

	// 检查参数数量
	CHECK_ARG_NUM(state, nx_query_record_value, 4);
	// 检查参数类型
	CHECK_ARG_OBJECT(state, nx_query_record_value, 1);
	CHECK_ARG_STRING(state, nx_query_record_value, 2);
	CHECK_ARG_INT(state, nx_query_record_value, 3);
	CHECK_ARG_INT(state, nx_query_record_value, 4);

	// 获取参数
	PERSISTID obj = pKernel->LuaToObject(state, 1);
	IGameObj* pObj = pKernel->GetGameObj(obj);
	if (pObj == NULL)
	{
		return 1;
	}

	const char * recname = pKernel->LuaToString(state, 2);
	IRecord* pRecord = pObj->GetRecord(recname);
	if (pRecord == NULL)
	{
		return 1;
	}

	int row = pKernel->LuaToInt(state, 3);
	int col = pKernel->LuaToInt(state, 4);

	switch (pRecord->GetColType(col))
	{
	case VTYPE_INT:
		pKernel->LuaPushInt(state, pRecord->QueryInt(row, col));
		break;
	case VTYPE_INT64:
		pKernel->LuaPushInt64(state, pRecord->QueryInt64(row, col));
		break;
	case VTYPE_FLOAT:
		pKernel->LuaPushFloat(state, pRecord->QueryFloat(row, col));
		break;
	case VTYPE_DOUBLE:
		pKernel->LuaPushDouble(state, pRecord->QueryDouble(row, col));
		break;
	case VTYPE_STRING:
		pKernel->LuaPushString(state, pRecord->QueryString(row, col));
		break;
	case VTYPE_WIDESTR:
		pKernel->LuaPushWideStr(state, pRecord->QueryWideStr(row, col));
		break;
	case VTYPE_OBJECT:
		pKernel->LuaPushObject(state, pRecord->QueryObject(row, col));
		break;
	}

	return 1;
}

// 原型：table nx_query_record_row_values(object obj, fast_string recname, int row)
// 功能：查询表格指定一行的数据
int nx_query_record_row_values(void * state)
{
	IKernel * pKernel = ILuaExtModule::GetKernel(state);

	// 检查参数数量
	CHECK_ARG_NUM(state, nx_query_record_row_values, 3);
	// 检查参数类型
	CHECK_ARG_OBJECT(state, nx_query_record_row_values, 1);
	CHECK_ARG_STRING(state, nx_query_record_row_values, 2);
	CHECK_ARG_INT(state, nx_query_record_row_values, 3);

	// 获取参数
	PERSISTID obj = pKernel->LuaToObject(state, 1);
	IGameObj* pObj = pKernel->GetGameObj(obj);
	if (pObj == NULL)
	{
		return 1;
	}

	const char * recname = pKernel->LuaToString(state, 2);
	IRecord* pRecord = pObj->GetRecord(recname);
	if (pRecord == NULL)
	{
		return 1;
	}

	int row = pKernel->LuaToInt(state, 3);

	CVarList value;
	pRecord->QueryRowValue(row, value);
	pKernel->LuaPushTable(state, value);

	return 1;
}

// 原型：bool nx_set_record_value(object obj, fast_string recname, int row, int col, value) 
// 功能：设置表格指定行列的值
int nx_set_record_value(void * state)
{
	IKernel * pKernel = ILuaExtModule::GetKernel(state);

	// 检查参数数量
	CHECK_ARG_NUM(state, nx_set_record_value, 5);
	// 检查参数类型
	CHECK_ARG_OBJECT(state, nx_set_record_value, 1);
	CHECK_ARG_STRING(state, nx_set_record_value, 2);
	CHECK_ARG_INT(state, nx_set_record_value, 3);
	CHECK_ARG_INT(state, nx_set_record_value, 4);

	// 获取参数
	PERSISTID obj = pKernel->LuaToObject(state, 1);
	IGameObj* pObj = pKernel->GetGameObj(obj);
	if (pObj == NULL)
	{
		return 1;
	}

	const char * recname = pKernel->LuaToString(state, 2);
	IRecord* pRecord = pObj->GetRecord(recname);
	if (pRecord == NULL)
	{
		return 1;
	}

	int row = pKernel->LuaToInt(state, 3);
	int col = pKernel->LuaToInt(state, 4);

	if (pKernel->LuaIsInt(state, 5))
	{
		int value = pKernel->LuaToInt(state, 5);
		pKernel->LuaPushBool(state, pRecord->SetInt(row, col, value));
	}
	else if (pKernel->LuaIsInt64(state, 5))
	{
		__int64 value = pKernel->LuaToInt64(state, 5);
		pKernel->LuaPushBool(state, pRecord->SetInt64(row, col, value));
	}
	else if (pKernel->LuaIsFloat(state, 5))
	{
		float value = pKernel->LuaToFloat(state, 5);
		pKernel->LuaPushBool(state, pRecord->SetFloat(row, col, value));
	}
	else if (pKernel->LuaIsDouble(state, 5))
	{
		double value = pKernel->LuaToDouble(state, 5);
		pKernel->LuaPushBool(state, pRecord->SetDouble(row, col, value));
	}
	else if (pKernel->LuaIsString(state, 5))
	{
		const char * value = pKernel->LuaToString(state, 5);
		pKernel->LuaPushBool(state, pRecord->SetString(row, col, value));
	}
	else if (pKernel->LuaIsWideStr(state, 5))
	{
		const wchar_t * value = pKernel->LuaToWideStr(state, 5);
		pKernel->LuaPushBool(state, pRecord->SetWideStr(row, col, value));
	}
	else if (pKernel->LuaIsObject(state, 5))
	{
		PERSISTID value = pKernel->LuaToObject(state, 5);
		pKernel->LuaPushBool(state, pRecord->SetObject(row, col, value));
	}

	return 1;
}

// 原型：bool nx_set_record_row_values(object obj, fast_string recname, int row, ...)
// 功能：设置表格一行的数据
int nx_set_record_row_values(void * state)
{
	IKernel * pKernel = ILuaExtModule::GetKernel(state);

	int arg_num = pKernel->LuaGetArgCount(state);
	if (arg_num < 3)
	{
		pKernel->LuaErrorHandler(state, "nx_set_record_row_values argument number less than 3");
		return 1;
	}

	// 检查参数类型
	CHECK_ARG_OBJECT(state, nx_set_record_row_values, 1);
	CHECK_ARG_STRING(state, nx_set_record_row_values, 2);
	CHECK_ARG_INT(state, nx_set_record_row_values, 3);

	// 获取参数
	PERSISTID obj = pKernel->LuaToObject(state, 1);
	IGameObj* pObj = pKernel->GetGameObj(obj);
	if (pObj == NULL)
	{
		return 1;
	}

	const char * recname = pKernel->LuaToString(state, 2);
	IRecord* pRecord = pObj->GetRecord(recname);
	if (pRecord == NULL)
	{
		return 1;
	}

	int row = pKernel->LuaToInt(state, 3);

	if (arg_num < 3 + pRecord->GetCols())
	{
		pKernel->LuaErrorHandler(state, "nx_set_record_row_values argument number too less");
		return 1;
	}

	CVarList values;
	ILuaExtModule::GetVarList(state, 4, arg_num, values);

	pKernel->LuaPushBool(state, pRecord->SetRowValue(row, values));
	return 1;
}

// 原型：bool nx_remove_record_row(object obj, fast_string recname, int row)
// 功能：删除在表格中的指定行
int nx_remove_record_row(void * state)
{
	IKernel * pKernel = ILuaExtModule::GetKernel(state);
	CHECK_ARG_NUM(state, nx_remove_record_row, 3);
	CHECK_ARG_OBJECT(state, nx_remove_record_row, 1);
	CHECK_ARG_STRING(state, nx_remove_record_row, 2);
	CHECK_ARG_INT(state, nx_remove_record_row, 3);

	PERSISTID obj = pKernel->LuaToObject( state, 1 );
	IGameObj* pObj = pKernel->GetGameObj(obj);
	if (pObj == NULL)
	{
		return 1;
	}

	const char* record_name = pKernel->LuaToString( state, 2 );
	IRecord* pRecord = pObj->GetRecord(record_name);
	if (pRecord == NULL)
	{
		return 1;
	}

	int row = pKernel->LuaToInt( state, 3 );
	pKernel->LuaPushBool( state, pRecord->RemoveRow(row) );
	return 1;
}

// 原型：bool nx_clear_record(object obj, fast_string recname)
// 功能：清除表格的内容
int nx_clear_record(void * state)
{
	IKernel * pKernel = ILuaExtModule::GetKernel(state);
	CHECK_ARG_NUM(state, nx_clear_record, 2);
	CHECK_ARG_OBJECT(state, nx_clear_record, 1);
	CHECK_ARG_STRING(state, nx_clear_record, 2);
	PERSISTID obj = pKernel->LuaToObject( state, 1 );
	IGameObj* pObj = pKernel->GetGameObj(obj);
	if (pObj == NULL)
	{
		return 1;
	}

	const char * recname = pKernel->LuaToString(state, 2);
	IRecord* pRecord = pObj->GetRecord(recname);
	if (pRecord == NULL)
	{
		return 1;
	}

	pKernel->LuaPushBool(state, pRecord->ClearRow());
	return 1;
}

#ifndef FSROOMLOGIC_EXPORTS
//原型：bool nx_set_record_flag(object obj, fast_string recname, int pos)
//功能：把表格作为标志位用
int nx_set_record_flag(void * state)
{
	IKernel * pKernel = ILuaExtModule::GetKernel(state);

	// 检查参数数量
	CHECK_ARG_NUM(state, nx_set_record_flag, 3);
	// 检查参数类型
	CHECK_ARG_OBJECT(state, nx_set_record_flag, 1);
	CHECK_ARG_STRING(state, nx_set_record_flag, 2);
	CHECK_ARG_INT(state, nx_set_record_flag, 3);

	// 获取参数
	PERSISTID obj = pKernel->LuaToObject(state, 1);
	const char * recname = pKernel->LuaToString(state, 2);
	int pos = pKernel->LuaToInt(state, 3);

	pKernel->LuaPushBool(state, pKernel->SetRecordFlag(obj, recname, pos));
	return 1;
}

//原型：bool nx_test_record_flag(object obj, fast_string recname, int pos)
//功能：把表格作为标志位用
int nx_test_record_flag(void * state)
{
	IKernel * pKernel = ILuaExtModule::GetKernel(state);

	// 检查参数数量
	CHECK_ARG_NUM(state, nx_test_record_flag, 3);
	// 检查参数类型
	CHECK_ARG_OBJECT(state, nx_test_record_flag, 1);
	CHECK_ARG_STRING(state, nx_test_record_flag, 2);
	CHECK_ARG_INT(state, nx_test_record_flag, 3);

	// 获取参数
	PERSISTID obj = pKernel->LuaToObject(state, 1);
	const char * recname = pKernel->LuaToString(state, 2);
	int pos = pKernel->LuaToInt(state, 3);

	pKernel->LuaPushBool(state, pKernel->TestRecordFlag(obj, recname, pos));
	return 1;
}

//原型：bool nx_test_record_flag(object obj, fast_string recname, int pos)
//功能：把表格作为标志位用
int nx_clear_record_flag(void * state)
{
	IKernel * pKernel = ILuaExtModule::GetKernel(state);

	// 检查参数数量
	CHECK_ARG_NUM(state, nx_clear_record_flag, 3);
	// 检查参数类型
	CHECK_ARG_OBJECT(state, nx_clear_record_flag, 1);
	CHECK_ARG_STRING(state, nx_clear_record_flag, 2);
	CHECK_ARG_INT(state, nx_clear_record_flag, 3);

	// 获取参数
	PERSISTID obj = pKernel->LuaToObject(state, 1);
	const char * recname = pKernel->LuaToString(state, 2);
	int pos = pKernel->LuaToInt(state, 3);

	pKernel->LuaPushBool(state, pKernel->ClearRecordFlag(obj, recname, pos));
	return 1;
}

//原型：bool nx_find_pubspace(string space)
//功能：查找是否存在指定公共空间
int nx_find_pubspace(void * state)
{
	IKernel * pKernel = ILuaExtModule::GetKernel(state);
	CHECK_ARG_NUM(state, nx_find_pubspace, 1);
	// 检查参数类型
	CHECK_ARG_STRING(state, nx_find_pubspace, 1);

	const char * space = pKernel->LuaToString(state, 1);
	pKernel->LuaPushBool(state, pKernel->FindPubSpace(space));
	return 1;
}

//原型：int nx_get_pubspace_count()
//功能：获得公共数据空间数量
int nx_get_pubspace_count(void * state)
{
	IKernel * pKernel = ILuaExtModule::GetKernel(state);

	pKernel->LuaPushInt(state, pKernel->GetPubSpaceCount());
	return 1;
}

//原型：table nx_get_pubspace_list()
//功能：获得公共数据空间名称列表
int nx_get_pubspace_list(void * state)
{
	IKernel * pKernel = ILuaExtModule::GetKernel(state);

	CVarList res;
	pKernel->GetPubSpaceList(res);

	pKernel->LuaPushTable(state, res);
	return 1;
}

//原型：table nx_get_pubdata_list(string space)
//功能：获得某个公共数据的所有属性
int nx_get_pubdata_list(void * state)
{
	IKernel * pKernel = ILuaExtModule::GetKernel(state);
	CHECK_ARG_NUM(state, nx_get_pubdata_list, 1);
	// 检查参数类型
	CHECK_ARG_STRING(state, nx_get_pubdata_list, 1);

	const char * space = pKernel->LuaToString(state, 1);
	IPubSpace * pPubSpace = pKernel->GetPubSpace(space);

	CVarList res;
	if (pPubSpace != NULL)
		pPubSpace->GetPubDataList(res);

	pKernel->LuaPushTable(state, res);
	return 1;
}

//原型：int nx_get_pubdata_count(string space)
//功能：获得某个公共数据的所有数据项的数量
int nx_get_pubdata_count(void * state)
{
	IKernel * pKernel = ILuaExtModule::GetKernel(state);
	CHECK_ARG_NUM(state, nx_get_pubdata_count, 1);
	// 检查参数类型
	CHECK_ARG_STRING(state, nx_get_pubdata_count, 1);

	const char * space = pKernel->LuaToString(state, 1);
	IPubSpace * pPubSpace = pKernel->GetPubSpace(space);

	int count = 0;
	if (pPubSpace != NULL)
		count = pPubSpace->GetPubDataCount();

	pKernel->LuaPushInt(state, count);
	return 1;
}

//原型：bool nx_find_pubdata(string space, wstring data)
//功能：查找指定公共空间是否存在指定数据
int nx_find_pubdata(void * state)
{
	IKernel * pKernel = ILuaExtModule::GetKernel(state);
	CHECK_ARG_NUM(state, nx_find_pubdata, 2);
	// 检查参数类型
	CHECK_ARG_STRING(state, nx_find_pubdata, 1);
	CHECK_ARG_WIDESTR(state, nx_find_pubdata, 2);

	const char * space = pKernel->LuaToString(state, 1);
	const wchar_t * data = pKernel->LuaToWideStr(state, 2);

	IPubSpace * pPubSpace = pKernel->GetPubSpace(space);

	bool bfound = false;
	if (pPubSpace != NULL)
		bfound = pPubSpace->FindPubData(data);

	pKernel->LuaPushBool(state, bfound);
	return 1;
}

//原型：string nx_get_pubdata_uid(string pubspace, wstring data)
//功能：获得公共数据空间中某个数据项的UID
int nx_get_pubdata_uid(void * state)
{
	IKernel * pKernel = ILuaExtModule::GetKernel(state);

	CHECK_ARG_NUM(state, nx_get_pubdata_uid, 2);
	// 检查参数类型
	CHECK_ARG_STRING(state, nx_get_pubdata_uid, 1);
	CHECK_ARG_WIDESTR(state, nx_get_pubdata_uid, 2);

	const char * space = pKernel->LuaToString(state, 1);
	const wchar_t * data = pKernel->LuaToWideStr(state, 2);

	fast_string uid;
	IPubSpace * pPubSpace = pKernel->GetPubSpace(space);
	if (pPubSpace != NULL)
	{
		IPubData * pPubData = pPubSpace->GetPubData(data);
		if (pPubData != NULL)
			uid = pPubData->GetUid();
	}

	pKernel->LuaPushString(state, uid.c_str());
	return 1;
}

//原型：bool nx_find_pubdata_property(string space, wstring data, fast_string prop)
//功能：查找指定公共空间的指定数据是否存在指定的属性名称
int nx_find_pubdata_property(void * state)
{
	IKernel * pKernel = ILuaExtModule::GetKernel(state);
	CHECK_ARG_NUM(state, nx_find_pubdata_property, 3);
	// 检查参数类型

	CHECK_ARG_STRING(state, nx_find_pubdata_property, 1);
	CHECK_ARG_WIDESTR(state, nx_find_pubdata_property, 2);
	CHECK_ARG_STRING(state, nx_find_pubdata_property, 3);

	const char * space = pKernel->LuaToString(state, 1);
	const wchar_t * data = pKernel->LuaToWideStr(state, 2);
	const char * prop = pKernel->LuaToString(state, 3);

	bool bfound = false;
	IPubSpace * pPubSpace = pKernel->GetPubSpace(space);
	if (pPubSpace != NULL)
	{
		IPubData * pPubData = pPubSpace->GetPubData(data);
		if (pPubData != NULL)
			bfound = pPubData->FindAttr(prop);
	}

	pKernel->LuaPushBool(state, bfound);
	return 1;
}

//原型：table nx_get_pubdata_property_list(string space, wstring data)
//功能：查找指定公共空间的指定数据的属性列表
int nx_get_pubdata_property_list(void * state)
{
	IKernel * pKernel = ILuaExtModule::GetKernel(state);
	CHECK_ARG_NUM(state, nx_get_pubdata_property_list, 2);
	// 检查参数类型

	CHECK_ARG_STRING(state, nx_get_pubdata_property_list, 1);
	CHECK_ARG_WIDESTR(state, nx_get_pubdata_property_list, 2);

	const char * space = pKernel->LuaToString(state, 1);
	const wchar_t * data = pKernel->LuaToWideStr(state, 2);

	CVarList res;
	IPubSpace * pPubSpace = pKernel->GetPubSpace(space);
	if (pPubSpace != NULL)
	{
		IPubData * pPubData = pPubSpace->GetPubData(data);
		if (pPubData != NULL)
			pPubData->GetAttrList(res);
	}

	pKernel->LuaPushTable(state, res);
	return 1;
}

//原型：int nx_get_pubdata_property_type(string space, wstring data, fast_string prop)
//功能：获得指定公共空间的指定数据是否存在指定的属性类型
int nx_get_pubdata_property_type(void * state)
{
	IKernel * pKernel = ILuaExtModule::GetKernel(state);
	CHECK_ARG_NUM(state, nx_get_pubdata_property_type, 3);
	// 检查参数类型

	CHECK_ARG_STRING(state, nx_get_pubdata_property_type, 1);
	CHECK_ARG_WIDESTR(state, nx_get_pubdata_property_type, 2);
	CHECK_ARG_STRING(state, nx_get_pubdata_property_type, 3);

	const char * space = pKernel->LuaToString(state, 1);
	const wchar_t * data = pKernel->LuaToWideStr(state, 2);
	const char * prop = pKernel->LuaToString(state, 3);

	int type = VTYPE_UNKNOWN;
	IPubSpace * pPubSpace = pKernel->GetPubSpace(space);
	if (pPubSpace != NULL)
	{
		IPubData * pPubData = pPubSpace->GetPubData(data);
		if (pPubData != NULL)
			type = pPubData->GetAttrType(prop);
	}

	pKernel->LuaPushInt(state, type);
	return 1;
}

//原型：var nx_query_pubdata_property(string space, wstring data, fast_string prop)
//功能：获得指定公共空间的指定数据是否存在指定的属性类型
int nx_query_pubdata_property(void * state)
{
	IKernel * pKernel = ILuaExtModule::GetKernel(state);
	CHECK_ARG_NUM(state, nx_query_pubdata_property, 3);
	// 检查参数类型

	CHECK_ARG_STRING(state, nx_query_pubdata_property, 1);
	CHECK_ARG_WIDESTR(state, nx_query_pubdata_property, 2);
	CHECK_ARG_STRING(state, nx_query_pubdata_property, 3);

	const char * space = pKernel->LuaToString(state, 1);
	const wchar_t * data = pKernel->LuaToWideStr(state, 2);
	const char * prop = pKernel->LuaToString(state, 3);

	IPubSpace * pPubSpace = pKernel->GetPubSpace(space);
	if (pPubSpace != NULL)
	{
		IPubData * pPubData = pPubSpace->GetPubData(data);
		if (pPubData != NULL)
		{
			switch(pPubData->GetAttrType(prop))
			{
			case VTYPE_INT:
				pKernel->LuaPushInt(state, pPubData->QueryAttrInt(prop));
				break;
			case VTYPE_INT64:
				pKernel->LuaPushInt64(state, pPubData->QueryAttrInt64(prop));
				break;
			case VTYPE_FLOAT:
				pKernel->LuaPushFloat(state, pPubData->QueryAttrFloat(prop));
				break;
			case VTYPE_DOUBLE:
				pKernel->LuaPushDouble(state, pPubData->QueryAttrDouble(prop));
				break;
			case VTYPE_STRING:
				pKernel->LuaPushString(state, pPubData->QueryAttrString(prop));
				break;
			case VTYPE_WIDESTR:
				pKernel->LuaPushWideStr(state, pPubData->QueryAttrWideStr(prop));
				break;
			case VTYPE_OBJECT:
				pKernel->LuaPushObject(state, pPubData->QueryAttrObject(prop));
				break;
			}
			return 1;
		}
	}

	pKernel->LuaPushInt(state, 0);
	return 1;
}

//原型：bool nx_find_pubdata_record(string space, wstring data, fast_string record)
//功能：查找指定公共空间的指定数据是否存在指定的表格名称
int nx_find_pubdata_record(void * state)
{
	IKernel * pKernel = ILuaExtModule::GetKernel(state);
	CHECK_ARG_NUM(state, nx_find_pubdata_record, 3);
	// 检查参数类型

	CHECK_ARG_STRING(state, nx_find_pubdata_record, 1);
	CHECK_ARG_WIDESTR(state, nx_find_pubdata_record, 2);
	CHECK_ARG_STRING(state, nx_find_pubdata_record, 3);

	const char * space = pKernel->LuaToString(state, 1);
	const wchar_t * data = pKernel->LuaToWideStr(state, 2);
	const char * record = pKernel->LuaToString(state, 3);

	bool bfound = false;
	IPubSpace * pPubSpace = pKernel->GetPubSpace(space);
	if (pPubSpace != NULL)
	{
		IPubData * pPubData = pPubSpace->GetPubData(data);
		if (pPubData != NULL)
			bfound = pPubData->FindRecord(record);
	}

	pKernel->LuaPushBool(state, bfound);
	return 1;
}

//原型：table nx_get_pubdata_record_list(string space, wstring data)
//功能：查找指定公共空间的指定数据的表格列表
int nx_get_pubdata_record_list(void * state)
{
	IKernel * pKernel = ILuaExtModule::GetKernel(state);
	CHECK_ARG_NUM(state, nx_get_pubdata_record_list, 2);
	// 检查参数类型

	CHECK_ARG_STRING(state, nx_get_pubdata_record_list, 1);
	CHECK_ARG_WIDESTR(state, nx_get_pubdata_record_list, 2);

	const char * space = pKernel->LuaToString(state, 1);
	const wchar_t * data = pKernel->LuaToWideStr(state, 2);

	CVarList res;
	IPubSpace * pPubSpace = pKernel->GetPubSpace(space);
	if (pPubSpace != NULL)
	{
		IPubData * pPubData = pPubSpace->GetPubData(data);
		if (pPubData != NULL)
			pPubData->GetRecordList(res);
	}

	pKernel->LuaPushTable(state, res);
	return 1;
}

//原型：int nx_get_pubdata_record_cols(string space, wstring data, fast_string record)
//功能：查找指定公共空间的指定数据指定的表格的列数
int nx_get_pubdata_record_cols(void * state)
{
	IKernel * pKernel = ILuaExtModule::GetKernel(state);
	CHECK_ARG_NUM(state, nx_get_pubdata_record_cols, 3);
	// 检查参数类型

	CHECK_ARG_STRING(state, nx_get_pubdata_record_cols, 1);
	CHECK_ARG_WIDESTR(state, nx_get_pubdata_record_cols, 2);
	CHECK_ARG_STRING(state, nx_get_pubdata_record_cols, 3);

	const char * space = pKernel->LuaToString(state, 1);
	const wchar_t * data = pKernel->LuaToWideStr(state, 2);
	const char * record = pKernel->LuaToString(state, 3);

	int cols = 0;
	IPubSpace * pPubSpace = pKernel->GetPubSpace(space);
	if (pPubSpace != NULL)
	{
		IPubData * pPubData = pPubSpace->GetPubData(data);
		if (pPubData != NULL)
			cols = pPubData->GetRecordCols(record);
	}

	pKernel->LuaPushInt(state, cols);
	return 1;
}

//原型：int nx_get_pubdata_record_rows(string space, wstring data, fast_string record)
//功能：查找指定公共空间的指定数据指定的表格的行数
int nx_get_pubdata_record_rows(void * state)
{
	IKernel * pKernel = ILuaExtModule::GetKernel(state);
	CHECK_ARG_NUM(state, nx_get_pubdata_record_rows, 3);
	// 检查参数类型

	CHECK_ARG_STRING(state, nx_get_pubdata_record_rows, 1);
	CHECK_ARG_WIDESTR(state, nx_get_pubdata_record_rows, 2);
	CHECK_ARG_STRING(state, nx_get_pubdata_record_rows, 3);

	const char * space = pKernel->LuaToString(state, 1);
	const wchar_t * data = pKernel->LuaToWideStr(state, 2);
	const char * record = pKernel->LuaToString(state, 3);

	int rows = 0;
	IPubSpace * pPubSpace = pKernel->GetPubSpace(space);
	if (pPubSpace != NULL)
	{
		IPubData * pPubData = pPubSpace->GetPubData(data);
		if (pPubData != NULL)
			rows = pPubData->GetRecordRows(record);
	}

	pKernel->LuaPushInt(state, rows);
	return 1;
}

//原型：int nx_get_pubdata_record_coltype(string space, wstring data, fast_string record, int col)
//功能：获得指定公共空间的指定数据的指定表格的指定列的数据类型
int nx_get_pubdata_record_coltype(void * state)
{
	IKernel * pKernel = ILuaExtModule::GetKernel(state);
	CHECK_ARG_NUM(state, nx_get_pubdata_record_coltype, 4);
	// 检查参数类型

	CHECK_ARG_STRING(state, nx_get_pubdata_record_coltype, 1);
	CHECK_ARG_WIDESTR(state, nx_get_pubdata_record_coltype, 2);
	CHECK_ARG_STRING(state, nx_get_pubdata_record_coltype, 3);
	CHECK_ARG_INT(state, nx_get_pubdata_record_coltype, 4);

	const char * space = pKernel->LuaToString(state, 1);
	const wchar_t * data = pKernel->LuaToWideStr(state, 2);
	const char * record = pKernel->LuaToString(state, 3);
	int col = pKernel->LuaToInt(state, 4);

	int type = VTYPE_UNKNOWN;
	IPubSpace * pPubSpace = pKernel->GetPubSpace(space);
	if (pPubSpace != NULL)
	{
		IPubData * pPubData = pPubSpace->GetPubData(data);
		if (pPubData != NULL)
			type = pPubData->GetRecordColType(record, col);
	}

	pKernel->LuaPushInt(state, type);
	return 1;
}

//原型：int nx_find_pubdata_record_value(string space, wstring data, fast_string record, int col, var value)
//功能：在指定公共空间的指定数据的指定表格的中查找指定列的指定值，返回行号
int nx_find_pubdata_record_value(void * state)
{
	IKernel * pKernel = ILuaExtModule::GetKernel(state);
	CHECK_ARG_NUM(state, nx_find_pubdata_record_value, 5);
	// 检查参数类型

	CHECK_ARG_STRING(state, nx_find_pubdata_record_value, 1);
	CHECK_ARG_WIDESTR(state, nx_find_pubdata_record_value, 2);
	CHECK_ARG_STRING(state, nx_find_pubdata_record_value, 3);
	CHECK_ARG_INT(state, nx_find_pubdata_record_value, 4);

	const char * space = pKernel->LuaToString(state, 1);
	const wchar_t * data = pKernel->LuaToWideStr(state, 2);
	const char * record = pKernel->LuaToString(state, 3);
	int col = pKernel->LuaToInt(state, 4);

	int row = -1;
	IPubSpace * pPubSpace = pKernel->GetPubSpace(space);
	if (pPubSpace != NULL)
	{
		IPubData * pPubData = pPubSpace->GetPubData(data);
		if (pPubData != NULL)
		{
			switch (pPubData->GetRecordColType(record, col))
			{
			case VTYPE_INT:
				row = pPubData->FindRecordInt(record, col, pKernel->LuaToInt(state, 5));
				break;
			case VTYPE_INT64:
				row = pPubData->FindRecordInt64(record, col, pKernel->LuaToInt64(state, 5));
				break;
			case VTYPE_FLOAT:
				row = pPubData->FindRecordFloat(record, col, pKernel->LuaToFloat(state, 5));
				break;
			case VTYPE_DOUBLE:
				row = pPubData->FindRecordDouble(record, col, pKernel->LuaToDouble(state, 5));
				break;
			case VTYPE_STRING:
				row = pPubData->FindRecordString(record, col, pKernel->LuaToString(state, 5));
				break;
			case VTYPE_WIDESTR:
				row = pPubData->FindRecordWideStr(record, col, pKernel->LuaToWideStr(state, 5));
				break;
			case VTYPE_OBJECT:
				row = pPubData->FindRecordObject(record, col, pKernel->LuaToObject(state, 5));
				break;
			}
		}
	}

	pKernel->LuaPushInt(state, row);
	return 1;
}

//原型：table nx_query_pubdata_record_row_values(string space, wstring data, fast_string record, int row)
//功能：获得指定公共空间的指定数据的指定表格的指定某行的数据
int nx_query_pubdata_record_row_values(void * state)
{
	IKernel * pKernel = ILuaExtModule::GetKernel(state);
	CHECK_ARG_NUM(state, nx_query_pubdata_record_row_values, 4);
	// 检查参数类型

	CHECK_ARG_STRING(state, nx_query_pubdata_record_row_values, 1);
	CHECK_ARG_WIDESTR(state, nx_query_pubdata_record_row_values, 2);
	CHECK_ARG_STRING(state, nx_query_pubdata_record_row_values, 3);
	CHECK_ARG_INT(state, nx_query_pubdata_record_row_values, 4);

	const char * space = pKernel->LuaToString(state, 1);
	const wchar_t * data = pKernel->LuaToWideStr(state, 2);
	const char * record = pKernel->LuaToString(state, 3);
	int row = pKernel->LuaToInt(state, 4);

	CVarList values;
	IPubSpace * pPubSpace = pKernel->GetPubSpace(space);
	if (pPubSpace != NULL)
	{
		IPubData * pPubData = pPubSpace->GetPubData(data);
		if (pPubData != NULL)
			pPubData->QueryRecordRowValue(record, row, values);
	}

	pKernel->LuaPushTable(state, values);
	return 1;
}

//原型：var nx_query_pubdata_record_value(string space, wstring data, fast_string record, int row, int col)
//功能：获得指定公共空间的指定数据的指定表格的指定某行某列的数据
int nx_query_pubdata_record_value(void * state)
{
	IKernel * pKernel = ILuaExtModule::GetKernel(state);
	CHECK_ARG_NUM(state, nx_query_pubdata_record_value, 5);
	// 检查参数类型

	CHECK_ARG_STRING(state, nx_query_pubdata_record_value, 1);
	CHECK_ARG_WIDESTR(state, nx_query_pubdata_record_value, 2);
	CHECK_ARG_STRING(state, nx_query_pubdata_record_value, 3);
	CHECK_ARG_INT(state, nx_query_pubdata_record_value, 4);
	CHECK_ARG_INT(state, nx_query_pubdata_record_value, 5);

	const char * space = pKernel->LuaToString(state, 1);
	const wchar_t * data = pKernel->LuaToWideStr(state, 2);
	const char * record = pKernel->LuaToString(state, 3);
	int row = pKernel->LuaToInt(state, 4);
	int col = pKernel->LuaToInt(state, 5);

	IPubSpace * pPubSpace = pKernel->GetPubSpace(space);
	if (pPubSpace != NULL)
	{
		IPubData * pPubData = pPubSpace->GetPubData(data);
		if (pPubData != NULL)
		{
			switch(pPubData->GetRecordColType(record, col))
			{
			case VTYPE_INT:
				pKernel->LuaPushInt(state, pPubData->QueryRecordInt(record, row, col));
				break;
			case VTYPE_INT64:
				pKernel->LuaPushInt64(state, pPubData->QueryRecordInt64(record, row, col));
				break;
			case VTYPE_FLOAT:
				pKernel->LuaPushFloat(state, pPubData->QueryRecordFloat(record, row, col));
				break;
			case VTYPE_DOUBLE:
				pKernel->LuaPushDouble(state, pPubData->QueryRecordDouble(record, row, col));
				break;
			case VTYPE_STRING:
				pKernel->LuaPushString(state, pPubData->QueryRecordString(record, row, col));
				break;
			case VTYPE_WIDESTR:
				pKernel->LuaPushWideStr(state, pPubData->QueryRecordWideStr(record, row, col));
				break;
			case VTYPE_OBJECT:
				pKernel->LuaPushObject(state, pPubData->QueryRecordObject(record, row, col));
				break;
			}
			return 1;
		}
	}

	pKernel->LuaPushInt(state, 0);
	return 1;
}
#endif // FSROOMLOGIC_EXPORTS

// 原型：bool nx_load_config(object obj)
// 功能：重新加载对象的配置文件
int nx_load_config(void * state)
{
	IKernel * pKernel = ILuaExtModule::GetKernel(state);

	// 检查参数数量
	CHECK_ARG_NUM(state, nx_load_config, 1);
	// 检查参数类型
	CHECK_ARG_OBJECT(state, nx_load_config, 1);
	PERSISTID obj = pKernel->LuaToObject(state, 1);

	pKernel->LuaPushBool(state, pKernel->LoadConfig(obj, pKernel->GetConfig(obj)));
	return 1;
}

// 原型：string nx_get_config_property(int configid, fast_string prop)
// 功能：物品未创建时，就可以直接根据id查询到配置文件中填写的某个属性的值
int nx_get_config_property(void * state)
{
	IKernel * pKernel = ILuaExtModule::GetKernel(state);

	// 检查参数数量
	CHECK_ARG_NUM(state, nx_get_config_property, 2);
	// 检查参数类型
	CHECK_ARG_STRING(state, nx_get_config_property, 1);
	CHECK_ARG_STRING(state, nx_get_config_property, 2);
	const char * configid = pKernel->LuaToString(state, 1);
	const char * prop = pKernel->LuaToString(state, 2);

	pKernel->LuaPushString(state, pKernel->GetConfigProperty(configid, prop));
	return 1;
}

// 原型：object nx_create_from_config(object container, fast_string config)
// 功能：根据配置文件创建物品
int nx_create_from_config(void * state)
{
	IKernel * pKernel = ILuaExtModule::GetKernel(state);

	CHECK_ARG_NUM(state, nx_create_from_config, 2);
	// 检查参数类型
	CHECK_ARG_OBJECT(state, nx_create_from_config, 1);
	CHECK_ARG_STRING(state, nx_create_from_config, 2);

	PERSISTID container = pKernel->LuaToObject(state, 1);//父对象
	const char * config = pKernel->LuaToString(state, 2);

	// 判断父容器是否存在
	if (!pKernel->Exists(container))
	{
		pKernel->LuaPushObject(state, PERSISTID());
		return 1;
	}
	
	// 在场景对象上创建临时对象，需要把临时对象放入临时对象容器中
	if (pKernel->Type(container) == TYPE_SCENE)
	{
		PERSISTID TmpObjContainer = pKernel->FindChild(container, L"SceneObjContainer", TYPE_HELPER);
		if (pKernel->Exists(TmpObjContainer))
		{
			container = TmpObjContainer;
		}
	}

	PERSISTID obj = pKernel->CreateFromConfig(container, "", config);
	pKernel->LuaPushObject(state, obj);

	return 1;
}

// 原型：object nx_createitem_from_config(object container, fast_string config, int num)
// 功能：根据配置文件创建物品
int nx_createitem_from_config(void * state)
{
	IKernel * pKernel = ILuaExtModule::GetKernel(state);

	CHECK_ARG_NUM(state, nx_createitem_from_config, 3);

	// 检查参数类型
	CHECK_ARG_OBJECT(state, nx_createitem_from_config, 1);
	CHECK_ARG_STRING(state, nx_createitem_from_config, 2);
	CHECK_ARG_INT(state, nx_createitem_from_config, 3);

	PERSISTID container = pKernel->LuaToObject(state, 1);//父对象
	const char * config = pKernel->LuaToString(state, 2);
	int count = pKernel->LuaToInt(state, 3);
	PERSISTID self = pKernel->Parent(container);

	// 判断父容器是否存在
	if (!pKernel->Exists(container))
	{
		pKernel->LuaPushObject(state, PERSISTID());
		return 1;
	}

	// 在场景对象上创建临时对象，需要把临时对象放入临时对象容器中
	if (pKernel->Type(container) == TYPE_SCENE)
	{
		PERSISTID TmpObjContainer = pKernel->FindChild(container, L"SceneObjContainer", TYPE_HELPER);
		if (pKernel->Exists(TmpObjContainer))
		{
			container = TmpObjContainer;
		}
	}

	PERSISTID obj = ItemBaseModule::m_pItemBaseModule->CreateItem(pKernel, container, config, count, FUNCTION_EVENT_ID_GM_COMMAND, true);
	
	pKernel->LuaPushObject(state, obj);

	return 1;
}

// 原型：object nx_create(object obj, fast_string script)
// 功能：根据脚本创建物品
int nx_create(void * state)
{
	IKernel * pKernel = ILuaExtModule::GetKernel(state);

	CHECK_ARG_NUM(state, nx_create, 2);
	// 检查参数类型
	CHECK_ARG_OBJECT(state, nx_create, 1);
	CHECK_ARG_STRING(state, nx_create, 2);

	PERSISTID obj = pKernel->LuaToObject(state, 1);
	const char * script = pKernel->LuaToString(state, 2);

	if (!pKernel->Exists(obj))
	{
		pKernel->LuaPushObject(state, PERSISTID());
		return 1;
	}

	// 在场景对象上创建临时对象，需要把临时对象放入临时对象容器中
	if (pKernel->Type(obj) == TYPE_SCENE)
	{
		PERSISTID TmpObjContainer = pKernel->FindChild(obj, L"SceneObjContainer", TYPE_HELPER);
		if (pKernel->Exists(TmpObjContainer))
		{
			obj = TmpObjContainer;
		}
	}

	pKernel->LuaPushObject(state, pKernel->Create(obj, script));
	return 1;
}

// 原型：object nx_create_container(object obj, fast_string script, int capacity)
// 功能：根据脚本创建容器
int nx_create_container(void * state)
{
	IKernel * pKernel = ILuaExtModule::GetKernel(state);

	CHECK_ARG_NUM(state, nx_create_container, 3);
	// 检查参数类型
	CHECK_ARG_OBJECT(state, nx_create_container, 1);
	CHECK_ARG_STRING(state, nx_create_container, 2);
	CHECK_ARG_INT(state, nx_create_container, 3);

	PERSISTID obj = pKernel->LuaToObject(state, 1);
	const char * script = pKernel->LuaToString(state, 2);
	int capacity = pKernel->LuaToInt(state, 3);

	pKernel->LuaPushObject(state, pKernel->CreateContainer(obj, script, capacity));
	return 1;
}

// 原型：bool nx_expand_container(object obj, int capacity)
// 功能：增大容器的容量
int nx_expand_container(void * state)
{
	IKernel * pKernel = ILuaExtModule::GetKernel(state);

	CHECK_ARG_NUM(state, nx_expand_container, 2);
	// 检查参数类型
	CHECK_ARG_OBJECT(state, nx_expand_container, 1);
	CHECK_ARG_INT(state, nx_expand_container, 2);

	PERSISTID obj = pKernel->LuaToObject(state, 1);
	int capacity = pKernel->LuaToInt(state, 2);

	pKernel->LuaPushBool(state, pKernel->ExpandContainer(obj, capacity));
	return 1;
}

// 原型：bool nx_shrink_container(object obj, int capacity)
// 功能：缩小容器的容量
int nx_shrink_container(void * state)
{
	IKernel * pKernel = ILuaExtModule::GetKernel(state);

	CHECK_ARG_NUM(state, nx_shrink_container, 2);
	// 检查参数类型
	CHECK_ARG_OBJECT(state, nx_shrink_container, 1);
	CHECK_ARG_INT(state, nx_shrink_container, 2);

	PERSISTID obj = pKernel->LuaToObject(state, 1);
	int capacity = pKernel->LuaToInt(state, 2);

	pKernel->LuaPushBool(state, pKernel->ShrinkContainer(obj, capacity));
	return 1;
}

// 原型：object nx_create_clone(object container, object prototype)
// 功能：复制物品
int nx_create_clone(void * state)
{
	IKernel * pKernel = ILuaExtModule::GetKernel(state);

	CHECK_ARG_NUM(state, nx_create_clone, 2);
	// 检查参数类型
	CHECK_ARG_OBJECT(state, nx_create_clone, 1);
	CHECK_ARG_OBJECT(state, nx_create_clone, 2);

	PERSISTID container = pKernel->LuaToObject(state, 1);
	PERSISTID prototype = pKernel->LuaToObject(state, 2);

	pKernel->LuaPushObject(state, pKernel->CreateClone(container, prototype, 0));
	return 1;
}

// 原型：object nx_create_to(object obj, fast_string script, float x, float y, float z, float orient, int capacity)
// 功能：根据脚本创建物体到场景的指定坐标
int nx_create_to(void * state)
{
	IKernel * pKernel = ILuaExtModule::GetKernel(state);

	CHECK_ARG_NUM(state, nx_create_to, 7);
	// 检查参数类型
	CHECK_ARG_OBJECT(state, nx_create_to, 1);
	CHECK_ARG_STRING(state, nx_create_to, 2);
	CHECK_ARG_FLOAT(state, nx_create_to, 3);
	CHECK_ARG_FLOAT(state, nx_create_to, 4);
	CHECK_ARG_FLOAT(state, nx_create_to, 5);
	CHECK_ARG_FLOAT(state, nx_create_to, 6);
	CHECK_ARG_INT(state, nx_create_to, 7);

	PERSISTID obj = pKernel->LuaToObject(state, 1);
	const char * script = pKernel->LuaToString(state, 2);
	float x = pKernel->LuaToFloat(state, 3);
	float y = pKernel->LuaToFloat(state, 4);
	float z = pKernel->LuaToFloat(state, 5);
	float orient = pKernel->LuaToFloat(state, 6);
	int capacity = pKernel->LuaToInt(state, 7);

	pKernel->LuaPushObject(state, pKernel->CreateTo(obj, script, x, y, z, orient, capacity));
	return 1;
}

// 原型：object nx_create_item(PERSISTID player, string config, int amount)
// 功能：创建物品到场景中的坐标位置
int nx_create_item(void * state)
{
	IKernel * pKernel = ILuaExtModule::GetKernel(state);

	CHECK_ARG_NUM(state, nx_create_item, 3);
	// 检查参数类型

	CHECK_ARG_OBJECT(state, nx_create_item, 1);
	CHECK_ARG_STRING(state, nx_create_item, 2);
	CHECK_ARG_INT(state, nx_create_item, 3);

	PERSISTID player = pKernel->LuaToObject(state, 1);
	const char * config = pKernel->LuaToString(state, 2);
	int amount = pKernel->LuaToInt(state, 3);

	PERSISTID obj;
	PERSISTID container = pKernel->GetViewportContainer(player, VIEWPORT_ITEM_BOX);
	if (pKernel->Exists(container))
	{
		obj = ItemBaseModule::m_pItemBaseModule->CreateItem(pKernel, container, config, amount, FUNCTION_EVENT_ID_GM_COMMAND, true);
	}

	pKernel->LuaPushObject(state, obj);
	
	return 1;
}

// 原型：object nx_create_obj(string config, int capacity, float x, float y, float z, float orient)
// 功能：创建物品到场景中的坐标位置
int nx_create_obj(void * state)
{
	IKernel * pKernel = ILuaExtModule::GetKernel(state);

	PERSISTID scene = pKernel->GetScene();
	IGameObj* pScene = pKernel->GetGameObj(scene);
	if (pScene == NULL)
	{
		return 1;
	}
	if (pScene->QueryInt("Grouping") >= 1)
	{
		//分组场景
		CHECK_ARG_NUM(state, nx_create_obj, 7);
		// 检查参数类型
		CHECK_ARG_STRING(state, nx_create_obj, 1);
		CHECK_ARG_INT(state, nx_create_obj, 2);
		CHECK_ARG_FLOAT(state, nx_create_obj, 3);
		CHECK_ARG_FLOAT(state, nx_create_obj, 4);
		CHECK_ARG_FLOAT(state, nx_create_obj, 5);
		CHECK_ARG_FLOAT(state, nx_create_obj, 6);
		CHECK_ARG_INT(state, nx_create_obj, 7);

		const char * config = pKernel->LuaToString(state, 1);
		int capacity = pKernel->LuaToInt(state, 2);
		float x = pKernel->LuaToFloat(state, 3);
		float y = pKernel->LuaToFloat(state, 4);
		float z = pKernel->LuaToFloat(state, 5);
		float orient = pKernel->LuaToFloat(state, 6);
		int nGroupID = pKernel->LuaToInt(state, 7);

		CVarList args;
		args << CREATE_TYPE_PROPERTY_VALUE << "GroupID" << nGroupID;
		PERSISTID obj = pKernel->CreateObjectArgs("", config, 0, x, y, z, orient, args);
		pKernel->LuaPushObject(state, obj);
	}
	else
	{
		CHECK_ARG_NUM(state, nx_create_obj, 6);
		// 检查参数类型
		CHECK_ARG_STRING(state, nx_create_obj, 1);
		CHECK_ARG_INT(state, nx_create_obj, 2);
		CHECK_ARG_FLOAT(state, nx_create_obj, 3);
		CHECK_ARG_FLOAT(state, nx_create_obj, 4);
		CHECK_ARG_FLOAT(state, nx_create_obj, 5);
		CHECK_ARG_FLOAT(state, nx_create_obj, 6);

		const char * config = pKernel->LuaToString(state, 1);
		int capacity = pKernel->LuaToInt(state, 2);
		float x = pKernel->LuaToFloat(state, 3);
		float y = pKernel->LuaToFloat(state, 4);
		float z = pKernel->LuaToFloat(state, 5);
		float orient = pKernel->LuaToFloat(state, 6);

		CVarList args;
		args << CREATE_TYPE_PROPERTY_VALUE << "GroupID" << -1;
		PERSISTID obj = pKernel->CreateObjectArgs("", config, 0, x, y, z, orient, args);
		pKernel->LuaPushObject(state, obj);
	}

	return 1;
}

// 原型：bool nx_destroy_self(object item)
// 功能：删除某个物品
int nx_destroy_self(void * state)
{
	IKernel * pKernel = ILuaExtModule::GetKernel(state);

	CHECK_ARG_NUM(state, nx_destroy_self, 1);
	// 检查参数类型
	CHECK_ARG_OBJECT(state, nx_destroy_self, 1);

	PERSISTID item = pKernel->LuaToObject(state, 1);
    if ( !pKernel->Exists(item) )
        return 0;

	pKernel->LuaPushBool(state, pKernel->DestroySelf(item));
	return 1;
}

// 原型：int nx_select(object self, object target, int funcid)
// 功能：对指定对象执行选择操作
int nx_select(void * state)
{
	IKernel * pKernel = ILuaExtModule::GetKernel(state);

	CHECK_ARG_NUM(state, nx_select, 3);
	// 检查参数类型
	CHECK_ARG_OBJECT(state, nx_select, 1);
	CHECK_ARG_OBJECT(state, nx_select, 2);
	CHECK_ARG_INT(state, nx_select, 3);

	PERSISTID self = pKernel->LuaToObject(state, 1);
	PERSISTID target = pKernel->LuaToObject(state, 2);
	int funcid = pKernel->LuaToInt(state, 3);

	pKernel->LuaPushInt(state, pKernel->Select(self, target, funcid));
	return 1;
}

// 原型：object nx_get_scene()
// 功能：获得当前场景
int nx_get_scene(void * state)
{
	IKernel * pKernel = ILuaExtModule::GetKernel(state);
	pKernel->LuaPushObject(state, pKernel->GetScene());
	return 1;
}

// 原型：int nx_get_scene_id()
// 功能：获得当前场景ID
int nx_get_scene_id(void * state)
{
	IKernel * pKernel = ILuaExtModule::GetKernel(state);
	pKernel->LuaPushInt(state, pKernel->GetSceneId());
	return 1;
}

// 原型：int nx_get_scene_maxid()
// 功能：获得最大的普通场景ID
int nx_get_scene_maxid(void * state)
{
	IKernel * pKernel = ILuaExtModule::GetKernel(state);
	pKernel->LuaPushInt(state, pKernel->GetSceneMaxId());
	return 1;
}

// 原型：string nx_get_scene_script(int scene_id)
// 功能：获得普通场景逻辑类名
int nx_get_scene_script(void * state)
{
	IKernel * pKernel = ILuaExtModule::GetKernel(state);
	CHECK_ARG_NUM(state, nx_get_scene_script, 1);
	// 检查参数类型
	CHECK_ARG_INT(state, nx_get_scene_script, 1);

	int scene_id = pKernel->LuaToInt(state, 1);

	pKernel->LuaPushString(state, pKernel->GetSceneScript(1));
	return 1;
}

// 原型：string nx_get_scene_config(int scene_id)
// 功能：获得普通场景配置名
int nx_get_scene_config(void * state)
{
	IKernel * pKernel = ILuaExtModule::GetKernel(state);
	CHECK_ARG_NUM(state, nx_get_scene_config, 1);
	// 检查参数类型
	CHECK_ARG_INT(state, nx_get_scene_config, 1);

	int scene_id = pKernel->LuaToInt(state, 1);

	pKernel->LuaPushString(state, pKernel->GetSceneConfig(scene_id));
	return 1;
}

// 原型：int nx_find_scene_id(string config)
// 功能：查找指定配置名的普通场景编号，返回-1表示没找到
int nx_find_scene_id(void * state)
{
	IKernel * pKernel = ILuaExtModule::GetKernel(state);
	CHECK_ARG_NUM(state, nx_find_scene_id, 1);
	// 检查参数类型
	CHECK_ARG_STRING(state, nx_find_scene_id, 1);

	const char * config = pKernel->LuaToString(state, 1);

	pKernel->LuaPushInt(state, pKernel->FindSceneId(config));
	return 1;
}

// 原型：bool nx_scene_exists(int scene_id)
// 功能：判断指定编号的场景是否存在
int nx_scene_exists(void * state)
{
	IKernel * pKernel = ILuaExtModule::GetKernel(state);
	CHECK_ARG_NUM(state, nx_scene_exists, 1);
	// 检查参数类型
	CHECK_ARG_INT(state, nx_scene_exists, 1);

	int scene_id = pKernel->LuaToInt(state, 1);

	pKernel->LuaPushInt(state, pKernel->GetSceneExists(scene_id));
	return 1;
}

// 原型：int nx_get_scene_player_count(int scene_id)
// 功能：获得指定场景内的玩家数量
int nx_get_scene_player_count(void * state)
{
	IKernel * pKernel = ILuaExtModule::GetKernel(state);
	CHECK_ARG_NUM(state, nx_get_scene_player_count, 1);
	// 检查参数类型
	CHECK_ARG_INT(state, nx_get_scene_player_count, 1);

	int scene_id = pKernel->LuaToInt(state, 1);

	pKernel->LuaPushInt(state, pKernel->GetScenePlayerCount(scene_id));
	return 1;
}

// 原型：table nx_get_scene_players(int scene_id)
// 功能：获得指定场景内的玩家名称列表（id为0表示所有场景）
int nx_get_scene_players(void * state)
{
	IKernel * pKernel = ILuaExtModule::GetKernel(state);
	CHECK_ARG_NUM(state, nx_get_scene_players, 1);
	// 检查参数类型
	CHECK_ARG_INT(state, nx_get_scene_players, 1);

	int scene_id = pKernel->LuaToInt(state, 1);

	CVarList res;
	pKernel->GetScenePlayerList(scene_id, res);

	pKernel->LuaPushTable(state, res);
	return 1;
}

// 原型：int nx_get_scene_class()
// 功能：取当前场景的类型（0-普通场景，1-副本场景的原型，2-副本场景）
int nx_get_scene_class(void * state)
{
	IKernel * pKernel = ILuaExtModule::GetKernel(state);

	pKernel->LuaPushInt(state, pKernel->GetSceneClass());
	return 1;
}

// 原型：int nx_get_clone_scene_count(int prototype_scene_id)
// 功能：获得指定原型场景号的副本数量
int nx_get_clone_scene_count(void * state)
{
	IKernel * pKernel = ILuaExtModule::GetKernel(state);

	CHECK_ARG_NUM(state, nx_get_clone_scene_count, 1);
	// 检查参数类型
	CHECK_ARG_INT(state, nx_get_clone_scene_count, 1);

	int prototype_scene_id = pKernel->LuaToInt(state, 1);
	pKernel->LuaPushInt(state, pKernel->GetCloneSceneCount(prototype_scene_id));
	return 1;
}

// 原型：table nx_get_clone_scene_list(int prototype_scene_id)
// 功能：获得指定原型场景号的副本场景的场景号列表
int nx_get_clone_scene_list(void * state)
{
	IKernel * pKernel = ILuaExtModule::GetKernel(state);

	CHECK_ARG_NUM(state, nx_get_clone_scene_list, 1);
	// 检查参数类型
	CHECK_ARG_INT(state, nx_get_clone_scene_list, 1);

	int prototype_scene_id = pKernel->LuaToInt(state, 1);

	CVarList res;
	pKernel->GetCloneSceneList(prototype_scene_id, res);

	pKernel->LuaPushTable(state, res);
	return 1;
}

// 原型：int nx_get_prototype_scene_id(int clone_scene_id)
// 功能：获得副本场景的原型场景ID
int nx_get_prototype_scene_id(void * state)
{
	IKernel * pKernel = ILuaExtModule::GetKernel(state);

	CHECK_ARG_NUM(state, nx_get_prototype_scene_id, 1);
	// 检查参数类型
	CHECK_ARG_INT(state, nx_get_prototype_scene_id, 1);

	int clone_scene_id = pKernel->LuaToInt(state, 1);

	pKernel->LuaPushInt(state, pKernel->GetPrototypeSceneId(clone_scene_id));
	return 1;
}

// 原型：bool nx_is_prototype_scene(int scene_id)
// 功能：指定场景号是否是副本场景的原型场景
int nx_is_prototype_scene(void * state)
{
	IKernel * pKernel = ILuaExtModule::GetKernel(state);

	CHECK_ARG_NUM(state, nx_is_prototype_scene, 1);
	// 检查参数类型
	CHECK_ARG_INT(state, nx_is_prototype_scene, 1);

	int scene_id = pKernel->LuaToInt(state, 1);

	pKernel->LuaPushBool(state, pKernel->IsPrototypeScene(scene_id));
	return 1;
}

// 原型：float, float, float, float nx_get_region()
// 功能：取地图范围（left,top,right,bottom 返回左上和右下角的坐标）
int nx_get_region(void * state)
{
	IKernel * pKernel = ILuaExtModule::GetKernel(state);

	float left, top, right, bottom;
	pKernel->GetMapBound(left, top, right, bottom);

	pKernel->LuaPushFloat(state, left);
	pKernel->LuaPushFloat(state, top);
	pKernel->LuaPushFloat(state, right);
	pKernel->LuaPushFloat(state, bottom);

	return 4;
}

// 原型：float nx_get_height(float x, float z)
// 功能：取高度(y值)
int nx_get_height(void * state)
{
	IKernel * pKernel = ILuaExtModule::GetKernel(state);

	CHECK_ARG_NUM(state, nx_get_height, 3);
	// 检查参数类型
	CHECK_ARG_FLOAT(state, nx_get_height, 1);
	CHECK_ARG_FLOAT(state, nx_get_height, 2);
	CHECK_ARG_INT(state, nx_get_height, 3);

	float x = pKernel->LuaToFloat(state, 1);
	float z = pKernel->LuaToFloat(state, 2);
	int nFloor = pKernel->LuaToInt(state, 3);

    int floor_count = pKernel->GetFloorCount(x, z);
	pKernel->LuaPushFloat(state, GetWalkHeight(pKernel, x, z, nFloor));
	return 1;
}

// 原型：int nx_get_grid_index(float x, float z)
// 功能：格子索引
int nx_get_grid_index(void * state)
{
	IKernel * pKernel = ILuaExtModule::GetKernel(state);

	CHECK_ARG_NUM(state, nx_get_grid_index, 2);
	// 检查参数类型
	CHECK_ARG_FLOAT(state, nx_get_grid_index, 1);
	CHECK_ARG_FLOAT(state, nx_get_grid_index, 2);

	float x = pKernel->LuaToFloat(state, 1);
	float z = pKernel->LuaToFloat(state, 2);

	int nGridIndex = pKernel->GetGridIndexByPos(x, z);
	pKernel->LuaPushInt(state, nGridIndex);
	return 1;
}

// 原型：int nx_get_map_type(float x, float z)
// 功能：取地表类型
int nx_get_map_type(void * state)
{
	IKernel * pKernel = ILuaExtModule::GetKernel(state);

	CHECK_ARG_NUM(state, nx_get_map_type, 2);
	// 检查参数类型
	CHECK_ARG_FLOAT(state, nx_get_map_type, 1);
	CHECK_ARG_FLOAT(state, nx_get_map_type, 2);

	float x = pKernel->LuaToFloat(state, 1);
	float z = pKernel->LuaToFloat(state, 2);

	pKernel->LuaPushInt(state, pKernel->GetMapType(x, z));
	return 1;
}

// 原型：int nx_get_walk_type(float x, float z)
// 功能：取行走类型
int nx_get_walk_type(void * state)
{
	IKernel * pKernel = ILuaExtModule::GetKernel(state);

	CHECK_ARG_NUM(state, nx_get_walk_type, 2);
	// 检查参数类型
	CHECK_ARG_FLOAT(state, nx_get_walk_type, 1);
	CHECK_ARG_FLOAT(state, nx_get_walk_type, 2);

	float x = pKernel->LuaToFloat(state, 1);
	float z = pKernel->LuaToFloat(state, 2);

	pKernel->LuaPushInt(state, pKernel->GetWalkType(x, z));
	return 1;
}

// 原型：bool nx_can_walk(float x, float z)
// 功能：测试一个点是否可行走区域
int nx_can_walk(void * state)
{
	IKernel * pKernel = ILuaExtModule::GetKernel(state);

	CHECK_ARG_NUM(state, nx_can_walk, 2);
	// 检查参数类型
	CHECK_ARG_FLOAT(state, nx_can_walk, 1);
	CHECK_ARG_FLOAT(state, nx_can_walk, 2);

	float x = pKernel->LuaToFloat(state, 1);
	float z = pKernel->LuaToFloat(state, 2);

	pKernel->LuaPushBool(state, pKernel->CanWalk(x, z));
	return 1;
}

// 原型：bool nx_obj_can_walk(object obj, float x, float z)
// 功能：测试一个点对象是否可行走
int nx_obj_can_walk(void * state)
{
	IKernel * pKernel = ILuaExtModule::GetKernel(state);

	CHECK_ARG_NUM(state, nx_obj_can_walk, 3);
	// 检查参数类型
	CHECK_ARG_OBJECT(state, nx_obj_can_walk, 1);
	CHECK_ARG_FLOAT(state, nx_obj_can_walk, 2);
	CHECK_ARG_FLOAT(state, nx_obj_can_walk, 3);

	PERSISTID obj = pKernel->LuaToObject(state, 1);
	float x = pKernel->LuaToFloat(state, 2);
	float z = pKernel->LuaToFloat(state, 3);

	pKernel->LuaPushBool(state, pKernel->ObjectCanWalk(obj, x, z));
	return 1;
}

// 原型：bool nx_line_can_walk(float step, float src_x, float src_z, float dst_x, float dst_z)
// 功能：测试直线是否可行走
int nx_line_can_walk(void * state)
{
	IKernel * pKernel = ILuaExtModule::GetKernel(state);

	CHECK_ARG_NUM(state, nx_line_can_walk, 5);
	// 检查参数类型
	CHECK_ARG_FLOAT(state, nx_line_can_walk, 1);
	CHECK_ARG_FLOAT(state, nx_line_can_walk, 2);
	CHECK_ARG_FLOAT(state, nx_line_can_walk, 3);
	CHECK_ARG_FLOAT(state, nx_line_can_walk, 4);
	CHECK_ARG_FLOAT(state, nx_line_can_walk, 5);

	float step = pKernel->LuaToFloat(state, 1);
	float src_x = pKernel->LuaToFloat(state, 2);
	float src_z = pKernel->LuaToFloat(state, 3);
	float dst_x = pKernel->LuaToFloat(state, 4);
	float dst_z = pKernel->LuaToFloat(state, 5);

	pKernel->LuaPushBool(state, pKernel->LineCanWalk(step, src_x, src_z, dst_x, dst_z));
	return 1;
}

// 原型：bool nx_obj_line_can_walk(object obj, float step, float src_x, float src_z, float dst_x, float dst_z)
// 功能：测试对象直线是否可行走
int nx_obj_line_can_walk(void * state)
{
	IKernel * pKernel = ILuaExtModule::GetKernel(state);

	CHECK_ARG_NUM(state, nx_obj_line_can_walk, 6);
	// 检查参数类型
	CHECK_ARG_OBJECT(state, nx_obj_line_can_walk, 1);
	CHECK_ARG_FLOAT(state, nx_obj_line_can_walk, 2);
	CHECK_ARG_FLOAT(state, nx_obj_line_can_walk, 3);
	CHECK_ARG_FLOAT(state, nx_obj_line_can_walk, 4);
	CHECK_ARG_FLOAT(state, nx_obj_line_can_walk, 5);
	CHECK_ARG_FLOAT(state, nx_obj_line_can_walk, 6);

	PERSISTID obj = pKernel->LuaToObject(state, 1);
	float step = pKernel->LuaToFloat(state, 2);
	float src_x = pKernel->LuaToFloat(state, 3);
	float src_z = pKernel->LuaToFloat(state, 4);
	float dst_x = pKernel->LuaToFloat(state, 5);
	float dst_z = pKernel->LuaToFloat(state, 6);

	pKernel->LuaPushBool(state, pKernel->ObjectLineCanWalk(obj, step, src_x, src_z, dst_x, dst_z));
	return 1;
}

// 原型：bool, new_x, new_y, new_z nx_trace_line_walk(float step, float src_x, float src_y, float src_z, float dst_x, float dst_z)
// 功能：测试对象直线是否可行走
int nx_trace_line_walk(void * state)
{
	IKernel * pKernel = ILuaExtModule::GetKernel(state);

	CHECK_ARG_NUM(state, nx_trace_line_walk, 6);
	// 检查参数类型
	CHECK_ARG_FLOAT(state, nx_trace_line_walk, 1);
	CHECK_ARG_FLOAT(state, nx_trace_line_walk, 2);
	CHECK_ARG_FLOAT(state, nx_trace_line_walk, 3);
	CHECK_ARG_FLOAT(state, nx_trace_line_walk, 4);
	CHECK_ARG_FLOAT(state, nx_trace_line_walk, 5);
	CHECK_ARG_FLOAT(state, nx_trace_line_walk, 6);

	float step = pKernel->LuaToFloat(state, 1);
	float src_x = pKernel->LuaToFloat(state, 2);
	float src_y = pKernel->LuaToFloat(state, 3);
	float src_z = pKernel->LuaToFloat(state, 4);
	float dst_x = pKernel->LuaToFloat(state, 5);
	float dst_z = pKernel->LuaToFloat(state, 6);

    if (::util_dot_distance(src_x, src_z, dst_x, dst_z) > 100.0f)
    {
        pKernel->LuaPushBool(state, false);

        return 1;
    }

	float new_x, new_y, new_z;

	bool result = pKernel->TraceLineWalk(step, src_x, src_y, src_z, dst_x, dst_z, new_x, new_y, new_z);
	pKernel->LuaPushBool(state, result);

	pKernel->LuaPushFloat(state, new_x);
	pKernel->LuaPushFloat(state, new_y);
	pKernel->LuaPushFloat(state, new_z);

	return 4;
}


// 原型：float, float, float, float nx_get_position(object obj)
// 功能：取对象坐标(返回x, y, z, orient)
int nx_get_position(void * state)
{
	IKernel * pKernel = ILuaExtModule::GetKernel(state);

	CHECK_ARG_NUM(state, nx_get_position, 1);
	// 检查参数类型
	CHECK_ARG_OBJECT(state, nx_get_position, 1);

    float x=0.0f, y=0.0f, z=0.0f, orient=0.0f;
	PERSISTID obj = pKernel->LuaToObject(state, 1);
    if ( pKernel->Exists(obj) )
    {
        x = pKernel->GetPosiX(obj);
        y = pKernel->GetPosiY(obj);
        z = pKernel->GetPosiZ(obj);
        orient = pKernel->GetOrient(obj);
    }

	pKernel->LuaPushFloat(state, x);
	pKernel->LuaPushFloat(state, y);
	pKernel->LuaPushFloat(state, z);
	pKernel->LuaPushFloat(state, orient);

	return 4;
}

// 原型：float nx_get_move_speed(object obj)
// 功能：取对象速度
int nx_get_move_speed(void * state)
{
	IKernel * pKernel = ILuaExtModule::GetKernel(state);

	CHECK_ARG_NUM(state, nx_get_move_speed, 1);
	// 检查参数类型
	CHECK_ARG_OBJECT(state, nx_get_move_speed, 1);

	PERSISTID obj = pKernel->LuaToObject(state, 1);

	pKernel->LuaPushFloat(state, pKernel->GetMoveSpeed(obj));
	return 1;
}

// 原型：float nx_get_rotate_speed(object obj)
// 功能：取转向速度
int nx_get_rotate_speed(void * state)
{
	IKernel * pKernel = ILuaExtModule::GetKernel(state);

	CHECK_ARG_NUM(state, nx_get_rotate_speed, 1);
	// 检查参数类型
	CHECK_ARG_OBJECT(state, nx_get_rotate_speed, 1);

	PERSISTID obj = pKernel->LuaToObject(state, 1);

	pKernel->LuaPushFloat(state, pKernel->GetRotateSpeed(obj));
	return 1;
}


// 原型：float nx_get_move_mode (object obj)
// 功能：获取对象的移动状态（返回值的枚举在 knlconst.h, define.lua）
int nx_get_move_mode (void * state)
{
	IKernel* pKernel = ILuaExtModule::GetKernel(state);

	CHECK_ARG_NUM(state, nx_get_move_mode, 1);
	// 检查参数类型
	CHECK_ARG_OBJECT(state, nx_get_move_mode, 1);

	PERSISTID obj = pKernel->LuaToObject(state, 1);
	pKernel->LuaPushInt (state, pKernel->GetMoveMode(obj)&0xff);

	return 1;
}

// 原型：bool nx_moveto(object obj, float x, float y, float z, float orient)
// 功能：切换到本场景的指定位置
int nx_moveto(void * state)
{
	IKernel * pKernel = ILuaExtModule::GetKernel(state);

	CHECK_ARG_NUM(state, nx_moveto, 5);
	// 检查参数类型
	CHECK_ARG_OBJECT(state, nx_moveto, 1);
	CHECK_ARG_FLOAT(state, nx_moveto, 2);
	CHECK_ARG_FLOAT(state, nx_moveto, 3);
	CHECK_ARG_FLOAT(state, nx_moveto, 4);
	CHECK_ARG_FLOAT(state, nx_moveto, 5);

	PERSISTID obj = pKernel->LuaToObject(state, 1);
	float x = pKernel->LuaToFloat(state, 2);
	float y = pKernel->LuaToFloat(state, 3);
	float z = pKernel->LuaToFloat(state, 4);
	float orient = pKernel->LuaToFloat(state, 5);

	pKernel->LuaPushBool(state, pKernel->MoveTo(obj, x, y, z, orient));
	
	//pKernel->CustomByKen(obj, CVarList() << SERVER_CUSTOMMSG_MOVE_TO << obj << x << y << z);

	return 1;
}

// 原型：bool nx_rotate(object obj, fast_string leftstate, fast_string rightstate, float angle, float rotatespeed)
// 功能：旋转角度
int nx_rotate(void * state)
{
	IKernel * pKernel = ILuaExtModule::GetKernel(state);

	CHECK_ARG_NUM(state, nx_rotate, 3);
	// 检查参数类型
	CHECK_ARG_OBJECT(state, nx_rotate, 1);
	CHECK_ARG_FLOAT(state, nx_rotate, 2);
	CHECK_ARG_FLOAT(state, nx_rotate, 3);

	PERSISTID obj = pKernel->LuaToObject(state, 1);
	float angle = pKernel->LuaToFloat(state, 2);
	float speed = pKernel->LuaToFloat(state, 3);

	pKernel->LuaPushBool(state, pKernel->Rotate(obj, angle, speed));
	return 1;
}

int nx_motion( void *state)
{
	IKernel * pKernel = ILuaExtModule::GetKernel(state);

	CHECK_ARG_NUM(state, nx_motion, 5);
	// 检查参数类型
	CHECK_ARG_OBJECT(state, nx_motion, 1);
	CHECK_ARG_FLOAT(state, nx_motion, 2);
	CHECK_ARG_FLOAT(state, nx_motion, 3);
	CHECK_ARG_FLOAT(state, nx_motion, 4);
	CHECK_ARG_FLOAT(state, nx_motion, 5);

	PERSISTID obj = pKernel->LuaToObject(state, 1);
	float runspeed = pKernel->LuaToFloat(state, 2);
	float rotatespeed = pKernel->LuaToFloat(state, 3);
	float x = pKernel->LuaToFloat(state,4);
	float z = pKernel->LuaToFloat(state,5);

	pKernel->LuaPushBool(state, pKernel->Motion(obj, runspeed, rotatespeed, x, z));
	return 1;
}

int nx_location( void *state)
{
	IKernel * pKernel = ILuaExtModule::GetKernel(state);

	CHECK_ARG_NUM(state, nx_location, 5);
	// 检查参数类型
	CHECK_ARG_OBJECT(state, nx_location, 1);
	CHECK_ARG_FLOAT(state, nx_location, 2);
	CHECK_ARG_FLOAT(state, nx_location, 3);
	CHECK_ARG_FLOAT(state, nx_location, 4);
	CHECK_ARG_FLOAT(state, nx_location, 5);

	PERSISTID obj = pKernel->LuaToObject(state, 1);
	float x = pKernel->LuaToFloat(state, 2);
	float y = pKernel->LuaToFloat(state, 3);
	float z = pKernel->LuaToFloat(state,4);
	float orient = pKernel->LuaToFloat(state,5);

	pKernel->LuaPushBool(state, pKernel->Locate(obj, x, y, z, orient));
	return 1;
}

// 原型：bool nx_motion_no_rotate(object obj, float speed, float x, float z)
// 功能：控制对象运动，不做旋转
int nx_motion_no_rotate(void * state)
{
	IKernel * pKernel = ILuaExtModule::GetKernel(state);

	CHECK_ARG_NUM(state, nx_motion_no_rotate, 4);
	// 检查参数类型
	CHECK_ARG_OBJECT(state, nx_motion_no_rotate, 1);
	CHECK_ARG_FLOAT(state, nx_motion_no_rotate, 2);
	CHECK_ARG_FLOAT(state, nx_motion_no_rotate, 3);
	CHECK_ARG_FLOAT(state, nx_motion_no_rotate, 4);

	PERSISTID obj = pKernel->LuaToObject(state, 1);
	float speed = pKernel->LuaToFloat(state, 2);
	float x = pKernel->LuaToFloat(state, 3);
	float z = pKernel->LuaToFloat(state, 4);

	pKernel->LuaPushBool(state, pKernel->MotionNoRotate(obj, speed, x, z));
	return 1;
}

// 原型：bool nx_stop(object obj)
// 功能：停止运动（移动和旋转）
int nx_stop(void * state)
{
	IKernel * pKernel = ILuaExtModule::GetKernel(state);

	CHECK_ARG_NUM(state, nx_stop, 1);
	// 检查参数类型
	CHECK_ARG_OBJECT(state, nx_stop, 1);

	PERSISTID obj = pKernel->LuaToObject(state, 1);

	pKernel->LuaPushBool(state, pKernel->Stop(obj));
	return 1;
}

// 原型：bool nx_stop_walk(object obj)
// 功能：停止运动（移动）
int nx_stop_walk(void * state)
{
	IKernel * pKernel = ILuaExtModule::GetKernel(state);

	CHECK_ARG_NUM(state, nx_stop_walk, 1);
	// 检查参数类型
	CHECK_ARG_OBJECT(state, nx_stop_walk, 1);

	PERSISTID obj = pKernel->LuaToObject(state, 1);

	pKernel->LuaPushBool(state, pKernel->StopWalk(obj));
	return 1;
}

// 原型：bool nx_stop_rotate(object obj)
// 功能：停止运动（旋转）
int nx_stop_rotate(void * state)
{
	IKernel * pKernel = ILuaExtModule::GetKernel(state);

	CHECK_ARG_NUM(state, nx_stop_rotate, 1);
	// 检查参数类型
	CHECK_ARG_OBJECT(state, nx_stop_rotate, 1);

	PERSISTID obj = pKernel->LuaToObject(state, 1);

	pKernel->LuaPushBool(state, pKernel->StopRotate(obj));
	return 1;
}

// 原型：float, float nx_get_dest_position(object obj)
// 功能：取目标点(返回x, z)
int nx_get_dest_position(void * state)
{
	IKernel * pKernel = ILuaExtModule::GetKernel(state);

	CHECK_ARG_NUM(state, nx_get_dest_position, 1);
	// 检查参数类型
	CHECK_ARG_OBJECT(state, nx_get_dest_position, 1);

	PERSISTID obj = pKernel->LuaToObject(state, 1);

	pKernel->LuaPushFloat(state, pKernel->GetDestX(obj));
	pKernel->LuaPushFloat(state, pKernel->GetDestZ(obj));
	return 2;
}

// 原型：float, float nx_get_motion_state(object obj)
// 功能：取目标点(返回state)
int nx_get_motion_state(void * state)
{
	IKernel* pKernel = ILuaExtModule::GetKernel(state);

    // 检查参数数量与类型
	CHECK_ARG_NUM(state, nx_get_motion_state, 1);	
	CHECK_ARG_OBJECT(state, nx_get_motion_state, 1);

	PERSISTID self = pKernel->LuaToObject(state, 1);
	pKernel->LuaPushInt (state, pKernel->GetMoveMode(self)&0xff);
	return 1;
}

// 原型：float nx_distance(object obj, object other)
// 功能：两对象的二维空间距离（x, z）
int nx_distance(void * state)
{
	IKernel * pKernel = ILuaExtModule::GetKernel(state);

	CHECK_ARG_NUM(state, nx_distance, 2);
	// 检查参数类型
	CHECK_ARG_OBJECT(state, nx_distance, 1);
	CHECK_ARG_OBJECT(state, nx_distance, 2);

	PERSISTID obj = pKernel->LuaToObject(state, 1);
	PERSISTID other = pKernel->LuaToObject(state, 2);

	pKernel->LuaPushFloat(state, pKernel->Distance2D(obj, other));
	return 1;
}

// 原型：float nx_distance3d(object obj, object other)
// 功能：两对象的三维空间距离（x, y, z）
int nx_distance3d(void * state)
{
	IKernel * pKernel = ILuaExtModule::GetKernel(state);

	CHECK_ARG_NUM(state, nx_distance3d, 2);
	// 检查参数类型
	CHECK_ARG_OBJECT(state, nx_distance3d, 1);
	CHECK_ARG_OBJECT(state, nx_distance3d, 2);

	PERSISTID obj = pKernel->LuaToObject(state, 1);
	PERSISTID other = pKernel->LuaToObject(state, 2);

	pKernel->LuaPushFloat(state, pKernel->Distance3D(obj, other));
	return 1;
}

// 原型：float nx_angle(object obj, object other)
// 功能：一个对象相对自己前方向的角度
int nx_angle(void * state)
{
	IKernel * pKernel = ILuaExtModule::GetKernel(state);

	CHECK_ARG_NUM(state, nx_angle, 2);
	// 检查参数类型
	CHECK_ARG_OBJECT(state, nx_angle, 1);
	CHECK_ARG_OBJECT(state, nx_angle, 2);

	PERSISTID obj = pKernel->LuaToObject(state, 1);
	PERSISTID other = pKernel->LuaToObject(state, 2);

	pKernel->LuaPushFloat(state, pKernel->Angle(obj, other));
	return 1;
}

// 原型：float nx_dot_angle(object obj, float x, float z)
// 功能：一个点相对自己前方向的角度
int nx_dot_angle(void * state)
{
	IKernel * pKernel = ILuaExtModule::GetKernel(state);

	CHECK_ARG_NUM(state, nx_dot_angle, 3);
	// 检查参数类型
	CHECK_ARG_OBJECT(state, nx_dot_angle, 1);
	CHECK_ARG_FLOAT(state, nx_dot_angle, 2);
	CHECK_ARG_FLOAT(state, nx_dot_angle, 3);

	PERSISTID obj = pKernel->LuaToObject(state, 1);
	float x = pKernel->LuaToFloat(state, 2);
	float z = pKernel->LuaToFloat(state, 3);

	pKernel->LuaPushFloat(state, pKernel->DotAngle(obj, x, z));
	return 1;
}

// 原型：int, float, float, float, float nx_get_location(string location)
// 功能：获得全局场景切换点的场景号和坐标信息
int nx_get_location(void * state)
{
	IKernel * pKernel = ILuaExtModule::GetKernel(state);

	CHECK_ARG_NUM(state, nx_get_location, 1);
	// 检查参数类型
	CHECK_ARG_STRING(state, nx_get_location, 1);

	const char * location = pKernel->LuaToString(state, 1);

	int sceneid;
	float x, y, z, orient;
	pKernel->GetSwitchLocation(location, sceneid, x, y, z, orient);

	pKernel->LuaPushInt(state, sceneid);
	pKernel->LuaPushFloat(state, x);
	pKernel->LuaPushFloat(state, y);
	pKernel->LuaPushFloat(state, z);
	pKernel->LuaPushFloat(state, orient);

	return 5;
}

// 原型：bool nx_switch_locate(object obj, int scene, float x, float y, float z, float orient)
// 功能：切换场景到指定的场景号和坐标
int nx_switch_locate(void * state)
{
	IKernel * pKernel = ILuaExtModule::GetKernel(state);

	CHECK_ARG_NUM(state, nx_switch_locate, 6);
	// 检查参数类型
	CHECK_ARG_OBJECT(state, nx_switch_locate, 1);
	CHECK_ARG_INT(state, nx_switch_locate, 2);
	CHECK_ARG_FLOAT(state, nx_switch_locate, 3);
	CHECK_ARG_FLOAT(state, nx_switch_locate, 4);
	CHECK_ARG_FLOAT(state, nx_switch_locate, 5);
	CHECK_ARG_FLOAT(state, nx_switch_locate, 6);

	PERSISTID obj = pKernel->LuaToObject(state, 1);
	int sceneid = pKernel->LuaToInt(state, 2);
	float x = pKernel->LuaToFloat(state, 3);
	float y = pKernel->LuaToFloat(state, 4);
	float z = pKernel->LuaToFloat(state, 5);
	float orient = pKernel->LuaToFloat(state, 6);

	pKernel->LuaPushBool(state, g_pAsynCtrlModule->SwitchLocate(pKernel, obj, sceneid, x, y, z, orient));
	return 1;
}

// 原型：bool nx_switch_born(object obj, int scene)
// 功能：切换到指定场景的出生点
int nx_switch_born(void * state)
{
	IKernel * pKernel = ILuaExtModule::GetKernel(state);

	CHECK_ARG_NUM(state, nx_switch_born, 2);
	// 检查参数类型
	CHECK_ARG_OBJECT(state, nx_switch_born, 1);
	CHECK_ARG_INT(state, nx_switch_born, 2);

	PERSISTID obj = pKernel->LuaToObject(state, 1);
	int sceneid = pKernel->LuaToInt(state, 2);

	pKernel->LuaPushBool(state, g_pAsynCtrlModule->SwitchBorn(pKernel, obj, sceneid));
	return 1;
}

// 原型：float, float, float, float nx_get_scene_born()
// 功能：获得场景的出生点位置
int nx_get_scene_born(void * state)
{
	IKernel * pKernel = ILuaExtModule::GetKernel(state);

	float x, y, z, orient;
	pKernel->GetSceneBorn(x, y, z, orient);

	pKernel->LuaPushFloat(state, x);
	pKernel->LuaPushFloat(state, y);
	pKernel->LuaPushFloat(state, z);
	pKernel->LuaPushFloat(state, orient);
	return 4;
}

// 原型：bool nx_set_scene_born(float x, float y, float z, float orient)
// 功能：设置场景的出生点位置
int nx_set_scene_born(void * state)
{
	IKernel * pKernel = ILuaExtModule::GetKernel(state);

	CHECK_ARG_NUM(state, nx_set_scene_born, 4);
	// 检查参数类型
	CHECK_ARG_FLOAT(state, nx_set_scene_born, 1);
	CHECK_ARG_FLOAT(state, nx_set_scene_born, 2);
	CHECK_ARG_FLOAT(state, nx_set_scene_born, 3);
	CHECK_ARG_FLOAT(state, nx_set_scene_born, 4);

	float x = pKernel->LuaToFloat(state, 1);
	float y = pKernel->LuaToFloat(state, 2);
	float z = pKernel->LuaToFloat(state, 3);
	float orient = pKernel->LuaToFloat(state, 4);

	pKernel->LuaPushBool(state, pKernel->SetSceneBorn(x, y, z, orient));
	return 1;
}

// 原型：int nx_type(object obj)
// 功能：获得对象的类型
// TYPE_SCENE	= 1,	// 场景
// TYPE_PLAYER	= 2,	// 玩家
// TYPE_NPC	= 4,	// NPC
// TYPE_ITEM	= 8,	// 物品
// TYPE_HELPER	= 16,	// 辅助对象
int nx_type(void * state)
{
	IKernel * pKernel = ILuaExtModule::GetKernel(state);

	// 检查参数数量
	CHECK_ARG_NUM(state, nx_type, 1);
	// 检查参数类型
	CHECK_ARG_OBJECT(state, nx_type, 1);
	PERSISTID obj = pKernel->LuaToObject(state, 1);

	pKernel->LuaPushInt(state, pKernel->Type(obj));
	return 1;
}

// 原型：string nx_get_config(object obj)
// 功能：获得物品的配置文件
int nx_get_config(void * state)
{
	IKernel * pKernel = ILuaExtModule::GetKernel(state);

	// 检查参数数量
	CHECK_ARG_NUM(state, nx_get_config, 1);
	// 检查参数类型
	CHECK_ARG_OBJECT(state, nx_get_config, 1);
	PERSISTID obj = pKernel->LuaToObject(state, 1);

	pKernel->LuaPushString(state, pKernel->GetConfig(obj));
	return 1;
}

// 原型：string nx_get_script(object obj)
// 功能：获得物品的脚本文件
int nx_get_script(void * state)
{
	IKernel * pKernel = ILuaExtModule::GetKernel(state);

	// 检查参数数量
	CHECK_ARG_NUM(state, nx_get_script, 1);
	// 检查参数类型
	CHECK_ARG_OBJECT(state, nx_get_script, 1);
	PERSISTID obj = pKernel->LuaToObject(state, 1);

	pKernel->LuaPushString(state, pKernel->GetScript(obj));
	return 1;
}

// 原型：bool nx_exists(object obj)
// 功能：对象是否存在
int nx_exists(void * state)
{
	IKernel * pKernel = ILuaExtModule::GetKernel(state);

	// 检查参数数量
	CHECK_ARG_NUM(state, nx_exists, 1);
	// 检查参数类型
	CHECK_ARG_OBJECT(state, nx_exists, 1);
	PERSISTID obj = pKernel->LuaToObject(state, 1);

	pKernel->LuaPushBool(state, pKernel->Exists(obj));
	return 1;
}

// 原型：object nx_parent(object obj)
// 功能：取父对象
int nx_parent(void * state)
{
	IKernel * pKernel = ILuaExtModule::GetKernel(state);

	// 检查参数数量
	CHECK_ARG_NUM(state, nx_parent, 1);
	// 检查参数类型
	CHECK_ARG_OBJECT(state, nx_parent, 1);
	PERSISTID obj = pKernel->LuaToObject(state, 1);

	pKernel->LuaPushObject(state, pKernel->Parent(obj));
	return 1;
}

// 原型：int nx_get_capacity(object obj)
// 功能：返回容器的容量
int nx_get_capacity(void * state)
{
	IKernel * pKernel = ILuaExtModule::GetKernel(state);

	// 检查参数数量
	CHECK_ARG_NUM(state, nx_get_capacity, 1);
	// 检查参数类型
	CHECK_ARG_OBJECT(state, nx_get_capacity, 1);

	PERSISTID obj = pKernel->LuaToObject(state, 1);

	pKernel->LuaPushInt(state, pKernel->GetCapacity(obj));
	return 1;
}

// 原型：int nx_get_child_count(object obj)
// 功能：返回子对象数量
int nx_get_child_count(void * state)
{
	IKernel * pKernel = ILuaExtModule::GetKernel(state);

	// 检查参数数量
	CHECK_ARG_NUM(state, nx_get_child_count, 1);
	// 检查参数类型
	CHECK_ARG_OBJECT(state, nx_get_child_count, 1);

	PERSISTID obj = pKernel->LuaToObject(state, 1);

	pKernel->LuaPushInt(state, pKernel->GetChildCount(obj));
	return 1;
}

// 原型：object nx_get_item(object obj, int pos)
// 功能：取容器指定位置的子对象（pos从1开始）
int nx_get_item(void * state)
{
	IKernel * pKernel = ILuaExtModule::GetKernel(state);

	// 检查参数数量
	CHECK_ARG_NUM(state, nx_get_item, 2);
	// 检查参数类型
	CHECK_ARG_OBJECT(state, nx_get_item, 1);
	CHECK_ARG_INT(state, nx_get_item, 2);

	PERSISTID obj = pKernel->LuaToObject(state, 1);
	int pos = pKernel->LuaToInt(state, 2);

	pKernel->LuaPushObject(state, pKernel->GetItem(obj, pos));
	return 1;
}

// 原型：int nx_get_index(object obj)
// 功能：取对象在容器中的位置
int nx_get_index(void * state)
{
	IKernel * pKernel = ILuaExtModule::GetKernel(state);

	// 检查参数数量
	CHECK_ARG_NUM(state, nx_get_index, 1);
	// 检查参数类型
	CHECK_ARG_OBJECT(state, nx_get_index, 1);

	PERSISTID obj = pKernel->LuaToObject(state, 1);

	pKernel->LuaPushInt(state, pKernel->GetIndex(obj));
	return 1;
}

// 原型：object nx_get_child(object obj, wstring name)
// 功能：用名字查找子对象
int nx_get_child(void * state)
{
	IKernel * pKernel = ILuaExtModule::GetKernel(state);

	// 检查参数数量
	CHECK_ARG_NUM(state, nx_get_child, 2);
	// 检查参数类型
	CHECK_ARG_OBJECT(state, nx_get_child, 1);
	CHECK_ARG_WIDESTR(state, nx_get_child, 2);

	PERSISTID obj = pKernel->LuaToObject(state, 1);
	const wchar_t * name = pKernel->LuaToWideStr(state, 2);

	pKernel->LuaPushObject(state, pKernel->GetChild(obj, name));
	return 1;
}

// 原型：table nx_get_child_list(object obj, int classtype)
// 功能：获得子对象列表
int nx_get_child_list(void * state)
{
	IKernel * pKernel = ILuaExtModule::GetKernel(state);

	// 检查参数数量
	CHECK_ARG_NUM(state, nx_get_child_list, 2);
	// 检查参数类型
	CHECK_ARG_OBJECT(state, nx_get_child_list, 1);
	CHECK_ARG_INT(state, nx_get_child_list, 2);

	PERSISTID obj = pKernel->LuaToObject(state, 1);
	int classtype = pKernel->LuaToInt(state, 2);

	CVarList res;
	pKernel->GetChildList(obj, classtype, res);
	pKernel->LuaPushTable(state, res);
	return 1;
}

// 原型：table nx_find_child_list_byconfig(object obj, fast_string config,int classtype)
// 功能：获得子对象列表
int nx_find_child_list_byconfig(void * state)
{
	IKernel * pKernel = ILuaExtModule::GetKernel(state);

	// 检查参数数量
	CHECK_ARG_NUM(state, nx_find_child_list_byconfig, 3);
	// 检查参数类型
	CHECK_ARG_OBJECT(state, nx_find_child_list_byconfig, 1);
	CHECK_ARG_STRING(state, nx_find_child_list_byconfig, 2);
	CHECK_ARG_INT(state, nx_find_child_list_byconfig, 3);

	PERSISTID obj = pKernel->LuaToObject(state, 1);
	const char* strConfigID = pKernel->LuaToString(state,2);
	int classtype = pKernel->LuaToInt(state, 3);

	CVarList res;
	pKernel->FindChildMoreByConfig(obj, strConfigID,classtype, res);
	pKernel->LuaPushTable(state, res);
	return 1;
}

// 原型：object nx_find_child_byconfig(object obj, wstring name)
// 功能：用名字查找子对象
int nx_find_child_byconfig(void * state)
{
	IKernel * pKernel = ILuaExtModule::GetKernel(state);

	// 检查参数数量
	CHECK_ARG_NUM(state, nx_find_child_byconfig, 3);
	// 检查参数类型
	CHECK_ARG_OBJECT(state, nx_find_child_byconfig, 1);
	CHECK_ARG_STRING(state, nx_find_child_byconfig, 2);
	CHECK_ARG_INT(state, nx_find_child_byconfig, 3);

	PERSISTID obj = pKernel->LuaToObject(state, 1);
	const char * config = pKernel->LuaToString(state, 2);
	int classtype = pKernel->LuaToInt(state, 3);

	pKernel->LuaPushObject(state, pKernel->FindChildByConfig(obj, config, classtype));
	return 1;
}


// 原型：table nx_get_around_list(object obj, float radius, int classtype, int max, int binvisible)
// 功能：获得指定对象周围的对象列表
int nx_get_around_list(void * state)
{
	IKernel * pKernel = ILuaExtModule::GetKernel(state);

	// 检查参数数量
	CHECK_ARG_NUM(state, nx_get_around_list, 5);
	// 检查参数类型
	CHECK_ARG_OBJECT(state, nx_get_around_list, 1);
	CHECK_ARG_FLOAT(state, nx_get_around_list, 2);
	CHECK_ARG_INT(state, nx_get_around_list, 3);
	CHECK_ARG_INT(state, nx_get_around_list, 4);
	CHECK_ARG_INT(state, nx_get_around_list, 5);

	PERSISTID obj = pKernel->LuaToObject(state, 1);
	float radius = pKernel->LuaToFloat(state, 2);
	int classtype = pKernel->LuaToInt(state, 3);
	int max = pKernel->LuaToInt(state, 4);
	bool binvisible = pKernel->LuaToInt(state, 5) == 1;

	CVarList res;
	pKernel->GetAroundList(obj, radius, classtype, max, res, binvisible);
	pKernel->LuaPushTable(state, res);
	return 1;
}

// 原型：table nx_get_point_list(object obj, float x, float z, float radius, int classtype, int max, int binvisible)
// 功能：获得指定对象周围的对象列表
int nx_get_point_list(void * state)
{
	IKernel * pKernel = ILuaExtModule::GetKernel(state);

	// 检查参数数量
	CHECK_ARG_NUM(state, nx_get_point_list, 7);
	// 检查参数类型
	CHECK_ARG_OBJECT(state, nx_get_point_list, 1);
	CHECK_ARG_FLOAT(state, nx_get_point_list, 2);
	CHECK_ARG_FLOAT(state, nx_get_point_list, 3);
	CHECK_ARG_FLOAT(state, nx_get_point_list, 4);
	CHECK_ARG_INT(state, nx_get_point_list, 5);
	CHECK_ARG_INT(state, nx_get_point_list, 6);
	CHECK_ARG_INT(state, nx_get_point_list, 7);

	PERSISTID obj = pKernel->LuaToObject(state, 1);
	float x = pKernel->LuaToFloat(state, 2);
	float z = pKernel->LuaToFloat(state, 3);
	float radius = pKernel->LuaToFloat(state, 4);
	int classtype = pKernel->LuaToInt(state, 5);
	int max = pKernel->LuaToInt(state, 6);
	bool binvisible = pKernel->LuaToInt(state, 7) == 1;

	CVarList res;
	pKernel->GetPointAroundList(obj, x, z, radius, classtype, max, res, binvisible);
	pKernel->LuaPushTable(state, res);
	return 1;
}

// 原型：int nx_new_gourpid()
// 功能：获得唯一的组号
int nx_new_gourpid(void * state)
{
	IKernel * pKernel = ILuaExtModule::GetKernel(state);

	pKernel->LuaPushInt(state, pKernel->NewGroupId());
	return 1;
}

// 原型：table nx_group_child_list(int groupid, int classtype)
// 功能：获得指定组的子对象列表
int nx_group_child_list(void * state)
{
	IKernel * pKernel = ILuaExtModule::GetKernel(state);

	// 检查参数数量
	CHECK_ARG_NUM(state, nx_group_child_list, 2);
	// 检查参数类型
	CHECK_ARG_INT(state, nx_group_child_list, 1);
	CHECK_ARG_INT(state, nx_group_child_list, 2);

	int groupid = pKernel->LuaToInt(state, 1);
	int classtype = pKernel->LuaToInt(state, 2);

	CVarList res;
	pKernel->GroupChildList(groupid, classtype, res);
	pKernel->LuaPushTable(state, res);
	return 1;
}

// 原型：bool nx_exchange(object container1, int pos1, object container2, int pos2)
// 功能：将对象移动另一个容器中
int nx_exchange(void * state)
{
	IKernel * pKernel = ILuaExtModule::GetKernel(state);

	CHECK_ARG_NUM(state, nx_exchange, 4);
	// 检查参数类型
	CHECK_ARG_OBJECT(state, nx_exchange, 1);
	CHECK_ARG_INT(state, nx_exchange, 2);
	CHECK_ARG_OBJECT(state, nx_exchange, 3);
	CHECK_ARG_INT(state, nx_exchange, 4);

	PERSISTID container1 = pKernel->LuaToObject(state, 1);
	int pos1 = pKernel->LuaToInt(state, 2);
	PERSISTID container2 = pKernel->LuaToObject(state, 3);
	int pos2 = pKernel->LuaToInt(state, 4);

	pKernel->LuaPushBool(state, pKernel->Exchange(container1, pos1, container2, pos2));
	return 1;
}

// 原型：bool nx_speech(object obj, fast_string fast_stringid, ...)
// 功能：发出聊天信息
int nx_speech(void * state)
{
	IKernel * pKernel = ILuaExtModule::GetKernel(state);

	// 检查参数数量
	const int MIN_ARGS = 2;

	int arg_num = pKernel->LuaGetArgCount(state);
	if (arg_num < MIN_ARGS)
	{
		pKernel->LuaErrorHandler(state, "nx_speech argument number less than 3");
		return 1;
	}

	// 检查参数类型
	CHECK_ARG_OBJECT(state, nx_speech, 1);
	CHECK_ARG_STRING(state, nx_speech, 2);

	// 获取参数
	PERSISTID obj = pKernel->LuaToObject(state, 1);
	const char *info = pKernel->LuaToString(state, 2);
	if (StringUtil::CharIsNull(info))
	{
		return 1;
	}

	CVarList msg;
	if (arg_num > MIN_ARGS)
		ILuaExtModule::GetVarList(state, MIN_ARGS + 1, arg_num, msg);

	pKernel->LuaPushBool(state, ::CustomSpeech(pKernel, obj, info, msg));
	return 1;
}

// 原型：bool nx_add_heartbeat(object obj, fast_string func, int time)
// 功能：添加心跳函数
int nx_add_heartbeat(void * state)
{
	IKernel * pKernel = ILuaExtModule::GetKernel(state);

	CHECK_ARG_NUM(state, nx_add_heartbeat, 3);
	// 检查参数类型
	CHECK_ARG_OBJECT(state, nx_add_heartbeat, 1);
	CHECK_ARG_STRING(state, nx_add_heartbeat, 2);
	CHECK_ARG_INT(state, nx_add_heartbeat, 3);

	PERSISTID obj = pKernel->LuaToObject(state, 1);
	const char * func = pKernel->LuaToString(state, 2);
	int time = pKernel->LuaToInt(state, 3);

	pKernel->LuaPushBool(state, pKernel->AddHeartBeat(obj, func, time));
	return 1;
}

// 原型：bool nx_add_countbeat(object obj, fast_string func, int time, int count)
// 功能：添加心跳函数
int nx_add_countbeat(void * state)
{
	IKernel * pKernel = ILuaExtModule::GetKernel(state);

	CHECK_ARG_NUM(state, nx_add_countbeat, 4);
	// 检查参数类型
	CHECK_ARG_OBJECT(state, nx_add_countbeat, 1);
	CHECK_ARG_STRING(state, nx_add_countbeat, 2);
	CHECK_ARG_INT(state, nx_add_countbeat, 3);
	CHECK_ARG_INT(state, nx_add_countbeat, 4);

	PERSISTID obj = pKernel->LuaToObject(state, 1);
	const char * func = pKernel->LuaToString(state, 2);
	int time = pKernel->LuaToInt(state, 3);
	int count = pKernel->LuaToInt(state, 4);

	pKernel->LuaPushBool(state, pKernel->AddCountBeat(obj, func, time, count));
	return 1;
}

// 原型：bool nx_remove_heartbeat(object obj, fast_string func)
// 功能：添加心跳函数
int nx_remove_heartbeat(void * state)
{
	IKernel * pKernel = ILuaExtModule::GetKernel(state);

	CHECK_ARG_NUM(state, nx_remove_heartbeat, 2);
	// 检查参数类型
	CHECK_ARG_OBJECT(state, nx_remove_heartbeat, 1);
	CHECK_ARG_STRING(state, nx_remove_heartbeat, 2);

	PERSISTID obj = pKernel->LuaToObject(state, 1);
	const char * func = pKernel->LuaToString(state, 2);

	pKernel->LuaPushBool(state, pKernel->RemoveHeartBeat(obj, func));
	return 1;
}

// 原型：bool nx_clear_heartbeat(object obj)
// 功能：清除心跳函数
int nx_clear_heartbeat(void * state)
{
	IKernel * pKernel = ILuaExtModule::GetKernel(state);

	CHECK_ARG_NUM(state, nx_clear_heartbeat, 1);
	// 检查参数类型
	CHECK_ARG_OBJECT(state, nx_clear_heartbeat, 1);

	PERSISTID obj = pKernel->LuaToObject(state, 1);

	pKernel->LuaPushBool(state, pKernel->ClearHeartBeat(obj));
	return 1;
}

// 原型：bool nx_find_heartbeat(object obj, fast_string func)
// 功能：添加心跳函数
int nx_find_heartbeat(void * state)
{
	IKernel * pKernel = ILuaExtModule::GetKernel(state);

	CHECK_ARG_NUM(state, nx_find_heartbeat, 2);
	// 检查参数类型
	CHECK_ARG_OBJECT(state, nx_find_heartbeat, 1);
	CHECK_ARG_STRING(state, nx_find_heartbeat, 2);

	PERSISTID obj = pKernel->LuaToObject(state, 1);
	const char * func = pKernel->LuaToString(state, 2);

	pKernel->LuaPushBool(state, pKernel->FindHeartBeat(obj, func));
	return 1;
}

// 原型：int nx_get_beattime(object obj, fast_string func)
// 功能：查询心跳频率（毫秒）
int nx_get_beattime(void * state)
{
	IKernel * pKernel = ILuaExtModule::GetKernel(state);

	CHECK_ARG_NUM(state, nx_get_beattime, 2);
	// 检查参数类型
	CHECK_ARG_OBJECT(state, nx_get_beattime, 1);
	CHECK_ARG_STRING(state, nx_get_beattime, 2);

	PERSISTID obj = pKernel->LuaToObject(state, 1);
	const char * func = pKernel->LuaToString(state, 2);

	pKernel->LuaPushInt(state, pKernel->GetBeatTime(obj, func));
	return 1;
}

// 原型：bool nx_add_viewport(object opplayer, int viewid, object container)
// 功能：给玩家添加容器视窗
int nx_add_viewport(void * state)
{
	IKernel * pKernel = ILuaExtModule::GetKernel(state);

	CHECK_ARG_NUM(state, nx_add_viewport, 3);
	// 检查参数类型
	CHECK_ARG_OBJECT(state, nx_add_viewport, 1);
	CHECK_ARG_INT(state, nx_add_viewport, 2);
	CHECK_ARG_OBJECT(state, nx_add_viewport, 3);

	PERSISTID opplayer = pKernel->LuaToObject(state, 1);
	int viewid = pKernel->LuaToInt(state, 2);
	PERSISTID container = pKernel->LuaToObject(state, 3);

	if (pKernel->Exists(container))
	{
		pKernel->LuaPushBool(state, pKernel->AddViewport(opplayer, viewid, container));
	}
	else
	{
		char szBuffer[256] = "[Error]nx_add_viewport: container not Exists view_id = %d";
		SPRINTF_S(szBuffer, szBuffer, viewid);
		::extend_warning(pKernel,  szBuffer);

		pKernel->LuaPushBool(state, false);
	}
	return 1;
}

// 原型：bool nx_remove_viewport(object opplayer, int viewid)
// 功能：玩家删除容器视窗
int nx_remove_viewport(void * state)
{
	IKernel * pKernel = ILuaExtModule::GetKernel(state);

	CHECK_ARG_NUM(state, nx_add_viewport, 2);
	// 检查参数类型
	CHECK_ARG_OBJECT(state, nx_remove_viewport, 1);
	CHECK_ARG_INT(state, nx_remove_viewport, 2);

	PERSISTID opplayer = pKernel->LuaToObject(state, 1);
	int viewid = pKernel->LuaToInt(state, 2);

	pKernel->LuaPushBool(state, pKernel->RemoveViewport(opplayer, viewid));
	return 1;
}

// 原型：bool nx_find_viewport(object opplayer, int viewid)
// 功能：玩家的视窗是否存在
int nx_find_viewport(void * state)
{
	IKernel * pKernel = ILuaExtModule::GetKernel(state);

	CHECK_ARG_NUM(state, nx_find_viewport, 2);
	// 检查参数类型
	CHECK_ARG_OBJECT(state, nx_find_viewport, 1);
	CHECK_ARG_INT(state, nx_find_viewport, 2);

	PERSISTID opplayer = pKernel->LuaToObject(state, 1);
	int viewid = pKernel->LuaToInt(state, 2);

	pKernel->LuaPushBool(state, pKernel->FindViewport(opplayer, viewid));
	return 1;
}

// 原型：object nx_get_viewport_container(object opplayer, int viewid)
// 功能：获得玩家视窗对应的容器
int nx_get_viewport_container(void * state)
{
	IKernel * pKernel = ILuaExtModule::GetKernel(state);

	CHECK_ARG_NUM(state, nx_get_viewport_container, 2);
	// 检查参数类型
	CHECK_ARG_OBJECT(state, nx_get_viewport_container, 1);
	CHECK_ARG_INT(state, nx_get_viewport_container, 2);

	PERSISTID opplayer = pKernel->LuaToObject(state, 1);
	int viewid = pKernel->LuaToInt(state, 2);

	pKernel->LuaPushObject(state, pKernel->GetViewportContainer(opplayer, viewid));
	return 1;
}

// 原型：bool nx_clear_viewport(object opplayer)
// 功能：清除玩家所有视窗
int nx_clear_viewport(void * state)
{
	IKernel * pKernel = ILuaExtModule::GetKernel(state);

	CHECK_ARG_NUM(state, nx_clear_viewport, 1);
	// 检查参数类型
	CHECK_ARG_OBJECT(state, nx_clear_viewport, 1);

	PERSISTID opplayer = pKernel->LuaToObject(state, 1);

	pKernel->LuaPushBool(state, pKernel->ClearViewport(opplayer));
	return 1;
}

// 原型：int nx_get_viewers(object container)
// 功能：取容器的视窗数
int nx_get_viewers(void * state)
{
	IKernel * pKernel = ILuaExtModule::GetKernel(state);

	CHECK_ARG_NUM(state, nx_get_viewers, 1);
	// 检查参数类型
	CHECK_ARG_OBJECT(state, nx_get_viewers, 1);

	PERSISTID container = pKernel->LuaToObject(state, 1);

	pKernel->LuaPushInt(state, pKernel->GetViewers(container));
	return 1;
}

// 原型：bool nx_close_viewers(object container)
// 功能：关闭容器的所有视窗
int nx_close_viewers(void * state)
{
	IKernel * pKernel = ILuaExtModule::GetKernel(state);

	CHECK_ARG_NUM(state, nx_close_viewers, 1);
	// 检查参数类型
	CHECK_ARG_OBJECT(state, nx_close_viewers, 1);

	PERSISTID container = pKernel->LuaToObject(state, 1);

	pKernel->LuaPushBool(state, pKernel->CloseViewers(container));
	return 1;
}

// 原型：bool nx_set_landpoint(object obj, fast_string point)
// 功能：设置登陆点名
int nx_set_landpoint(void * state)
{
	IKernel * pKernel = ILuaExtModule::GetKernel(state);

	CHECK_ARG_NUM(state, nx_set_landpoint, 2);
	// 检查参数类型
	CHECK_ARG_OBJECT(state, nx_set_landpoint, 1);
	CHECK_ARG_STRING(state, nx_set_landpoint, 2);

	PERSISTID obj = pKernel->LuaToObject(state, 1);
	const char * point = pKernel->LuaToString(state, 2);

	pKernel->LuaPushBool(state, pKernel->SetLandPoint(obj, point));
	return 1;
}

// 原型：bool nx_set_landposition(object obj, int sceneid, float x, float y, float z, float orient)
// 功能：设置登陆位置
int nx_set_landposition(void * state)
{
	IKernel * pKernel = ILuaExtModule::GetKernel(state);

	CHECK_ARG_NUM(state, nx_set_landposition, 6);
	// 检查参数类型
	CHECK_ARG_OBJECT(state, nx_set_landposition, 1);
	CHECK_ARG_INT(state, nx_set_landposition, 2);
	CHECK_ARG_FLOAT(state, nx_set_landposition, 3);
	CHECK_ARG_FLOAT(state, nx_set_landposition, 4);
	CHECK_ARG_FLOAT(state, nx_set_landposition, 5);
	CHECK_ARG_FLOAT(state, nx_set_landposition, 6);

	PERSISTID obj = pKernel->LuaToObject(state, 1);
	int sceneid = pKernel->LuaToInt(state, 2);
	float x = pKernel->LuaToFloat(state, 3);
	float y = pKernel->LuaToFloat(state, 4);
	float z = pKernel->LuaToFloat(state, 5);
	float orient = pKernel->LuaToFloat(state, 6);

	pKernel->LuaPushBool(state, pKernel->SetLandPosi(obj, sceneid, x, y, z, orient));
	return 1;
}

// 原型：bool nx_break_player(object obj)
// 功能：强制玩家下线
int nx_break_player(void * state)
{
	IKernel * pKernel = ILuaExtModule::GetKernel(state);

	CHECK_ARG_NUM(state, nx_break_player, 1);
	// 检查参数类型
	CHECK_ARG_OBJECT(state, nx_break_player, 1);

	PERSISTID obj = pKernel->LuaToObject(state, 1);

	//pKernel->LuaPushBool(state, PlayerBaseModule::BreakPlayer(pKernel, obj));
	return 1;
}

#ifndef FSROOMLOGIC_EXPORTS
// 原型：bool nx_break_account(string account)
// 功能：强制指定帐号的玩家下线
int nx_break_account(void * state)
{
	IKernel * pKernel = ILuaExtModule::GetKernel(state);

	CHECK_ARG_NUM(state, nx_break_account, 1);
	// 检查参数类型
	CHECK_ARG_STRING(state, nx_break_account, 1);

	const char * account = pKernel->LuaToString(state, 1);

	pKernel->LuaPushBool(state, pKernel->BreakByAccount(account));
	return 1;
}

// 原型：bool nx_break_byname(wstring name)
// 功能：强制指定名称的玩家下线
int nx_break_byname(void * state)
{
	IKernel * pKernel = ILuaExtModule::GetKernel(state);

	CHECK_ARG_NUM(state, nx_break_byname, 1);
	// 检查参数类型
	CHECK_ARG_WIDESTR(state, nx_break_byname, 1);

	const wchar_t * name = pKernel->LuaToWideStr(state, 1);

	// 发消息立刻踢下线 注意 直接用引擎的接口breakplayer会引起断线续连, 玩家会重新被连接上
	//pKernel->CommandByName(name, CVarList()<<COMMAND_BREAK_PLAYER);

	pKernel->LuaPushBool(state, true);

	return 1;
}

// 原型：bool nx_block_player(wstring name, int year, int month, int day, int hour, int min, int sec)
// 功能：禁止角色上线直到指定的时间为止
int nx_block_player(void * state)
{
	IKernel * pKernel = ILuaExtModule::GetKernel(state);

	CHECK_ARG_NUM(state, nx_block_player, 8);
	// 检查参数类型
	CHECK_ARG_WIDESTR(state, nx_block_player, 2);
	CHECK_ARG_INT(state, nx_block_player, 3);
	CHECK_ARG_INT(state, nx_block_player, 4);
	CHECK_ARG_INT(state, nx_block_player, 5);
	CHECK_ARG_INT(state, nx_block_player, 6);
	CHECK_ARG_INT(state, nx_block_player, 7);
	CHECK_ARG_INT(state, nx_block_player, 8);

	const wchar_t * name = pKernel->LuaToWideStr(state, 2);
	int year = pKernel->LuaToInt(state, 3);
	int month = pKernel->LuaToInt(state, 4);
	int day = pKernel->LuaToInt(state, 5);
	int hour = pKernel->LuaToInt(state, 6);
	int min = pKernel->LuaToInt(state, 7);
	int sec = pKernel->LuaToInt(state, 8);

	pKernel->LuaPushBool(state, pKernel->BlockPlayer(name, year, month, day, hour, min, sec));

	return 1;
}


// 原型：bool nx_block_player_minite(wstring name, int min)
// 功能：禁止角色上线直到指定的时间为止
int nx_block_player_minite(void * state)
{
	IKernel * pKernel = ILuaExtModule::GetKernel(state);

	CHECK_ARG_NUM(state, nx_block_player_minite, 3);
	// 检查参数类型
	CHECK_ARG_WIDESTR(state, nx_block_player_minite, 2);
	CHECK_ARG_INT(state, nx_block_player_minite, 3);

	const wchar_t * name = pKernel->LuaToWideStr(state, 2);
	int min = pKernel->LuaToInt(state, 3);

	time_t now = time(NULL);
	now += min*60;

	tm* t = localtime(&now);

	pKernel->LuaPushBool(state, pKernel->BlockPlayer(name, t->tm_year+1900, t->tm_mon+1, 
		t->tm_mday, t->tm_hour, t->tm_min, t->tm_sec));

	return 1;
}
#endif // FSROOMLOGIC_EXPORTS

// 原型：object nx_find_player(wstring name)
// 功能：在当前场景内查找玩家
int nx_find_player(void * state)
{
	IKernel * pKernel = ILuaExtModule::GetKernel(state);

	CHECK_ARG_NUM(state, nx_find_player, 1);
	// 检查参数类型
	CHECK_ARG_WIDESTR(state, nx_find_player, 1);

	const wchar_t * name = pKernel->LuaToWideStr(state, 1);

	pKernel->LuaPushObject(state, pKernel->FindPlayer(name));
	return 1;
}

// 原型：int nx_get_player_scene(wstring name)
// 功能：取玩家所在场景号
int nx_get_player_scene(void * state)
{
	IKernel * pKernel = ILuaExtModule::GetKernel(state);

	CHECK_ARG_NUM(state, nx_get_player_scene, 1);
	// 检查参数类型
	CHECK_ARG_WIDESTR(state, nx_get_player_scene, 1);

	const wchar_t * name = pKernel->LuaToWideStr(state, 1);

	pKernel->LuaPushInt(state, pKernel->GetPlayerScene(name));
	return 1;
}

// 原型：int nx_get_player_count()
// 功能：取游戏在线玩家数量
int nx_get_player_count(void * state)
{
	IKernel * pKernel = ILuaExtModule::GetKernel(state);

	pKernel->LuaPushInt(state, pKernel->GetPlayerCount());
	return 1;
}

#ifndef FSROOMLOGIC_EXPORTS
// 原型：int nx_get_online_count()
// 功能：取游戏在线玩家数量 参数为场景id, 如果<=0则取全服务器的在线数量
int nx_get_online_count(void * state)
{
	IKernel * pKernel = ILuaExtModule::GetKernel(state);
	CHECK_ARG_NUM(state, nx_get_online_count, 1);
	CHECK_ARG_INT(state, nx_get_online_count, 1);

	int scid = pKernel->LuaToInt(state, 1);

	int count = 0;
	if (scid <= 0)
	{
		count = pKernel->GetOnlineCount();
	}
	else
	{
		count = pKernel->GetSceneOnlineCount(scid);
	}

	pKernel->LuaPushInt(state, count);
	return 1;
}

// 原型：int nx_get_offline_count()
// 功能：取游戏离线玩家数量
int nx_get_offline_count(void * state)
{
	IKernel * pKernel = ILuaExtModule::GetKernel(state);

	pKernel->LuaPushInt(state, pKernel->GetOfflineCount());
	return 1;
}

// 原型：bool nx_send_public_message(...);
// 功能：发送消息到公共数据服务器
int nx_send_public_message(void * state)
{
	IKernel * pKernel = ILuaExtModule::GetKernel(state);

	int arg_num = pKernel->LuaGetArgCount(state);

	CVarList msg;
	ILuaExtModule::GetVarList(state, 1, arg_num, msg);

	pKernel->LuaPushBool(state, pKernel->SendPublicMessage(msg));
	return 1;
}
#endif // FSROOMLOGIC_EXPORTS

// 原型：table nx_split_string(string info, fast_string delims);
// 功能：分割字符串(split_info:根据哪些字符来进行分割，可以同时指定多个字符)
int nx_split_string(void * state)
{
	IKernel * pKernel = ILuaExtModule::GetKernel(state);

	// 检查参数数量
	CHECK_ARG_NUM(state, nx_split_string, 2);

	// 检查参数类型
	CHECK_ARG_STRING(state, nx_split_string, 1);
	CHECK_ARG_STRING(state, nx_split_string, 2);

	const char * info = pKernel->LuaToString(state, 1);
	const char * delims = pKernel->LuaToString(state, 2);

	// 获取参数
	CVarList res;
	util_split_string(res, info, delims);

	pKernel->LuaPushTable(state, res);
	return 1;
}

// 原型：table nx_split_wstring(wstring info, wstring delims);
// 功能：分割字符串(split_info:根据哪些字符来进行分割，可以同时指定多个字符)
int nx_split_wstring(void * state)
{
	IKernel * pKernel = ILuaExtModule::GetKernel(state);

	// 检查参数数量
	CHECK_ARG_NUM(state, nx_split_wstring, 2);

	// 检查参数类型
	CHECK_ARG_WIDESTR(state, nx_split_wstring, 1);
	CHECK_ARG_WIDESTR(state, nx_split_wstring, 2);

	const wchar_t * info = pKernel->LuaToWideStr(state, 1);
	const wchar_t * delims = pKernel->LuaToWideStr(state, 2);

	// 获取参数
	CVarList res;
	util_split_wstring(res, info, delims);

	pKernel->LuaPushTable(state, res);
	return 1;
}

// 原型：int nx_get_ticks();
// 功能：
int nx_get_ticks(void * state)
{
	IKernel * pKernel = ILuaExtModule::GetKernel(state);

	pKernel->LuaPushInt64(state, ::util_get_ticks());
	return 1;
}

// 原型：table nx_execute(string lua_file, fast_string lua_func, ...)
// 功能：执行某个脚本的某个函数
int nx_execute(void * state)
{
	IKernel * pKernel = ILuaExtModule::GetKernel(state);

	const int MIN_ARGS = 2;
	int arg_num = pKernel->LuaGetArgCount(state);
	if (arg_num < MIN_ARGS)
	{
		pKernel->LuaErrorHandler(state, "nx_execute argument number less than 2");
		pKernel->LuaPushBool(state, false);
		return 1;
	}

	// 检查参数类型
	CHECK_ARG_STRING(state, nx_execute, 1);
	CHECK_ARG_STRING(state, nx_execute, 2);

	const char * script = pKernel->LuaToString(state, 1);
	const char * func = pKernel->LuaToString(state, 2);

	CVarList args;
	if (ILuaExtModule::GetVarList(state, MIN_ARGS + 1, arg_num, args))
	{
		CVarList res;
		if (::RunLua(pKernel, script, func, args, -1, &res))
		{
			pKernel->LuaPushTable(state, res);
			return 1;
		}
	}

	return 0;
}

// 原型：bool nx_reload_preload_config(string ini_file);
// 功能：重新加载配置文件
int nx_reload_preload_config(void * state)
{
	IKernel * pKernel = ILuaExtModule::GetKernel(state);

	// 检查参数数量
	CHECK_ARG_NUM(state, nx_reload_preload_config, 1);

	// 检查参数类型
	CHECK_ARG_STRING(state, nx_reload_preload_config, 1);

	const char * file_name = pKernel->LuaToString(state, 1);

	bool bRet = pKernel->PreloadConfig(file_name);

	pKernel->LuaPushBool(state, bRet);	

	return 1;
}

// 原型：int nx_get_year_month_day
// 功能：获得系统年月日
int nx_get_year_month_day(void * state)
{
	IKernel * pKernel = ILuaExtModule::GetKernel(state);
	//取年月日
	int nYear  = 0;
	int nMonth = 0;
	int nDate  = 0;
	util_get_year_month_day(nYear, nMonth, nDate);
	CVarList res;
	res<<nYear<<nMonth<<nDate;
	pKernel->LuaPushTable(state, res);
	
	return 1;
}

// 原型：int nx_get_day_of_week
// 功能：获得周日
int nx_get_day_of_week(void * state)
{
	IKernel * pKernel = ILuaExtModule::GetKernel(state);
	int nWeekDay = ::util_get_day_of_week();
	pKernel->LuaPushInt(state, nWeekDay);
	return 1;
}

// 原型：int nx_get_hour_minute_second
// 功能：获得系统时分秒
int nx_get_hour_minute_second(void * state)
{
	IKernel * pKernel = ILuaExtModule::GetKernel(state);
	//取时分秒
	int nHour = 0;
	int nMinute = 0;
	int nSec  = 0;
	::util_get_hour_minute_second(nHour, nMinute, nSec);
	CVarList res;
	res<<nHour<<nMinute<<nSec;
	pKernel->LuaPushTable(state, res);
	return 1;
}

//原型：nx_util_get_time_64
// 功能：获得64位系统时间
int nx_util_get_time_64(void * state)
{
	IKernel * pKernel = ILuaExtModule::GetKernel(state);
	__int64 time = ::util_get_time_64();
	CVarList res;
	pKernel->LuaPushInt64(state, time);
	return 1;
}

// 原型：int64 nx_int64_inc_int64(int64, int64)  
// 功能：两个64int相加
int nx_int64_inc_int64(void* state)
{
	IKernel * pKernel = ILuaExtModule::GetKernel(state);
    CHECK_ARG_NUM(state, nx_int64_inc_int64, 2);
    // 检查参数类型
    CHECK_ARG_INT64(state, nx_int64_inc_int64, 1);
    CHECK_ARG_INT64(state, nx_int64_inc_int64, 2);

    __int64 left = pKernel->LuaToInt64(state, 1);
    __int64 right= pKernel->LuaToInt64(state, 2);

    __int64 total = left + right;

    pKernel->LuaPushInt64(state, total);
    return 1;
}

// 原型：int64 nx_int64_dec_int64(int64, int64)  
// 功能：两个64int相减
int nx_int64_dec_int64(void* state)
{
    IKernel * pKernel = ILuaExtModule::GetKernel(state);
    CHECK_ARG_NUM(state, nx_int64_dec_int64, 2);
    // 检查参数类型
    CHECK_ARG_INT64(state, nx_int64_dec_int64, 1);
    CHECK_ARG_INT64(state, nx_int64_dec_int64, 2);

    __int64 left = pKernel->LuaToInt64(state, 1);
    __int64 right= pKernel->LuaToInt64(state, 2);

    __int64 total = left - right;

    pKernel->LuaPushInt64(state, total);
    return 1;
}

// 原形：bool nx_int64_compare_int64(int64, int64)
// 功能：两个64int是否相等
int nx_int64_compare_int64(void* state)
{
    IKernel * pKernel = ILuaExtModule::GetKernel(state);
    CHECK_ARG_NUM(state, nx_int64_compare_int64, 2);
    // 检查参数类型
    CHECK_ARG_INT64(state, nx_int64_compare_int64, 1);
    CHECK_ARG_INT64(state, nx_int64_compare_int64, 2);

    __int64 left = pKernel->LuaToInt64(state, 1);
    __int64 right= pKernel->LuaToInt64(state, 2);

    bool res = false;
    if (left == right)
    {
        res = true;
    }

    pKernel->LuaPushBool(state, res);
    return 1;
}

// 原形：bool nx_int64_large_int64(int64, int64)
// 功能：左64int是否大于右64int
int nx_int64_large_int64(void* state)
{
    IKernel * pKernel = ILuaExtModule::GetKernel(state);
    CHECK_ARG_NUM(state, nx_int64_large_int64, 2);
    // 检查参数类型
    CHECK_ARG_INT64(state, nx_int64_large_int64, 1);
    CHECK_ARG_INT64(state, nx_int64_large_int64, 2);

    __int64 left = pKernel->LuaToInt64(state, 1);
    __int64 right= pKernel->LuaToInt64(state, 2);

    bool res = false;
    if (left > right)
    {
        res = true;
    }

    pKernel->LuaPushBool(state, res);
    return 1;
}

inline int GetQuadrant(len_t orient)
{
	len_t pi = 3.14f;
	len_t orient_temp = orient - orient * (orient / (2 * pi));

	if (orient_temp < pi / 2) return 1;
	else if(orient_temp < pi) return 2;
	else if(orient_temp < pi * 3 / 2) return 3;
	else return 4;
}

// 原形：bool nx_face_or_back(object, target)
// 功能：object是否面对着target
int nx_face_or_back(void* state)
{
    IKernel * pKernel = ILuaExtModule::GetKernel(state);
    CHECK_ARG_NUM(state, nx_face_or_back, 2);
    // 检查参数类型
    CHECK_ARG_OBJECT(state, nx_face_or_back, 1);
    CHECK_ARG_OBJECT(state, nx_face_or_back, 2);

    PERSISTID one = pKernel->LuaToObject(state, 1);
    PERSISTID two = pKernel->LuaToObject(state, 2);

	len_t orient_one = pKernel->GetOrient(one);
	len_t orient_two = pKernel->GetOrient(two);

	int quadrant_one = GetQuadrant(orient_one);
	int quadrant_two = GetQuadrant(orient_two);

    bool res = false;
    if ((quadrant_one + 2) % 4 == quadrant_two)
    {
        res = true;
    }

    pKernel->LuaPushBool(state, res);
    return 1;
}

// 获得一个空的对象号
// PERSISTID nx_null_obj()
int nx_null_obj(void * state)
{
	IKernel * pKernel = ILuaExtModule::GetKernel(state);
	pKernel->LuaPushObject(state, PERSISTID());
	return 1;
}

int nx_get_attr_saving(void * state)
{
	// 获得核心指针
	IKernel * pKernel = ILuaExtModule::GetKernel(state);

	// 检查参数数量
	CHECK_ARG_NUM(state, nx_get_attr_saving, 2);
	// 检查参数类型
	CHECK_ARG_OBJECT(state, nx_get_attr_saving, 1);
	CHECK_ARG_STRING(state, nx_get_attr_saving, 2);

	// 获取参数
	PERSISTID obj     = pKernel->LuaToObject(state, 1);
	IGameObj* pObj = pKernel->GetGameObj(obj);
	if (pObj == NULL)
	{
		return 1;
	}

	const char * prop = pKernel->LuaToString(state, 2);
	pKernel->LuaPushBool(state, pObj->GetAttrSaving(prop));
	// 表示返回值数量为1
	return 1;
}

int nx_get_attr_visible(void * state)
{
	// 获得核心指针
	IKernel * pKernel = ILuaExtModule::GetKernel(state);

	// 检查参数数量
	CHECK_ARG_NUM(state, nx_get_attr_visible, 2);
	// 检查参数类型
	CHECK_ARG_OBJECT(state, nx_get_attr_visible, 1);
	CHECK_ARG_STRING(state, nx_get_attr_visible, 2);

	// 获取参数
	PERSISTID obj     = pKernel->LuaToObject(state, 1);
	IGameObj* pObj = pKernel->GetGameObj(obj);
	if (pObj == NULL)
	{
		return 1;
	}

	const char * prop = pKernel->LuaToString(state, 2);
	pKernel->LuaPushBool(state, pObj->GetAttrVisible(prop));
	return 1;
}

#ifndef FSROOMLOGIC_EXPORTS
int nx_get_record_visible(void * state)
{
	// 获得核心指针
	IKernel * pKernel = ILuaExtModule::GetKernel(state);

	// 检查参数数量
	CHECK_ARG_NUM(state, nx_get_record_visible, 2);
	// 检查参数类型
	CHECK_ARG_OBJECT(state, nx_get_record_visible, 1);
	CHECK_ARG_STRING(state, nx_get_record_visible, 2);

	// 获取参数
	PERSISTID obj     = pKernel->LuaToObject(state, 1);
	const char * rec = pKernel->LuaToString(state, 2);
	pKernel->LuaPushBool(state, pKernel->GetRecordVisible(obj, rec));
	return 1;
}

int nx_get_record_saving(void * state)
{
	// 获得核心指针
	IKernel * pKernel = ILuaExtModule::GetKernel(state);

	// 检查参数数量
	CHECK_ARG_NUM(state, nx_get_record_saving, 2);
	// 检查参数类型
	CHECK_ARG_OBJECT(state, nx_get_record_saving, 1);
	CHECK_ARG_STRING(state, nx_get_record_saving, 2);

	// 获取参数
	PERSISTID obj     = pKernel->LuaToObject(state, 1);
	const char * rec = pKernel->LuaToString(state, 2);
	pKernel->LuaPushBool(state, pKernel->GetRecordSaving(obj, rec));
	return 1;
}
#endif // FSROOMLOGIC_EXPORTS

//分割逗号字符串
int nx_split_comma_string(void *state)
{
	// 获得核心指针
	IKernel* pKernel = ILuaExtModule::GetKernel(state);

	// 检查参数数量
	CHECK_ARG_NUM(state, nx_split_comma_string, 1);

	// 检查参数类型
	CHECK_ARG_STRING(state, nx_split_comma_string, 1);

	// 获取参数
	std::string allid = pKernel->LuaToString(state, 1);

	//分解参数
	CVarList args;
	util_split_string(args, allid, ",");

	//返回
	pKernel->LuaPushTable(state, args);
	return 1;
}

// 获得当前所在层信息0-15普通层，200水面，300空中，400水下
int nx_cur_floor(void* state)
{
	// 获得核心指针
	IKernel* pKernel = ILuaExtModule::GetKernel(state);

	// 检查参数数量
	CHECK_ARG_NUM(state, nx_cur_floor, 1);

	// 检查参数类型
	CHECK_ARG_OBJECT(state, nx_cur_floor, 1);

	// 获取参数
	PERSISTID self = pKernel->LuaToObject(state, 1);

	if (pKernel->Exists(self))
	{
		//int floor = pKernel->GetFloor(self);

		//fast_string info = "floor id = " + util_int64_as_string(floor);

		//::CustomSysInfo(pKernel, self, TIPSTYPE_GMINFO_MESSAGE, info.c_str(), CVarList());
	}
	
	return 1;
}

//导出对象的属性和表清单
int nx_output_obj_to_xml(void* state)
{
	// 获得核心指针
	IKernel* pKernel = ILuaExtModule::GetKernel(state);

	// 检查参数数量
	CHECK_ARG_NUM(state, nx_output_obj_to_xml, 2);

	// 检查参数类型
	CHECK_ARG_OBJECT(state, nx_output_obj_to_xml, 1);
	CHECK_ARG_STRING(state, nx_output_obj_to_xml, 2);


	// 获取参数
	PERSISTID self = pKernel->LuaToObject(state, 1);
	const char* file = pKernel->LuaToString(state, 2);

	bool res = out_put_xml(pKernel, self, file);

	pKernel->LuaPushBool(state,res);

	return 1;
}

// 原型：void nx_trace(string info)
// 功能：输出一段信息到Trace Log文件
int nx_print_script_invoke_info(void * state)
{
	// 获得核心指针
	IKernel * pKernel = ILuaExtModule::GetKernel(state);

	// 检查参数数量
	CHECK_ARG_NUM(state, nx_trace, 1);
	// 检查参数类型
	CHECK_ARG_OBJECT(state, nx_trace, 1);
	// 获取参数
	const  PERSISTID& player = pKernel->LuaToObject(state, 1);

	if (pKernel->Exists(player))
	{
		PRINT_SCRIPT_INVOKE_INFO(pKernel, player);
	}
	// 表示返回值数量为0
	return 0;
}
//自动寻路到某点
int nx_movetopoint(void *state)
{
	// 获得核心指针
	IKernel* pKernel = ILuaExtModule::GetKernel(state);

	// 检查参数数量
	CHECK_ARG_NUM(state, nx_movetopoint, 3);

	// 检查参数类型
	CHECK_ARG_OBJECT(state, nx_movetopoint, 1);
	CHECK_ARG_FLOAT(state, nx_movetopoint, 2);
	CHECK_ARG_FLOAT(state, nx_movetopoint, 3);
	// 获取参数
	PERSISTID player = pKernel->LuaToObject(state,1);
	len_t  dx = pKernel->LuaToFloat(state, 2);
	len_t  dz = pKernel->LuaToFloat(state, 3);

	AIFunction::StartMoveToPoint(pKernel, player, dx, dz);
	return 1;
}
int nx_addbuff(void *state)
{
	IKernel* pKernel = ILuaExtModule::GetKernel(state);
	CHECK_ARG_NUM(state,nx_addbuff ,2);

	CHECK_ARG_OBJECT(state, nx_addbuff, 1);
	CHECK_ARG_INT(state,nx_addbuff,2);
	PERSISTID player = pKernel->LuaToObject(state,1);
	IGameObj* pObj = pKernel->GetGameObj(player);
	if (pObj == NULL)
	{
		return 1;
	}

	int param = pKernel->LuaToInt(state,2);
	int64_t maxHP = pObj->QueryInt64("MaxHP");
	int defend = pObj->QueryInt("Defend");
	int minAttack = pObj->QueryInt("MinAttack");
	int maxAttack = pObj->QueryInt("MaxAttack");
	pObj->SetInt64("MaxHP",maxHP*param);
	pObj->SetInt("Defend",defend*param);
	pObj->SetInt("MinAttack",minAttack*param);
	pObj->SetInt("MaxAttack",maxAttack*param);
	pObj->SetInt64("HP",pObj->QueryInt64("MaxHP"));

	return 1;
}

//根据坐标换算格子
int nx_getgrid_bypos(void *state)
{
	IKernel* pKernel = ILuaExtModule::GetKernel(state);
	CHECK_ARG_NUM(state, nx_getgrid_bypos, 2);

	CHECK_ARG_FLOAT(state, nx_getgrid_bypos, 1);
	CHECK_ARG_FLOAT(state, nx_getgrid_bypos, 2);

	float posx = pKernel->LuaToFloat(state, 1);
	float posz = pKernel->LuaToFloat(state, 2);
	int iIndex = pKernel->GetGridIndexByPos(posx, posz);
	pKernel->LuaPushInt(state, iIndex);

	return 1;
}
 
//根据格子换算坐标
int nx_getpos_bygrid(void *state)
{
	IKernel* pKernel = ILuaExtModule::GetKernel(state);
	CHECK_ARG_NUM(state, nx_getpos_bygrid, 1);

	CHECK_ARG_INT(state, nx_getpos_bygrid, 1);

	int iIndex = pKernel->LuaToInt(state, 1);
	len_t posx, posz;
	pKernel->GetPosByIndex(iIndex, posx, posz);
	CVarList res;
	res << posx << posz;
	pKernel->LuaPushTable(state, res);

	return 1;
}

#ifndef FSROOMLOGIC_EXPORTS
int secret_finish(void *state)
{
    IKernel* pKernel = ILuaExtModule::GetKernel(state);
    CHECK_ARG_NUM(state, secret_finish, 2);
    CHECK_ARG_INT(state, secret_finish, 2);

    int result = pKernel->LuaToInt(state, 2);

    PERSISTID player = pKernel->LuaToObject(state,1);

    //SecretSceneModule::GMFinish(pKernel, player, result);

    return 1;
}

int gm_add_secret(void *state)
{
    IKernel* pKernel = ILuaExtModule::GetKernel(state);
    CHECK_ARG_NUM(state, gm_add_secret, 3);
    CHECK_ARG_INT(state, gm_add_secret, 2);
    CHECK_ARG_INT(state, gm_add_secret, 3);

    int nSceneId = pKernel->LuaToInt(state, 2);
    int value = pKernel->LuaToInt(state, 4);

    PERSISTID player = pKernel->LuaToObject(state,1);
    //SecretSceneModule::GMAddTimes(pKernel, player, nSceneId, value);

    return 1;
}

int open_all_secret_scene(void *state)
{
    IKernel* pKernel = ILuaExtModule::GetKernel(state);

    PERSISTID player = pKernel->LuaToObject(state,1);

    //SecretSceneModule::GMOpenAll(pKernel, player);    

    return 1;
}
int set_clone_coin(void *state)
{
    IKernel* pKernel = ILuaExtModule::GetKernel(state);

	CHECK_ARG_NUM(state, gm_add_secret, 2);
	CHECK_ARG_INT(state, gm_add_secret, 2);
	int times = pKernel->LuaToInt(state, 2);

    PERSISTID player = pKernel->LuaToObject(state,1);

	//SecretSceneModule::GMSetCloneCoin(pKernel, player, times);

	return 1;
}
int open_one_secret_scene(void *state)
{
    IKernel* pKernel = ILuaExtModule::GetKernel(state);
    CHECK_ARG_NUM(state, open_one_secret_scene, 2);
    CHECK_ARG_INT(state, open_one_secret_scene, 2);
    int sceneID = pKernel->LuaToInt(state, 2);
    PERSISTID player = pKernel->LuaToObject(state, 1);

    //SecretSceneModule::GMOpenOne(pKernel, player, sceneID);
    return 1;
}

//修改最大的循环次数
int nx_set_max_circulate_count(void* state)
{
	IKernel* pKernel = LuaExtModule::GetKernel(state);
	CHECK_ARG_NUM(state, nx_set_max_circulate_count, 2);
	// 检查参数类型
	CHECK_ARG_OBJECT(state, nx_set_max_circulate_count, 1);
	CHECK_ARG_INT(state, nx_set_max_circulate_count, 2);

	PERSISTID player = pKernel->LuaToObject(state,1);
	int count = pKernel->LuaToInt(state, 2);

	LuaExtModule::SeMaxCirculateCount(pKernel, player, count);
	return 1;
}

// 分发消息到所有服务器member
int nx_dispatch_to_member(void* state)
{
	IKernel* pKernel = LuaExtModule::GetKernel(state);
	if (pKernel == NULL)
	{
		return 0;
	}

	// 参数个数
	int num = pKernel->LuaGetArgCount(state);
	// 收集参数
	CVarList args;
	LoopBeginCheck(dispatch);
	for (int i = 1; i <= num; ++i)
	{
		LoopDoCheck(dispatch);
		if (pKernel->LuaIsObject(state, i))
		{
			args << pKernel->LuaToObject(state, i);
		}
		else if (pKernel->LuaIsInt(state, i))
		{
			args << pKernel->LuaToInt(state, i);
		}
		else if (pKernel->LuaIsInt64(state, i))
		{
			args << pKernel->LuaToInt64(state, i);
		}
		else if (pKernel->LuaIsString(state, i))
		{
			args << pKernel->LuaToString(state, i);
		}
		else if (pKernel->LuaIsWideStr(state, i))
		{
			args << pKernel->LuaToWideStr(state, i);
		}
		else if (pKernel->LuaIsFloat(state, i))
		{
			args << pKernel->LuaToFloat(state, i);
		}
		else if (pKernel->LuaIsDouble(state, i))
		{
			args << pKernel->LuaToDouble(state, i);
		}
	}

	// 公共空间
	IPubSpace* pPubSpace = pKernel->GetPubSpace(PUBSPACE_DOMAIN);
	if (pPubSpace == NULL)
	{
		return 0;
	}

	// 公共数据名称
	static std::wstring strPubDataName = L"";
	if (strPubDataName.empty())
	{
		strPubDataName = std::wstring(L"Domain_SceneList") + StringUtil::StringAsWideStr(StringUtil::IntAsString(pKernel->GetServerId()).c_str());
	}

	// 场景公共数据空间
	IPubData* pPubData = pPubSpace->GetPubData(strPubDataName.c_str());
	if (pPubData == NULL)
	{
		return 0;
	}

	// 场景表
	IRecord* pRecord = pPubData->GetRecord(SERVER_MEMBER_FIRST_LOAD_SCENE_REC);
	if (pRecord == NULL)
	{
		return 0;
	}

	// 分发命令
	CVarList msg;
	msg << COMMAND_MSG_DISPATCH_GM
		<< args;

	int rows = pRecord->GetRows();
	LoopBeginCheck(a);
	for(int i = 0; i < rows; ++i)
	{
		LoopDoCheck(a);

		pKernel->CommandToScene(pRecord->QueryInt(i, SERVER_MEMBER_FIRST_LOAD_SCENE_COL_scene_id), msg);
	}
	
	return 1;
}
#endif // FSROOMLOGIC_EXPORTS

LuaExtModule::MapGMFunc LuaExtModule::m_mapGMFunc;

bool LuaExtModule::Init(IKernel* pKernel)
{
	if (NULL == pKernel)
	{
		Assert(false);

		return false;
	}

	g_pAsynCtrlModule = (AsynCtrlModule *)pKernel->GetLogicModule("AsynCtrlModule");

#ifndef FSROOMLOGIC_EXPORTS
	//修改最大循环次数
	pKernel->AddIntCommandHook("scene", COMMAND_SET_MAX_CIRCULATE_COUNT, LuaExtModule::CommandSeMaxCirculateCount);
	pKernel->AddIntCommandHook("scene", COMMAND_MSG_DISPATCH_GM, LuaExtModule::OnCommandDispatchGM);
#endif // FSROOMLOGIC_EXPORTS

	DECL_LUA_EXT(nx_msgbox);
	DECL_LUA_EXT(nx_trace);
	DECL_LUA_EXT(nx_echo);
	DECL_LUA_EXT(nx_findmodule);

#ifndef FSROOMLOGIC_EXPORTS
	DECL_LUA_EXT(nx_get_district_id);
	DECL_LUA_EXT(nx_get_server_id);
#endif // FSROOMLOGIC_EXPORTS

	DECL_LUA_EXT(nx_sysinfo);
	DECL_LUA_EXT(nx_sysinfo_byname);
	DECL_LUA_EXT(nx_sysinfo_bychannel);
	DECL_LUA_EXT(nx_sysinfo_byscene);
	DECL_LUA_EXT(nx_sysinfo_bygroup);
	DECL_LUA_EXT(nx_sysinfo_broadcast);

	DECL_LUA_EXT(nx_custom);
	DECL_LUA_EXT(nx_custom_byname);
	DECL_LUA_EXT(nx_custom_byken);
	DECL_LUA_EXT(nx_custom_bygroup);
#ifndef FSROOMLOGIC_EXPORTS
	DECL_LUA_EXT(nx_custom_byscene);
	DECL_LUA_EXT(nx_custom_byworld);
	DECL_LUA_EXT(nx_custom_bychannel);
#endif // FSROOMLOGIC_EXPORTS

	DECL_LUA_EXT(nx_command);
	DECL_LUA_EXT(nx_command_byname);
	DECL_LUA_EXT(nx_command_byscene);
	DECL_LUA_EXT(nx_command_bygroup);
	DECL_LUA_EXT(nx_command_toscene);
	DECL_LUA_EXT(nx_command_toscenegroup);
#ifndef FSROOMLOGIC_EXPORTS
	DECL_LUA_EXT(nx_command_byworld);
	DECL_LUA_EXT(nx_command_toallscene);
#endif // FSROOMLOGIC_EXPORTS

	DECL_LUA_EXT(nx_find_property);
	DECL_LUA_EXT(nx_get_property_list);
	DECL_LUA_EXT(nx_get_property_type);
	DECL_LUA_EXT(nx_query_property);
	DECL_LUA_EXT(nx_set_property);
	DECL_LUA_EXT(nx_inc_property);

	DECL_LUA_EXT(nx_set_property_flag);
	DECL_LUA_EXT(nx_clear_property_flag);
	DECL_LUA_EXT(nx_test_property_flag);

	DECL_LUA_EXT(nx_find_data);
	DECL_LUA_EXT(nx_get_data_type);
	DECL_LUA_EXT(nx_add_data);
	DECL_LUA_EXT(nx_remove_data);
	DECL_LUA_EXT(nx_query_data);
	DECL_LUA_EXT(nx_set_data);

	DECL_LUA_EXT(nx_find_record);
	DECL_LUA_EXT(nx_get_record_list);
	DECL_LUA_EXT(nx_get_record_rows);
	DECL_LUA_EXT(nx_get_record_cols);
	DECL_LUA_EXT(nx_get_record_coltype);
	DECL_LUA_EXT(nx_add_record_row);
	DECL_LUA_EXT(nx_add_record_row_values);
	DECL_LUA_EXT(nx_find_record_value);
	DECL_LUA_EXT(nx_query_record_value);
	DECL_LUA_EXT(nx_query_record_row_values);
	DECL_LUA_EXT(nx_set_record_value);
	DECL_LUA_EXT(nx_set_record_row_values);
	DECL_LUA_EXT(nx_remove_record_row);
	DECL_LUA_EXT(nx_clear_record);
	DECL_LUA_EXT(nx_get_record_max_rows);
#ifndef FSROOMLOGIC_EXPORTS
	DECL_LUA_EXT(nx_set_record_flag);
	DECL_LUA_EXT(nx_test_record_flag);
	DECL_LUA_EXT(nx_clear_record_flag);

	DECL_LUA_EXT(nx_find_pubspace);
	DECL_LUA_EXT(nx_get_pubspace_count);
	DECL_LUA_EXT(nx_get_pubspace_list);
	DECL_LUA_EXT(nx_get_pubdata_list);
	DECL_LUA_EXT(nx_get_pubdata_count);
	DECL_LUA_EXT(nx_find_pubdata);
	DECL_LUA_EXT(nx_get_pubdata_uid);
	DECL_LUA_EXT(nx_find_pubdata_property);
	DECL_LUA_EXT(nx_get_pubdata_property_list);
	DECL_LUA_EXT(nx_get_pubdata_property_type);
	DECL_LUA_EXT(nx_query_pubdata_property);
	DECL_LUA_EXT(nx_find_pubdata_record);
	DECL_LUA_EXT(nx_get_pubdata_record_list);
	DECL_LUA_EXT(nx_get_pubdata_record_cols);
	DECL_LUA_EXT(nx_get_pubdata_record_rows);
	DECL_LUA_EXT(nx_get_pubdata_record_coltype);
	DECL_LUA_EXT(nx_find_pubdata_record_value);
	DECL_LUA_EXT(nx_query_pubdata_record_row_values);
	DECL_LUA_EXT(nx_query_pubdata_record_value);
#endif // FSROOMLOGIC_EXPORTS

	DECL_LUA_EXT(nx_cur_floor);

	DECL_LUA_EXT(nx_load_config);
	DECL_LUA_EXT(nx_get_config_property);

	DECL_LUA_EXT(nx_create_from_config);
	DECL_LUA_EXT(nx_createitem_from_config);
	DECL_LUA_EXT(nx_create);
	DECL_LUA_EXT(nx_create_container);
	DECL_LUA_EXT(nx_expand_container);
	DECL_LUA_EXT(nx_shrink_container);
	DECL_LUA_EXT(nx_create_clone);
	DECL_LUA_EXT(nx_create_to);
	DECL_LUA_EXT(nx_create_item);
	DECL_LUA_EXT(nx_create_obj);
	DECL_LUA_EXT(nx_destroy_self);

	DECL_LUA_EXT(nx_select);

	DECL_LUA_EXT(nx_get_scene);
	DECL_LUA_EXT(nx_get_scene_id);
	DECL_LUA_EXT(nx_get_scene_maxid);
	DECL_LUA_EXT(nx_get_scene_script);
	DECL_LUA_EXT(nx_get_scene_config);
	DECL_LUA_EXT(nx_find_scene_id);
	DECL_LUA_EXT(nx_scene_exists);
	DECL_LUA_EXT(nx_get_scene_player_count);
	DECL_LUA_EXT(nx_get_scene_players);
	DECL_LUA_EXT(nx_get_scene_class);
	DECL_LUA_EXT(nx_get_clone_scene_count);
	DECL_LUA_EXT(nx_get_clone_scene_list);
	DECL_LUA_EXT(nx_get_prototype_scene_id);
	DECL_LUA_EXT(nx_is_prototype_scene);

	DECL_LUA_EXT(nx_get_region);
	DECL_LUA_EXT(nx_get_height);
	DECL_LUA_EXT(nx_get_grid_index);
	DECL_LUA_EXT(nx_get_map_type);
	DECL_LUA_EXT(nx_get_walk_type);
	DECL_LUA_EXT(nx_can_walk);
	DECL_LUA_EXT(nx_obj_can_walk);
	DECL_LUA_EXT(nx_line_can_walk);
	DECL_LUA_EXT(nx_obj_line_can_walk);
	DECL_LUA_EXT(nx_trace_line_walk);

	DECL_LUA_EXT(nx_get_position);
	DECL_LUA_EXT(nx_get_move_speed);
	DECL_LUA_EXT(nx_get_rotate_speed);
    DECL_LUA_EXT(nx_get_move_mode);

	DECL_LUA_EXT(nx_moveto);
	DECL_LUA_EXT(nx_rotate);
	DECL_LUA_EXT(nx_motion);
	DECL_LUA_EXT(nx_location);
	DECL_LUA_EXT(nx_motion_no_rotate);
	DECL_LUA_EXT(nx_stop);
	DECL_LUA_EXT(nx_stop_walk);
	DECL_LUA_EXT(nx_stop_rotate);
	DECL_LUA_EXT(nx_get_dest_position);
    DECL_LUA_EXT(nx_get_motion_state);

	DECL_LUA_EXT(nx_distance);
	DECL_LUA_EXT(nx_distance3d);
	DECL_LUA_EXT(nx_angle);
	DECL_LUA_EXT(nx_dot_angle);

	DECL_LUA_EXT(nx_get_location);
	DECL_LUA_EXT(nx_switch_locate);
	DECL_LUA_EXT(nx_switch_born);
	DECL_LUA_EXT(nx_get_scene_born);
	DECL_LUA_EXT(nx_set_scene_born);

	DECL_LUA_EXT(nx_type);
	DECL_LUA_EXT(nx_get_config);
	DECL_LUA_EXT(nx_get_script);
	DECL_LUA_EXT(nx_exists);
	DECL_LUA_EXT(nx_parent);

	DECL_LUA_EXT(nx_get_capacity);
	DECL_LUA_EXT(nx_get_child_count);
	DECL_LUA_EXT(nx_get_item);
	DECL_LUA_EXT(nx_get_index);
	DECL_LUA_EXT(nx_get_child);
	DECL_LUA_EXT(nx_get_child_list);
	DECL_LUA_EXT(nx_get_around_list);
	DECL_LUA_EXT(nx_get_point_list);

	DECL_LUA_EXT(nx_new_gourpid);
	DECL_LUA_EXT(nx_group_child_list);
	DECL_LUA_EXT(nx_exchange);
	DECL_LUA_EXT(nx_speech);

	DECL_LUA_EXT(nx_add_heartbeat);
	DECL_LUA_EXT(nx_add_countbeat);
	DECL_LUA_EXT(nx_remove_heartbeat);
	DECL_LUA_EXT(nx_clear_heartbeat);
	DECL_LUA_EXT(nx_find_heartbeat);
	DECL_LUA_EXT(nx_get_beattime);

	DECL_LUA_EXT(nx_add_viewport);
	DECL_LUA_EXT(nx_remove_viewport);
	DECL_LUA_EXT(nx_find_viewport);
	DECL_LUA_EXT(nx_get_viewport_container);
	DECL_LUA_EXT(nx_clear_viewport);
	DECL_LUA_EXT(nx_get_viewers);
	DECL_LUA_EXT(nx_close_viewers);

	DECL_LUA_EXT(nx_set_landpoint);
	DECL_LUA_EXT(nx_set_landposition);

	DECL_LUA_EXT(nx_find_player);
#ifndef FSROOMLOGIC_EXPORTS
	DECL_LUA_EXT(nx_break_player);
	DECL_LUA_EXT(nx_break_account);
	DECL_LUA_EXT(nx_break_byname);
	DECL_LUA_EXT(nx_block_player);
	DECL_LUA_EXT(nx_block_player_minite);
	DECL_LUA_EXT(nx_get_player_scene);
	DECL_LUA_EXT(nx_get_player_count);
	DECL_LUA_EXT(nx_get_offline_count);
	
	DECL_LUA_EXT(nx_send_public_message);
#endif // FSROOMLOGIC_EXPORTS

	//其他算法相关的函数
	DECL_LUA_EXT(nx_split_string);
	DECL_LUA_EXT(nx_split_wstring);
	DECL_LUA_EXT(nx_get_ticks);

	DECL_LUA_EXT(nx_execute);

	//
	DECL_LUA_EXT(nx_reload_preload_config);
	DECL_LUA_EXT(nx_get_year_month_day);
	DECL_LUA_EXT(nx_get_day_of_week);
	DECL_LUA_EXT(nx_get_hour_minute_second);

    //64int 操作
	DECL_LUA_EXT(nx_util_get_time_64);
    DECL_LUA_EXT(nx_int64_inc_int64);
    DECL_LUA_EXT(nx_int64_dec_int64);
    DECL_LUA_EXT(nx_int64_compare_int64);
    DECL_LUA_EXT(nx_int64_large_int64);
	DECL_LUA_EXT(nx_null_obj);
    DECL_LUA_EXT(nx_face_or_back);
	DECL_LUA_EXT(nx_find_child_list_byconfig);
	DECL_LUA_EXT(nx_find_child_byconfig);

#ifndef FSROOMLOGIC_EXPORTS
	//属性检测
	DECL_LUA_EXT(nx_get_attr_saving);
	DECL_LUA_EXT(nx_get_attr_visible);
	DECL_LUA_EXT(nx_get_record_saving);
	DECL_LUA_EXT(nx_get_record_visible);
#endif // FSROOMLOGIC_EXPORTS

	DECL_LUA_EXT(nx_split_comma_string);

	DECL_LUA_EXT(nx_output_obj_to_xml);

	DECL_LUA_EXT(nx_print_script_invoke_info);
#ifndef FSROOMLOGIC_EXPORTS
	DECL_LUA_EXT(nx_get_online_count);
#endif // FSROOMLOGIC_EXPORTS
	DECL_LUA_EXT(nx_movetopoint);
	DECL_LUA_EXT(nx_addbuff);

	//格子对应位置
	DECL_LUA_EXT(nx_getgrid_bypos);
	//位置对应格子编号
	DECL_LUA_EXT(nx_getpos_bygrid);

#ifndef FSROOMLOGIC_EXPORTS
    DECL_LUA_EXT(secret_finish);
    DECL_LUA_EXT(gm_add_secret);
    DECL_LUA_EXT(open_all_secret_scene);
	DECL_LUA_EXT(set_clone_coin);
	DECL_LUA_EXT(nx_set_max_circulate_count);
    DECL_LUA_EXT(open_one_secret_scene);
#endif // FSROOMLOGIC_EXPORTS

#ifndef FSROOMLOGIC_EXPORTS
	DECL_LUA_EXT(nx_dispatch_to_member);
#endif // FSROOMLOGIC_EXPORTS

    return true;
}

bool LuaExtModule::Shut(IKernel* pKernel)
{
	return true;
}

#ifndef FSROOMLOGIC_EXPORTS
int LuaExtModule::CommandSeMaxCirculateCount(IKernel* pKernel, const PERSISTID& self,
											 const PERSISTID& sender, const IVarList& args)
{
	int count = args.IntVal(1);
	SetMaxCirculateCount(count);
	return 0;
}

void LuaExtModule::SeMaxCirculateCount(IKernel* pKernel, const PERSISTID& self, int count)
{
	CVarList var_list;
	var_list << COMMAND_SET_MAX_CIRCULATE_COUNT << count;
	pKernel->CommandToAllScene(var_list);

	//发消息给公服设置最大循环数,借用竞技场模块修改总的循环次数
// 	CVarList pub_list;
// 	pub_list << PUBSPACE_DOMAIN << ArenaModule::GetDomainName(pKernel).c_str() << PUBDATA_MSG_MAX_CIRCULATE_COUNT << count;
// 	pKernel->SendPublicMessage(pub_list);

	//发消息给离线服务器设置最大循环数,借用好友模块修改总的循环次数
	CVarList snsMsg;
	snsMsg << SPACE_SNS << L"SnsFriend" << SNSDATA_MSG_SET_MAX_CIRCULATE_COUNT << count;
	pKernel->SendSnsMessage(pKernel->GetDistrictId(), pKernel->GetServerId(), pKernel->GetMemberId(), pKernel->GetSceneId(), pKernel->SeekRoleUid(pKernel->QueryWideStr(self, "Name")), snsMsg);
}

// 分发GM命令
int LuaExtModule::OnCommandDispatchGM(IKernel* pKernel, const PERSISTID& self, const PERSISTID& sender, const IVarList& args)
{
	if (pKernel == NULL)
	{
		return 0;
	}

	// 参数不足 0 主消息ID 1 为GM所用角色PERSISID 2为要调用的命令函数名
	int count = static_cast<int>(args.GetCount());
	if (count < 3)
	{
		return 0;
	}

	// 命令函数名
	const char* strFunc = args.StringVal(2);
	if (StringUtil::CharIsNull(strFunc))
	{
		return 0;
	}

	// 查找函数
	MapGMFunc::iterator iter = m_mapGMFunc.find(strFunc);
	if (iter == m_mapGMFunc.end())
	{
		return 0;
	}

	const GM_CALL_BACK_FUNC& func= iter->second;
	if (func == NULL)
	{
		return 0;
	}

	// 整理参数
	CVarList msg;
	msg.Append(args, 3, count - 3);
	
	// 执行命令
	func(pKernel, args.ObjectVal(1), msg);

	return 0;
}
#endif // FSROOMLOGIC_EXPORTS

// 注册GM分发命令
int LuaExtModule::RegisterCallBackFunction(const char* strFunc, GM_CALL_BACK_FUNC func)
{
	// 查找函数
	MapGMFunc::iterator iter = m_mapGMFunc.find(strFunc);
	if (iter != m_mapGMFunc.end())
	{
		// 已经注册过了
		Assert(0);
		return 0;
	}

	m_mapGMFunc.insert(std::make_pair(strFunc, func));

	return 0;
}