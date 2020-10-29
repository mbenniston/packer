# packer

A simple command line tool to pass data into a C/C++ header/source format

## Usage

``` packer [-i input_filename] [-o output_filename] [-d header_filename] [-v variable_name] [-t variable_type] [-p] [-c] ```

#### Options:

* -i "filename" (for input file)
* -o "filename" (for output file)
* -v, specifies variable name
* -t, specifies the variable type
* -d, specifies the filename for the header (the header file is ommited by default)
* -p, enables the use of pragma once for header guarding
* -c, enables the use of extern "C" in the header file

## Compiling

#### On *nix platforms:
Run make in the source directory.

#### For other platforms:
Just compile the packer.c source file with the compiler of your choice

## Example

test.txt contains some text data

```
$ packer -i test.txt
const unsigned char var_name[] = { 0x48, 0x65, 0x6C, 0x6C, 0x6F, 0x20, 0x74, 0x68, ... };
```



