int n;
#include <semaphore.h>
sem_t s = 1;

sem_t p, q;
int data;
void *producer(void *) {
	while(1) {
	//int t=produce();
	sem_wait(&p);
	//data=t;
	sem_post(&q);
}

}
void * consumer(void *) {
	while (1) {
		int t;
		sem_wait(&q);
		t=data;
		sem_post(&p);
		//consume(1);
	}

}

int main() {
	return 0;
}
