sources := $(filter-out common.c,$(wildcard *.c))
execu := $(notdir $(basename $(sources)))
topexecu := $(addprefix ../../$(notdir $(CURDIR))-,$(execu))
common_s := $(wildcard common.c)
common_o := $(subst .c,.o,$(common_s))

headers := -I ../../lib
libcnrt := ../../lib/libcnrt.a
extralibs := -lm

.PHONY : all clean

all : $(execu) 

$(execu) : % : %.o $(common_o) $(libcnrt) 
	$(LINK.o) $^ $(extralibs) -o $@
	cp $@ $(patsubst %,../../$(notdir $(CURDIR))-%,$@)

COMPILE.c += $(headers) -std=c99 -g -Wall -Wunused 

ifneq ($(MAKECMDGOALS),clean)
-include $(subst .c,.d,$(sources) $(common_s))
endif

%.d : %.c
	@$(CC) -M $(CPPFLAGS) $(headers) $< > $@.$$$$; \
	sed 's,\($*\)\.o[ :]*,\1.o $@ : ,g' < $@.$$$$ > $@; \
	rm -f $@.$$$$

clean :
	@$(RM) *.o *.d $(execu) $(topexecu)
