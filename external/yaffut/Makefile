OSTYPE = $(shell ./.ostype)
ifeq ($(OSTYPE),cygwin)
OS_LD   = -lcomctl32 -mwindows -e _mainCRTStartup
OS_CPP  = -DWIN32
endif

DELIV = yaffut-test
TEST = example.o simple.o fail.o parameters.o noel_llopis.o
OBJS = $(TEST) gui.o

EXAMPLES = $(wildcard *.cpp)
READMES = AUTHORS FEATURES LICENSE RATIONALE README README.emacs TODO
DOCFILES = $(READMES) doc.ps


CFLAGS = -g -W -Wall
CXXFLAGS = $(CFLAGS)
FOX_VERSION = $(shell fox-config --version 2>/dev/null)
ifneq ($(FOX_VERSION),)
FOX_CFLAGS = $(shell fox-config --cflags)
FOX_LDFLAGS = $(shell fox-config --libs)
DELIV += yaffut-gui
else
$(warning fox-config not found, not building yaffut-gui)
endif
LDFLAGS  = -lboost_thread -lboost_regex $(FOX_LDFLAGS) $(OS_LD)
CPPFLAGS = $(OS_CPP) $(FOX_CFLAGS)
ARFLAGS  = rc
DOC      = ./cpp2ps

all: $(DELIV)

%.d: %.cpp
	@$(CXX) $(CPPFLAGS) -MM $< | sed 's,\($*\)\.o[ :]*,\1.o $@ : ,g' >> $@
	@echo $@ 

%.o: %.cpp
	$(CXX) $(CPPFLAGS) $(CXXFLAGS) -c -o $@ $<

yaffut-gui: yaffut-test
yaffut-gui: gui.o
	$(CXX) -o $@ $< $(LDFLAGS)

yaffut-test: $(TEST)
	$(CXX) -o $@ $^

ifneq ($(FOX_VERSION),)
check: ./yaffut-gui ./yaffut-test
	./yaffut-gui ./yaffut-test
else
check: ./yaffut-test
	./yaffut-test
endif

doc.ps:
	$(DOC) $@ *.h *.cpp

OBJECTS = $(TEST) gui.o

ifneq ($(MAKECMDGOALS),clean)
-include $(OBJECTS:%.o=%.d)
endif

clean:
	rm -f $(DELIV) $(TEST) $(OBJECTS) $(OBJECTS:%.o=%.d)

dist:
	bzr export yaffut-$(shell bzr revno).tar.gz

prefix=$(DESTDIR)/usr/local
includedir=$(prefix)/include
datadir=$(prefix)/share
docdir=$(datadir)/doc
packagedocdir=$(datadir)/doc/yaffut

install: all
	install -m 755 -d $(bindir)
	install -m 755 yaffut-test $(bindir)
ifneq ($(FOX_VERSION),)
	install -m 755 yaffut-gui $(bindir)
endif
	install -m 755 -d $(includedir)
	install -m 644 yaffut.h $(includedir)
	install -m 755 -d $(packagedocdir)
	install -m 644 $(DOCFILES) $(packagedocdir)
	install -m 644 doc.ps $(packagedocdir)
	install -m 755 -d $(packagedocdir)/examples
	install -m 644 $(EXAMPLES) $(packagedocdir)/examples

uninstall:
	rm $(bindir)/yaffut-test
	-rm $(bindir)/yaffut-gui
	-rmdir $(bindir)
	rm $(includedir)/yaffut.h
	-rmdir $(includedir)
	rm $(EXAMPLES:%=$(packagedocdir)/examples/%)
	-rmdir $(packagedocdir)/examples
	rm $(DOCFILES:%=$(packagedocdir)/%)
	-rmdir $(packagedocdir)
	-rmdir $(docdir)
	-rmdir $(datadir)
	-rmdir $(prefix)
