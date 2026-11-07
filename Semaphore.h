#ifndef __SEMAPHORE_H
#define __SEMAPHORE_H

namespace semaphore{

typedef unsigned int semaphore_t;
  
/*
 * Semaphore - A simple semaphore class.
 * Author Jacob Psimos 2020
 */
class Semaphore{
  public:
    Semaphore()
      : sem(0), lastsem(0) {}
    semaphore_t take();
    bool give(const semaphore_t &semaphore);
    bool got(const semaphore_t &semaphore);
    bool available() const;
    void wait() const;
    void wait(const unsigned int &microsecs) const;
  private:
    volatile semaphore_t sem;
    volatile semaphore_t lastsem;
};

semaphore_t Semaphore::take(){
  semaphore_t semaphore = 0;
  if(available()){
    semaphore = (++sem == 0) ? ++sem : sem;
  }
  return semaphore;
}

bool Semaphore::give(const semaphore_t &semaphore){
  bool gave = false;
  if(semaphore == sem){
    lastsem = sem;
    gave = true;
  }
  return gave;
}

bool Semaphore::got(const semaphore_t &semaphore){
  bool same = semaphore == sem;
  return same;
}

bool Semaphore::available() const{
  return lastsem == sem;
}

void Semaphore::wait() const{
  while(!available()){}
}

void Semaphore::wait(const unsigned int &microsecs) const{
  unsigned int elapsed = 0;
  while(elapsed < microsecs && !available()){
    delayMicroseconds(100);
    elapsed += 100;
  }
}

} //end namespace semaphore
#endif
