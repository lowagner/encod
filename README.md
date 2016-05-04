# encod
randomize any file (unicode or otherwise) with a byte-sized alphabet

## Make the executable

Do a `make` in the directory.

## Usage

As long as the total number of unique characters you use is less than 256
(any utf8 unicode character counts as one character), you can use the program
to encode a file.  The fewer unique characters you use, the more the encoded
file will look like a random binary file.

Run `./encod main.cc` to create a `main.cc-decoder` and `main.cc-encoded` file.
You can then run the program in decode mode by running 
`./encod -d main.cc-decoder main.cc-encoded`, which will create the file
`main.cc-encoded-decoded` which should be the same as the original `main.cc` file.

## Neat stuff

You can then use the program `ent` to see how random the file is:
`ent main.cc-encoded` will give some output.  (Install on Debian/Ubuntu
via  `apt-get install ent`.)

## License

All the files besides `utf8.cc/h` are under the AGPL v3 license (in LICENSE),
and `utf8.cc/h` are distributed with the license found in their headers.  They
have been modified a bit from http://www.cprogramming.com/tutorial/unicode.html
