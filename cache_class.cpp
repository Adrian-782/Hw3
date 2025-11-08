#include <iostream>
#include <fstream>
#include <cmath>

using namespace std;

class Entry {
public:
    Entry();
    ~Entry();
    void display(ofstream& outfile);

    void set_tag(unsigned long _tag) { tag = _tag; }
    unsigned long get_tag() { return tag; }

    void set_valid(bool _valid) { valid = _valid; }
    bool get_valid() { return valid; }

    void set_ref(int _ref) { ref = _ref; }
    int get_ref() { return ref; }

private:
    bool valid;
    unsigned long tag;
    int ref;
};

class Cache {
public:
    Cache(int, int);
    ~Cache();

    void display(ofstream& outfile);

    int get_index(unsigned long addr);
    unsigned long get_tag(unsigned long addr);

    unsigned long retrieve_addr(int way, int index);

    bool hit(ofstream& outfile, unsigned long addr);
    void update(unsigned long addr);

private:
    int assoc;
    unsigned num_entries;
    int num_sets;
    Entry **entries;
    int access_count;
};

Entry::Entry() {
    valid = false;
    tag = 0;
    ref = -1;
}

Entry::~Entry() {
}

void Entry::display(ofstream& outfile) {
    outfile << "Valid: " << valid << " Tag: " << tag << " Ref: " << ref << endl;
}

Cache::Cache(int entries, int associativity) {
    assoc = associativity;
    num_entries = entries;
    num_sets = entries / associativity;
    access_count = 0;
    
    this->entries = new Entry*[num_sets];
    for (int i = 0; i < num_sets; i++) {
        this->entries[i] = new Entry[assoc];
    }
}

Cache::~Cache() {
    for (int i = 0; i < num_sets; i++) {
        delete[] entries[i];
    }
    delete[] entries;
}

int Cache::get_index(unsigned long addr) {
    int index_bits = log2(num_sets);
    unsigned long mask = (1 << index_bits) - 1;
    return addr & mask;
}

unsigned long Cache::get_tag(unsigned long addr) {
    int index_bits = log2(num_sets);
    return addr >> index_bits;
}

bool Cache::hit(ofstream& outfile, unsigned long addr) {
    int index = get_index(addr);
    unsigned long tag = get_tag(addr);
    access_count++;
    
    for (int way = 0; way < assoc; way++) {
        if (entries[index][way].get_valid() && entries[index][way].get_tag() == tag) {
            entries[index][way].set_ref(access_count);
            return true;
        }
    }
    return false;
}

void Cache::update(unsigned long addr) {
    int index = get_index(addr);
    unsigned long tag = get_tag(addr);
    access_count++;
    
    for (int way = 0; way < assoc; way++) {
        if (!entries[index][way].get_valid()) {
            entries[index][way].set_valid(true);
            entries[index][way].set_tag(tag);
            entries[index][way].set_ref(access_count);
            return;
        }
    }
    int lru_way = 0;
    int min_ref = entries[index][0].get_ref();
    
    for (int way = 1; way < assoc; way++) {
        if (entries[index][way].get_ref() < min_ref) {
            min_ref = entries[index][way].get_ref();
            lru_way = way;
        }
    }
    
    entries[index][lru_way].set_tag(tag);
    entries[index][lru_way].set_ref(access_count);
}

unsigned long Cache::retrieve_addr(int way, int index) {
    if (entries[index][way].get_valid()) {
        int index_bits = log2(num_sets);
        unsigned long tag = entries[index][way].get_tag();
        return (tag << index_bits) | index;
    }
    return 0;
}

void Cache::display(ofstream& outfile) {
    outfile << "Cache Configuration:" << endl;
    outfile << "Total entries: " << num_entries << endl;
    outfile << "Associativity: " << assoc << endl;
    outfile << "Number of sets: " << num_sets << endl;
    
    for (int i = 0; i < num_sets; i++) {
        outfile << "Set " << i << ":" << endl;
        for (int j = 0; j < assoc; j++) {
            outfile << "  Way " << j << ": ";
            entries[i][j].display(outfile);
        }
    }
}

