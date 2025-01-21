#ifndef __egzamin_h__
#define __egzamin_h__

struct egzamHello {
    long int messageType; // 1 - Main, 2 - Thread1, 3 - Thread2
    int studentID;
    int codeForQuestion;
    unsigned long int threadID;
};

struct egzamQuestion {
    long int messageType; // To (studentID);
    unsigned long int threadID; // From 
    int codeForAnswer;                                
    int question; 
};

struct egzamAnswer {
    long int messageType; // To (threadID); 
    int studentID; // From
    int codeForGrade;
    int answer;
};

struct egzamGrade {
    long int messageType; // To (studentID); 
    unsigned long int threadID; // From 
    float grade;
};

#endif //__egzamin_h__
