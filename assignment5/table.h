#ifndef TABLE_H
#define TABLE_H

#include <map>
#include <string>
#include <pthread.h>
#include <map>

class Table {
private:
  std::string m_name;
  // TODO: add member variables
  std::map<std::string, std::string> table;
  std::map<std::string, std::string> changes;
  bool has_changes;
  bool is_locked;
  pthread_mutex_t table_lock;
  bool created = false;

  // copy constructor and assignment operator are prohibited
  Table( const Table & );
  Table &operator=( const Table & );

public:
  /*
  * Table object constructor
  * 
  * Parameters:
  *     name - name of table to create
  */
  Table( const std::string &name );

  /*
  * Table object destructor
  */
  ~Table();

  /*
  * Gets the name of the table
  * 
  * Returns:
  *     string name of the table
  */
  std::string get_name() const { return m_name; }

  /*
  * Locks the table with pthread_mutex
  */
  void lock();

  /*
  * Unlocks the table with pthread_mutex
  */
  void unlock();

  /*
  * Tries to lock the table with pthread_mutex
  * Used in transactions
  *
  * Returns:
  *     boolean true if table sucessfully locked
  *
  * Throws:
  *     FailedTransaction if table can not be locked immediately
  */
  bool trylock();

  /*
  * Checks if table was created in a transaction
  *
  * Returns:
  *     boolean true if table was created in a transaction
  */
  bool created_in_transaction() {return created;}

  /*
  * Sets created in a transaction to true
  */
  void set_created() {created=true;}

  // Note: these functions are only called while the table's lock is held
  
  /*
  * Sets the value of the specified key to the specified value in proposed changes
  *
  * Parameters:
  *     key - key to set value
  *     value - specified value to set to the key
  */
  void set( const std::string &key, const std::string &value );

  /*
  * Checks if Table has specified key
  *
  * Parameters:
  *     key - key to search for in the Table
  *
  * Returns:
  *     boolean true if key exists
  */
  bool has_key( const std::string &key );

  /*
  * Gets the value of the specified key
  *
  * Parameters:
  *     key - key to get value from
  *
  * Returns:
  *     string value
  *
  * Throws:
  *     OperationException if key does not exist in Table
  */
  std::string get( const std::string &key );

  /*
  * Copies the proposed changes onto the actual table
  */
  void commit_changes();

  /*
  * Undoes proposed changes by setting proposed changes table to the actual table
  */
  void rollback_changes();

  pthread_mutex_t get_mutex() {return table_lock;}
};

#endif // TABLE_H