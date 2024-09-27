// CMSC 341 - Fall 2023 - Project 3

#include "pqueue.h"
#include <math.h>
#include <algorithm>
#include <random>
#include <vector>
using namespace std;

const char * FAIL_STATEMENT = "*****TEST FAILED: ";
const char * PASS_STATEMENT = "*****TEST PASSED: ";

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

    bool NPLcheck(PQueue& pq){
        //traverse heap and make sure left child always has a larger npl than right child
        bool checker = true;
        if (pq.m_heap == nullptr){
            return false;
        }
        else{
            return rNPL(pq.m_heap, checker);
        }
    }

    bool rNPL(Node* n, bool c){
        if (n == nullptr){
            return c;
        }
        
        if (n->m_left == nullptr && n->m_right == nullptr){
            c = true;
        }
        else if (n->m_left != nullptr && n->m_right == nullptr){
            c = true;
        }
        else if(n->m_left == nullptr && n->m_right != nullptr){
            c = false;
        }
        else{
            if (n->m_left->getNPL() > n->m_right->getNPL()){
                c = true;
            }
            else{
                c = false;
            }
        }

        c = rNPL(n->m_left, c);
        if (c == false){
            return false;
        }
        c = rNPL(n->m_right, c);
        if (c == false){
            return false;
        }
        return c;
    }

    bool largerParent(PQueue& pq){
        //traverse heap and make sure children are smaller than parent
        bool checker = true;
        if (pq.m_heap == nullptr){
            return false;
        }
        else{
            return rLargerParent(pq.m_heap, checker, pq.m_priorFunc);
        }
    }

    bool rLargerParent(Node* r, bool c, prifn_t priFn){
        if (r == nullptr){
            return c;
        }
        
        if (r->m_left == nullptr && r->m_right == nullptr){
            c = true;
        }
        else if (r->m_left != nullptr && r->m_right == nullptr){
            if (priFn(r->m_patient) >= priFn(r->m_left->m_patient)){
                c = true;
            }
            else{
                c = false;
            }
        }
        else if(r->m_left == nullptr && r->m_right != nullptr){
            if (priFn(r->m_patient) >= priFn(r->m_right->m_patient)){
                c = true;
            }
            else{
                c = false;
            }
        }
        else{
            if ((priFn(r->m_patient) >= priFn(r->m_right->m_patient)) && (priFn(r->m_patient) >= priFn(r->m_left->m_patient))){
                c = true;
            }
            else{
                c = false;
            }
        }

        c = rLargerParent(r->m_left, c, priFn);
        if (c == false){
            return false;
        }
        c = rLargerParent(r->m_right, c, priFn);
        if (c == false){
            return false;
        }
        return c;
    }

    bool smallerParent(PQueue& pq){
        //traverse heap and make sure childrem are greater than parent
        bool checker = true;
        if (pq.m_heap == nullptr){
            return false;
        }
        else{
            return rSmallerParent(pq.m_heap, checker, pq.m_priorFunc);
        }
    }

    bool rSmallerParent(Node* r, bool c, prifn_t priFn){
        if (r == nullptr){
            return c;
        }
        
        if (r->m_left == nullptr && r->m_right == nullptr){
            c = true;
        }
        else if (r->m_left != nullptr && r->m_right == nullptr){
            if (priFn(r->m_patient) <= priFn(r->m_left->m_patient)){
                c = true;
            }
            else{
                c = false;
            }
        }
        else if(r->m_left == nullptr && r->m_right != nullptr){
            if (priFn(r->m_patient) <= priFn(r->m_right->m_patient)){
                c = true;
            }
            else{
                c = false;
            }
        }
        else{
            if ((priFn(r->m_patient) <= priFn(r->m_right->m_patient)) && (priFn(r->m_patient) <= priFn(r->m_left->m_patient))){
                c = true;
            }
            else{
                c = false;
            }
        }

        c = rSmallerParent(r->m_left, c, priFn);
        if (c == false){
            return false;
        }
        c = rSmallerParent(r->m_right, c, priFn);
        if (c == false){
            return false;
        }

        return c;
    }

    bool compareHeaps(PQueue& lhs, PQueue& rhs){
        //traverse both heaps and make sure both have nodes with the same information
        bool checker = true;
        if (lhs.m_size != rhs.m_size) {
            return false;
        }
        else{
            return recurseHeaps(lhs.m_heap, rhs.m_heap, checker);
        }
    }

    bool recurseHeaps(Node* l, Node* r, bool c){
        if (l == nullptr || r == nullptr){
            return c;
        }

        if (l->m_patient == r->m_patient){
            c = true;
        }
        else{
            c = false;
        }

        c = recurseHeaps(l->m_left, r->m_left, c);
        if (c == false){
            return false;
        }
        c = recurseHeaps(l->m_right, r->m_right, c);
        if (c == false){
            return false;
        }

        return c;
    }

    PQueue populatePQ(PQueue& pq, int size){
        Random nameGen(0,NUMNAMES-1);
        Random temperatureGen(MINTEMP,MAXTEMP);
        Random oxygenGen(MINOX,MAXOX);
        Random respiratoryGen(MINRR,MAXRR);
        Random bloodPressureGen(MINBP,MAXBP);
        Random nurseOpinionGen(MINOPINION,MAXOPINION);

        for (int i=0;i<size;i++){
            Patient patient(nameDB[nameGen.getRandNum()],
                        temperatureGen.getRandNum(),
                        oxygenGen.getRandNum(),
                        respiratoryGen.getRandNum(),
                        bloodPressureGen.getRandNum(),
                        nurseOpinionGen.getRandNum());
            pq.insertPatient(patient);
        }

        return pq;
    }

    Patient getPatient(PQueue& pq){
        return pq.m_heap->m_patient;
    }

};

int main(){
    Tester tester;

    Random nameGen(0,NUMNAMES-1);
    Random temperatureGen(MINTEMP,MAXTEMP);
    Random oxygenGen(MINOX,MAXOX);
    Random respiratoryGen(MINRR,MAXRR);
    Random bloodPressureGen(MINBP,MAXBP);
    Random nurseOpinionGen(MINOPINION,MAXOPINION);

    //////////////////////////INSERT TESTING//////////////////////////

    cout << endl << "INSERT TESTING" << endl;

    
    //Normal case testing 10 skew heap insertions
    {
        cout << "Normal case testing 10 skew heap insertions" << endl;

        PQueue aQueue(priorityFn1, MINHEAP, SKEW);
        for (int i=0;i<10;i++){
            Patient patient(nameDB[nameGen.getRandNum()],
                        temperatureGen.getRandNum(),
                        oxygenGen.getRandNum(),
                        respiratoryGen.getRandNum(),
                        bloodPressureGen.getRandNum(),
                        nurseOpinionGen.getRandNum());
            aQueue.insertPatient(patient);
        }

        if (tester.smallerParent(aQueue)){
            cout << PASS_STATEMENT << endl;
        }
        else{
            cout << FAIL_STATEMENT << endl;
        }
    }

    //Testing 100 skew heap inserts
    {
        cout << "Testing 100 skew heap inserts" << endl;
        PQueue aQueue(priorityFn1, MINHEAP, SKEW);
        for (int i=0;i<100;i++){
            Patient patient(nameDB[nameGen.getRandNum()],
                        temperatureGen.getRandNum(),
                        oxygenGen.getRandNum(),
                        respiratoryGen.getRandNum(),
                        bloodPressureGen.getRandNum(),
                        nurseOpinionGen.getRandNum());
            aQueue.insertPatient(patient);
        }

        if (tester.smallerParent(aQueue)){
            cout << PASS_STATEMENT << endl;
        }
        else{
            cout << FAIL_STATEMENT << endl;
        }

    }

    //testing 300 skew heap inserts
    {
        cout << "testing 300 skew heap inserts" << endl;
        PQueue aQueue(priorityFn1, MINHEAP, SKEW);
        for (int i=0;i<300;i++){
            Patient patient(nameDB[nameGen.getRandNum()],
                        temperatureGen.getRandNum(),
                        oxygenGen.getRandNum(),
                        respiratoryGen.getRandNum(),
                        bloodPressureGen.getRandNum(),
                        nurseOpinionGen.getRandNum());
            aQueue.insertPatient(patient);
        }

        if (tester.smallerParent(aQueue)){
            cout << PASS_STATEMENT << endl;
        }
        else{
            cout << FAIL_STATEMENT << endl;
        }
    }

    //Normal case testing 10 leftist heap insertions
    {
        cout << "Normal case testing 10 leftist heap insertions" << endl;
        PQueue aQueue(priorityFn1, MINHEAP, LEFTIST);
        for (int i=0;i<10;i++){
            Patient patient(nameDB[nameGen.getRandNum()],
                        temperatureGen.getRandNum(),
                        oxygenGen.getRandNum(),
                        respiratoryGen.getRandNum(),
                        bloodPressureGen.getRandNum(),
                        nurseOpinionGen.getRandNum());
            aQueue.insertPatient(patient);
        }

        if (tester.smallerParent(aQueue) && tester.NPLcheck(aQueue)){
            cout << PASS_STATEMENT << endl;
        }
        else{
            cout << FAIL_STATEMENT << endl;
        }
    }

    //Testing 100 leftist heap inserts
    {
        cout << "Testing 100 leftist heap inserts" << endl;
        PQueue aQueue(priorityFn1, MINHEAP, LEFTIST);
        for (int i=0;i<100;i++){
            Patient patient(nameDB[nameGen.getRandNum()],
                        temperatureGen.getRandNum(),
                        oxygenGen.getRandNum(),
                        respiratoryGen.getRandNum(),
                        bloodPressureGen.getRandNum(),
                        nurseOpinionGen.getRandNum());
            aQueue.insertPatient(patient);
        }

        if (tester.smallerParent(aQueue) && tester.NPLcheck(aQueue)){
            cout << PASS_STATEMENT << endl;
        }
        else{
            cout << FAIL_STATEMENT << endl;
        }
    }

    //testing 300 leftist heap inserts
    {
        cout << "testing 300 leftist heap inserts" << endl;
        PQueue aQueue(priorityFn1, MINHEAP, LEFTIST);
        for (int i=0;i<300;i++){
            Patient patient(nameDB[nameGen.getRandNum()],
                        temperatureGen.getRandNum(),
                        oxygenGen.getRandNum(),
                        respiratoryGen.getRandNum(),
                        bloodPressureGen.getRandNum(),
                        nurseOpinionGen.getRandNum());
            aQueue.insertPatient(patient);
        }

        if (tester.smallerParent(aQueue) && tester.NPLcheck(aQueue)){
            cout << PASS_STATEMENT << endl;
        }
        else{
            cout << FAIL_STATEMENT << endl;
        }
    }

    //testing nodes with increasing priority on skew heap min
    {
        cout << "testing nodes with increasing priority on skew heap min" << endl;
        PQueue aQueue(priorityFn1, MINHEAP, SKEW);
        for (int i=0;i<7;i++){
            Patient patient(nameDB[nameGen.getRandNum()],
                        MINTEMP + i,
                        MINOX + i,
                        MINRR + i,
                        MINBP + i,
                        MAXOPINION - i);
            aQueue.insertPatient(patient);
        }

        if (tester.smallerParent(aQueue)){
            cout << PASS_STATEMENT << endl;
        }
        else{
            cout << FAIL_STATEMENT << endl;
        }
    }

    //testing nodes with decreasing priority on skew heap min
    {
        cout << "testing nodes with decreasing priority on skew heap min" << endl;
        PQueue aQueue(priorityFn1, MINHEAP, SKEW);
        for (int i=0;i<7;i++){
            Patient patient(nameDB[nameGen.getRandNum()],
                        MAXTEMP - i,
                        MAXOX - i,
                        MAXRR - i,
                        MAXBP - i,
                        MINOPINION + i);
            aQueue.insertPatient(patient);
        }

        if (tester.smallerParent(aQueue)){
            cout << PASS_STATEMENT << endl;
        }
        else{
            cout << FAIL_STATEMENT << endl;
        }
    }

    //testing nodes with increasing priority on leftist heap min
    {
        cout << "testing nodes with increasing priority on leftist heap min" << endl;
        PQueue aQueue(priorityFn1, MINHEAP, LEFTIST);
        for (int i=0;i<7;i++){
            Patient patient(nameDB[nameGen.getRandNum()],
                        MINTEMP + i,
                        MINOX + i,
                        MINRR + i,
                        MINBP + i,
                        MAXOPINION - i);
            aQueue.insertPatient(patient);
        }

        if (tester.smallerParent(aQueue) && tester.NPLcheck(aQueue)){
            cout << PASS_STATEMENT << endl;
        }
        else{
            cout << FAIL_STATEMENT << endl;
        }
    }

    //testing nodes with decreasing priority on leftist heap min
    {
        cout << "testing nodes with decreasing priority on leftist heap min" << endl;
        PQueue aQueue(priorityFn1, MINHEAP, LEFTIST);
        for (int i=0;i<7;i++){
            Patient patient(nameDB[nameGen.getRandNum()],
                        MAXTEMP - i,
                        MAXOX - i,
                        MAXRR - i,
                        MAXBP - i,
                        MINOPINION + i);
            aQueue.insertPatient(patient);
        }

        if (tester.smallerParent(aQueue) && tester.NPLcheck(aQueue)){
            cout << PASS_STATEMENT << endl;
        }
        else{
            cout << FAIL_STATEMENT << endl;
        }
    }

    //testing nodes with increasing priority on skew heap max
    {
        cout << "testing nodes with increasing priority on skew heap max" << endl;
        PQueue aQueue(priorityFn1, MAXHEAP, SKEW);
        for (int i=0;i<7;i++){
            Patient patient(nameDB[nameGen.getRandNum()],
                        MINTEMP + i,
                        MINOX + i,
                        MINRR + i,
                        MINBP + i,
                        MAXOPINION - i);
            aQueue.insertPatient(patient);
        }

        if (tester.largerParent(aQueue)){
            cout << PASS_STATEMENT << endl;
        }
        else{
            cout << FAIL_STATEMENT << endl;
        }
    }

    //testing nodes with decreasing priority on skew heap max
    {
        cout << "testing nodes with decreasing priority on skew heap max" << endl;
        PQueue aQueue(priorityFn1, MAXHEAP, SKEW);
        for (int i=0;i<7;i++){
            Patient patient(nameDB[nameGen.getRandNum()],
                        MAXTEMP - i,
                        MAXOX - i,
                        MAXRR - i,
                        MAXBP - i,
                        MINOPINION + i);
            aQueue.insertPatient(patient);
        }

        if (tester.largerParent(aQueue)){
            cout << PASS_STATEMENT << endl;
        }
        else{
            cout << FAIL_STATEMENT << endl;
        }
    }

    //testing nodes with increasing priority on leftist heap max
    {
        cout << "testing nodes with increasing priority on leftist heap max" << endl;
        PQueue aQueue(priorityFn1, MAXHEAP, LEFTIST);
        for (int i=0;i<7;i++){
            Patient patient(nameDB[nameGen.getRandNum()],
                        MINTEMP + i,
                        MINOX + i,
                        MINRR + i,
                        MINBP + i,
                        MAXOPINION - i);
            aQueue.insertPatient(patient);
        }

        if (tester.largerParent(aQueue) && tester.NPLcheck(aQueue)){
            cout << PASS_STATEMENT << endl;
        }
        else{
            cout << FAIL_STATEMENT << endl;
        }
    }

    //testing nodes with decreasing priority on leftist heap max
    {
        cout << "testing nodes with decreasing priority on leftist heap max" << endl;
        PQueue aQueue(priorityFn1, MAXHEAP, LEFTIST);
        for (int i=0;i<7;i++){
            Patient patient(nameDB[nameGen.getRandNum()],
                        MAXTEMP - i,
                        MAXOX - i,
                        MAXRR - i,
                        MAXBP - i,
                        MINOPINION + i);
            aQueue.insertPatient(patient);
        }

        if (tester.largerParent(aQueue) && tester.NPLcheck(aQueue)){
            cout << PASS_STATEMENT << endl;
        }
        else{
            cout << FAIL_STATEMENT << endl;
        }
    }


    //////////////////////////COPY CONSTRUCTOR TESTING TESTING//////////////////////////
    
    cout << endl << "COPY CONSTRUCTOR TESTING TESTING" << endl;


    //Normal case copy skew heap with 10 nodes
    {
        cout << "Normal case copy skew heap with 10 nodes" << endl;
        PQueue aQ(priorityFn1, MAXHEAP, SKEW);
        tester.populatePQ(aQ, 10);
        PQueue bQ(aQ);
        if (tester.compareHeaps(aQ, bQ)){
            cout << PASS_STATEMENT << endl;
        }
        else{
            cout << FAIL_STATEMENT << endl; 
        }
    }

    //Testing copy skew heap with 100 nodes
    {
        cout << "Testing copy skew heap with 100 nodes" << endl;
        PQueue aQ(priorityFn1, MAXHEAP, SKEW);
        tester.populatePQ(aQ, 100);
        PQueue bQ(aQ);
        if (tester.compareHeaps(aQ, bQ)){
            cout << PASS_STATEMENT << endl;
        }
        else{
            cout << FAIL_STATEMENT << endl; 
        }
    }

    //testing copy skew heap with 300 nodes
    {
        cout << "testing copy skew heap with 300 nodes" << endl;
        PQueue aQ(priorityFn1, MAXHEAP, SKEW);
        tester.populatePQ(aQ, 300);
        PQueue bQ(aQ);
        if (tester.compareHeaps(aQ, bQ)){
            cout << PASS_STATEMENT << endl;
        }
        else{
            cout << FAIL_STATEMENT << endl; 
        }
    }

    //Normal case copy leftist heap with 10 nodes
    {
        cout << "Normal case copy leftist heap with 10 nodes" << endl;
        PQueue aQ(priorityFn1, MAXHEAP, LEFTIST);
        tester.populatePQ(aQ, 10);
        PQueue bQ(aQ);
        if (tester.compareHeaps(aQ, bQ)){
            cout << PASS_STATEMENT << endl;
        }
        else{
            cout << FAIL_STATEMENT << endl; 
        }
    }

    //Testing copy leftist heap with 100 nodes
    {
        cout << "Testing copy leftist heap with 100 nodes" << endl;
        PQueue aQ(priorityFn1, MAXHEAP, LEFTIST);
        tester.populatePQ(aQ, 100);
        PQueue bQ(aQ);
        if (tester.compareHeaps(aQ, bQ)){
            cout << PASS_STATEMENT << endl;
        }
        else{
            cout << FAIL_STATEMENT << endl; 
        }
    }

    //testing copy leftist heap with 300 nodes
    {
        cout << "testing copy leftist heap with 300 nodes" << endl;
        PQueue aQ(priorityFn1, MAXHEAP, LEFTIST);
        tester.populatePQ(aQ, 300);
        PQueue bQ(aQ);
        if (tester.compareHeaps(aQ, bQ)){
            cout << PASS_STATEMENT << endl;
        }
        else{
            cout << FAIL_STATEMENT << endl; 
        }
    }   


    //////////////////////////ASSIGNMENT OPERATOR TESTING//////////////////////////
    
    cout << endl << "ASSIGNMENT OPERATOR TESTING" << endl;

    //Normal case copy skew heap with 10 nodes
    {
        cout << "Normal case copy skew heap with 10 nodes" << endl;
        PQueue aQ(priorityFn1, MAXHEAP, SKEW);
        tester.populatePQ(aQ, 10);
        PQueue bQ(priorityFn1, MAXHEAP, SKEW);
        tester.populatePQ(bQ, 10);

        bQ = aQ;

        if (tester.compareHeaps(aQ, bQ)){
            cout << PASS_STATEMENT << endl;
        }
        else{
            cout << FAIL_STATEMENT << endl; 
        }

    }

    //Testing copy skew heap with 100 nodes
    {
        cout << "Testing copy skew heap with 100 nodes" << endl;
        PQueue aQ(priorityFn1, MAXHEAP, SKEW);
        tester.populatePQ(aQ, 100);
        PQueue bQ(priorityFn1, MAXHEAP, SKEW);
        tester.populatePQ(bQ, 100);

        bQ = aQ;

        if (tester.compareHeaps(aQ, bQ)){
            cout << PASS_STATEMENT << endl;
        }
        else{
            cout << FAIL_STATEMENT << endl; 
        }
    }

    //testing copy skew heap with 300 nodes
    {
        cout << "testing copy skew heap with 300 nodes" << endl;
        PQueue aQ(priorityFn1, MAXHEAP, SKEW);
        tester.populatePQ(aQ, 300);
        PQueue bQ(priorityFn1, MAXHEAP, SKEW);
        tester.populatePQ(bQ, 300);

        bQ = aQ;

        if (tester.compareHeaps(aQ, bQ)){
            cout << PASS_STATEMENT << endl;
        }
        else{
            cout << FAIL_STATEMENT << endl; 
        }
    }

    //Normal case copy leftist heap with 10 nodes
    {
        cout << "Normal case copy leftist heap with 10 nodes" << endl;
        PQueue aQ(priorityFn1, MAXHEAP, LEFTIST);
        tester.populatePQ(aQ, 10);
        PQueue bQ(priorityFn1, MAXHEAP, LEFTIST);
        tester.populatePQ(bQ, 70);

        bQ = aQ;

        if (tester.compareHeaps(aQ, bQ)){
            cout << PASS_STATEMENT << endl;
        }
        else{
            cout << FAIL_STATEMENT << endl; 
        }
    }

    //Testing copy leftist heap with 100 nodes
    {
        cout << "Testing copy leftist heap with 100 nodes" << endl;
        PQueue aQ(priorityFn1, MAXHEAP, LEFTIST);
        tester.populatePQ(aQ, 100);
        PQueue bQ(priorityFn1, MAXHEAP, LEFTIST);
        tester.populatePQ(bQ, 10);

        bQ = aQ;

        if (tester.compareHeaps(aQ, bQ)){
            cout << PASS_STATEMENT << endl;
        }
        else{
            cout << FAIL_STATEMENT << endl; 
        }
    }

    //testing copy leftist heap with 300 nodes
    {
        cout << "testing copy leftist heap with 300 nodes" << endl;
        PQueue aQ(priorityFn1, MAXHEAP, LEFTIST);
        tester.populatePQ(aQ, 300);
        PQueue bQ(priorityFn1, MAXHEAP, LEFTIST);
        tester.populatePQ(bQ, 1);

        bQ = aQ;

        if (tester.compareHeaps(aQ, bQ)){
            cout << PASS_STATEMENT << endl;
        }
        else{
            cout << FAIL_STATEMENT << endl; 
        }
    }

    //testing minheap skew = maxheap skew
    {
        cout << "testing minheap skew = maxheap skew" << endl;
        PQueue aQ(priorityFn1, MINHEAP, SKEW);
        tester.populatePQ(aQ, 20);
        PQueue bQ(priorityFn1, MAXHEAP, SKEW);
        tester.populatePQ(bQ, 20);

        aQ = bQ;

        if (tester.compareHeaps(aQ, bQ)){
            cout << PASS_STATEMENT << endl;
        }
        else{
            cout << FAIL_STATEMENT << endl; 
        }
    }

    //testing skewheap max = leftist max heap
    {
        cout << "testing skewheap max = leftist max heap" << endl;
        PQueue aQ(priorityFn1, MAXHEAP, SKEW);
        tester.populatePQ(aQ, 20);
        PQueue bQ(priorityFn1, MAXHEAP, LEFTIST);
        tester.populatePQ(bQ, 20);

        bQ = aQ;

        if (tester.compareHeaps(aQ, bQ)){
            cout << PASS_STATEMENT << endl;
        }
        else{
            cout << FAIL_STATEMENT << endl; 
        }
    }

    //empty = normal
    {
        cout << "Testing empty = normal" << endl;
        PQueue aQ(priorityFn1, MAXHEAP, LEFTIST);
        tester.populatePQ(aQ, 20);
        PQueue bQ(priorityFn1, MAXHEAP, LEFTIST);

        //bQ is empty
        bQ = aQ;

        if (tester.compareHeaps(aQ, bQ)){
            cout << PASS_STATEMENT << endl;
        }
        else{
            cout << FAIL_STATEMENT << endl; 
        }
    }

    //normal = empty
    {
        cout << "Testing normal = empty" << endl;
        PQueue aQ(priorityFn1, MAXHEAP, LEFTIST);
        tester.populatePQ(aQ, 20);
        PQueue bQ(priorityFn1, MAXHEAP, LEFTIST);

        //aQ is empty
        aQ = bQ;

        if (tester.compareHeaps(aQ, bQ)){
            cout << PASS_STATEMENT << endl;
        }
        else{
            cout << FAIL_STATEMENT << endl; 
        }
    }

    //empty = empty
    {
        cout << "empty = empty" << endl;
        PQueue aQ(priorityFn1, MAXHEAP, LEFTIST);
        PQueue bQ(priorityFn1, MAXHEAP, LEFTIST);

        aQ = bQ;

        if (tester.compareHeaps(aQ, bQ)){
            cout << PASS_STATEMENT << endl;
        }
        else{
            cout << FAIL_STATEMENT << endl; 
        }
    }



    //////////////////////////getNEXTPatient TESTING//////////////////////////


    cout << endl << "getNEXTPatient TESTING" << endl;
    

    //////////////////////////////////////////////////////////////CHECK THE ONE TIME CALLS THEY DONT WORK

    // calling 1 time skew
    {
        cout << "calling 1 time skew" << endl;
        PQueue aQ(priorityFn1, MAXHEAP, SKEW);
        tester.populatePQ(aQ, 20);

        if (tester.largerParent(aQ)){
            cout << PASS_STATEMENT << endl;
        }
        else{
            cout << FAIL_STATEMENT << endl;
        }
    }

    //calling on whole heap skew
    {
        cout << "calling on whole heap skew" << endl;
        bool test = true;
        PQueue aQ(priorityFn1, MAXHEAP, SKEW);
        tester.populatePQ(aQ, 20);

        for (int i = 0; i < 20; i++){
            if (tester.getPatient(aQ) == aQ.getNextPatient() && tester.largerParent(aQ)){
                test = true;
            }
            else{
                test = false;
            }
        }

        if (test == true){
            cout << PASS_STATEMENT << endl;
        }
        else{
            cout << FAIL_STATEMENT << endl;
        }
        
    }

    // calling 1 time Leftist
    {
        cout << "calling 1 time Leftist" << endl;
        PQueue aQ(priorityFn1, MINHEAP, LEFTIST);
        tester.populatePQ(aQ, 20);

        
        if (tester.smallerParent(aQ) && tester.NPLcheck(aQ)){
            cout << PASS_STATEMENT << endl;
        }
        else{
            cout << FAIL_STATEMENT << endl;
        }
    }

    //calling on whole heap leftist
    {
        cout << "calling on whole heap leftist" << endl;
        bool test = true;
        PQueue aQ(priorityFn1, MINHEAP, LEFTIST);
        tester.populatePQ(aQ, 20);

        for (int i = 0; i < 20; i++){
            if (tester.getPatient(aQ) == aQ.getNextPatient() && tester.smallerParent(aQ) && tester.NPLcheck(aQ)){
                test = true;
            }
            else{
                test = false;
            }
        }

        if (test == true){
            cout << PASS_STATEMENT << endl;
        }
        else{
            cout << FAIL_STATEMENT << endl;
        }
    }

    //calling on empty obj
    //should throw an out_of_range exception
    {
        cout << "calling on empty obj. Should throw an out_of_range exception" << endl;
        PQueue aQ(priorityFn1, MINHEAP, LEFTIST);
        try{
            aQ.getNextPatient();
        }
        catch(exception &e){
            cout << PASS_STATEMENT << endl;
        }
    }


    //////////////////////////MERGE TESTING//////////////////////////
    
    cout << endl << "MERGE TESTING" << endl;

    //test on single node
    {
        cout << "test on single node" << endl;
        PQueue aQ(priorityFn1, MINHEAP, LEFTIST);
        tester.populatePQ(aQ, 10);
        PQueue bQ(priorityFn1, MINHEAP, LEFTIST);
        Patient patient(nameDB[nameGen.getRandNum()],
                        MINTEMP + 1,
                        MINOX + 1,
                        MINRR + 1,
                        MINBP + 1,
                        MAXOPINION - 1);
            bQ.insertPatient(patient);

        aQ.mergeWithQueue(bQ);

        if (tester.smallerParent(aQ) && tester.NPLcheck(aQ)){
            cout << PASS_STATEMENT << endl;
        }
        else{
            cout << FAIL_STATEMENT << endl;
        }

        
    }

    //testing two full heaps merging
    {
        cout << "testing two full heaps merging" << endl;
        PQueue aQ(priorityFn1, MINHEAP, LEFTIST);
        tester.populatePQ(aQ, 10);
        PQueue bQ(priorityFn1, MINHEAP, LEFTIST);
        tester.populatePQ(bQ, 15);

        aQ.mergeWithQueue(bQ);

        if (tester.smallerParent(aQ) && tester.NPLcheck(aQ)){
            cout << PASS_STATEMENT << endl;
        }
        else{
            cout << FAIL_STATEMENT << endl;
        }
    }

    //testing skew min
    {
        cout << "testing skew min" << endl;
        PQueue aQ(priorityFn1, MINHEAP, SKEW);
        tester.populatePQ(aQ, 10);
        PQueue bQ(priorityFn1, MINHEAP, SKEW);
        tester.populatePQ(bQ, 15);

        aQ.mergeWithQueue(bQ);

        if (tester.smallerParent(aQ)){
            cout << PASS_STATEMENT << endl;
        }
        else{
            cout << FAIL_STATEMENT << endl;
        }
    }

    //testing skew max
    {
        cout << "testing skew max" << endl;
        PQueue aQ(priorityFn1, MAXHEAP, SKEW);
        tester.populatePQ(aQ, 10);
        PQueue bQ(priorityFn1, MAXHEAP, SKEW);
        tester.populatePQ(bQ, 15);

        aQ.mergeWithQueue(bQ);

        if (tester.largerParent(aQ)){
            cout << PASS_STATEMENT << endl;
        }
        else{
            cout << FAIL_STATEMENT << endl;
        }
    }
    

    //testing leftist max
    {
        cout << "testing leftist max" << endl;
        PQueue aQ(priorityFn1, MAXHEAP, LEFTIST);
        tester.populatePQ(aQ, 10);
        PQueue bQ(priorityFn1, MAXHEAP, LEFTIST);
        tester.populatePQ(bQ, 15);

        aQ.mergeWithQueue(bQ);

        if (tester.largerParent(aQ) && tester.NPLcheck(aQ)){
            cout << PASS_STATEMENT << endl;
        }
        else{
            cout << FAIL_STATEMENT << endl;
        }
    }

    //empty object merge with full heap
    {
        cout << "empty object merge with full heap" << endl;
        PQueue aQ(priorityFn1, MAXHEAP, LEFTIST);
        PQueue bQ(priorityFn1, MAXHEAP, LEFTIST);
        tester.populatePQ(bQ, 15);

        aQ.mergeWithQueue(bQ);

        if (tester.largerParent(aQ) && tester.NPLcheck(aQ)){
            cout << PASS_STATEMENT << endl;
        }
        else{
            cout << FAIL_STATEMENT << endl;
        }
    }

    //full heap with empty merge
    {
        cout << "full heap with empty merge" << endl;
        PQueue aQ(priorityFn1, MAXHEAP, LEFTIST);
        tester.populatePQ(aQ, 15);
        PQueue bQ(priorityFn1, MAXHEAP, LEFTIST);

        aQ.mergeWithQueue(bQ);

        if (tester.largerParent(aQ) && tester.NPLcheck(aQ)){
            cout << PASS_STATEMENT << endl;
        }
        else{
            cout << FAIL_STATEMENT << endl;
        }
    }

    //different sturcture
    {
        cout << "different sturcture" << endl;
        PQueue aQ(priorityFn1, MAXHEAP, SKEW);
        tester.populatePQ(aQ, 15);
        PQueue bQ(priorityFn1, MAXHEAP, LEFTIST);
        tester.populatePQ(bQ, 15);

        try{
            aQ.mergeWithQueue(bQ);
        }
        catch(exception &e){
            cout << PASS_STATEMENT << endl;
        }
    }

    //different priority
    {
        cout << "different priority" << endl;
        PQueue aQ(priorityFn1, MAXHEAP, SKEW);
        tester.populatePQ(aQ, 15);
        PQueue bQ(priorityFn2, MAXHEAP, SKEW);
        tester.populatePQ(bQ, 15);

        try{
            aQ.mergeWithQueue(bQ);
        }
        catch(exception &e){
            cout << PASS_STATEMENT << endl;
        }
    }

    //differet heaptype
    {
        cout << "differet heaptype" << endl;
        PQueue aQ(priorityFn2, MINHEAP, SKEW);
        tester.populatePQ(aQ, 15);
        PQueue bQ(priorityFn2, MAXHEAP, SKEW);
        tester.populatePQ(bQ, 15);

        try{
            aQ.mergeWithQueue(bQ);
        }
        catch(exception &e){
            cout << PASS_STATEMENT << endl;
        }

    }

    //merge on self
    {
        cout << "merge on self" << endl;
        PQueue aQ(priorityFn1, MAXHEAP, SKEW);
        tester.populatePQ(aQ, 15);

        aQ.mergeWithQueue(aQ);

        if (tester.largerParent(aQ)){
            cout << PASS_STATEMENT << endl;
        }
        else{
            cout << FAIL_STATEMENT << endl;
        }
    }


    //////////////////////////SET PRIORITY TESTING//////////////////////////
    
    cout << endl << "SET PRIORITY TESTING" << endl;

    //set priortiy 1 to priority 2
    {
        cout << "set priortiy 1 to priority 2" << endl;
        PQueue aQ(priorityFn1, MAXHEAP, LEFTIST);
        tester.populatePQ(aQ, 15);
        aQ.setPriorityFn(priorityFn2, MAXHEAP);

        if (tester.largerParent(aQ) && tester.NPLcheck(aQ)){
            cout << PASS_STATEMENT << endl;
        }
        else{
            cout << FAIL_STATEMENT << endl;
        }
    }

    //set priority 2 to priority 1
    {
        cout << "set priority 2 to priority 1" << endl;
        PQueue aQ(priorityFn2, MAXHEAP, LEFTIST);
        tester.populatePQ(aQ, 15);
        aQ.setPriorityFn(priorityFn1, MAXHEAP);

        if (tester.largerParent(aQ) && tester.NPLcheck(aQ)){
            cout << PASS_STATEMENT << endl;
        }
        else{
            cout << FAIL_STATEMENT << endl;
        }
    }

    //set priority 1 to priority 1
    {
        cout << "set priority 1 to priority 1" << endl;
        PQueue aQ(priorityFn1, MAXHEAP, LEFTIST);
        tester.populatePQ(aQ, 15);
        aQ.setPriorityFn(priorityFn1, MAXHEAP);

        if (tester.largerParent(aQ) && tester.NPLcheck(aQ)){
            cout << PASS_STATEMENT << endl;
        }
        else{
            cout << FAIL_STATEMENT << endl;
        }
    }

    //set max to min
    {
        cout << "set max to min" << endl;
        PQueue aQ(priorityFn1, MAXHEAP, LEFTIST);
        tester.populatePQ(aQ, 15);
        aQ.setPriorityFn(priorityFn1, MINHEAP);

        if (tester.smallerParent(aQ) && tester.NPLcheck(aQ)){
            cout << PASS_STATEMENT << endl;
        }
        else{
            cout << FAIL_STATEMENT << endl;
        }
    }

    //set min to max
    {
        cout << "set min to max" << endl;
        PQueue aQ(priorityFn1, MINHEAP, LEFTIST);
        tester.populatePQ(aQ, 15);
        aQ.setPriorityFn(priorityFn1, MAXHEAP);

        if (tester.largerParent(aQ) && tester.NPLcheck(aQ)){
            cout << PASS_STATEMENT << endl;
        }
        else{
            cout << FAIL_STATEMENT << endl;
        }
    }

    //set max to max
    {
        cout << "set max to max" << endl;

        PQueue aQ(priorityFn1, MAXHEAP, LEFTIST);
        tester.populatePQ(aQ, 15);
        aQ.setPriorityFn(priorityFn1, MAXHEAP);

        if (tester.largerParent(aQ) && tester.NPLcheck(aQ)){
            cout << PASS_STATEMENT << endl;
        }
        else{
            cout << FAIL_STATEMENT << endl;
        } 
    }
    

    //////////////////////////SET STRUCTURE TESTING//////////////////////////

    cout << endl << "SETSTRUCTURE TESTING" << endl;

    //set leftist to skew
    {
        cout << "set leftist to skew" << endl;
        PQueue aQ(priorityFn1, MINHEAP, LEFTIST);
        tester.populatePQ(aQ, 15);
        aQ.setStructure(SKEW);

        if (tester.smallerParent(aQ)){
            cout << PASS_STATEMENT << endl;
        }
        else{
            cout << FAIL_STATEMENT << endl;
        }
    }

    //set skew to leftist
    {
        cout << "set skew to leftist" << endl;
        PQueue aQ(priorityFn1, MINHEAP, SKEW);
        tester.populatePQ(aQ, 15);
        aQ.setStructure(LEFTIST);

        if (tester.smallerParent(aQ) && tester.NPLcheck(aQ)){
            cout << PASS_STATEMENT << endl;
        }
        else{
            cout << FAIL_STATEMENT << endl;
        }
    }

    //set skew to skew
    {
        cout << "set skew to skew" << endl;
        PQueue aQ(priorityFn1, MINHEAP, SKEW);
        tester.populatePQ(aQ, 15);
        aQ.setStructure(SKEW);

        if (tester.largerParent(aQ)){
            cout << PASS_STATEMENT << endl;
        }
        else{
            cout << FAIL_STATEMENT << endl;
        }
    }

    //set leftist to leftst
    {
        cout << "set leftist to leftst" << endl;
        PQueue aQ(priorityFn1, MINHEAP, LEFTIST);
        tester.populatePQ(aQ, 15);
        aQ.setStructure(LEFTIST);

        if (tester.largerParent(aQ) && tester.NPLcheck(aQ)){
            cout << PASS_STATEMENT << endl;
        }
        else{
            cout << FAIL_STATEMENT << endl;
        }
    }

    //large heap test 
    {
        cout << "large heap test" << endl;
        PQueue aQ(priorityFn1, MINHEAP, SKEW);
        tester.populatePQ(aQ, 300);
        aQ.setStructure(LEFTIST);

        if (tester.largerParent(aQ) && tester.NPLcheck(aQ)){
            cout << PASS_STATEMENT << endl;
        }
        else{
            cout << FAIL_STATEMENT << endl;
        }
    }

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
