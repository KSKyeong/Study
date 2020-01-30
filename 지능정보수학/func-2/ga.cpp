
#pragma once

#include "stdafx.h"
#include "ga.h"

struct chromosome pop1[POPULATION_SIZE], pop2[POPULATION_SIZE];
struct chromosome *pop= pop1, *pop_next= pop2;
struct chromosome best;

double cdf_arr[POPULATION_SIZE];

void init_pop();
void ga_impl();
void evaluate(struct chromosome *f);
int select(double f_sum);
void crossover(int i1, int i2);
void mutation();
void print_result(double f_sum);
bool get_best();
void keep_best();
void cdf_fitness(double fitness_sum);

int generations;
int selected[POPULATION_SIZE];
double weight;

void copy_genes(int dest[], int src[]) {
	for (int i=0; i<GENES; i++) 
		dest[i]= src[i];
}
void init_pop() {
	int i, point, idx;

	weight= 6.0/255;

	for (i=0; i<GENES; i++) {
		pop[0].genes_1[i]= pop[0].genes_2[i]= 0;
		pop[1].genes_1[i]= pop[1].genes_2[i]= 1;
	}
	idx= 2;
	for (i=0; idx<POPULATION_SIZE&&i<GENES; i++) {
		copy_genes(pop[idx].genes_1, pop[0].genes_1);
		copy_genes(pop[idx].genes_2, pop[0].genes_2);
		pop[idx].genes_1[i]= 1;
		pop[idx].genes_2[i]= 1;
		idx++;
	}
	for (i=0; idx<POPULATION_SIZE&&i<GENES; i++) {
		copy_genes(pop[idx].genes_1, pop[1].genes_1);
		copy_genes(pop[idx].genes_2, pop[1].genes_2);
		pop[idx].genes_1[i]= 0;
		pop[idx].genes_2[i]= 0;
		idx++;
	}
	i= 2;
	while (idx<POPULATION_SIZE) {
		copy_genes(pop[idx].genes_1, pop[i].genes_1);
		copy_genes(pop[idx].genes_2, pop[i].genes_2);
		point= rand()%GENES;
		if (pop[idx].genes_1[point]==0)
			pop[idx].genes_1[point]= 1;
		else
			pop[idx].genes_1[point]= 0;

		point= rand()%GENES;
		if (pop[idx].genes_2[point]==0)
			pop[idx].genes_2[point]= 1;
		else
			pop[idx].genes_2[point]= 0;
		idx++;
		i++;
	}

	for (int i=0; i<POPULATION_SIZE; i++) 
		evaluate(pop+i); 
	
	best.fitness= 0;
	get_best();
}

void ga_impl() {
	int i;
	double fitness_sum;
	struct chromosome *pop_temp;

	while (generations++<GA_END) {

		fitness_sum= 0.;
		for (i=0; i<POPULATION_SIZE; i++)
			fitness_sum += pop[i].fitness;

		print_result(fitness_sum);

		cdf_fitness(fitness_sum);
		for (i=0; i<POPULATION_SIZE; i++)
			selected[i]= select(fitness_sum);

		for (i=0; i<POPULATION_SIZE; i++)
			pop_next[i]= pop[selected[i]];
		SWAP(pop, pop_next, pop_temp);

		for (i=0; i<POPULATION_SIZE; i += 2) {
			crossover(i, i+1);
		}

		mutation();
		
		for (i=0; i<POPULATION_SIZE; i++)
			evaluate(pop+i);

		if (get_best()) 
			keep_best();
	}

	print_result(fitness_sum);
}


bool get_best() {
	bool flag= TRUE;
	for (int i=0; i<POPULATION_SIZE; i++) {
		if (best.fitness<pop[i].fitness) {
			best= pop[i];
			flag= FALSE;
		}
	}
	return flag;
}
void keep_best() {
	int point= rand()%POPULATION_SIZE;
	pop[point]= best;
}
void evaluate(chromosome *f) {
	int w= 1;
	int x= 0;
	int y= 0;
	for (int i=GENES-1; i>=0; i--) {
		x += f->genes_1[i]*w;
		y += f->genes_2[i]*w;
		w *= 2;
	}
	f->x= x*weight-3.;
	f->y= y*weight-3.;
	double x_2= pow(f->x,2);
	double x_3= x_2*f->x;
	double y_2= pow(f->y,2);
	double y_3= y_2*f->y;

	f->fitness= pow(1-f->x,2)*exp(-x_2-pow(f->y+1,2)) - (f->x-x_3-y_3)*exp(-x_2-y_2);
}
void print_result(double f_sum) {
	double f_x, f_y, f_fitness;

	printf("generation: %d, fitness avg: %f\n", generations, (double)f_sum/POPULATION_SIZE);
	f_fitness= 0.;
	for (int i=0; i<POPULATION_SIZE; i++) {
		if (f_fitness<pop[i].fitness) {
			f_x= pop[i].x;
			f_y= pop[i].y;
			f_fitness= pop[i].fitness;
		}
	}
	printf("max: x:%f, y:%f, f(x):%f\n", f_x, f_y, f_fitness);
}

void mutation() {
	for (int i=0; i<POPULATION_SIZE; i++) {
		for (int j=0; j<GENES; j++) {
			if (drand()<=PMUT) {
				if (pop[i].genes_1[j]==0)
					pop[i].genes_1[j]= 1;
				else
					pop[i].genes_1[j]= 0;
			}
			if (drand()<=PMUT) {
				if (pop[i].genes_2[j]==0)
					pop[i].genes_2[j]= 1;
				else
					pop[i].genes_2[j]= 0;
			}
		}
	}
}
void crossover(int i1, int i2) {
	int tmp;
	int point;

	if (drand() <= PCROSS) {
		point= random(1, GENES);
		for (int i= point; i<GENES; i++) 
			SWAP(pop[i1].genes_1[i], pop[i2].genes_1[i], tmp);
	}
	if (drand() <= PCROSS) {
		point= random(1, GENES);
		for (int i= point; i<GENES; i++) 
			SWAP(pop[i1].genes_2[i], pop[i2].genes_2[i], tmp);
	}
}

void cdf_fitness(double fitness_sum) {
	double cdf= 0.0;
	
	for (int i=0; i<POPULATION_SIZE; i++) 
		 cdf_arr[i]= cdf +=pop[i].fitness;
}
int select(double f_sum) {
	int i;
	double r;

	r= drand() * f_sum;
	
	for (i=0; i<POPULATION_SIZE; i++) {
		if (r<=cdf_arr[i])
			return i;
	}
	return POPULATION_SIZE-1;
} 
	 