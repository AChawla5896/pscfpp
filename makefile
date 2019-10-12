include src/config.mk
# ==========================================================================
.PHONY: all-cpu util pscf fd1d pspc \
        clean veryclean html clean-html

# ==============================================================================
# Main build targets

all-cpu:
	cd bld; $(MAKE) all

util:
	cd bld; $(MAKE) util

pscf:
	cd bld; $(MAKE) util

fd1d:
	cd bld; $(MAKE) pscf

pspc:
	cd bld; $(MAKE) pspc

pspg:
	cd bld; $(MAKE) pspg

# ==========================================================================
# Test targets

test:
	@cd src/util/tests; $(MAKE) all; $(MAKE) run
	@cd src/pscf/tests; $(MAKE) all; $(MAKE) run
	@cd src/fd1d/tests; $(MAKE) all; $(MAKE) run
	@cat src/util/tests/count >> count
	@cat src/pscf/tests/count >> count
	@cat src/fd1d/tests/count >> count
	@echo " "
	@echo "Summary"
	@cat count
	@rm -f count

# =========================================================================
# Clean targets

clean:
	cd bld; $(MAKE) clean
	cd src; $(MAKE) clean

clean-tests:
	cd src/; $(MAKE) clean-tests

clean-bin:
	-rm -f $(BIN_DIR)/makeDep
	-rm -f $(BIN_DIR)/pscf*
 
veryclean:
	make clean-bin
	cd bld; $(MAKE) veryclean
	rm bld/makefile
	cd src; $(MAKE) veryclean
	cd doc; $(MAKE) clean

# =========================================================================
# HTML Documentation
 
html:
	cd doc; $(MAKE) html

clean-html:
	cd doc; $(MAKE) clean

# ==========================================================================
