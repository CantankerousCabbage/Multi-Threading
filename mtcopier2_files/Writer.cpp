/**
 * startup code provided by Paul Miller for COSC1114 - Operating Systems
 * Principles
 **/
#include "Writer.h"
#include "Timer.h"
#include "Reader.h"

/**
 * Static variables for managing synchronisation
 **/
string Writer::outName;

int Writer::dequeueWait;
int Writer::pushWait;

bool Writer::finished;
bool Writer::first;
bool Writer::empty;

pthread_mutex_t Writer::queueLock;
pthread_mutex_t Writer::writeLock;
pthread_cond_t Writer::popCond;
pthread_cond_t Writer::writeCond;

shared_ptr<Timer> Writer::timer;
std::ofstream Writer::out;
std::deque<std::string> Writer::queue;

Writer::Writer() {
    tLog = make_shared<TimeLog>();
}

Writer::~Writer(){}

bool Writer::init(const std::string& name, shared_ptr<Timer> timer) {
    
    Writer::timer = timer;
    Writer::outName = name;
    out.open(name);
    bool fileCheck = out.good();
    if (fileCheck) {
        Writer::dequeueWait = INITIAL;
        Writer::finished = false;
        Writer::first = true;
        Writer::empty = false;

        pthread_mutex_init(&queueLock, NULL);
        pthread_mutex_init(&writeLock, NULL);
        pthread_cond_init(&popCond, NULL);
        pthread_cond_init(&writeCond, NULL);  
    }
    return fileCheck;
}

void Writer::run() {
    if(pthread_create(&writeThread, NULL, &runner, this)){
        std::cout << "Error: unable create write thread" << std::endl;
        exit(-1);
    }
}

void* Writer::runner(void* arg) { 
    
    Writer* writer = (Writer*) arg;
    writer->execute();
    // while (!finished) {
    //     writer->dequeue();
    //     writer->writeData();
    // } 
    return NULL;
}
void Writer::execute(){

    while (!finished || !empty) {
        this->tLog->startLockTimer();  
        pthread_mutex_lock(&queueLock);
        this->tLog->endLockTimer(tLog->lockOne);

         this->dequeue();

        this->tLog->startLockTimer();
        pthread_mutex_lock(&writeLock);
        this->tLog->endLockTimer(this->tLog->lockTwo);

        pthread_mutex_unlock(&queueLock);
        
        this->writeData();
        pthread_mutex_unlock(&writeLock);
    } 
}
void Writer::dequeue(){
    
    // this->tLog->startLockTimer();  
    // pthread_mutex_lock(&queueLock);
    // this->tLog->endLockTimer(tLog->lockOne);

    tLog->startWaitTimer();
    while(queue.size() == 0 && !finished){
        Writer::dequeueWait++;
        pthread_cond_wait(&popCond, &queueLock);
        Writer::dequeueWait--;
    } 
    tLog->endWaitTimer();
        if(queue.size()){
            this->writeLine = queue.front();
            queue.pop_front();
            // std::cout << "Pop: " << queue.size() << std::endl;
        } else if(finished) {
            pthread_cond_signal(&popCond);
            pthread_mutex_unlock(&queueLock);
            pthread_exit(NULL);
        }
        
     
    if(queue.size()){
        pthread_cond_signal(&popCond);   
    } else {
        
        pthread_cond_signal(&Reader::pushCond);        
    }

    // this->tLog->startLockTimer();
    // pthread_mutex_lock(&writeLock);
    // this->tLog->endLockTimer(this->tLog->lockTwo);

    // pthread_mutex_unlock(&queueLock);
}

void Writer::writeData(){
    
        if(!Writer::first){
            out << "\n";   
        } 
        if(Writer::first){
            Writer::first = false; 
        } 
        out << this->writeLine;
    // std::cout << "Pop-W: " << dequeueWait << " Push-W: " << pushWait << std::endl;
    // pthread_mutex_unlock(&writeLock);
}

void Writer::append(const std::string& line, Reader* reader) {
   
    reader->tLog->startLockTimer();
    pthread_mutex_lock(&queueLock);
    reader->tLog->endLockTimer(reader->tLog->lockTwo);
  
    reader->tLog->startWaitTimer();
    while(queue.size() == BUFFER && !finished){
        pushWait++;
        pthread_cond_wait(&Reader::pushCond, &queueLock);  
        pushWait--;      
    }  
    reader->tLog->endWaitTimer();

    if(reader->valid){
        queue.push_back(line);
    }
    // queue.push_back(line);
    
    
    // std::cout << "Push: " << queue.size() << std::endl;
    if(queue.size() == BUFFER)
    {   
        pthread_cond_signal(&popCond);
        // if(!finished){
        //     pthread_cond_signal(&popCond);
        // } else {
        //     pthread_cond_signal(&Reader::pushCond);
        // }
        
    } else {   
        if(finished && !pushWait){
            pthread_cond_signal(&popCond);
        } else {
            pthread_cond_signal(&Reader::pushCond); 
        }
        
    }

    pthread_mutex_unlock(&queueLock);
    pthread_mutex_unlock(&Reader::readLock);     
}

void Writer::cleanUp() {
    pthread_mutex_destroy(&queueLock);
    pthread_mutex_destroy(&writeLock);
    pthread_cond_destroy(&popCond);
    
}

bool Writer::reset(){
    out.open(outName);
    bool fileCheck = out.good();
    if(fileCheck) {
        Writer::dequeueWait = INITIAL;
        Writer::finished = false;
        Writer::first = true;
        Writer::empty = false;
    }
    std::cout << "filechck W: " << fileCheck << std::endl;
   return fileCheck;
}

void Writer::resetInstance() {
    this->writeLine = "";
    this->tLog->reset();
}

void Writer::setFinished() {
    finished = true;
}

pthread_t Writer::getThread() {
    return writeThread;
}


void Writer::close(){
    Writer::out.close();
}


