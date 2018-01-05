/*
 * =====================================================================================
 *
 *       Filename:  Language.h
 *
 *    Description:  语言包定义宏
 *
 *        Version:  1.0
 *        Created:  2015-11-02 11:26:19 
 *       Revision:  none
 *
 *         Author:  xubin
 *        Company:  Taichuan
 *
 * =====================================================================================
 */
#ifndef _LANGUAGE_H
#define _LANGUAGE_H

#ifdef __cplusplus
extern "C" {
#endif  /* __cplusplus */

	enum LANGUAG_TYPE{
		CHINEASE = 0,
		ENGLISH,
		TURKISH,
		TAIWAN,
		RUSSIAN,
		UKRAINIAN,
		LANGUAGE_NUM
	};

	enum {
		WORD_MENU,//菜单
		WORD_UNLOCK,//开锁
		WORD_MONITOR,//监视
		WORD_CALLCENTER,//中心
		WORD_MESSAGE,//查阅
		WORD_SAFE_SATUS,//防区状态
		WORD_WEEK1,//星期一
		WORD_WEEK2,//星期二
		WORD_WEEK3,//星期三
		WORD_WEEK4,//星期四
		WORD_WEEK5,//星期五
		WORD_WEEK6,//星期六
		WORD_WEEK7,//星期日
		WORD_YEAR,//年
		WORD_MOUNTH,//月
		WORD_DATE,//日
		WORD_UNLOCK_SUCCESS,//开锁成功
		WORD_QUITE_MODE,//免扰模式
		WORD_QUIET_MODE_SUCCESS,//已设置免扰模式
		WORD_QUIET_MODE_EXIT,//已退出免扰模式


		WORD_CALL,//呼叫
		WORD_CALLING,//正在呼叫
		WORD_CALLING_FAIL,//呼叫失败
		WORD_ANSWER,//接听
		WORD_HANG_UP,//挂机
		WORD_VEDIO_FAIL,//视频连接中
		WORD_TALKING,//正在通话
		WORD_WATING,//等待接听
		WORD_LEFT_TIME,//剩余时间
		WORD_SECOND,//秒
		WORD_CALLING_BUZY,//呼叫用户忙!

		WORD_BACK,//返回
		WORD_UP,//上翻
		WORD_DOWN,//下翻
		WORD_CONFIRM,//确认

		WORD_ALARMNG_AND_DIS,//布防撤防
		WORD_SET_ROOM_NUM,//房号设置
		WORD_SYSINFO,//设备信息
		WORD_DISARMING_PASSWD,//撤防密码
		WORD_HELP,//帮助信息
		WORD_LANGUAGE,//语言选择
		WORD_CANCEL,//取消

		WORD_INSERT_ALARMING_PASSWD,//请输入撤防密码
		WORD_ZONE,//防 区
		WORD_ZONE1,//防 区1
		WORD_ZONE2,//防 区2
		WORD_ZONE3,//防 区3
		WORD_ZONE4,//防 区4
		WORD_ZONE5,//防 区5
		WORD_ZONE6,//防 区6
		WORD_ZONE7,//防 区7
		WORD_ZONE8,//防 区8
		WORD_OUT_ALARM,//外出布防
		WORD_IN_ALARM,//留守布防
		WORD_ALARM_SUCCESS, //布防成功
		WORD_ALARM_SUCCESS_SEND_TO_CENTER, //有线安防设备已布防
		WORD_FORCE_ALARM_SUCCESS, //强制布防成功
		WORD_FORCE_ALARM_SUCCESS_SEND_TO_CENTER, //有线安防设备已执行强制布防
		WORD_ALARM_FAIL, //布防失败，是否强制布防
		WORD_DISALARM_SUCCESS, //撤防成功
		WORD_DISALARM_SUCCESS_SEND_TO_CENTER, //有线安防设备已撤防
		WORD_INSERT_NEW_ALARMING_PASSWD, //请输入新密码
		WORD_INSERT_NEW_ALARMING_PASSWD_AGAIN, //请再次输入新密码
		WORD_INCORRECT_PASSWD, //密码错误
		WORD_UNSAME_PASSWD, //两次密码不一致，请重输
		WORD_CHANGE_SUCCESS, //密码修改成功,按"取消"键返回主界面
		WORD_DEVICE_ALARMING,//设备报警

		WORD_INSERT_SETTING_PASSWD,//请输入工程密码

		WORD_DEVICE_INFO,//设备信息
		WORD_GET_DEVICE_ID,//按确认件获取设备ID
		WORD_DEVICE_NUM,//设备编号
		WORD_SYSTEM_VERSION,//系统版本
		WORD_SOFTWARE_VERSION,//软件版本
		WORD_RELEASE_DATE,//发布日期
		WORD_IP_ADD,//本机IP
		WORD_NET_MASK,//子网掩码
		WORD_GATE_WAY,//默认网关
		WORD_BUILD,//栋
		WORD_FLOOR,//层
		WORD_ROOM,//房
		WORD_MAC_ADD,//MAC地址
		WORD_CENTER_IP,//管理中心
		WORD_DMK_IP,//门口机
		WORD_GET_DEVICE_ID_ING,//正在获取设备ID请稍后...
		WORD_GET_DEVICE_ID_SUCCESS,//成功获取新ID
		WORD_GET_DEVICE_ID_FAIL,//获取设备ID失败，请检查网络设置



		WORD_CHOICE_LANGUAGE,//语言选择
		WORD_COMFIRM_AND_REBOOT,//请按确认重启生效

		WORD_UPDATING,//正在下载更新程序，请勿关闭电源
		WORD_REBOOT,//更新成功，系统准备重启
		WORD_UPDATE_FAIL,//更新失败

		WORD_CHECK_MESSAGE,//查阅信息
		WORD_CLEAR_MESSAGE,//清除
		WORD_LEAVE_MESSAGE,//留言

		WORD_MESSAGE_TITLE,//信息来源
		WORD_MESSAGE_TIME,//接收时间
		WORD_MESSAGE_WORD,//信息内容

		WORD_MESSAGE_CLEAR,//再按清除键将删除本条记录
		WORD_MESSAGE_CLEAR_ALL,//再按留言键将清空全部记录

		WORD_UNBROKEN_ALARMING,//设备被拆除，请安装好后重新上电！
		WORD_MACHINE_BROKEN,//机器被拆

		WORD_VOLUME_SET,//音量设置
		WORD_VOLUME_SET_OK,//音量设置成功
		WORD_VOLUME_SET_FAILED,//音量设置失败
		WORD_SET_TEN,//设置十位
		WORD_SET_ONE,//设置个位
		WORD_VOLUME_SET_EXIT,//再按“取消”退出设置
		WORD_NEXT,//下一位

		WORD_SEARCHING_CENTER,//正在搜索管理中心...
		WORD_GETTING_ROOM_NUM,//正在获取房号信息...
		WORD_GETTING_ROOM_NUM_FAIL,//获取数据超时，房号设置失败
		WORD_SETTING_TMP_IP,//正在设置临时IP，请稍候...
		WORD_SELECT_CENTER,//请选择一个管理中心
		WORD_SELECT_QH,//请选择期号
		WORD_SELECT_DH,//请选择栋号
		WORD_SELECT_DYH,//请选择单元号
		WORD_SELECT_CH,//请选择层号
		WORD_SELECT_FH,//请选择房号
		WORD_SELECT_FJH,//请选择分机号
		WORD_DMK_NAME,//户门口机
		WORD_SETTING_OK_AND_SAVE,//房号设置成功, 点击返回
		WORD_SEND_DATA_ERROR,//数据传输错误
		WORD_PROCESSING,//正在处理，请稍候...

		WORD_RECORD,//记录查询
		WORD_TALK_RECORD,//通话记录
		WORD_ALARM_RECORD,//报警记录

		WORD_CALL_IN,//呼入
		WORD_CALL_OUT,//呼出到
		WORD_SWITCH,//切换
		WORD_ALARM_WORD,//报警内容
		WORD_ALARM_TIME,//报警时间
		WORD_SWITCH_CLEAR_ALL,//再按切换键将清空全部记录
		WORD_CAN_NOT_CLEAR_ALARM,//不能清除报警记录

		WORD_RECOVER,//数据损坏，正在恢复数据，系统将重启3次，请勿关闭电源

		WORD_ELEVATOR_ARRIVED,//电梯已到达!
		WORD_CALLING_ELEVATOR,//正在呼唤电梯!
		WORD_NO_DMK,//没配置门口机!


		WORD_DEVICE_BUTTON1,//紧急按钮1
		WORD_DEVICE_SMOG1,//烟雾1
		WORD_DEVICE_INFRARED1,//红外1
		WORD_DEVICE_DOOR_CONTACT1,//门磁1
		WORD_DEVICE_INFRARED2,//红外2
		WORD_DEVICE_DOOR_CONTACT2,//门磁2
		WORD_DEVICE_GENERAL1,//通用报警1
		WORD_DEVICE_GENERAL2,//通用报警2

		WORD_LEAVE_RECORD,//查阅留言
		WORD_FILE_NAME,	//文件名称
		WORD_RECORD_TIME,	//记录时间

		WORD_MEMERY_NOT_ENOUGH,	//空间不足
		WORD_ELEVATOR,	//电梯
		WORD_NO_VIDEO,//无视频

		WORD_TIME_SET,//时间设置
		WORD_TIME_SET_TIME,//时 钟:
		WORD_TIME_SET_DATE,//日 期:
		WORD_TIME_SET_SYNC,//是否同步管理中心?
		WORD_TIME_SET_SUCCESS,//时间设置成功

		WORD_DEVICE_TYPE_WRONG,//设备类型不匹配
		WORD_DEVICE_TYPE_MASTER,//主机
		WORD_DEVICE_TYPE_SLAVE,//分机
		WORD_DEVICE_TYPE,//机型
		WORD_PROTOCOL,  //协议类型:

		WORD_TALK_WITH_OTHER,//户户对讲
		WORD_EDIT,//编辑
		WORD_CALL_ADDRESS,//呼叫地址
		WORD_ADD,//添加
		WORD_DELETE,//删除
		WORD_ADD_SUCSEES,//添加成功,请返回查看
		WORD_CAN_NOT_SET_MYSELF_IP,//不能设本机IP
		WORD_ADDRESS_EXIST,//地址已存在
		WORD_SET_HOUSE_SUCSEES,//设置成功
		WORD_HOUSE_CLEAR,//再按删除键将删除本条记录
		WORD_HOUSE_CLEAR_ALL,//再按添加键将清空全部记录

		WORD_LIGHT_SET,//亮度设置
		WORD_LIGHT_SET_SUCCESS,//亮度设置成功

		WORD_HELP_OP,//操作说明
		WORD_HELP_CALL,//呼叫住户：输入房号，按#键
		WORD_HELP_CLEAR,//清    除：输入错误，按*键
		WORD_HELP_CENTER,//管理中心：输入0000，按#键

		WORD_PLEASE_INSERT_PHONE_NUM,//请输入房号
		WORD_MONITOR_LOCAL, // 正在监视本机
		WORD_MONITOR_COMM_ERR, // 对讲通信异常

		WORD_CARD_SUCCESS,	// 刷卡成功
		WORD_CARD_USELESS,	// 卡已停用
		WORD_CARD_LOSS,	// 卡已挂失
		WORD_CARD_OVERDUE,	// 卡已过期
		WORD_CARD_UNREGIST,	// 未注册卡

		WORD_PHONE_NUM_DOT_EXIST,	// 房号不存在，请重新输入 
		WORD_HELP_PASSWORD,	// 长按0返回 退格(*) 确定(#)

		WORD_HELP_CALL_USER,	//按#键呼叫中心
		WORD_HELP_CALL_CENTER,	//按#键呼叫
		WORD_HELP_LEAVE_WORD,	//留  言：长按0键,管理中心不能留言
		WORD_HELP_HAND_UP,		//挂  机：按#键

		WORD_INSERT_UNLOCK_PASSWD,	//请输入开锁密码
		WORD_SETTING_PRESS_KEY,		//请按对应键选择
		WORD_SETTING_USER,			//用户设置
		WORD_SETTING_LOCAL,			//本机设置
		WORD_SETTING_SYSTEM,		//系统参数
		WORD_SETTING_PASSWORD,		//本机密码
		WORD_SETTING_ACCESS,		//门禁参数
		WORD_SETTING_ELEVATOR,		//电梯联动
		WORD_SETTING_REBOOT,		//重新启动
		WORD_SETTING_DEVTYPE,		//设置机型
		WORD_SETTING_RETURN,		//返回请按#键

		WORD_CLEAR_ALL,		//清空
		WORD_FIND,			//查询
		WORD_RECORD_NUM,	//记录数
		WORD_ROOM_NUM,		//房间号码
		WORD_IP,			//IP地址
		WORD_HELP_EDIT_IP,		//选项(*)长按0(.)删除(#)
		WORD_HELP_BUTTON,	//选项(*)确定(#)
		WORD_ROOM_NUM_SORT,	//房号
		WORD_EXSIT,			//已存在
		WORD_NOT_EXSIT,		//不存在
		WORD_PLEASE_INSERT_IP,//请输入IP
		WORD_SUCCESS,		// 成功
		WORD_FAILED,		// 失败
		WORD_CAN_NOT_DELETE_CENTER,		// 不能删除管理中心
		WORD_NET_SETTING,		// 网络参数设置
		WORD_SAVE,		// 保存
		WORD_GET_ID,		// 获取ID
		WORD_WRONG_FORMATE,		// 格式错误
		WORD_FRAME_PERIOD,		// 帧周期
		WORD_ALRM_DOOR,		// 开门报警
		WORD_ENABLE_ALRM_DOOR,		// 使能开门报警
		WORD_ENABLE_PASSWORD,		// 启用密码开锁
		WORD_HELP_EDIT,		//选项(*)删除(#)
		WORD_HELP_MENU,		//选项(*)上移(8)下移(0)选择(#)
		WORD_SETTING,		//设置
		WORD_PLEASE_INSERT_SIX_PASSWORD,		//注意：需要输入6位密码
		WORD_NEW_SYSTEM_PASSWORD,		// 新工程密码
		WORD_NEW_SYSTEM_PASSWORD_CONFIRM,		// 新工程密码确认
		WORD_NEW_UNLOCK_PASSWORD,		// 新开锁密码
		WORD_NEW_UNLOCK_PASSWORD_CONFIRM,		// 新开锁密码确认
		WORD_SYSTEM_PASSWORD,		// 工程密码
		WORD_UNLOCK_PASSWORD,		// 开锁密码
		WORD_CARD_NUM,		// 卡号
		WORD_CARD_TYPE,		// 卡类型
		WORD_CARD_TYPE_OWNER,		// 住户卡
		WORD_CARD_TYPE_MANAGE,		// 物管卡
		WORD_CARD_TYPE_TEMP,		// 临时卡
		WORD_CARD_TYPE_INVALID,		// 无效卡
		WORD_PLEASE_INSERT_CARD_NUM,// 请输入卡号

		WORD_HOUR,// 时
		WORD_MINUTE,// 分
		WORD_HELP_SYNC_TIME,// 已同步，不能修改时间
		WORD_ELEVATOR_ADDRESS,// 电梯地址
		WORD_FLOOR_NUM,// 楼层号
		WORD_ENABLE_ELEVATOR,// 是否启用电梯联动
		WORD_SINGLE_VERSION, // 单片机版本
		WORD_HELP_TOPMESSAGE, // 返回(*)确认(#)
		WORD_CONFIRM_CLEAR_ALL_IP, //确认清除全部用户数据吗？
		WORD_CONFIRM_CLEAR_ALL_CARD, //确认清除全部用户卡数据吗？
		WORD_HELP_EDIT_ELEVATOR, //选项(*)长按0(-)删除(#)
		WORD_DOOR_OPEN, //门已打开
		WORD_DOOR_CLOSED, //门已关闭
		WORD_DOOR_OPEN_TOO_LONG, //门被打开时间过长!
		WORD_ENABLE_UNBROKEN,	// 启用防拆报警
		WORD_LEAVE_MESSAGE_ING,	// 正在留言
		WORD_YEAR_ENC,//年 (外文字)
		WORD_MONTH_ENC,//月 (外文字)
		WORD_DATE_ENC,//日 (外文字)
		WORD_ELEVATOR_LOBBY,//大堂号
		WORD_UPDATE_CARD,//正在更新卡数据，请勿断电
		WORD_UPDATE_CARD_SUCCESS,//卡数据更新完成,请稍后
		WORD_UPDATE_CARD_FAIL,//卡数据更新失败，请联系管理员
		WORD_UPDATE_USER,//正在更新用户数据，请勿断电
		WORD_UPDATE_USER_SUCCESS,//用户数据更新完成,请稍后
		WORD_UPDATE_USER_FAIL,//用户数据更新失败，请联系管理员
		WORD_CONTACT_LIST,		//通讯录
		WORD_USERSEARCH_TITLE,	//鎴蜂富鏌ヨ鐣岄潰鏍囬淇℃伅
		WORD_USERSEARCH_HELP,	//鎴蜂富鏌ヨ鐣岄潰甯姪淇℃伅
		WORD_UNIT,				//单元
		WORD_NO_UNIT,			//无单元

	};


	typedef struct _LanguageStruct{
		char *name;
		char string[128];
	}LanguageStruct;

	extern LanguageStruct Word[];
	int languageLoad(void);
#ifdef __cplusplus
}
#endif  /* __cplusplus */

#endif

