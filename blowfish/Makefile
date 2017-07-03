SRC=bf_cfb64.c \
	bf_enc.c \
	bf_skey.c \
	cipher.c

HDRS=bf_pi.h bf_locl.h

cipher: $(SRC) $(HDRS)
	$(CC) -o cipher $(SRC) -g -O2

