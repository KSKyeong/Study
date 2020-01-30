
#pragma once

#include "stdafx.h"
#include "ga.h"

Chromosome pop1[POPULATION_SIZE], pop2[POPULATION_SIZE];
Chromosome *pop, *pop_next;
Chromosome best;

void init_pop();
void ga_impl();
int select(double f_sum);
void print_result(double f_sum);
bool get_best();
void keep_best();

int generations;
int best_iter;
int selected[POPULATION_SIZE];
double fitness_wheel[POPULATION_SIZE];

double mat[CITIES][CITIES];
struct point pos[CITIES];

FILE *fp_hist;

Chromosome::Chromosome() {
}
Chromosome::~Chromosome() {
}

void Chromosome::evaluate() {
	int i; 
	distance= 0.;
	for (i = 0; i<CITIES - 1; i++)
		distance += mat[genes[i]][genes[i+1]];
	distance += mat[genes[i]][genes[0]];
	fitness= 1.0/distance;	

}

void Chromosome::init(int *gp) {
	int p1, p2, t;

	memcpy(genes, gp, CITIES*sizeof(int));
	p1 = rand() % CITIES;
	p2 = rand() % CITIES;
	SWAP(genes[p1], genes[p2], t);
	evaluate();
}
void Chromosome::mutation_ex() {
	int p1, p2, t;

	p1 = rand() % CITIES;
	do {
		p2 = rand() % CITIES;
	} while (p1 == p2);
	SWAP(genes[p1], genes[p2], t);
	evaluate();
}
void Chromosome::mutation_inv() {
	int p1, p2, t;
	int i, j, lower, upper;
	int tmp[CITIES];

	p1 = rand() % CITIES;
	do {
		p2 = rand() % CITIES;
	} while (p1==p2);
	if (p1 < p2) {
		while (p1<p2) {
			SWAP(genes[p1], genes[p2], t);
			p1++, p2--;
		}
	}
	else {
		lower = p2 + 1;
		upper = CITIES - p1;
		memcpy(tmp, genes + p1, upper*sizeof(int));
		memcpy(tmp + upper, genes, lower*sizeof(int));
		i = 0; j = upper + lower -1;
		while (i<j) {
			SWAP(tmp[i], tmp[j], t);
			i++, j--;
		}
		memcpy(genes+p1, tmp, upper*sizeof(int));
		memcpy(genes, tmp + upper, lower*sizeof(int));
	}
	evaluate();
}

void ordering(int src[], int dest[], int p1, int p2) {
	int i, j, p, g;
	bool flag;

	p= 0;
	for (i = 0; i<CITIES; i++) {
		g= src[i];
		flag= true;
		for (j=p1; flag&&j<=p2; j++) {
			if (g==dest[j])
				flag= false;
		}
		if (flag) {
			while (p<CITIES) {
				if (dest[p]<0) {
					dest[p++]= g;
					break;
				}
				else
					p++;
			}
		}
	}
}

void Chromosome::crossover(Chromosome *pch) {
	Chromosome ch1, ch2;
	int p1, p2;
	int lower, upper;

	memset(ch1.genes, -1, CITIES*sizeof(int));
	memset(ch2.genes, -1, CITIES*sizeof(int));
	p1 = rand() % CITIES;
	do {
		p2 = rand() % CITIES;
	} while (p1==p2);

	//int t;
	//if (p2<p1)
	//	SWAP(p1, p2, t);

	if (p1 < p2) {
		memcpy(ch1.genes + p1, pch->genes + p1, (p2 - p1 + 1)*sizeof(int));
		memcpy(ch2.genes + p1, this->genes + p1, (p2 - p1 + 1)*sizeof(int));
	}
	else {
		lower = p2 + 1;
		upper = CITIES - p1;
		memcpy(ch1.genes, pch->genes + p1, upper*sizeof(int));
		memcpy(ch1.genes + upper, pch->genes, lower*sizeof(int));
		memcpy(ch2.genes, this->genes + p1, upper*sizeof(int));
		memcpy(ch2.genes + upper, this->genes, lower*sizeof(int));
		p1 = 0; p2 = upper + lower - 1;
	}

	ordering(this->genes, ch1.genes, p1, p2);
	ordering(pch->genes, ch2.genes, p1, p2);
	ch1.evaluate();
	ch2.evaluate();
	if (this->fitness<ch1.fitness || drand()<0.1)
		*this = ch1;
	if (pch->fitness<ch2.fitness || drand()<0.1)
		*pch = ch2;
}

void init_pop() {
	pop= pop1, pop_next= pop2;

	for (int i = 0; i<CITIES; i++)
		pop[0].genes[i]= i;
	pop[0].evaluate();
	for (int i=1; i<POPULATION_SIZE; i++) {
		pop[i].init(pop[i-1].genes);
	}

	best.fitness= 0;
	get_best();
}

void ga_impl() {
	int i, j;
	double fitness_sum;
	Chromosome *pop_temp;

	fopen_s(&fp_hist, str_hist, "wt");

	best_iter= generations= 0;
	//while (generations<GA_END) {
	while (best_iter<BEST_MAX) {
		fitness_wheel[0] = pop[0].fitness;
		for (i = 1; i<POPULATION_SIZE; i++)
			fitness_wheel[i] = fitness_wheel[i - 1] + pop[i].fitness;
		fitness_sum = fitness_wheel[POPULATION_SIZE - 1];

		//for (i = 0; i<POPULATION_SIZE; i++) 
		//	selected[i] = select(fitness_sum);

		selected[i]= select(fitness_sum);
		for (i=1; i<POPULATION_SIZE; i++) {
			do {
				j = select(fitness_sum);
			} while (j == selected[i - 1]);
			selected[i] = j;
		}

		for (i=0; i<POPULATION_SIZE; i++)
			pop_next[i]= pop[selected[i]];
		SWAP(pop, pop_next, pop_temp);

		for (i=0; i<POPULATION_SIZE-1; i += 2) {
			if (drand() <= PCROSS) {
					pop[i].crossover(pop+i+1);
			}
		}

		for (i=0; i<POPULATION_SIZE; i++) {
			if (drand() <= PMUT)  {
				if (drand() <= 0.5)
					pop[i].mutation_inv();
				else
					pop[i].mutation_ex();
			}
		}
	
		if (get_best()) 
			keep_best();

		if (generations%1000 ==0) 
			print_result(fitness_sum);

		generations++;
	}

	print_result(fitness_sum);
	fclose(fp_hist);
}

bool get_best() {
	bool flag= true;
	for (int i=0; i<POPULATION_SIZE; i++) {
		if (best.fitness<=pop[i].fitness) {
			if (best.fitness<pop[i].fitness) {
				best = pop[i];
				best_iter = 0;
			}
			flag= false;
		}
	}
	best_iter++;
	return flag;
}
void keep_best() {
	int point= rand()%POPULATION_SIZE;
	pop[point]= best;
}

void print_result(double f_sum) {
	double avg= 1.0/(f_sum/POPULATION_SIZE);

	printf("generation-best: %d, distance avg:%g, min:%g\n", generations,avg, best.distance);

	fprintf(fp_hist, "%d\t%g\t%g\n", generations, avg, best.distance);
}

int select(double f_sum) {
	int i;
	double r;

	r = drand() * f_sum;
	for (i = 0; i<POPULATION_SIZE - 1; i++) {
		if (r <= fitness_wheel[i])
			return i;
	}
	return POPULATION_SIZE - 1;
} 
	