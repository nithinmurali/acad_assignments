FILES := 13d070058.pdf 
PICS := $(wildcard *.png)
PYC := $(wildcard *.pyc)

main :   
	mkdir output
	python source/main_13d070058.py
	python source/animate.py
	jupyter nbconvert --to html source/13d070058.ipynb
	mv source/13d070058.html ./output
	make everything

everything: 
	cp source/bib_file.bib . 
	pdflatex -output-directory output source/13d070058.tex 
	bibtex output/13d070058.aux
	pdflatex -output-directory output source/13d070058.tex
	pdflatex -output-directory output source/13d070058.tex
	rm bib_file.bib

.PHONY: clean test
test:
	pytest source/main_test.py
clean:	 
	rm -rf output
	rm -rf source/*.pyc
	rm -rf source/__pycache__
