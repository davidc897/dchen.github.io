TARGETS = build test stylecheck formatcheck all noskiptest grade clean old_tests

.PHONY: $(TARGETS)

tools/output:
	@mkdir -p tools/output

$(TARGETS): tools/output
	@$(MAKE) -C tools/cppaudit $(MAKECMDGOALS)
