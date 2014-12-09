/* Copyright notice

 Copyright  2014 Longxiang Wang
 wanglongxiang@stu.xjtu.edu.cn
 Developed at the University of Xi'an Jiaotong University.

 This file is part of Impressions.

 Impressions is free software: you can redistribute it and/or modify
 it under the terms of the GNU General Public License as published by
 the Free Software Foundation, either version 3 of the License, or
 (at your option) any later version.

 Impressions is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 GNU General Public License for more details.

 You should have received a copy of the GNU General Public License
 along with Impressions.  If not, see <http://www.gnu.org/licenses/>.

 GNU General Public License in file named COPYING
 Modified Date 11.23.2013
 */
/*

 * DedupFileGenerator.h
 *
 *  Created on: 2013-10-4
 *      Author: wlx
 */

#ifndef DEDUPFILEGENERATOR_H_
#define DEDUPFILEGENERATOR_H_
#include <iostream>
#include <list>
#include "stat.h"
#include <string>
#include <set>
#include <map>
//#include "sqlite3.h"
//#include "MysqlOperation.h"
#include "leveldb/db.h"
#include <queue>

using namespace std;
using namespace leveldb;

struct chunkInfo {
	int size;
	string md5Value;
};

//创建一个list容器的实例LISTINT

typedef vector<chunkInfo> VectorChunkInfo;

class DedupChunkInfoGenerator {
private:
	long capacity;
	long accumulateSize;
	long realSize;
	long dedupSize;
	VectorChunkInfo chunkInfoVector;
	Random rand;
	int minChunkSize;
	int maxChunkSize;
	//size distribution param, generalized extreme value distribution
	double k;
	double sigma;
	double mu;
	//dedup chunk number distribution param, gamma distribution
	double a; //location
	double b; //scale
	double c; //shape
	VectorChunkInfo::iterator chunkInfoIter;
	/*sqlite3 * db;
	 string sql;
	 char *zErrMsg;
	 sqlite3_stmt *selectStmt;*/
	//MysqlOperation mysqlOp;
	int ret;
	int sizefd;
	int dedupCountfd;

	DB *db;
	set<string> tokenSet;
	set<string>::iterator tsIter;
	queue <string> tokenQueue;
	int queueSize;
	//token map
	map<string,string> tokenMap;
	map<string ,string >::iterator tmap_it;
	leveldb::Iterator * db_it;

public:
	DedupChunkInfoGenerator();
	DedupChunkInfoGenerator(Random rand);
	void generateDedupChunkList();
	string generateChunkContent(int size);
	chunkInfo getNextChunkInfo();
	int getMinChunkSize();
	string getChunkContent(string hash);
	char * getChunkContent();
	void closeDB();
	void writeDedupInfo();

	void openDB();
	void readTokenMapToDB();
	string getTokenBlock();
	char getSpace();
	string generateTokenBlock();
	void loadTokenToMap();

	// added to generate Zipf-Mandelbort distribution
	long ZipfMandelbrot(long N, double q, double s);
	double getH(long N, double q, double s);

private:
	string getString(const int n);
};

#endif /* DEDUPFILEGENERATOR_H_ */
