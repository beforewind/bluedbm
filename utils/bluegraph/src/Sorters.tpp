#ifndef __SORTERS_TPP__
#define __SORTERS_TPP__

#include <cstdlib>

template <class keyType, class valType>
void* get_offset_ptr(void* buffer, int offset) {
	void* np = (void*)(((uint8_t*)buffer)+(sizeof(keyType)+sizeof(valType))*offset);
	return np;
}

template <class keyType, class valType>
void swap_kv(void* a, void* b) {
	keyType *pa = (keyType*)a;
	keyType *pb = (keyType*)b;
	valType* va = (valType*)(pa+1);
	valType* vb = (valType*)(pb+1);

	keyType tk = *pa;
	valType tv = *va;

	*pa = *pb;
	*va = *vb;

	*pb = tk;
	*vb = tv;
}

// returns true if a <= b
template <class keyType, class valType>
bool compare_kv(void* a, void* b) {
	keyType *pa = (keyType*)a;
	keyType *pb = (keyType*)b;

	keyType ka = *pa;
	keyType kb = *pb;

	if ( ka > kb ) return false;
	if ( ka < kb ) return true;

	valType* va = (valType*)(pa+1);
	valType* vb = (valType*)(pb+1);

	if ( *va > *vb ) return false;

	return true;
}

template <class keyType, class valType>
bool compareeq_kv(void* a, void* b) {
	keyType *pa = (keyType*)a;
	keyType *pb = (keyType*)b;

	keyType ka = *pa;
	keyType kb = *pb;

	if ( ka != kb ) return false;

	valType* va = (valType*)(pa+1);
	valType* vb = (valType*)(pb+1);

	if ( *va != *vb ) return false;

	return true;
}

template <class keyType, class valType>
int count_from_bytes(int bytes) {
	return (bytes/(sizeof(keyType)+sizeof(valType)));
}
template <class keyType>
int count_from_bytes(int bytes) {
	return (bytes/(sizeof(keyType)));
}

template <class keyType, class valType>
bool check_sorted(void* buffer, int count) {
	for ( int i = 1; i < count; i++ ) {
		void* lastp = get_offset_ptr<keyType,valType>(buffer, i-1);
		void* curp = get_offset_ptr<keyType,valType>(buffer, i);

		if ( !compare_kv<keyType,valType>(lastp,curp) ) {
			return false;
		}
	}
	return true;
}
template <class keyType>
bool check_sorted(void* buffer, int count) {
	for ( int i = 1; i < count; i++ ) {
		void* lastp = get_offset_ptr<keyType>(buffer, i-1);
		void* curp = get_offset_ptr<keyType>(buffer, i);

		if ( !compare_kv<keyType>(lastp,curp) ) {
			return false;
		}
	}
	return true;
}

template <class keyType, class valType>
void bubble_sort_block(void* buffer, int count) {
	//uint64_t* buffer = (uint64_t*)bufferv;


	for ( int i = 0; i < count; i++ ) {
		void* bi = get_offset_ptr<keyType,valType>(buffer, i);
		for ( int j = i; j < count; j++ ) {
			void* bj = get_offset_ptr<keyType,valType>(buffer, j);

			if ( !compare_kv<keyType,valType>(bi,bj) ) {
				swap_kv<keyType,valType>(bi,bj);
			}
		}
	}

	for ( int i = 1; i < count; i++ ) {
		void* lastp = get_offset_ptr<keyType,valType>(buffer, i-1);
		void* curp = get_offset_ptr<keyType,valType>(buffer, i);

		if ( !compare_kv<keyType,valType>(lastp,curp) ) {
			printf( "Block sort has errors!\n" );
		}
	}
}

template <class keyType, class valType>
int compareKvp(const void*a, const void*b) {
	keyType ak = *((keyType*)a);
	keyType bk = *((keyType*)b);

	if ( ak < bk ) return -1;
	if ( ak > bk ) return 1;
	return 0;
}

template <class keyType, class valType>
void quick_sort_lib(void* buffer, int count) {
	qsort(buffer, count, sizeof(keyType)+sizeof(valType),compareKvp<keyType,valType>);
	if ( !check_sorted<keyType,valType>(buffer,count) ) {
		printf( "qsort failed to sort!\n" );
	}
}

template <class keyType, class valType>
void quick_sort_block(void* buffer, int count) {
	qsort(buffer, count, sizeof(keyType)+sizeof(valType),compareKvp<keyType,valType>);
	return;
/*
	uint64_t* buffer = (uint64_t*)bufferv;
	*/
	//printf( "sort called with block size %d\n", count );

	if ( count <= 1 ) return;

	if ( count <= 32 ) {
		bubble_sort_block<keyType, valType>(buffer,count);
		return;
	};

	if ( check_sorted<keyType,valType>(buffer,count) ) {
		return;
	}

	void* zp = get_offset_ptr<keyType,valType>(buffer, 0);
	//uint64_t z[2] = {buffer[0], buffer[1]};

	int nel = 0;
	bool alleq = true;
	for ( int i = 1; i < count; i++ ) {
		void* cp = get_offset_ptr<keyType,valType>(buffer, i);
		if ( !compareeq_kv<keyType,valType> (zp,cp) ) {
			alleq = false;
			nel = i;
			break;
		}
	}
	if ( alleq ) return;

	if ( nel > 0 ) {
	
		//int mid = count/2;
		//int div = (mid>nel)?mid:nel;
		int div = nel+(count-nel)/2;
		void* cp = get_offset_ptr<keyType,valType>(buffer, div);
	
		//void* cp = get_offset_ptr<keyType,valType>(buffer, nel);
		swap_kv<keyType,valType>(zp,cp);
	} else return;

	int lc = 1;
	for ( int i = 2; i < count; i++ ) {
		//uint64_t a[2] = {buffer[i*2], buffer[i*2+1]};
		void* cp = get_offset_ptr<keyType,valType>(buffer, i);
		if ( compare_kv<keyType,valType>(cp,zp) ) {
			//swap(buffer, i,lc);
			void* lp = get_offset_ptr<keyType,valType>(buffer, lc);
			swap_kv<keyType,valType>(lp,cp);
			lc++;
		}
	}
	void* lp = get_offset_ptr<keyType,valType>(buffer, lc-1);
	swap_kv<keyType,valType>(zp,lp);

	void* lpn = get_offset_ptr<keyType,valType>(buffer, lc);
	quick_sort_block<keyType,valType>(buffer, lc);
	quick_sort_block<keyType,valType>(lpn, count-lc);
}

#endif
