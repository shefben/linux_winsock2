# Makefile for Linux Winsock2 Wrapper Library

CC = gcc
AR = ar
CFLAGS = -Wall -Wextra -O2 -fPIC -std=c99 -D_GNU_SOURCE -pthread
CXXFLAGS = -Wall -Wextra -O2 -fPIC -std=c++98 -D_GNU_SOURCE -pthread

# Library name
LIB_NAME = libws2_32
STATIC_LIB = $(LIB_NAME).a
SHARED_LIB = $(LIB_NAME).so

# Source files
SOURCES = winsock2.c \
          wsa_extended.c \
          wsa_events.c \
          wsa_addr.c \
          ms_extensions.c

# Object files
OBJECTS = $(SOURCES:.c=.o)

# Header files
HEADERS = winsock2.h \
          ws2tcpip.h \
          mswsock.h

# Default target
all: $(STATIC_LIB) $(SHARED_LIB)

# Static library
$(STATIC_LIB): $(OBJECTS)
	$(AR) rcs $@ $^
	@echo "Static library $(STATIC_LIB) created successfully"

# Shared library
$(SHARED_LIB): $(OBJECTS)
	$(CC) -shared -o $@ $^ -pthread
	@echo "Shared library $(SHARED_LIB) created successfully"

# Compile C source files
%.o: %.c $(HEADERS)
	$(CC) $(CFLAGS) -c $< -o $@

# Install target
install: $(STATIC_LIB) $(SHARED_LIB)
	install -d $(DESTDIR)/usr/local/lib
	install -m 644 $(STATIC_LIB) $(DESTDIR)/usr/local/lib/
	install -m 755 $(SHARED_LIB) $(DESTDIR)/usr/local/lib/
	install -d $(DESTDIR)/usr/local/include
	install -m 644 $(HEADERS) $(DESTDIR)/usr/local/include/
	@echo "Library installed to $(DESTDIR)/usr/local"

# Uninstall target
uninstall:
	rm -f $(DESTDIR)/usr/local/lib/$(STATIC_LIB)
	rm -f $(DESTDIR)/usr/local/lib/$(SHARED_LIB)
	rm -f $(DESTDIR)/usr/local/include/winsock2.h
	rm -f $(DESTDIR)/usr/local/include/ws2tcpip.h
	rm -f $(DESTDIR)/usr/local/include/mswsock.h
	@echo "Library uninstalled from $(DESTDIR)/usr/local"

# Test program
test: test_winsock.c $(STATIC_LIB)
	$(CC) $(CFLAGS) -o test_winsock test_winsock.c -L. -lws2_32 -pthread
	@echo "Test program compiled successfully"

# Clean target
clean:
	rm -f $(OBJECTS) $(STATIC_LIB) $(SHARED_LIB) test_winsock
	@echo "Cleaned build artifacts"

# Help target
help:
	@echo "Linux Winsock2 Wrapper Library Makefile"
	@echo "========================================"
	@echo ""
	@echo "Available targets:"
	@echo "  all        - Build both static and shared libraries (default)"
	@echo "  static     - Build static library only"
	@echo "  shared     - Build shared library only"
	@echo "  install    - Install libraries and headers to /usr/local"
	@echo "  uninstall  - Remove installed files"
	@echo "  test       - Build test program"
	@echo "  clean      - Remove all build artifacts"
	@echo "  help       - Show this help message"
	@echo ""
	@echo "Usage examples:"
	@echo "  make                    # Build libraries"
	@echo "  make test               # Build test program"
	@echo "  make install            # Install system-wide"
	@echo "  make clean              # Clean build files"

.PHONY: all install uninstall test clean help
