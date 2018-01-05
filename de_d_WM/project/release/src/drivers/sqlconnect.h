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
    unsigned int Size;			//包长度
    unsigned int Error;			//必须为0
    unsigned int RecordCount;	//数据集记录总数
    unsigned int RecordSize;    //单条记录长度
    unsigned int FieldCount;	//字段名总数
    unsigned int HaveExternData;//是否有扩展型字段
}SQL_DATASETHEAD;

//字段名集
typedef struct
{
    char FieldName[16];	            //字段名，包含空结束符0
    char FieldType;		            //字段类型
    char Reserved1;		            //保留，必须为0
    unsigned short FieldSize;		//字段长度，必须4个字节的倍数
}SQL_DATASETFIELD;

enum {TYPE_CHAR,TYPE_INT,TYPE_FLOAT,TYPE_DATETIME,TYPE_BLOBFIELD};

//----------------------------------------------------------------------------
struct QueryPrivate;        //私有数据

typedef struct _TMyField
{
	struct QueryPrivate *Private;
    SQL_DATASETFIELD *Field;											//字段属性指针
    int Offset;															//偏移量
    char * (*AsChar)(struct _TMyField *This,char *Buf,int Size);		//作为字段型读取
    int (*AsBlobData)(struct _TMyField *This,char *Buf,int Size);    //作为二进制读取
    int (*AsInt)(struct _TMyField *This);								//作为整型读取
    double (*AsFloat)(struct _TMyField *This);							//作为浮点型读取
    char * (*AsDateTime)(struct _TMyField *This,char *Buf,int Size);	//作为日期型读取
} TMyField,*PField;
//----------------------------------------------------------------------------
#define QUERY_THIS struct _TMyQuery *This

typedef struct _TMyQuery
{
    struct QueryPrivate * Private;
    PField Fields;

	void (*Destroy)(QUERY_THIS);        //销毁

	void (*SetSrvIP)(QUERY_THIS,const char *pSrvIP);		//更改服务器IP地址
	BOOL (*IsConnected)(QUERY_THIS);		//是否连接
	BOOL (*Connect)(QUERY_THIS);			//连接
	BOOL (*DisConnect)(QUERY_THIS,BOOL ClearAll);		//断开

	int (*GetSQLText)(QUERY_THIS,char *pBuf,int Size);  //取SQL命令行
    void (*SetSQLText)(QUERY_THIS,char *SqlCmd);        //设置SQL命令行
    BOOL (*Active)(QUERY_THIS);			//Query是否激活(只读)
    BOOL (*Open)(QUERY_THIS);           //打开
    void (*Close)(QUERY_THIS);          //关闭
    BOOL (*ExecSQL)(QUERY_THIS);        //执行
    int (*RecordCount)(QUERY_THIS);		//记录数(只读)
    int (*FieldCount)(QUERY_THIS);		//字段数量(只读)

	void (*First)(QUERY_THIS);          //首记录
    void (*Last)(QUERY_THIS);           //末记录
    void (*Prior)(QUERY_THIS);          //上一记录
    void (*Next)(QUERY_THIS);           //下一记录
    void (*SetRecNo)(QUERY_THIS,int RecNo);       //跳到记录号
    int (*RecNo)(QUERY_THIS);          //返回记录号
    PField (*FieldByName)(QUERY_THIS,char *Name);       //返回字段
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