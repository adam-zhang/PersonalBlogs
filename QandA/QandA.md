# the questions and answers in interviews.

1. Q: why do destructors in derived classes must be virtual?
   A: because if you use a pointer of a base class and actually pointing to derived class, in this situation, if you delete the pointer, it just delete the base class but not derived class and memory leaks happend.


