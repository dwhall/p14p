SHELL = /bin/sh
RST2HTML := rst2html
RST2LATEX := rst2latex
HTML_DEST := ../html/
LATEX_DEST := ../latex/
LATEX_FILES := $(LATEX_DEST)*.tex
DOC_SOURCES := $(wildcard *.txt)
LATEX_SOURCES := $(wildcard $(LATEX_FILES))
DOC_HTML := $(addprefix $(HTML_DEST),$(DOC_SOURCES:.txt=.html))
DOC_LATEX := $(addprefix $(LATEX_DEST),$(DOC_SOURCES:.txt=.tex))


.PHONY: all html latex clean

# Generates html from restructured text source
$(HTML_DEST)%.html : %.txt
	$(RST2HTML) $< $@

# Generates the latex from restructured text source
$(LATEX_DEST)%.tex : %.txt
	$(RST2LATEX) $< $@

# Default action is to generate all document formats
all : html latex

# Generates html documentation
html : $(DOC_HTML)

# Generates the latex documentation
latex : $(DOC_LATEX)

# Removes files made by default make
clean :
	$(RM) $(DOC_HTML) $(DOC_LATEX)
