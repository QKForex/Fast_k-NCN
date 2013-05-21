 #include "Distance.h"

namespace Common {

	int compareDistances(const void* a, const void* b) {
		Distance arg1 = *reinterpret_cast<const Distance*>(a);
		Distance arg2 = *reinterpret_cast<const Distance*>(b);
		if (arg1.distValue < arg2.distValue) return -1;
		if (arg1.distValue > arg2.distValue) return 1;
		return 0;
	}

	int compareNCNDistances(const void* a, const void* b) {
		NCNDistance arg1 = *reinterpret_cast<const NCNDistance*>(a);
		NCNDistance arg2 = *reinterpret_cast<const NCNDistance*>(b);
		if (arg1.distValue < arg2.distValue) return -1;
		if (arg1.distValue > arg2.distValue) return 1;
		return 0;
	}

	void swapDistances(Distance* dists,
		const int samIndexToMoveToBack, const int samIndexToMoveFromBack) {
		if (samIndexToMoveToBack != samIndexToMoveFromBack) {
			Distance tempDistance(dists[samIndexToMoveToBack]);
			dists[samIndexToMoveToBack] = dists[samIndexToMoveFromBack];
			dists[samIndexToMoveFromBack] = tempDistance;
		}
	}


	// select i-st largest number from A[p] to A[r]
	Distance randomizedSelect(Distance* A, int p, int r, int i) {
		if (p == r)
			return A[p];
		int q = randomizedPartition(A, p, r);
		int k = q - p + 1;
		if (i == k) // the pivot value is the answer
			return A[q];
		else if (i < k) // on the left
			return randomizedSelect(A, p, q-1, i);
		else // on the right
			return randomizedSelect(A, q+1, r, i-k);
	}

	// get the index of the number in the middle
	// s.t. subarray on the left <= A[index] <= subarray on the right
	int randomizedPartition(Distance* A, int p, int r) {
		int i = rand() % (r-p+1) + p;
		Distance tmp = A[i]; A[i] = A[r]; A[r] = tmp;
		return partition(A, p, r);
	}
		
	// pick A[r] as the flag
	// i for the left array <= A[r]
	// j for iterating the whole array
	// finally, exchange A[r] in the middle
	// return the new middle index
	int partition(Distance* A, int p, int r) {
		DistanceValue x = A[r].distValue;
		int i = p - 1;
		for (int j = p; j <= r-1; ++j)
			if (A[j].distValue <= x) {
				++i;
				Distance tmp = A[i]; A[i] = A[j]; A[j] = tmp;
			}
		Distance tmp = A[i+1]; A[i+1] = A[r]; A[r] = tmp;
		return i+1;
	}
}