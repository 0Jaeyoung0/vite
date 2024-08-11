#컴파일러 지정
CC = gcc -std=c99
CFLAGS = -Wall -Wextra -g

#소스코드와 목적파일
SOURCES = main.c editor.c

OBJECTS = main.o editor.o

#최종결과물 이름
OUT = vite

#타겟(OUT)에 대한 (OBJECTS)의 종속성
$(OUT): $(OBJECTS)
	$(CC) -o $(OUT) $(OBJECTS) $(LIBS)

#각 소스 파일의 종속성 설정
main.o: main.c editor.h
	$(CC) -c main.c

editor.o: editor.c editor.h
	$(CC) -c editor.c

-include dependencies.mk

#make clean의 명령어 내용
clean:
	del $(OUT) $(OBJECTS)