include $(DBG_APP_BUILD_PATH)/build.mk

send : netlink-exam-user-send.o
	$(CC) -o send netlink-exam-user-send.c
netlink-exam-user-send.o : netlink-exam-user-send.c
	$(CC) -c netlink-exam-user-send.c
