pkglib_LTLIBRARIES += parallel/parallel.la

parallel_parallel_la_CPPFLAGS =
parallel_parallel_la_CPPFLAGS += $(AM_CPPFLAGS)
parallel_parallel_la_CPPFLAGS += $(JANSSON_CPPFLAGS)

parallel_parallel_la_LDFLAGS =
parallel_parallel_la_LDFLAGS += $(AM_LDFLAGS)
parallel_parallel_la_LDFLAGS += $(JANSSON_LDFLAGS)
parallel_parallel_la_LDFLAGS  += $(XERCES_LDFLAGS)
parallel_parallel_la_LDFLAGS += $(AM_LDFLAGS)

parallel_parallel_la_LIBADD = -ldl
parallel_parallel_la_LIBADD += -lzmq
parallel_parallel_la_LIBADD += -ljansson

parallel_parallel_la_SOURCES =
parallel_parallel_la_SOURCES += parallel/init.cpp
parallel_parallel_la_SOURCES += parallel/main.cpp
parallel_parallel_la_SOURCES += parallel/residential.h
parallel_parallel_la_SOURCES += parallel/parallel_class.h
parallel_parallel_la_SOURCES += parallel/parallel_class.cpp
