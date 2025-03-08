COMP64_REP = C:\Programs\mingw-w64\x86_64-8.1.0-posix-seh-rt_v6-rev0\mingw64\lib
COMP32_REP = C:/Programs/MinGW/lib

INC_REP = C:/PROJECTS_CROSS/NetProg/include

OBJ32= ../x86/obj/Card.o ../x86/obj/EMVError.o ../x86/obj/EMVTools.o ../x86/obj/EMVConnect.o ../x86/obj/RouterConnect.o
OBJ64= ../x64/obj/Card.o ../x64/obj/EMVError.o ../x64/obj/EMVTools.o ../x64/obj/EMVConnect.o ../x64/obj/RouterConnect.o


../x86/obj/%.o: ../../../../src/%.c
	g++ -c $< -DYY_USE_PROTOS -DWINDOWS -I$(INC_REP) -Wno-write-strings -o  $@

../x86/obj/main.o: ../../../../src/main.cpp
	g++ -c ../../../../src/main.cpp -DYY_USE_PROTOS -DWINDOWS -I$(INC_REP) -Wno-write-strings -o ../x86/obj/main.o

../x64/obj/%.o: ../../../../src/%.c
	g++ -c $< -DYY_USE_PROTOS -DWINDOWS -I$(INC_REP) -Wno-write-strings -o  $@	

../x64/obj/main.o: ../../../../src/main.cpp
	g++ -c ../../../../src/main.cpp -DYY_USE_PROTOS -DWINDOWS -I$(INC_REP) -Wno-write-strings -o ../x64/obj/main.o

EMVClient32: ../x86/obj/main.o $(OBJ32)
	g++  -o ../x86/exe/EMVClient32 ../x86/obj/main.o $(OBJ32) -LC:/PROJECTS_CROSS/NetProg/system/WINDOWS/GNU/x86/lib -lNetProg32 -lws2_32  -lodbc32 -lwinscard

EMVClient64: ../x64/obj/main.o $(OBJ64)
	g++  -o ../x86/exe/EMVClient64 ../x64/obj/main.o $(OBJ64) -LC:/PROJECTS_CROSS/NetProg/system/WINDOWS/GNU/x64/lib -lNetProg64 -lws2_32  -lodbc32 -lwinscard

clean32:
	del ..\x86\obj\*.o 

clean64:
	del ..\x64\obj\*.o 	