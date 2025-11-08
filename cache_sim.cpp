#include <iostream>
#include <fstream>
#include <cstdlib>
#include <string>
#include "cache_class.cpp"

using namespace std;

const int MAX_SIZE = 10000;

int main(int argc, char* argv[]) {
    if (argc < 4) {
        cout << "Usage: " << endl;
        cout << " ./cache_sim num_entries associativity filename" << endl;
        return 0;
    }

    unsigned entries = atoi(argv[1]);
    unsigned assoc = atoi(argv[2]);
    string input_filename = argv[3];

    cout << "Number of entries: " << entries << endl;
    cout << "Associativity: " << assoc << endl;
    cout << "Input File Name: " << input_filename << endl;

    Cache cache(entries, assoc);

    string output_filename = "cache_sim_output";

    ofstream output;
    ifstream input;

    input.open(input_filename.c_str());

    if (!input.is_open()) {
        cerr << "Could not open input file " << input_filename << ". Exiting ..." << endl;
        exit(0);
    }

    unsigned long *nums = new unsigned long[MAX_SIZE];
    int count = 0;
    while (!input.eof() && count < MAX_SIZE) {
        input >> nums[count];
        count++;
    }
    input.close();

    output.open(output_filename.c_str());

    for (int i = 0; i < count; i++) {
        unsigned long addr = nums[i];
       
        if (cache.hit(output, addr)) {
            output << addr << " : HIT" << endl;
        } else {
            output << addr << " : MISS" << endl;
            cache.update(addr);
        }
    }

    output.close();
    
    delete[] nums;
    
    cout << "Output written to " << output_filename << endl;
    
    return 0;
}