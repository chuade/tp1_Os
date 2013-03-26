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


// thread d'essai
static struct task_struct *my_thread;
static struct task_struct *my_thread2;

// structure pour gerer le memory manager
struct task_in_memory
{
   struct my_task my_task_in_memory;
   struct list_head head_memory_task;
   int last_use_memory_block; // dernier index ou commence la memoire disponible
   int rest_of_memory_blocks;
   // index du debut d'ecriture dans la memoire
   // chaque tache garde cet index
   int started_index_in_memory; 
}tasks;
// liste des taches en memoires
LIST_HEAD(list_of_task_in_memory);

//structure pour gerer le memory manager
struct partition_memory 
{
    int debut;
    int fin;
    int taille;
    int isfree; // dit si la partition est libre ou occupee
    struct list_head head_partition_memory;
};
LIST_HEAD(partition_memory_list);

//liste des threads choisi par le scheduler 
//short-term pour etre execute en attente d'ecution 
//LIST_HEAD(scheduler_short_term_tasks);


// liste des taches dans le ready queue
struct scheduler_task_list
{
  struct my_task scheduler_task;
  struct list_head head_scheduler_task;
};
// on initialise une liste qui va contenir
// la liste des taches du scheduler-short-term
LIST_HEAD(ready_queue_tasks);

//structure du memory manager pour garder une trace des taches
// en memoire et un pointeur du dernier block memoire utilise


// semaphore pour gerer la l,ajout d'une tache dans le ready queue par le scheduler
// long-term et la lecture et la suppression de la tache
// dans le ready queue par le scheduler-short-term 
#define BUFFER_SIZE 3
struct semaphore mutex =__SEMAPHORE_INITIALIZER(mutex,1); 
struct semaphore full = __SEMAPHORE_INITIALIZER(full,0);
struct semaphore empty = __SEMAPHORE_INITIALIZER(empty,BUFFER_SIZE);

// semaphore pour gerer plusieurs producteurs qui veulent
// ajouter une tache dans le reay queue. Ceci c'est dans le cas
// ou on a plus de 1000 taches
struct semaphore semaphore_task = __SEMAPHORE_INITIALIZER(semaphore_task,0);



//Tableau contenant toute les tâches,
//initialisées pseudo-aléatoirement.
//C'est là que vous trouverez les informations 
//de traitement pour une tâche.
//(Vous pourrez ensuite les copier dans une
//structure de donnée de votre choix si nécessaire)
#define NB_TASK 10
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
{	 int nbre=1;
     up(&semaphore_task);
    // le thread entre dans sa section critique
    printk("Je suis dans ma section critique %d\n", nbre++);
    down(&semaphore_task);   
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
//Verifie s'il ya de la memoire disponible
// fonction du nombre de block de la ou des taches
// ajoute les tache dans la liste des taches qui vont etre ex
int verify_memory_block()
{
	//on limite le nombre de taches dans la liste du scheduler-short term a 5
	//On prend maximum 5 taches dans le ready queue pour les mettre dans
	// le short-term list s'il ya assez de memoire disponible
	unsigned int i=0;
        struct list_head *p=NULL;
	struct scheduler_task_list *datastr;
	struct task_in_memory *new_task ;

	printk("memoire disponible : %d\n", tasks.rest_of_memory_blocks);
	list_for_each_entry( datastr, &ready_queue_tasks, head_scheduler_task)
	{
	    if((i<5) && (datastr->scheduler_task.nb_required_memory_blocks < tasks.rest_of_memory_blocks))
	    {
		new_task = (struct task_in_memory *)
		    kmalloc(sizeof(struct task_in_memory), GFP_KERNEL);
		new_task->my_task_in_memory = datastr->scheduler_task;
		printk("tache dans la memoire %d et son nombre de blocks %d\n", i, new_task->my_task_in_memory.nb_required_memory_blocks);
		
	   	list_add(&new_task->head_memory_task ,&list_of_task_in_memory);
		tasks.rest_of_memory_blocks -= new_task->my_task_in_memory.nb_required_memory_blocks; 
	    }
	    else	
	 	break;
	   i++;
	}
	struct task_in_memory *t;
	list_for_each_entry( t, &list_of_task_in_memory, head_memory_task){
		printk("nombre de block de chaque tache dans la memoire %d\n", t->my_task_in_memory.nb_required_memory_blocks ); 
	    }
	printk("\n\n");
   return 0;
}

//Fonction qui met a jour la table de partition de la memoire
void update_memory_partition()
{
	LIST_HEAD(temporary_list);
	//structure de test
	 struct partition_memory *new_task1, *new_task2, *new_task3, *new_task4, *new_task5;
		   new_task1 = (struct partition_memory *)
		    kmalloc(sizeof(struct partition_memory), GFP_KERNEL);		
		new_task1->debut = 0;
		new_task1->fin = 2;
		new_task1->isfree = true;
		

	 
		   new_task2 = (struct partition_memory *)
		    kmalloc(sizeof(struct partition_memory), GFP_KERNEL);		
		new_task2->debut = 3;
		new_task2->fin = 6;
		new_task2->isfree = false;
		

		   new_task3 = (struct partition_memory *)
		    kmalloc(sizeof(struct partition_memory), GFP_KERNEL);		 	
		new_task3->debut = 7;
		new_task3->fin = 10;
		new_task3->isfree = true;

		 new_task4 = (struct partition_memory *)
		    kmalloc(sizeof(struct partition_memory), GFP_KERNEL);		 	
		new_task4->debut = 11;
		new_task4->fin = 13;
		new_task4->isfree = true;

		 new_task5 = (struct partition_memory *)
		    kmalloc(sizeof(struct partition_memory), GFP_KERNEL);		 	
		new_task5->debut = 14;
		new_task5->fin = 17;
		new_task5->isfree = false;
		
		list_add(&new_task5->head_partition_memory, &partition_memory_list);
		list_add(&new_task4->head_partition_memory, &partition_memory_list);
		list_add(&new_task3->head_partition_memory, &partition_memory_list);
		list_add(&new_task2->head_partition_memory, &partition_memory_list);
		list_add(&new_task1->head_partition_memory, &partition_memory_list);

   int i=0, j=0, k=0;
   // previous_free dit si la partion precedente est libre
   bool previous_free = false, creerStruct=false;
   struct  partition_memory *p;
   
   list_for_each_entry(p, &partition_memory_list, head_partition_memory)
      {
	if(p->isfree)
	  {
	     if(previous_free)
		{
		    j = p->fin;
		     creerStruct = true;
			
		}
	     else
		{
		   i = p->debut; 
		   j = p->fin;
		   creerStruct = true;
		   previous_free = true;
		}
	//printk("tache dans la partition de la memoire %d\n", p->isfree);
	  }
	  else if(creerStruct)
		{
		  printk("Je ne suis pas vide %d\n", p->isfree);
		   //on cree une structure pour joindre
		   // les partitions libres et successives
		   //et on ajoute dans une nouvelle liste de partition
		   struct partition_memory *new_task;
		   new_task = (struct partition_memory *)
		    kmalloc(sizeof(struct partition_memory), GFP_KERNEL);		
		new_task->debut = i;
		new_task->fin = j;
		new_task->isfree = true;
		list_add(&new_task->head_partition_memory, &temporary_list);
		previous_free = false;
		}
		else
		    {
			i = 0;
			j = 0;
			creerStruct = false;
		    }
      }
	// On retourne la nouvelle liste de partition ou on remplace
        //l'ancienne liste par la nouvelle
	// on desalloue la memoire pour partition_memory_list
	//  partition_memory_list = temporary_list ; a verifier
	struct partition_memory *t;
	list_for_each_entry( t, &temporary_list, head_partition_memory){
		printk("nouvelle liste de partition debut = %d  et fin = %d\n",t->debut, t->fin); 
	    }
}


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


/* Cette fonction est appelee lors du chargement du module */
int simple_init(void)
{
	unsigned int i;

	//Print into the kernel log file
        printk(KERN_INFO "Loading Module\n");

	//Tasks initialisation. Ne pas modifier.
	init(my_waiting_tasks);
	do_gettimeofday(&nano0);

	//declarations des taches dans le memory manager
	struct task_in_memory  my_memory_task;
	//initialisation de la liste des partitons de la memoire
	struct partition_memory *partition;
	/*partition = (struct partition_memory *)
		    kmalloc(sizeof(struct partition_memory), GFP_KERNEL);
		 partition->debut=0;
		 partition->fin=10;
		 partition->taille = 10;
		 partition->isfree = 1;
		INIT_LIST_HEAD(&partition->head_partition_memory);
	list_add(&partition->head_partition_memory, &partition_memory_list);	
 	*/
	
	// tache de test pr le scheduler-short term
	//INIT_LIST_HEAD(&ready_queue_tasks.head_scheduler_task);
	// On prend quelque taches dans my_waiting_task pour
	// simuler le memory manager
	struct list_head *p=NULL;
	struct scheduler_task_list *datastr;
	struct scheduler_task_list *new_task ;
	for (i = 0; i<NB_TASK - 4; ++i)
	{	  
		new_task = (struct scheduler_task_list *)
		    kmalloc(sizeof(struct scheduler_task_list), GFP_KERNEL);		
		new_task->scheduler_task = my_waiting_tasks[i];
		INIT_LIST_HEAD(&new_task->head_scheduler_task);
		printk("tache %d avec priorite %d\n", i, new_task->scheduler_task.priority);
		
	   list_add(&new_task->head_scheduler_task ,&ready_queue_tasks);		
	   
	   
	}
	if(list_empty(&ready_queue_tasks))
	  printk("ya rien");
	else{
	   printk("nombre de tache ds ready queue %d\n", list_empty(&ready_queue_tasks));
	}
	list_for_each_entry( datastr, &ready_queue_tasks, head_scheduler_task){
		printk("ready task priority %d\n", datastr->scheduler_task.nb_required_memory_blocks); 
	    }

	// On initialise le nombre de block du memory manager a 10
	tasks.rest_of_memory_blocks = 10;
	
	//On peuple la liste des taches du memory manager pour test
	// en ajoutant une tache a la liste seulement si on ne depasse pas le
	//nombre de bloc disponible en memoire
	verify_memory_block();

	char our_thread[8] = "my_thread";
	 my_thread = kthread_create (calculate_task_weight, NULL, our_thread);
	char our_thread2[8] = "my_thread2";
 	my_thread2 = kthread_create (calculate_task_weight, NULL, our_thread2);
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
	 update_memory_partition();
	
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
