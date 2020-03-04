#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void merge_sort(void *v[], int left, int right,
    int (*comp)(void *, void *));



void merge_sort(void *v[], int left, int right,
    int (*comp)(void *, void *))
{
    void merge(void *v[], int left,int middle, int right,
        int (*comp)(void *, void *));

    int middle;
    if (left >= right) /* do nothing if array contains */
        return; /* fewer than two elements */
    middle = ((right+left)/2);
    merge_sort(v, left, middle, comp);
    merge_sort(v, middle+1, right, comp);
    merge(v, left, middle, right, comp);
}

void merge(void *v[], int left,int middle, int right,
    int (*comp)(void *, void *)){
        
    void** auxLeft = (void *) malloc((middle - left+1)*sizeof(void *));
    void** auxRight= (void *) malloc((right - middle)*sizeof(void *));
    int j=0;
    for(int i =left ; i<=middle ; i++){
        auxLeft[j] = v[i];
        j++;
    }
    j=0;
    for(int i =middle+1 ; i<=right ; i++){
        auxRight[j] = v[i];
        j++;
    }
    int l=0,r=0;
    for(int i=left ; i<=right ; i++){
        if(l > middle - left){
            v[i] = auxRight[r];
            r++;
        }else if(r > right- (middle+1)){
            v[i] = auxLeft[l];
            l++;
        }else{
            int compare = ((* comp)( auxLeft[l], auxRight[r]));
            if(compare>0){
                //right goes first.
                v[i] = auxRight[r];
                r++;
            }else{
                //left goes first;
                v[i] = auxLeft[l];
                l++;
            }
        }
    }
    free(auxRight);
    free(auxLeft);
}
int numcmp(char *s1, char *s2) {
    double val1, val2;
    val1 = atof(s1);
    val2 = atof(s2);

    if(val1 < val2) {
        return -1;
    }
    else if(val1 > val2){
        return 1;
    }
    return 0;
}

void print(){
    //TODO
}


int main(int argc, char** argv){
    char numeric = 0;
    if(argc > 1 && strcmp(argv[1], "-n")==0){
        numeric = 1;
    }
    if(argc>1){
        merge_sort((void *)argv, numeric? 2 : 1, argc-1, (int(*)(void *, void *)) numeric ? numcmp:strcmp);
        for(int i=2;i<argc;i++){
            printf("%s ", argv[i]);
        }printf("\n"); 
    }   
}
