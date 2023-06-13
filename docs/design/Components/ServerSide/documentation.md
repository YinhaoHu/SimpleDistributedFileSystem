# SDFS Server Side Documentation

---

## Specifactions

* One process serves multiple threads.

* Concurrency is supported by simple thread pool(With 16 slots).

---

## Component

* Thread Pool
  
  * For each thead, if the client has not sent any message for a long time, we disconnect.
  
  * Fixed slots for simplicity.
