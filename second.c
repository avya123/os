#include <pthread.h>		
#include <semaphore.h>		
#include <stdio.h>	
#include<math.h>		//for using random function.


//pthread_t *Students[100];

pthread_t *Students;		
pthread_t TA;		

int ChairsCount = 0;
int CurrentIndex = 0;

sem_t TA_Sleep;
sem_t Student_Sem;
sem_t ChairsSem[3];
pthread_mutex_t ChairAccess;

void *Student_Activity(void *threadID) 
{

	printf("Student %ld is doing programming assignment.\n", (long)threadID);
	sleep(3);		

	printf("Student %ld needs help from the TA\n", (long)threadID);

	pthread_mutex_lock(&ChairAccess);
	int count = ChairsCount;
	pthread_mutex_unlock(&ChairAccess);
	if(count < 3)		//Student tried to sit on a chair.
	{
		if(count == 0)		//If student sits on first empty chair, wake up the TA.
		sem_post(&TA_Sleep);

		else
			printf("Student %ld sat on a chair waiting for the TA to finish. \n", (long)threadID);
		
		// lock
		pthread_mutex_lock(&ChairAccess);
		int index = (CurrentIndex + ChairsCount) % 3;
		ChairsCount++;
		printf("Student sat on chair.Chairs Remaining: %d\n", 3 - ChairsCount);
		pthread_mutex_unlock(&ChairAccess);

		// unlock
		sem_wait(&ChairsSem[index]);		//Student leaves his/her chair.
		printf("\t Student %ld is getting help from the TA. \n", (long)threadID);
		sem_wait(&Student_Sem);		//Student waits to go next.
		printf("Student %ld left TA room.\n",(long)threadID);
	}
	
	else 
		printf("Student %ld will return at another time. \n", (long)threadID);
		//If student didn't find any chair to sit on.
}

// ------------------------------------------------------------------------------------------

void *TA_Activity()
{
	sem_wait(&TA_Sleep);		//TA is currently sleeping.
	printf("\n\tTA has been awakened by a student.\t\n");

	while(1)
	{
		// lock
		pthread_mutex_lock(&ChairAccess);
		
		if(ChairsCount == 0) 
		{
			//if chairs are empty, break the loop.
			pthread_mutex_unlock(&ChairAccess);
			break;
		}
		//TA gets next student on chair.
		sem_post(&ChairsSem[CurrentIndex]);
		ChairsCount--;
		printf("Student left his/her chair. Remaining Chairs %d\n", 3 - ChairsCount);
		CurrentIndex = (CurrentIndex + 1) % 3;
		pthread_mutex_unlock(&ChairAccess);

		printf("\t TA is currently helping the student.\n");
		sem_post(&Student_Sem);
		sleep(3);
	}
}

// ------------------------------------------------------------------------------------------

void main()
{
	int cstudents;		
	int i,input;

	sem_init(&TA_Sleep, 0, 0);
	sem_init(&Student_Sem, 0, 0);
	
	for(i = 0; i < 3; i++)			//Chairs array of 3 semaphores.
	{	
		sem_init(&ChairsSem[i], 0, 1);		
	}

	//input=0;
	input=rand()%10;
	pthread_mutex_init(&ChairAccess, NULL);
	
	if(input<=1)
	{
		printf("Number of Students not specified. Using default (4) students.\n");
		cstudents = 4;
	}
	
	else
	{
		printf("Number of Students specified. Creating %d threads.\n",cstudents);
		cstudents = input;
	}
	
	Students = malloc(sizeof(cstudents));	//create size of n no. of students.
	
	pthread_create(&TA, NULL, TA_Activity, NULL);	
	
	for(i = 0; i <cstudents; i++)
	{
		pthread_create(&Students[i], NULL, Student_Activity,(void*) (long)i);
	}
	
	pthread_join(TA, NULL);
	
	for(i = 0; i <cstudents; i++)
	{
		pthread_join(Students[i], NULL);
	}
}
