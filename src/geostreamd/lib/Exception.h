#ifndef GEOSTREAM_LIB_EXCEPTION_H
#define GEOSTREAM_LIB_EXCEPTION_H

/**
 * Define classes that represent exceptions.
 *
 * @author jiezhang - created on Mar 21, 2005
 */

#include <string>

namespace Geostream {
  namespace Lib { 
    class Exception {
    public:
      Exception (std::string& msg) : errMsg (msg) {
      }

      Exception (const char *msg) : errMsg (msg) {
      }

      Exception () {
		errMsg = "" ;
      }

      virtual std::string& getErrorMessage () {
		return errMsg ;
      }
    protected:
      std::string errMsg ;
    } ;

    class IllegalStateException : public Exception {
    public:
      IllegalStateException (std::string& msg) {
	errMsg = "IllegalStateException: " + msg ;
      }

      virtual std::string& getErrorMessage () {
	return errMsg ;
      }
    } ;

    class LockException : public Exception {
    public:
      LockException (std::string& msg) {
	errMsg = "LockException: " + msg ;
      }

      virtual std::string& getErrorMessage () {
	return errMsg ;
      }
    } ;

    class SocketException : public Exception {
    public:
      SocketException (std::string& msg) {
	errMsg = "SocketException: " + msg ;
      }

      virtual std::string& getErrorMessage () {
	return errMsg ;
      }
    } ;

    class GeostreamServerException : public Exception {
    public:
      GeostreamServerException (std::string& msg) {
	errMsg = "GeostreamServerException: " + msg ;
      }

      virtual std::string& getErrorMessage () {
	return errMsg ;
      }
    } ;

	class ErrnoException : public Exception {
	public:
	  ErrnoException (void) {
		char err_str[255];
		//		strerror_r(errno,err_str,255);
		errMsg = err_str;
	  }
	};	
  }
}

#endif
