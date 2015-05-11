## Welcome ##

**GBParsy** is a library of functions that parses the GenBank flatfile, which is a representative and popular sequence format. The library is optimized for speed and efficient use of memory so it can rapidly parse large sequence such as _Arabidopsis_ chromosome for genomic analysis. **GBParsyPy** is additional version of GBParsy for the Python. GBParsyPy adopted GBParsy as a core parser so GBParsyPy inherited all of its features from GBParsy.

## Current Status ##
Version 0.6.1 of GBParsy and GBParsyPy is stable and is recommended for all users.

## Changes ##
  * **Version 0.5.4 (Feb. 29, 2008)** The project name is changed from **GBFP** to **GBParsy**. However, we would not change the program name in order to prevent confusion.
  * **Version 0.5.5 (Apr. 30, 2008)** Fixed segmentation fault for a sequence containing a too long note, such as NC\_003070 (_Arabidopsis thaliana_ chromosome 1, complete sequence) version 6.
  * **Version 0.5.6 (May 14, 2008)** Checked GBParsy by [Valgrind](http://valgrind.org/) for 2660 chromosome, plasmid and organelle sequences downloaded from [GenBank](ftp://ftp.ncbi.nih.gov/genomes/) and fixed a few memory leak bugs ([Valgrind logs](http://gbfp.googlecode.com/files/valgrind_log.tgz)).
  * **Version 0.5.7 (Jun 25, 2008)**
    * Added medline and remark entry in the reference data
    * Increased flexibility to parse LOCUS line in safety
    * Added new option(-s) to seqext and seqext.py to search qualifier data
  * **Version 0.6.0 (July 10, 2008)** Added a Python class, gbparsy, which generates a [SeqRecord](http://biopython.org/wiki/SeqRecord) instance of [BioPython](http://biopython.org/wiki/Main_Page) from a parsed result.
  * **Version 0.6.1 (July 26, 2008)**
    * Fixed memory leaks of GBParsyPy
    * Increased compatibility with [SeqRecord](http://biopython.org/wiki/SeqRecord) of [BioPython](http://biopython.org/wiki/Main_Page)
    * Changed Python example for new GBParsy class

## Publication ##
  * T.-H. Lee, Y.-K. Kim and B.H. Nahm (2008) [GBParsy: A GenBank flatfile parser library with high speed.](http://www.biomedcentral.com/1471-2105/9/321/abstract) _BMC Bioinformatics_, **9:321**.

## Contact ##
If you have comments, questions and suggestions to the GBParsy, please use [Wiki](http://code.google.com/p/gbfp/w/list) or [Issue](http://code.google.com/p/gbfp/issues/list) system or send a mail to thlee@bio.mju.ac.kr.