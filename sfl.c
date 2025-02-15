//Plesa-Marian-Cosmin 311CA
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>

#define DIE(assertion, call_description)				\
	do {								\
		if (assertion) {					\
			fprintf(stderr, "(%s, %d): ",			\
					__FILE__, __LINE__);		\
			perror(call_description);			\
			exit(errno);				        \
		}							\
	} while (0)

typedef struct info_t info_t;
struct info_t {
	void *data;
	size_t size;
	unsigned long long address;
};

typedef struct dll_node_t dll_node_t;
typedef struct dll_node_t {
	void *data; /* Pentru ca datele stocate sa poata avea orice tip, folosim un
				   pointer la void. */
	dll_node_t *prev, *next;
} dll_node_t;

typedef struct doubly_linked_list_t doubly_linked_list_t;
struct doubly_linked_list_t {
	dll_node_t *head;
	size_t data_size;
	size_t size;
};

typedef struct {
	doubly_linked_list_t **listoflists;
	size_t number_lists, number_bytes_per_list;
	unsigned long long addresstart;
} sfl_t;

doubly_linked_list_t *dll_create(unsigned int data_size)
{
	/* TODO */
	doubly_linked_list_t *newlist = malloc(sizeof(doubly_linked_list_t));
	DIE(!newlist, "eroare");
	newlist->head = NULL;
	newlist->size = 0;
	newlist->data_size = data_size;
	return newlist;
}

dll_node_t *alocarenod(const void *data, size_t data_size,
					   unsigned long long naddress)
{
	dll_node_t *nodulet = malloc(sizeof(dll_node_t));
	nodulet->data = malloc(sizeof(info_t));
	((info_t *)(nodulet->data))->data = malloc(data_size);
	((info_t *)(nodulet->data))->address = naddress;
	((info_t *)(nodulet->data))->size = data_size;
	return nodulet;
}

dll_node_t
*dll_get_nth_node(doubly_linked_list_t *list, unsigned int n)
{
	if (!list)
		return NULL;
	dll_node_t *dorit = list->head;
	if (n == 0)
		return dorit;
	for (int i = 0; i < n; i++)
		dorit = dorit->next;
	return dorit;
}

dll_node_t *dll_get_nth_node_address(doubly_linked_list_t *list,
									 unsigned int *n,
									 unsigned long long address)
{
	if (!list)
		return NULL;
	dll_node_t *dorit = list->head;
	if (((info_t *)dorit->data)->address > address) {
		*n = 0;
		return dorit;
	}
	int i = 0;
	while (i < list->size - 1) {
		dorit = dorit->next;
		if (((info_t *)dorit->data)->address > address)
			return dorit;
		i++;
	}
	return dorit;
}

void
dll_add_nth_node(doubly_linked_list_t *list, unsigned int n,
				 const void *new_data, unsigned long long address,
				 int size)
{
	dll_node_t *newfirst = alocarenod(new_data, size, address);
	if (n > list->size)
		n = list->size;
	if (list->size == 0) {
		list->head = newfirst;
		newfirst->next = newfirst;
		newfirst->prev = newfirst;
		list->size++;
		return;
	}
	dll_node_t *nnewfirst = dll_get_nth_node_address(list, &n, address);
	dll_node_t *bnewfirst = nnewfirst->prev;
	if (((info_t *)nnewfirst->data)->address < address) {
		nnewfirst = nnewfirst->next;
		bnewfirst = bnewfirst->next;
	}
	bnewfirst->next = newfirst;
	nnewfirst->prev = newfirst;
	newfirst->next = nnewfirst;
	newfirst->prev = bnewfirst;
	if (n == 0)
		list->head = newfirst;
	list->size++;
}

doubly_linked_list_t*
dll_add_nth_node_by_node(doubly_linked_list_t *list, unsigned int n,
						 const void *new_data, dll_node_t *nodulet)
{
	if (n > list->size)
		n = list->size;
	if (list->size == 0) {
		list->head = nodulet;
		nodulet->next = nodulet;
		nodulet->prev = nodulet;
		list->size++;
		return list;
	}
	dll_node_t *nnewfirst =
	dll_get_nth_node_address(list, &n, ((info_t *)nodulet->data)->address);
	dll_node_t *bnewfirst = nnewfirst->prev;
	if (((info_t *)nnewfirst->data)->address <
		((info_t *)nodulet->data)->address) {
		nnewfirst = nnewfirst->next;
		bnewfirst = bnewfirst->next;
	}
	bnewfirst->next = nodulet;
	nnewfirst->prev = nodulet;
	nodulet->next = nnewfirst;
	nodulet->prev = bnewfirst;
	if (n == 0)
		list->head = nodulet;
	list->size++;
	return list;
}

dll_node_t *dll_remove_nth_node(doubly_linked_list_t *list, unsigned int n)
{
	if (list->size == 0)
		return NULL;
	if (n > list->size - 1)
		n = list->size - 1;
	dll_node_t *dorit = dll_get_nth_node(list, n);
	dll_node_t *dupa = dorit->next;
	dll_node_t *inapoi = dorit->prev;
	dorit->next->prev = inapoi;
	dorit->prev->next = dupa;

	if (n == 0)
		list->head = dupa;
	list->size--;
	return dorit;
}

sfl_t init_heap(sfl_t *sfl, unsigned long long address,
				size_t numberlists, size_t nrbyteslist)
{
	sfl->listoflists = (doubly_linked_list_t **)
	malloc(numberlists * sizeof(doubly_linked_list_t *));
	sfl->number_lists = numberlists;
	sfl->addresstart = address;
	sfl->number_bytes_per_list = nrbyteslist;
	unsigned long long current_address = address;
	int currsize = 8;
	for (int i = 0; i < numberlists; i++) {
		sfl->listoflists[i] = (doubly_linked_list_t *)
		malloc(sizeof(doubly_linked_list_t));
		sfl->listoflists[i]->head = NULL;
		sfl->listoflists[i]->data_size = currsize;
		sfl->listoflists[i]->size = 0;
		int x = nrbyteslist / currsize;
		int j = 0;
		while (j != x) {
			dll_add_nth_node(sfl->listoflists[i], j,
							 NULL, current_address, currsize);
			j++;
			current_address = current_address + currsize;
		}
		currsize = currsize * 2;
	}
	return (*sfl);
}

void dump_mem(sfl_t *sfl, doubly_linked_list_t *list_all,
			  unsigned int mcalls, size_t all_mem, unsigned int x, int fc)
{
	dll_node_t *current;
	size_t total_memory = 0;
	size_t free_blocks = 0;
	size_t free_mem = 0;
	for (size_t i = 0; i < sfl->number_lists; i++) {
		current = sfl->listoflists[i]->head;
		free_blocks = free_blocks + sfl->listoflists[i]->size;
		free_mem = free_mem +
		sfl->listoflists[i]->size * sfl->listoflists[i]->data_size;
	}
	total_memory = free_mem + all_mem;
	printf("+++++DUMP+++++\n");
	printf("Total memory: %ld bytes\n", total_memory);
	printf("Total allocated memory: %ld bytes\n", all_mem);
	printf("Total free memory: %ld bytes\n", free_mem);
	printf("Free blocks: %ld\n", free_blocks);
	printf("Number of allocated blocks: %ld\n", list_all->size);
	printf("Number of malloc calls: %d\n", mcalls);
	printf("Number of fragmentations: %d\n", x);
	printf("Number of free calls: %d\n", fc);
	for (size_t i = 0; i < sfl->number_lists; i++) {
		if (sfl->listoflists[i]->size > 0) {
			printf("Blocks with %ld bytes - %ld free block(s) :",
				   sfl->listoflists[i]->data_size, sfl->listoflists[i]->size);
			dll_node_t *curnode = sfl->listoflists[i]->head;
		for (size_t j = 0; j < sfl->listoflists[i]->size; j++) {
			printf(" 0x%llx", ((info_t *)curnode->data)->address);
			curnode = curnode->next;
		}
		printf("\n");
		}
	}
	printf("Allocated blocks :");
	dll_node_t *malcur = list_all->head;
	for (size_t z = 0; z < list_all->size; z++) {
		printf(" (0x%llx - %ld)", ((info_t *)malcur->data)->address,
			   ((info_t *)malcur->data)->size);
		malcur = malcur->next;
	}
	printf("\n");
	printf("-----DUMP-----");
	printf("\n");
}

doubly_linked_list_t **fragmentation_add(sfl_t *sfl,
										 unsigned long long address,
									     size_t data_size,
										 doubly_linked_list_t *mall)
{
	int new_position = 0;
	for (int i = 0; i < sfl->number_lists; i++) {
		if (sfl->listoflists[i]->data_size == data_size) {
			dll_add_nth_node(sfl->listoflists[i],
							 1, NULL, address, data_size);
			return sfl->listoflists;
		}
	}
	for (int i = 1; i < sfl->number_lists; i++) {
		if (data_size > sfl->listoflists[i - 1]->data_size &&
		    data_size < sfl->listoflists[i]->data_size)
			new_position = i;
	}
	sfl->number_lists++;
	doubly_linked_list_t **auxlist =
	realloc(sfl->listoflists, sfl->number_lists *
		    sizeof(doubly_linked_list_t *));
	sfl->listoflists = auxlist;
	for (int i = sfl->number_lists - 1; i > new_position; i--)
		sfl->listoflists[i] = sfl->listoflists[i - 1];

	doubly_linked_list_t *baubau = dll_create(data_size);
	dll_add_nth_node(baubau, 1, NULL, address, data_size);
	sfl->listoflists[new_position] = baubau;
	return sfl->listoflists;
}

doubly_linked_list_t *malloc_heaps(sfl_t *sfl, doubly_linked_list_t *mallist,
								   unsigned int *mc, size_t *allocated_memory,
								   int nrbytes, unsigned int *fgc, int rec)
{
	int malloc = 0;
	for (int i = 0; i < sfl->number_lists; i++) {
		if (sfl->listoflists[i]->size > 0 &&
			sfl->listoflists[i]->data_size == nrbytes) {
			if (mallist->size == 0) {
				mallist =
				dll_add_nth_node_by_node(mallist,
										 0, NULL,
										 dll_remove_nth_node
										 (sfl->listoflists[i], 0));
				(*allocated_memory) += sfl->listoflists[i]->data_size;
				malloc = 1;
			} else {
				mallist =
				dll_add_nth_node_by_node(mallist,
										 1, NULL,
										 dll_remove_nth_node
										 (sfl->listoflists[i], 0));
				(*allocated_memory) += sfl->listoflists[i]->data_size;
				malloc = 1;
			}
			(*mc)++;
			break;
		} else if (sfl->listoflists[i]->size > 0 &&
				   sfl->listoflists[i]->data_size > nrbytes) {
			if (mallist->size == 0) {
				dll_node_t *current =
				dll_remove_nth_node(sfl->listoflists[i], 0);
				dll_add_nth_node(mallist,
								 0, NULL, ((info_t *)current->data)->address,
								 nrbytes);
				sfl->listoflists =
				fragmentation_add(sfl, ((info_t *)current->data)->address +
								  nrbytes, ((info_t *)current->data)->size -
								  nrbytes, mallist);
				(*allocated_memory) += nrbytes;
				(*fgc)++;
				free(((info_t *)current->data)->data);
				free(current->data);
				free(current);
				malloc = 1;
			} else {
				dll_node_t *current =
				dll_remove_nth_node(sfl->listoflists[i], 0);
				dll_add_nth_node(mallist, 1,
								 NULL, ((info_t *)current->data)->address,
								 nrbytes);
				sfl->listoflists =
				fragmentation_add(sfl, ((info_t *)current->data)->address +
								nrbytes, ((info_t *)current->data)->size -
								nrbytes, mallist);
				(*allocated_memory) += nrbytes;
				(*fgc)++;
				free(((info_t *)current->data)->data);
				free(current->data);
				free(current);
				malloc = 1;
			}
			(*mc)++;
			break;
		}
	}
	if (malloc != 1)
		printf("Out of memory\n");
	return mallist;
}

unsigned int dll_get_size(doubly_linked_list_t *list)
{
	return list->size;
}

doubly_linked_list_t *free_list(sfl_t *sfl, doubly_linked_list_t *malloc_list,
								unsigned long long address, int *fc,
								size_t *allocated_memory, int rec)
{
	dll_node_t *ptr = malloc_list->head;
	for (int i = 0; i < malloc_list->size; i++) {
		if (address == ((info_t *)ptr->data)->address) {
			(*fc)++;
			(*allocated_memory) = (*allocated_memory) -
			((info_t *)ptr->data)->size;
			ptr =  dll_remove_nth_node(malloc_list, i);
			sfl->listoflists =
			fragmentation_add(sfl, address,
							  ((info_t *)ptr->data)->size, malloc_list);
			free(((info_t *)ptr->data)->data);
			free(ptr->data);
			free(ptr);
			return malloc_list;
		}
		ptr = ptr->next;
	}
	printf("Invalid free\n");
	return malloc_list;
}

void destroy_heap(sfl_t *sfl, doubly_linked_list_t **list)
{
	dll_node_t *ptr = (*list)->head;
	int i = 0;
	while (i < (*list)->size) {
		dll_node_t *aux = ptr->next;
		free(((info_t *)ptr->data)->data);
		free(ptr->data);
		free(ptr);
		ptr = aux;
		i++;
	}
	free((*list));
	i = 0;
	for (i = 0; i < sfl->number_lists; i++) {
		ptr = sfl->listoflists[i]->head;
		int j = 0;
		while (j < sfl->listoflists[i]->size) {
			dll_node_t *aux = ptr->next;
			free(((info_t *)ptr->data)->data);
			free(ptr->data);
			free(ptr);
			ptr = aux;
			j++;
		}
		free(sfl->listoflists[i]);
	}
	free(sfl->listoflists);
}

void read(sfl_t *sfl, doubly_linked_list_t *mall, unsigned long long address,
		  size_t size, size_t allocated, int x, int y, int z, int *working)
{
	dll_node_t *curr = mall->head;
	unsigned long long free_memory = 0;
	unsigned long long curr_adr = address;
	int poz = -1;
	for (int i = 0; i < mall->size; i++) {
		if (((info_t *)curr->data)->address + ((info_t *)curr->data)->size >=
			address && ((info_t *)curr->data)->address <= address) {
			if (address + size <= ((info_t *)curr->data)->size +
			((info_t *)curr->data)->address) {
				free_memory = free_memory + size;
			} else {
				free_memory = free_memory +
				(((info_t *)curr->data)->address +
				((info_t *)curr->data)->size - address);
			}
		} else if (address <= ((info_t *)curr->data)->address &&
				   address + size >= ((info_t *)curr->data)->address &&
				   address + size <= ((info_t *)curr->data)->address +
					((info_t *)curr->data)->size) {
			free_memory = free_memory +
			address + size - ((info_t *)curr->data)->address;
		} else if (address <= ((info_t *)curr->data)->address &&
				   address + size > ((info_t *)curr->data)->address
				+ ((info_t *)curr->data)->size) {
			free_memory = free_memory + ((info_t *)curr->data)->size;
		}
		curr = curr->next;
		if (free_memory > 0 && poz == -1)
			poz = i;
	}
	if (free_memory < size) {
		printf("Segmentation fault (core dumped)\n");
		dump_mem(sfl, mall, y, allocated, x, z);
		(*working) = 0;
		return;
	}
	int allowed_bytes;
	curr = dll_get_nth_node(mall, poz)->prev;
	for (size_t i = poz; i < mall->size; i++) {
		curr = curr->next;
		if (((info_t *)curr->data)->size >= address + size - curr_adr)
			allowed_bytes = address + size - curr_adr;
		else
			allowed_bytes = ((info_t *)curr->data)->size;
		if (((info_t *)curr->data)->address <= address)
			printf("%.*s", allowed_bytes,
				   (char *)((info_t *)curr->data)->data +
				   (address - ((info_t *)curr->data)->address));
		else if (((info_t *)curr->data)->address == curr_adr)
			printf("%.*s", allowed_bytes,
				   (char *)((info_t *)curr->data)->data);
		curr_adr += ((info_t *)curr->data)->address +
		((info_t *)curr->data)->size - curr_adr;
		if (curr_adr >= address + size)
			break;
	}
	printf("\n");
}

doubly_linked_list_t *write(sfl_t *sfl, doubly_linked_list_t *mall, char *data,
							unsigned long long address, size_t size,
							size_t allocated, int x, int y, int z, int *working)
{
	dll_node_t *curr = mall->head;
	if (size > strlen(data))
		size = strlen(data);
	unsigned long long free_memory = 0;
	int poz = -2;
	for (int i = 0; i < mall->size; i++) {
		if (((info_t *)curr->data)->address + ((info_t *)curr->data)->size >=
		address && ((info_t *)curr->data)->address <= address) {
			if (address + size <= ((info_t *)curr->data)->size +
			((info_t *)curr->data)->address) {
				free_memory = free_memory + size;
			} else {
				free_memory = free_memory +
				(((info_t *)curr->data)->address +
				((info_t *)curr->data)->size - address);
			}
		} else if (address <= ((info_t *)curr->data)->address &&
				   address + size >= ((info_t *)curr->data)->address &&
				   address + size <= ((info_t *)curr->data)->address +
				((info_t *)curr->data)->size) {
			free_memory = free_memory + address + size -
			((info_t *)curr->data)->address;
		} else if (address <= ((info_t *)curr->data)->address &&
				   address + size > ((info_t *)curr->data)->address +
				   ((info_t *)curr->data)->size) {
			free_memory = free_memory + ((info_t *)curr->data)->size;
		}
		curr = curr->next;
		if (free_memory > 0 && poz == -2)
			poz = i;
	}
	if (free_memory < size) {
		printf("Segmentation fault (core dumped)\n");
		dump_mem(sfl, mall, y, allocated, x, z);
		(*working) = 0;
		return mall;
	}
	unsigned long long write_memory = 0;
	char *aux_data = malloc(size + 1);
	curr = dll_get_nth_node(mall, poz)->prev;
	for (size_t i = poz; i < mall->size; i++) {
		curr = curr->next;
		free_memory = 0;
		if (((info_t *)curr->data)->address + ((info_t *)curr->data)->size >=
		    address && ((info_t *)curr->data)->address <= address) {
			free_memory = free_memory +
			(address + size <= ((info_t *)curr->data)->address +
			((info_t *)curr->data)->size) ? size :
			(((info_t *)curr->data)->address +
			((info_t *)curr->data)->size - address);
		} else if (address <= ((info_t *)curr->data)->address &&
				   address + size <= ((info_t *)curr->data)->address +
					((info_t *)curr->data)->size) {
			free_memory = free_memory + address + size -
			((info_t *)curr->data)->address;
		} else if (address <= ((info_t *)curr->data)->address &&
				   address + size > ((info_t *)curr->data)->address +
				   ((info_t *)curr->data)->size) {
			free_memory = free_memory + ((info_t *)curr->data)->size;
		}
		if (((info_t *)curr->data)->address > address) {
			memcpy(aux_data, data + (((info_t *)curr->data)->address - address),
				   free_memory);
			memcpy(((info_t *)curr->data)->data, aux_data, free_memory);
		} else {
			memcpy(aux_data, data, free_memory);
			memcpy(((info_t *)curr->data)->data + (address -
				   ((info_t *)curr->data)->address),
				   aux_data, free_memory);
		}
		write_memory += free_memory;
		if (write_memory == size)
			break;
	}
	free(aux_data);
	return mall;
}

int main(void)
{
	char command[30] = {0};
	unsigned int mcalls = 0;
	unsigned int fgc = 0;
	size_t al_mem = 0;
	sfl_t sfl;
	int fcalls = 0;
	doubly_linked_list_t *allocated_blocks = dll_create(0);
	int working = 1;
	int reconstruction_type;
	while (working) {
		scanf("%s", command);
		if (strcmp(command, "INIT_HEAP") == 0) {
			unsigned long long address;
			size_t number_lists, number_bytes_per_list;
			scanf("%llx %ld %ld %d", &address, &number_lists,
				  &number_bytes_per_list, &reconstruction_type);
			sfl = init_heap(&sfl, address, number_lists,
							number_bytes_per_list);
		} else if (strcmp(command, "MALLOC") == 0) {
			int nr_bytes;
			scanf("%d", &nr_bytes);
			allocated_blocks =
			malloc_heaps(&sfl, allocated_blocks, &mcalls,
						 &al_mem, nr_bytes, &fgc, reconstruction_type);
		} else if (strcmp(command, "FREE") == 0) {
			unsigned long long address_to_free;
			scanf("%llx", &address_to_free);
			allocated_blocks =
			free_list(&sfl, allocated_blocks, address_to_free,
					  &fcalls, &al_mem, reconstruction_type);
		} else if (strcmp(command, "READ") == 0) {
			unsigned long long address_to_read;
			size_t nr_bytes;
			scanf("%llx %ld", &address_to_read, &nr_bytes);
			read(&sfl, allocated_blocks, address_to_read, nr_bytes,
				 al_mem, fgc, mcalls, fcalls, &working);
			if (working == 0)
				destroy_heap(&sfl, &allocated_blocks);
		} else if (strcmp(command, "WRITE") == 0) {
			char *data = malloc(600 * sizeof(char));
			unsigned long long address_to_write;
			size_t nr_bytes_write;
			scanf("%llx", &address_to_write);
			scanf(" \"%600[^\"]\"", data);
			scanf("%ld", &nr_bytes_write);
			write(&sfl, allocated_blocks, data, address_to_write,
				  nr_bytes_write, al_mem, fgc, mcalls, fcalls, &working);
			if (working == 0)
				destroy_heap(&sfl, &allocated_blocks);
			free(data);
		} else if (strcmp(command, "DUMP_MEMORY") == 0) {
			dump_mem(&sfl, allocated_blocks, mcalls, al_mem, fgc, fcalls);
		} else if (strcmp(command, "DESTROY_HEAP") == 0) {
			destroy_heap(&sfl, &allocated_blocks);
			break;
		}
	}
	return 0;
}
