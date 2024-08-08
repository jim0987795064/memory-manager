#include "MemManager.h"

void Memory_Manager(char process, int virtual_page_number, char* TLB_Policy, char* Page_Policy, char* Frame_Policy)
{


    int process_number = process - 65;
    LRU_times++;
    Local_FIFO_Position[process_number]++;
    Total_Access[process_number]++;



    Find_TLB_entry = 0;


    //TLB hit or not hit
    Total_Effective_Access[process_number]++;
    for(int i = 0; i < 32; i++)
    {

        if(TLB[i].VPN == virtual_page_number)
        {

            TLB[i].count = LRU_times;
            page_table[process_number][virtual_page_number].Reference = 1;
            frame[page_table[process_number][virtual_page_number].PFN].reference = 1;

            printf("Process %c, TLB Hit, %d=>%d\n",
                   process, virtual_page_number, TLB[i].PFN);

            fprintf(trace_output, "Process %c, TLB Hit, %d=>%d\n",
                    process, virtual_page_number, TLB[i].PFN);

            TLB_Hit_Times[process_number]++;
            return;
        }
    }

    Total_Effective_Access[process_number]++;
    TLB_Hit_Times[process_number]++;


    //page fault

    if(page_table[process_number][virtual_page_number].PFN == -1)
    {
        Page_Fault_Times[process_number]++;
        //search free frame
        for(int i = 0; i < physical_frame; i++)
        {

            if(frame[i].process == ' ')
            {

                frame[i].process = process;
                frame[i].virtual_page_number = virtual_page_number;
                frame[i].reference = 1;
                page_table[process_number][virtual_page_number].PFN = i;
                page_table[process_number][virtual_page_number].Reference = 1;
                page_table[process_number][virtual_page_number].Present = 1;
                page_table[process_number][virtual_page_number].Local_FIFO_Count = Local_FIFO_Position[process_number];


                //free TLB entry
                for(int j = 0; j < 32; j++)
                {

                    if(TLB[j].VPN == -1)
                    {

                        TLB[j].VPN = virtual_page_number;
                        TLB[j].PFN = i;
                        TLB[j].count = LRU_times;
                        Find_TLB_entry = 1;
                        break;
                    }
                }
                //no free TLB entry
                if(Find_TLB_entry != 1)
                {
                    if((strcmp(TLB_Policy, "RANDOM")) == 0)
                        Random(virtual_page_number, i);
                    else if((strcmp(TLB_Policy, "LRU")) == 0)
                        LRU(virtual_page_number, i);

                }

                printf("Process %c, TLB Miss, Page Fault, %d,Evict -1 of Process %c to -1, %d<<%d\n",
                       process, i, process, virtual_page_number,
                       page_table[process_number][virtual_page_number].disk_block_number);

                fprintf(trace_output, "Process %c, TLB Miss, Page Fault, %d,Evict -1 of Process %c to -1, %d<<%d\n",
                        process, i, process, virtual_page_number,
                        page_table[process_number][virtual_page_number].disk_block_number);

                printf("Process %c, TLB Hit, %d=>%d\n",
                       process, virtual_page_number, i);

                fprintf(trace_output, "Process %c, TLB Hit, %d=>%d\n",
                        process, virtual_page_number, i);
                return;
            }
        }
        //page fault，no free frame
        if((strcmp(Frame_Policy, "GLOBAL")) == 0)
        {
            if((strcmp(Page_Policy, "FIFO")) == 0)
            {
                Global_FIFO(process, virtual_page_number, TLB_Policy, Page_Policy, Frame_Policy);
            }
            else if((strcmp(Page_Policy, "CLOCK")) == 0)
                Global_Clock(process, virtual_page_number, TLB_Policy, Page_Policy, Frame_Policy);
        }

        else if((strcmp(Frame_Policy, "LOCAL")) == 0)
        {
            //printf("%s\n", "ok");
            if((strcmp(Page_Policy, "FIFO")) == 0)
                Local_FIFO(process, virtual_page_number, TLB_Policy, Page_Policy, Frame_Policy);
            else if((strcmp(Page_Policy, "CLOCK")) == 0)
                Local_Clock(process, virtual_page_number, TLB_Policy, Page_Policy, Frame_Policy);
        }
    }

    //pault hit
    else
    {

        //update reference bit
        page_table[process_number][virtual_page_number].Reference = 1;
        frame[page_table[process_number][virtual_page_number].PFN].reference = 1;

        printf("Process %c, TLB Miss, Page Hit, %d=>%d\n",
               process, virtual_page_number,
               page_table[process_number][virtual_page_number].PFN);

        fprintf(trace_output, "Process %c, TLB Miss, Page Hit, %d=>%d\n",
                process, virtual_page_number,
                page_table[process_number][virtual_page_number].PFN);

        printf("Process %c, TLB Hit, %d=>%d\n",
               process, virtual_page_number,
               page_table[process_number][virtual_page_number].PFN);

        fprintf(trace_output, "Process %c, TLB Hit, %d=>%d\n",
                process, virtual_page_number,
                page_table[process_number][virtual_page_number].PFN);
        //update TLB
        for(int j = 0; j < 32; j++)
        {

            if(TLB[j].VPN == -1)
            {

                TLB[j].VPN = virtual_page_number;
                TLB[j].PFN = page_table[process_number][virtual_page_number].PFN;
                TLB[j].count = LRU_times;
                Find_TLB_entry = 1;
                break;
            }
        }
        //no free TLB entry
        if(Find_TLB_entry != 1)
        {
            if((strcmp(TLB_Policy, "RANDOM")) == 0)
                Random(virtual_page_number, page_table[process_number][virtual_page_number].PFN);
            else if((strcmp(TLB_Policy, "LRU")) == 0)
                LRU(virtual_page_number, page_table[process_number][virtual_page_number].PFN);

        }
    }


}

void Local_Clock(char process, int virtual_page_number, char* TLB_Policy, char* Page_Policy, char* Frame_Policy)
{

    int replace_position, fine_replace_position = 0;

    while(!fine_replace_position)
    {
        for(int i = Local_Clock_Position[process - 65]; i < physical_frame; i++)
        {
            if(frame[i].process == process && frame[i].reference == 0)
            {
                replace_position = i;
                fine_replace_position = 1;
                break;
            }

            if(frame[i].process == process && frame[i].reference == 1)
                frame[i].reference = 0;

            if(!fine_replace_position && i == (physical_frame - 1))
                Local_Clock_Position[process - 65] = 0;


        }
    }

    page_table[frame[replace_position].process - 65][frame[replace_position].virtual_page_number].PFN = -1;
    page_table[frame[replace_position].process - 65][frame[replace_position].virtual_page_number].Reference = 0;
    page_table[frame[replace_position].process - 65][frame[replace_position].virtual_page_number].Present = 0;

    int number = 0;
    for(number = 0; number < DISK_SIZE; number++)
        if(disk[number] == -1)
        {
            disk[number] = 1;
            page_table[frame[replace_position].process - 65][frame[replace_position].virtual_page_number].disk_block_number = number;
            break;
        }

    if(page_table[process - 65][virtual_page_number].disk_block_number != -1)
        disk[page_table[process - 65][virtual_page_number].disk_block_number] = -1;

    if(frame[replace_position].process == process)
        for(int j = 0; j < 32; j++)
        {
            if(TLB[j].VPN == frame[replace_position].virtual_page_number)
            {

                TLB[j].VPN = -1;
                TLB[j].PFN = -1;
                TLB[j].count = 0;//for LRU
                break;
            }
        }

    for(int j = 0; j < 32; j++)
    {

        if(TLB[j].VPN == -1)
        {

            TLB[j].VPN = virtual_page_number;
            TLB[j].PFN = replace_position;
            TLB[j].count = LRU_times;
            Find_TLB_entry = 1;
            break;
        }
    }
    //no free TLB entry
    if(Find_TLB_entry != 1)
    {
        if((strcmp(TLB_Policy, "RANDOM")) == 0)
            Random(virtual_page_number, replace_position);//check this line
        else if((strcmp(TLB_Policy, "LRU")) == 0)
            LRU(virtual_page_number, replace_position);

    }

    printf("Process %c, TLB Miss, Page Fault, %d, Evict %d of Process %c to %d, %d<<%d\n",
           process,
           replace_position,
           frame[replace_position].virtual_page_number,
           frame[replace_position].process,
           number,
           virtual_page_number,
           page_table[process - 65][virtual_page_number].disk_block_number);
    fprintf(trace_output, "Process %c, TLB Miss, Page Fault, %d, Evict %d of Process %c to %d, %d<<%d\n",
            process,
            replace_position,
            frame[replace_position].virtual_page_number,
            frame[replace_position].process,
            number,
            virtual_page_number,
            page_table[process - 65][virtual_page_number].disk_block_number);

    printf("Process %c, TLB Hit, %d=>%d\n",
           process, virtual_page_number, replace_position);

    fprintf(trace_output, "Process %c, TLB Hit, %d=>%d\n",
            process, virtual_page_number, replace_position);

    //process the page which should page in from disk
    frame[replace_position].process = process;
    frame[replace_position].virtual_page_number = virtual_page_number;
    frame[replace_position].reference = 1;
    page_table[process - 65][virtual_page_number].PFN = replace_position;
    page_table[process - 65][virtual_page_number].Reference = 1;
    page_table[process - 65][virtual_page_number].Present = 1;
    page_table[process - 65][virtual_page_number].disk_block_number = -1;


    Local_Clock_Position[process - 65] = replace_position + 1;

    if(Local_Clock_Position[process - 65] >= physical_frame)
        Local_Clock_Position[process - 65] = 0;


}

void Global_Clock(char process, int virtual_page_number, char* TLB_Policy, char* Page_Policy, char* Frame_Policy)
{

    int replace_position, fine_replace_position = 0;

    while(!fine_replace_position)
    {
        for(int i = Global_Clock_Position; i < physical_frame; i++)
        {
            if(frame[i].reference == 0)
            {

                replace_position = i;

                fine_replace_position = 1;
                break;
            }

            if(frame[i].reference == 1)
                frame[i].reference = 0;

            if(!fine_replace_position && i == (physical_frame - 1))
                Global_Clock_Position = 0;


        }
    }

    page_table[frame[replace_position].process - 65][frame[replace_position].virtual_page_number].PFN = -1;
    page_table[frame[replace_position].process - 65][frame[replace_position].virtual_page_number].Reference = 0;
    page_table[frame[replace_position].process - 65][frame[replace_position].virtual_page_number].Present = 0;

    int number = 0;
    for(number = 0; number < DISK_SIZE; number++)
        if(disk[number] == -1)
        {
            disk[number] = 1;
            page_table[frame[replace_position].process - 65][frame[replace_position].virtual_page_number].disk_block_number = number;
            break;
        }


    if(page_table[process - 65][virtual_page_number].disk_block_number != -1)
        disk[page_table[process - 65][virtual_page_number].disk_block_number] = -1;

    if(frame[replace_position].process == process)
        for(int j = 0; j < 32; j++)
        {
            if(TLB[j].VPN == frame[replace_position].virtual_page_number)
            {

                TLB[j].VPN = -1;
                TLB[j].PFN = -1;
                TLB[j].count = 0;//for LRU
                break;
            }
        }

    for(int j = 0; j < 32; j++)
    {

        if(TLB[j].VPN == -1)
        {

            TLB[j].VPN = virtual_page_number;
            TLB[j].PFN = replace_position;
            TLB[j].count = LRU_times;
            Find_TLB_entry = 1;
            break;
        }
    }
    //no free TLB entry
    if(Find_TLB_entry != 1)
    {
        if((strcmp(TLB_Policy, "RANDOM")) == 0)
            Random(virtual_page_number, replace_position);
        else if((strcmp(TLB_Policy, "LRU")) == 0)
            LRU(virtual_page_number, replace_position);

    }

    printf("Process %c, TLB Miss, Page Fault, %d, Evict %d of Process %c to %d, %d<<%d\n",
           process,
           replace_position,
           frame[replace_position].virtual_page_number,
           frame[replace_position].process,
           number,
           virtual_page_number,
           page_table[process - 65][virtual_page_number].disk_block_number);

    fprintf(trace_output, "Process %c, TLB Miss, Page Fault, %d, Evict %d of Process %c to %d, %d<<%d\n",
            process,
            replace_position,
            frame[replace_position].virtual_page_number,
            frame[replace_position].process,
            number,
            virtual_page_number,
            page_table[process - 65][virtual_page_number].disk_block_number);
    printf("Process %c, TLB Hit, %d=>%d\n",
           process, virtual_page_number, replace_position);

    fprintf(trace_output, "Process %c, TLB Hit, %d=>%d\n",
            process, virtual_page_number, replace_position);

    //process the page which should page in from disk
    frame[replace_position].process = process;
    frame[replace_position].virtual_page_number = virtual_page_number;
    frame[replace_position].reference = 1;
    page_table[process - 65][virtual_page_number].PFN = replace_position;
    page_table[process - 65][virtual_page_number].Reference = 1;
    page_table[process - 65][virtual_page_number].Present = 1;
    page_table[process - 65][virtual_page_number].disk_block_number = -1;


    Global_Clock_Position = replace_position + 1;

    if(Global_Clock_Position >= physical_frame)
        Global_Clock_Position = 0;

}



void Local_FIFO(char process, int virtual_page_number, char* TLB_Policy, char* Page_Policy, char* Frame_Policy)
{

    int min = 0;
    int min_position = 0;
    int PFN_Position = 0;

    for(int i = 0; i < virtual_page; i++)
    {
        if(page_table[process - 65][i].Local_FIFO_Count != -1)
        {
            min = page_table[process - 65][i].Local_FIFO_Count;
            min_position = i;
            break;
        }
    }
    for(int i = 0; i < virtual_page; i++)
    {
        if(page_table[process - 65][i].Local_FIFO_Count != -1 && page_table[process - 65][i].Local_FIFO_Count < min)
        {
            min = page_table[process - 65][i].Local_FIFO_Count;
            min_position = i;
        }
    }

    PFN_Position = page_table[process - 65][min_position].PFN;

    page_table[process - 65][min_position].PFN = -1;
    page_table[process - 65][min_position].Local_FIFO_Count = -1;
    page_table[process - 65][min_position].Reference = 0;
    page_table[process - 65][min_position].Present = 0;

    int number = 0;
    for(number = 0; number < DISK_SIZE; number++)
        if(disk[number] == -1)
        {
            disk[number] = 1;
            page_table[process - 65][min_position].disk_block_number = number;
            break;
        }
    if(page_table[process - 65][virtual_page_number].disk_block_number != -1)
        disk[page_table[process - 65][virtual_page_number].disk_block_number] = -1;
    for(int j = 0; j < 32; j++)
    {
        if(TLB[j].VPN == min_position)
        {

            TLB[j].VPN = -1;
            TLB[j].PFN = -1;
            TLB[j].count = 0;
            break;
        }
    }

    for(int j = 0; j < 32; j++)
    {
        if(TLB[j].VPN == -1)
        {

            TLB[j].VPN = virtual_page_number;
            TLB[j].PFN = PFN_Position;
            TLB[j].count = LRU_times;
            Find_TLB_entry = 1;
            break;
        }
    }

    if(Find_TLB_entry != 1)
    {
        if((strcmp(TLB_Policy, "RANDOM")) == 0)
            Random(virtual_page_number, PFN_Position);
        else if((strcmp(TLB_Policy, "LRU")) == 0)
            LRU(virtual_page_number, PFN_Position);

    }

    printf("Process %c, TLB Miss, Page Fault, %d, Evict %d of Process %c to %d, %d<<%d\n",
           process,
           PFN_Position,
           frame[PFN_Position].virtual_page_number,
           frame[PFN_Position].process,
           number,
           virtual_page_number,
           page_table[process - 65][virtual_page_number].disk_block_number);

    fprintf(trace_output, "Process %c, TLB Miss, Page Fault, %d, Evict %d of Process %c to %d, %d<<%d\n",
            process,
            PFN_Position,
            frame[PFN_Position].virtual_page_number,
            frame[PFN_Position].process,
            number,
            virtual_page_number,
            page_table[process - 65][virtual_page_number].disk_block_number);

    printf("Process %c, TLB Hit, %d=>%d\n",
           process, virtual_page_number, PFN_Position);

    fprintf(trace_output, "Process %c, TLB Hit, %d=>%d\n",
            process, virtual_page_number, PFN_Position);

    //process the page which should page in from disk
    frame[PFN_Position].process = process;
    frame[PFN_Position].virtual_page_number = virtual_page_number;
    page_table[process - 65][virtual_page_number].PFN = PFN_Position;
    page_table[process - 65][virtual_page_number].Reference = 1;
    page_table[process - 65][virtual_page_number].Present = 1;
    page_table[process - 65][virtual_page_number].disk_block_number = -1;




    page_table[process - 65][virtual_page_number].Local_FIFO_Count = Local_FIFO_Position[process - 65];

}




void Global_FIFO(char process, int virtual_page_number, char* TLB_Policy, char* Page_Policy, char* Frame_Policy)
{



    //process the page which should page out to disk
    page_table[frame[Global_FIFO_Position].process - 65][frame[Global_FIFO_Position].virtual_page_number].PFN = -1;
    page_table[frame[Global_FIFO_Position].process - 65][frame[Global_FIFO_Position].virtual_page_number].Reference = 0;
    page_table[frame[Global_FIFO_Position].process - 65][frame[Global_FIFO_Position].virtual_page_number].Present = 0;




    //disk position of page in page

    int number = 0;
    for(number = 0; number < DISK_SIZE; number++)
        if(disk[number] == -1)
        {
            disk[number] = 1;
            page_table[frame[Global_FIFO_Position].process - 65][frame[Global_FIFO_Position].virtual_page_number].disk_block_number = number;
            break;
        }

    if(page_table[process - 65][virtual_page_number].disk_block_number != -1)
        disk[page_table[process - 65][virtual_page_number].disk_block_number] = -1;


    //VPN which is paged out, should be cleard in TLB
    if(frame[Global_FIFO_Position].process == process)
        for(int j = 0; j < 32; j++)
        {
            if(TLB[j].VPN == frame[Global_FIFO_Position].virtual_page_number)
            {

                TLB[j].VPN = -1;
                TLB[j].PFN = -1;
                TLB[j].count = 0;
                break;
            }
        }
    for(int j = 0; j < 32; j++)
    {

        if(TLB[j].VPN == -1)
        {

            TLB[j].VPN = virtual_page_number;
            TLB[j].PFN = Global_FIFO_Position;
            TLB[j].count = LRU_times;
            Find_TLB_entry = 1;
            break;
        }
    }
    //no free TLB entry
    if(Find_TLB_entry != 1)
    {
        if((strcmp(TLB_Policy, "RANDOM")) == 0)
            Random(virtual_page_number, Global_FIFO_Position);
        else if((strcmp(TLB_Policy, "LRU")) == 0)
            LRU(virtual_page_number, Global_FIFO_Position);

    }

    printf("Process %c, TLB Miss, Page Fault, %d, Evict %d of Process %c to %d, %d<<%d\n",
           process,
           Global_FIFO_Position,
           frame[Global_FIFO_Position].virtual_page_number,
           frame[Global_FIFO_Position].process,
           number,
           virtual_page_number,
           page_table[process - 65][virtual_page_number].disk_block_number);

    fprintf(trace_output, "Process %c, TLB Miss, Page Fault, %d, Evict %d of Process %c to %d, %d<<%d\n",
            process,
            Global_FIFO_Position,
            frame[Global_FIFO_Position].virtual_page_number,
            frame[Global_FIFO_Position].process,
            number,
            virtual_page_number,
            page_table[process - 65][virtual_page_number].disk_block_number);

    printf("Process %c, TLB Hit, %d=>%d\n",
           process, virtual_page_number, Global_FIFO_Position);

    fprintf(trace_output, "Process %c, TLB Hit, %d=>%d\n",
            process, virtual_page_number, Global_FIFO_Position);

    //process the page which should page in from disk
    frame[Global_FIFO_Position].process = process;
    frame[Global_FIFO_Position].virtual_page_number = virtual_page_number;
    page_table[process - 65][virtual_page_number].PFN = Global_FIFO_Position;
    page_table[process - 65][virtual_page_number].Reference = 1;
    page_table[process - 65][virtual_page_number].Present = 1;
    page_table[process - 65][virtual_page_number].disk_block_number = -1;



    Global_FIFO_Position++;

    if(Global_FIFO_Position >= physical_frame)
        Global_FIFO_Position = 0;
}

void Random(int virtual_page_number, int physical_frame_number)
{

    srand(time(NULL));

    //TLB random replace
    replace_position = rand()%32;

    TLB[replace_position].VPN = virtual_page_number;
    TLB[replace_position].PFN = physical_frame_number;

}

void LRU(int VPN, int PFN)
{
    int min = TLB[0].count;
    int min_position = 0;
    for(int k = 0; k < 32; k++)
    {
        if(min > TLB[k].count)
        {
            min = TLB[k].count;
            min_position = k;
        }
    }
    TLB[min_position].VPN = VPN;
    TLB[min_position].PFN = PFN;
    TLB[min_position].count = LRU_times;
}


void Set_Sys_Config()
{

    const char* filename = "sys_config.txt";
    config_buffer = malloc(sizeof(char) * 1000000);

    FILE* input_file = fopen(filename, "r");

    fread(config_buffer, 1000, 1, input_file);

    char* token_config = strtok(config_buffer, "\n");
    const char ch = ':';




    while (token_config != NULL)
    {
        if(config == 1)
            TLB_Replacement_Policy = memchr(token_config, ch, strlen(token_config));
        else if(config == 2)
            Page_Replacement_Policy = memchr(token_config, ch, strlen(token_config));
        else if(config == 3)
            Frame_Allocation_Policy = memchr(token_config, ch, strlen(token_config));
        else if(config == 4)
            Number_of_Processes = memchr(token_config, ch, strlen(token_config));
        else if(config == 5)
            Number_of_Virtual_Page = memchr(token_config, ch, strlen(token_config));
        else if(config == 6)
            Number_of_Physical_Frame = memchr(token_config, ch, strlen(token_config));
        token_config = strtok(NULL, "\n");
        config++;
    }
    sscanf(TLB_Replacement_Policy, ": %s\0", TLB_Replacement_Policy);
    sscanf(Page_Replacement_Policy, ": %s\0", Page_Replacement_Policy);
    sscanf(Frame_Allocation_Policy, ": %s\0", Frame_Allocation_Policy);
    sscanf(Number_of_Processes, ": %s\0", Number_of_Processes);
    sscanf(Number_of_Virtual_Page, ": %s\0", Number_of_Virtual_Page);
    sscanf(Number_of_Physical_Frame, ": %s\0", Number_of_Physical_Frame);


    TLB_Policy = malloc(strlen(TLB_Replacement_Policy) + 1);
    Page_Policy = malloc(strlen(Page_Replacement_Policy) + 1);
    Frame_Policy = malloc(strlen(Frame_Allocation_Policy) + 1);

    char *content_of_Processes = malloc(strlen(Number_of_Processes) + 1);
    char *content_of_Virtual_Page = malloc(strlen(Number_of_Virtual_Page) + 1);
    char *content_of_Physical_Frame = malloc(strlen(Number_of_Physical_Frame) + 1);

    strcpy(TLB_Policy, TLB_Replacement_Policy);
    strcpy(Page_Policy, Page_Replacement_Policy);
    strcpy(Frame_Policy, Frame_Allocation_Policy);

    strcpy(content_of_Processes, Number_of_Processes);
    process = atoi(content_of_Processes);
    strcpy(content_of_Virtual_Page, Number_of_Virtual_Page);
    virtual_page = atoi(content_of_Virtual_Page);
    strcpy(content_of_Physical_Frame, Number_of_Physical_Frame);
    physical_frame = atoi(content_of_Physical_Frame);



    memset(config_buffer, ' ', 250);

    //show(TLB_Policy, Page_Policy, Frame_Policy);
    //initial page_table,-1 means not in memory
    for(int i = 0; i < process; i++)
        for(int j = 0; j < virtual_page; j++)
        {
            page_table[i][j].PFN = -1;
            page_table[i][j].Reference = 0;
            page_table[i][j].Present = 0;
            page_table[i][j].Local_FIFO_Count = -1;
            page_table[i][j].disk_block_number = -1;
        }

    for(int i = 0; i < 20; i++)
        Local_FIFO_Position[i] = 0;

    for(int i = 0; i < 20; i++)
        Local_Clock_Position[i] = 0;

    //no page in disk
    for(int i = 0; i < DISK_SIZE; i++)
        disk[i] = -1;



    //initial physical frame
    for(int i = 0; i < physical_frame; i++)
    {
        frame[i].process = ' ';
        frame[i].virtual_page_number = -1;
        frame[i].reference = 0;
    }

    for(int i = 0; i < 20; i++)
    {
        Total_Access[i] = 0;
        Page_Fault_Times[i] = 0;
        Page_Fault_Rate[i] = 0;
        Total_Effective_Access[i] = 0;
        TLB_Hit_Times[i] = 0;
        Effective_Access_Time[i] = 0;
    }


    Flush_TLB();

    //clear number
    for(int i = 0; i < 4; i++)
        number[i] = ' ';


    fclose(input_file);
}
void Flush_TLB()
{

    LRU_times = 0;


    for(int i = 0; i < 32; i++)
    {
        TLB[i].VPN = -1;
        TLB[i].PFN = -1;
        TLB[i].count = 0;
    }
}

void Trace(char* TLB_Policy, char* Page_Policy, char* Frame_Policy)
{
    const char* filename = "trace.txt";
    //char* config_buffer = malloc(sizeof(char) * 20000);
    char* trace;

    FILE* input_file = fopen(filename, "r");
    trace_output = fopen( "trace_output.txt", "w" );
    analysis = fopen( "analysis.txt", "w" );

    fread(config_buffer, 100000, 1, input_file);

    char *token_trace = strtok(config_buffer, "\n");
    //must be initialized
    //char *number = malloc(7 * sizeof(char));
    trace = malloc(strlen(token_trace) + 1);
    int VPN;
    while (token_trace != NULL)
    {


        sscanf(token_trace, "Reference( %[^)]", trace);
        if(strlen(trace) == 4)
            strncpy(number, trace + 3, 1);
        else if(strlen(trace) == 5)
            strncpy(number, trace + 3, 2);
        else if(strlen(trace) == 6)
            strncpy(number, trace + 3, 3);
        else if(strlen(trace) == 7)
            strncpy(number, trace + 3, 4);

        VPN = atoi(number);
        //clear number
        for(int i = 0; i < 4; i++)
            number[i] = ' ';

        //flush TLB
        if(current_process != ' ' &&  current_process != trace[0])
            Flush_TLB();


        current_process = trace[0];
        //printf("%s %s %s\n", TLB_Policy, Page_Policy, Frame_Policy);
        Memory_Manager(trace[0], VPN, TLB_Policy, Page_Policy, Frame_Policy);

        token_trace = strtok(NULL, "\n");

    }
}


void Process_Analysis()
{
    for(int i = 0; i < process; i++)
    {

        Effective_Access_Time[i] = 120*TLB_Hit_Times[i]/Total_Effective_Access[i] +
                                   220*(1 - TLB_Hit_Times[i]/Total_Effective_Access[i]);

        Effective_Access_Time[i] = (int)(Effective_Access_Time[i]*1000 + 0.5)/1000.0;
        printf("Process %c, Effective Access Time = %.3f\n",
               i + 65, Effective_Access_Time[i]);
        fprintf(analysis, "Process %c, Effective Access Time = %.3f\n",
                i + 65, Effective_Access_Time[i]);

        Page_Fault_Rate[i] = Page_Fault_Times[i]/Total_Access[i];
        Page_Fault_Rate[i] = (int)(Page_Fault_Rate[i]*1000 + 0.5)/1000.0;
        printf("Process %c, Page Fault Rate: %.3f\n",
               i + 65, Page_Fault_Rate[i]);

        fprintf(analysis, "Process %c, Page Fault Rate: %.3f\n",
                i + 65, Page_Fault_Rate[i]);
    }
}

int main()
{

    number = malloc(4 * sizeof(char));
    current_process = malloc(1 * sizeof(char));
    current_process = ' ';


    Set_Sys_Config();

    Trace(TLB_Policy, Page_Policy, Frame_Policy);

    Process_Analysis();


    return 0;
}