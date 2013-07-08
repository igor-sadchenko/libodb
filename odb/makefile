# file      : odb/makefile
# copyright : Copyright (c) 2009-2013 Code Synthesis Tools CC
# license   : GNU GPL v2; see accompanying LICENSE file

include $(dir $(lastword $(MAKEFILE_LIST)))../build/bootstrap.make

cxx :=             \
callback.cxx       \
exceptions.cxx     \
database.cxx       \
vector-impl.cxx    \
connection.cxx     \
lazy-ptr-impl.cxx  \
prepared-query.cxx \
query-dynamic.cxx  \
result.cxx         \
schema-catalog.cxx \
session.cxx        \
statement.cxx      \
tracer.cxx         \
transaction.cxx

# Implementation details.
#
cxx +=                      \
details/buffer.cxx          \
details/condition.cxx       \
details/lock.cxx            \
details/mutex.cxx           \
details/shared-ptr/base.cxx

# POSIX-based implementation details.
#
posix_thr_cxx :=             \
details/posix/exceptions.cxx \
details/posix/thread.cxx

# Win32-based implementation details.
#
win32_thr_cxx :=             \
details/win32/init.cxx       \
details/win32/once.cxx       \
details/win32/tls.cxx        \
details/win32/condition.cxx  \
details/win32/thread.cxx     \
details/win32/exceptions.cxx

win32_dll_cxx := details/win32/dll.cxx

cxx_tun := $(cxx)

ifeq ($(libodb_threads),posix)
cxx_tun += $(posix_thr_cxx)
endif

ifeq ($(libodb_threads),win32)
cxx_tun += $(win32_thr_cxx)

$(call include,$(bld_root)/ld/configuration-lib.make) # ld_lib_type

ifeq ($(ld_lib_type),shared)
cxx_tun += $(win32_dll_cxx)
endif
endif

cxx_obj := $(addprefix $(out_base)/,$(cxx_tun:.cxx=.o))
cxx_od  := $(cxx_obj:.o=.o.d)

odb.l             := $(out_base)/odb.l
odb.l.cpp-options := $(out_base)/odb.l.cpp-options

default := $(out_base)/
dist    := $(out_base)/.dist
clean   := $(out_base)/.clean

# Build.
#
$(odb.l): $(cxx_obj)

ifeq ($(libodb_threads),posix)
$(odb.l): -lpthread
endif

$(cxx_obj) $(cxx_od): $(odb.l.cpp-options) $(out_base)/details/config.h
$(odb.l.cpp-options): value := -I$(out_root) -I$(src_root)

ifdef libodb_threads
$(out_base)/details/config.h: | $(out_base)/details/.
	@echo '/* file      : odb/details/config.h'                      >$@
	@echo ' * note      : automatically generated'                  >>$@
	@echo ' */'                                                     >>$@
	@echo ''                                                        >>$@
	@echo '#ifndef ODB_DETAILS_CONFIG_H'                            >>$@
	@echo '#define ODB_DETAILS_CONFIG_H'                            >>$@
	@echo ''                                                        >>$@
ifeq ($(libodb_threads),posix)
	@echo '#define ODB_THREADS_POSIX 1'                             >>$@
endif
ifeq ($(libodb_threads),win32)
	@echo '#define ODB_THREADS_WIN32 1'                             >>$@
endif
ifeq ($(libodb_threads),none)
	@echo '#define ODB_THREADS_NONE 1'                              >>$@
endif
	@echo ''                                                        >>$@
	@echo '#endif /* ODB_DETAILS_CONFIG_H */'                       >>$@
endif

$(call include-dep,$(cxx_od),$(cxx_obj),$(out_base)/details/config.h)

# Convenience alias for default target.
#
$(out_base)/: $(odb.l)

# Dist.
#
$(dist): export sources := $(cxx)
$(dist): export posix_threads_sources := $(posix_thr_cxx)
$(dist): export win32_threads_sources := $(win32_thr_cxx)
$(dist): export win32_dll_sources := $(win32_dll_cxx)

$(dist): export headers := $(subst $(src_base)/,,$(shell find $(src_base) \
-path $(src_base)/details/posix -a -prune -a -false -o \
-path $(src_base)/details/win32 -a -prune -a -false -o \
-name '*.hxx' -o -name '*.ixx' -o -name '*.txx'))
$(dist): export posix_threads_headers := $(subst $(src_base)/,,$(shell find \
$(src_base)/details/posix -name '*.hxx' -o -name '*.ixx' -o -name '*.txx'))
$(dist): export win32_headers := details/win32/windows.hxx
$(dist): export win32_threads_headers := $(foreach \
h,$(win32_headers),$(filter-out $h,$(subst $(src_base)/,,$(shell find \
$(src_base)/details/win32 -name '*.hxx' -o -name '*.ixx' -o -name '*.txx'))))

$(dist): data_dist := details/config-vc.h
$(dist): export extra_dist := $(data_dist) \
libodb-vc8.vcproj libodb-vc9.vcproj \
libodb-vc10.vcxproj libodb-vc10.vcxproj.filters \
libodb-vc11.vcxproj libodb-vc11.vcxproj.filters
$(dist): export interface_version = $(shell sed -e \
's/^\([0-9]*\.[0-9]*\).*/\1/' $(src_root)/version)

$(dist):
	$(call dist-data,$(sources) $(posix_threads_sources) \
$(win32_threads_sources) $(win32_dll_sources) $(headers) \
$(posix_threads_headers) $(win32_headers) $(win32_threads_headers) \
$(data_dist) details/config.h.in)
	$(call meta-vc8proj,$(src_base)/libodb-vc8.vcproj)
	$(call meta-vc9proj,$(src_base)/libodb-vc9.vcproj)
	$(call meta-vc10proj,$(src_base)/libodb-vc10.vcxproj)
	$(call meta-vc11proj,$(src_base)/libodb-vc11.vcxproj)
	$(call meta-automake)

# Clean.
#
$(clean): $(odb.l).o.clean           \
  $(odb.l.cpp-options).clean         \
  $(addsuffix .cxx.clean,$(cxx_obj)) \
  $(addsuffix .cxx.clean,$(cxx_od))
	$(call message,rm $$1,rm -f $$1,$(out_base)/details/config.h)

# Generated .gitignore.
#
ifeq ($(out_base),$(src_base))
$(odb.l): | $(out_base)/.gitignore

$(out_base)/.gitignore: files := details/config.h
$(clean): $(out_base)/.gitignore.clean

$(call include,$(bld_root)/git/gitignore.make)
endif

# How to.
#
$(call include,$(bld_root)/dist.make)
$(call include,$(bld_root)/meta/vc8proj.make)
$(call include,$(bld_root)/meta/vc9proj.make)
$(call include,$(bld_root)/meta/vc10proj.make)
$(call include,$(bld_root)/meta/vc11proj.make)
$(call include,$(bld_root)/meta/automake.make)

$(call include,$(bld_root)/cxx/o-l.make)
$(call include,$(bld_root)/cxx/cxx-o.make)
$(call include,$(bld_root)/cxx/cxx-d.make)
