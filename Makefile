# Makefile for Linux Winsock2 and Winsock 1.1 Wrapper Libraries

CC = gcc
AR = ar
CFLAGS = -Wall -Wextra -O2 -fPIC -std=c99 -D_GNU_SOURCE -pthread
CXXFLAGS = -Wall -Wextra -O2 -fPIC -std=c++98 -D_GNU_SOURCE -pthread

# Winsock 2.2 library (ws2_32.dll)
WS2_LIB_NAME = libws2_32
WS2_STATIC_LIB = $(WS2_LIB_NAME).a
WS2_SHARED_LIB = $(WS2_LIB_NAME).so

# Winsock 1.1 library (wsock32.dll)
WSOCK_LIB_NAME = libwsock32
WSOCK_STATIC_LIB = $(WSOCK_LIB_NAME).a
WSOCK_SHARED_LIB = $(WSOCK_LIB_NAME).so

# Winsock 2.2 source files
WS2_SOURCES = winsock2.c \
              wsa_extended.c \
              wsa_events.c \
              wsa_addr.c \
              ms_extensions.c

# Winsock 1.1 source files
WSOCK_SOURCES = winsock2.c \
                wsa_events.c \
                wsock32.c

# Object files
WS2_OBJECTS = $(WS2_SOURCES:.c=.o)
WSOCK_OBJECTS = $(WSOCK_SOURCES:.c=.o)

# Header files
WS2_HEADERS = winsock2.h \
              winsock2_api.h \
              ws2tcpip.h \
              mswsock.h \
              windows_types.h

WSOCK_HEADERS = winsock.h \
                windows_types.h

# Default target - build both libraries
all: ws2_32 wsock32

# Winsock 2.2 library targets
ws2_32: $(WS2_STATIC_LIB) $(WS2_SHARED_LIB)

$(WS2_STATIC_LIB): $(WS2_OBJECTS)
	$(AR) rcs $@ $^
	@echo "Static library $(WS2_STATIC_LIB) created successfully"

$(WS2_SHARED_LIB): $(WS2_OBJECTS)
	$(CC) -shared -o $@ $^ -pthread
	@echo "Shared library $(WS2_SHARED_LIB) created successfully"

# Winsock 1.1 library targets
wsock32: $(WSOCK_STATIC_LIB) $(WSOCK_SHARED_LIB)

$(WSOCK_STATIC_LIB): $(WSOCK_OBJECTS)
	$(AR) rcs $@ $^
	@echo "Static library $(WSOCK_STATIC_LIB) created successfully"

$(WSOCK_SHARED_LIB): $(WSOCK_OBJECTS)
	$(CC) -shared -o $@ $^ -pthread
	@echo "Shared library $(WSOCK_SHARED_LIB) created successfully"

# Compile C source files
%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

# Install target
install: all
	install -d $(DESTDIR)/usr/local/lib
	install -m 644 $(WS2_STATIC_LIB) $(DESTDIR)/usr/local/lib/
	install -m 755 $(WS2_SHARED_LIB) $(DESTDIR)/usr/local/lib/
	install -m 644 $(WSOCK_STATIC_LIB) $(DESTDIR)/usr/local/lib/
	install -m 755 $(WSOCK_SHARED_LIB) $(DESTDIR)/usr/local/lib/
	install -d $(DESTDIR)/usr/local/include
	install -m 644 $(WS2_HEADERS) $(DESTDIR)/usr/local/include/
	install -m 644 $(WSOCK_HEADERS) $(DESTDIR)/usr/local/include/
	@echo "Libraries installed to $(DESTDIR)/usr/local"

# Uninstall target
uninstall:
	rm -f $(DESTDIR)/usr/local/lib/$(WS2_STATIC_LIB)
	rm -f $(DESTDIR)/usr/local/lib/$(WS2_SHARED_LIB)
	rm -f $(DESTDIR)/usr/local/lib/$(WSOCK_STATIC_LIB)
	rm -f $(DESTDIR)/usr/local/lib/$(WSOCK_SHARED_LIB)
	rm -f $(DESTDIR)/usr/local/include/winsock2.h
	rm -f $(DESTDIR)/usr/local/include/winsock2_api.h
	rm -f $(DESTDIR)/usr/local/include/winsock.h
	rm -f $(DESTDIR)/usr/local/include/ws2tcpip.h
	rm -f $(DESTDIR)/usr/local/include/mswsock.h
	rm -f $(DESTDIR)/usr/local/include/windows_types.h
	@echo "Libraries uninstalled from $(DESTDIR)/usr/local"

# Test programs
test: test_ws2_32 test_wsock32

test_ws2_32: test_winsock.c $(WS2_STATIC_LIB)
	$(CC) $(CFLAGS) -o test_winsock test_winsock.c -L. -lws2_32 -pthread
	@echo "Winsock 2.2 test program compiled successfully"

test_wsock32: test_winsock1.c $(WSOCK_STATIC_LIB)
	$(CC) $(CFLAGS) -o test_winsock1 test_winsock1.c -L. -lwsock32 -pthread
	@echo "Winsock 1.1 test program compiled successfully"

# Clean target
clean:
	rm -f *.o $(WS2_STATIC_LIB) $(WS2_SHARED_LIB) $(WSOCK_STATIC_LIB) $(WSOCK_SHARED_LIB) test_winsock test_winsock1
	@echo "Cleaned build artifacts"

# Help target
help:
	@echo "Linux Winsock Wrapper Libraries Makefile"
	@echo "========================================="
	@echo ""
	@echo "Available targets:"
	@echo "  all          - Build both ws2_32 and wsock32 libraries (default)"
	@echo "  ws2_32       - Build Winsock 2.2 libraries only"
	@echo "  wsock32      - Build Winsock 1.1 libraries only"
	@echo "  install      - Install libraries and headers to /usr/local"
	@echo "  uninstall    - Remove installed files"
	@echo "  test         - Build both test programs"
	@echo "  test_ws2_32  - Build Winsock 2.2 test program"
	@echo "  test_wsock32 - Build Winsock 1.1 test program"
	@echo "  clean        - Remove all build artifacts"
	@echo "  help         - Show this help message"
	@echo ""
	@echo "Usage examples:"
	@echo "  make                    # Build all libraries"
	@echo "  make ws2_32             # Build Winsock 2.2 only"
	@echo "  make wsock32            # Build Winsock 1.1 only"
	@echo "  make test               # Build test programs"
	@echo "  make install            # Install system-wide"
	@echo "  make clean              # Clean build files"

.PHONY: all ws2_32 wsock32 install uninstall test test_ws2_32 test_wsock32 clean help
