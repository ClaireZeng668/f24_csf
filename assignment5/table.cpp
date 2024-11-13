#include <cassert>
#include "table.h"
#include "exceptions.h"
#include "guard.h"

Table::Table( const std::string &name )
  : m_name( name ), table()
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
  // TODO: implement
}

void Table::unlock()
{
  // TODO: implement
}

bool Table::trylock()
{
  // TODO: implement
}

void Table::set( const std::string &key, const std::string &value )
{
  table[key] = value;
}

std::string Table::get( const std::string &key )
{
  return table.at(key);
}

bool Table::has_key( const std::string &key )
{
  return table.count(key);
}

void Table::commit_changes()
{
  // TODO: implement
}

void Table::rollback_changes()
{
  // TODO: implement
}
