#ifndef TIMER_TIMER_H_
#define TIMER_TIMER_H_

void init_TimerA0(float frecuency);
void reset_TimerA0(void);
void stop_TimerA0(void);
void ISR_TimerA0(unsigned short int *state, unsigned short int *state_change);

void init_TimerA1(float frecuency);
void reset_TimerA1(void);
void stop_TimerA1(void);

#endif /* TIMER_TIMER_H_ */
