#include <iostream>
#include <string.h>
#include <stdlib.h>

#define MB 1024*1024

using namespace std;

int main(){
	int op;
	cout << "alocar 50mb?\n";
	while(cin >> op && op){
		void *block = (void*)malloc(50*MB);
		memset(block, 1, 50*MB);
		cout << "alocar mais 50mb?\n";
	}
}
