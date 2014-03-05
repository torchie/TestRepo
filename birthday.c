#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
/*
Kyle Reynolds | k2563809
COP3502C MWF 11:30 AM Marshall Tappen
Assignment 4: Closest Birthday
Wednesday March 20, 2013
 
OS X 10.8.3, BBEdit 10.5.2 + Terminal.app bash | CodeRunner 1.3.1
gcc i686-apple-darwin11-llvm-gcc-4.2
clang-421.11.65 x86_64

Lubuntu 12.10 amd64
gcc (Ubuntu/Linaro 4.7.2-2ubuntu1) 4.7.2
VirtualBox 4.2.10

makeshift logging preserved for posterity
unit tests preserved for posterity
 */
////////////////////////////////////////////////////////////////
//Classes
////////////////////////////////////////////////////////////////

//An individual student.
typedef struct birth_person {
	char firstname[29];
	char lastname[29];
	
	int day;
	char month[20];
	int year;
} person;

//A group of individuals.
typedef struct birth_class { 
	int number_of_students;
	int number_of_queries; 
	person* student;
	person* query;
	
} birth_class;

//A group of birth_classes.
typedef struct session { 
	int number_of_classes;
	birth_class* birthnumber;
} session;

////////////////////////////////////////////////////////////////
//Function Declarations
////////////////////////////////////////////////////////////////

int convert_month(char* month);
int date_delta(int day, int month, int year);
int days_in_month(int day, int month, int year);
session* read_file(char* filename);
int earlier_birthday(person* x, person* y);
void swapstudent(person* a, person* b);
person* closest_birthday(birth_class* group, person* x);
void sort_class(birth_class* x, int beginning, int end);
void printtest();
int read_test();

////////////////////////////////////////////////////////////////
//Definitions
////////////////////////////////////////////////////////////////


//Converts a string month to a numeric month.
//Preconditions: month is Uppercase, English, Gregorian.
int convert_month(char* month) {
	if(strcmp(month, "JANUARY") == 0)
		return 1;
		
	if(strcmp(month, "FEBRUARY") == 0)
		return 2;
		
	if(strcmp(month, "MARCH") == 0)
		return 3;
		
	if(strcmp(month, "APRIL") == 0)
		return 4;
		
	if(strcmp(month, "MAY") == 0)
		return 5;
		
	if(strcmp(month, "JUNE") == 0)
		return 6;
		
	if(strcmp(month, "JULY") == 0)
		return 7;
		
	if(strcmp(month, "AUGUST") == 0)
		return 8;
		
	if(strcmp(month, "SEPTEMBER") == 0)
		return 9;
		
	if(strcmp(month, "OCTOBER") == 0)
		return 10;
		
	if(strcmp(month, "NOVEMBER") == 0)
		return 11;
		
	if(strcmp(month, "DECEMBER") == 0)
		return 12;
	
	return 0;
}

//Returns the distance from January 1st of the year to the entered date.
//Preconditions: month, day, year are Gregorian.
int date_delta(int day, int month, int year) {
	int i;
	int final = 0;

	for(i = 1; i < month; i++) {
		final+=days_in_month(day, i, year);
	}
	
	final+=day;
	return final;
}

//Prints a list of all students and queries in a session.
void print_session(session* current) {
	session* all = current;
	int i, j;
	birth_class births;
	
	for(i = 0; i < all->number_of_classes; i++) {
		//Read students.
		births = all->birthnumber[i];
		for(j = 0; j < births.number_of_students; j++)
			printf("Student #%d. Name %s %s. Birthday %d %d %d\n", j+1, births.student[j].firstname, births.student[j].lastname, convert_month(births.student[j].month), births.student[j].day, births.student[j].year);
			
		for(j = 0; j < births.number_of_queries; j++)
			printf("Query #%d. Name %s %s.\n", j+1, births.query[j].firstname, births.query[j].lastname);
	}
}

//Reads a supplied file into a session.
session* read_file(char* filename) {
	session* temp = malloc(sizeof(session));
	FILE *ifp = fopen(filename, "r");
	
	fscanf(ifp, "%d", &temp->number_of_classes);
	int i, j;
	i = 0;
	temp->birthnumber = calloc(temp->number_of_classes, sizeof(birth_class)); 
	
	//printf("%d classes", temp->number_of_classes);
	for(i = 0; i < temp->number_of_classes; i++) {
		//Number of students in a class:
		birth_class* newbirth = malloc(sizeof(birth_class));
		fscanf(ifp, "%d", &newbirth->number_of_students);
		
		newbirth->student = calloc(newbirth->number_of_students, sizeof(person));
		//printf("\nclass %d number of students %d", i+1, newbirth->number_of_students);
		
		//Read students.
		for(j = 0; j < newbirth->number_of_students; j++) {
			fscanf(ifp, "%s", newbirth->student[j].firstname);
			//printf("%s", newbirth->student[j].firstname);
			
			fscanf(ifp, "%s", newbirth->student[j].lastname);
			//printf("%s", newbirth->student[j].lastname);
			
			fscanf(ifp, "%s", newbirth->student[j].month);
			fscanf(ifp, "%d", &newbirth->student[j].day);
			fscanf(ifp, "%d", &newbirth->student[j].year);
		}
		
		//Read queries.
		fscanf(ifp, "%d", &newbirth->number_of_queries);
		newbirth->query = calloc(newbirth->number_of_students, sizeof(person));
		
		for(j = 0; j < newbirth->number_of_queries; j++) {
			fscanf(ifp, "%s",  newbirth->query[j].firstname);
			fscanf(ifp, "%s", newbirth->query[j].lastname);
		}
		temp->birthnumber[i] = *newbirth;
	}
	
	fclose(ifp);
	return temp;	
}

//Convenience function for sort_class.
//Sorts by birthdate ignoring year, then last name, then firstname.
int earlier_birthday(person* x, person* y) {
	int result;
	int xdelta = date_delta(x->day, convert_month(x->month), x->year);
	int ydelta = date_delta(y->day, convert_month(y->month), y->year);
	
	if(xdelta < ydelta) 
		result = 1; 
	if(xdelta > ydelta)  
		result = 0; 

	//printf("test same sitch deltas: %s %s: %d, %s %s: %d\n" ,x->firstname, x->lastname, xdelta, y->firstname, y->lastname, ydelta);
	
	//Evaluate depending on last name — if same, use first name.
	if(xdelta == ydelta) {
		if(strcmp(x->lastname, y->lastname) != 0) {
			result = (strcmp(x->lastname, y->lastname) < 0);
		} else if(strcmp(x->firstname, y->firstname) != 0) {
			result = (strcmp(x->firstname, y->firstname) < 0);
		}
	}
	return result;
}

//Swaps the locations of two people.
//Convenience function for sorting.
void swapstudent(person* a, person* b) {
	person temp = *a;
	*a = *b;
	*b = temp;	
}

//Quicksort-based arrangement of persons in a class by birthday; uses earlier_birthday.
//This could be wrapped with a function that takes care of that for the user but ehh... version 2.
//sort_class' original argument should be called with the class' original size at first.
void sort_class(birth_class* x, int begin, int end) { 
	if(end > (begin + 1)) {
		person* pivot = &x->student[begin];
		int left = begin + 1;
		int right = end;
		while(left < right) {
			if(earlier_birthday(&x->student[left], pivot)) {
				left++;				
			}
			else {
				swapstudent(&x->student[left], &x->student[--right]);
			}
		}
		swapstudent(&x->student[--left], &x->student[begin]);
		sort_class(x, begin, left);
		sort_class(x, right, end);	
	}
}

//Finds the location in an array of the person in a class.
//Preconditions (program-wide): the application is array-based or the container allows numeric position.
int get_person_location(birth_class* group, person* x) {
	int i;
	for(i = 0; i < group->number_of_students; i++) {
		if((strcmp(x->firstname, group->student[i].firstname) == 0 ) && (strcmp(x->lastname, group->student[i].lastname) == 0))
			return i;
	}
	return 0;
}

/*
Thirty days hath September,
April, June, and November.
All the rest have thirty-one,
Excepting February alone,
And that has twenty-eight days clear,
And twenty-nine in each leap year. 

Returns 29 if the month is February and the year is divisible by four.*/
int days_in_month(int day, int month, int year) {
	if(month == 11 || month == 4 || month == 6 || month == 9)
		return 30;
	
	if(month == 1 || month == 3 || month == 5 || month == 7 || month == 8 || month == 10 || month == 12)
		return 31;
		
	if(month == 2) {
		if(year%4 == 0 && day == 29) {
			return 29;
		} else {
			return 28;
		}
	}
	return 0;
	
}

//Determines the closest birthday to the queried person in the group.
//Preconditions: person exists in group.
person* closest_birthday(birth_class* group, person* x) {
	//Array locations;
	int xindex = get_person_location(group, x);
	int left;
	int right;
	
	//From-Beginning-of-Year deltas:
	int left_age;
	int x_age;
	int right_age;
	left = xindex-1;
	right = xindex+1;

	if(left < 0) {
		//printf("gotta wrap left %d, xindex is %d\n", left, xindex);
		left = group->number_of_students-1;
	}
	
	if(right >= group->number_of_students) {
		//printf("gotta wrap right %d, group is %d people", right, group->number_of_students);
		right = 0;
	}
	
	//printf("indices: %d %d %d\n", left, xindex, right);
	left_age = date_delta(group->student[left].day, convert_month(group->student[left].month), group->student[left].year);
	right_age = date_delta(group->student[right].day, convert_month(group->student[right].month), group->student[right].year);
	x_age = date_delta(group->student[xindex].day, convert_month(group->student[xindex].month), group->student[xindex].year); 
	//printf("left age %d x age %d right age %d\n", left_age, x_age, right_age);
	
	//Deltas
	int left_delta = abs(x_age-left_age);
	int right_delta = abs(x_age-right_age);
	
	//Wraparound.
	if(abs(right_delta-365) < right_delta)
		right_delta = abs(right_delta-365);
	if(abs(left_delta-365) < left_delta)
		left_delta = abs(left_delta-365);
	
	//printf("ldelta %s %s %d rdelta %s %s %d\n\n",group->student[left].firstname, group->student[left].lastname, left_delta, group->student[right].firstname, group->student[right].lastname, right_delta);
	int result = left_delta < right_delta ? left : right;
	
	//Default to right in case of equidistance.
	if(left_delta == right_delta) 
		result = right;
	
	return &group->student[result];
}

//Prints a title for a unit test.
//Convenience function.
void printtest(char* name) {
    int i;
    printf("\n");
    
    for(i = 0; i < 36; i++)
        printf("=");

    printf("\n");
    
    /*for(i = 0; i < 12; i++)
        printf(" ");*/
    
    printf("%s\n", name);
    for(i = 0 ; i < 36; i++)
        printf("=");
    
    printf("\n\n");
}

//It all ends here, baby.
int read_test() { 
	//printtest("Read Birthday File into Session");
	session* all = read_file("birthday.txt");
	//print_session(all);
	int i;
	/*
	printtest("Sort");
	sort_class(&all->birthnumber[0],0,all->birthnumber[0].number_of_students);
	sort_class(&all->birthnumber[1],0,all->birthnumber[1].number_of_students);
	print_session(all);
	
	printtest("Get Person Location");
	printf("%d", get_person_location(&all->birthnumber[0], &all->birthnumber[0].query[1]));
	
	printtest("Days in Months");

	for(i = 1; i < 13; i++) {
		printf("%d, 1943: -> %d\n", i, days_in_month(i, 1943));
	}
	
	for(i = 1; i < 13; i++) {
		printf("%d, 1944: -> %d\n", i, days_in_month(i, 1944));
	}
	*/
	//int date_delta(int day, int month, int year)
	/*printtest("Date Deltas; number of days elapsed since January 1, 0 AD");
	
	printf("Year 0\n");
	printf("%d %d %d: %d\n", 1, 1, 0, date_delta(1, 1, 0));
	printf("%d %d %d: %d\n", 1, 2, 0, date_delta(1, 2, 0));
	printf("%d %d %d: %d\n", 1, 3, 0, date_delta(1, 3, 0));
	printf("%d %d %d: %d\n", 1, 4, 0, date_delta(1, 4, 0));
	printf("%d %d %d: %d\n", 1, 5, 0, date_delta(1, 5, 0));
	printf("%d %d %d: %d\n", 1, 6, 0, date_delta(1, 6, 0));
	printf("%d %d %d: %d\n", 1, 7, 0, date_delta(1, 7, 0));
	printf("%d %d %d: %d\n", 1, 8, 0, date_delta(1, 8, 0));
	printf("%d %d %d: %d\n", 1, 9, 0, date_delta(1, 9, 0));
	printf("%d %d %d: %d\n", 1, 10, 0, date_delta(1, 10, 0));
	printf("%d %d %d: %d\n", 1, 11, 0, date_delta(1, 11, 0));
	printf("%d %d %d: %d\n", 15, 12, 0, date_delta(15, 12, 0));
	printf("%d %d %d: %d\n", 16, 12, 0, date_delta(16, 12, 0));
	
	printf("%d %d %d: %d\n", 31, 12, 0, date_delta(31, 12, 0));
	printf("year 1\n");
	printf("%d %d %d: %d\n", 1, 1, 1, date_delta(1, 1, 1));
	printf("%d %d %d: %d\n", 1, 3, 1, date_delta(1, 3, 1));
	printf("other:\n");
	printf("%d %d %d: %d\n", 1, 1, 0, date_delta(1, 1, 0));
	printf("%d %d %d: %d\n", 1, 1, 1, date_delta(1, 1, 1));
	printf("%d %d %d: %d\n", 1, 1, 2, date_delta(1, 1, 2));
	printf("%d %d %d: %d\n", 1, 1, 3, date_delta(1, 1, 3));

	printtest("Closest Birthday");
	printf("Closest birthday to %s: %s", all->birthnumber[0].query[1].firstname, closest_birthday(&all->birthnumber[0], &all->birthnumber[0].query[1])->firstname);
	
	printtest("Fringe Tests");
	printf("swapping %s with %s: earlier birthday %d\n", all->birthnumber[1].student[2].lastname, all->birthnumber[1].student[3].lastname, earlier_birthday(&all->birthnumber[1].student[2], &all->birthnumber[1].student[3]));
	printf("swapping %s with %s: earlier birthday %d", all->birthnumber[1].student[0].lastname, all->birthnumber[1].student[1].lastname, earlier_birthday(&all->birthnumber[1].student[0], &all->birthnumber[1].student[1]));
*/	
	
	//printtest("Final Assignment");
	birth_class* births;
	int j;
	for(i = 0; i < all->number_of_classes; i++) {
		//Read students.
		births = &all->birthnumber[i];
		sort_class(births, 0, births->number_of_students);

		printf("Class #%d:\n\n", i+1);
		//print_session(all);
					
		for(j = 0; j < births->number_of_queries; j++) {
			printf("%s %s has the closest birthday to %s %s.\n", closest_birthday(&all->birthnumber[i], &all->birthnumber[i].query[j])->firstname, closest_birthday(&all->birthnumber[i], &all->birthnumber[i].query[j])->lastname, all->birthnumber[i].query[j].firstname, all->birthnumber[i].query[j].lastname);
		}
		
		printf("\n");
	}
	
	return 0;
}

int main(int argc, char *argv[]) {
	return read_test();
}
