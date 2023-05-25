CC=g++ -std=c++11
FLAGS=-g -pedantic -Wall -Wextra -Wno-error
COMPILE=$(CC) $(FLAGS)
UEYE_INCLUDE=/opt/ids/ueye/include
OBJECTS_REC=obj/main_rec.o obj/ueye_camera.o obj/console.o
UEYE_DIRS=bin

all: rec

rec:$(OBJECTS_REC)
	mkdir $(UEYE_DIRS)
	$(COMPILE) $(OBJECTS_REC) -I$(UEYE_INCLUDE) -L/usr/lib -lueye_api -lpthread -o bin/record_video

obj/main_rec.o:main_rec.cpp ueye_camera.h console.cpp
	$(COMPILE) -c main_rec.cpp -I$(UEYE_INCLUDE) -L/usr/lib -lueye_api -lpthread -o obj/main_rec.o

obj/ueye_camera.o:ueye_camera.h ueye_camera.cpp
	$(COMPILE) -c ueye_camera.cpp -I$(UEYE_INCLUDE) -L/usr/lib -lueye_api -o obj/ueye_camera.o

obj/console.o:console.h console.cpp
	$(COMPILE) -c console.cpp -o obj/console.o

clean:
	-rm -r $(OBJECTS_REC) $(UEYE_DIRS) 
