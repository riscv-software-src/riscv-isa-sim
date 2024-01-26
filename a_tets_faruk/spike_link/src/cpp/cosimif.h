#ifndef COSIMIF_H
#define COSIMIF_H

void init();

void step();

// /// @brief for key and value arrays: packed dimension size: dim0; num entries: dim1; entry size in packets: dim2
// /// @param key_array the keys of the unordered_map is written to this array
// /// @param value_array the values of the unordered_map is written to this array in same order with keys written in the key_array
// /// @param num_entries_inserted is the output parameter to specify the caller how many elements are valid in the output
// void get_last_commit(const svOpenArrayHandle key_array, const svOpenArrayHandle value_array, int *num_entries_inserted);

#endif // COSIMIF_H