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
#include <string.h>
#include "DedupChunkInfoGenerator.h"
#include "md5.h"
#include <sstream>
//#include "stat.h"
#include <cmath>
#include <stdlib.h>

#include "leveldb/db.h"
#include "leveldb/options.h"

#include "strTools.h"
#include "posix_lib.h"
#include "impress.h"
#include <limits>

//#define DEBUG
using namespace leveldb;
DedupChunkInfoGenerator::DedupChunkInfoGenerator() {

}
DedupChunkInfoGenerator::DedupChunkInfoGenerator( Random rand
		) {
	this->minChunkSize = 512;
	this->maxChunkSize = 4 * 1024;

	this->a = 0;

	this->accumulateSize = 0;
	this->rand = rand;
	this->realSize = 0;
	this->dedupSize = 0;
	this->queueSize = 1000;
}

chunkInfo DedupChunkInfoGenerator::getNextChunkInfo() {

	chunkInfo tmpci;
	std::stringstream ss;
	string str;
	if (chunkInfoIter == chunkInfoVector.end()) {
		chunkInfoIter = chunkInfoVector.begin();
	}
	tmpci = *chunkInfoIter;
	chunkInfoIter++;
	str.clear();
	ss.str("");
	ss << tmpci.size << endl;
	str = ss.str();
	pos_write(sizefd, (char *) str.data(), str.length());
	return tmpci;
}
int DedupChunkInfoGenerator::getMinChunkSize() {
	return this->minChunkSize;
}
void DedupChunkInfoGenerator::loadTokenToMap() {
	db_it = db->NewIterator(leveldb::ReadOptions());
	for (db_it->SeekToFirst(); db_it->Valid(); db_it->Next()) {
		string key = db_it->key().ToString();
		string value = db_it->value().ToString();
		tokenMap.insert(pair<string,string>(key, value));
	}
}
string DedupChunkInfoGenerator::getTokenBlock() {
	string tokenBlock;
	tokenBlock = generateTokenBlock();
	return tokenBlock;

}
string DedupChunkInfoGenerator::generateTokenBlock() {

	long N = 50;
	double zipValue, q = 1.0, s = 2.0;

	stringstream ss;
	string value("");

	int tokenNum=20;
	for (int i = 0; i < tokenNum; i++) {
		//zipValue = ZipfMandelbrot(0.2627, -1.33, 0);	 	//old data
		zipValue = ZipfMandelbrot(0.4402, -2.388, 0);		//java data
		//zipValue = ZipfMandelbrot(0.2869, -1.446, 0);		//cdata data

		string valueStr;
		valueStr = getString(zipValue);
		tmap_it = tokenMap.find(valueStr);
		if (tmap_it == tokenMap.end()) {
			i--;
			continue;
		} else {
			value = tmap_it->second;
		}
		ss << value << " ";
	}
	ss<<"\n";
	return ss.str();
}
char DedupChunkInfoGenerator::getSpace() {
	int randNum = rand.uniformDiscrete(1, 100);
	char c;
	if (randNum > 80) {
		c = '\n';
	} else {
		c = ' ';
	}
	return c;
}
void DedupChunkInfoGenerator::closeDB() {
	delete db;
}
void DedupChunkInfoGenerator::openDB() {
	/**
	 * opens a database connection to "./leveldb_dir"
	 */
	Options options;
	options.create_if_missing = true;

	cout << "open leveldb" << endl;
	leveldb::Status status = leveldb::DB::Open(options, "./leveldb_dir", &db);

	if (!status.ok())
		cout << status.ToString() << endl;
	assert(status.ok());
}
void DedupChunkInfoGenerator::readTokenMapToDB() {

	//ifstream in("./tokenData/token_list.txt");
	ifstream in("./tokenData/token_list_java.txt");
	if (!in.is_open()) {
		cout << "Error opening file";
		exit(1);
	}
	while (!in.eof()) {
		string lineStr;
		getline(in, lineStr);
		if (lineStr.empty())
			break;
		vector<string> strVector = split(lineStr, "\t");
		string number = strVector[0];
		string token = strVector[1];
		cout << "number is " << number << "token is " << token << endl;
		db->Put(WriteOptions(), number, token);
	}

}

string DedupChunkInfoGenerator::getString(const int n)

{

	std::stringstream newstr;
	newstr << n;
	return newstr.str();

}
void DedupChunkInfoGenerator::writeDedupInfo() {
	double dedupRatio = realSize / dedupSize;
	mode_t mode = S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH;
	int fd = 0;
	char strerr[100];
	string fileName = "result/dedup_info.txt";
	if ((fd = pos_creat(fileName.data(), mode)) < 0) {
		strerror_r(errno, strerr, 100);
		print_debug(1, "Error: Unable to create file= %s %d\n", fileName.data(),
				errno);
	}
	std::ostringstream s;
	string str = s.str();
	pos_write(fd, str.data(), str.size());
	pos_close(fd);
	pos_close(sizefd);

}

/**
 *  y = a*x^b + c
 *  k = ((y-c)/a)^1/b
 */
long DedupChunkInfoGenerator::ZipfMandelbrot(double a, double b, double c) {

	double f = std::rand()/(double)RAND_MAX;
	double d = (f-c)/a;
	double temp = pow(d, 1.0/b);
	long zipfValue = temp  + 1;

	//cout << zipfValue << " \n";

	return zipfValue;
}
