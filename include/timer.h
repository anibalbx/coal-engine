#ifndef __TIMER_H__
#define __TIMER_H__ 1
#include <chrono>


class Timer {
public:
  Timer();
  void reset();
  double elapsed() const;

private:
  typedef std::chrono::high_resolution_clock clock_;
  typedef std::chrono::duration<double, std::ratio<1> > second_;
  std::chrono::time_point<clock_> m_beg;
};

#endif
