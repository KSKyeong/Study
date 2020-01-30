
#pragma once

#include "stdafx.h"
#include "ga.h"

void read_data();
void write_result(unsigned seed, double duration);
void init_pop();
void delete_pop();
void ga_impl();

char str_in[20], str_hist[20], str_out[20];

int _tmain(int argc, _TCHAR* argv[])
{
	clock_t t_start, t_end;;
	unsigned seed;
	double duration;

	sprintf_s(str_in, "data_%d.txt", CITIES);
	sprintf_s(str_hist, "hist_%d.txt", CITIES);
	sprintf_s(str_out, "out_%d.txt", CITIES);

	seed= seedval;
	srand(seed);

	read_data();
	init_pop();

	t_start= clock();
	ga_impl();
	t_end= clock();
	duration= (double)(t_end-t_start)/CLOCKS_PER_SEC;
	printf("경과시간: %f초\n", duration);
	write_result(seed, duration);

	return 0;
}

void read_data() {
	double dist;

	sprintf_s(str_in, "data_%d.txt", CITIES);
	FILE *fp_in;
	fopen_s(&fp_in, str_in, "rt");

	fscanf_s(fp_in, "%d", &n_city);
	for (int i=0; i<n_city; i++)
		fscanf_s(fp_in, "%lf %lf", &(pos[i].x), &(pos[i].y));
	for (int i=0; i<n_city; i++) {
		mat[i][i]= 0.;
		for (int j=i+1; j<n_city; j++) {
			dist= sqrt(pow(pos[i].x-pos[j].x, 2)+pow(pos[i].y-pos[j].y, 2));
			mat[i][j]= mat[j][i]= dist;
		}
	}
	fclose(fp_in);
}
void write_result(unsigned seed, double duration) {
#ifdef _DEBUG
	if (fabs(best.distance-1.0/best.fitness)>0.001) {
		printf("ERROR:%.3f-%.3f\n", best.distance, 1.0/best.fitness);
	}

#endif
	int n;
	FILE *fp_out;
	fopen_s(&fp_out, str_out, "wt");

	fprintf(fp_out, "도시수:%d\n",n_city);
	fprintf(fp_out, "시드:%u", seed);
	fprintf(fp_out, "해집단 크기:%d\t세대수:%d\t교차율:%g\t변이율:%g\n",
		 POPULATION_SIZE, generations, PCROSS, PMUT);
	fprintf(fp_out, "실행시간\t%.2f초\n", duration);
	fprintf(fp_out, "전체 거리\t%.3f\n", best.distance);
	for (int i=0; i<n_city; i++) {
		n= best.genes[i];
		fprintf(fp_out, "%d\t%.3f\t%.3f\n", n, pos[n].x, pos[n].y);
	}
	n= best.genes[0];
	fprintf(fp_out, "%d\t%.3f\t%.3f\n", n, pos[n].x, pos[n].y);

	fclose(fp_out);
}