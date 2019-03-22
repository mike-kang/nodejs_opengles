
TARGET:=test
SRC:=common/LoadShaders.cpp common/startScreen.cpp common/texture.cpp Server.cpp main.cpp
OBJ:=$(SRC:%.cpp=%.o)
CXXFLAGS := -I/opt/vc/include -I./common -g -std=c++11
all : $(TARGET)

$(TARGET) : $(OBJ)
	g++ -o $@ -L/opt/vc/lib -lbcm_host -pthread -lEGL -lGLESv2 $^

.PHONY : clean

clean :
	rm *.o
	rm $(TARGET)






