-- 文件名: robot_def.lua
-- 说明: common const define
-- 作者:

-- 全局函数
-- 弹窗
-- ex_msg_box( string title, string content)
-- 转换成数值类型
-- ex_number( var )
-- 转换成字符串
-- ex_string( var )

-- 创建 CBytes 对象
-- bytes()
-- CBytes写字节(其他类型写类似)
-- tb_bytes.write_char(tb_bytes,char_val)
-- CBytes读取字节(其他类型读取类似)
-- tb_bytes.read_char(tb_bytes)

----------------------------------------------------------------
-- robot 配置相关
----------------------------------------------------------------
-- CHARGE_URL = "http://123.207.7.182:8080/v1/tickets"
-- REGISTER_URL = "http://123.207.7.182:7080/acc/regist"
-- TRANS_URL = "http://123.207.7.182:7080/restful/acc/third/conver"

-- Charge,Register,Trans 請求url
TRANS_URL = "http://192.168.9.8:7080/restful/acc/third/conver"
CHARGE_URL = "http://192.168.9.8:11000/api/v1/login"
REGISTER_URL = "http://192.168.9.8:11000/api/v1/login"

-- charge 配置：group,server_id, 内网，默认为"1","1".具体取值，请询问运维人员，谢谢
CHARGE_GROUP = "1"
CHARGE_SERVER_ID="1"

--外网计费
-- TRANS_URL = "http://123.207.74.46:10000/restful/acc/third/conver"
-- CHARGE_URL = "http://123.207.74.46:10000/v1/login"
-- REGISTER_URL = "http://123.207.74.46:10000/v1/acc/"

----------------------------------------------------------------
-- ROBOT控制参数 可实时更改
----------------------------------------------------------------
ROBOT_PREFIX_ACCOUNT = "srobot"       --帐号前缀
ROBOT_PREFIX_NAME    = "brobot"         --名字前缀

ROBOT_ENTRY_IP   = "127.0.0.1"        -- Entry服務器ip地址
ROBOT_ENTRY_PORT = 2001               -- Entry服務器端口,默認2001
ROBOT_LOGIN_TYPE = 1           		  -- 1,使用测试账号库登录; 2,使用xy计费登陆


ROBOT_MODE_TYPE_SYNC = true              -- 机器人执行方式：并发，随机
ROBOT_MODE_TIME_RANDOM=false             -- 时间随机
ROBOT_MODE_TIME_INTERVAL=1000            -- 时间间隔，单位ms，默认1000
ROBOT_MODE_TIME_INTERVAL_ADDIT=0         -- 时间附加，单位ms，默认1000

----------------------------------------------------------------
-- ROBOT控制参数 配置文件
----------------------------------------------------------------
ROBOT_TASK_IS_WORKING = false            -- 機器人任務工作中
ROBOT_MOVE_IS_WORKING = false            -- 機器人行走工作中

ROBOT_MOVE_VERIFY_TARGET_RANGE=true      -- 機器人行走，是否校驗行走到目標範圍內
ROBOT_MOVE_VALID_TARGET_RANGE=20.0       -- 機器人行走到達目標範圍內的長度

----------------------------------------------------------------
-- ROBOT代碼配置參數
----------------------------------------------------------------
ROBOT_PROCESS_CUSTOM_COUNT=20            -- Robot自定義流程，菜單數量
----------------------------------------------------------------
-- ROBOT其他配置
----------------------------------------------------------------
CHECK_AUTO_LOGIN_DISCONNECT_TIME = 5000	--检测自动登录或离线的频率
EXCEED_LOGIN_TIME = 5000				--登录到某一步的超时时间
IS_RANDOM_TYPE = false					-- 使用随机测试

----------------------------------------------------------------
-- 服务器消息id定义
----------------------------------------------------------------
STOC_LOGIN_SUCCEED = 0
STOC_PROPERTY_TABLE = STOC_LOGIN_SUCCEED+1
STOC_RECORD_TABLE = STOC_PROPERTY_TABLE+1
STOC_ENTRY_SCENE = STOC_RECORD_TABLE+1
STOC_EXIT_SCENE = STOC_ENTRY_SCENE+1
STOC_ADD_OBJECT = STOC_EXIT_SCENE+1
STOC_REMOVE_OBJECT = STOC_ADD_OBJECT+1
STOC_SCENE_PROPERTY = STOC_REMOVE_OBJECT+1
STOC_OBJECT_PROPERTY = STOC_SCENE_PROPERTY+1
STOC_CREATE_VIEW = STOC_OBJECT_PROPERTY+1
STOC_DELETE_VIEW = STOC_CREATE_VIEW+1
STOC_VIEW_PROPERTY = STOC_DELETE_VIEW+1
STOC_VIEW_ADD = STOC_VIEW_PROPERTY+1
STOC_VIEW_REMOVE = STOC_VIEW_ADD+1
STOC_VIEW_CHANGE = STOC_VIEW_REMOVE+1
STOC_RECORD_ADD_ROW = STOC_VIEW_CHANGE+1
STOC_RRECORD_DEL_ROW = STOC_RECORD_ADD_ROW+1
STOC_RECORD_GRID = STOC_RRECORD_DEL_ROW+1
STOC_RECORD_CLEAR = STOC_RECORD_GRID+1
STOC_SPEECH = STOC_RECORD_CLEAR+1
STOC_SYSTEM_INFO = STOC_SPEECH+1
STOC_MENU = STOC_SYSTEM_INFO+1
STOC_CLEAR_MENU = STOC_MENU+1
STOC_CUSTOM = STOC_CLEAR_MENU+1
STOC_LOCATION = STOC_CUSTOM+1
STOC_MOVING = STOC_LOCATION+1
STOC_ALL_DEST = STOC_MOVING+1
STOC_WARNING = STOC_ALL_DEST+1
STOC_FROM_GMCC = STOC_WARNING+1
STOC_ALL_PROP = STOC_FROM_GMCC+1
STOC_ADD_MORE_OBJECT = STOC_ALL_PROP+1
STOC_REMOVE_MORE_OBJECT = STOC_ADD_MORE_OBJECT+1

STOC_SERVER_INFO = 39
STOC_SET_VERIFY = STOC_SERVER_INFO+1
STOC_SET_ENCODE = STOC_SET_VERIFY+1
STOC_ERROR_CODE = STOC_SET_ENCODE+1
STOC_WORLD_INFO = STOC_ERROR_CODE+1
STOC_IDLE = STOC_WORLD_INFO+1
STOC_QUEUE = STOC_IDLE+1
STOC_TERMINATE = STOC_QUEUE+1
STOC_LINK_TO = STOC_TERMINATE+1
STOC_UNLINK = STOC_LINK_TO+1
STOC_LINK_MOVE = STOC_UNLINK+1

----------------------------------------------------------------
-- 客户端消息id定义
----------------------------------------------------------------
CTOS_LOGIN = 0
CTOS_CREATE_ROLE = CTOS_LOGIN+1
CTOS_DELETE_ROLE = CTOS_CREATE_ROLE+1
CTOS_CHOOSE_ROLE = CTOS_DELETE_ROLE+1
CTOS_REVIVE_ROLE = CTOS_CHOOSE_ROLE+1
CTOS_WORLD_INFO = CTOS_REVIVE_ROLE+1
CTOS_READY = CTOS_WORLD_INFO+1
CTOS_CUSTOM = CTOS_READY+1
CTOS_REQUEST_MOVE = CTOS_CUSTOM+1
CTOS_SELECT = CTOS_REQUEST_MOVE+1
CTOS_SPEECH = CTOS_SELECT+1
CTOS_GET_VERIFY = CTOS_SPEECH+1
CTOS_RET_ENCODE = CTOS_GET_VERIFY+1

-- 验证串的长度
VERIFY_MAX_LEN = 16;
SRV_VERSION=123456
VERITY_STRING = "0000000000000000"

--int的字节数
INT_SIZE=4

----------------------------------------------------------------
--客户端发送到服务器的消息
----------------------------------------------------------------
--发送GM命令，格式：int msgid, wstring info
CLIENT_CUSTOMMSG_GM = 19


--世界BOSS活动相关消息
CLIENT_CUSTOMMSG_WORLD_BOSS_ACTIVE = 238

--小退
CLIENT_CUSTOMMSG_QUIT_GAME = 500


----------------------------------------------------------------
--ROBOT服务
----------------------------------------------------------------

----------------------------------------------------------------
--服务器发送至客户端的消息
----------------------------------------------------------------
--发送可挑战玩家列表
-- int rank			排名
-- string name		玩家名
-- int fight_power	战斗力
-- int role_key		RoleKey
SERVER_CUSTOMMSG_ARENA_FIGHTER_LIST = 40

--原样返回客户端内容
SERVER_CUSTOMMSG_ROUND_TRIP = 210

----------------------------------------------------------------
-- 视图相关定义
----------------------------------------------------------------
VIEW_ID_EQUIP = 1                          -- 装备栏
VIEW_ID_TOOL = 2                           -- 道具栏
VIEW_ID_DEPOT = 3                          -- 仓库栏
VIEW_ID_SKILL_CONTAINER = 4                -- 技能容器
VIEW_ID_ATTACK_CONTAINER = 5               -- 普通攻击技能容器
VIEW_ID_BUFFER = 6                         -- BUFFER容器
VIEW_ID_GODS_BOX = 7                       -- 神灵装备容器
VIEW_ID_GODS_BAG = 8                       -- 神灵背包容器
VIEW_ID_EQUIP_BAG = 9                      -- 装备背包容器
VIEW_ID_JEWEL_BAG = 10                     -- 宝石背包容器
VIEW_ID_GODS_SOUL_BAG = 11                 -- 神灵魂魄容器
VIEW_ID_SURPLUS_BAG = 12                   -- 临时背包
VIEW_ID_MAX = 12                           -- 视图id最大值
