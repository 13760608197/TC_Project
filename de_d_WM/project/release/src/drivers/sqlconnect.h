#ifdef WIN32

#ifndef SQLCONNECTH
#define SQLCONNECTH

#ifndef FALSE
    #define FALSE 0
    #define TRUE 1
    #define BOOL int
#endif
//----------------------------------------------------------------------------
typedef struct
{
    unsigned int Size;
    unsigned int Type;
}SQLCOMMUNICATION;

typedef struct
{
    unsigned int SqlType;
    unsigned int CmdSize;
}SQLREQUEST;

typedef struct
{
    unsigned int Size;
    int Error;
    unsigned int ErrMsgSize;
}SQL_ANSWER;

typedef struct
{
    unsigned int Size;			//������
    unsigned int Error;			//����Ϊ0
    unsigned int RecordCount;	//���ݼ���¼����
    unsigned int RecordSize;    //������¼����
    unsigned int FieldCount;	//�ֶ�������
    unsigned int HaveExternData;//�Ƿ�����չ���ֶ�
}SQL_DATASETHEAD;

//�ֶ�����
typedef struct
{
    char FieldName[16];	            //�ֶ����������ս�����0
    char FieldType;		            //�ֶ�����
    char Reserved1;		            //����������Ϊ0
    unsigned short FieldSize;		//�ֶγ��ȣ�����4���ֽڵı���
}SQL_DATASETFIELD;

enum {TYPE_CHAR,TYPE_INT,TYPE_FLOAT,TYPE_DATETIME,TYPE_BLOBFIELD};

//----------------------------------------------------------------------------
struct QueryPrivate;        //˽������

typedef struct _TMyField
{
	struct QueryPrivate *Private;
    SQL_DATASETFIELD *Field;											//�ֶ�����ָ��
    int Offset;															//ƫ����
    char * (*AsChar)(struct _TMyField *This,char *Buf,int Size);		//��Ϊ�ֶ��Ͷ�ȡ
    int (*AsBlobData)(struct _TMyField *This,char *Buf,int Size);    //��Ϊ�����ƶ�ȡ
    int (*AsInt)(struct _TMyField *This);								//��Ϊ���Ͷ�ȡ
    double (*AsFloat)(struct _TMyField *This);							//��Ϊ�����Ͷ�ȡ
    char * (*AsDateTime)(struct _TMyField *This,char *Buf,int Size);	//��Ϊ�����Ͷ�ȡ
} TMyField,*PField;
//----------------------------------------------------------------------------
#define QUERY_THIS struct _TMyQuery *This

typedef struct _TMyQuery
{
    struct QueryPrivate * Private;
    PField Fields;

	void (*Destroy)(QUERY_THIS);        //����

	void (*SetSrvIP)(QUERY_THIS,const char *pSrvIP);		//���ķ�����IP��ַ
	BOOL (*IsConnected)(QUERY_THIS);		//�Ƿ�����
	BOOL (*Connect)(QUERY_THIS);			//����
	BOOL (*DisConnect)(QUERY_THIS,BOOL ClearAll);		//�Ͽ�

	int (*GetSQLText)(QUERY_THIS,char *pBuf,int Size);  //ȡSQL������
    void (*SetSQLText)(QUERY_THIS,char *SqlCmd);        //����SQL������
    BOOL (*Active)(QUERY_THIS);			//Query�Ƿ񼤻�(ֻ��)
    BOOL (*Open)(QUERY_THIS);           //��
    void (*Close)(QUERY_THIS);          //�ر�
    BOOL (*ExecSQL)(QUERY_THIS);        //ִ��
    int (*RecordCount)(QUERY_THIS);		//��¼��(ֻ��)
    int (*FieldCount)(QUERY_THIS);		//�ֶ�����(ֻ��)

	void (*First)(QUERY_THIS);          //�׼�¼
    void (*Last)(QUERY_THIS);           //ĩ��¼
    void (*Prior)(QUERY_THIS);          //��һ��¼
    void (*Next)(QUERY_THIS);           //��һ��¼
    void (*SetRecNo)(QUERY_THIS,int RecNo);       //������¼��
    int (*RecNo)(QUERY_THIS);          //���ؼ�¼��
    PField (*FieldByName)(QUERY_THIS,char *Name);       //�����ֶ�
} TMyQuery;

TMyQuery * CreateQuery(char *Host,char *User,char *Password);

BOOL QueryOpen(TMyQuery *Query,char *SqlStr);
BOOL QueryExec(TMyQuery *Query,char *SqlStr);
char* QueryOfChar(TMyQuery *Query,char *FieldName,char *cBuf,int Size);
int QueryOfBlobData(TMyQuery *Query,char *FieldName,char *cBuf,int Size);
int QueryOfInt(TMyQuery *Query,char *FieldName);
double QueryOfFloat(TMyQuery *Query,char *FieldName);
char* QueryOfDateTime(TMyQuery *Query,char *FieldName,char *cBuf,int Size);
//----------------------------------------------------------------------------
#endif	//SQLCONNECTH
 
#endif	//WIN32