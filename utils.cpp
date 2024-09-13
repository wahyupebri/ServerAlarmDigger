#define _CRT_SECURE_NO_WARNINGS
#define _CRT_NON_CONFORMING_SWPRINTFS
#define _WINSOCK_DEPRECATED_NO_WARNINGS
#include <WinSock2.h>
#include <ws2tcpip.h>
#include <iphlpapi.h>
#include <stdlib.h>
#include <stdio.h>
#include <iostream>
#include <curl/curl.h>
#include <chrono>
#include <time.h>
#include "data.h"
#include "utils.h"
#include <math.h>
const char* RESTIP = "http://127.0.0.1/travelwarning/";
//typedef struct data{}device;

using namespace std;
int generateErrorCode(int code[], int len) {
	int flags = (code[0] << 4) | (code[1] << 2) | code[2];
	return flags;
}

size_t curlCallback(void* ptr, size_t size, size_t nmemb, char** userdata) {
    size_t total = size * nmemb;
    *userdata = (char*)realloc(*userdata, total + 1);
    memcpy(*userdata, ptr, total);
    (*userdata)[total] = '\0';
    //printf("%s\n", *userdata);
    return total;
}
int curl(device * pDev, char* link) {
    CURL* curl=NULL;
    CURLcode res;
    char* buffCurl = NULL;

    if (curl) {

        /* example.com is redirected, so we tell libcurl to follow redirection */

        struct curl_slist* list = NULL;
        list = curl_slist_append(list, "Connection: keep - alive");
        list = curl_slist_append(list, "Content-Length: 0");
        curl_easy_setopt(curl, CURLOPT_HTTPHEADER, list);
        curl_easy_setopt(curl, CURLOPT_URL, link);

        curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, 0L);
        curl_easy_setopt(curl, CURLOPT_SSL_VERIFYHOST, 0L);

        curl_easy_setopt(curl, CURLOPT_TCP_KEEPALIVE, 1L);
        curl_easy_setopt(curl, CURLOPT_TIMEOUT, TIMEOUT);
        curl_easy_setopt(curl, CURLOPT_CONNECTTIMEOUT, TIMEOUT - 1);

        curl_easy_setopt(curl, CURLOPT_SSL_OPTIONS, (long)CURLSSLOPT_AUTO_CLIENT_CERT);
        //curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, curlCallback);
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, &buffCurl);
        /* Perform the request, res will get the return code */
        res = curl_easy_perform(curl);
        /* Check for errors */
        if (res != CURLE_OK) {
            fprintf(stderr, "[ERROR] %d was failed to query data, %s\n", pDev->name,
                curl_easy_strerror(res));
            //hauler->countFails++;
            return -1; //**WAJIB
        }
    }
    /*char data[50];
    if (parsingResult(buffCurl, data) > 0)
    {
        //postData(&data, hauler->name);
        //printf("INFO: %s Success\n", hauler->name);
        free(buffCurl);
    }*/
    return 1;
}

int parsingResult(char* temp, int* result, const char *checker,const char terminatechar) {
    //char fake[] = "{\"RealTimeParam\":{\"FeatureTagName\":\"\",\"Param\":[{\"UID64\":\"14354522324547796997d\",\"ParamName\":\"LiveWeight\",\"ParamValue\":\"16000.35\",\"ParamType\":\"value\",\"UnitID\":\"0022\",\"UnitSystem\":\"SI\"},{\"UID64\":\"45317475607904261d\",\"ParamName\":\"PayloadState\",\"ParamValue\":\"123\",\"ParamType\":\"state\",\"UnitID\":\"\",\"UnitSystem\":\"SI\"},{\"UID64\":\"14353677898921213957d\",\"ParamName\":\"BodyState\",\"ParamValue\":\"102\",\"ParamType\":\"state\",\"UnitID\":\"\",\"UnitSystem\":\"SI\"},{\"UID64\":\"14353677898937532421d\",\"ParamName\":\"Inclination\",\"ParamValue\":\"103.5\",\"ParamType\":\"value\",\"UnitID\":\"000A\",\"UnitSystem\":\"SI\"},{\"UID64\":\"14353677898920689669d\",\"ParamName\":\"Wheel-BasedVehicleSpeed\",\"ParamValue\":\"104\",\"ParamType\":\"value\",\"UnitID\":\"0004\",\"UnitSystem\":\"SI\"}]}}";
    //char* pXml = fake;
    //printf("%s\n", temp);
    char* pXml = temp;
    char* position_ptr = NULL;
    
        //printf("xml %s \n", pfake);
        position_ptr = strstr(pXml, checker);
        if (!position_ptr){
            *result = 2;
            return *result;
        }
        position_ptr = position_ptr + strlen(checker);
        char value[20]="";
        char* pValue = value;
        int cValue = 0;
        while (*position_ptr != terminatechar) {
            *pValue = *position_ptr;
            pValue++;
            position_ptr++;
            cValue++;
        }

        *pValue = '\0';
        pXml = temp;
        pXml = position_ptr;
        pValue = pValue - cValue;
        //printf("Hasil parsing %s\n", value);
        if (value[0] == '1') {
            *result = 1;
            return 1;
        }
        else if (value[0] == '0') {
            *result = 0;
            return 0;
        }
        return 2;
}
int parsingResult_(char* temp, int* result, const char* checker, const char terminatechar) {
    //char fake[] = "{\"RealTimeParam\":{\"FeatureTagName\":\"\",\"Param\":[{\"UID64\":\"14354522324547796997d\",\"ParamName\":\"LiveWeight\",\"ParamValue\":\"16000.35\",\"ParamType\":\"value\",\"UnitID\":\"0022\",\"UnitSystem\":\"SI\"},{\"UID64\":\"45317475607904261d\",\"ParamName\":\"PayloadState\",\"ParamValue\":\"123\",\"ParamType\":\"state\",\"UnitID\":\"\",\"UnitSystem\":\"SI\"},{\"UID64\":\"14353677898921213957d\",\"ParamName\":\"BodyState\",\"ParamValue\":\"102\",\"ParamType\":\"state\",\"UnitID\":\"\",\"UnitSystem\":\"SI\"},{\"UID64\":\"14353677898937532421d\",\"ParamName\":\"Inclination\",\"ParamValue\":\"103.5\",\"ParamType\":\"value\",\"UnitID\":\"000A\",\"UnitSystem\":\"SI\"},{\"UID64\":\"14353677898920689669d\",\"ParamName\":\"Wheel-BasedVehicleSpeed\",\"ParamValue\":\"104\",\"ParamType\":\"value\",\"UnitID\":\"0004\",\"UnitSystem\":\"SI\"}]}}";
    //char* pXml = fake;
    //printf("%s\n", temp);
    char* pXml = temp;
    char* position_ptr = NULL;

    
    position_ptr = strstr(pXml, checker);
    if (!position_ptr) {
        *result = 2;
        return *result;
    }
    position_ptr = position_ptr + strlen(checker);
    char value[20]="";
    char* pValue = value;
    int cValue = 0;
    while (*position_ptr != terminatechar) {
        *pValue = *position_ptr;
        pValue++;
        position_ptr++;
        cValue++;
    }

    *pValue = '\0';
    pXml = temp;
    pXml = position_ptr;
    pValue = pValue - cValue;
    //printf("Hasil parsing %s\n", value);
    if (value[0] == '\"') {
        *result = 1;
        return 1;
    }
    return 2;
}
int postData(char* link) {
    CURL* curl;
    CURLcode res;
    /* In windows, this will init the winsock stuff */
    curl_global_init(CURL_GLOBAL_ALL);

    /* get a curl handle */
    curl = curl_easy_init();
    if (curl) {
        curl_easy_setopt(curl, CURLOPT_URL, RESTIP);
        curl_easy_setopt(curl, CURLOPT_POSTFIELDS, link);

        /* Perform the request, res will get the return code */
        res = curl_easy_perform(curl);
        /* Check for errors */
        if (res != CURLE_OK)
            fprintf(stderr, "[ERROR] Utils %s, %s\n", link,
                curl_easy_strerror(res));

        /* always cleanup */
        curl_easy_cleanup(curl);
    }
    curl_global_cleanup();
    curl = NULL;
    link = NULL;
    //free(res);
    return 0;

}
int postData(device* pDev, char *link) {
    CURL* curl;
    CURLcode res;
    /* In windows, this will init the winsock stuff */
    curl_global_init(CURL_GLOBAL_ALL);

    /* get a curl handle */
    curl = curl_easy_init();
    if (curl) {
        curl_easy_setopt(curl, CURLOPT_URL, RESTIP);
        curl_easy_setopt(curl, CURLOPT_POSTFIELDS, link);

        /* Perform the request, res will get the return code */
        res = curl_easy_perform(curl);
        /* Check for errors */
        if (res != CURLE_OK)
            fprintf(stderr, "[ERROR] Utils %d failed to post, %s\n", pDev->name,
                curl_easy_strerror(res));

        /* always cleanup */
        curl_easy_cleanup(curl);
    }
    curl_global_cleanup();
    curl = NULL;
    link = NULL;
    //free(res);
    return 0;

}

long long getTimeInMillis() {
    using namespace chrono;
    auto duration = system_clock::now().time_since_epoch();
    long long millis = duration_cast<milliseconds>(duration).count();
    return millis;
}
int waitForMillis(long long t) {
    auto starttime = getTimeInMillis();
    for (;;) {
        if (getTimeInMillis() - starttime >= t)
            return 0;
    }
}
double convertCharToDouble(char* c) {
    double temp = strtod(c, NULL);
    //printf("Hasil conversi %f\n",temp);
    return temp;
}
void convertIntToCharArray(int i,char* c, int len) {
    //memset(c,len,0);
    sprintf(c, "%d", i);
    //printf("Hasil conversi %s %d\n",c,i);
    return;
}
double isValidDistance(position* first, position* second) {
    double x_ = 0;
    double y_ = 0;
    x_ = second->x-first->x;
    y_ = second->y - first->y;
    double d = sqrt((x_ * x_) + (y_ * y_));
    if (VALIDMAX < d) {
        return 0;
    }
    else
        return d;
}
int isNotMoving(position* first, position* second,int *limit) {
    double x_ = 0;
    double y_ = 0;
    x_ = second->x - first->x;
    y_ = second->y - first->y;
    double d = sqrt((x_ * x_) + (y_ * y_));
    if (*limit > d) {
        return 1;
    }
    else
        return 0;
}