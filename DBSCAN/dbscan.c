#include <stdio.h>
#include <math.h>

typedef struct
{
    float x;
    float y;
} Point;

float distance(Point a, Point b)
{
    float dx = a.x - b.x;
    float dy = a.y - b.y;

    return sqrtf(dx * dx + dy * dy);
}

int check_neighbours(Point arr_points[], int n, int ind, float eps, int neighbours_arr[]){

    int cnt = 0;

    for(int pn=0;pn<n;pn++){
        if(distance(arr_points[ind], arr_points[pn])<=eps){
            neighbours_arr[cnt++] = pn; // can be a neighbour
        }
    }
    return cnt;
}
void expand_cluster(Point arr_points[], int n, int labels[], int ind, int neighbours_arr[], int neighbours_cnt, int cluster, float eps,int minpts){

    int i=0;

    while(i<neighbours_cnt){
        int pn = neighbours_arr[i];

        //if its noise we can bring this to this cluster c
        if(labels[pn] == -1) labels[pn] = cluster;

        else if (labels[pn] == 0){
            labels[pn] = cluster;

            //find other neighbours of pn
            int neighbours_pn_arr[100];

            int neighbours_pn_cnt = check_neighbours(arr_points,n,pn,eps,neighbours_pn_arr);

            if(neighbours_pn_cnt >= minpts){
               //add in the queue again
               for(int j=0;j<neighbours_pn_cnt;j++)
               neighbours_arr[neighbours_cnt++] = neighbours_pn_arr[j]; 
            }
        }
    i++;
    }
}
void dbscan(Point arr_points[], int n, float eps, int minpts, int labels[]){

    int cluster = 0;

    for(int i=0;i<n;i++){
        if(labels[i] != 0){
            continue;
        }

        int neighbours_arr[100];

        int neighbours_cnt = check_neighbours(arr_points,n,i,eps,neighbours_arr);

        if(neighbours_cnt < minpts)
            labels[i] = -1; //noise and can be changed later to another cluster if eligible
        else{
            //we found a cluster so
            cluster+=1;
            expand_cluster(arr_points,n,labels,i,neighbours_arr,neighbours_cnt,cluster,eps,minpts);
        }
    }
}

int main(){
    Point arr_points[] = {{1.0, 1.0},{2.0, 1.0},{1.0, 2.0},{2.0, 2.0},{1.5, 1.5},{8.0, 8.0},{8.5, 8.0}};

    int n = sizeof(arr_points)/sizeof(arr_points[0]);
    float eps = 1.5;
    int minpts = 3;

    int labels[100];

    for(int i=0;i<100;i++){
        labels[i] = 0;
    }

    dbscan(arr_points,n,eps,minpts,labels);

    for(int i=0;i<n;i++){
        int labl = labels[i];
        if(labl == -1){
            printf("NOISE\n");
        }
        else{
            printf("Cluster%d\n",labl);
        }
    }

}