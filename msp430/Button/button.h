#ifndef BUTTON_BUTTON_H_
#define BUTTON_BUTTON_H_

void init_Debouncer(void);
void ISR_Port2(unsigned short int *state, unsigned short int *state_change);
void init_Sensor(void);
void enable_OP_ISR(void);
void enable_CL_ISR(void);

#endif /* BUTTON_BUTTON_H_ */
