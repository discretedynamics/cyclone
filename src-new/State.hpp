#ifndef __state_hpp_
#define __state_hpp_

class State
{
  int mNumStates;
  std::vector<int> mState;
public:
  State(int numstates, int numvars); // will create a zero State.
  
  int* getState() { return mState.data(); }
  void setFromIndex(long index);

  long getIndex() const;
};

#endif

// Local Variables:
// compile-command: "make "
// indent-tabs-mode: nil
// End:
