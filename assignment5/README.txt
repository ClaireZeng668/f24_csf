CONTRIBUTIONS

Prasi Thapa:
server.h/cpp
client_connection.h/cpp
edits to MS1 files to help MS2 functionality

Claire Zeng:
server.h/cpp
client_connection.h/cpp
edits to MS1 files to help MS2 functionality

Synchronization Report:

All server table objects needed to be synchronized because all clients have concurrent access to them and are able to create 
and modify tables. Synchronization allows clients/threads to create and modify tables in an orderly way such that all behavior 
is defined and prevents data races and lost updates.

Synchronization is done by using pthread mutex locks. Because table operations are not atomic, the thread acquires the locks 
on the table before the operation and unlocks it after it is done so that when one thread acquires the lock to a table, no other 
threads are able to access that table until the first thread releases the lock. 

In transactions, if the thread can not immediately acquire the lock to the table object, it throws a FailedTransaction and 
immediately exits the transaction.
In autocommit mode, the guard object is declared right before the line to execute the request ensuring that the thread has the lock
before it makes any modifications and unlocks the guard object through its destructor when it goes out of scope.
Thus, if another thread attempts to acquire the same lock, it will either fail the transaction (in transaction mode) or wait 
until the lock becomes available (in autocommit mode)

In addition, a thread in transaction mode does not make direct changes to the table but rather proposes changes. Thus, if an 
error were to occur and an exception thrown that causes the thread to exit the transaction, the table does not enter a state of 
being partly changed. Instead, the table is untouched and the proposed changes are rolled back. However, if a transaction does 
succeed, the changes are commited and the table is actually changed. This is also why it is important to lock the table in a 
transaction because in proposing changes, another thread attempting to also modify the table is not allowed to (prevents a data 
race).

There is a vector of tables unique to each ClientConnection object that tracks which tables the thread has acquired 
the lock for. When an exception is thrown or a valid COMMIT message is received while in a transaction, it loops through the vector 
and unlocks all the tables. This is relevant only when in transaction mode because autocommit mode immediately releases the lock 
upon completion of the request.