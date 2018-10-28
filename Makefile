
pathInc = -I./
DLibs =

CC = clang++ $(CXXFLAGS) -c -Wall -std=c++14 -ferror-limit=100 $(pathInc)
LD = clang++ $(LDFLAGS) obj/*.o $(DLibs)

all : fodder

fodder: 	main
		mv *.o obj/
		$(LD) -o SOS_Unpacker

main:
		$(CC) SOS_Unpacker.cpp stdafx.cpp capsimg/CAPSImg/*.cpp capsimg/Codec/*.cpp capsimg/Core/*.cpp 
		mkdir -p obj

clean:
		rm obj/*.o


