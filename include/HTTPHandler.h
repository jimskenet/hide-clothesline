#ifndef HTTP_HANDLER_H
#define HTTP_HANDLER_H

#include <NetworkSetup.h>

// HTTP handlers
void handleRoot();
void handleScan();
void handleConnect();
void handleStatus();
void handleReset();

// Server setup
void setupServer();

#endif