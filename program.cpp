#include "DataHelper.h"
#include <iostream>
#include <omp.h>
#include <algorithm>
#include <queue>


int number_of_threads;
bool flag_variable = false;
std::queue<long> input;
std::vector<std::string> output;


void producer(DataHelper& dh) {
    long value = -1;
    do {
        value = dh.getData();
        input.push(value);
        
    } while (value != -1);

    flag_variable = true;
}

void consumer(DataHelper& dh) {
    bool close = false;
	bool sorted_list = false;
    do {
        long value = -1;
#pragma omp critical(pop)
        {
        
        if (!input.empty()) {
                value = input.front();
                input.pop();
            close = (value == -1);
        } else {
            close = flag_variable;
        }
        } 
        if (value != -1) {
            output.push_back(dh.analyze(value));
			if( (int)output.size() >= number_of_threads-1 ) {
				
				if(!sorted_list) {
					std::sort(output.begin(), output.end());
					sorted_list = true;
				}
			#pragma omp critical(print)
				{
					std::cout << output.front() << std::endl;
					output.erase(output.begin());
				}
			}
		}
    } while (!close);
	#pragma omp critical(print)
	while(!output.empty()) {
	{
			std::cout << output.front() << std::endl;
			output.erase(output.begin());
	}
	} 
}

void process(DataHelper& dh) {
#pragma omp parallel
    {
	number_of_threads = omp_get_num_threads();
	#pragma omp for
	for(int i=0; i<number_of_threads; i++) {
		if(i == 0){ 
		producer(dh); 
		}
		else{
		consumer(dh);
		}
	}

    } 
}
//------------------------------------------------------------
// DO NOT MODIFY CODE BELOW THIS LINE
//------------------------------------------------------------

int main(int argc, char *argv[]) {
    if (argc < 2) {
	std::cerr << "Usage: " << argv[0] << " <#NumToProcess> [<saltValue>]\n";
	return 1;
    }
    // Convert command-line arguments to integers.
    const int numToProcess = atoi(argv[1]);
    const int saltValue    = (argc > 2 ? atoi(argv[2]) : 0);
    // Create the data helper
    DataHelper dh(numToProcess, saltValue);
    // Process numbers from the data helper
    process(dh);
    // All close.
    return 0;
}
