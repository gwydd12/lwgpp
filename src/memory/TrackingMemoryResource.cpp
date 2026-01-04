#include "TrackingMemoryResource.h"

using namespace memory;

TrackingMemoryResource::Stats TrackingMemoryResource::get_stats() const noexcept {
    return Stats{
        total_bytes_allocated_.load(),
            alloc_calls_.load(),
            current_bytes_.load(),
            peak_bytes_.load()
        };
}

void TrackingMemoryResource::reset_stats() noexcept {
    total_bytes_allocated_ = 0;
    alloc_calls_ = 0;
    current_bytes_ = 0;
    peak_bytes_ = 0;
}

void* TrackingMemoryResource::do_allocate(const std::size_t bytes, const std::size_t alignment) {
    void* p = parent_->allocate(bytes, alignment);
    total_bytes_allocated_ += bytes;
    ++alloc_calls_;
    current_bytes_ += bytes;
    if (current_bytes_.load() > peak_bytes_.load()) {
        peak_bytes_ = current_bytes_.load();
    }
    return p;
}

void TrackingMemoryResource::do_deallocate(void* p, const std::size_t bytes, const std::size_t alignment) {
    parent_->deallocate(p, bytes, alignment);
    current_bytes_ -= bytes;
}

bool TrackingMemoryResource::do_is_equal(const memory_resource& other) const noexcept {
    if (const auto* o = dynamic_cast<const TrackingMemoryResource*>(&other)) {
        return parent_->is_equal(*o->parent_);
    }
    return false;
}

