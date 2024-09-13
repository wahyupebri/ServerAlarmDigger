#define _CRT_SECURE_NO_WARNINGS
#include "model.h"
#include "utils.h"
#include "data.h"

double getSQLTotalDistance(device* pDev) {
	MYSQL *conn;
	MYSQL_RES* result;
	MYSQL_ROW row;
	conn = mysql_init(NULL);
	if (conn == NULL) {
		fprintf(stderr, "[ERROR] Model mysql_init() failed\n");
		return 0;
	}
	if (mysql_real_connect(conn, "localhost", "root", "", "tracking_digger", 0, NULL, 0) == NULL) {
		fprintf(stderr, "[ERROR] Model mysql_real_connect() failed\n");
		mysql_close(conn);
		return 0;
	}
	char query[200]="SELECT * from position where pos_machine=";
	char convert[6];
	convertIntToCharArray(pDev->name, convert, 5);
	strcat(query, convert);
	strcat(query, " AND pos_timestamp>=NOW()-INTERVAL ");
	convertIntToCharArray(pDev->MAXTIME, convert, 5);
	strcat(query, convert);
	strcat(query, " MINUTE");
	if (mysql_query(conn, query)) {
		fprintf(stderr,"[ERROR] Model Query failed,%s, %s\n",query,mysql_error(conn));
		mysql_close(conn);
		return 0;
	}
	
	result = mysql_store_result(conn);
	my_ulonglong rows = mysql_num_rows(result);
	fprintf(stdout, "[INFO] Model %d has total rows %llu\n", pDev->name,rows);
	position *arrayPosition= (position*)malloc(rows*sizeof(position));
	position* pPosition = arrayPosition;
	int counter = 0;
	while ((row = mysql_fetch_row(result))) {
		pPosition->seq = counter;
		pPosition->x = convertCharToDouble(row[3]);
		pPosition->y = convertCharToDouble(row[4]);
		counter++;
		pPosition++;
	}
	double total = 0;
	for (int a = 0; a < counter;a++) {
		if (a > 1) {
			double temp=isValidDistance(&arrayPosition[a-1], &arrayPosition[a]);
			if (temp > 0) {
				total = total + temp;
			}
		}
	}
	if (isNotMoving(&arrayPosition[0], &arrayPosition[counter - 1], &pDev->BIAS)) {
		total = isValidDistance(&arrayPosition[0], &arrayPosition[counter - 1]);
		fprintf(stdout, "[INFO] Model Success connect to MySQL, %d still in range %f\n", pDev->name, total);
	}
	else {
		total = total- pDev->BIAS;
		if (total < 0) {
			total = 0;
		}
	}
	mysql_free_result(result);
	free(arrayPosition);
	fprintf(stdout, "[INFO] Model Success connect to MySQL, %d is moving %f\n", pDev->name,total);
	mysql_close(conn);
	return total;
}
void logSQL(device *pDev,int* name, const char* message, const int messageId, int statusMaxExceed, int statusConn, int statusDI) {
	int arrStatus[] = {statusMaxExceed, statusConn, statusDI};
	int errCode = generateErrorCode(arrStatus, 3);
	//write to DB log
	char temp[150] = "";
	char convert[6];
	int len = sizeof(convert) / sizeof(char);
	strcat(temp, "option=logs");
	strcat(temp, "&machine=");
	convertIntToCharArray(*name, convert, len);
	strcat(temp, convert);
	strcat(temp, "&flags=");
	convertIntToCharArray(errCode, convert, len);
	strcat(temp, convert);
	strcat(temp, "&distance=");
	convertIntToCharArray((int)pDev->totalDistance, convert, len);
	strcat(temp, convert);
	strcat(temp, "&message=");
	strcat(temp, message);

	//printf("link %s\n", temp);
	postData(pDev, temp);
	return;
}
int setSQLStatus(device *dev, int statusRelay) {
	//curl rest
	char temp[80]="";
	char convert[6];
	int len = sizeof(convert) / sizeof(char);
	strcat(temp, "option=updateStatus");
	strcat(temp, "&machine=");
	convertIntToCharArray(dev->name, convert, len);
	strcat(temp, convert);
	strcat(temp, "&isonline=");//aktual koneksi
	convertIntToCharArray(dev->isOnline, convert, len);
	strcat(temp, convert);

	strcat(temp, "&isexceed=");//aktual perhitungan perpindahan
	convertIntToCharArray(dev->isMaxDistance, convert, len);
	strcat(temp, convert);

	strcat(temp, "&isalarm=");//aktual kondisi alarm di mesin
	convertIntToCharArray(dev->alarmStatus, convert, len);
	strcat(temp, convert);
	strcat(temp, "&distance=");//aktual total perpindahan
	convertIntToCharArray((int)dev->totalDistance, convert, len);
	strcat(temp, convert);
	postData(dev, temp);
	return 0;
}
int deleteData(int lastMinutes) {
	//curl rest
	char temp[80] = "";
	char convert[6];
	int len = sizeof(convert) / sizeof(char);
	strcat(temp, "option=deleteOld");
	strcat(temp, "&old=");
	convertIntToCharArray(DELETEOLDERTHAN, convert, len);
	strcat(temp, convert);
	//printf("link %s\n", temp);
	postData(temp);
	return 0;
}