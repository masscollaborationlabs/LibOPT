#include "bso.h"

/* BSO-related functions */

/* It clusters the agents and returns a pointer with the best agent's ID per cluster.
Parameters:
s: search space */
int *k_means(SearchSpace *s){
    int *best_cluster = NULL, *nearest = NULL, *ctr = NULL, i, j, r;
    char *is_chosen = NULL, OK;
    double **center = NULL, old_error, error = DBL_MAX, min_distance, distance;
    double **center_mean = NULL, *best_fitness_cluster = NULL;
    
    if(!s){
        fprintf(stderr,"\nSearch space not allocated @k_means.\n");
        exit(-1);
    }
        
    best_cluster = (int *)malloc(s->k*sizeof(int));
    best_fitness_cluster = (double *)malloc(s->k*sizeof(double));
    nearest = (int *)malloc(s->m*sizeof(int));
    ctr = (int *)calloc(s->m,sizeof(int));
    center = (double **)malloc(s->k*sizeof(double *));
    center_mean = (double **)malloc(s->k*sizeof(double *));
    is_chosen = (char *)calloc(s->m,sizeof(char));
    
    /* initializing k centers with samples choosen at random */
    for (i = 0; i < s->k; i++){
	    center[i] = (double *)malloc(s->n*sizeof(double));
	    center_mean[i] = (double *)calloc(s->n,sizeof(double));
	    OK = j = 0;
	    do{
		j++;
		r = (int)GenerateUniformRandomNumber(0, s->m);
		if(!is_chosen[r]){
		    is_chosen[r] = 1;
		    OK = 1;
		}
	    }while((!OK) && (j <= s->m));
	    if(j > s->m){
		fprintf(stderr,"\nProblems initializing the k centers @k_means. Probably k is too large.\n");
		exit(-1);
	    }
	    for (j = 0; j < s->n; j++) center[i][j] = s->a[r]->x[j];
    }
    
    /* main algorithm */
    do{
	old_error = error;
	error = 0;

	/* for each idea, it finds the nearest center */
	for(i = 0; i < s->m; i++){
	    min_distance = DBL_MAX;
		
	    for (j = 0; j < s->k; j++){
		distance = EuclideanDistance(s->a[i]->x, center[j], s->n);
		if (distance < min_distance){
               	    nearest[i] = j;
               	    min_distance = distance;
	        }
	    }
		
	    /* after finding the nearest center, we accumulate
	    the idea's coordinates at this center */
            for (j = 0; j < s->n; j++)
		center_mean[nearest[i]][j] += s->a[i]->x[j];
		
	    ctr[nearest[i]]++;
            error += min_distance;
	}
	error /= s->m;
		
	/* updating the centers */
	for (i = 0; i < s->k; i++){
	    for (j = 0; j < s->n; j++){
		if(ctr[i]) center[i][j] = center_mean[i][j]/ctr[i];
		else center[i][j] = center_mean[i][j];
	    }
	}
    }while(fabs(error-old_error) > 1e-5);
    
    /* identifying the best idea (smallest fitness) per cluster */
    for(i = 0; i < s->k; i++)
	best_fitness_cluster[i] = DBL_MAX;
    
    for(i = 0; i < s->m; i++){
	if(s->a[i]->fit < best_fitness_cluster[nearest[i]]){
	    best_fitness_cluster[nearest[i]] = s->a[i]->fit;
	    best_cluster[nearest[i]] = i;
	}
    }
    /*************************************************************/
    
    for (i = 0; i < s->k; i++){
   	free(center[i]);
	free(center_mean[i]);
    }
    free(center);
    free(center_mean);
    free(is_chosen);
    free(nearest);
    free(ctr);
    free(best_fitness_cluster);
    
    return best_cluster;
}
/****************************/

/* It executes the Brain Storm Optimization for function minimization
Parameters:
s: search space
Evaluate: pointer to the function used to evaluate particles
arg: list of additional arguments */
void runBSO(SearchSpace *s, prtFun Evaluate, ...){
    va_list arg, argtmp;
    int i, t, *best = NULL;

    va_start(arg, Evaluate);
    va_copy(argtmp, arg);

    if(!s){
        fprintf(stderr,"\nSearch space not allocated @runBSO.\n");
        exit(-1);
    }

    EvaluateSearchSpace(s, _BSO_, Evaluate, arg); /* Initial evaluation */

    for(t = 1; t <= s->iterations; t++){
        fprintf(stderr,"\nRunning iteration %d/%d ... ", t, s->iterations);
        va_copy(arg, argtmp);

	/* clustering ideas */
	best = k_means(s);
	    
        /* for each idea */
        //for(i = 0; i < s->m; i++){
	    
        //}

	//EvaluateSearchSpace(s, _PSO_, Evaluate, arg);

	free(best);
	fprintf(stderr, "OK (minimum fitness value %lf)", s->gfit);
    }

    va_end(arg);
}
/*************************/