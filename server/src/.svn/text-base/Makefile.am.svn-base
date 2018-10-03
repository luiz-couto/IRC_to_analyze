bin_PROGRAMS = textual

AM_CXXFLAGS = -I"../include" $(INTI_CFLAGS) -D_DEBUG

textual_SOURCES = auxiliar.cpp ban.cpp channels.cpp \
	client.cpp configreader.cpp database.cpp dispatcher.cpp \
	logger.cpp main.cpp mainframe.cpp server.cpp \
	user.cpp wildcard.cpp
textual_LDADD = $(INTI_LIBS)

