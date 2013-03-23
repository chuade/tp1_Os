#include <linux/init.h>
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/kthread.h>
#include <linux/sched.h>
#include <linux/pid.h>
#include <linux/list.h>
#include <linux/thread_info.h>
#include <linux/timer.h>
#include <linux/delay.h>
#include <linux/workqueue.h>
#include <linux/semaphore.h>
#include <linux/random.h>
#include <my_kernel_module.h>

//Structure de tache
//Laisser au minimum les informations 
//de base
struct my_task
{
  int weight;
  int priority;
  int nb_required_memory_blocks; 
  int estimated_exec_time;
  struct list_head new_task;
};

int nbre=0;

// semaphore dans le cas ou on a plusieurs threads qui
// appique une ponderation a chaque tache
typedef struct 
{
   int nombre_of_waiting_task;
   struct list_head wainting_tasks;
}semaphore;

//liste des threads en attente d'ecrire
//ou de lire dans le ready queue dans le cas
// ou on utilise plusieurs threads pour ponderer les taches
// et peupler le ready queue
LIST_HEAD(my_tasks);


// thread d'essai
static struct task_struct *my_thread;
static struct task_struct *my_thread2;


/* initilisation des variables pour la 
* semaphore producteur-consommateur
 */

// liste des taches pretes a etre executes
LIST_HEAD(ready_queue_tasks);
// on commence avec trois threads
#define BUFFER_SIZE 3
struct semaphore mutex =__SEMAPHORE_INITIALIZER(mutex,1); 
struct semaphore full = __SEMAPHORE_INITIALIZER(full,0);
struct semaphore empty = __SEMAPHORE_INITIALIZER(empty,BUFFER_SIZE);

//Tableau contenant toute les tâches,
//initialisées pseudo-aléatoirement.
//C'est là que vous trouverez les informations 
//de traitement pour une tâche.
//(Vous pourrez ensuite les copier dans une
//structure de donnée de votre choix si nécessaire)
#define NB_TASK 1
struct my_task my_waiting_tasks[NB_TASK];

//Les blocs mémoire utilisés et/ou non
//utilisés. int == 0 si non utilisé, 1 si oui
#define NB_MEMORY_BLOCKS 10
int my_memory_blocks[NB_MEMORY_BLOCKS];

//Valeurs pseudo-aléatoire pour initialiser
//les temps d'attente active du traitement
//d'une tâche. Ne pas modifier.
#define MAX_PRIORITY_LEVEL 3
#define MAX_TASK_EXECUTION_TIME 100
static int seedN0 = 71;
static int seedN1 = 97;
static int maxRandomValue = 100;
int get_random_fibonacci(int mod)
{	
	int next = (seedN0 + seedN1)%maxRandomValue;
	seedN0 = seedN1;
	seedN1 = next;
	return next%mod;
}

//Fonction d'initialisation des données
//Veuillez ne pas modifier cette fonction
//ainsi que son appel dans le chargement
// de module.
int init(struct my_task* ptasks)
{
	//Ne pas modifier cette fonction
	unsigned int i;
	for (i = 0; i<NB_TASK; ++i)
	{
		ptasks[i].priority = get_random_fibonacci(MAX_PRIORITY_LEVEL);
		ptasks[i].nb_required_memory_blocks = get_random_fibonacci(NB_MEMORY_BLOCKS);
		ptasks[i].estimated_exec_time = get_random_fibonacci(MAX_TASK_EXECUTION_TIME);
		
		
	}                 

	return 0;
}

//fonction producteur qui ajoute une tache dans le ready queue
// apres avoir calcule la ponderation
//indice : indice de la tache dans my_waiting_task
void producteur(int indice)
{
  
	// modifie le weight de la tache 
	//calculate_task_weight(indice);
	 down(&empty);
	 down(&mutex);
	char car = 'p';
	 test_me(&car);
	 //add_task_on_ready_queue(my_waiting_task[indice]);
	 up(&mutex);
	 up(&full);
	
   
}

//fonction qui calcule la ponderation de chaque tache
int calculate_task_weight(int indice)
{
    int weight_task = 10;
    my_waiting_tasks[indice].weight = 10;
    
    return 0;
}

// ajoute la tache dans le ready queue
int add_task_on_ready_queue(struct my_task new_task )
{
     list_add(&new_task, &ready_queue_tasks);

	return 0;
}

//Le consommateur enleve les taches du ready queue
// en fonction de la memoire disponible
void consommateur()
{
    //taches a executer en fonction de la memoire disponible
    struct my_task *ready_task;
    // permettra de prendre chaque tache dans le ready_queue_tasks
    struct my_task task;
    // declarer pr parcourir la liste des taches 
    struct list_head *pos;
    int memoire_disponible = 8;
    int i;
    // on considere pr le moment que le thread scheduler
    // short term prend une seule tache ds le ready queue
   
	down(&full);
	down(&mutex);
	char car = 'c';
	test_me(&car);
	// on enleve la tache du ready queue
	//remove_task_from_ready_queue();
	up(&mutex);
	up(&empty);
	
  
}

int test_me(char *c){	
	int weight_tasks;
	weight_tasks = (my_waiting_tasks[0].priority*0.8) + (my_waiting_tasks[0].nb_required_memory_blocks*0.15) + (my_waiting_tasks[0].estimated_exec_time*0.05); 
	
	printk("produce %s!!\n ",c);
	return 0;
}



/* fontion pour aquerir et relacher la semaphore
    void acquire_semaphore (semaphore *S, task_struct &new_thread)            
    {
      disable_interrupts ();
      up(&my_semaphore);
      if (S.number_of_waiting_threads < 0){ 
	//ajoute le thread qui veut acceder au ready queue
	// a la liste des taches d'ecrire dans le ready queue
	list_add(&new_thread, &S.waiting_tasks);                                
      	enable_interrupts ();

    }

   void release_semaphore (semaphore *S)        
    {
      disable_interrupts ();
      S.number_of_waiting_threads++;

      if (S.number_of_waiting_threads <= 0)
        {
      	//enleve le thread de la liste des threads en attente
	list_remove(&new_thread, &S.waiting_tasks);
        }                                                
      enable_interrupts ();
    }
*/


//Variable contenant le temps depuis la fin
// de l'initialisation des données
static struct timeval nano0;

//Fonction de simulation d'un tâche, que vous devrez appeler
//une fois sur chacune des tâches
int simulate_task_thread_function(struct my_task *ptask,int *mem_addr)
{
	//Ecrire dans la memoire allouee
	int i;
	struct task_struct *current_task = get_current();
	for(i=0;i<ptask->nb_required_memory_blocks;++i)
	{
		mem_addr[i]=current_task->pid;
	}

	// Code ajoute/

	

	//Ne pas modifier directement cette fonction
	struct timeval nano1;
	long int diff;
	mdelay(ptask->estimated_exec_time);
	do_gettimeofday(&nano1);
	diff = (nano1.tv_sec-nano0.tv_sec)*1000;
	diff+= (nano1.tv_usec-nano0.tv_usec)/1000;
	printk("Simulated task execution : waited for %d ms : Time since start of scheduling : %ld msec \n", ptask->estimated_exec_time, diff);
	printk("priority: %ld",ptask->priority);

	//Verifier que les donnees n'ont pas ete modifiees pendant l'attente
	for(i=0;i<ptask->nb_required_memory_blocks;++i)
	{
		if(mem_addr[i]!=current_task->pid) 
		{
			printk("Memory leak\n", ptask->estimated_exec_time, diff);
			break;
		}
	}
	return 0;
}

int test(){	
	int weight_tasks;
	weight_tasks = (my_waiting_tasks[0].priority*0.8) + (my_waiting_tasks[0].nb_required_memory_blocks*0.15) + (my_waiting_tasks[0].estimated_exec_time*0.05); 
	
	printk("This is my weight %d !!\n ", my_waiting_tasks[0].priority);
	return 0;
}


/* Cette fonction est appelee lors du chargement du module */
int simple_init(void)
{
	int i;

	//Print into the kernel log file
        printk(KERN_INFO "Loading Module\n");

	//Tasks initialisation. Ne pas modifier.
	init(my_waiting_tasks);
	do_gettimeofday(&nano0);

	/* code ajoute */
		my_waiting_tasks[0].weight = 0;
		list_add(&(my_waiting_tasks[0]), &ready_queue_tasks);

	char our_thread[8] = "my_thread";
	 my_thread = kthread_create (producteur, NULL, our_thread);
	char our_thread2[8] = "my_thread2";
 	my_thread2 = kthread_create (consommateur, NULL, our_thread2);
         if((my_thread)){
		printk(KERN_INFO "thread producteur ");		
		wake_up_process(my_thread);
		printk("\n");
	}
	if((my_thread2)){
		printk(KERN_INFO "thread consommateur");		
		wake_up_process(my_thread2);
		printk("\n");
	} 
	 

	/* Fin du code ajoute */     

	//Task execution simulation
	//Ici, le traitement des tâches est réalisé séquentiellement
	//par une boucle for, ce qui contraint chaque iteration à attendre 
	//la fin de la précédente.
	for (i = 0; i<NB_TASK; ++i)
	{
		int first_elt = 0;
		int *task_first_mem_elt_addr=&(my_memory_blocks[first_elt]);
		//simulate_task_thread_function(&(my_waiting_tasks[i]),task_first_mem_elt_addr);
	}
	
       return 0;
}

/* Cette fonction est appelee lors du decargement du module. */
void simple_exit(void) 
{
	//Print into the kernel log file
	printk(KERN_INFO "Removing Module\n");
}

/* Macros d'enregistrement des points d'entree et de sortie du module. */
module_init( simple_init );
module_exit( simple_exit );

MODULE_LICENSE("GPL");
MODULE_DESCRIPTION("Simple Module");
MODULE_AUTHOR("SGG");
