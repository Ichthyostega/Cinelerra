include Makevars


OBJS := \
	de.mo \
	es.mo \
	fr.mo \
	sl.mo


all: $(OBJS)


install:
	@for i in $(OBJS) ; \
	do \
		BASENAME=`echo $$i | sed 's/\.mo//'` ; \
		DST=cinelerra.mo ; \
		DIR=/usr/share/locale/$$BASENAME/LC_MESSAGES ; \
		echo mkdir -p $$DIR ; \
		mkdir -p $$DIR ; \
		echo cp $$i $$DIR/$$DST ; \
		cp $$i $$DIR/$$DST ; \
	done

clean:
	rm -f $(OBJS)

$(OBJS):
	msgfmt -c -o $@ $(subst .mo,.po,$@)

de.mo:   de.po
es.mo:   es.po
fr.mo:   fr.po
sl.mo:   sl.po


