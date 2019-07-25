LDFLAG =-O2 -g -Wall -DLINUX -lpthread -l${P2P}_API -lstdc++ 
LDFLAG+=-I ../../Include/${P2P}/ 
LDFLAG+=-L ../../Lib/${OS}/${BOARD}

ifeq ($(BOARD), x64)
LDFLAG+=-m64 
exe=64
else
	ifeq ($(BOARD), x86)
		LDFLAG+=-m32 
		exe=
	endif
endif

ifeq ($(OS), Linux)
LDFLAG+=-Wl,-rpath=./
dylib=so
endif

ifeq ($(OS), osX)
dylib=dylib
endif

all: clean
	${CXX} ./ConnectionTester.cpp -o ConnectionTester${exe} ${LDFLAG} -s
	${CXX} ./ReadWriteTester.cpp -o ReadWriteTester${exe} ${LDFLAG} -s
	${CXX} ./ListenTester.cpp -o ListenTester_${P2P}${exe} ${LDFLAG} -s
	
	cp -rf ../../Lib/${OS}/${BOARD}/lib${P2P}_API.${dylib} ./
ifeq ($(OS), osX)
	rm -rf ./*.dSYM
endif

clean:
	rm -rf *.o *~ *.bak  2.7z
	rm -rf ./lib${P2P}_API.${dylib}
	rm -rf ConnectionTester  ReadWriteTester ListenTester_${P2P}
	rm -rf ConnectionTester64  ReadWriteTester64 ListenTester_${P2P}64 
