BASIC_CFLAGS := -g -Wall -Wextra
INCLUDE:= -I./include
LFLAGS:=-lm -lpthread -L./lib -Wl,-rpath,./lib -llsl64

CFLAGS:=$(BASIC_CFLAGS) $(INCLUDE) $(CFLAGS)
CXXFLAGS:=$(BASIC_CFLAGS) -std=c++11 $(INCLUDE) $(CXXFLAGS)

TARGETS:= lslsub_OTB
all: $(TARGETS) clean

lslsub_OTB: src/*.cpp
$(TARGETS):
	$(CXX) $(CXXFLAGS) $^ -o $@ $(LFLAGS)

-include $(FILES:.cpp=.dpp)
%.cpp.o: %.cpp
	$(CXX) -c $(CXXFLAGS) $< -o $@
	@$(CXX) -MM $(CXXFLAGS) $< > $*.dpp
	@mv -f $*.dpp $*.dpp.tmp
	@sed -e 's|.*:|$*.cpp.o:|' < $*.dpp.tmp > $*.dpp
	@sed -e 's/.*://' -e 's/\\$$//' < $*.dpp.tmp | fmt -1 | \
	  sed -e 's/^ *//' -e 's/$$/:/' >> $*.dpp
	@rm -f $*.dpp.tmp

-include $(FILES:.c=.d)
%.c.o: %.c
	$(CC) -c $(CFLAGS) $< -o $@
	@$(CC) -MM $(CFLAGS) $< > $*.d
	@mv -f $*.d $*.d.tmp
	@sed -e 's|.*:|$*.c.o:|' < $*.d.tmp > $*.d
	@sed -e 's/.*://' -e 's/\\$$//' < $*.d.tmp | fmt -1 | \
	  sed -e 's/^ *//' -e 's/$$/:/' >> $*.d
	@rm -f $*.d.tmp

clean:
	-$(RM) src/*.o src/*.d src/*.dpp $(PREBUILDARTIFACTS)