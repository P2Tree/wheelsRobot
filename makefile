DIR_INC = ./inc
DIR_SRC = ./src
DIR_OBJ = ./obj
DIR_BIN = ./bin

SRC = $(wildcard ${DIR_SRC}/*.c)
OBJ = $(patsubst %.c,${DIR_OBJ}/%.o,$(notdir ${SRC}))

TAG = robot
BIN_TAG = ${DIR_BIN}/${TAG}

CC = arm-fsl-linux-gnueabi-gcc
#CC = clang
CFLAGS = -pthread -lm -I${DIR_INC}
CFLAGS += -Wall
#CFLAGS += -g

${BIN_TAG} : ${OBJ}
	$(CC) $(CFLAGS) $(OBJ) -o $@

${OBJ} : ${DIR_OBJ}/%.o : ${DIR_SRC}/%.c
	$(CC) -c $(CFLAGS) $< -o $@

#${DIR_OBJ}/robot.o : ${DIR_INC}/robot.h ${DIR_INC}/gy953_com.h

# ${DIR_OBJ}/gy953_com.o : ${DIR_INC}/gy953_com.h ${DIR_INC}/gy953_uart.h

# ${DIR_OBJ}/gy953_uart.o : ${DIR_INC}/gy953_uart.h

.PHONY : clean cleanall install

clean :
	-rm -rf $(OBJ)
cleanall :
	-rm -rf $(BIN_TAG) $(OBJ)

install :
	./tools/autoftp.sh


