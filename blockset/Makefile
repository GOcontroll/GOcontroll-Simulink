CC=aarch64-none-linux-gnu-gcc
INCLUDES = -I$(PWD)/lib
INCLUDES += -I$(PWD)/code
INCLUDES += -I$(PWD)/code/XCP
INCLUDES += -I$(PWD)/lib/IIO
INCLUDES += -I$(PWD)/lib/OAES

LIBS = $(PWD)/lib/IIO/libiio.a
LIBS += $(PWD)/lib/OAES/liboaes_lib.a

FLAGS = -shared -fPIC -Wall -Wextra -pedantic

TARGET=GOcontroll.so

SOURCES = $(shell echo code/*.c)
SOURCES += $(shell echo code/XCP/*.c)

OBJECTS = $(SOURCES:.c=.o)

all: $(TARGET)

$(TARGET): $(OBJECTS)
	$(CC) $(FLAGS) -o $(TARGET) $(OBJECTS)