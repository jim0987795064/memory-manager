#ifndef MEMMANAGER_H
#define MEMMANAGER_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#define DISK_SIZE 40960

char* TLB_Replacement_Policy, *Page_Replacement_Policy, *Frame_Allocation_Policy;
char* Number_of_Processes, *Number_of_Virtual_Page, *Number_of_Physical_Frame;
//char* TLB_Policy, *Page_Policy, *Frame_Policy, content[];
//char content[];
char* TLB_Policy, *Page_Policy, *Frame_Policy;
char* config_buffer;
char  current_process;
int process = 0, virtual_page = 0, physical_frame = 0;
int config = 1;
int Find_TLB_entry = 0;
unsigned int LRU_times = 0, Global_FIFO_Position = 0, Global_Clock_Position = 0;
unsigned int Local_FIFO_Position[20], Local_Clock_Position[20];
float Total_Access[20], Page_Fault_Times[20], Page_Fault_Rate[20],
      Total_Effective_Access[20], TLB_Hit_Times[20], Effective_Access_Time[20];

FILE *trace_output, *analysis;

struct memory
{
    char process;
    int virtual_page_number;
    int reference;
};
struct TLB_entry
{
    int VPN;
    int PFN;
    int count;
};
struct Page_Table
{

    int PFN;
    int Reference;
    int Present;
    int Local_FIFO_Count;
    int disk_block_number;

};

struct Free_Frame_List
{
    int free_frame_position;
    struct Free_Frame_List *next;
};

struct Replacement_List
{
    char process;
    int virtual_page_number;
};

struct memory frame[1024];
struct TLB_entry TLB[32];
struct Free_Frame_List *free_frame;
struct Page_Table page_table[20][2048];
int disk[DISK_SIZE];


char *number;
int replace_position;



void Set_Sys_Config();
void Memory_Manager(char, int, char* TLB_Policy, char* Page_Policy, char* Frame_Policy);
void Trace(char* TLB_Policy, char* Page_Policy, char* Frame_Policy);
void Random(int, int);
void LRU(int, int);
void Global_FIFO(char, int, char* TLB_Policy, char* Page_Policy, char* Frame_Policy);
void Local_FIFO(char, int, char* TLB_Policy, char* Page_Policy, char* Frame_Policy);
void Global_Clock(char, int, char* TLB_Policy, char* Page_Policy, char* Frame_Policy);
void Local_Clock(char, int, char* TLB_Policy, char* Page_Policy, char* Frame_Policy);
void Flush_TLB();
void Process_Analysis();

#endif