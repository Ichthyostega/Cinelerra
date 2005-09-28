include ../../global_config

OBJS = $(OBJDIR)/suv.o
THEME_DATA := $(OBJDIR)/suv_data.o

OUTPUT_THEME := ../$(OBJDIR)/suv.plugin
AVC_DIR := $(shell expr ../../libavc* )
LIBRAW_DIR := $(shell expr ../../libraw1394* )

CFLAGS += -I$(AVC_DIR) -I$(LIBRAW_DIR)

$(OUTPUT_THEME): $(OBJS) $(THEME_DATA)
	$(LDLINKER) -o $(OUTPUT_THEME) $(OBJS) $(THEME_DATA)
	strip $(OUTPUT_THEME)



include ../../plugin_config

$(OBJDIR)/suv.o: suv.C

$(THEME_DATA):
	cd $(OBJDIR) && \
	../../../guicast/$(OBJDIR)/bootstrap suv_data.o ../data/*.png

inst:
	rm -f $(PLUGINDIR)/suv.plugin
	cp $(OUTPUT_THEME) $(PLUGINDIR)
