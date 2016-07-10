XmlDiff is a small utility program to compare two XML files.  It expects the
files to be mostly in order and cannot do the intelligent ordering that programs
like WinMerge or BeyondCompare can do.  It is great when comparing the output
of two versions of an analysis program, such that the two files will be mostly
the same except small differences.

The program will attempt to split the contents of XML elements into tokens
using white space and commas, then compare them as numbers if possible. If
not numbers they will be compared as text strings. (The delimiter characters
can be changed by the user at run-time.)

An optional configuration file can be used and a sample is included with this
package. Some settings can be specified both in the configuration file and
on the command-line, in which case the command-line value overrides the
configuration file.


Dependencies: TinyXML

The required files from TinyXML are included in this solution as a separate
project since they were small.  This library is available in the Boeing Open
Source Toolkit.
