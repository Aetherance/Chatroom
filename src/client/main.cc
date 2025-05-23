#include"client.h"
#include"sig.h"
#include<iostream>

int main() {
  SignalBlocker on;

  Client client;
  client.run();

  return 0;
}