all:
	@make -C capi -j$(shell nproc) install
	@make -C gluTest

clean:
	@make -C capi distclean
	@make -C gluTest clean
