include ../global_config

# burn must come before any other effecttv plugin
# colors must come before any plugin
# parametric must come before fourier plugins
# motion must come before perspective

DIRS = \
	1080to480 \
	720to480 \
	colors \
	bandslide \
	bandwipe \
	blur \
	brightness \
	burn \
	parametric \
	aging \
	cdripper \
	chromakey \
	colorbalance \
	compressor \
	crossfade \
	decimate \
	defaulttheme \
	deinterlace \
	delayaudio \
	delayvideo \
	denoise \
	denoisefft \
	denoisevideo \
	despike \
	dissolve \
	dot \
	downsample \
	fieldframe \
	flash \
	flip \
	framefield \
	freeverb \
	freezeframe \
	gain \
	gradient \
	histogram \
	holo \
	huesaturation \
	interpolateaudio \
	interpolatevideo \
	invertaudio \
	invertvideo \
	irissquare \
	ivtc \
	level \
	linearblur \
	loopaudio \
	loopvideo \
	motion \
	normalize \
	oilpainting \
	overlay \
	perspective \
	pitch \
	polar \
	radialblur \
	reframe \
	resample \
	reverb \
	reverseaudio \
	reversevideo \
	rgb601 \
	rotate \
	scale \
	sharpen \
	shiftinterlace \
	slide \
	spectrogram \
	swapchannels \
	synthesizer \
	threshold \
	timeavg \
	timestretch \
	titler \
	translate \
	unsharp \
	videoscope \
	wave \
	whirl \
	wipe \
	yuv \
	zoomblur

# Make static table of all plugins
ifeq ($(DO_STATIC), y)

endif


all: $(DIRS) $(OBJS)
	@ for i in $(DIRS) ; \
	do \
		 $(MAKE) -C $$i ; \
	done

clean:
	find \( -name core \
		-o -name '*.o' \
		-o -name '*.plugin' \) -exec rm -rf {} \;

wc:
	cat *.C *.h | wc
	$(LOOP_BEGIN) \
		 $(MAKE) -C $$i wc; \
	$(LOOP_END)


# Can't strip the themes so instead the plugins strip themselves
install:
#	strip $(OBJDIR)/*.plugin
	rm -rf /usr/lib/cinelerra
	mkdir -p /usr/lib/cinelerra
	cp $(OBJDIR)/*.plugin /usr/lib/cinelerra
	cp -a titler/fonts /usr/lib/cinelerra





