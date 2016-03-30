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



#define QNUM 50
#define pnum 5
#define tnum 5

int parentpid = 0;
sem_t query_buff_mutex;

typedef struct buffer
{
    struct querry
    {
        char data[10];
        long pid;
    }queries[QNUM];
    
    int size = QNUM;
    int top = 0;

    int push(char *dat, long pid)
    {
        if(top != size)
        {
            queries[top].data = *dat;
            queries[top].pid = pid;
            
            top++;
            return 0;
        }
        else
            return 1;
    }

    int pop(char *data, long* spid)
    {
        if(top != 0)
        {
            *data = queries[top-1].data;
            *spid = queries[top-1].pid;
            top--;
            return 0
        }
        else
        {
            return 1;
        }
    }

}query_buff;

typedef struct pidbuff
{
    long pids[pnum];
    int size = QNUM;
    int top = 0;

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
            spid = pids[top-1];
            top--;
            return 0
        }
        else
        {
            return 1;
        }
    }

}pid_buff;

struct thread_data{
   int  thread_id;
   char *data;
};

char* id = "aaaaaaaaaa";


//the query handler thread
void *query_handler(void *t) 
{
    int i;
    long my_id = (long)t;
    struct thread_data *my_data;
    my_data = (struct thread_data *) t;
   
    printf("Starting query handler(): thread %ld ; query %s\n", my_id);



    pthread_exit(NULL);
}

//the query maker process
void query_maker() 
{
  //long my_id = (long)t;

    printf("Starting query maker(): process \n");

    while(1)
    {
        //generate query
        id = id+1;
        
        //TODO use mutex ;
        //attemp data push or wait
        int flag = 0;
        while(1)
        {
            sem_wait(&query_buff_mutex);
            flag = buff.push(id,getpid()) != 0
            sem_post(&query_buff_mutex);
            
            if (flag == 0)
            {
                //data write sucessfull
                break;
            }
            kill(parentpid, 30);
            raise(SIGSTOP); //SIGCONT
        }

        //do dummy compute
        int result;
        for (int i = 0; i < 10000; ++i)
        {
            for (int j = 0; j < 1000; ++j)
            {
                result = i*j;
            }
        }
        
        sleep(100);
    }
}


struct sigaction act;
void sighandler(int signum, siginfo_t *info, void *ptr)
{
    printf("Received signal %d\n", signum);
    printf("Signal originates from process %lu\n",
        (unsigned long)info->si_pid);
    if (signum == 30)
    {
        //append process pid to wakr buffer
        pid_buff.push((long)info->si_pid);
    }
}

int main (int argc, char *argv[])
{
    
    //initalize shared memory
    int id;
    query_buff *queries;
    id = shmget(key, N * sizeof(FILE_entry), IPC_CREAT | 0644);
    if (id < 0) {
        printf("shmget error\n");
        exit(1);
    }
    queries = (query_buff *) shmat(id, NULL, 0);
    sem_init(&mutex, 1, 1);


    parentpid = getpid();
    int childpid[pnum];

    //create query maker processes
    for (int i = 0; i < pnum; ++i)
    {
        int cpid = fork();
        childpid[i] = cpid;
    }


    if(getpid == parentpid)
    {
        //action for parent process
        
        //set the signal handler for pid queue
        signal(SIGINT, sig_handler);
        memset(&act, 0, sizeof(act));
        act.sa_sigaction = sighandler;
        act.sa_flags = SA_SIGINFO;
        sigaction(SIGTERM, &act, NULL);
        
        //create threads
        pthread_t threads[tnum];

        while(1)
        {
            char data*;
            long spid;
            
            //fetch data
            sem_wait(&query_buff_mutex);
            flag = buff.pop(data,spid);
            sem_post(&query_buff_mutex);
            if(flag != 0)
            {
                sleep(1);
                continue;
            }else
            {
            //generate threads
                struct thred_data t;
                t.data = data;
                t.id = 
                int rc = pthread_create(&threads[t], NULL, query_handler, (void *) &);
            }




        }

        pthread_exit(NULL);
    }
    else
    {
        //action for maker processes
        query_maker();
    }

    
    pthread_exit(NULL);

}
