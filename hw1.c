#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <omp.h>

//Use single precision float/real variables throughout the excercise.
void initialize(double *x, int n){
  int i, j;
  double random;
  int nest;
  double temp;
  for(i = 0; i < n; i++){
    for(j = 0; j < n; j++){
      //temp = (double)((float)rand() / (float)RAND_MAX);
      //temp = (double)rand() / (double)((unsigned)RAND_MAX);
      // temp = (double)((rand()%10000)/(double)10000);
      // x[i*n + j] = temp;
      // printf("%d\n", temp);

      random = (double)((rand()%10000)/(double)10000);
      //printf("%9.7f\n", random);
      x[i*n + j] = random;
      if(x[i*n + j] < 0){
        printf("Val < 0\n");
      }
      if(x[i*n + j] > 1){
        printf("Val > 1\n");
      }
      //printf("%9.7f\n", random);
      //printf("%9.7f\n", x[i*n + j]);
    }
  }
}

void smooth(double *x, double*y, int n, double a, double b, double c){
  printf("%i\n", omp_get_num_procs());
  printf("%i\n", omp_get_max_threads());
  printf("%i\n", omp_get_num_threads());

  int i, j;
  // #pragma omp parallel private(i,j)
  // {
  for(i = 1; i < n - 1; i++){
    for(j = 1; j < n - 1; j++){
      y[i*n + j] = a * (x[(i-1)*n+(j-1)] + x[(i-1)*n+(j+1)] + x[(i+1)*n+(j-1)] + x[(i+1)*n+(j+1)])
                  + b * (x[(i-1)*n+(j)] + x[(i+1)*n+(j)] + x[(i)*n+(j-1)] + x[(i)*n+(j+1)])
                  + c * (x[i*n+j]);
    }
  }
  // }
}

void count(double *y, int n, double t, int *res){
  int i, j;
  int result;
  result = 0;
  #pragma omp parallel reduction(+:result)
    #pragma omp for private(i,j)
      for(i = 1; i < n - 1; i++){

        for(j = 1; j < n - 1; j++){
          if(y[i*n + j] < t){
            result++;
          }
        }
  }
  //res = result;
  *res = result;
  //res = &result;
  printf("%i\n", res);
}

int main(){

  omp_set_num_threads(omp_get_num_procs());
  printf("%i\n", omp_get_num_procs());
  printf("%i\n", omp_get_max_threads());
  printf("%i\n", omp_get_num_threads());

//  srand(time(NULL));
  srand(1234);

  double i0, i1, i2, i3, i4, i5, i6;

  int resx = 0, resy = 0;
  // resx = (int *) malloc(sizeof(int));
  // resy = (int *) malloc(sizeof(int));
  const double a = 0.05;
  const double b = 0.1;
  const double c = 0.4;

  double t = 0.1; //threshold
  int n = 16384+2; //the 2 elements are the border of the array

  //Declare two arrays, using malloc
  //Allocate the arrays of n in each direction
  //2D arrays have to be contiguous in memory
  double *x, *y;

  i0 = clock();
  x = (double *) malloc(n * n * sizeof(double)); //allocate n*n doubles
  i1 = clock();
  y = (double *) malloc(n * n * sizeof(double)); //allocate n*n doubles

  i2 = clock();
  initialize(x, n);
  i3 = clock();
  smooth(x, y, n, a, b, c);
  i4 = clock();
  count(x, n, t, &resx);
  i5 = clock();
  count(y, n, t, &resy);
  i6 = clock();

  printf("Summary\n");
  printf("-------\n");
  printf("Number of elements in a row/column        ::              %i\n", n);
  printf("Number of inner elements in a row/column  ::              %i\n", n-2);
  printf("Total number of elements                  ::              %i\n", n*n);
  printf("Total number of inner elements            ::              %i\n", (n-2)*(n-2));
  printf("Memory (GB) used per array                ::              %f\n", sizeof(double)*n*n);
  printf("Threshold                                 ::              %3.2f\n", t);
  printf("Smoothing constants (a, b, c)             ::              %3.2f %3.2f %3.2f\n", a, b, c);
  printf("Number    of elements below threshold (X) ::              %i\n", resx);
  printf("Fraction  of elements below threshold     ::              %f\n", ((double)resx/(n*n)));
  printf("Number    of elements below threshold (Y) ::              %i\n", resy);
  printf("Fraction  of elements below threshold     ::              %f\n", ((double)resy/((n-2)*(n-2))));
  printf("\nAction        ::  time/s    Time Resolution = 1.0E-04\n");
  printf("-------\n");
  printf("CPU: Alloc-X    ::  %f\n", (i1 - i0) / (float)CLOCKS_PER_SEC);
  printf("CPU: Alloc-Y    ::  %f\n", (i2 - i1) / (float)CLOCKS_PER_SEC);
  printf("CPU: Init-X     ::  %f\n", (i3 - i2) / (float)CLOCKS_PER_SEC);
  printf("CPU: Smooth     ::  %f\n", (i4 - i3) / (float)CLOCKS_PER_SEC);
  printf("CPU: Count-X    ::  %f\n", (i5 - i4) / (float)CLOCKS_PER_SEC);
  printf("CPU: Count-Y    ::  %f\n", (i6 - i5) / (float)CLOCKS_PER_SEC);

  return 0;
}
