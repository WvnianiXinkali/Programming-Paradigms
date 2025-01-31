#include "hashset.h"
#include <assert.h>
#include <stdlib.h>
#include <string.h>

void HashSetNew(hashset *h, int elemSize, int numBuckets,
		HashSetHashFunction hashfn, HashSetCompareFunction comparefn, HashSetFreeFunction freefn){
		assert(elemSize > 0 && numBuckets > 0 && hashfn != NULL && comparefn != NULL);
		h->comparefn = comparefn;
		h->hashfn = hashfn;
		h->num_buckets = numBuckets;
		h->buck = malloc(numBuckets * sizeof(vector));
		assert(h->buck != NULL);
		h->log_len = 0;
		for(int i = 0; i < numBuckets; i++){
			VectorNew(&h->buck[i], elemSize, freefn, 0);
	}
}

// void VectorSort(vector *v, VectorCompareFunction comparefn);
//void VectorMap(vector *v, VectorMapFunction mapfn, void *auxData);
//int VectorSearch(const vector *v, const void *key, VectorCompareFunction searchfn, int startIndex, bool isSorted);
//void VectorDelete(vector *v, int position);
//void VectorReplace(vector *v, const void *elemAddr, int position);
//void VectorAppend(vector *v, const void *elemAddr);
//void VectorInsert(vector *v, const void *elemAddr, int position);
//void *VectorNth(const vector *v, int position);
//int VectorLength(const vector *v);
//void VectorDispose(vector *v);
//void VectorNew(vector *v, int elemSize, VectorFreeFunction freefn, int initialAllocation);

void HashSetDispose(hashset *h){
	for(int i = 0; i < h->num_buckets; i++){
		VectorDispose(&h->buck[i]);
	}
	free(h->buck);
}

int HashSetCount(const hashset *h){
	return h->log_len; 
}

void HashSetMap(hashset *h, HashSetMapFunction mapfn, void *auxData){
	assert(mapfn != NULL);
	for(int i = 0; i < h->num_buckets; i ++){
		VectorMap(&h->buck[i], mapfn, auxData);
	}
}

// void VectorSort(vector *v, VectorCompareFunction comparefn);
//void VectorMap(vector *v, VectorMapFunction mapfn, void *auxData);
//int VectorSearch(const vector *v, const void *key, VectorCompareFunction searchfn, int startIndex, bool isSorted);
//void VectorDelete(vector *v, int position);
//void VectorReplace(vector *v, const void *elemAddr, int position);
//void VectorAppend(vector *v, const void *elemAddr);
//void VectorInsert(vector *v, const void *elemAddr, int position);
//void *VectorNth(const vector *v, int position);
//int VectorLength(const vector *v);
//void VectorDispose(vector *v);
//void VectorNew(vector *v, int elemSize, VectorFreeFunction freefn, int initialAllocation);

void HashSetEnter(hashset *h, const void *elemAddr){
	assert(elemAddr != NULL);
	int code = h->hashfn(elemAddr, h->num_buckets);
	assert(code >= 0 && code < h->num_buckets);
	int find = VectorSearch(&h->buck[code], elemAddr, h->comparefn, 0, 0);
	if(find >= 0){
		VectorReplace(&h->buck[code], elemAddr, find);
	} else {
		VectorAppend(&h->buck[code], elemAddr);
		h->log_len++;
	}
}

void *HashSetLookup(const hashset *h, const void *elemAddr){
	assert(elemAddr != NULL);
    int code = h->hashfn(elemAddr, h->num_buckets);
    assert(code >= 0 && code < h->num_buckets);
	int find = VectorSearch(&h->buck[code], elemAddr, h->comparefn, 0, 0);
	if(find >= 0) return VectorNth(&h->buck[code], find);
	return NULL;
}
