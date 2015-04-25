CXX ?= g++
CXXFLAGS = -std=c++0x -Wno-psabi
LDFLAGS = -lapt-pkg -ludev -lblkid

OBJS = main.o isoarchive_method.o repo.o misc.o
isoarchive: $(OBJS)
	$(CXX) $(CXXFLAGS) $^ $(LDFLAGS) -o $@
CLEAN += isoarchive
$(OBJS): isoarchive_method.h repo.h misc.h

.PHONY: tests
tests: test-drives test-fstype test-mount

test-drives.cpp: misc.h
test-drives: test-drives.o misc.o
	$(CXX) $(CXXFLAGS) $^ $(LDFLAGS) -o $@
CLEAN += test-drives

test-fstype.cpp: misc.h
test-fstype: test-fstype.o misc.o
	$(CXX) $(CXXFLAGS) $^ $(LDFLAGS) -o $@
CLEAN += test-fstype

test-mount.cpp: misc.h
test-mount: test-mount.o misc.o
	$(CXX) $(CXXFLAGS) $^ $(LDFLAGS) -o $@
CLEAN += test-mount

.PHONY: clean
clean:
	rm -f *.o $(CLEAN)

.PHONY: install
install: isoarchive
	install -D $^ $(DESTDIR)/usr/lib/apt/methods
