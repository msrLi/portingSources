# -*- makefile -*-
#----------------------------------------------------------------------------
#       GNU ACE Workspace
#
# 
#
# This file was generated by MPC.  Any changes made directly to
# this file will be lost the next time it is generated.
#
# MPC Command:
# /home/johnny/DOC_ROOT/stage-4584/ACE_wrappers/bin/mwc.pl -type gnuace -exclude TAO/TAO_ACE.mwc -workers 8 -recurse -hierarchy -relative ACE_ROOT=/home/johnny/DOC_ROOT/stage-4584/ACE_wrappers -relative TAO_ROOT=/home/johnny/DOC_ROOT/stage-4584/ACE_wrappers/TAO
#
#----------------------------------------------------------------------------

MAKEFILE = GNUmakefile

ifeq ($(findstring k,$(MAKEFLAGS)),k)
  KEEP_GOING = -
endif

include $(ACE_ROOT)/include/makeinclude/macros.GNU

all: MT_Stress_Idl MT_Stress_Client MT_Stress_Server

depend: MT_Stress_Idl-depend MT_Stress_Client-depend MT_Stress_Server-depend

REMAINING_TARGETS := $(filter-out all depend,$(TARGETS_NESTED:.nested=)) $(CUSTOM_TARGETS)

$(REMAINING_TARGETS):
	$(KEEP_GOING)@$(MAKE) -f GNUmakefile.MT_Stress_Idl $(@)
	$(KEEP_GOING)@$(MAKE) -f GNUmakefile.MT_Stress_Client $(@)
	$(KEEP_GOING)@$(MAKE) -f GNUmakefile.MT_Stress_Server $(@)

.PHONY: MT_Stress_Idl
MT_Stress_Idl:
	$(KEEP_GOING)@$(MAKE) -f GNUmakefile.MT_Stress_Idl all

.PHONY: MT_Stress_Idl-depend
MT_Stress_Idl-depend:
	$(KEEP_GOING)@$(MAKE) -f GNUmakefile.MT_Stress_Idl depend

.PHONY: MT_Stress_Client
MT_Stress_Client: MT_Stress_Idl
	$(KEEP_GOING)@$(MAKE) -f GNUmakefile.MT_Stress_Client all

.PHONY: MT_Stress_Client-depend
MT_Stress_Client-depend:
	$(KEEP_GOING)@$(MAKE) -f GNUmakefile.MT_Stress_Client depend

.PHONY: MT_Stress_Server
MT_Stress_Server: MT_Stress_Idl MT_Stress_Client
	$(KEEP_GOING)@$(MAKE) -f GNUmakefile.MT_Stress_Server all

.PHONY: MT_Stress_Server-depend
MT_Stress_Server-depend:
	$(KEEP_GOING)@$(MAKE) -f GNUmakefile.MT_Stress_Server depend

project_name_list:
	@echo MT_Stress_Client
	@echo MT_Stress_Idl
	@echo MT_Stress_Server
