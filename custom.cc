//Computer Architecture II | Project 2
//This is a project code from Team 30, Team associates: Shashank Shivakumar Muthkur, Amitava Ray, Anjali
//Team leader is Anjali with ASU ID: 1234157186 >>> print("The modulus result is {}".format(1234157186%18)) The modulus result is 2, Branch Address and Global History Bits are 8. Operation between them is AND.

#include "cpu/pred/custom.hh"
#include "base/bitfield.hh"
#include "base/intmath.hh"
#include "cpu/nativetrace.hh"
#include "arch/generic/vec_pred_reg.hh"

int n=100000;
int n1=100000;
int counter=0;
int counter_1=0;
int counter_2=0;
int counter_3=0;

//The constructor initializes the branch predictor's components when an instance is created.
CustomBP::CustomBP(const CustomBPParams &params)
    : BPredUnit(params),
      globalHistory(params.numThreads, 0), // Initialize global history register for each thread to 0
      globalHistoryBits(params.globalHistoryBits), // Number of bits in the global history register
      Pred_Size(params.PredictorSize), // Predictor size which would be 256 in our configuration
      PHT_Counter_BITS(params.PHTCtrlBits), // Number of bits in the counters of the predictor
      PHT_Counters(Pred_Size, SatCounter8(PHT_Counter_BITS))

{
    // Debug print statements for initialization
    printf("CustomBP initialized with: \n");
    printf("Predictor Size: %u, Global History Bits: %u, PHTCtrlBits: %u\n", Pred_Size, globalHistoryBits, PHT_Counter_BITS);
   
   

    if(!isPowerOf2(Pred_Size)){
        fatal("Invalid choice predictor size!\n");
    }// check that choice predictor size is a power of 2

    // this is equivalent to mask(log2(PredictorSize)
    branchAddrMask = Pred_Size - 1;


    globalHistoryMask = mask(globalHistoryBits); // GH

    // Set thresholds for the predictors' counters
    // This is equivalent to (2^(Ctr))/2 - 1
    PHTThreshold = (1ULL << (PHT_Counter_BITS - 1)) - 1;

// constructor ends
//
}

// helper function that calculates the index into the local history table
inline
unsigned
CustomBP::Calculate_Local_History_Index(Addr &branch_addr)
{
    // Get low order bits after removing instruction offset.
    return (branch_addr >> instShiftAmt) & branchAddrMask;
}


// helper function that updates the global history register when a branch is taken
inline
void
CustomBP::Update_Global_History_Taken(ThreadID tid)
{
    //Increments counter_2 and, if it’s a multiple of n1, prints a message that the global history was updated as "Taken."
    //Shifts the globalHistory for the thread (tid) left by 1, adding a '1' bit to signify the branch was taken.
    //Applies globalHistoryMask using a bitwise AND to ensure globalHistory remains within the bit length specified by globalHistoryMask.
        if(counter_2++ % n1 ==0){
                printf(" %d  Global History Taken Updated\n",counter_2);
        }
        globalHistory[tid] = (globalHistory[tid] << 1) | 1;
        globalHistory[tid] = globalHistory[tid] & globalHistoryMask;
}

inline
void
CustomBP::Update_Global_History_Not_Taken(ThreadID tid)
{
    //Similarly increments counter_2, printing a message if it’s a multiple of n1 that the global history was updated as "Not Taken."
    //Shifts the globalHistory for tid left by 1, adding a '0' bit to represent that the branch was not taken.
    //Also applies globalHistoryMask to keep the history within the specified number of bits.
        if(counter_2++ % n1 == 0){
                printf(" %d  Global History Not Taken Updated\n",counter_2);
        }
        globalHistory[tid] = (globalHistory[tid] << 1);
        globalHistory[tid] = globalHistory[tid] & globalHistoryMask;
}


void
CustomBP::btbUpdate(ThreadID tid, Addr branch_addr, void * &BP_History)
{
    // Updates the Branch Target Buffer (BTB) with the target address of a branch, ensuring future predictions have accurate branch target information.
    // Clear the least significant bit of the global history
    globalHistory[tid] &= (globalHistoryMask & ~1ULL);
    if (counter_3++ % n1 == 0){
    printf("BTB Update: Branch Addr: %#lx, ThreadID: %d\n", branch_addr, tid);
    }

}

bool
CustomBP::lookup(ThreadID tid, Addr branch_addr, void * &BP_History){

    //The lookup function is called to predict the outcome of a branch. It usually takes the branch address and the current global history to do
    //

    if (counter_3++ % n1 == 0){
    printf("Lookup: Branch Addr: %#lx, ThreadID: %u\n", branch_addr, tid);
    }


    unsigned branchAddrIdx = (branch_addr >> instShiftAmt) & branchAddrMask;
    unsigned globalHistoryIdx = globalHistory[tid] & globalHistoryMask;


    // AND Operation on the Branch Address and Global History
    unsigned PHTIdx = (branchAddrIdx & globalHistoryIdx) & branchAddrMask;


    // Predict taken or "true" if counter value is higher than threshold, else not-taken or "false"
    bool PHT_prediction = PHT_Counters[PHTIdx] > PHTThreshold;

    
    // Create BPHistory and pass it back to be recorded.
    BPHistory *history = new BPHistory;
    history->globalHistory = globalHistory[tid] & globalHistoryMask;
    if (counter_3++ % n1 == 0){
    printf("ThreadID: %u, history: %u (masked)\n", tid, history);
    }


    BP_History = static_cast<void*>(history);

    if(counter_1++ % n == 0){
            printf("Prediction = %d\n",PHT_prediction);
    }

    if (PHT_prediction){
        Update_Global_History_Taken(tid);
        return true;
    }else{
        Update_Global_History_Not_Taken(tid);
        return false;
    }

}

void
CustomBP::uncondBranch(ThreadID tid, Addr pc, void * &BP_History)
{
    //Functionality: Marks a branch as unconditional, automatically predicting it as taken, skipping prediction logic for these branches since their outcome is known.
    //Modifies the history using the thread, prediction attribute and global history attribute of the history.
    if (counter_3++ % n1 == 0){
    printf("Unconditional Branch: PC: %#lx, ThreadID: %d\n", pc, tid);
    }
    // Create BPHistory and pass it back to be recorded.
    BPHistory *history = new BPHistory;
    history->globalHistory = globalHistory[tid] & globalHistoryMask;;
    BP_History = static_cast<void *>(history);

    Update_Global_History_Taken(tid);

}

void
CustomBP::update(ThreadID tid, Addr branch_addr, bool taken,
                     void *BP_History, bool squashed, const StaticInstPtr & inst, Addr corrTarget)
{

    assert(BP_History);

    BPHistory *history = static_cast<BPHistory*>(BP_History);

    if (squashed) {
        // Global history restore and update
        globalHistory[tid] = (history->globalHistory << 1) | taken;
        if (counter_3++ % n1 == 0){
        printf("Update: Squashed. Restoring global history for ThreadID: %d\n", tid);
        }
        return;
    }

    unsigned branchAddrIdx = (branch_addr >> instShiftAmt) & branchAddrMask;
    unsigned globalHistoryIdx = history->globalHistory & globalHistoryMask;


    // Find the correct counter to increment or decrement
    unsigned PHT_predictor_idx = (branchAddrIdx & globalHistoryIdx) & branchAddrMask;

    
    //DPRINTF(CDebug, "This is CDebug Taken ==== %d",taken);
    //printf("This is an ordinary printf %d",taken);

    if(counter % n == 0)
    {
            printf("This is after 10000th %d is value of Taken",taken);
    }

    if(taken)
    {
        PHT_Counters[PHT_predictor_idx]++; //increment
        if(counter % n == 0)
        {
                printf("PHT Counter incremented\n");
        }
        counter++;

    }
    else
    {
        PHT_Counters[PHT_predictor_idx]--; //decrement
        if(counter % n == 0)
        {
                printf("PHT Counter decremented\n");
        }
        counter++;

    }

    // We're done with this history, now delete it.
    delete history;

}

void
CustomBP::squash(ThreadID tid, void *BP_History)
{
    //Clears or resets prediction information if a misprediction occurs, helping to prevent incorrect predictions from affecting future branches.
    //printf("Squash: Restoring global history for ThreadID: %d\n", tid);
    BPHistory *history = static_cast<BPHistory*>(BP_History);
    if (counter_3++ % n1 == 0){
    printf("Squash: Restoring global history for ThreadID: %d\n", tid);
    }
    // Restore global history to state prior to this branch.
    globalHistory[tid] = history->globalHistory;

    delete history;

}



