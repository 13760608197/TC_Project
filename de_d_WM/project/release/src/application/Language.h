/*
 * =====================================================================================
 *
 *       Filename:  Language.h
 *
 *    Description:  ���԰������
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
		WORD_MENU,//�˵�
		WORD_UNLOCK,//����
		WORD_MONITOR,//����
		WORD_CALLCENTER,//����
		WORD_MESSAGE,//����
		WORD_SAFE_SATUS,//����״̬
		WORD_WEEK1,//����һ
		WORD_WEEK2,//���ڶ�
		WORD_WEEK3,//������
		WORD_WEEK4,//������
		WORD_WEEK5,//������
		WORD_WEEK6,//������
		WORD_WEEK7,//������
		WORD_YEAR,//��
		WORD_MOUNTH,//��
		WORD_DATE,//��
		WORD_UNLOCK_SUCCESS,//�����ɹ�
		WORD_QUITE_MODE,//����ģʽ
		WORD_QUIET_MODE_SUCCESS,//����������ģʽ
		WORD_QUIET_MODE_EXIT,//���˳�����ģʽ


		WORD_CALL,//����
		WORD_CALLING,//���ں���
		WORD_CALLING_FAIL,//����ʧ��
		WORD_ANSWER,//����
		WORD_HANG_UP,//�һ�
		WORD_VEDIO_FAIL,//��Ƶ������
		WORD_TALKING,//����ͨ��
		WORD_WATING,//�ȴ�����
		WORD_LEFT_TIME,//ʣ��ʱ��
		WORD_SECOND,//��
		WORD_CALLING_BUZY,//�����û�æ!

		WORD_BACK,//����
		WORD_UP,//�Ϸ�
		WORD_DOWN,//�·�
		WORD_CONFIRM,//ȷ��

		WORD_ALARMNG_AND_DIS,//��������
		WORD_SET_ROOM_NUM,//��������
		WORD_SYSINFO,//�豸��Ϣ
		WORD_DISARMING_PASSWD,//��������
		WORD_HELP,//������Ϣ
		WORD_LANGUAGE,//����ѡ��
		WORD_CANCEL,//ȡ��

		WORD_INSERT_ALARMING_PASSWD,//�����볷������
		WORD_ZONE,//�� ��
		WORD_ZONE1,//�� ��1
		WORD_ZONE2,//�� ��2
		WORD_ZONE3,//�� ��3
		WORD_ZONE4,//�� ��4
		WORD_ZONE5,//�� ��5
		WORD_ZONE6,//�� ��6
		WORD_ZONE7,//�� ��7
		WORD_ZONE8,//�� ��8
		WORD_OUT_ALARM,//�������
		WORD_IN_ALARM,//���ز���
		WORD_ALARM_SUCCESS, //�����ɹ�
		WORD_ALARM_SUCCESS_SEND_TO_CENTER, //���߰����豸�Ѳ���
		WORD_FORCE_ALARM_SUCCESS, //ǿ�Ʋ����ɹ�
		WORD_FORCE_ALARM_SUCCESS_SEND_TO_CENTER, //���߰����豸��ִ��ǿ�Ʋ���
		WORD_ALARM_FAIL, //����ʧ�ܣ��Ƿ�ǿ�Ʋ���
		WORD_DISALARM_SUCCESS, //�����ɹ�
		WORD_DISALARM_SUCCESS_SEND_TO_CENTER, //���߰����豸�ѳ���
		WORD_INSERT_NEW_ALARMING_PASSWD, //������������
		WORD_INSERT_NEW_ALARMING_PASSWD_AGAIN, //���ٴ�����������
		WORD_INCORRECT_PASSWD, //�������
		WORD_UNSAME_PASSWD, //�������벻һ�£�������
		WORD_CHANGE_SUCCESS, //�����޸ĳɹ�,��"ȡ��"������������
		WORD_DEVICE_ALARMING,//�豸����

		WORD_INSERT_SETTING_PASSWD,//�����빤������

		WORD_DEVICE_INFO,//�豸��Ϣ
		WORD_GET_DEVICE_ID,//��ȷ�ϼ���ȡ�豸ID
		WORD_DEVICE_NUM,//�豸���
		WORD_SYSTEM_VERSION,//ϵͳ�汾
		WORD_SOFTWARE_VERSION,//����汾
		WORD_RELEASE_DATE,//��������
		WORD_IP_ADD,//����IP
		WORD_NET_MASK,//��������
		WORD_GATE_WAY,//Ĭ������
		WORD_BUILD,//��
		WORD_FLOOR,//��
		WORD_ROOM,//��
		WORD_MAC_ADD,//MAC��ַ
		WORD_CENTER_IP,//��������
		WORD_DMK_IP,//�ſڻ�
		WORD_GET_DEVICE_ID_ING,//���ڻ�ȡ�豸ID���Ժ�...
		WORD_GET_DEVICE_ID_SUCCESS,//�ɹ���ȡ��ID
		WORD_GET_DEVICE_ID_FAIL,//��ȡ�豸IDʧ�ܣ�������������



		WORD_CHOICE_LANGUAGE,//����ѡ��
		WORD_COMFIRM_AND_REBOOT,//�밴ȷ��������Ч

		WORD_UPDATING,//�������ظ��³�������رյ�Դ
		WORD_REBOOT,//���³ɹ���ϵͳ׼������
		WORD_UPDATE_FAIL,//����ʧ��

		WORD_CHECK_MESSAGE,//������Ϣ
		WORD_CLEAR_MESSAGE,//���
		WORD_LEAVE_MESSAGE,//����

		WORD_MESSAGE_TITLE,//��Ϣ��Դ
		WORD_MESSAGE_TIME,//����ʱ��
		WORD_MESSAGE_WORD,//��Ϣ����

		WORD_MESSAGE_CLEAR,//�ٰ��������ɾ��������¼
		WORD_MESSAGE_CLEAR_ALL,//�ٰ����Լ������ȫ����¼

		WORD_UNBROKEN_ALARMING,//�豸��������밲װ�ú������ϵ磡
		WORD_MACHINE_BROKEN,//��������

		WORD_VOLUME_SET,//��������
		WORD_VOLUME_SET_OK,//�������óɹ�
		WORD_VOLUME_SET_FAILED,//��������ʧ��
		WORD_SET_TEN,//����ʮλ
		WORD_SET_ONE,//���ø�λ
		WORD_VOLUME_SET_EXIT,//�ٰ���ȡ�����˳�����
		WORD_NEXT,//��һλ

		WORD_SEARCHING_CENTER,//����������������...
		WORD_GETTING_ROOM_NUM,//���ڻ�ȡ������Ϣ...
		WORD_GETTING_ROOM_NUM_FAIL,//��ȡ���ݳ�ʱ����������ʧ��
		WORD_SETTING_TMP_IP,//����������ʱIP�����Ժ�...
		WORD_SELECT_CENTER,//��ѡ��һ����������
		WORD_SELECT_QH,//��ѡ���ں�
		WORD_SELECT_DH,//��ѡ�񶰺�
		WORD_SELECT_DYH,//��ѡ��Ԫ��
		WORD_SELECT_CH,//��ѡ����
		WORD_SELECT_FH,//��ѡ�񷿺�
		WORD_SELECT_FJH,//��ѡ��ֻ���
		WORD_DMK_NAME,//���ſڻ�
		WORD_SETTING_OK_AND_SAVE,//�������óɹ�, �������
		WORD_SEND_DATA_ERROR,//���ݴ������
		WORD_PROCESSING,//���ڴ������Ժ�...

		WORD_RECORD,//��¼��ѯ
		WORD_TALK_RECORD,//ͨ����¼
		WORD_ALARM_RECORD,//������¼

		WORD_CALL_IN,//����
		WORD_CALL_OUT,//������
		WORD_SWITCH,//�л�
		WORD_ALARM_WORD,//��������
		WORD_ALARM_TIME,//����ʱ��
		WORD_SWITCH_CLEAR_ALL,//�ٰ��л��������ȫ����¼
		WORD_CAN_NOT_CLEAR_ALARM,//�������������¼

		WORD_RECOVER,//�����𻵣����ڻָ����ݣ�ϵͳ������3�Σ�����رյ�Դ

		WORD_ELEVATOR_ARRIVED,//�����ѵ���!
		WORD_CALLING_ELEVATOR,//���ں�������!
		WORD_NO_DMK,//û�����ſڻ�!


		WORD_DEVICE_BUTTON1,//������ť1
		WORD_DEVICE_SMOG1,//����1
		WORD_DEVICE_INFRARED1,//����1
		WORD_DEVICE_DOOR_CONTACT1,//�Ŵ�1
		WORD_DEVICE_INFRARED2,//����2
		WORD_DEVICE_DOOR_CONTACT2,//�Ŵ�2
		WORD_DEVICE_GENERAL1,//ͨ�ñ���1
		WORD_DEVICE_GENERAL2,//ͨ�ñ���2

		WORD_LEAVE_RECORD,//��������
		WORD_FILE_NAME,	//�ļ�����
		WORD_RECORD_TIME,	//��¼ʱ��

		WORD_MEMERY_NOT_ENOUGH,	//�ռ䲻��
		WORD_ELEVATOR,	//����
		WORD_NO_VIDEO,//����Ƶ

		WORD_TIME_SET,//ʱ������
		WORD_TIME_SET_TIME,//ʱ ��:
		WORD_TIME_SET_DATE,//�� ��:
		WORD_TIME_SET_SYNC,//�Ƿ�ͬ����������?
		WORD_TIME_SET_SUCCESS,//ʱ�����óɹ�

		WORD_DEVICE_TYPE_WRONG,//�豸���Ͳ�ƥ��
		WORD_DEVICE_TYPE_MASTER,//����
		WORD_DEVICE_TYPE_SLAVE,//�ֻ�
		WORD_DEVICE_TYPE,//����
		WORD_PROTOCOL,  //Э������:

		WORD_TALK_WITH_OTHER,//�����Խ�
		WORD_EDIT,//�༭
		WORD_CALL_ADDRESS,//���е�ַ
		WORD_ADD,//���
		WORD_DELETE,//ɾ��
		WORD_ADD_SUCSEES,//��ӳɹ�,�뷵�ز鿴
		WORD_CAN_NOT_SET_MYSELF_IP,//�����豾��IP
		WORD_ADDRESS_EXIST,//��ַ�Ѵ���
		WORD_SET_HOUSE_SUCSEES,//���óɹ�
		WORD_HOUSE_CLEAR,//�ٰ�ɾ������ɾ��������¼
		WORD_HOUSE_CLEAR_ALL,//�ٰ���Ӽ������ȫ����¼

		WORD_LIGHT_SET,//��������
		WORD_LIGHT_SET_SUCCESS,//�������óɹ�

		WORD_HELP_OP,//����˵��
		WORD_HELP_CALL,//����ס�������뷿�ţ���#��
		WORD_HELP_CLEAR,//��    ����������󣬰�*��
		WORD_HELP_CENTER,//�������ģ�����0000����#��

		WORD_PLEASE_INSERT_PHONE_NUM,//�����뷿��
		WORD_MONITOR_LOCAL, // ���ڼ��ӱ���
		WORD_MONITOR_COMM_ERR, // �Խ�ͨ���쳣

		WORD_CARD_SUCCESS,	// ˢ���ɹ�
		WORD_CARD_USELESS,	// ����ͣ��
		WORD_CARD_LOSS,	// ���ѹ�ʧ
		WORD_CARD_OVERDUE,	// ���ѹ���
		WORD_CARD_UNREGIST,	// δע�Ῠ

		WORD_PHONE_NUM_DOT_EXIST,	// ���Ų����ڣ����������� 
		WORD_HELP_PASSWORD,	// ����0���� �˸�(*) ȷ��(#)

		WORD_HELP_CALL_USER,	//��#����������
		WORD_HELP_CALL_CENTER,	//��#������
		WORD_HELP_LEAVE_WORD,	//��  �ԣ�����0��,�������Ĳ�������
		WORD_HELP_HAND_UP,		//��  ������#��

		WORD_INSERT_UNLOCK_PASSWD,	//�����뿪������
		WORD_SETTING_PRESS_KEY,		//�밴��Ӧ��ѡ��
		WORD_SETTING_USER,			//�û�����
		WORD_SETTING_LOCAL,			//��������
		WORD_SETTING_SYSTEM,		//ϵͳ����
		WORD_SETTING_PASSWORD,		//��������
		WORD_SETTING_ACCESS,		//�Ž�����
		WORD_SETTING_ELEVATOR,		//��������
		WORD_SETTING_REBOOT,		//��������
		WORD_SETTING_DEVTYPE,		//���û���
		WORD_SETTING_RETURN,		//�����밴#��

		WORD_CLEAR_ALL,		//���
		WORD_FIND,			//��ѯ
		WORD_RECORD_NUM,	//��¼��
		WORD_ROOM_NUM,		//�������
		WORD_IP,			//IP��ַ
		WORD_HELP_EDIT_IP,		//ѡ��(*)����0(.)ɾ��(#)
		WORD_HELP_BUTTON,	//ѡ��(*)ȷ��(#)
		WORD_ROOM_NUM_SORT,	//����
		WORD_EXSIT,			//�Ѵ���
		WORD_NOT_EXSIT,		//������
		WORD_PLEASE_INSERT_IP,//������IP
		WORD_SUCCESS,		// �ɹ�
		WORD_FAILED,		// ʧ��
		WORD_CAN_NOT_DELETE_CENTER,		// ����ɾ����������
		WORD_NET_SETTING,		// �����������
		WORD_SAVE,		// ����
		WORD_GET_ID,		// ��ȡID
		WORD_WRONG_FORMATE,		// ��ʽ����
		WORD_FRAME_PERIOD,		// ֡����
		WORD_ALRM_DOOR,		// ���ű���
		WORD_ENABLE_ALRM_DOOR,		// ʹ�ܿ��ű���
		WORD_ENABLE_PASSWORD,		// �������뿪��
		WORD_HELP_EDIT,		//ѡ��(*)ɾ��(#)
		WORD_HELP_MENU,		//ѡ��(*)����(8)����(0)ѡ��(#)
		WORD_SETTING,		//����
		WORD_PLEASE_INSERT_SIX_PASSWORD,		//ע�⣺��Ҫ����6λ����
		WORD_NEW_SYSTEM_PASSWORD,		// �¹�������
		WORD_NEW_SYSTEM_PASSWORD_CONFIRM,		// �¹�������ȷ��
		WORD_NEW_UNLOCK_PASSWORD,		// �¿�������
		WORD_NEW_UNLOCK_PASSWORD_CONFIRM,		// �¿�������ȷ��
		WORD_SYSTEM_PASSWORD,		// ��������
		WORD_UNLOCK_PASSWORD,		// ��������
		WORD_CARD_NUM,		// ����
		WORD_CARD_TYPE,		// ������
		WORD_CARD_TYPE_OWNER,		// ס����
		WORD_CARD_TYPE_MANAGE,		// ��ܿ�
		WORD_CARD_TYPE_TEMP,		// ��ʱ��
		WORD_CARD_TYPE_INVALID,		// ��Ч��
		WORD_PLEASE_INSERT_CARD_NUM,// �����뿨��

		WORD_HOUR,// ʱ
		WORD_MINUTE,// ��
		WORD_HELP_SYNC_TIME,// ��ͬ���������޸�ʱ��
		WORD_ELEVATOR_ADDRESS,// ���ݵ�ַ
		WORD_FLOOR_NUM,// ¥���
		WORD_ENABLE_ELEVATOR,// �Ƿ����õ�������
		WORD_SINGLE_VERSION, // ��Ƭ���汾
		WORD_HELP_TOPMESSAGE, // ����(*)ȷ��(#)
		WORD_CONFIRM_CLEAR_ALL_IP, //ȷ�����ȫ���û�������
		WORD_CONFIRM_CLEAR_ALL_CARD, //ȷ�����ȫ���û���������
		WORD_HELP_EDIT_ELEVATOR, //ѡ��(*)����0(-)ɾ��(#)
		WORD_DOOR_OPEN, //���Ѵ�
		WORD_DOOR_CLOSED, //���ѹر�
		WORD_DOOR_OPEN_TOO_LONG, //�ű���ʱ�����!
		WORD_ENABLE_UNBROKEN,	// ���÷��𱨾�
		WORD_LEAVE_MESSAGE_ING,	// ��������
		WORD_YEAR_ENC,//�� (������)
		WORD_MONTH_ENC,//�� (������)
		WORD_DATE_ENC,//�� (������)
		WORD_ELEVATOR_LOBBY,//���ú�
		WORD_UPDATE_CARD,//���ڸ��¿����ݣ�����ϵ�
		WORD_UPDATE_CARD_SUCCESS,//�����ݸ������,���Ժ�
		WORD_UPDATE_CARD_FAIL,//�����ݸ���ʧ�ܣ�����ϵ����Ա
		WORD_UPDATE_USER,//���ڸ����û����ݣ�����ϵ�
		WORD_UPDATE_USER_SUCCESS,//�û����ݸ������,���Ժ�
		WORD_UPDATE_USER_FAIL,//�û����ݸ���ʧ�ܣ�����ϵ����Ա
		WORD_CONTACT_LIST,		//ͨѶ¼
		WORD_USERSEARCH_TITLE,	//户主查询界面标题信息
		WORD_USERSEARCH_HELP,	//户主查询界面帮助信息
		WORD_UNIT,				//��Ԫ
		WORD_NO_UNIT,			//�޵�Ԫ

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

