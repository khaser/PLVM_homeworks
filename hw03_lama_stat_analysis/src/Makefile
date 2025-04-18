TOPFILE = lamac
OCAMLC = ocamlfind c
OCAMLOPT = ocamlfind opt
OCAMLDEP = ocamlfind dep
SOURCES = version.ml stdpath.ml Language.ml Pprinter.ml SM.ml X86.ml Driver.ml
CAMLP5 = -syntax camlp5o -package ostap.syntax,GT.syntax,GT.syntax.all
PXFLAGS = $(CAMLP5)
BFLAGS = -rectypes -g -w -13-58 -package GT,ostap,unix
OFLAGS = $(BFLAGS)

all: # depend metagen #  $(TOPFILE)
	dune build ./Driver.exe
	ln -sf ../_build/default/src/Driver.exe lamac

metagen:
	echo "let version = \"Version `git rev-parse --abbrev-ref HEAD`, `git rev-parse --short HEAD`, `git rev-parse --verify HEAD |git show --no-patch --no-notes --pretty='%cd'`\"" > version.ml
	echo "let path = \"`opam var share`/Lama\"" > stdpath.ml

depend: $(SOURCES)
	$(OCAMLDEP) $(PXFLAGS) *.ml > .depend

$(TOPFILE): $(SOURCES:.ml=.cmx)
	$(OCAMLOPT) -o $(TOPFILE) $(OFLAGS) -linkpkg $(SOURCES:.ml=.cmx)

$(TOPFILE).byte: $(SOURCES:.ml=.cmo)
	$(OCAMLC) -o $(TOPFILE).byte $(BFLAGS) -linkpkg $(SOURCES:.ml=.cmo)

clean:
	$(RM) $(TOPFILE) *.cm[ioxa] *.annot *.o *.opt *.byte *~ .depend
	dune clean

-include .depend
# generic rules

###############
%.cmi: %.mli
	$(OCAMLC) -c $(BFLAGS) $(PXFLAGS) $<

# Note: cmi <- mli should go first
%.cmi: %.ml
	$(OCAMLC) -c $(BFLAGS) $(PXFLAGS) $<

%.cmo: %.ml
	$(OCAMLC) -c $(BFLAGS) $(PXFLAGS) $<

%.o: %.ml
	$(OCAMLOPT) -c $(OFLAGS) $(STATIC) $(PXFLAGS) $<

%.cmx: %.ml
	$(OCAMLOPT) -c $(OFLAGS) $(STATIC) $(PXFLAGS) $<