#include "servicethread.h"
#include <string>
#include <QDebug>

void ServiceThread::cleanUp() {
  delete service;
  service = nullptr;
  delete _config;
  _config = new Config();
}

ServiceThread::ServiceThread(QObject *parent) :
  QThread(parent),
  service(nullptr),
  _config(new Config()) {}

Config& ServiceThread::config() {
  return *_config;
}

ServiceThread::~ServiceThread() {
  stop();
  delete _config;
}

void ServiceThread::stop() {
  if (isRunning() && service) {
    service->stop();
    wait();
    cleanUp();
  }
}

void ServiceThread::run() {
  try {
    service = new Service(*_config);
    service->run();
  } catch (const std::exception &e) {
    cleanUp();
    emit startFailed();
  }
}
