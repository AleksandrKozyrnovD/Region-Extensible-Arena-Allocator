#include "rwlock.h"

void rwlock_init(rwlock_t *lock)
{
    pthread_mutex_init(&lock->mutex, NULL);
    pthread_cond_init(&lock->readers_cond, NULL);
    pthread_cond_init(&lock->writers_cond, NULL);
    lock->readers_count = 0;
    lock->writers_count = 0;
    lock->waiting_writers = 0;
}

 void read_lock(rwlock_t *lock)
 {
     pthread_mutex_lock(&lock->mutex);
     while (lock->writers_count > 0 || lock->waiting_writers > 0)
     {
         pthread_cond_wait(&lock->readers_cond, &lock->mutex);
     }
     lock->readers_count++;
     pthread_mutex_unlock(&lock->mutex);
 }

 void read_unlock(rwlock_t *lock)
 {
     pthread_mutex_lock(&lock->mutex);
     lock->readers_count--;
     if (lock->readers_count == 0)
     {
         pthread_cond_signal(&lock->writers_cond);
     }
     pthread_mutex_unlock(&lock->mutex);
 }

 void write_lock(rwlock_t *lock)
 {
     pthread_mutex_lock(&lock->mutex);
     lock->waiting_writers++;
     while (lock->readers_count > 0 || lock->writers_count > 0)
     {
         pthread_cond_wait(&lock->writers_cond, &lock->mutex);
     }
     lock->waiting_writers--;
     lock->writers_count++;
     pthread_mutex_unlock(&lock->mutex);
 }

void write_unlock(rwlock_t *lock)
{
    pthread_mutex_lock(&lock->mutex);
    lock->writers_count--;
    if (lock->waiting_writers > 0)
    {
        pthread_cond_signal(&lock->writers_cond);
    }
    else
    {
        pthread_cond_broadcast(&lock->readers_cond);
    }
    pthread_mutex_unlock(&lock->mutex);
}
