#! /usr/bin/make -f
SHELL=/bin/bash
#BBOX=-9000000,-18000000,9000000,18000000
BBOX=0,0,1,1

TEST_REGIONS=perl -I/home/quinn/projects/geostreams/perl/DCT/lib /home/quinn/projects/geostreams/perl/DCT/test-regions --bbox=$(BBOX)
DCT=perl -I/home/quinn/projects/geostreams/perl/DCT/lib /home/quinn/projects/geostreams/perl/DCT/dct.pl --bbox=$(BBOX)
CTEST=time /home/quinn/projects/geostreams/src/dct/dctWindowQueryTest
SEGTEST=time /home/quinn/projects/geostreams/src/dct/segment-tree/SegmentTreeTest

moves=-3 -2 -1
sizes=0.0001 0.001 0.01 0.1 0.2 
rands=0 0.25 0.5 0.75 0
counts=128 256 512 1024 2048 4096 8192 16384 
ds=0.0025 0.005 0.01 0.02

# Default numbers
Stabs=1000
Count=5000
Aspect=0.7,1.4
Area=0.01,0.1
Dis = 1e-3,1e-2

#exp=0 1 2 3 4 5 6 7 8 9 a b c d e f
exp=0

default:
	echo 'H';

.SUFFIXES:

clean:
	rm -f $(def.box);

clean-db:

c%.box :
	$(TEST_REGIONS) --count=$* --aspect=$(Aspect) --area=$(Area) --type=UniformRandom > $@


define STAB

c$(1)d$(2).stab:
	$(TEST_REGIONS) --aspect=$(Aspect) --count=$(Stabs) --type=RandomWalk --dis=$(2),$(2) > $$@

c$(1)d$(2).insert c$(1)d$(2).stats c$(1)d$(2).out: c$(1).box c$(1)d$(2).stab
	$(DCT) --inserts=c$(1)d$(2).insert --stats=c$(1)d$(2).stats --out=c$(1)d$(2).out --boxes=c$(1).box --stabs=c$(1)d$(2).stab

c$(1)d$(2).stats.db: c$(1)d$(2).stats
	perl -n -a -F' ' -e 'print join("\t",$$$$ARGV,@F),"\n" unless $$$$F[0]=~/^\#/;' $$< | psql  -c 'COPY random.stats from STDIN' -d geostreams
	touch $$@

c$(1)d$(2).c.out: c$(1).box c$(1)d$(2).stab
	if [[ -e gmon.out ]]; then rm gmon.out; fi
	$(CTEST)  c$(1).box c$(1)d$(2).stab 1 100000 > $$@
	[[ -e gmon.out ]] && mv gmon.out c$(1)d$(2).c.gmon.out

c$(1)d$(2).s.out: c$(1).box c$(1)d$(2).stab
	if [[ -e gmon.out ]]; then rm gmon.out; fi
	$(CTEST)  c$(1).box c$(1)d$(2).stab > $$@
	[[ -e gmon.out ]] && mv gmon.out c$(1)d$(2).c.gmon.out

endef

count := $(foreach c,$(counts),$(patsubst %,c$(c)d%.out,$(ds)))
c.count := $(foreach c,$(counts),$(patsubst %,c$(c)d%.c.out,$(ds)))
c.count := $(foreach c,$(counts),$(patsubst %,c$(c)d%.s.out,$(ds)))

db := $(foreach c,$(counts),$(patsubst %,c$(c)d%.stats.db,$(ds)))
$(foreach c,$(counts),$(foreach d,$(ds),$(eval $(call STAB,$(c),$(d)))))

count: $(count) $(c.count)
db: $(db)
boxes: $(patsubst %,c%.box,$(counts))