#ifndef __state_hpp_
#define __state_hpp_

#include <ostream>

class State
{
  friend std::ostream& operator<<(std::ostream& o, const State& u);
  int mNumStates;
  std::vector<int> mState; // 0..numvars-1
public:
  State(int numstates, int numvars) // create a zero State.
    : mNumStates(numstates),
      mState(numvars, 0)
  {
    // require that numvars >= 1.
    // require that mNumStates >= 2.
  }

  State(int numstates, const std::vector<int>& state_vals)
    : mNumStates(numstates),
      mState(state_vals)
  {
  }

  int* getState() { return mState.data(); }

  void setFromState(const State& v)
  {
    if (mNumStates != v.mNumStates or mState.size() != v.mState.size())
      {
        throw "expected same number of states and variables";
      }

    for (int i=0; i<mState.size(); ++i) mState[i] = v.mState[i];
  }

  void setFromIndex(long index)
  {
    for (int i = mState.size() - 1; i >= 0; i--)
      {
        mState[i] = index % mNumStates;
        index /= mNumStates;
      }
  }
  // 3-ary (mNumStates = 3), numvars = 4. index can be in range 0..3^4
  // index = 17.  What state is this?
  // state[3] = 17%3 = 2.  Now set index = 17/3 = 5
  // state[2] = 5%3 = 2.  Now we set index = 5/3 = 1
  // state[1] = 1%3 = 1.  index = 1/3 = 0
  // state[0] = 0%3 = 0
  // index == state[3]*1 + state[2]*3 + state[1]*3^2 + state[0]*3^3
  // 17 in base 3 is: 0122 = 1*3^2 + 2*3 + 2*1

  long getIndex() const
  {
    long result = mState[0];
    for (int i=1; i < mState.size(); i++)
      result = (result * mNumStates) + mState[i];
    return result;
  }
};

inline std::ostream& operator<<(std::ostream& o, const State& u)
{
  for (long i=0; i < u.mState.size(); ++i)
    {
      if (i > 0) o << " ";
      o << u.mState[i];
    }
  return o;
}
#endif

// Local Variables:
// compile-command: "make "
// indent-tabs-mode: nil
// End:
