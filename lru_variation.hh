#ifndef __MEM_CACHE_REPLACEMENT_POLICIES_LRU_VARIATION_HH__
#define __MEM_CACHE_REPLACEMENT_POLICIES_LRU_VARIATION_HH__

#include "mem/cache/replacement_policies/base.hh"

namespace ReplacementPolicy {

class LRU_Variation : public Base
{
  protected:
    /** LRU_Variation-specific replacement data. */
    struct LRU_VariationData : public ReplacementData
    {
        int blockIndex;  // Index to fetch positions from IPV_Graph
        std::shared_ptr<std::vector<int>> positions;

        LRU_VariationData(int idx, std::shared_ptr<std::vector<int>> pos)
            : blockIndex(idx), positions(pos) {}
    };

    const std::vector<int> IPV_Graph{
        0, 0, 1, 0, 3, 0, 3, 2, 1, 0, 5, 1, 0, 0, 7, 11}; // Hardcoded IPV
    const int insertionState = 5;  // Explicit insertion state (from | 5)

    std::shared_ptr<std::vector<int>> positions;
    mutable int blockCount;

  public:
    explicit LRU_Variation(const Params &p);

    void invalidate(const std::shared_ptr<ReplacementData>&) const override;
    void touch(const std::shared_ptr<ReplacementData>&) const override;
    void reset(const std::shared_ptr<ReplacementData>&) const override;
    ReplaceableEntry* getVictim(const ReplacementCandidates&) const override;
    std::shared_ptr<ReplacementData> instantiateEntry() override;
};

} // namespace ReplacementPolicy

#endif // __MEM_CACHE_REPLACEMENT_POLICIES_LRU_VARIATION_HH__


