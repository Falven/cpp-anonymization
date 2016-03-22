//===--- SystemException.hpp - An exception class for system errors.-*- C++ ---*-===//
//
//                          Francisco Aguilera
//
//===----------------------------------------------------------------------------===//
///
/// \file
/// This file contains an Exception class definition that wraps system errors.
/// The class contains the ErrorNumber and ErrorMessage associated with such
/// system error.
///
//===----------------------------------------------------------------------------===//

#ifndef SOCK_EXCEPTION_H
#define SOCK_EXCEPTION_H

#include <string>		// string
#include <exception>	// exception
#include <stdio.h> 		// strerror
#include <string.h>		// strerror
#include <errno.h> 		// errno

class SystemException : public std::exception {
public:
  SystemException() : ErrorNumber(errno), ErrorMessage(strerror(ErrorNumber)) {}

  SystemException(const std::string &Message)
      : ErrorNumber(-1), ErrorMessage(Message) {}

  ~SystemException() throw() {}

  virtual const char *what() const throw() { return ErrorMessage.c_str(); }

private:
  const int ErrorNumber;
  const std::string ErrorMessage;
};

#endif