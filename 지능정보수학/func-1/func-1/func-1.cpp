// func-1.cpp : 콘솔 응용 프로그램에 대한 진입점을 정의합니다.
//
#pragma once

#include "stdafx.h"
#include "ga.h"

void init_pop();
void ga_impl();

int _tmain(int argc, _TCHAR* argv[])
{
	clock_t t_start, t_end;;
	double duration;

	srand(seedval);

	init_pop();
	t_start = clock();
	ga_impl();
	t_end = clock();
	duration = (double)(t_end - t_start) / CLOCKS_PER_SEC;
	printf("경과시간: %f\n", duration);

	return 0;
}
