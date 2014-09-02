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
 */
/*
 *
 *
 *  Created on: 2013-10-4
 *      Author: wlx
 */
#include "DedupChunkInfoGenerator.h"
#include "stat.h"
#include<sys/time.h>

int main(void) {
	DedupChunkInfoGenerator dciGenerator = DedupChunkInfoGenerator(	);
	dciGenerator.openDB();
	dciGenerator.readTokenMapToDB();
	dciGenerator.closeDB();
}
