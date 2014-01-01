
all: format-code

format-code:
	for i in `ls *.cpp`; \
		do \
			uncrustify -c default.cfg --replace $$i; \
		done
