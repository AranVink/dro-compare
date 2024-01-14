CC = gcc
CFLAGS = -Wall -Wextra -pedantic
INSTALL_PATH = /usr/local/bin

TARGET = dro_compare

SRC_FILES = drocompare.c

all: $(TARGET)

$(TARGET): $(SRC_FILES)
	$(CC) $(CFLAGS) $(SRC_FILES) -o $(TARGET)

install:
	cp $(TARGET) $(INSTALL_PATH)

clean:
	rm -f $(TARGET)