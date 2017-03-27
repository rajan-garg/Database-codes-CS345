#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <limits.h>
#include <math.h>

int global_depth, keys_per_bucket, how_to_delete;
int **dir;

void create_dir(void)
{
	int slots;
	slots = (int) pow(2,global_depth);
	dir = (int **)malloc(slots*sizeof(int *));

	int i,j;
	for(i=0; i<slots; i++)
	{
		dir[i] = (int *)malloc((keys_per_bucket + 1)* sizeof(int));
	}

	for(i=0; i<slots; i++)
	{
		dir[i][0] = global_depth;
		for(j=1; j < (keys_per_bucket + 1); j++)
		{
			dir[i][j] = INT_MIN;
		}
	}
}

int get_hash(int value, int depth)
{
	int slots = (int) pow(2,depth);
	return (value % slots);
}

int get_index(int hash_value)
{
	int i;
	for(i=1; i< (keys_per_bucket+1); i++)
	{
		if(dir[hash_value][i] == INT_MIN)
			return i;
	}
	return -1;
}

char *get_id(int i, int local_depth)
{
	char *id = (char *) malloc( (local_depth+1) * sizeof(char) );
	id[local_depth] = '\0';
	while(local_depth--)
	{
		id[local_depth] = i%2 + '0';
		i = i/2;
	}
	return id;
}

void allocate_buckets(int index)
{
	dir[index][0]++;
	int slots = (int) pow(2,dir[index][0]);

	dir[index + slots/2] = (int *)malloc((keys_per_bucket+1) * sizeof(int));
	dir[index + slots/2][0] = dir[index][0];
	int i;
	for(i=1; i < (keys_per_bucket+1); i++)
	{
		dir[index + slots/2][i] = INT_MIN;
	}  
}

void relocate_keys(int index)
{
	int slots = (int) pow(2,dir[index][0]);
	int i, j = 1;
	for(i=1; i < (keys_per_bucket+1); i++)
	{
		int temp = get_hash(dir[index][i],dir[index][0]);
		if(temp != index)
		{
			dir[index + slots/2][j++] = dir[index][i];
			dir[index][i] = INT_MIN;
		}
	}
}



void double_dir(void)
{
	global_depth++;
	int slots = (int) pow(2,global_depth);
	dir = (int **)realloc(dir, slots * sizeof(int *));

	int i;
	for(i=0; i< slots/2; i++)
	{
		dir[i + slots/2] = dir[i];
	}
}

void insert(int value)
{
	int hash_value = get_hash(value, global_depth);
	int idx = get_index(hash_value);

	if(idx != -1)
	{
		dir[hash_value][idx] = value;
		char *id = get_id(hash_value,dir[hash_value][0]);
		printf("Inserted \t%d\t at \t%s\t location.\n", value, id);
		free(id);
	}
	else
	{
		if(dir[hash_value][0] != global_depth)
		{
			hash_value = get_hash(value, dir[hash_value][0]);
			allocate_buckets(hash_value);
			//relocate_pointers(hash_value);
			relocate_keys(hash_value);

			hash_value = get_hash(value, dir[hash_value][0]);
			idx = get_index(hash_value);
			dir[hash_value][idx] = value;

			char *id = get_id(hash_value,dir[hash_value][0]);
			printf("Inserted \t%d\t at \t%s\t location.\n", value, id);
			free(id);
		}
		else
		{
			double_dir();
			allocate_buckets(hash_value);
			relocate_keys(hash_value);

			hash_value = get_hash(value, dir[hash_value][0]);
			idx = get_index(hash_value);
			dir[hash_value][idx] = value;

			char *id = get_id(hash_value,dir[hash_value][0]);
			printf("Inserted \t%d\t at \t%s\t location.\n", value, id);
			free(id);
		}
	}
}

void search(int value)
{
	int hash_value = get_hash(value, global_depth);
	int i;
	for(i=1; i< (keys_per_bucket+1); i++)
	{
		if(dir[hash_value][i] == value)
		{
			char *id = get_id(hash_value,dir[i][0]);
			printf("Value \t%d\t is found in bucket \t%s.\t\n", value, id);
			free(id);
			return;
		}
	}
	printf("Value \t%d\t is not present.\n", value);
	return;
}

void status(void)
{
	printf("Global depth = %d\n",global_depth);
	int i,j;
	int slots = pow(2,global_depth);
	for(i=0;i<slots;i++)
	{
		if(i<pow(2,dir[i][0])){
			char * temp = get_id( i ,dir[i][0]);
			printf("%s --> ",temp);
			for(j=1;j<(keys_per_bucket+1);j++)
			{
				if(dir[i][j]!=INT_MIN)
					printf(" %d ",dir[i][j]);
			}
			printf("\n");
		}
	}
}

void delete_lazy(int value)
{
	int hash_value = get_hash(value, global_depth);
	int i;
	for(i=1; i< (keys_per_bucket+1); i++)
	{
		if(dir[hash_value][i] == value)
		{
			dir[hash_value][i] = INT_MIN;
			char *id = get_id(hash_value,dir[hash_value][0]);
			printf("Deleted Value \t%d\t from \t%s.\t\n", value, id);
			free(id);
			break;
		}
	}
	
}

int maxy(int a, int b)
{
	if(a>=b)
		return a;
	else
		return b;
}

int miny(int a, int b)
{
	if(a<=b)
		return a;
	else
		return b;
}

void delete_bucket_merge(int value)
{
	delete_lazy(value);
	int hash_value = get_hash(value, global_depth);
	int slots = pow(2,global_depth);
	int x= (hash_value + slots/2)%slots;
	int j,c=0;
	for(j=1;j<(keys_per_bucket+1);j++)
	{
		if(dir[hash_value][j]!=INT_MIN)
			c++;
		if(dir[x][j]!=INT_MIN)
			c++;
	}
	if(c<=keys_per_bucket && dir[hash_value][0]==global_depth && global_depth!=1)
	{
		dir[hash_value][0]--;
		dir[x][0]--;
		int max = maxy(hash_value,x);
		int min = miny(hash_value,x);
		for(j=1;j<(keys_per_bucket+1);j++)
		{
			if(dir[max][j]!=INT_MIN)
			{
					int hash_val = get_hash(dir[max][j] , dir[max][0]);
					int idx = get_index(hash_val);
					dir[hash_val][idx]=dir[max][j];
					dir[max][j] = INT_MIN;

			}
		}
		dir[max]=dir[min];
	}

}

void delete_merge_dir(int value)
{
	delete_bucket_merge(value);
	int slots = pow(2,global_depth);
	int i,flag=0;
	for(i=0 ; i<slots ; i++)
	{
		if(dir[i][0] >= global_depth)
			flag=1;
	}
	if(flag==0)
	{
		global_depth--;
		int slots = (int) pow(2,global_depth);
		dir = (int **)realloc(dir, slots * sizeof(int *));

	}
}

int main(int argc, char **argv)
{
	global_depth = atoi(argv[1]);
	keys_per_bucket = atoi(argv[2]);
	how_to_delete = atoi(argv[3]);

	char filename[50];
	strcpy(filename,argv[4]);

	FILE *fp = fopen(filename,"r");

	create_dir();

	char operation[50];
	char temp;
	temp = getc(fp);
	int value;

	while(temp != EOF)
	{
		ungetc(temp,fp);
		fscanf(fp, "%s", operation);

		if(!strcmp(operation,"insert"))
		{
			fscanf(fp, "%d", &value);
			insert(value);
		}
		else if(!strcmp(operation,"search"))
		{
			fscanf(fp, "%d", &value);
			search(value);
		}
		else if(!strcmp(operation,"status"))
		{
			status();
		}
		else if(!strcmp(operation,"delete"))
		{
			fscanf(fp, "%d", &value);
			if(how_to_delete == 0)
				delete_lazy(value);
			else if(how_to_delete == 1)
			 	delete_bucket_merge(value);
			else if(how_to_delete == 2)
		 		delete_merge_dir(value);
		}
		temp = getc(fp);
	}
	fclose(fp);
	return 0;
}
