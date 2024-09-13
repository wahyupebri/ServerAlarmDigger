#ifndef MODEL_H
#define MODEL_H
#include <curl/curl.h>
#include <mysql/mysql.h>
#include "data.h"
int deleteData(int lastMinutes);
double getSQLTotalDistance(device* pDev);
/*messageId
1 error
2 warning
3 info
*/
void logSQL(device* pDev,int* name, const char* message, const int messageId, int statusMaxExceed, int statusConn, int statusDI);
int setSQLStatus(device *dev, int statusRelay);//status relay 1 on, 0 off & status connection 1 online, 0 offline
#endif