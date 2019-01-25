# CC65 Charmap

Creates the charmappings for cc65 assembler and compiler to convert PETSCII to screencode. Might also be usefull for other assemblers supporting the '.charmap' command.

This can be used when you play to write directly to the screen and don't want to use any of the standard output features.

The advantage is that you don't have to annotate every string with scrcode, instead you can just .byte and mix characters and bytes.

Also you can use it in mnemonics as well, which is not possible with the .scrcode macro.

i.E.

lda #'A'<br>
sta $0400

A pull request was created to include this in the cc65 release. If this is not successfull or not yet done, you can also download my fork here: https://github.com/skeetor/cc65
which contains the 'c64_screencode.inc' file which was created with the help of this tool.
