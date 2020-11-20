all:
	@make -C capi install
	@make -C gluTest

clean:
	@make -C capi distclean
	@make -C gluTest clean
