all: tr-trigger

tr-trigger: tr-trigger.c
	gcc -o tr-trigger tr-trigger.c -lX11 -Wall
