#�����Ϸ� ����
CC = gcc -std=c99
CFLAGS = -Wall -Wextra -g

#�ҽ��ڵ�� ��������
SOURCES = main.c editor.c

OBJECTS = main.o editor.o

#��������� �̸�
OUT = vite

#Ÿ��(OUT)�� ���� (OBJECTS)�� ���Ӽ�
$(OUT): $(OBJECTS)
	$(CC) -o $(OUT) $(OBJECTS) $(LIBS)

#�� �ҽ� ������ ���Ӽ� ����
main.o: main.c editor.h
	$(CC) -c main.c

editor.o: editor.c editor.h
	$(CC) -c editor.c

-include dependencies.mk

#make clean�� ��ɾ� ����
clean:
	del $(OUT) $(OBJECTS)