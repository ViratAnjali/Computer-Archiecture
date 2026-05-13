#include "mem/cache/replacement_policies/lru_variation.hh"
#include <cassert>
#include <memory>

namespace ReplacementPolicy {

LRU_Variation::LRU_Variation(const Params &p)
    : Base(p),
      positions(std::make_shared<std::vector<int>>(IPV_Graph.size(), -1)),
      blockCount(0)
{
}

void
LRU_Variation::invalidate(const std::shared_ptr<ReplacementData>& replacementData) const
{
    auto replData = std::static_pointer_cast<LRU_VariationData>(replacementData);
    (*positions)[replData->blockIndex] = -1; // Invalidate block position
}

void
LRU_Variation::touch(const std::shared_ptr<ReplacementData>& replacementData) const
{
    auto replData = std::static_pointer_cast<LRU_VariationData>(replacementData);
    int oldPosition = (*positions)[replData->blockIndex];
    int newPosition = IPV_Graph[replData->blockIndex];

    for (size_t i = 0; i < positions->size(); ++i) {
        if ((*positions)[i] >= newPosition && (*positions)[i] < oldPosition) {
            (*positions)[i]++;
        }
    }
    (*positions)[replData->blockIndex] = newPosition;
}

void
LRU_Variation::reset(const std::shared_ptr<ReplacementData>& replacementData) const
{
    auto replData = std::static_pointer_cast<LRU_VariationData>(replacementData);

    for (size_t i = 0; i < positions->size(); ++i) {
        if ((*positions)[i] >= insertionState && (*positions)[i] != -1) {
            (*positions)[i]++;
        }
    }
    (*positions)[replData->blockIndex] = insertionState;
    blockCount++;
}

ReplaceableEntry*
LRU_Variation::getVictim(const ReplacementCandidates& candidates) const
{
    assert(candidates.size() > 0);

    ReplaceableEntry* victim = candidates[0];
    for (const auto& candidate : candidates) {
        auto replDataCandidate = std::static_pointer_cast<LRU_VariationData>(
            candidate->replacementData);
        auto replDataVictim = std::static_pointer_cast<LRU_VariationData>(
            victim->replacementData);

        if ((*positions)[replDataCandidate->blockIndex] >
            (*positions)[replDataVictim->blockIndex]) {
            victim = candidate;
        }
    }
    return victim;
}

std::shared_ptr<ReplacementData>
LRU_Variation::instantiateEntry()
{
    int idx = blockCount % IPV_Graph.size();
    return std::make_shared<LRU_VariationData>(idx, positions);
}

} // namespace ReplacementPolicy


