#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include  <sys/types.h>
#include  <sys/ipc.h>
#include  <sys/shm.h>
#include  <stdio.h>
#include <unistd.h> //for signals
#include <stdio.h>
#include <string>
#include <unistd.h>
#include <semaphore.h>
#include <time.h>   // for nanosleep
#include <sys/time.h>
#include <iostream>
#include <fstream>
#include <sstream>

#define NUM_THREADS 5 
#define MAX_SEATS 4
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

    int get_count()
    {
        int count=0;
        for (int i = 0; i < NUM_THREADS ; ++i)
        {
            if(active[i]==1){
                count++;
            }
        }
        return count;
    }
}threads_info;

typedef struct querry_flight_s{
    enum operations{STATUS=0, BOOK=1, CANCEL=2} operation;
    int flight_id;
    int handling_thread;
    int id;
}querry_flight;

pthread_mutex_t db_mutex;
pthread_mutex_t cout_mutex;
pthread_mutex_t threads_info_mutex;
pthread_cond_t thread_pool_cv;

pthread_mutex_t worker_mutex[NUM_THREADS];
pthread_cond_t worker_cv[NUM_THREADS];
querry_flight querries[NUM_THREADS];

void sleep_ms(int milliseconds) // cross-platform sleep function
{
    struct timespec ts;
    ts.tv_sec = milliseconds / 1000;
    ts.tv_nsec = (milliseconds % 1000) * 1000000;
    nanosleep(&ts, NULL);
}


void *worker_thread(void *threadid)
{
    long tid;
    tid = (long)threadid;
    querry_flight *querry;
    querry = &querries[tid];
    
    // cout<<"worker thread : "<<tid<<" created"<<endl;

    while(true)
    {
        pthread_mutex_lock(&worker_mutex[tid]);
        pthread_cond_wait(&worker_cv[tid], &worker_mutex[tid]);
        pthread_mutex_unlock(&worker_mutex[tid]);                  

        pthread_mutex_lock(&cout_mutex);
        // cout<<"     enter : "<<querries[tid].id<<" "<<tid<<endl;
        pthread_mutex_unlock(&cout_mutex);
        // querry = (querry_flight *) vquerry;
        
        string enum_names[3]={"status", "book", "cancel"};
        string status;


        if(querry->operation == querry_flight_s::STATUS) {
        
            pthread_mutex_lock(&db_mutex);
            if(flight_seats[querry->flight_id] < MAX_SEATS){
                status = "available";
            }else{
                status = "not available";
            }
            pthread_mutex_unlock(&db_mutex);
        
        } else if(querry->operation == querry_flight_s::BOOK) {
        
            pthread_mutex_lock(&db_mutex);
            if(flight_seats[querry->flight_id] < MAX_SEATS){
                flight_seats[querry->flight_id]++;
                status = "seats " + std::to_string(flight_seats[querry->flight_id]);
            }else{
                status = "flight full";
            }
            pthread_mutex_unlock(&db_mutex);
        
        } else if(querry->operation == querry_flight_s::CANCEL) {
        
            pthread_mutex_lock(&db_mutex);
            if(flight_seats[querry->flight_id] > 0){
                flight_seats[querry->flight_id]--;
                status = "seats "+ std::to_string(flight_seats[querry->flight_id]);
            }else{
                status = "flight empty";
            }
            pthread_mutex_unlock(&db_mutex);        
        }
        // sleep_ms(10);
        pthread_mutex_lock(&cout_mutex);
        cout<<"worker thread: "<< querry->handling_thread<<" operation: ("<<querry->id<<", "<<querry->flight_id<<", "<<enum_names[querry->operation]<<") result: "<<status<<endl;
        pthread_mutex_unlock(&cout_mutex);

        pthread_mutex_lock(&threads_info_mutex);
        threads_info.removeid(querry->handling_thread);
        pthread_cond_signal(&thread_pool_cv);
        pthread_mutex_unlock(&threads_info_mutex);
    }
    // delete querry;
    pthread_exit(NULL);
}

int main(int argc, char *argv[])
{
	pthread_t threads[NUM_THREADS];
	string line;

	ifstream trans_file ("transactions");
	if (trans_file.is_open())
	{

        for(int t=0; t<NUM_THREADS; t++){
            long tid=t;
            int rc = pthread_create(&threads[tid], NULL, worker_thread, (void *)tid);
            if (rc != 0)
            {
                printf("main Thread: cant create thread !!! \n");
                sleep_ms(10);
                continue;
            }else
            {
                pthread_mutex_lock(&cout_mutex);
                printf("main Thread: created thread %d !!! \n", t);
                pthread_mutex_unlock(&cout_mutex);
            }
        }

		while ( getline (trans_file, line) )
		{
            // cout<<"     "<<line<<endl;
            // sleep_ms(10);
    		if(line == "END"){
    			cout<<"All queries have been processed"<<endl;
                break;
    		}
            int flight_id, qid, op, tid;
            string operation;
            istringstream ss(line);
            ss>>qid>>flight_id>>operation;

            if(! (flight_id < NUM_FLIGHTS)){
                cout<<"Invalid flight no "<<flight_id<<endl;
                continue;
            }
            if(operation=="status"){
               op = 0; 
            }else if(operation=="book"){
                op = 1;
            }else if(operation=="cancel"){
                op = 2;
            }else{
                cout<<"invalid operation"<<endl;
                continue;
            }

            while(true)
            {
                int trc = threads_info.getid(tid);
                if(trc == 0){
                    //thread availabe
                    
                    // querry_flight* querry = new querry_flight();
                    querries[tid].operation = (querry_flight_s::operations)op;
                    querries[tid].flight_id = flight_id;
                    querries[tid].handling_thread = tid;
                    querries[tid].id = qid;

                    // querry[tid] = *querry;

                    pthread_mutex_lock(&worker_mutex[tid]);
                    pthread_cond_signal(&worker_cv[tid]);
                    pthread_mutex_unlock(&worker_mutex[tid]);

                    pthread_mutex_lock(&cout_mutex);
                    printf("main Thread: worker %d assigned to querry %d \n", tid, querries[tid].id);
                    pthread_mutex_unlock(&cout_mutex);
                    break;
                }else{
                    //no threads available
                    pthread_mutex_lock(&cout_mutex);
                    printf("main Thread : Thread pool filled !!! sleeping\n");
                    pthread_mutex_unlock(&cout_mutex);

                    pthread_mutex_lock(&threads_info_mutex);
                    pthread_cond_wait(&thread_pool_cv, &threads_info_mutex);
                    pthread_mutex_unlock(&threads_info_mutex);
                    
                    pthread_mutex_lock(&cout_mutex);
                    printf("main Thread : Thread pool vacent !!! waking up\n");
                    pthread_mutex_unlock(&cout_mutex);

                    continue;
                }
            }
		}

        void* status;
        for(int t=0; t<NUM_THREADS; t++) {
            pthread_join(threads[t], &status);
        }
		trans_file.close();
	}
}


