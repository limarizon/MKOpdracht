#include <iostream>
#include <vector>

void myFunc();

template <class T>
int BSearch(std::vector<T>* vec, T* value, int start, int end);
template <class T>
void swap(std::vector<T>* vec, T* t1, T* t2);

//int main(){
//	myFunc();
//	std::cout << "Test."<< std::endl;
//	return 0;
//}

void myFunc(){
	std::vector<int>* vec = new std::vector<int>();
	vec->push_back(5);
	vec->push_back(8);

	int val = 7;
	int pos = BSearch(vec, &val, 0, vec->size() - 1 );
	vec->insert(vec->begin() + pos,val);

	val = 9;
	pos = BSearch(vec, &val, 0, vec->size() -1 );
	auto it = pos == vec->size() ? vec->end() : vec->begin() + pos;
	vec->insert(it,val);	


	pos = BSearch(vec, &val, 0, vec->size() -1 );
    it = pos == vec->size() ? vec->end() : vec->begin() + pos;
    vec->insert(it,val);


	auto swap1 = 5;
	auto swap2 = 7;
	swap(vec, &swap1, &swap2);


	std::cout << "Printing..." << std::endl;
	for(auto element: *vec){
		std::cout << element << " "; 
	}
	std::cout << std::endl;
	delete vec;
}

template <class T>
int BSearch(std::vector<T>* vec, T* value, int start, int end){
	while(start<=end){
		int index = (start + end) / 2;
		if(vec->at(index) < *value){
			start = index + 1;
		}else if(vec->at(index) > *value){
			end = index -1;
		}else{
			return index;
		}
	}
	if(start == 0){
		return 0;
	}else{
		return end+1;
	}
}

template <class T>
void swap(std::vector<T>* vec, T* t1, T* t2){
	int pos1 = BSearch(vec, t1, 0, vec->size()-1);
	int pos2 = BSearch(vec, t2, 0, vec->size() - 1);
	T upper = vec->at( pos1 );
	T lower = vec->at( pos2 );
	if(vec->begin() + pos1 == vec->end()){
		std::cerr << "ERROR SWAP";
	}
	else if (!(pos1 == pos2+1 || pos1 + 1 == pos2)) {
		std::cerr << "ERROR SWAP";
	}
}
