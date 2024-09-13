#ifndef DATA_H
#define DATA_H
const int MACHINEMAXNUMBER = 10; //maximum jumlah mesin
const int RELAYNO = 2;//nomor relay, default 0
const int DELETEOLDERTHAN = 20;//minutes
const int MAXALARMDURATION = 3600;//detik
typedef struct data {
	char ip[20];
	int name;
	int alarmStatus; //default 0
	double totalDistance;//total perpindahan dalam 10 menit, default 0
	int isOnline;//default 0
	int relayNo;//lihat RELAYNO
	int isMaxDistance;//deafult 0
	int BIAS;//meter
	int MAXTIME;//minutes
	int MAXDISTANCE;//meter
	INT64 timestamp;//alarm activation duration [UPDATE Sep 2024]
}device;
typedef struct dataPositionFromMySQL {
	int seq;
	double x;
	double y;
}position;
#endif
