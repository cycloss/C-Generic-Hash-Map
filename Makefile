SRC_DIR = src/
TEST_DIR = test/
SOURCE_FILES = $(addprefix $(SRC_DIR), hashMap.c)
HEADER_FILES = $(addprefix $(SRC_DIR), hashMap.h)
TEST_NAME = hashMapTest
TEST_FILE = $(TEST_DIR)$(TEST_NAME).c

OS := $(shell uname)

.PHONY: test
test: $(SOURCE_FILES) $(HEADER_FILES)
	gcc -Wall $(SOURCE_FILES) $(TEST_FILE) -lGenericStructures -o $(TEST_NAME)

./$(TEST_NAME): test

leakCheck: ./$(TEST_NAME)
	valgrind --leak-check=full ./$(TEST_NAME)
	rm -rf $(TEST_NAME).dSYM

ifeq ($(OS), Darwin)

LIB_NAME = libGenericStructures.a
OBJECT_FILES = hashmap.o
DESTDIR = /usr/local

.PHONY: install
install: $(LIB_NAME) $(HEADER_FILES)
	cp $(HEADER_FILES) $(DESTDIR)/include

$(LIB_NAME): $(OBJECT_FILES)
	ar -rcs $(DESTDIR)/lib/$(LIB_NAME) $(OBJECT_FILES)
	rm -f $(OBJECT_FILES)

$(OBJECT_FILES): $(SOURCE_FILES)
	gcc -c $(SOURCE_FILES)

.PHONY: uninstall
uninstall:
	rm -f $(DESTDIR)/lib/$(LIB_NAME)
	rm -f $(addprefix $(DESTDIR)/include/, $(HEADER_FILES))

endif