CC = g++
CFLAGS = -g  -Wall 
#-pg
LFLAGS = -L./ -lm 
LEVELDBFLAGS= -lleveldb -lpthread
CFLAGS += -I./include/ 

all:	libra
libra: 	word.o get_spec.o extension.o posix_lib.o montecarlo.o impress.o  fragment.o depth.o ssp.o \
 			DedupChunkInfoGenerator.o md5.o strTools.o
	$(CC) $(CFLAGS) $(LFLAGS) word.o impress.o posix_lib.o get_spec.o extension.o montecarlo.o fragment.o depth.o ssp.o\
		 DedupChunkInfoGenerator.o md5.o strTools.o $(LEVELDBFLAGS) -o libra
loadToken:	posix_lib.o  strTools.o DedupChunkInfoGenerator.o\
 			loadToken.o 
	$(CC) $(CFLAGS) $(LFLAGS) posix_lib.o strTools.o\
		 loadToken.o  DedupChunkInfoGenerator.o $(LEVELDBFLAGS) -o loadToken
montecarlo.o:	montecarlo.cpp 
	$(CC) $(CFLAGS) -c montecarlo.cpp
get_spec.o:	get_spec.cpp 
	$(CC) $(CFLAGS) -c get_spec.cpp 
word.o:	word.cpp 
	$(CC) $(CFLAGS) -c word.cpp 
extension.o:	extension.cpp
	$(CC) $(CFLAGS) -c extension.cpp 
impress.o:	impress.cpp
	$(CC) $(CFLAGS) -c impress.cpp  
posix_lib.o:	posix_lib.cpp
	$(CC) $(CFLAGS) -c posix_lib.cpp 
fragment.o:	fragment.cpp
	$(CC) $(CFLAGS) -c fragment.cpp 
depth.o:	depth.cpp
	$(CC) $(CFLAGS) -c depth.cpp 
ssp.o:	ssp.cpp
	$(CC) $(CFLAGS) -c ssp.cpp  
DedupChunkInfoGenerator.o:	 DedupChunkInfoGenerator.cpp 
	$(CC) $(CFLAGS) -c DedupChunkInfoGenerator.cpp 
md5.o: md5.cpp
	$(CC) $(CFLAGS) -c md5.cpp
loadToken.o:	 loadToken.cpp
	$(CC) $(CFLAGS) -c loadToken.cpp 
strTools.o:		strTools.cpp
	$(CC) $(CFLAGS) -c strTools.cpp 
clean: 
	rm -f *.o  *.out *~ impressions libra loadToken
	rm -r impress_home
