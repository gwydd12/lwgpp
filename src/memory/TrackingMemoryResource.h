#ifndef LWGPP_TRACKINGMEMORYRESOURCE_H
#define LWGPP_TRACKINGMEMORYRESOURCE_H
#include <atomic>
#include <ostream>
#include <memory_resource>

namespace memory {
    class TrackingMemoryResource : public std::pmr::memory_resource {
    public:
        struct Stats {
            std::size_t total_bytes_allocated = 0;
            std::size_t alloc_calls = 0;
            std::size_t current_bytes = 0;
            std::size_t peak_bytes = 0;
        };

        explicit TrackingMemoryResource(memory_resource* parent = std::pmr::get_default_resource())
            : parent_(parent) {}

        [[nodiscard]] Stats get_stats() const noexcept {
            return Stats{
            total_bytes_allocated_.load(),
                alloc_calls_.load(),
                current_bytes_.load(),
                peak_bytes_.load()
            };
        }

        void reset_stats() noexcept {
            total_bytes_allocated_ = 0;
            alloc_calls_ = 0;
            current_bytes_ = 0;
            peak_bytes_ = 0;
        }

    protected:
        void* do_allocate(const std::size_t bytes, const std::size_t alignment) override {
            void* p = parent_->allocate(bytes, alignment);
            total_bytes_allocated_ += bytes;
            ++alloc_calls_;
            current_bytes_ += bytes;
            if (current_bytes_.load() > peak_bytes_.load()) {
                peak_bytes_ = current_bytes_.load();
            }
            return p;
        }

        void do_deallocate(void* p, const std::size_t bytes, const std::size_t alignment) override {
            parent_->deallocate(p, bytes, alignment);
            current_bytes_ -= bytes;
        }

        [[nodiscard]] bool do_is_equal(const memory_resource& other) const noexcept override {
            if (const auto* o = dynamic_cast<const TrackingMemoryResource*>(&other)) {
                return parent_->is_equal(*o->parent_);
            }
            return false;
        }

    private:
        memory_resource* parent_;
        std::atomic<std::size_t> total_bytes_allocated_{0};
        std::atomic<std::size_t> alloc_calls_{0};
        std::atomic<std::size_t> current_bytes_{0};
        std::atomic<std::size_t> peak_bytes_{0};
    };

    inline std::ostream& operator<<(std::ostream& os, const TrackingMemoryResource::Stats& s) {
        os << "total_bytes_allocated: " << s.total_bytes_allocated
           << ", alloc_calls: " << s.alloc_calls
           << ", current_bytes: " << s.current_bytes
           << ", peak_bytes: " << s.peak_bytes;
        return os;
    }
}

#endif //LWGPP_TRACKINGMEMORYRESOURCE_H