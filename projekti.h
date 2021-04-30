#ifndef PROJEKTI_H
#define PROJEKTI_H

//structs for date and entry
typedef struct {
    int month;
    int day;
    int hour;
} Date;

typedef struct {
    char description[80];
    Date date;
} Entry;

//functions in projekti.c
int datecmp(const void* entry1, const void* entry2);
unsigned int cdrlen(Entry* calendar);
Entry* add_entry(Entry* calendar, char* buffer);
Entry* delete_entry(Entry* calendar, char* buffer);
void print_calendar(Entry* calendar, char* buffer);
void save_calendar(Entry* calendar, char* buffer);
Entry* load_calendar(Entry* calendar, char* buffer);
void free_calendar(Entry* calendar);

#endif  //PROJEKTI_H