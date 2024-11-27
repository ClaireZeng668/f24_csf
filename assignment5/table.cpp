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
  pthread_mutex_init(&this->table_lock, NULL);
}

Table::~Table()
{
  pthread_mutex_destroy(&this->table_lock);
}

void Table::lock()
{
  int result = pthread_mutex_lock(&this->table_lock);
  if (result != 0) {
    throw FailedTransaction("Table failed to lock");
  }
  is_locked = true;
}

void Table::unlock()
{
  int result = pthread_mutex_unlock(&this->table_lock);
  if (result != 0) {
    throw FailedTransaction("Table failed to unlock");
  }
  is_locked = false;
}

bool Table::trylock()
{
  if (is_locked) {
    return false;
  }
  int result = pthread_mutex_trylock(&this->table_lock);
  if (result != 0) {
    throw FailedTransaction("Table failed to lock for a transaction");
  }
  is_locked = true;
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
