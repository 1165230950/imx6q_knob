include $(DBG_APP_BUILD_PATH)/build.mk

send : netlink-exam-user-send.o
	$(CC) -o send netlink-exam-user-send.c
netlink-exam-user-send.o : netlink-exam-user-send.c
	$(CC) -c netlink-exam-user-send.c

knob : read_knob.o
	$(CC) -o send read_knob.c
read_knob.o : read_knob.c
	$(CC) -c read_knob.c
