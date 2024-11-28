// ServerAlarmDigger.cpp : This file contains the 'main' function. Program execution begins and ends there.
//
#define _CRT_SECURE_NO_WARNINGS
#include <iostream>
#include "adam.h"
#include "model.h"
#include "data.h"
#include "utils.h"
#include <stdio.h>
#include <thread>
#include <vector>
device devList[] = {
    {"10.4.25.185",6206,0,0,0,RELAYNO,0,30,15,250,0},
    {"10.4.25.145",6208,0,0,0,RELAYNO,0,30,15,250,0},
    {"10.4.25.105",6210,0,0,0,RELAYNO,0,30,15,250,0},
    {"10.4.25.45",6211,0,0,0,RELAYNO,0,30,15,250,0},
    {"10.4.25.50",6212,0,0,0,RELAYNO,0,30,15,250,0},
    {"10.4.34.175",6214,0,0,0,RELAYNO,0,30,15,250,0},
    {"10.4.25.195",6215,0,0,0,RELAYNO,0,30,15,250,0},
    {"10.4.25.150",6217,0,0,0,RELAYNO,0,30,15,250,0},
    {"10.4.25.30",6218,0,0,0,RELAYNO,0,30,15,250,0},
    {"10.4.25.35",6219,0,0,0,RELAYNO,0,30,15,250,0}
};
int threadMain(device *pDevice) {
    pDevice->totalDistance = getSQLTotalDistance(pDevice);
    INT64 duration = 0;
    if (pDevice->totalDistance > pDevice->MAXDISTANCE && !pDevice->alarmStatus) {//aktivasi relay jika distance > MAXDISTANCE dan alarm status masih tidak aktif
        if (pDevice->isMaxDistance < 1) {
            pDevice->isMaxDistance = 1;//set status telah max distance
            pDevice->timestamp = getTimeInMillis();//isi dengan timestamp
        }
        int status = setRelayOn(pDevice->ip, pDevice->relayNo);
        

        
        if (status == 1) {
            printf("[INFO] Main %d alarm on\n", pDevice->name);
            pDevice->isOnline = 1;//status online
            pDevice->alarmStatus = 1;//status alarm aktif
        }
        else {
            printf("[ERROR] Main %d exceed max distance but offline\n", pDevice->name);
            logSQL(pDevice, &pDevice->name, "Alarm failed to activate due to offline connection", 1, pDevice->isMaxDistance, pDevice->isOnline, getStatusAdamRelay(pDevice->ip, RELAYNO));
        }
        char alarmstatus[100] = "Elapsed time (seconds) after reaching max distance is ";
        char convert[10];
        duration = (getTimeInMillis() - pDevice->timestamp) / 1000;
        convertIntToCharArray(duration, convert, 6);
        strcat(alarmstatus,convert);
        logSQL(pDevice, &pDevice->name, alarmstatus, 3, pDevice->isMaxDistance, pDevice->isOnline, status);

    }
    else {
        pDevice->isMaxDistance = 0;
    }

    fprintf(stdout, "[INFO] Main Get status %d\n",pDevice->name);
    int relay = getStatusAdamRelay(pDevice->ip, pDevice->relayNo);//status relay yang ditentukan oleh RELAYNO

    if (relay == 2) {
        pDevice->isOnline = 0;//unable to connect
    }
    if (relay == 1) {
        pDevice->isOnline = 1;
        pDevice->alarmStatus = 1;//alarm on
        
    }
    if (relay == 0) {
        pDevice->isOnline = 1;
        pDevice->alarmStatus = 0;//alarm off
    }
    printf("[INFO] Main machine %d: relay is %d, connection is %d, distance is %f (%d)\n", pDevice->name, relay, pDevice->isOnline, pDevice->totalDistance, pDevice->isMaxDistance);
    setSQLStatus(pDevice, relay);
    //logSQL(pDevice, &pDevice->name, "Alarm failed to activate due to offline connection", 1, pDevice->isMaxDistance, pDevice->isOnline, relay);
    if (pDevice->timestamp > 0) {//aktivasi timer 60 menit setelah adam aktif baru bisa dimatikan [update Sep 2024]
        duration = (getTimeInMillis() - pDevice->timestamp) / 1000;
        printf("[INFO] Main %d alarm have already activated for  %lld secs\n", pDevice->name, duration);
        if (duration < MAXALARMDURATION) {
            setRelayOn(pDevice->ip, pDevice->relayNo);
        }
        else {
            pDevice->timestamp = 0;
            printf("[INFO] Main %d alarm have deactivated after  %lld secs\n", pDevice->name, duration);
        }
    }
    
    return 0;
}
int main()
{
    size_t sizeDevList = sizeof(devList);
    const int lenDevList = (int)(sizeDevList / sizeof(device));
    long long start;
    long long end;
    //loop forever
    while(1){
        start = getTimeInMillis();
        std::vector<std::thread*> threadJobs;
        for (int a = 0; a < lenDevList; a++) {
            //start thread
            threadJobs.push_back(new std::thread(threadMain, &devList[a]));
        }
        for (auto thread : threadJobs) {
            thread->join();
            delete thread;
        }
        end = getTimeInMillis();
        deleteData(DELETEOLDERTHAN);
        printf("[INFO] Main Duration = %llu\n", end-start);
    }//end loop
    return 0;
}

// Run program: Ctrl + F5 or Debug > Start Without Debugging menu
// Debug program: F5 or Debug > Start Debugging menu

// Tips for Getting Started: 
//   1. Use the Solution Explorer window to add/manage files
//   2. Use the Team Explorer window to connect to source control
//   3. Use the Output window to see build output and other messages
//   4. Use the Error List window to view errors
//   5. Go to Project > Add New Item to create new code files, or Project > Add Existing Item to add existing code files to the project
//   6. In the future, to open this project again, go to File > Open > Project and select the .sln file
