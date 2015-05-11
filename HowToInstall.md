# Install GBParsy and GBParsyPy #
## Common Process ##
  1. Download the compressed library.
    * [gbfp-0.6.1.tgz](http://gbfp.googlecode.com/files/gbfp-0.6.1.tgz)
  1. Uncompress the file into a directory which you want.
```
$ tar xvfz gbfp-0.6.1.tgz
```
## Install GBParsy ##
    1. Make and install the library
```
$ cd gbfp-0.6.1
$ make
$ make install (as a root)
$ cd ..
```
## Install GBParsyPy ##
    1. Make and install the module
```
$ cd gbfp-0.6.1/gbfpy
$ python setup.py build
$ python setup.py install (as a root)
$ cd ../..
```
## Install example program ##
  1. Make and install the example program, seqext
```
$ cd gbfp-0.6.1/example
$ make
$ make install (as a root)
$ cd ..
```