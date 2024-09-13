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
#include "data.h"
#include "adam.h"
#include "utils.h"
#include <string.h>
const char* DIGITALOUTPUTACTIVATE[] = { "DO1 = 1","DO2 = 1","DO3 = 1","DO4 = 1" };//nomor relay
const char* AUTHORIZATION = "cm9vdDowMDAwMDAwMA==";
size_t curlGetStatusCallback(void* ptr, size_t size, size_t nmemb, char** userdata) {
    size_t total = size * nmemb;
    *userdata = (char*)realloc(*userdata, total + 1);
    memcpy(*userdata, ptr, total);
    (*userdata)[total] = '\0';
    //printf("%s\n", *userdata);
    return total;
}
int getStatusAdamRelay(char* ip, int relaynumber) {
    CURL* curl;
    CURLcode res;
    char* buffCurl = NULL;
    //printf("Alamat 1 %p \n", buffCurl);
    int result = 2;//means unable to connect as default
    char link[80]="http://";
    strcat(link, ip);
    strcat(link, "/digitaloutput/");
    char convert[3];
    convertIntToCharArray(relaynumber, convert, 3);
    strcat(link, convert);
    strcat(link, "/value");
    
    /* In windows, this will init the winsock stuff */
    curl_global_init(CURL_GLOBAL_ALL);

    /* get a curl handle */
    curl = curl_easy_init();
    if (curl) {

        char auth_header[100];
        snprintf(auth_header, sizeof(auth_header), "Authorization: Basic %s", AUTHORIZATION);
        struct curl_slist* headers = NULL;
        headers = curl_slist_append(headers, auth_header);
        curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);
        curl_easy_setopt(curl, CURLOPT_URL, link);
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, curlGetStatusCallback);
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, &buffCurl);
        curl_easy_setopt(curl, CURLOPT_TCP_KEEPALIVE, 1L);
        curl_easy_setopt(curl, CURLOPT_TIMEOUT, TIMEOUT);
        curl_easy_setopt(curl, CURLOPT_CONNECTTIMEOUT, TIMEOUT - 1);
        /* Perform the request, res will get the return code */
        res = curl_easy_perform(curl);
        /* Check for errors */
        if (res != CURLE_OK)
            fprintf(stderr, "[ERROR] Adam GET %s, %s\n",ip,
                curl_easy_strerror(res));

        /* always cleanup */
        curl_easy_cleanup(curl);
        if(buffCurl){
        //printf("Alamat 2 %p \n", buffCurl);
        result = parsingResult(buffCurl, &result, "<VALUE>", '<');
        }
        if (buffCurl) {
            free(buffCurl);
        }
        //}
    }
    curl_global_cleanup();
    curl = NULL;
    //printf("LINK %s\n",link);
    return result;
}
int setRelayOn(char* ip,int relaynumber) {
	//activate relay
    CURL* curl;
    CURLcode res;
    char* buffCurl = NULL;
    char link[80] = "http://";
    strcat(link, ip);
    strcat(link, "/digitaloutput/all/value");
    int result = 2;
    /* In windows, this will init the winsock stuff */
    curl_global_init(CURL_GLOBAL_ALL);

    /* get a curl handle */
    curl = curl_easy_init();
    if (curl) {

        char auth_header[100];
        snprintf(auth_header, sizeof(auth_header), "Authorization: Basic %s", AUTHORIZATION);
        struct curl_slist* headers = NULL;
        headers = curl_slist_append(headers, auth_header);
        headers = curl_slist_append(headers, "Content-Type: application/x-www-form-urlencoded");
        curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);
        //curl_easy_setopt(curl, CURLOPT_POST, 1L);
        curl_easy_setopt(curl, CURLOPT_URL, link);
        curl_easy_setopt(curl, CURLOPT_POSTFIELDS, DIGITALOUTPUTACTIVATE[relaynumber]);
        curl_easy_setopt(curl, CURLOPT_TCP_KEEPALIVE, 1L);
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, curlGetStatusCallback);
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, &buffCurl);
        curl_easy_setopt(curl, CURLOPT_TIMEOUT, TIMEOUT);
        curl_easy_setopt(curl, CURLOPT_CONNECTTIMEOUT, TIMEOUT - 1);
        /* Perform the request, res will get the return code */
        res = curl_easy_perform(curl);
        /* Check for errors */
        if (res != CURLE_OK)
            fprintf(stderr, "[ERROR] Adam POST %s, %s\n", ip,
                curl_easy_strerror(res));

        /* always cleanup */
        curl_easy_cleanup(curl);
        if (buffCurl) {
            result = parsingResult_(buffCurl, &result, "OK", '>');
        }
        if (buffCurl) {
            free(buffCurl);
        }
        
    }
    curl_global_cleanup();
    curl = NULL;
    //printf("LINK %s\n", link);
    return result;
}