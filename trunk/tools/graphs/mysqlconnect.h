//mysqlconnect.c
void TextConnectDb0(void);
void TextConnectDbUBC(void);
void ConnectToOptionalUBCDb(unsigned uDatacenter);
void logfileLine0(const char *cFunction,const char *cLogline,const unsigned uContainer);
MYSQL gMysql;
MYSQL gMysqlUBC;
char *gcUBCDBIP0=DBIP0;
char *gcUBCDBIP1=DBIP1;
char gcUBCDBIP0Buffer[32]={""};
char gcUBCDBIP1Buffer[32]={""};
FILE *gLfp0=NULL;
#define cUBCLOGFILE "/tmp/unxsGraphsUBC.log"
char gcQuery[1024];
