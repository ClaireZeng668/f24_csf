#ifndef VALUE_STACK_H
#define VALUE_STACK_H

#include <stack>
#include <vector>
#include <string>

class ValueStack {
private:
  // TODO: member variable(s)
  std::stack<std::string> val_stack;

public:
  /*
  * ValueStack object constructor
  */
  ValueStack();

  /*
  * ValueStack object destructor
  */
  ~ValueStack();

  /*
  * Checks if ValueStack is empty
  *
  * Returns:
  *     boolean true if empty
  */
  bool is_empty() const;

  /*
  * Checks if ValueStack is empty
  *
  * Parameters:
  *     value - string value to push onto the ValueStack
  */
  void push( const std::string &value );

  /*
  * Gets the top value of ValueStack
  *
  * Returns:
  *     string of top value
  *
  * Throws:
  *     OperationException if ValueStack is empty
  */
  std::string get_top() const;

  /*
  * Removes the top value of ValueStack
  *
  * Throws:
  *     OperationException if ValueStack is empty
  */
  void pop();
};

#endif // VALUE_STACK_H
