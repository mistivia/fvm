target = foo
cflags = -g
ldflags = -lm
cc = gcc

src = $(shell find src/ -name '*.c' -not -name 'main.c')
obj = $(src:.c=.o)
tests=$(shell find tests/ -name '*.c')
tests_bin=$(tests:.c=.bin)

all: $(target)

$(target): $(obj) src/main.o
	$(cc) $(cflags) $(ldflags) -o $@ $^

test: $(tests_bin)
	@echo
	@echo "Run tests:"
	@scripts/runall.sh $^

$(obj):%.o:%.c
	$(cc) -c $(cflags) $< -MD -MF $@.d -o $@

src/main.o:src/main.c
	$(cc) -c $(cflags) $< -MD -MF $@.d -o $@

$(tests_bin):%.bin:%.c $(obj)
	$(cc) -Isrc/ $(cflags) $(ldflags) $< $(obj) -MD -MF $@.d -o $@

clean:
	-rm $(shell find tests/ -name '*.bin')
	-rm $(shell find . -name '*.o' -or -name '*.d')
	-rm $(target)

DEPS := $(shell find . -name *.d)
ifneq ($(DEPS),)
include $(DEPS)
endif
