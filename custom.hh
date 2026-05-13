//Computer Architecture II | Project 2
//This is a project code from Team 30, Team associates: Shashank Shivakumar Muthkur, Amitava Ray, Anjali
//Team leader is Anjali with ASU ID: 1234157186 >>> print("The modulus result is {}".format(1234157186%18)) The modulus result is 2, Branch Address and Global History Bits are 8. Operation between them is AND.

#ifndef __CPU_PRED_CUSTOM_BP_HH__  //Header Guard
#define __CPU_PRED_CUSTOM_BP_HH__


#include <vector>

#include "cpu/pred/bpred_unit.hh"
#include "params/BranchPredictor.hh"
#include "params/CustomBP.hh"
#include "base/types.hh"
#include "base/sat_counter.hh"


class CustomBP : public BPredUnit {

    public:
        // Default constructor for the Branch Predictor

        CustomBP(const CustomBPParams &params);

        bool lookup(ThreadID tid, Addr branch_addr, void * &bp_history);
       /*
    @param branch_addr The address of the branch to look up.
    @param bp_history Pointer that will be set to the BPHistory object.
    @return True if the branch is predicted as taken, false otherwise.

       */


        void uncondBranch(ThreadID tid, Addr pc, void * &bp_history);

        void btbUpdate(ThreadID tid, Addr branch_addr, void * &bp_history);
        /*
    @param branch_addr The address of the branch being queried.
    @param bp_history Pointer to the current branch predictor history state.
    @return Returns true if the branch is predicted as taken, otherwise false.
        */


        void update(ThreadID tid, Addr branch_addr, bool taken, void *bp_history,
                bool squashed, const StaticInstPtr & inst, Addr corrTarget);

        void squash(ThreadID tid, void *bp_history);
    
       
    private:

    unsigned PHT_Counter_BITS;
    unsigned Pred_Size;
    unsigned globalHistoryMask;
    

    unsigned Calculate_Local_History_Index(Addr &branch_addr);

    void Update_Global_History_Taken(ThreadID tid);
    void Update_Global_History_Not_Taken(ThreadID tid);
    /** Updates global history to reflect a taken branch. */


    struct BPHistory {
        unsigned globalHistory;
    };
    


/** Indicator for an invalid predictor index */

    static const int invalidPredictorIndex = -1;

    /** Global history register. Contains as much history as specified by
     *  globalHistoryBits. Actual number of bits used is determined by
     *  globalHistoryMask and choiceHistoryMask. */
    std::vector<unsigned> globalHistory;

/** Bit width of the global history, defining the maximum entries in 
 *  both global and choice predictor tables.
 */

    
    unsigned globalHistoryBits;

    //unsigned globalHistoryMask;

    /** Mask to apply to globalHistory to access choice history table.
     *  Based on choicePredictorSize.*/
    unsigned branchAddrMask;

    

    /** Total number of entries in the choice predictor. */
// unsigned Pred_Size;

/** Bit count for each counter in the choice predictor. */
// unsigned PHT_Counter_BITS;

/** Array holding counters that comprise the choice predictor. */

    std::vector<SatCounter8> PHT_Counters;

  /** Threshold values for counter: above the threshold indicates taken,
 *  while equal to or below indicates not taken.
 */

    unsigned PHTThreshold;
};


#endif // __CPU_PRED_CUSTOM_BP_HH__

