#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <limits.h>

int number_of_pages, keys_per_page, buffer_per_input;
int n, runs, run_size;
int total_pages;

struct page
{
	int *data;
};

int ceiling(int a,int b)
{
	if(a%b == 0)
		return a/b;
	else 
		return a/b +1;
}

void merge(int arr[], int l, int m, int r)
{
    int i, j, k;
    int n1 = m - l + 1;
    int n2 =  r - m;
    /* create temp arrays */
    int L[n1], R[n2];
    /* Copy data to temp arrays L[] and R[] */
    for (i = 0; i < n1; i++)
        L[i] = arr[l + i];
    for (j = 0; j < n2; j++)
        R[j] = arr[m + 1+ j];
    /* Merge the temp arrays back into arr[l..r]*/
    i = 0; // Initial index of first subarray
    j = 0; // Initial index of second subarray
    k = l; // Initial index of merged subarray
    while (i < n1 && j < n2)
    {
        if (L[i] <= R[j])
        {
            arr[k] = L[i];
            i++;
        }
        else
        {
            arr[k] = R[j];
            j++;
        }
        k++;
    }
    /* Copy the remaining elements of L[], if there
       are any */
    while (i < n1)
    {
        arr[k] = L[i];
        i++;
        k++;
    }
    /* Copy the remaining elements of R[], if there
       are any */
    while (j < n2)
    {
        arr[k] = R[j];
        j++;
        k++;
    }
}

void mergesort(int arr[], int l, int r)
{
    if (l < r)
    {
        int m = l+(r-l)/2;
 
        // Sort first and second halves
        mergesort(arr, l, m);
        mergesort(arr, m+1, r);
 
        merge(arr, l, m, r);
    }
}

page * make_pages(int * arr)
{
	page * head;
	total_pages = ceiling(n, keys_per_page);							//total n/k pages
	head = (page *)malloc(total_pages * sizeof(page));					//head is pointer pointing to these n/k pages

	int i,j;
	for(i=0;i<total_pages-1;i++)
	{
		head[i].data = (int *)malloc(keys_per_page*sizeof(int));
		for(j=0;j<keys_per_page;j++)
		{
			head[i].data[j] = arr[i*keys_per_page + j];					//head[i] is the ith page and data is an array containing k elements
		}
	}
	head[total_pages-1].data = (int *)malloc((n-(total_pages-1)*keys_per_page)*sizeof(int));
	for(j=0;j<(n-(total_pages-1)*keys_per_page);j++)
		{
			head[total_pages-1].data[j]=arr[(total_pages-1)*keys_per_page + j];
		}																				//corner case: if number of elements is not divisible by k then last page will have lesser elements than k

	return head;
}

void print_array(int *arr, int num)
{
	for(int i=0 ;i< ceiling(num,keys_per_page)-1 ;i++)
	{
		for(int j=0;j<keys_per_page  ;j++)
		{
			printf("%d ", arr[i*keys_per_page + j]);
		}
		printf("\t");
	}
	for(int j=0;j<num-(ceiling(num,keys_per_page)-1)*keys_per_page  ;j++)
		{
			printf("%d ", arr[(ceiling(num,keys_per_page)-1)*keys_per_page + j]);
		}
	printf("\n");
}

void print_array_out(int *arr, int num)
{
	for(int i=0 ;i< num;i++)
	{
			printf("%d ", arr[i]);
	}
	printf("\n");
}

void pass_zero(page * head)
{
	runs = total_pages;
	//printf("%d\n",runs);
	run_size = 1;
	//mergesort(head[0].data,0,keys_per_page-1);
	//printf("sorted first page\n");
	for(int i=0; i< runs-1 ;i++)
	{
		mergesort(head[i].data, 0 ,keys_per_page-1);			//internally sort all the pages
	}
	mergesort(head[runs-1].data, 0 , n -1 -(runs-1)*keys_per_page);
	//printf("sorted pages returned\n");
	int factor = number_of_pages;
	int runs_out, run_size_out;
	runs_out = ceiling(runs, factor);
	run_size_out = run_size*factor;
	printf("----------starting pass 0----------\n" );
	printf("number of input runs %d\n",runs );
	printf("number of output runs %d\n",runs_out );
	int arr[run_size_out*keys_per_page];						//take B pages at a time into memory and merge them 
	for(int i=0;i < runs_out-1; i++)
	{
		printf("input of run %d\n", i+1);

		for(int j=0 ;j< run_size_out*keys_per_page; j++)
		{
			arr[j] = head[i*run_size_out+j/keys_per_page].data[j%keys_per_page];						//merge these B/b -1 arrays
		}
		print_array(arr, run_size_out*keys_per_page);
		mergesort(arr, 0 , run_size_out*keys_per_page -1);
		printf("output of run %d\n",i+1 );
		print_array_out(arr, run_size_out*keys_per_page);
	}
	printf("input of run %d\n",runs_out);
	int ar[n-(runs_out-1)*run_size_out*keys_per_page];													//remaining elements are n-(runsout-1)run_size*keys
	for(int j=0 ;j< (n-(runs_out-1)*run_size_out*keys_per_page); j++)
		{
			ar[j] = head[(runs_out-1)*run_size_out+j/keys_per_page].data[j%keys_per_page];						//merge these B/b -1 arrays
		}
	print_array(ar, n-(runs_out-1)*run_size_out*keys_per_page);									//error
	mergesort(ar, 0 , n-(runs_out-1)*run_size_out*keys_per_page -1);
	printf("output of run %d\n",runs_out );
	print_array_out(ar, n-(runs_out-1)*run_size_out*keys_per_page);
	runs= runs_out;															//runs will be n/B
	run_size= run_size_out;													//size will B pages per run

}

void pass_i(page * head, int t)
{
	int factor = (number_of_pages/buffer_per_input -1);
	int runs_out, run_size_out;
	runs_out = ceiling(runs, factor);							//same as pass zero except that factor now is B/b-1
	run_size_out = run_size*factor;
	printf("----------starting pass %d----------\n",t );
	printf("number of input runs %d\n",runs );
	printf("number of output runs %d\n",runs_out );

	int arr[run_size_out*keys_per_page];
	for(int i=0;i < runs_out-1; i++)
	{
		printf("input of run %d\n", i+1);

		for(int j=0 ;j< run_size_out*keys_per_page; j++)
		{
			arr[j] = head[i*run_size_out+j/keys_per_page].data[j%keys_per_page];						//merge these B/b -1 arrays
		}
		print_array(arr, run_size_out*keys_per_page);
		mergesort(arr, 0 , run_size_out*keys_per_page -1);
		printf("output of run %d\n",i+1 );
		print_array_out(arr, run_size_out*keys_per_page);
	}
	printf("input of run %d\n",runs_out);
	int ar[n-(runs_out-1)*run_size_out*keys_per_page];													//remaining elements are n-(runsout-1)run_size*keys
	for(int j=0 ;j< (n-(runs_out-1)*run_size_out*keys_per_page); j++)
		{
			ar[j] = head[(runs_out-1)*run_size_out+j/keys_per_page].data[j%keys_per_page];						//merge these B/b -1 arrays
		}
	print_array(ar, n-(runs_out-1)*run_size_out*keys_per_page);									//error
	mergesort(ar, 0 , n-(runs_out-1)*run_size_out*keys_per_page -1);
	printf("output of run %d\n",runs_out );
	print_array_out(ar, n-(runs_out-1)*run_size_out*keys_per_page);
	runs= runs_out;
	run_size= run_size_out;
}

void extsort(int * arr)
{
	page * head= make_pages(arr);					//make pages of n elements of each size k. 
	//printf("pages made\n");
	pass_zero(head);
	int i=1;
	while(runs > 1)
	{
		pass_i(head , i++);							//merge p/b-1 sorted runs and output 1 run
	}
}

int main(int argc, char **argv) 		
{
	number_of_pages = atoi(argv[1]);		//B is number of buffer pages
	keys_per_page = atoi(argv[2]);			//k is keys per page
	buffer_per_input = atoi(argv[3]);		//b is buffer per input

	char input[50];							//	./a.out B k b input.txt output.txt
	strcpy(input,argv[4]);

	char output[50];
	strcpy(output,argv[5]);

	FILE *fp = fopen(input,"r");
	char temp;
	temp = getc(fp);
	ungetc(temp,fp);

	fscanf(fp, "%d",&n);				//first element in file is total input size
	temp = getc(fp);
	int arr[n];

	for(int i=0;i<n;i++)
	{
		ungetc(temp,fp);
		fscanf(fp, "%d",&arr[i]);			//read input from file
		temp = getc(fp);
	}
	fclose(fp);

	extsort(arr);
}