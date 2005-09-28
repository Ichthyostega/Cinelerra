#TARGET_DIR := /home/httpd/html/cinelerra
TARGET_DIR := /amazon/home/httpd/html/cinelerra

IMAGES = \
	arrow.png \
	autokeyframe.png \
	camera.png \
	channel.png \
	crop.png \
	cut.png \
	expandpatch_checked.png \
	eyedrop.png \
	fitautos.png \
	ibeam.png \
	left_justify.png \
	magnify.png \
	mask.png \
	mutepatch_up.png \
	paste.png \
	projector.png \
	protect.png \
	record.png \
	recordpatch_up.png \
	rewind.png \
	singleframe.png \
	show_meters.png \
	titlesafe.png \
	toolwindow.png \
	top_justify.png \
	wrench.png \
	magnify.png

$(OUTPUT):
	cinelerra.html

all:
	for i in $(IMAGES) ; \
		do if ! test -e $$i ; \
			then cp -a ../plugins/suv/data/$$i . ; \
		fi ; \
	done
	makeinfo --html --no-split cinelerra.texi
#	makeinfo --html cinelerra.texi

install:
	cp $(IMAGES) $(TARGET_DIR)
	cp cinelerra.html $(TARGET_DIR)

clean:
	rm -f $(IMAGES) $(OUTPUT)
