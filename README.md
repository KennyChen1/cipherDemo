Kenny Chen

This programs encrypts or decrypts a file

USEAGE: 	cipher [-devhs] [-p PASSWD] infile outfile

Supports the -s extra credit option.

Without the -p option, it will prompt for a password with a "Enter Password: " prompt and with -s it will ask again with a "Enter Password Again: "

Handles arguement using get opt

With - as the infile, it will read as stdin and reads until EOF which is the ctrl d one char at a time

When succesfully completed it will say "done!\n"