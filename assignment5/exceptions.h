#ifndef EXCEPTIONS_H
#define EXCEPTIONS_H

#include <stdexcept>

// Exception to indicate that a message was invalid
// (wrong format, unknown command line, too many or too few
// arguments, etc.)
class InvalidMessage : public std::runtime_error {
public:
  InvalidMessage( const std::string &msg )
    : std::runtime_error( msg )
  { }

  ~InvalidMessage()
  { }
};

// Exception to indicate a communication error (e.g.,
// failed to read data from a client socket, failed to
// create or listen on server socket, etc.)
class CommException : public std::runtime_error {
public:
  CommException( const std::string &msg )
    : std::runtime_error( msg )
  { }

  ~CommException()
  { }
};

// Exception indicating a requested protocol operation couldn't be
// performed (e.g., attempt to create a table, but the table
// already exists.)
class OperationException : public std::runtime_error {
public:
  OperationException( const std::string &msg )
    : std::runtime_error( msg )
  { }

  ~OperationException()
  { }
};

// Exception indicating that a transaction has failed because
// a lock that is needed can't be immediately acquired.
// This kind of exception is recoverable, but requires special
// handling.
class FailedTransaction : public std::runtime_error {
public:
  FailedTransaction( const std::string &msg )
    : std::runtime_error( msg )
  { }

  ~FailedTransaction()
  { }
};

#endif // EXCEPTIONS_H
