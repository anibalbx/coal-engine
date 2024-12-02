
#include <timer.h>
#include <iostream>
#include <chrono>

Timer::Timer()
{
  m_beg = clock_::now();
}

void Timer::reset()
{
  m_beg = clock_::now();

}

double Timer::elapsed() const
{
  return std::chrono::duration_cast<std::chrono::milliseconds>(
    clock_::now() - m_beg).count();
}
