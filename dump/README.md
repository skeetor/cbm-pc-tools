# Dump

Dump is a tool wich can be used to process files in various ways. The simplest usage is to copy a file.
Since **dump** can copy files from and to archives this is one possible usage.

One important thing to note is that the output file must always come before the inputfile. The commands can usually be abbreviated, so use the help for more details. If no output file is specified, the output goes to the console.

In general the commands are given with '--longoption' or '-shortoption'.

All numeric arguments can be given in any of the supported formats:  
$ae = hex
0xae = hex
0aeh = hex
12h = hex
%1001 = binary
'A' = character

Parameters may usually appear in any order, but the outcome may be different.

To copy a file from an archive simple use it like a directory:  
**Example:**  
```dump -o myfile.txt -i myarchive.zip\dir\myfile.txt```

Currently supported is zip and gz.
Note that writing to archives is not yet implemented, only reading from them.

## Commands:  

**help**: Print a short help
<br>
<br>
<br>
**output**: Output file. Use '-' for console. If no outputfile is specified console is the default. You can change the outputfile by using output multiple times, but only the last one is used.
**Example:**  Copy two files from an archive  
```dump -o myfile2.txt -i myarchive.zip\dir\myfile1.txt -o myfile2.txt -i myarchive.zip\dir\myfile2.txt```
<br>
<br>
<br>
**input**: Input file. Use '-' for console. If no inputfile is specified, console is the default. Note that, if using the console, the input can not be changed after that. Otherwise you can specify multiple input files. Using 'i' multiple times or specifying a list of files has the same effect.  
If additional arguments are given which should affect the input file (i.e. like skip), only the first file is affected. In this case multiple input files have to be specfied with their own set of additional parameters.

**Example:**  Merge two files into a single file  
```dump -o mergedfile.txt -i myfile1.txt myfile2.txt```  
```dump -o mergedfile.txt -i myfile1.txt -i myfile2.txt```  
<br>
<br>
**skip**: Skip the first N bytes from a single input file.

**Example:**  Cut first two bytes (the address) from the next input  
```dump -o mergedfile.txt -s 2 -i myfile1.txt```  
<br>
Merging two files where the first two bytes are ommited only from the first input  
```dump -o mergedfile.txt -s 2 -i myfile1.txt myfile1.txt```  
<br>
Merging two files where the first two bytes are ommited from the first and the first four from the second input  
```dump -o mergedfile.txt -s 2 -i myfile1.txt -s 4 -i myfile1.txt```  
<br>
<br>
**length**: Read only N bytes from the next input

**Example:**  Read fist 8 bytes from input  
```dump -o mergedfile.txt -l 8 -i myfile1.txt```  
<br>
Take the first 8 bytes from input and append a seoncd file  
```dump -o mergedfile.txt -l 8 -i myfile1.txt myfile1.txt```  
<br>
Take the first 8 bytes from each input  
```dump -o mergedfile.txt -l 8 -i myfile1.txt -l 8 -i myfile1.txt```  
<br>
Skip the first two bytes from each input and take the following 8 bytes  
```dump -o mergedfile.txt -s 2 -l 8 -i myfile1.txt -s 2 -l 8 -i myfile1.txt```  
<br>
<br>
**write**: Write custom data to output. The output can be specified in 8 (default)
16, 32 or 64 bit width with the size=N operator.  
If a string or a character is given, you can use the following translations:  
* \n = newline
* \r = Carriage return
* \t = TAB

Any other character after the ```\``` is passed on as provided, so if you want to add a \ you
have to use it two times. i.E. ```\\``` or ```\"``` or ```\'``` for putting quotes.
<br>
<br>
**Example:**  Change the load address to $c000 by writing the new address and skip the address from the original input  
```dump -o mergedfile.txt -w size=16 $c000 -s 2 -i myfile1.txt```  
<br>
Set the load adress to $0801 (similar as above but with 8 bit width) and without skipping the two bytes from the input.
```dump -o mergedfile.txt -w $01 $08 -i myfile1.txt```  
<br>
Append four bytes at the end 
```dump -o mergedfile.txt -i myfile1.txt -w 'A' 'B' 0aeh 0x12```  
<br>
Write a string in ANSI and then in wide character
```dump -o myfile.txt -w "\"ANSI\"" 0 size=16 "\'WIDECHAR'" 0 size=8 '\n'```  
<br>
<br>
**data** writes the data in columns  

```data [<columns>] [dec[=unsigned(default)|signed]|bin|hex[=cbm(default)|asm|c] [<lineprefix>(default=".byte") <header> <postfix> <column separator>]```

* columns : optional number of columns (default is 16)
* format : mandatory
	* dec
		* unsigned (default)
		* signed
	* bin
	* hex
		* cbm (default) : outputs with leading '$' i.E. $ae
		* asm : output with trailing 'h'. If the number starts with a letter a '0' is prepended. i.E. 23h, 0aeh
		* c : output with C-style prefix: 0xae
* lineprefix : optional text which is printed when a new line starts
* header : optional text which is printed once at the top
* postfix : optional text which is printed once at the bottom
* column seperator : optional text which is printed for each column (default = ',')


**Example:**  
Write as an assembler byte definition with 8 columns  
```dump -o mergedfile.txt -d 8 -i myfile1.txt```  
<br>
Write as a C source byte definition with 8 columns. lineprefix
is empty and we define a header and postfix
```dump -o source.c -w "const char mybuffer[] = {" -d 8 hex=c "" "" -i myfile1.txt -w "};\n"```  
<br>
Dump default as unsigned decimals  
```dump -o mergedfile.txt -d dec -i myfile1.txt```  
```dump -o mergedfile.txt -d dec=unsigned -i myfile1.txt```  
<br>
Dump default as signed decimals  
```dump -o mergedfile.txt -d 8 dec=signed -i myfile1.txt```  
<br>
Simple hex dump  
```dump -o mergedfile.txt -d 16 hex "" "" -i myfile1.txt```  
<br>
<br>
**hexdump** Dumps a hexdump with address and ASCII part. The adress and the ASCII can be
removed, which would be the same as the above example.  
You can specify the columns and also the adresswidth.
* ascii (default is ansii)
  * ascii=off No characters
  * ascii=screen Input values are converted form C64 screen characters (not yet implemented)
  * ascii=petsci Converted from PETSCII charset

format
* 
<br>
<br>
**Example:**  
Default hexdump with 16 columns to console
```dump -x -i myfile1.txt```  
<br>
Hexdump with 16 columns and 32 bit adress columns to console
```dump -x 16 32 -i myfile1.txt```  

Hexdump with 8 columns without characters
```dump -o hexdump.txt -x 8 ascii=off -i myfile1.txt```  

Hexdump with 8 columns without adress (adresswidth = 0)
```dump -o hexdump.txt -x 8 0 -i myfile1.txt```  
