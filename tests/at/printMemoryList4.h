//
// Created by ariel on 25/06/2021.
//

#ifndef OS_WET4_PRINTMEMORYLIST_H
#define OS_WET4_PRINTMEMORYLIST_H

//void printMemory1(void* start);
//void printMemory2(void* start);
//void printMemory3(void* start);
//void printMemory4(void* start);

#include <iostream>
#include "malloc_4.h"

typedef struct stats_t {
	size_t num_free_blocks = 0;
	size_t num_free_bytes = 0;
	size_t num_allocated_blocks = 0;
	size_t num_allocated_bytes = 0;
	size_t num_meta_data_bytes = 0;
} stats;


template<class T>
void printMemory(void *start, bool onlyList) {
	T *current = (T *) start;
	size_t size = 0;
	int blocks = 0;
	if (!onlyList) {
		std::cout << "Printing Memory List\n";
	}
	while (current->size != 0) {
		if (current->free) {
			std::cout << "|F:" << current->size;
		} else {
			std::cout << "|U:" << current->size;
		}
		size += current->size;
		blocks++;
		current = current->next_l;
	}
	std::cout << "|";
	if (!onlyList) {
		std::cout << std::endl << "Memory Info:\nNumber Of Blocks: " << blocks << "\nTotal Size (without Metadata): " << size << std::endl;
		std::cout << "Size of Metadata: " << sizeof(T) << std::endl;
	}
}

template<class T>
void checkSizeIsMultipleOf8(T *m) {
	if(m->size % 8 != 0) {
		std::cout << "block size is not multiple of 8" << std::endl;
	}
}

int _getOffset(void *address) {
	int offset = (unsigned long long) address % 8;
	if (offset == 0) { // todo check
		return offset;
	}
	offset = 8 - offset;
	return offset;
}

template<class T>
void printMemory4(void *start, bool onlyList) {
	T *current = (T *) start;
	size_t size = 0;
	int blocks = 0;
	if (!onlyList) {
		std::cout << "Printing Memory List\n";
	}
	while (current->size != 0) {
		if (_getOffset((char *) current) != 0 || _getOffset((char *) current + sizeof(T)) != 0) {
			std::cout << "|bad offset: " << _getOffset((char *) current ) << " ->|";
			checkSizeIsMultipleOf8<T>(current);
		}
		if (current->free) {
			std::cout << "|F:" << current->size;
		} else {
			std::cout << "|U:" << current->size;
		}
		size += current->size;
		blocks++;
		current = current->next_l;
	}
	std::cout << "|";
	if (!onlyList) {
		std::cout << std::endl << "Memory Info:\nNumber Of Blocks: " << blocks << "\nTotal Size (without Metadata): " << size << std::endl;
		std::cout << "Size of Metadata: " << sizeof(T) << std::endl;
	}
}

void resetStats(stats &current_stats) {
	current_stats.num_allocated_blocks = 0;
	current_stats.num_allocated_bytes = 0;
	current_stats.num_free_blocks = 0;
	current_stats.num_free_bytes = 0;
	current_stats.num_meta_data_bytes = 0;
}


template<class T>
void updateStats(void *start, stats &current_stats, size_t bytes_mmap, int blocks_mmap) {
	resetStats(current_stats);
	T *current = (T *) start;
	while (current->size != 0) {
		current_stats.num_meta_data_bytes += sizeof(T);
		current_stats.num_allocated_bytes += current->size;
		current_stats.num_allocated_blocks++;
		if (current->free) {
			current_stats.num_free_blocks++;
			current_stats.num_free_bytes += current->size;
		}
		current = current->next_l;
	}
	current_stats.num_meta_data_bytes += sizeof(T) * blocks_mmap;
	current_stats.num_allocated_bytes += bytes_mmap;
	current_stats.num_allocated_blocks += blocks_mmap;
}


template<class T>
void updateStats4(void *start, stats &current_stats, size_t bytes_mmap, int blocks_mmap) {
	resetStats(current_stats);
	T *current = (T *) start;
	while (current->size != 0) {
		current_stats.num_meta_data_bytes += sizeof(T);
		current_stats.num_allocated_bytes += current->size + _get_padding();
		current_stats.num_allocated_blocks++;
		if (current->free) {
			current_stats.num_free_blocks++;
			current_stats.num_free_bytes += current->size;
		}
		current = current->next_l;
	}
	current_stats.num_meta_data_bytes += sizeof(T) * blocks_mmap;
	current_stats.num_allocated_bytes += bytes_mmap + blocks_mmap*_get_padding();
	current_stats.num_allocated_blocks += blocks_mmap;
}


#endif //OS_WET4_PRINTMEMORYLIST_H
