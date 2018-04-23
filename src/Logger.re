let logCrash = (message: string, payload: string) =>
  LogStore.add({
    logType: "crash",
    level: 10,
    message,
    payload,
    on: Date.now(),
  });

let logError = (message: string, payload: string) =>
  LogStore.add({
    logType: "error",
    level: 5,
    message,
    payload,
    on: Date.now(),
  });

let logWarning = (message: string, payload: string) =>
  LogStore.add({
    logType: "warning",
    level: 3,
    message,
    payload,
    on: Date.now(),
  });

let logInfo = (message: string, payload: string) =>
  LogStore.add({logType: "info", level: 1, message, payload, on: Date.now()});