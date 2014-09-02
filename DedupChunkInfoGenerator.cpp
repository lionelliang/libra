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
#include "stat.h"

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
	/*
	 mode_t mode = S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH;
	 char strerr[100];
	 string fileName = "result/size.txt";
	 if ((sizefd = pos_creat(fileName.data(), mode)) < 0) {
	 strerror_r(errno, strerr, 100);
	 print_debug(1, "Error: Unable to create file= %s %d\n", fileName.data(),
	 errno);
	 }
	 fileName.clear();
	 fileName = "result/dedupCount.txt";
	 if ((dedupCountfd = pos_creat(fileName.data(), mode)) < 0) {
	 strerror_r(errno, strerr, 100);
	 print_debug(1, "Error: Unable to create file= %s %d\n", fileName.data(),
	 errno);
	 }*/
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
	//int randNum = rand.uniformDiscrete(1, 100);
	string tokenBlock;
/*	bool readFromQueue = false;
	int size = tokenQueue.size();
	if (size >= 1000) {
		readFromQueue = true;
	}
	if (randNum > 70 && (!tokenQueue.empty())) {
		readFromQueue = true;
	}*/
	tokenBlock = generateTokenBlock();
	/*	readFromQueue=false;
	 //cout<<"queue size is: "<<size<<"\t"<<readFromQueue<<endl;
	 if(readFromQueue) {

	 tokenBlock=tokenQueue.front();
	 tokenQueue.pop();
	 int size=tokenBlock.size();
	 realSize+=size;
	 }else {

	 tokenBlock=generateTokenBlock();
	 //tokenQueue.push(tokenBlock);
	 }*/
	return tokenBlock;

}
string DedupChunkInfoGenerator::generateTokenBlock() {
	/*	double gammaA = 0.167673;
	 double gammaB = 239383;
	 double a = 0;
	 double b = gammaB;
	 double c = gammaA;*/
	//this paramter is for all the linux token
	double words_mu = 6.09101;
	double words_sigma = 3.82877;
	double lgValue;
	stringstream ss;
	string value("");
//double gammaValue = rand.gamma(a, b, c);
//gammaValue = ceil(gammaValue);
	//int tokenNUm = rand.uniformDiscrete(100, 200);
	int tokenNum=20;
	for (int i = 0; i < tokenNum; i++) {
		lgValue = rand.lognormal(a, words_mu, words_sigma);
		lgValue = floor(lgValue)+1;
		string valueStr;
		valueStr = getString(lgValue);
		/*		Slice key = valueStr;
		 Status status = db->Get(ReadOptions(), key, &value);
		 if (!status.ok()) {
		 continue;
		 }*/
		/*the tokenMap holds the key-value entry.key is the token number.
		*value is the token string.*/
		tmap_it = tokenMap.find(valueStr);
		if (tmap_it == tokenMap.end()) {
			/*cout << "error,the token not found in token map" << endl;
			exit(-1);*/
			//cout<<valueStr<<" not found"<< endl;
			i--;
			continue;
		} else {
			value = tmap_it->second;
		}
		ss << value << " ";
/*		if (i % 20 == 0) {
			ss << "\n";
		}*/
		//} while (value.empty());
		//cout << value << endl;
		//caculate the dedup ratio
		/*		tsIter = tokenSet.find(value);
		 //cout << "find is dedup" << endl;
		 if (tsIter == tokenSet.end()) {
		 tokenSet.insert(value);
		 int tokenLen = value.length();
		 realSize += tokenLen;
		 dedupSize += tokenLen;
		 } else {
		 realSize += value.length();
		 }*/
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
//sqlite3_close(db);
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
	assert(status.ok());
}
void DedupChunkInfoGenerator::readTokenMapToDB() {

//ifstream in("./tokenData/token_list_hadoop.txt");
	ifstream in("./tokenData/token_list.txt");
	if (!in.is_open()) {
		cout << "Error opening file";
		exit(1);
	}
//WriteOptions wOptions=WriteOptions();
	while (!in.eof()) {
		//cout<<i++<<endl;
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
	//s << "dedup ratio is:" << dedupRatio << endl;
	/*s<<"real size is :"<<realSize/1024/1024<<"m"<<endl;
	 s<<"after dedup size is :"<<dedupSize/1024/1024<<"m"<<endl;*/
	//s << "real size is :" << realSize << "byte" << endl;
	//s << "after dedup size is :" << dedupSize << "byte" << endl;
	string str = s.str();
	pos_write(fd, str.data(), str.size());
	pos_close(fd);
	pos_close(sizefd);

}

