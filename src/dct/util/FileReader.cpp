/**
 * Define a data structure to read a file, retrieving the data
 * line by line and breaking the line string into words that are
 * separated by spaces.
 *
 * @author jzhang - created on Jun 26, 2004
 */
#include "FileReader.h"

FileReader::FileReader (char* filename) {
  fid = fopen (filename, "r") ;
  if (!fid) {
    std::cerr << "Can not open the data file " << filename << "\n" ;
    exit (1) ;
  }

  lineBuffer = (char*)malloc (LINE_BUFFER_SIZE) ;
  fgets (lineBuffer, LINE_BUFFER_SIZE, fid) ;
}

FileReader::~FileReader () {
  if (fid != NULL) {
    fclose (fid) ;
  }

  if (lineBuffer != NULL) {
    delete lineBuffer ;
  }
}

bool FileReader::eof () {
  return (feof (fid)) ;
}

bool FileReader::getNextLine 
  (char* resultWords[], int numOfWords) {
  if (!feof (fid)) {
    int wordNo = 0 ;
    char* ptr = strtok (lineBuffer, " ") ;
    while (ptr != NULL && wordNo < numOfWords) {
      resultWords[wordNo] = (char*)malloc (strlen(ptr)+1) ;
      strcpy (resultWords[wordNo], ptr) ;
      wordNo ++ ;

      ptr = strtok (NULL, " ") ;
    }

    // read the next line.
    fgets (lineBuffer, LINE_BUFFER_SIZE, fid) ;

    // if there are not enough words
    if (wordNo < numOfWords) {
      for (int i=0; i<numOfWords; i++) {
        delete resultWords[i] ;
        resultWords[i] = NULL ;
      }
      return false ;
    }

    return true ;
  } else {
    for (int i=0; i<numOfWords; i++) {
      delete resultWords[i] ;
      resultWords[i] = NULL ;
    }
    return false ;
  }
}
