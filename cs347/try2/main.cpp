#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include  <sys/types.h>
#include  <sys/ipc.h>
#include  <sys/shm.h>
#include  <stdio.h>
#include <unistd.h> //for signals
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <semaphore.h>
#include <time.h>   // for nanosleep
#include <sys/time.h>
#include <iostream>
#include <fstream>
#include <sstream>

#define NUM_THREADS 5 
#define MAX_SEATS 30
#define NUM_FLIGHTS 10

using namespace std;

// shared data
int flight_seats[NUM_FLIGHTS];

// to keep track of threads
struct current_threads
{
    bool active[NUM_THREADS];
    
    void init()
    {
        for (int i = 0; i < NUM_THREADS; ++i)
        {
            active[i] = 0;
        }
    }

    int getid(int &id)
    {   
        for (int i = 0; i < NUM_THREADS ; ++i)
        {
            if (active[i] == 0)
            {
                active[i] = 1;
                id = i;
                //printf(" Thread_pool : id %d issued\n", id);
                return 0;
            }
        }
        //thread pool filled
        return -1;
    }

    void removeid(int id)
    {
        //printf("        Thread_pool : id %d removed\n", id);
        active[id] = 0;
    }
}threads_info;

typedef struct querry_flight_s{
    enum operations{STATUS=0, BOOK=1, CANCEL=2} operation;
    int flight_id;
    int handling_thread;
}querry_flight;

pthread_mutex_t db_mutex;
pthread_mutex_t threads_info_mutex;
pthread_cond_t thread_pool_cv;


void sleep_ms(int milliseconds) // cross-platform sleep function
{
    struct timespec ts;
    ts.tv_sec = milliseconds / 1000;
    ts.tv_nsec = (milliseconds % 1000) * 1000000;
    nanosleep(&ts, NULL);
}


void *worker_thread(void *vquerry)
{
    querry_flight *querry;
    querry = (querry_flight *) vquerry;

    if(querry->operation == querry_flight_s::STATUS) {
        pthread_mutex_lock(&db_mutex);
        if(flight_seats[querry->flight_id] < MAX_SEATS){
            cout<<"available"<<endl;
        }else{
            cout<<"not available"<<endl;
        }
        pthread_mutex_unlock(&db_mutex);
    } else if(querry->operation == querry_flight_s::BOOK) {
        pthread_mutex_lock(&db_mutex);
        if(flight_seats[querry->flight_id] < MAX_SEATS){
            flight_seats[querry->flight_id]++;
        }else{
            cout<<"not available to book"<<endl;
        }
        pthread_mutex_unlock(&db_mutex);
    } else if(querry->operation == querry_flight_s::CANCEL) {
        pthread_mutex_lock(&db_mutex);
        if(flight_seats[querry->flight_id] > 0){
            flight_seats[querry->flight_id]--;
        }else{
            cout<<"not filled"<<endl;
        }
        pthread_mutex_unlock(&db_mutex);        
    }
    pthread_mutex_lock(&threads_info_mutex);
    threads_info.removeid(querry->handling_thread);
    pthread_cond_signal(&thread_pool_cv);
    pthread_mutex_lock(&threads_info_mutex);

    pthread_exit(NULL);
}

int main(int argc, char *argv[])
{
	pthread_t threads[NUM_THREADS];
	string line;

	ifstream trans_file ("transactions");
	if (trans_file.is_open())
	{
		while ( getline (trans_file, line) )
		{
    		if(line == "END"){
    			cout<<"All queries have been processed"<<endl;
                return 0;
    		}
            int flight_id, op, tid;
            istringstream ss(line);
            ss>>flight_id>>op;

            while(true)
            {
                int trc = threads_info.getid(tid);
                if(trc == 0){
                    //thread availabe
                    
                    querry_flight querry;
                    querry.operation = (querry_flight_s::operations)op;
                    querry.flight_id = flight_id;
                    querry.handling_thread = tid;

                    int rc = pthread_create(&threads[tid], NULL, worker_thread, (void *)&querry);
                    if (rc != 0)
                    {
                        printf("main Thread: cant create thread !!! \n");
                        threads_info.removeid(tid);
                        sleep_ms(10);
                        continue;
                    }
                    else
                    {
                        printf("main Thread: worker %d generated \n", tid);
                        break;
                    }
                }else{
                    //no threads available
                    printf("main Thread : Thread pool filled !!! sleeping\n");
                    pthread_mutex_lock(&threads_info_mutex);
                    pthread_cond_wait(&thread_pool_cv, &threads_info_mutex);
                    printf("main Thread : Thread pool vacent !!! waking up\n");

                    continue;
                }
            }
		}
		trans_file.close();
	}
}


