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
  Table( const std::string &name );
  ~Table();

  std::string get_name() const { return m_name; }

  void lock();
  void unlock();
  bool trylock();
  bool created_in_transaction() {return created;}
  void set_created() {created=true;}

  // Note: these functions should only be called while the
  // table's lock is held!
  void set( const std::string &key, const std::string &value );
  bool has_key( const std::string &key );
  std::string get( const std::string &key );
  void commit_changes();
  void rollback_changes();
};

#endif // TABLE_H