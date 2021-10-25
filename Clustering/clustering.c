#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <limits.h>
#include <float.h>
#include "../Vector/vector.h"
#include "../hashTable/hashTableList/hashTableList.h"
#include "../hashTable/hashTable.h"
#include "../LSH/lsh.h"

#define SQUARE(x) ((x)*(x))

#define TRUE 1
#define FALSE 0
#define CONVERGENCE 100

extern int numOfVecs;
extern int d;
extern int hashTableSize;

int existsInArray(int *array,int check,int arraySize){
  for(int i=0;i<arraySize;i++){
    if(array[i]==-1){
      return FALSE;
    }
    if(array[i]==check){
      return TRUE;
    }
  }
  return FALSE;
}


void minDistToCentroids(Vector v,Vector* vecs,Vector *clusters,int numOfClusters,double *minDistance){

  for(int i=0;i<numOfClusters;i++){
    if(clusters[i]==NULL){
      break;
    }
    double tempDist = distance_metric(clusters[i],v,d);
    if(tempDist<(*minDistance)){
        (*minDistance) = tempDist;
    }
  }
}

void minDistbetweenCentroids(Vector *centroids,int numOfClusters,double *minDistance){
  // can be impoved
  for(int i=0;i<numOfClusters;i++){
    if(centroids[i]==NULL){
      break;
    }
    for(int j=0;j<numOfClusters;j++){
      if(i!=j){
        if(centroids[j]==NULL){
          break;
        }
        double tempDist = distance_metric(centroids[i],centroids[j],d);
        if(tempDist<(*minDistance)){
          (*minDistance) = tempDist;
        }
      }

    }
  }
}

void kmeansplusplus(Vector* vecs,int numOfClusters,Vector* clusters,double *props){
  int t=1;
  int selectedCluster[numOfClusters];
  for(int i=0;i<numOfClusters;i++){
    selectedCluster[i] = -1;
  }
  int selectFirstCluster = rand()%numOfVecs;
  selectedCluster[0] = selectFirstCluster;
  clusters[0] = copyVector(vecs[selectFirstCluster]);
  double sum = 0.0;
  while(t<numOfClusters){
    for(int i = 0; i<numOfVecs;i++){
      if(existsInArray(selectedCluster,i,numOfClusters)){
        continue;
      }
      double minDist = DBL_MAX;
      minDistToCentroids(vecs[i],vecs,clusters,numOfClusters,&minDist);
      props[i] = minDist;

      sum += SQUARE(minDist);
    }

    double maxProb = DBL_MAX;
    int maxProbIndex = -1;

    for(int i = 0; i<numOfVecs;i++){
      if(existsInArray(selectedCluster,i,numOfClusters)){
        continue;
      }
      props[i] = SQUARE(props[i])/sum;
      if(props[i]<maxProb){
        maxProb=props[i];
        maxProbIndex = i;
      }
    }
    selectedCluster[t] = maxProbIndex;
    clusters[t++] = copyVector(vecs[maxProbIndex]);
  }
}

int centroidsCovnerge(Vector *new,Vector *old,int numOfClusters,int d){
  if(old==NULL) return FALSE;
  for(int i=0;i<numOfClusters;i++){
    printf("%f\n",distance_metric(new[i],old[i],d));
    if(distance_metric(new[i],old[i],d)>CONVERGENCE){
      return FALSE;
    }
  }
  return TRUE;
}

int findClosestCentroid(Vector v,Vector *clusters,int numOfClusters){
  int minDistIndex = -1;
  double minDist = DBL_MAX;
  for(int i=0;i<numOfClusters;i++){
    double tempDist = distance_metric(v,clusters[i],d);
    if(tempDist<minDist){
      minDistIndex = i;
      minDist = tempDist;
    }
  }
  return minDistIndex;
}

void lloyds(Vector* clusters,Vector *oldClusters,Vector* vectors,List* clustersList,int numberOfVectors,int numOfClusters) {
  static int flag=0;
  if(flag) //skip it for the first time
    for(int i=0;i<numOfClusters;i++){
      Vector newCenter;
      if(clustersList[i]!=NULL){
        newCenter=listMeanOfCluster(clustersList[i],d);
      }
      else{
        newCenter=copyVector(oldClusters[i]);
      }
      listDelete(clustersList[i],0);
      clustersList[i] = NULL;

      clusters[i]=newCenter;
    }

  for(int i=0;i<numberOfVectors;i++){
    int closestCentroid = findClosestCentroid(vectors[i],clusters,numOfClusters);
    clustersList[closestCentroid] = listInsert(clustersList[closestCentroid],vectors[i],d);
  }
  flag=1;
}

void clustering(List vecList,int numOfClusters){
  Vector *vectors;
  Vector *clusters;
  Vector *oldClusters = NULL;
  double *props;

  vectors = transformListToArray(vecList,numOfVecs);
  clusters = malloc(numOfClusters*sizeof(Vector));
  oldClusters = malloc(numOfClusters*sizeof(Vector));
  for(int i=0;i<numOfClusters;i++){
    clusters[i] = NULL;
    oldClusters[i] = NULL;
  }
  props = calloc(numOfVecs,sizeof(double));


  kmeansplusplus(vectors,numOfClusters,clusters,props);

  int firstIter = TRUE;

  //lloyds
  List *clustersList=malloc(numOfClusters*sizeof(List *));
  for(int i=0;i<numOfClusters;i++){
    clustersList[i]=initializeList();
  }
  int count=0;
  while((count<2) || !centroidsCovnerge(clusters,oldClusters,numOfClusters,d)){
  // while(firstIter || count<20){
    printf("ITER %d\n",count);
    count++;
    if(!firstIter){
      Vector *temp = oldClusters;
      oldClusters=clusters;
      clusters = temp;
      for(int i=0;i<numOfClusters;i++){
        if(clusters[i]!=NULL){
          deleteVector(clusters[i]);
          clusters[i] = NULL;
        }
      }
    }

    //
    lloyds(clusters,oldClusters,vectors,clustersList,numOfVecs,numOfClusters);

    firstIter=FALSE;
  }

  printf("==============================\n");
  //
  // for(int i=0;i<numOfClusters;i++){
  //   printf("- CLUSTER :%d\n",i);
  //   printVector(clusters[i]);
  //   printf("- CLUSTER LIST\n");
  //   listPrint(clustersList[i]);
  // }

  //Reverse approach
  HashTable *clustersHt=malloc(numOfClusters*sizeof(HashTable *));
  for(int i=0;i<numOfClusters;i++){
    clustersHt[i]= htInitialize(50); // TODO: CHANGE SIZE
  }
  double radius=DBL_MAX;
  vectors = transformListToArray(vecList,numOfVecs);
  kmeansplusplus(vectors,numOfClusters,clusters,props);
  minDistbetweenCentroids(clusters,numOfClusters,&radius);
  hashTableSize=numOfVecs/8;
  LSH lsh = initializeLSH(6);
  for(int i=0;i<numOfVecs;i++)
    insertToLSH(lsh,vectors[i]);
  for(int i=0;i<numOfVecs;i++)
    initVectorConflictArr(vectors[i],numOfClusters);

  // while(){ // stop when the 80% of vectors has a cluster (do it with global var)
    List confList=initializeList();
    for(int i=0;i<numOfClusters;i++){
      radiusNeigborClustering(lsh,clusters[i],radius,clustersHt[i],i,confList);

    }


    // manage the conflicts to clusters, conflicts vectors are inside the list
    // at the conflicts vectors set the corresponding indexes of their array (conflictArr) to zero
    listDelete(confList,0);
    radius*=2;
  // }




  for(int i=0;i<numOfClusters;i++){
    listDelete(clustersList[i],0);
    deleteVector(oldClusters[i]);
    deleteVector(clusters[i]);
    htDelete(clustersHt[i],0);
  }
  free(clustersList);

  free(props);
  free(vectors);
  free(oldClusters);
  free(clusters);
  destroyLSH(lsh);
}
