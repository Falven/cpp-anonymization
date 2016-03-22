//===------ EAIException.hpp - An exception class for eai errors.-*- C++ ------*-===//
//
//                          Francisco Aguilera
//
//===----------------------------------------------------------------------------===//
///
/// \file
/// This file contains an Exception class definition that wraps eai errors.
/// EAI errors are errors thrown by functions that handle certain socket activity
/// such as getaddrinfo. This class is just a wrapper around these errors
// to contain the ErrorCode and ErrorMessage associated with such EAI_ERROR.
///
//===----------------------------------------------------------------------------===//

#ifndef EAI_EXCEPTION_H
#define EAI_EXCEPTION_H

#include <sys/types.h>	// gai_strerror
#include <sys/socket.h> // gai_strerror
#include <netdb.h>		// gai_strerror
#include <exception>	// exception
#include <string>		// string

class EAIException : public std::exception {
public:
  EAIException(int ErrorCode)
      : ErrorCode(ErrorCode), ErrorMessage(gai_strerror(ErrorCode)) {}

  ~EAIException() throw() {}

  virtual const char *what() const throw() { return ErrorMessage.c_str(); }

private:
  const int ErrorCode;
  const std::string ErrorMessage;
};

#endif