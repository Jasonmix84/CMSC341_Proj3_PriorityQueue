// CMSC 341 - Fall 2023 - Project 3

#include "pqueue.h"
#include <math.h>
#include <algorithm>
#include <random>
#include <vector>
using namespace std;

// Priority functions compute an integer priority for a patient.  Internal
// computations may be floating point, but must return an integer.

int priorityFn1(const Patient & patient);
int priorityFn2(const Patient & patient);

// a name database for testing purposes
const int NUMNAMES = 20;
string nameDB[NUMNAMES] = {
    "Ismail Carter", "Lorraine Peters", "Marco Shaffer", "Rebecca Moss",
    "Lachlan Solomon", "Grace Mclaughlin", "Tyrese Pruitt", "Aiza Green", 
    "Addie Greer", "Tatiana Buckley", "Tyler Dunn", "Aliyah Strong", 
    "Alastair Connolly", "Beatrix Acosta", "Camilla Mayo", "Fletcher Beck",
    "Erika Drake", "Libby Russo", "Liam Taylor", "Sofia Stewart"
};

// We can use the Random class to generate the test data randomly!
enum RANDOM {UNIFORMINT, UNIFORMREAL, NORMAL, SHUFFLE};
class Random {
public:
    Random(int min, int max, RANDOM type=UNIFORMINT, int mean=50, int stdev=20) : m_min(min), m_max(max), m_type(type)
    {
        if (type == NORMAL){
            //the case of NORMAL to generate integer numbers with normal distribution
            m_generator = std::mt19937(m_device());
            //the data set will have the mean of 50 (default) and standard deviation of 20 (default)
            //the mean and standard deviation can change by passing new values to constructor 
            m_normdist = std::normal_distribution<>(mean,stdev);
        }
        else if (type == UNIFORMINT) {
            //the case of UNIFORMINT to generate integer numbers
            // Using a fixed seed value generates always the same sequence
            // of pseudorandom numbers, e.g. reproducing scientific experiments
            // here it helps us with testing since the same sequence repeats
            m_generator = std::mt19937(10);// 10 is the fixed seed value
            m_unidist = std::uniform_int_distribution<>(min,max);
        }
        else if (type == UNIFORMREAL) { //the case of UNIFORMREAL to generate real numbers
            m_generator = std::mt19937(10);// 10 is the fixed seed value
            m_uniReal = std::uniform_real_distribution<double>((double)min,(double)max);
        }
        else { //the case of SHUFFLE to generate every number only once
            m_generator = std::mt19937(m_device());
        }
    }
    void setSeed(int seedNum){
        // we have set a default value for seed in constructor
        // we can change the seed by calling this function after constructor call
        // this gives us more randomness
        m_generator = std::mt19937(seedNum);
    }

    void getShuffle(vector<int> & array){
        // the user program creates the vector param and passes here
        // here we populate the vector using m_min and m_max
        for (int i = m_min; i<=m_max; i++){
            array.push_back(i);
        }
        shuffle(array.begin(),array.end(),m_generator);
    }

    void getShuffle(int array[]){
        // the param array must be of the size (m_max-m_min+1)
        // the user program creates the array and pass it here
        vector<int> temp;
        for (int i = m_min; i<=m_max; i++){
            temp.push_back(i);
        }
        std::shuffle(temp.begin(), temp.end(), m_generator);
        vector<int>::iterator it;
        int i = 0;
        for (it=temp.begin(); it != temp.end(); it++){
            array[i] = *it;
            i++;
        }
    }

    int getRandNum(){
        // this function returns integer numbers
        // the object must have been initialized to generate integers
        int result = 0;
        if(m_type == NORMAL){
            //returns a random number in a set with normal distribution
            //we limit random numbers by the min and max values
            result = m_min - 1;
            while(result < m_min || result > m_max)
                result = m_normdist(m_generator);
        }
        else if (m_type == UNIFORMINT){
            //this will generate a random number between min and max values
            result = m_unidist(m_generator);
        }
        return result;
    }

    double getRealRandNum(){
        // this function returns real numbers
        // the object must have been initialized to generate real numbers
        double result = m_uniReal(m_generator);
        // a trick to return numbers only with two deciaml points
        // for example if result is 15.0378, function returns 15.03
        // to round up we can use ceil function instead of floor
        result = std::floor(result*100.0)/100.0;
        return result;
    }
    
    private:
    int m_min;
    int m_max;
    RANDOM m_type;
    std::random_device m_device;
    std::mt19937 m_generator;
    std::normal_distribution<> m_normdist;//normal distribution
    std::uniform_int_distribution<> m_unidist;//integer uniform distribution
    std::uniform_real_distribution<double> m_uniReal;//real uniform distribution

};

class Tester{
    public:
};

int main(){
    Random nameGen(0,NUMNAMES-1);
    Random temperatureGen(MINTEMP,MAXTEMP);
    Random oxygenGen(MINOX,MAXOX);
    Random respiratoryGen(MINRR,MAXRR);
    Random bloodPressureGen(MINBP,MAXBP);
    Random nurseOpinionGen(MINOPINION,MAXOPINION);
    PQueue aQueue(priorityFn2, MINHEAP, LEFTIST);
    PQueue askewQueue(priorityFn2, MINHEAP, LEFTIST);
    for (int i=0;i<10;i++){
        Patient patient(nameDB[nameGen.getRandNum()],
                    temperatureGen.getRandNum(),
                    oxygenGen.getRandNum(),
                    respiratoryGen.getRandNum(),
                    bloodPressureGen.getRandNum(),
                    nurseOpinionGen.getRandNum());
        cout << patient << endl;
        aQueue.insertPatient(patient);
        askewQueue.insertPatient(patient);
    }
    cout << "\nDump of the leftist heap queue with priorityFn2 (MINHEAP):\n";
    aQueue.dump();

    cout << "\nDump of the SKEW heap queue with priorityFn2 (MINHEAP):\n";
    askewQueue.dump();


    aQueue.setStructure(SKEW);
    askewQueue.setStructure(LEFTIST);
    cout << "\nDump of the skew heap queue with priorityFn2 (MINHEAP):\n";
    aQueue.dump();
    cout << "\nPreorder traversal of the nodes in the queue with priorityFn2 (MINHEAP):\n";
    aQueue.printPatientQueue();
    aQueue.setPriorityFn(priorityFn1, MAXHEAP);
    cout << "\nDump of the skew queue with priorityFn1 (MAXHEAP):\n";
    aQueue.dump();

    PQueue aQueueCopy(aQueue);
    cout << "\nDump of the copy of aQUEUE with skew queue with priorityFn1 (MAXHEAP):\n";
    aQueueCopy.dump();

    PQueue cPQ(priorityFn2, MINHEAP, LEFTIST);
    for (int i=0;i<10;i++){
        Patient patient(nameDB[nameGen.getRandNum()],
                    temperatureGen.getRandNum(),
                    oxygenGen.getRandNum(),
                    respiratoryGen.getRandNum(),
                    bloodPressureGen.getRandNum(),
                    nurseOpinionGen.getRandNum());
        cPQ.insertPatient(patient);
    }
    cout << "\nDump of cOQ the skew queue with priorityFn1 (MAXHEAP):\n";
    cPQ.dump();

    cPQ = aQueue;

    cout << "\nDump of the cPQ the skew queue with priorityFn1 (MAXHEAP):\n";
    cPQ.dump();
    cout << endl;

    // for (int i =0; i < 10; i++){
    //     cout << "\nThe root of aQueue is " << aQueue.getNextPatient() << endl;
    //     cout << "dump after removind root for the " << i << "th time " <<  endl;
    //     aQueue.dump(); 
    //     cout << aQueue.numPatients() << endl;
    // }
    cout << endl;
    askewQueue.dump(); 
    for (int i =0; i < 10; i++){
        cout << "\nThe root of aQueue is " << askewQueue.getNextPatient() << endl;
        cout << "dump after removind root for the " << i << "th time " <<  endl;
        askewQueue.dump(); 
        cout << askewQueue.numPatients() << endl;
    }
    askewQueue.dump(); 

    return 0;
}

int priorityFn1(const Patient & patient) {
    //this function works with a MAXHEAP
    //priority value is determined based on some criteria
    //priority value falls in the range [115-242]
    //temperature + respiratory + blood pressure
    //the highest priority would be 42+40+160 = 242
    //the lowest priority would be 35+10+70 = 115
    //the larger value means the higher priority
    int priority = patient.getTemperature() + patient.getRR() + patient.getBP();
    return priority;
}

int priorityFn2(const Patient & patient) {
    //this function works with a MINHEAP
    //priority value is determined based on some criteria
    //priority value falls in the range [71-111]
    //nurse opinion + oxygen
    //the highest priority would be 1+70 = 71
    //the lowest priority would be 10+101 = 111
    //the smaller value means the higher priority
    int priority = patient.getOpinion() + patient.getOxygen();
    return priority;
}
