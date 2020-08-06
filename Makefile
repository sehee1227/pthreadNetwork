CC = g++
CFLAGS = -W -Wall
TARGET = chatTest
SRCS = clientChat.cpp cond.cpp main.cpp serverChat.cpp socketservice.cpp TCPCliSocket.cpp TCPServSocket.cpp
OBJS := $(SRCS:%.cpp=%.o)
OBJS := $(SRCS:%.c=%.o)
LDFLAGS = -pthread

all : $(TARGET)

$(TARGET): $(OBJS)
	$(CC) $(CFLAGS) -o $@ $^ $(LDFLAGS)

clean :
	rm -f $(OBJS) $(TARGET)