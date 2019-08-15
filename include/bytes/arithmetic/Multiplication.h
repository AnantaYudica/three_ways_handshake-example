#ifndef BYTES_ARITHMETIC_MULTIPLICATION_H_
#define BYTES_ARITHMETIC_MULTIPLICATION_H_

#include <cstdint>
#include <cstddef>
#include <memory>

#include "../sys/Endian.h"
#include "../ptr/Segment.h"
#include "../Pointer.h"
#include "../Assign.h"
#include "Addition.h"
#include "bitwise/Shift.h"

namespace bytes
{
namespace arithmetic
{

class Multiplication
{
public:
    typedef std::shared_ptr<bytes::Pointer> PointerPtrType;
    typedef bytes::ptr::Segment SegmentType;
    typedef const SegmentType ConstSegmentType;
    typedef std::shared_ptr<SegmentType> SegmentPtrType;
    typedef std::shared_ptr<ConstSegmentType> ConstSegmentPtrType;
public:
    static inline void Operator(ConstSegmentPtrType a_segment, 
        ConstSegmentPtrType b_segment,
        PointerPtrType result_ptr, SegmentPtrType result_segment);
    static inline void Operator(SegmentPtrType a_segment, 
        ConstSegmentPtrType b_segment,
        PointerPtrType result_ptr, SegmentPtrType result_segment);
    static inline void Operator(ConstSegmentPtrType a_segment, 
        SegmentPtrType b_segment,
        PointerPtrType result_ptr, SegmentPtrType result_segment);
    static inline void Operator(SegmentPtrType a_segment, 
        SegmentPtrType b_segment,
        PointerPtrType result_ptr, SegmentPtrType result_segment);
public:
    static inline void Operator(PointerPtrType a_result_ptr,
        SegmentPtrType a_result_segment, ConstSegmentPtrType b_segment);
    static inline void Operator(PointerPtrType a_result_ptr,
        SegmentPtrType a_result_segment, SegmentPtrType b_segment);
private:
    inline Multiplication() = delete;
public:
    inline ~Multiplication() = delete;
};

inline void Multiplication::Operator(ConstSegmentPtrType a_segment, 
    ConstSegmentPtrType b_segment,
    PointerPtrType result_ptr, SegmentPtrType result_segment)
{
    const auto & endian = bytes::sys::Endian();
    const auto a_size = a_segment->Size();
    const auto b_size = b_segment->Size();
    const auto result_size = result_segment->Size();
    bytes::Pointer mul_ptr{result_size};
    auto mul_trait = std::make_shared<bytes::Trait>(result_segment->Trait());
    auto mul_segment = mul_ptr.Share(0, result_size, mul_trait);
    for (std::size_t i = 0; i < b_size; ++i)
    {
        bytes::Assign::Operator(mul_segment, std::uint8_t(0));
        std::uint16_t carry = 0;
        std::uint8_t * carry_ptr = reinterpret_cast<std::uint8_t *>(&carry);
        std::size_t j;
        for (j = 0; j < a_size; ++j)
        {
            if (b_segment->At(i) == 0 && carry == 0) break;
            std::uint16_t val = a_segment->At(j);
            val *= b_segment->At(i);
            std::uint8_t * pval = reinterpret_cast<std::uint8_t *>(&val);
            carry += pval[endian.At(0, 0, 2)];
            mul_segment->At(j) = carry_ptr[endian.At(0, 0, 2)];
            carry >>= sizeof(std::uint8_t) * 8;
            carry += pval[endian.At(1, 0, 2)];
        }
        const std::size_t resize = (carry_ptr[endian.At(1, 0, 2)] != 0 ? 2 :
            (carry_ptr[endian.At(0, 0, 2)] != 0 ? 1 : 0));
        if (resize > 0)
            mul_ptr.Reallocate(mul_segment->Size() + resize, mul_segment);
        mul_segment->At(j++) = carry_ptr[endian.At(0, 0, 2)];
        mul_segment->At(j) = carry_ptr[endian.At(1, 0, 2)];
        bytes::arithmetic::bitwise::Shift::Operator(mul_segment, 8 * i);
        bytes::arithmetic::Addition::Operator(result_ptr, result_segment, 
            mul_segment);
    }
}

inline void Multiplication::Operator(SegmentPtrType a_segment, 
    ConstSegmentPtrType b_segment,
    PointerPtrType result_ptr, SegmentPtrType result_segment)
{
    Multiplication::Operator(
        std::const_pointer_cast<ConstSegmentType>(a_segment),
        b_segment, result_ptr, result_segment);
}

inline void Multiplication::Operator(ConstSegmentPtrType a_segment, 
    SegmentPtrType b_segment,
    PointerPtrType result_ptr, SegmentPtrType result_segment)
{
    Multiplication::Operator(a_segment,
        std::const_pointer_cast<ConstSegmentType>(b_segment),
        result_ptr, result_segment);
}

inline void Multiplication::Operator(SegmentPtrType a_segment, 
    SegmentPtrType b_segment,
    PointerPtrType result_ptr, SegmentPtrType result_segment)
{
    Multiplication::Operator(
        std::const_pointer_cast<ConstSegmentType>(a_segment),
        std::const_pointer_cast<ConstSegmentType>(b_segment), 
        result_ptr, result_segment);
}

inline void Multiplication::Operator(PointerPtrType a_result_ptr,
    SegmentPtrType a_result_segment, ConstSegmentPtrType b_segment)
{
    const std::size_t expand_size = a_result_segment->Size() + 
        b_segment->Size();
    auto result_ptr = std::make_shared<bytes::Pointer>(expand_size);
    auto result_trait = 
        std::make_shared<bytes::Trait>(a_result_segment->Trait());
    auto result_segment = result_ptr->Share(0, expand_size, result_trait);
    bytes::Assign::Operator(result_segment, std::uint8_t(0));
    Multiplication::Operator(a_result_segment, b_segment, result_ptr, 
        result_segment);
    std::size_t reduce_size = 0;
    for (std::size_t i = result_segment->Size() - 1; 
        i < result_segment->Size(); --i, ++reduce_size)
    {
        if (result_segment->At(i) != std::uint8_t(0)) break;
    }
    bytes::Assign::Operator(result_ptr, a_result_segment, 0, 
        result_segment, 0, result_segment->Size() - reduce_size);
}

inline void Multiplication::Operator(PointerPtrType a_result_ptr,
    SegmentPtrType a_result_segment, SegmentPtrType b_segment)
{}

} //!arithmetic

} //!bytes

#endif //!BYTES_ARITHMETIC_MULTIPLICATION_H_