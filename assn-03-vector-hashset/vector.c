#include "vector.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <search.h>

void VectorNew(vector *v, int elemSize, VectorFreeFunction freeFn, int initialAllocation){
    assert(elemSize > 0);
    assert(initialAllocation >= 0);
    if(initialAllocation == 0) initialAllocation = 10;
    v->elem_size = elemSize;
    v->alloc_len = initialAllocation;
    v->sizer = initialAllocation;
    v->log_len = 0;
    v->free_fn = freeFn;
    v->elems = malloc(v->alloc_len * v->elem_size);
    assert(v->elems != NULL);
}

void VectorDispose(vector *v){
    if(v->free_fn != NULL){
        for(int i = 0; i < v->log_len; i++){
            v->free_fn((char *)v->elems + i * v->elem_size);
        }
    }
    free(v->elems);
}

int VectorLength(const vector *v){
    return v->log_len; 
}

void *VectorNth(const vector *v, int position){
    assert(position >= 0 && position < v->log_len);
    void * elem_pos = ((char *)v->elems) + position * v->elem_size;
    return elem_pos;
}

void VectorReplace(vector *v, const void *elemAddr, int position){
    assert(position >= 0 && position < v->log_len);
    if(v->free_fn != NULL) v->free_fn((char *)v->elems + position * v->elem_size);
    memcpy((char *)v->elems + position * v->elem_size, elemAddr, v->elem_size);
}

void VectorInsert(vector *v, const void *elemAddr, int position){
    assert(position >= 0 && position <= v->log_len);
    if(v->log_len == position){
        VectorAppend(v, elemAddr);
        return;
    }
    if(v->log_len == v->alloc_len){
        v->alloc_len = v->alloc_len + v->sizer;
        v->elems = realloc(v->elems, v->alloc_len * v->elem_size);
        assert(v->elems != NULL);
    }
    memmove(((char *)v->elems) + (position + 1) * v->elem_size,(char*) v->elems + position*v->elem_size, v->elem_size * (v->log_len - position));
    memcpy(((char *)v->elems) + position * v->elem_size, elemAddr, v->elem_size);
    v->log_len++;
}

void VectorAppend(vector *v, const void *elemAddr){
    if(v->log_len == v->alloc_len){
        v->alloc_len = v->alloc_len + v->sizer;
        v->elems = realloc(v->elems, v->alloc_len * v->elem_size);
        assert(v->elems != NULL);
    }
    memcpy((char *)v->elems + v->elem_size * v->log_len, elemAddr, v->elem_size);
    v->log_len++;
}

void VectorDelete(vector *v, int position){
    assert(position >= 0 && position < v->log_len);
    if(v->free_fn != NULL) v->free_fn(((char *)v->elems) + position * v->elem_size);
    if(position != v->log_len - 1) memcpy(((char *)v->elems) + position * v->elem_size, ((char *)v->elems) + (position + 1) * v->elem_size, (v->log_len - position - 1) * v->elem_size);
    v->log_len --;
}

void VectorSort(vector *v, VectorCompareFunction compare){
    assert(compare != NULL);
    qsort(v->elems, v->log_len, v->elem_size, compare);
}

void VectorMap(vector *v, VectorMapFunction mapFn, void *auxData){
    assert(mapFn != NULL);
    for(int i = 0; i < v->log_len; i++){
        mapFn((((char *)v->elems) + i * v->elem_size), auxData);
    }
}

static const int kNotFound = -1;
int VectorSearch(const vector *v, const void *key, VectorCompareFunction searchFn, int startIndex, bool isSorted){
    assert(startIndex >= 0 && startIndex <= v->log_len);
    void * answ = NULL;
    if(isSorted){
        answ = bsearch(key, (char *)v->elems + startIndex, v->log_len - startIndex, v->elem_size, searchFn);
    } else {
        size_t k = v->log_len - startIndex;
        size_t m = v->elem_size;
        answ = lfind(key, (char *)v->elems + startIndex, &k, m, searchFn);
    }
    if(answ != NULL) return ((char *)answ - (char *)v->elems) / v->elem_size;
    return kNotFound; 
} 
