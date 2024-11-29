#include "value_stack.h"
#include "exceptions.h"

ValueStack::ValueStack()
  // TODO: initialize member variable(s) (if necessary)
{
}

ValueStack::~ValueStack()
{
}

bool ValueStack::is_empty() const
{
  return val_stack.empty();
}

void ValueStack::push( const std::string &value )
{
  val_stack.push(value);
}

std::string ValueStack::get_top() const
{
  if (val_stack.empty()) {
    throw OperationException("ValueStack is empty");
  }
  return val_stack.top();
}

void ValueStack::pop()
{
  if (val_stack.empty()) {
    throw OperationException("ValueStack is empty");
  }
  val_stack.pop();
}
