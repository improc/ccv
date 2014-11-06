rm config.mk
rm makefile
cp config.mk_linux.in config.mk
cp makefile_preconfigured makefile 
make
