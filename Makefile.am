
AUTOMAKE_OPTIONS = subdir-objects

sbin_PROGRAMS = gobonet_backend
bin_SCRIPTS = src/gobonet

gobonet_backend_SOURCES = src/gobonet_backend.c

if SETUID_INSTALL
install-data-hook:
	chmod 4755 $(DESTDIR)$(sbindir)/gobonet_backend
endif

