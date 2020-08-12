CC = g++
CFLAGS = -W -Wall
TARGET = chatTest
SRCS = Socket.cpp datalink.cpp clientChat.cpp cond.cpp main.cpp serverChat.cpp serverUserChat.cpp socketservice.cpp  TCPSocket.cpp
OBJS := $(SRCS:%.cpp=%.o)
LDFLAGS = -pthread

all : $(TARGET)

$(TARGET): $(OBJS)
	$(CC) $(CFLAGS) -o $@ $^ $(LDFLAGS)

clean :
	rm -f $(OBJS) $(TARGET)
