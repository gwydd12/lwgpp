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

        Stats get_stats() const noexcept;
        void reset_stats() noexcept;

    protected:
        void* do_allocate(std::size_t bytes, std::size_t alignment) override;
        void do_deallocate(void* p, std::size_t bytes, std::size_t alignment) override;
        bool do_is_equal(const memory_resource& other) const noexcept override;

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