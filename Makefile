# Witchaven Makefile for GNU Make

# Create Makefile.user yourself to provide your own overrides
# for configurable values
-include Makefile.user

##
##
## CONFIGURABLE OPTIONS
##
##

# Engine options
#  USE_POLYMOST   - enables Polymost renderer
#  USE_OPENGL     - enables OpenGL support in Polymost
#     Define as 0 to disable OpenGL
#     Define as USE_GL2 (or 1 or 2) for GL 2.0/2.1 profile
#     Define as USE_GL3 (or 3) for GL 3.2 Core profile
#     Define as USE_GLES2 (or 12) for GLES 2.0 profile
#  USE_ASM        - enables the use of assembly code if supported
#
USE_POLYMOST ?= 1
USE_OPENGL ?= 1
USE_ASM ?= 1

# Debugging options
#  RELEASE - 1 = optimised release build, no debugging aids
RELEASE ?= 1

# Path where game data is installed
DATADIR ?= /usr/local/share/games/jfwhaven

##
##
## HERE BE DRAGONS
##
##

ENGINEROOT=jfbuild
ENGINEINC=$(ENGINEROOT)/include
LIBSMACKERSRC=libsmacker
AUDIOLIBROOT=jfaudiolib
SRC=src
RSRC=rsrc

CC?=gcc
CXX?=g++
RC?=windres

OURCFLAGS=-g -W -Wall -fno-strict-aliasing -std=c99
OURCXXFLAGS=-g -W -Wall -fno-exceptions -fno-rtti -std=c++98
OURCPPFLAGS=-I$(SRC) -I$(ENGINEINC) -I$(AUDIOLIBROOT)/include -I$(LIBSMACKERSRC)
OURLDFLAGS=

ifneq ($(RELEASE),0)
	# Default optimisation settings
	CFLAGS?=-fomit-frame-pointer -O2
	CXXFLAGS?=-fomit-frame-pointer -O2
else
	CFLAGS?=-Og
	CXXFLAGS?=-Og
endif

# Filename extensions, used in Makefile.deps etc
o=o
res=o

include $(ENGINEROOT)/Makefile.shared

# Apply shared flags
OURCFLAGS+= $(BUILDCFLAGS)
OURCXXFLAGS+= $(BUILDCXXFLAGS)
OURCPPFLAGS+= $(BUILDCPPFLAGS)
OURLDFLAGS+= $(BUILDLDFLAGS)

include $(AUDIOLIBROOT)/Makefile.shared

OURLDFLAGS+= $(JFAUDIOLIB_LDFLAGS)

GAMEOBJS= \
	$(SRC)/b5compat.$o \
	$(SRC)/config.$o \
	$(SRC)/whani.$o \
	$(SRC)/whaven.$o \
	$(SRC)/whfx.$o \
	$(SRC)/whinp.$o \
	$(SRC)/whmenu.$o \
	$(SRC)/whnet.$o \
	$(SRC)/whobj.$o \
	$(SRC)/whplr.$o \
	$(SRC)/whsmk.$o \
	$(SRC)/whsndmod.$o \
	$(SRC)/whtag.$o \
	$(SRC)/datascan.$o \
	$(RSRC)/palette.$o

GAMEOBJS+= \
	$(LIBSMACKERSRC)/smacker.$o

EDITOROBJS=$(SRC)/whbuild.$o

# Specialise for the platform
ifeq ($(PLATFORM),LINUX)
	OURLDFLAGS+= $(JFAUDIOLIB_LDFLAGS)
	OURCPPFLAGS+= -DDATADIR=\"$(DATADIR)\"
endif
ifeq ($(PLATFORM),BSD)
	OURLDFLAGS+= $(JFAUDIOLIB_LDFLAGS) -pthread
	OURCPPFLAGS+= -DDATADIR=\"$(DATADIR)\"
endif
ifeq ($(PLATFORM),WINDOWS)
	GAMEOBJS+= $(SRC)/gameres.$(res) \
		$(SRC)/startwin_game.$o
	EDITOROBJS+= $(SRC)/buildres.$(res)
	OURLDFLAGS+= -lole32 -ldsound \
	       $(AUDIOLIBROOT)/third-party/mingw32/lib/libvorbisfile.a \
	       $(AUDIOLIBROOT)/third-party/mingw32/lib/libvorbis.a \
	       $(AUDIOLIBROOT)/third-party/mingw32/lib/libogg.a
endif
ifeq ($(PLATFORM),DARWIN)
	GAMEOBJS+= $(SRC)/StartupWinController.$o
	OURLDFLAGS+= -framework AppKit
endif

ifeq ($(RENDERTYPE),SDL)
	OURCFLAGS+= $(SDLCONFIG_CFLAGS)
	OURLDFLAGS+= $(SDLCONFIG_LIBS)

	ifeq (1,$(HAVE_GTK))
		OURCFLAGS+= $(GTKCONFIG_CFLAGS)
		OURLDFLAGS+= $(GTKCONFIG_LIBS)

		GAMEOBJS+= $(SRC)/startgtk_game.$o \
			$(RSRC)/startgtk_game_gresource.$o
		EDITOROBJS+= $(RSRC)/startgtk_build_gresource.$o
	endif

	GAMEOBJS+= $(RSRC)/sdlappicon_game.$o
	EDITOROBJS+= $(RSRC)/sdlappicon_build.$o
endif

# Source-control version stamping
ifneq (,$(findstring git version,$(shell git --version)))
GAMEOBJS+= $(SRC)/version-auto.$o
EDITOROBJS+= $(SRC)/version-auto.$o
else
GAMEOBJS+= $(SRC)/version.$o
EDITOROBJS+= $(SRC)/version.$o
endif

# TARGETS
.PHONY: clean veryclean all
all: whaven$(EXESUFFIX) build$(EXESUFFIX)

include Makefile.deps

$(ENGINEROOT)/%:
	$(MAKE) -C $(@D) -f Makefile \
		USE_POLYMOST=$(USE_POLYMOST) \
		USE_OPENGL=$(USE_OPENGL) \
		USE_ASM=$(USE_ASM) \
		RELEASE=$(RELEASE) $(@F)

$(AUDIOLIBROOT)/%:
	$(MAKE) -C $(@D) \
		RELEASE=$(RELEASE) $(@F)

whaven$(EXESUFFIX): $(GAMEOBJS) $(ENGINEROOT)/$(ENGINELIB) $(AUDIOLIBROOT)/$(JFAUDIOLIB)
	$(CXX) $(CPPFLAGS) $(OURCPPFLAGS) $(CXXFLAGS) $(OURCXXFLAGS) -o $@ $^ $(LDFLAGS) $(OURLDFLAGS)

build$(EXESUFFIX): $(EDITOROBJS) $(ENGINEROOT)/$(EDITORLIB) $(ENGINEROOT)/$(ENGINELIB)
	$(CXX) $(CPPFLAGS) $(OURCPPFLAGS) $(CXXFLAGS) $(OURCXXFLAGS) -o $@ $^ $(LDFLAGS) $(OURLDFLAGS)

# RULES
$(SRC)/%.$o: $(SRC)/%.c
	$(CC) $(CPPFLAGS) $(OURCPPFLAGS) $(CFLAGS) $(OURCFLAGS) -c $< -o $@

$(SRC)/%.$o: $(SRC)/%.m
	$(CC) $(CPPFLAGS) $(OURCPPFLAGS) $(CFLAGS) $(OURCFLAGS) -c $< -o $@

$(LIBSMACKERSRC)/%.$o: $(LIBSMACKERSRC)/%.c
	$(CC) $(CPPFLAGS) $(OURCPPFLAGS) $(CFLAGS) $(OURCFLAGS) -c $< -o $@

$(RSRC)/%.$o: $(RSRC)/%.c
	$(CC) $(CPPFLAGS) $(OURCPPFLAGS) $(CFLAGS) $(OURCFLAGS) -c $< -o $@

$(SRC)/%.$(res): $(SRC)/%.rc
	$(RC) -i $< -o $@ --include-dir=$(SRC) --include-dir=$(ENGINEINC)

$(RSRC)/%.c: $(RSRC)/%.dat | $(ENGINEROOT)/bin2c$(EXESUFFIX)
	$(ENGINEROOT)/bin2c$(EXESUFFIX) $< default_$* > $@
$(RSRC)/%_gresource.c $(RSRC)/%_gresource.h: $(RSRC)/%.gresource.xml
	glib-compile-resources --generate --manual-register --c-name=startgtk --target=$@ --sourcedir=$(RSRC) $<
$(RSRC)/sdlappicon_%.c: $(RSRC)/%.png | $(ENGINEROOT)/generatesdlappicon$(EXESUFFIX)
	$(ENGINEROOT)/generatesdlappicon$(EXESUFFIX) $< > $@

# PHONIES
clean::
	-rm -f $(GAMEOBJS) $(EDITOROBJS)
veryclean:: clean
	-rm -f whaven$(EXESUFFIX) build$(EXESUFFIX) core*

clean::
	$(MAKE) -C $(ENGINEROOT) $@
	$(MAKE) -C $(AUDIOLIBROOT) $@
veryclean::
	$(MAKE) -C $(ENGINEROOT) $@

.PHONY: $(SRC)/version-auto.c
$(SRC)/version-auto.c:
	printf "const char *game_version = \"%s\";\n" "$(shell git describe --always || echo git error)" > $@
	echo "const char *game_date = __DATE__;" >> $@
	echo "const char *game_time = __TIME__;" >> $@
