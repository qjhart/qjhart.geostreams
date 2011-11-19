#ifndef FILEREADER_H
#define FILEREADER_H

/**
 * Define a data structure to read a file with the following format
 * per line:
 *   id  <x1min> <y1min> ... <x1max> <y1max> ...
 *
 * @author jzhang - created on Jun 26, 2004
 */
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <iostream>

#define LINE_BUFFER_SIZE 1024

class FileReader {

private:
  FILE *fid ;
  char *lineBuffer ;

public:
  FileReader (char*) ;
  ~FileReader () ;

  bool eof () ;

  /**
   * Read the next line from the file, and break the line string into
   * words that are separated by spaces. This funtion only returns
   * the given number of words by storing these words in "resultWords", and
   * also returns true.
   *
   * If this is end of file, or there are less words than requested,
   * then return false.
   */
  bool getNextLine (char* resultWords[], int numOfWords) ;
} ;

#endif
