include ../../global_config

OBJS = $(OBJDIR)/dot.o  \
	$(OBJDIR)/dotwindow.o \
	../burn/$(OBJDIR)/effecttv.o \
	../colors/$(OBJDIR)/plugincolors.o

OUTPUT = ../$(OBJDIR)/dot.plugin
CFLAGS += -I../burn


include ../../plugin_config

$(OBJDIR)/dot.o: dot.C
$(OBJDIR)/dotwindow.o: dotwindow.C
