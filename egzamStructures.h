#ifndef __egzamin_h__
#define __egzamin_h__

struct egzamHello {
    long int messageType; // 1 - Main, 2 - Thread1, 3 - Thread2
    int studentID;
    int threadID;
}

struct egzamQuestion {
    long int messageType; // To (studentID);
    int threadID; // From 
    int question; 
}

struct egzamAnswer {
    long int messageType; // To (threadID); 
    int studentID; // From
    int answer;
}

struct egzamGrade {
    long int messageType; // To (studentID); 
    int threadID; // From 
    float grade;
}

#endif //__egzamin_h__
