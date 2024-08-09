# Memory Manager Code Explanation

This code is an implementation of a memory management system for an operating system, simulating a memory hierarchy with a Translation Lookaside Buffer (TLB), page tables, and disk storage. The system supports multiple processes, each with its own virtual address space, and manages page replacement policies and frame allocation strategies.

## Key Components

### 1. Data Structures
- **`memory`**: Represents a frame in physical memory, storing the process ID and virtual page number.
- **`TLB_entry`**: Represents an entry in the TLB, mapping a Virtual Page Number (VPN) to a Physical Frame Number (PFN).
- **`Page_Table`**: Represents the page table for a process, mapping virtual pages to physical frames and maintaining other metadata like reference bits and disk block numbers.
- **`Free_Frame_List`**: A linked list of free frames available for allocation.
- **`Replacement_List`**: Stores the process and virtual page number for pages that are replaced (evicted) from memory.

### 2. Memory Management Functions

#### `Memory_Manager`
- Manages the memory access for a process.
- Checks the TLB for a hit; if not found, checks the page table.
- Handles page faults by finding a free frame or evicting a page using a specified page replacement policy.
- Updates the TLB based on the configured replacement policy (e.g., Random, LRU).

#### `Local_Clock`, `Global_Clock`
- Implement the Clock page replacement algorithm in local and global contexts, respectively.
- The Clock algorithm checks if pages are referenced; if not, they are candidates for eviction.

#### `Local_FIFO`, `Global_FIFO`
- Implement the FIFO page replacement algorithm for local and global contexts, respectively.
- Pages are evicted in the order they were loaded into memory.

#### `Random`, `LRU`
- Implement TLB replacement policies.
- `Random` replaces a random TLB entry, while `LRU` (Least Recently Used) replaces the entry that was accessed the longest time ago.

### 3. Configuration and Initialization

#### `Set_Sys_Config`
- Reads system configuration from a file (`sys_config.txt`), including TLB replacement policy, page replacement policy, frame allocation policy, number of processes, virtual pages, and physical frames.
- Initializes data structures, including page tables, TLB, and physical memory frames.

#### `Flush_TLB`
- Clears the TLB by setting all entries to invalid (VPN = -1).

### 4. Tracing and Analysis

#### `Trace`
- Reads memory access traces from a file (`trace.txt`) and simulates memory accesses.
- Calls `Memory_Manager` to handle each memory access, logging TLB hits, page faults, and other memory operations.

#### `Process_Analysis`
- Calculates and prints statistics for each process, including the Effective Access Time (EAT) and Page Fault Rate.
- EAT is computed considering the time to access memory with or without a TLB hit.
- Page Fault Rate is the ratio of page faults to total memory accesses for a process.

### 5. Main Function

#### `main`
- The entry point of the program.
- Initializes the system by calling `Set_Sys_Config`, then processes memory accesses using `Trace`.
- After processing, it performs an analysis of memory access patterns using `Process_Analysis`.

## Summary

This code provides a simulation of memory management in an operating system, including TLB management, page replacement, and frame allocation. It supports multiple processes, each with its own virtual address space, and can be configured to use different memory management policies. The system tracks and analyzes memory access patterns, providing detailed statistics on the performance of the memory hierarchy.
