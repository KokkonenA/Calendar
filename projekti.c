#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "projekti.h"

//compare the given entries by date
int datecmp(const void* entry1, const void* entry2) {
    const Entry* e1 = entry1;
    const Entry* e2 = entry2;

    Date date1 = e1->date;
    Date date2 = e2->date;

    int d1 = date1.hour + 24 * date1.day + 24 * 31 * date1.month;
    int d2 = date2.hour + 24 * date2.day + 24 * 31 * date2.month;

    if (d1 < d2) {
        return -1;
    } else if (d1 == d2){
        return 0;
    } else {
        return 1;
    }
}

//return current length of the calendar.
unsigned int cdrlen(Entry* calendar) {
    unsigned int count = 0;

    while(calendar->description[0] != '\0') {
        count++;
        calendar++;
    }
    return count;
}

Entry* add_entry(Entry* calendar, char* buffer) {
    char description[80];
    int month;
    int day;
    int hour;

    //scan the string array given as parameter. Skip the first character.
    int scan = sscanf(buffer,"%*c %s %d %d %d", description, &month, &day, &hour);

    //check if input scan was succesfull
    if (scan != 4) {
        printf("Wrong type of input format\n");
        return calendar;
    }

    //check if the date and time is reasonable
    if (month < 0 || month > 12 || day < 1 || day > 31 || hour < 1 || hour > 24) {
        printf("Incorrect type of date\n");
        return calendar;
    }

    //create new instance of Date and initialize the parametres with given values
    Date date = { month, day, hour };

    //construct new entry which is used as key in binsary search
    Entry key = { "key", date };
    Entry* search = bsearch(&key, calendar, cdrlen(calendar), sizeof(Entry), datecmp);

    //if an entry with same date is found return null
    if (search != NULL) {
        printf("Current date already reserved.\n");
        return calendar;
    }
    calendar = realloc(calendar, (cdrlen(calendar) + 2) * sizeof(Entry));
    
    //find pointer to the last element of the reallocated array
    Entry* last = calendar;

    while (last->description[0] != 0) {
        last++;
    }
    //copy description and date to last element and set up a new "zero-element"
    last->date = date;

    memset(last->description, 0, 80);
    memset((last + 1)->description, 0, 80);
    
    strcpy(last->description, description);
    strcpy((last+1)->description, "\0");

    printf("Entry added\n");
    return calendar;
}

Entry* delete_entry(Entry* calendar, char* buffer) {
    Date date;

    //scan the string array given as parameter. Skip the first character
    int scan = sscanf(buffer,"%*c %d %d %d", &date.month, &date.day, &date.hour);

    //check if input scan was succesfull
    if (scan != 3) {
        printf("Wrong type of input format\n");
        return calendar;
    }

    //construct new entry struct with the given date
    //Used as a key in binary search.
    Entry key = { "key", date };
    Entry* search = bsearch(&key, calendar, cdrlen(calendar), sizeof(Entry), datecmp);
    
    //if such was date was not found, return the calendar unchanged.
    if (search == NULL) {
        printf("Couldn't find entry with given date and time.\n");
        return calendar;
    }
    
    //else move elements after the deleted ones one step back in the array
    while((search + 1)->description[0] != '\0') {
        Entry* next = (search + 1);
        char* nextDesc = next->description;

        memset(search->description, 0, 80);
        strcpy(search->description, nextDesc);
        search->date = next->date;
        search++;
    }
    memset(search->description, 0, 80);
    strcpy(search->description, "\0");

    //realloc the calendar
    calendar = realloc(calendar, (cdrlen(calendar) + 1) * sizeof(Entry));
    
    //return pointer to changed calendar
    printf("Entry deleted\n");
    return calendar;
}

void print_calendar(Entry* calendar, char* buffer) {
    unsigned int length = cdrlen(calendar);     //length of the calendar

    //if length != 0 qsort is used to sort the calendar by date and each element is printed
    if (length == 0) {
        printf("No entries in the calendar at this moment.\n");
    } else {
        qsort(calendar, length, sizeof(Entry), datecmp);

        while(calendar->description[0] != 0) {
            Date date = calendar->date;
            printf("%s %02d.%02d. klo %02d\n",
                calendar->description, date.day, date.month, date.hour);
            calendar++;
        }
    }
}

void save_calendar(Entry* calendar, char* buffer) {
    char filename[80];

    //scan the string array given as paramter. The first char is skipped.
    int scan = sscanf(buffer,"%*c %s", filename);

    //if scan is not successful return.
    if (scan != 1) {
        printf("Wrong type of input format\n");
        return;
    }

    //open new file. Its name is given in the input
	FILE* f = fopen(filename, "wb");

    //if opening fales, return.
    if (!f) {
        printf("Could not open the file.\n");
        return;
    }
    
    //write the calendar as binary to the opened file.
    while (calendar->description[0] != '\0') {
        fwrite(calendar, sizeof(Entry), 1, f);
        calendar++;
    }

    //if all went well, close the file and return.
    printf("Calendar saved.\n");
    fclose(f);
}

Entry* load_calendar(Entry* calendar, char* buffer) {
    char filename[80];

    //scan the string array given as a parameter. Skip the first character.
    int scan = sscanf(buffer,"%*c %s", filename);

    //if scan was unsuccsesfull, return NULL
    if (scan != 1) {
        printf("Wrong type of input format\n");
        return NULL;
    }

    //open the file whose name was given.
	FILE *f = fopen(filename, "rb");

    //if opening was unsuccesfull, return NULL;
    if (!f) {
        printf("Could not open the file.\n");
        return NULL;
    }    
    Entry* newCalendar = malloc(sizeof(Entry)); //allocate space for a new calendar
    unsigned int count = 0;

    //read the file which is assumed to be in binary and store the data into the anew callendar.
    while(!feof(f)) {
        fread(newCalendar + count, sizeof(Entry), 1, f);
        newCalendar = realloc(newCalendar, (count + 2) * sizeof(Entry));
        count++;
    }
    //set the "zero-element" to the end
    memset((newCalendar + count - 1)->description, 0, 80);
    strcpy((newCalendar + count - 1)->description, "\0");

    //close the file, free the memory allocated in the old calendar and return pointer
    //to the start of the new calendar.
    fclose(f);
    free(calendar);
    return newCalendar;
}   

int main(void) {
    Entry* calendar = malloc(sizeof(Entry));    //Dynamic array for the entries.

    //give the last (and so far only) element description "\0" which will be used
    //all around the program to determine if the end of the array has been reached.
    //In the comments I will refer to this element as the "zero-element".
    memset(calendar->description, 0, 80);
    strcpy(calendar->description, "\0");

    int running = 1;    //determines if the loop continues to run or not after each cycle
    
    //main loop. Asks user input.
    while (running) {
        char buffer[80];                    //initialize string array where the input is temporarily stored

        fgets(buffer, sizeof(buffer), stdin);              //store the input to buffer
        char firstLetter = buffer[0];       //the first letter tells what is to be done.

        switch(firstLetter) {
            case 'A':
                calendar = add_entry(calendar, buffer);          //add entry to the calendar
                break;
            case 'D':
                calendar = delete_entry(calendar, buffer);       //delete entry from the calendar
                break;
            case 'L':
                print_calendar(calendar, buffer);    //print the calendar
                break;
            case 'W':
                save_calendar(calendar, buffer);     //save the calendar to a file
                break;
            case 'O':
            {
                Entry* newCalendar = load_calendar(calendar, buffer); //loads the calendar from a file

                if (newCalendar != NULL) {
                    calendar = newCalendar;
                    printf("Calendar loaded\n");
                } else {
                    printf("Loading failed.\n");
                }
                break;
            }
            case 'Q':
                running = 0;            //frees the memory allocated and quits the program
                free(calendar);
                printf("Quitting program.\n");
                break;
            default:
                printf("Unknown command: %c\n", firstLetter);   //not recognised input
                break;
        }
    }
}