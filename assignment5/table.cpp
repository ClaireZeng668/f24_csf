#include <cassert>
#include <pthread.h>
#include "table.h"
#include "exceptions.h"
#include "guard.h"

Table::Table( const std::string &name )
  : m_name( name ), table(), changes(), has_changes(false)
  // TODO: initialize additional member variables
{
  // TODO: implement
}

Table::~Table()
{
  // TODO: implement
}

void Table::lock()
{
  // TODO: implement (commented code was written before covering mutex in class, please ignore)
  // pthread_mutex_t lock;
  // int result = pthread_mutex_lock(&lock);
  // if (result != 0) {
  //   //handle error
  // }
  // is_locked = true;
}

void Table::unlock()
{
  // TODO: implement (commented code was written before covering mutex in class, please ignore)
  //result = pthread_mutex_unlock(&lock);
  // if (result != 0) {
  //   //handle error
  // }
  //is_locked = false;
}

bool Table::trylock()
{
  // TODO: implement (commented code was written before covering mutex in class, please ignore)
  //if (is_locked) {
    //return false;
  //}
  //result = pthread_mutex_trylock(&lock);
  // if (result != 0) {
  //   //handle error?
  //rollback changes
  //unlock other stuff?
  //return false;
  // }
  //is_locked = true;
  return true;
}

void Table::set( const std::string &key, const std::string &value )
{
  changes[key] = value;
  has_changes = true;
}

std::string Table::get( const std::string &key )
{
  return changes[key];
}

bool Table::has_key( const std::string &key )
{
  return changes.count(key);
}

void Table::commit_changes()
{
  if (has_changes) {
    for (auto it = changes.begin(); it != changes.end(); it++) {
      std::string key = it->first;
      std::string value = it->second;
      table[key] = value;
    }
    has_changes = false;
  }
}

void Table::rollback_changes()
{
  changes = table;
  has_changes = false;
}
