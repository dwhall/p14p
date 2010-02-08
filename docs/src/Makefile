SHELL = /bin/sh
RST2HTML := rst2html
RST2LATEX := rst2latex
PDFLATEX := pdflatex
HTML_DEST := ../html/
LATEX_DEST := ../latex/
PDF_DEST := ../pdf/
LATEX_FILES := $(LATEX_DEST)*.tex
DOC_SOURCES := $(wildcard *.txt)
LATEX_SOURCES := $(wildcard $(LATEX_FILES))
DOC_HTML := $(addprefix $(HTML_DEST),$(DOC_SOURCES:.txt=.html))
DOC_LATEX := $(addprefix $(LATEX_DEST),$(DOC_SOURCES:.txt=.tex))
DOC_PDF := $(addprefix $(PDF_DEST),$(DOC_SOURCES:.txt=.pdf))
PDF_TEMP_FILES = $(wildcard $(PDF_DEST)*.out) $(wildcard $(PDF_DEST)*.log) \
$(wildcard $(PDF_DEST)*.aux)


.PHONY: all html latex pdf clean

# Generates html from restructured text source
$(HTML_DEST)%.html : %.txt
	-$(RST2HTML) $< $@

# Generates the latex from restructured text source
$(LATEX_DEST)%.tex : %.txt
	-$(RST2LATEX) $< $@

# Generates the pdf from generated latex docs
$(PDF_DEST)%.pdf : $(LATEX_DEST)%.tex
	-$(PDFLATEX) -output-directory=$(PDF_DEST) \
-interaction=nonstopmode $< &> /dev/null

# Default action is to generate all document formats
all : html latex pdf

# Generates html documentation
html : $(DOC_HTML)

# Generates the latex documentation
latex : $(DOC_LATEX)

# Generates the pdf documentation
pdf : latex $(DOC_PDF)
	# XXX Ugh. This is a horrible hack; the removals don't happen on my
	# machine unless the sleep is present. Need a Make expert to check this
	# out (my guess is that the latex/pdf stuff is running in another
	# process, and the sleep gives some more time for those processes to
	# finish so that the out/log/aux files are actually present when the rm
	# is called).
	sleep 5
	rm $(PDF_DEST)*.out $(PDF_DEST)*.log $(PDF_DEST)*.aux

# Removes generated documentation files
clean :
	@$(RM) $(DOC_HTML) $(DOC_LATEX) $(DOC_PDF)
