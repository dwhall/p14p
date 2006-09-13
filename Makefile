#
# Available arguments:
#     TARGET = [DESKTOP, AVR]
#
# Default values:
#     TARGET = DESKTOP
#

# Build configuration
TARGET := DESKTOP

# Tools configuration
SHELL = /bin/sh
CP := cp
MKDIR := mkdir -p
TAGS := ctags
CSCOPE := cscope

VPATH := . src/vm src/lib docs/src


# Export config to child makefiles
export TARGET

.PHONY: all vm pmstdlib docs TAGS dist check clean

all : vm

vm :
	$(MAKE) -C src/vm

html : docs/src/*.txt
	$(MKDIR) docs/html
	$(MAKE) -C docs/src

TAGS :
	$(TAGS) -R *
	$(CSCOPE) -b -c -R
	cd src/tools && pycscope.py -R *.py

dist : check docs
ifndef PYMITE_RELEASE
	$(error Must define PYMITE_RELEASE=RR)
else
	# TODO: issue #5
	# Make a script in tools/ that will:
	# 	- make fresh export (no .svn folders),
	#   - build docs
	#   - make pymite-RR.tar.gz
	#	- create release tag in svn repos
endif

check :
	$(MAKE) -C src/tests

# Removes all files created during default make
clean :
	$(MAKE) -C src/vm clean

