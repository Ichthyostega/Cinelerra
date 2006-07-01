include ../../global_config

OBJS = $(OBJDIR)/livevideo.o
DV_DIR := $(shell expr ../../quicktime/libdv-* )
CFLAGS += -I$(DV_DIR)

OUTPUT = ../$(OBJDIR)/livevideo.plugin

include ../../plugin_config


$(OBJDIR)/livevideo.o: livevideo.C
