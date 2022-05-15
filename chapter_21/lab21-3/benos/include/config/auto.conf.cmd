deps_config := \
	init/Kconfig \
	arch/arm64/Kconfig

include/config/auto.conf: \
	$(deps_config)

$(deps_config): ;
