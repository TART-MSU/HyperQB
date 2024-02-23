PROJNAME=translate

# Makefile configuration
OCAMLBUILD=ocamlbuild.native

# Folders
PROGS=${SRC}/progs
SRC=./src
BIN=bin

TRANSLATE=translate
BOOL=bool
UNROLL=unroll
GENQBF=genqbf

SYS=`uname -s`

REVISION=`svn info | grep Revision | cut -d ' ' -f 2`
MYPATH=`echo $(PATH)`
DATE=`svn info | grep Date | cut -d ' ' -f 7-10 | cut -c 2-17`
TIME=`svn info | grep Date | cut -d ' ' -f 5-6`


check_tool = @if [[ `command -v $(1)` ]] ; then  echo "[ OK ]  --  $(1)"; \
                                           else  echo "[    ]  --  $(1)";  fi
copy = @mkdir -p $(2); cp $(1) $(2)/$(3)

.PHONY: profile debug clean all expand unexpand translate bool unroll

# Flags

OCAMLBUILD_FLAGS= -r -j 0 -build-dir _build 

OCAML_FLAGS=-cflags -w,-4-32

PROFILE_FLAGS=-ocamlc ocamlcp -ocamlopt ocamloptp
DEBUG_FLAGS=

LIBS = unix,str

# Compilation rules

all: $(TRANSLATE) $(BOOL)  $(GENQBF)

# write-revision:
# 	@echo "let value = "${REVISION} > $(PROGS)/Revision.ml
# 	@echo "let date = \"${DATE}\"" >> $(PROGS)/Revision.ml
# 	@echo "let time = \"${TIME}\"" >> $(PROGS)/Revision.ml

set-my-path:
	@echo "let mypath = \"${MYPATH}\"" > $(PROGS)/MyPath.ml

# profile: write-revision set-my-path
# 	$(OCAMLBUILD) $(PROFILE_FLAGS) $(OCAMLBUILD_FLAGS) $(OCAML_FLAGS) -libs $(LIBS) $(LEAP).p.native
# 	$(call copy,./_build/$(PROGS)/leap/$(LEAP).p.native,$(BIN),$(LEAP).p.native)

# debug: write-revision set-my-path
# 	$(OCAMLBUILD) $(DEBUG_FLAGS) $(OCAMLBUILD_FLAGS) $(OCAML_FLAGS) -libs $(LIBS) $(LEAP).d.byte
# 	$(call copy,./_build/$(PROGS)/leap/$(LEAP).d.byte,$(BIN),$(LEAP).d.byte)


#$(TRANSLATE).byte: write-revision set-my-path
#$(TRANSLATE).byte: set-my-path
$(TRANSLATE).byte: 
	$(OCAMLBUILD) $(OCAMLBUILD_FLAGS) $(OCAML_FLAGS) -libs $(LIBS) $@
	$(call copy,./_build/$(PROGS)/$@,$(BIN),$@)

#$(TRANSLATE): write-revision set-my-path
#$(TRANSLATE):  set-my-path
$(TRANSLATE):  
	$(OCAMLBUILD) $(OCAMLBUILD_FLAGS) $(OCAML_FLAGS) -libs $(LIBS) $@.native
	$(call copy,./_build/$(PROGS)/$@.native,$(BIN),$@)

#$(BOOL).byte: set-my-path
$(BOOL).byte: 
	$(OCAMLBUILD) $(OCAMLBUILD_FLAGS) $(OCAML_FLAGS) -libs $(LIBS) $@
	$(call copy,./_build/$(PROGS)/$@,$(BIN),$@)
#$(BOOL):  set-my-path
$(BOOL):  
	$(OCAMLBUILD) $(OCAMLBUILD_FLAGS) $(OCAML_FLAGS) -libs $(LIBS) $@.native
	$(call copy,./_build/$(PROGS)/$@.native,$(BIN),$@)

#$(UNROLL).byte: set-my-path
$(UNROLL).byte: 
	$(OCAMLBUILD) $(OCAMLBUILD_FLAGS) $(OCAML_FLAGS) -libs $(LIBS) $@
	$(call copy,./_build/$(PROGS)/$@,$(BIN),$@)
#$(UNROLL):  set-my-path
$(UNROLL):  
	$(OCAMLBUILD) $(OCAMLBUILD_FLAGS) $(OCAML_FLAGS) -libs $(LIBS) $@.native
	$(call copy,./_build/$(PROGS)/$@.native,$(BIN),$@)

#$(GENQBF).byte: set-my-path
$(GENQBF).byte: 
	$(OCAMLBUILD) $(OCAMLBUILD_FLAGS) $(OCAML_FLAGS) -libs $(LIBS) $@
	$(call copy,./_build/$(PROGS)/$@,$(BIN),$@)
#$(GENQBF):  set-my-path
$(GENQBF):  
	$(OCAMLBUILD) $(OCAMLBUILD_FLAGS) $(OCAML_FLAGS) -libs $(LIBS) $@.native
	$(call copy,./_build/$(PROGS)/$@.native,$(BIN),$@)

clean:
	$(OCAMLBUILD) -clean
	@rm -rf $(BIN)/*
	@rm -f leap.odocl


dist:   clean
	tar  zcvf ../${PROJNAME}-`date +"%Y-%m-%d-%H-%M-%S"`.tar.gz \
				--exclude=tmp --exclude=test --exclude=.svn --exclude=*.o . 

# expand:
# 	@echo "Expanding tabs..."
# 	@for i in `find examples/* $(SRC)/* -type f | grep -v \\.swp | grep -v \\.svn | grep -v \\.py` ; do expand -t 2 $$i > temp.file ; mv temp.file $$i; done
# 	@for i in `find examples/* $(SRC)/* -type f | grep \\.sh` ; do chmod +x $$i; done

# unexpand:
# 	@echo "Unexpanding tabs..."
# 	@for i in `find examples/* $(SRC)/* -type f | grep -v \\.swp | grep -v \\.svn | grep -v \\.py` ; do unexpand -t 2 $$i > temp.file ; mv temp.file $$i; done
# 	@for i in `find examples/* $(SRC)/* -type f | grep \\.sh` ; do chmod +x $$i; done

cleartmp:
	@echo "Erasing temporary editor's files..."
	@for i in `find . -type f | grep \\.sw` ; do rm $$i ; done
	@echo "OK"
