#ifndef BYTES_ARITHMETIC_BITWISE_AND_H_
#define BYTES_ARITHMETIC_BITWISE_AND_H_

#include <utility>
#include <cstddef>
#include <memory>

#include "../../sys/Endian.h"
#include "../../Pointer.h"
#include "../../ptr/Segment.h"

namespace bytes
{
namespace arithmetic
{
namespace bitwise
{

class And
{
public:
    typedef std::shared_ptr<bytes::Pointer> PointerPtrType;
    typedef bytes::ptr::Segment SegmentType;
    typedef const SegmentType ConstSegmentType;
    typedef std::shared_ptr<SegmentType> SegmentPtrType;
    typedef std::shared_ptr<ConstSegmentType> ConstSegmentPtrType;
public:
    static inline void Operator(ConstSegmentPtrType a_segment,
        ConstSegmentPtrType b_segment, SegmentPtrType result_segment);
    static inline void Operator(SegmentPtrType a_segment,
        ConstSegmentPtrType b_segment, SegmentPtrType result_segment);
    static inline void Operator(ConstSegmentPtrType a_segment,
        SegmentPtrType b_segment, SegmentPtrType result_segment);
    static inline void Operator(SegmentPtrType a_segment,
        SegmentPtrType b_segment, SegmentPtrType result_segment);
    static inline void Operator(PointerPtrType a_result_ptr,
        SegmentPtrType a_result_segment, ConstSegmentPtrType b_segment);
    static inline void Operator(PointerPtrType a_result_ptr,
        SegmentPtrType a_result_segment, SegmentPtrType b_segment);
private:
    inline And() = delete;
public:
    inline ~And() = delete;
};


inline void And::Operator(ConstSegmentPtrType a_segment,
        ConstSegmentPtrType b_segment, SegmentPtrType result_segment)
{
    for(std::size_t i = 0; i < result_segment->Size(); ++i)
    {
        const bool a_is_end = a_segment->IsEnd(i),
            b_is_end = b_segment->IsEnd(i);
        if (a_is_end || b_is_end) result_segment->At(i) = std::uint8_t(0);
        else result_segment->At(i) = a_segment->At(i) & b_segment->At(i);
    }
}

inline void And::Operator(SegmentPtrType a_segment,
        ConstSegmentPtrType b_segment, SegmentPtrType result_segment)
{
    And::Operator(std::const_pointer_cast<ConstSegmentType>(a_segment), 
        b_segment, result_segment);
}

inline void And::Operator(ConstSegmentPtrType a_segment,
        SegmentPtrType b_segment, SegmentPtrType result_segment)
{
    And::Operator(a_segment, 
        std::const_pointer_cast<ConstSegmentType>(b_segment), result_segment);
}

inline void And::Operator(SegmentPtrType a_segment,
        SegmentPtrType b_segment, SegmentPtrType result_segment)
{
    And::Operator(std::const_pointer_cast<ConstSegmentType>(a_segment), 
        std::const_pointer_cast<ConstSegmentType>(b_segment), result_segment);
}

inline void And::Operator(PointerPtrType a_result_ptr,
    SegmentPtrType a_result_segment, ConstSegmentPtrType b_segment)
{
    const auto b_size = b_segment->Size();
    if (b_size > a_result_segment->Size())
        a_result_ptr->Reallocate(b_size, a_result_segment);
    And::Operator(a_result_segment, b_segment, a_result_segment);
}

inline void And::Operator(PointerPtrType a_result_ptr,
        SegmentPtrType a_result_segment, SegmentPtrType b_segment)
{
    And::Operator(a_result_ptr, a_result_segment, 
        std::const_pointer_cast<ConstSegmentType>(b_segment));
}

} //!bitwise

} //!arithmetic

} //!bytes

#endif //!BYTES_ARITHMETIC_BITWISE_AND_H_
