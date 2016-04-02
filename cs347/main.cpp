#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include  <sys/types.h>
#include  <sys/ipc.h>
#include  <sys/shm.h>
#include  <stdio.h>
#include <unistd.h> //for signals
#include <stdio.h>
#include <signal.h>
#include <string.h>
#include <unistd.h>
#include <semaphore.h>
#include <time.h>   // for nanosleep

#define QNUM 50
#define pnum 5
#define tnum 5

int parentpid = 0;
sem_t query_buff_mutex;
pthread_mutex_t threads_info_mutex;

static sigset_t   signal_mask;

//sharede buffer
typedef struct buffer
{
    struct querry
    {
        char data[10];
        long pid;
    }queries[QNUM];
    
    int size;
    int top;

    void init()
    {   
        printf("querry buffer: initalized \n");
        size = QNUM;
        top = 0;
    }

    int push(char dat[], long pid)
    {
        
        if(top != size)
        {
            strcpy(queries[top].data, dat);
            queries[top].pid = pid;
            top++;
            printf("querry buffer: data pushed %s %ld \n", queries[top-1].data, pid );
            return 0;
        }
        else
            return 1;
    }

    int pop(char data[], long* spid)
    {
        if(top != 0)
        {
            strcpy( data, queries[top-1].data);
            *spid = queries[top-1].pid;
            top--;
            printf("querry buffer: in data pop : %s, %ld \n", data, *spid );
            return 0;
        }
        else
        {
            return 1;
        }
    }

}query_buff;

//keep track of processes
struct pidbuff
{
    long pids[pnum];
    int size;
    int top;

    void init()
    {
        size = QNUM;
        top = 0;
    }

    int push(long pid)
    {
        if(top != size)
        {
            pids[top] = pid;
            top++;
            return 0;
        }
        else
            return 1;
    }

    int pop(long* spid)
    {
        if(top != 0)
        {
            *spid = pids[top-1];
            top--;
            return 0;
        }
        else
        {
            return 1;
        }
    }

}pid_buff;

//padd dat ati threads
struct thread_data{
   int  id;
   char data[10];
};

//to keep track of threads
struct current_threads
{
    bool active[tnum];
    
    void init()
    {
        for (int i = 0; i < tnum; ++i)
        {
            active[i] = 0;
        }
    }

    int getid(int &id)
    {   
        for (int i = 0; i < tnum ; ++i)
        {
            if (active[i] == 0)
            {
                active[i] = 1;
                id = i;
                printf("        Thread_pool : id %d issued\n", id);
                return 0;
            }
        }
        //thread pool filled
        return -1;
    }

    void removeid(int id)
    {
        printf("        Thread_pool : id %d removed\n", id);
        active[id] = 0;
    }
}threads_info;

query_buff *queries;
struct sigaction act;
struct thread_data t[tnum];


void sleep_ms(int milliseconds) // cross-platform sleep function
{
    struct timespec ts;
    ts.tv_sec = milliseconds / 1000;
    ts.tv_nsec = (milliseconds % 1000) * 1000000;
    nanosleep(&ts, NULL);
}


void generate_id(char *dest, size_t length) {
    char charset[] = "0123456789"
                     "abcdefghijklmnopqrstuvwxyz"
                     "ABCDEFGHIJKLMNOPQRSTUVWXYZ";

    while (length-- > 0) {
        size_t index = (double) rand() / RAND_MAX * (sizeof charset - 1);
        *dest++ = charset[index];
    }
    *dest = '\0';
}

//the query handler thread
void *query_handler(void *t) 
{
    int i;
    struct thread_data *my_data;
    my_data = (struct thread_data *) t;
    long my_id = (long)my_data->id;
   
    printf("Starting query handler(): thread %ld ; query %s\n", my_id, my_data->data);

    //do dummy compute
    int result;
    for (int i = 0; i < 10000; ++i)
    {
        for (int j = 0; j < 1000; ++j)
        {
            result = i*j;
        }
    }

    sleep_ms(100);

    //acknowledge that it has completed
    pthread_mutex_lock (&threads_info_mutex);
    threads_info.removeid(my_id);
    pthread_mutex_unlock (&threads_info_mutex);
    
    pthread_exit(NULL);
}

//the query maker process
void query_maker() 
{
 
    printf("    query maker %d: Starting \n", getpid());

    while(1)
    {
        printf("    query maker %d: generateing query \n", getpid());
        char id[10] = "aabbccdde";
        //generate query
        //generate_id(&id, 10);
        
       //attemp data push
        int flag = 0;
        while(1)
        {
            sem_wait(&query_buff_mutex);
            flag = queries->push(id,getpid());
            sem_post(&query_buff_mutex);
            
            if (flag == 0)
            {
                //data write sucessfull
                printf("    query maker %d: data write sucessfull \n", getpid());                
                break;
            }
            
            printf("    query maker %d: data write EROR !!!!!%d \n", getpid(),flag);                
            //wait until data is available            
            kill(parentpid, 30);
            raise(SIGSTOP); //SIGCONT
        }

        //printf("query maker %d : dummy enter\n", getpid());
        
        //do dummy compute
        int result;
        for (int i = 0; i < 10000; ++i)
        {
            for (int j = 0; j < 1000; ++j)
            {
                result = i*j;
            }
        }
        
        printf("sleeping query maker(): process %d\n", getpid());
        sleep_ms(100);
    }
}


void sighandler(int signum, siginfo_t *info, void *ptr)
{
    printf("Main thread %d : Received signal %d\n",getpid() , signum);
    printf("Main thread : Signal originates from process %lu\n",
        (unsigned long)info->si_pid);
    if (signum == 30)
    {
        //append process pid to wakr buffer
        pid_buff.push((long)info->si_pid);
    }
    else if(signum == 10)
    {
        //wake a  query maker process
        long spid;
        pid_buff.pop(&spid);
        kill(spid, SIGCONT);
    }
}

    
int main (int argc, char *argv[])
{
    //initalize shared memory
    int id;
    key_t key = 12222;
    id = shmget(key, sizeof(query_buff), IPC_CREAT | 0644);
    if (id < 0) {
        printf("MAIN THREAD: Shmget error !!!!!!!\n");
        exit(1);
    }
    queries = (query_buff *) shmat(id, NULL, 0);
    sem_init(&query_buff_mutex, 1, 1);

    parentpid = getpid();
    int childpid[pnum];
    threads_info.init();
    queries->init();
    pid_buff.init();


    printf("parent: creating childs\n");
    //create query maker processes
    for (int i = 0; i < pnum; ++i)
    {
        if(getpid() == parentpid)
        {
            int cpid = fork();
            childpid[i] = cpid;
        }
    }
    

    if(getpid() == parentpid)
    {
        //action for parent process
        printf("parent procees action initalized\n");

        //set the signal handler for pid queue
        //signal(SIGINT, sighandler);
        memset(&act, 0, sizeof(act));
        act.sa_sigaction = sighandler;
        act.sa_flags = SA_SIGINFO;
        sigaction(SIGTERM, &act, NULL);
        
        //create threads
        pthread_t threads[tnum];

        int flag=0;
        while(1)
        {
            char data[10];
            long spid;
            
            //fetch data
            printf("parent: fetching data from buffer \n");
            sem_wait(&query_buff_mutex);
            flag = queries->pop(data,&spid);
            sem_post(&query_buff_mutex);
            
            if(flag != 0)
            {
                printf("parent : buffer empty [%d]!! waiting \n",flag);
                sleep_ms(10);
                continue;
            }
            else
            {
                while(1)
                {
                    //generate a thread
                    printf("parent: generateing a worker for : %s \n", data);
                    int tid;
                    pthread_mutex_lock (&threads_info_mutex);
                    int trc = threads_info.getid(tid);
                    pthread_mutex_unlock (&threads_info_mutex);
                    strcpy(t[tid].data, data);
                    t[tid].id = tid;

                    if (trc == 0)
                    {   
                        //thread availabe
                        int rc = pthread_create(&threads[tid], NULL, query_handler, (void *) &t[tid]);
                        if (rc != 0)
                        {
                            printf("main Thread: cant create thread !!! \n");
                            pthread_mutex_lock (&threads_info_mutex);
                            threads_info.removeid(tid);
                            pthread_mutex_unlock (&threads_info_mutex);
                            continue;
                        }
                        else
                        {
                            printf("main Thread: worker %d generated \n", tid);
                            break;
                        }
                    }
                    else
                    {
                        //no threads available
                        printf("main Thread: Thread pool filled !!! sleeping\n");
                        sleep_ms(10);
                        continue;
                    }
                }
            }
        }

        shmdt(&queries);
        exit(0);
    }
    else
    {
        //action for maker processes
        query_maker();
    }
}
