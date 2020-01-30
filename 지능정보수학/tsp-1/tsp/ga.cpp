
#pragma once

#include "stdafx.h"
#include "ga.h"

Chromosome pop1[POPULATION_SIZE], pop2[POPULATION_SIZE];
Chromosome* pop, * pop_next;
Chromosome best;

void init_pop();
void ga_impl();
int select(double f_sum);
void print_result(double f_sum);
bool get_best();
void keep_best();

int generations;
int selected[POPULATION_SIZE];

double mat[CITIES][CITIES];
struct point pos[CITIES];
int n_city;
int wheel[POPULATION_SIZE];

FILE* fp_hist;

Chromosome::Chromosome() {
}
Chromosome::~Chromosome() {
}

void Chromosome::evaluate() {
	int i;
	distance = 0.;
	for (i = 0; i < n_city - 1; i++)
		distance += mat[genes[i]][genes[i + 1]];
	distance += mat[genes[i]][genes[0]];
	fitness = 1.0 / distance;

}

void Chromosome::init(int* gp) {
	int p1, p2, t;

	memcpy(genes, gp, n_city * sizeof(int));
	p1 = rand() % n_city;
	p2 = rand() % n_city;
	SWAP(genes[p1], genes[p2], t);
	evaluate();
}

void Chromosome::mutation() {
	int p1, p2, t;
	int mid, times;

	p1 = rand() % n_city;
	do {
		p2 = rand() % n_city;
	} while (p1 == p2);
	if (p1 < p2) {
		mid = (p1 + p2) / 2;
		while (p1 <= mid) {
			SWAP(genes[p1], genes[p2], t);
			p1++, p2--;
		}
	}
	else if (p1 > p2) {
		times = (n_city - (p1 - p2 - 1)) / 2;
		for (int i = 0; i < times; i++) {
			SWAP(genes[p1], genes[p2], t);
			p1 = (p1 + 1) % n_city;
			p2 = (p2 - 1 + n_city) % n_city;
		}
	}
	evaluate();
}
void ordering(int src[], int dest[], int p1, int p2) {
	int i, j, p, g;
	bool flag;

	p = 0;
	for (i = 0; i < n_city; i++) {
		g = src[i];
		flag = true;
		for (j = p1; flag && j <= p2; j++) {
			if (g == dest[j])
				flag = false;
		}
		if (flag) {
			while (p < n_city) {
				if (dest[p] < 0) {
					dest[p++] = g;
					break;
				}
				else
					p++;
			}
		}
	}
}

void Chromosome::crossover(Chromosome* pch) {
	Chromosome ch1, ch2;
	int i, p1, p2, t;

	p1 = rand() % n_city;
	do {
		p2 = rand() % n_city;
	} while (p1 == p2);
	if (p2 < p1)
		SWAP(p1, p2, t);

	memset(ch1.genes, -1, n_city * sizeof(int));
	memset(ch2.genes, -1, n_city * sizeof(int));
	for (i = p1; i <= p2; i++) {
		ch1.genes[i] = pch->genes[i];
		ch2.genes[i] = this->genes[i];
	}
	ordering(this->genes, ch1.genes, p1, p2);
	ordering(pch->genes, ch2.genes, p1, p2);
	ch1.evaluate();
	ch2.evaluate();
}

void init_pop() {
	pop = pop1, pop_next = pop2;

	for (int i = 0; i < n_city; i++)
		pop[0].genes[i] = i;
	pop[0].evaluate();
	for (int i = 1; i < POPULATION_SIZE; i++) {
		pop[i].init(pop[i - 1].genes);
	}

	best.fitness = 0;
	get_best();
}

void ga_impl() {
	int i;
	double fitness_sum;
	Chromosome* pop_temp;

	fopen_s(&fp_hist, str_hist, "wt");

	generations = 0;
	while (generations < GA_END) {
		fitness_sum = 0.;


		for (i = 0; i < POPULATION_SIZE; i++) {
			fitness_sum += pop[i].fitness;
		}

		for (i = 0; i < POPULATION_SIZE; i++) {
			if (i == 0) {
				wheel[i] = pop[i].fitness;
			}
			else {
				wheel[i] = pop[i].fitness + wheel[i - 1];
			}
		}

		for (i = 0; i < POPULATION_SIZE; i++) {
			selected[i] = select(fitness_sum);
		}

		for (i = 0; i < POPULATION_SIZE; i++)
			pop_next[i] = pop[selected[i]];
		SWAP(pop, pop_next, pop_temp);

		for (i = 0; i < POPULATION_SIZE - 1; i += 2) {
			if (drand() <= PCROSS) {
				pop[i].crossover(pop + i + 1);

			}
		}

		for (i = 0; i < POPULATION_SIZE; i++) {
			if (drand() <= PMUT) {
				pop[i].mutation();
			}
		}

		if (get_best())
			keep_best();

		if (generations % 100 == 0)
			print_result(fitness_sum);

		generations++;
	}

	print_result(fitness_sum);
	fclose(fp_hist);
}

bool get_best() {
	bool flag = true;
	for (int i = 0; i < POPULATION_SIZE; i++) {
		if (best.fitness <= pop[i].fitness) {
			if (best.fitness < pop[i].fitness) {
				best = pop[i];
			}
			flag = false;
		}
	}
	return flag;
}
void keep_best() {
	int point = rand() % POPULATION_SIZE;
	pop[point] = best;
}

void print_result(double f_sum) {
	double avg = 1.0 / (f_sum / POPULATION_SIZE);

	printf("generation-best: %d, distance avg:%g, min:%g\n", generations, avg, best.distance);

	fprintf(fp_hist, "%d\t%g\t%g\n", generations, avg, best.distance);
}

int select(double f_sum) {
	int i;
	double r;//, cdf_sum;

	r = drand() * f_sum;
	//cdf_sum= 0.0;

	for (i = 0; i < POPULATION_SIZE; i++) {
		if (r <= wheel[i])
			return i;
	}
	return POPULATION_SIZE - 1;
}