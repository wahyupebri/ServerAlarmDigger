#ifndef UTILS_H
#define UTILS_H
//#include "data.h"
const long TIMEOUT = 5L;
const int VALIDMAX = 100;//maximum perpindahan antara 2 titik yang masih dikatakan masuk akal
long long getTimeInMillis();
int postData(device* pDev, char* link);
int postData(char* link);
int parsingResult(char* temp, int* result, const char* checker, const char terminatechar);
int parsingResult_(char* temp, int* result, const char* checker, const char terminatechar);
void convertIntToCharArray(int i, char* c, int len);
int generateErrorCode(int code[], int len);
double convertCharToDouble(char* c);
double isValidDistance(position* first, position* second);
int isNotMoving(position* first, position* second, int* limit);
#endif