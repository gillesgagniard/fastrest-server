lib_LTLIBRARIES = libfastrest_server.la

libfastrest_server_la_SOURCES = \
	server.cpp controller.cpp http.cpp dispatcher.cpp \
	threadpool.cpp
libfastrest_server_la_CPPFLAGS = $(BOOST_CPPFLAGS) -I../ext -I../include
libfastrest_server_la_LDFLAGS = $(BOOST_THREAD_LDFLAGS) -Wl,-Bsymbolic
libfastrest_server_la_LIBADD = \
    $(BOOST_THREAD_LIBS) $(BOOST_SYSTEM_LIBS) \
    $(BOOST_PROGRAM_OPTIONS_LIBS) \
    $(BOOST_LOG_LIBS) $(BOOST_LOG_SETUP_LIBS)
