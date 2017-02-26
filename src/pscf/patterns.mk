# ---------------------------------------------------------------------
# File: src/pscf/patterns.mk
#
# This makefile contains the pattern rule used to compile all sources
# files in the directory tree rooted at the src/pscf directory, which
# contains all source code for the Util namespace. It is included by
# all "makefile" files in this directory tree. 
#
# This file should be included in other makefiles after inclusion of
# the files src/config.mk and src/pscf/config.mk because this file
# uses makefile variables defined in those files.
#-----------------------------------------------------------------------

# All pscf-specific libraries needed in src/pscf
PSCF_LIBS=$(pscf_LIB) $(util_LIB) 

# All libraries needed for executables in src/pscf, including external
LIBS=$(PSCF_LIBS)
ifdef PSCF_GSL
LIBS+=$(PSCF_GSL_LIB) 
endif

# Preprocessor macro definitions needed in src/pscf
DEFINES=$(PSCF_DEFS) $(UTIL_DEFS)

# Dependencies on build configuration files
MAKE_DEPS= -A$(BLD_DIR)/config.mk
MAKE_DEPS+= -A$(BLD_DIR)/util/config.mk
MAKE_DEPS+= -A$(BLD_DIR)/pscf/config.mk

# Pattern rule to compile *.cpp class source files in src/pscf
$(BLD_DIR)/%.o:$(SRC_DIR)/%.cpp
	$(CXX) $(CPPFLAGS) $(CXXFLAGS) $(INCLUDES) $(DEFINES) -c -o $@ $<
ifdef MAKEDEP
	$(MAKEDEP) $(INCLUDES) $(DEFINES) $(MAKE_DEPS) -S$(SRC_DIR) -B$(BLD_DIR) $<
endif

# Pattern rule to compile *.cc test programs in src/pscf/tests
$(BLD_DIR)/% $(BLD_DIR)/%.o: $(SRC_DIR)/%.cc $(PSCF_LIBS)
	$(CXX) $(TESTFLAGS) $(INCLUDES) $(DEFINES) -c -o $@ $<
	$(CXX) $(LDFLAGS) $(INCLUDES) $(DEFINES) -o $(@:.o=) $@ $(LIBS)
ifdef MAKEDEP
	$(MAKEDEP) $(INCLUDES) $(DEFINES) $(MAKE_DEPS) -S$(SRC_DIR) -B$(BLD_DIR) $<
endif

