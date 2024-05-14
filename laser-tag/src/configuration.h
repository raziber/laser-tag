#ifndef CONFIGURATION_H
#define CONFIGURATION_H

// comment out to diable
#define SERIAL_PRINT

// set the sensors' ports
#define PORTS 1, 4, 2, 5, 6

// Declare pointers to be used globally
extern int *ports;
extern int numPorts;

// functions in configuraion.cpp
void initConfiguration();

#endif
