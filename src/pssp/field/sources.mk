pssp_field_= \
  pssp/field/FFT.cpp \
  pssp/field/RField.cpp \
  pssp/field/RFieldDft.cpp \
  pssp/field/FFT.cpp \
  pssp/field/FieldIo.cpp 

pssp_field_SRCS=\
     $(addprefix $(SRC_DIR)/, $(pssp_field_))
pssp_field_OBJS=\
     $(addprefix $(BLD_DIR)/, $(pssp_field_:.cpp=.o))

