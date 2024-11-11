#ifndef GUARD_H
#define GUARD_H

#include <pthread.h>

class Guard {
private:
  pthread_mutex_t &m_lock;

  // copy constructor and assignment operator are prohibited
  Guard( const Guard & );
  Guard &operator=( const Guard & );

public:
  Guard( pthread_mutex_t &lock )
    : m_lock( lock )
  {
    pthread_mutex_lock( &m_lock );
  }

  ~Guard()
  {
    pthread_mutex_unlock( &m_lock );
  }
};

#endif // GUARD_H
