#include "bit_vector.h"

// See bit_vector.h
bit_vector_t* bit_vector_create(size_t size, bit_t value)
{

    if((int) size <= 0) {
        return NULL;
    }

    int newsize = CLAMP32(size);

    bit_vector_t* vect = malloc(sizeof(bit_vector_t));
    if(vect!= NULL) {  // if malloc successful
        vect->size = size;
        vect->content = calloc(newsize/IMAGE_LINE_WORD_BITS, sizeof(uint32_t));
        if(vect->content != NULL) { // if calloc successful
            vect->allocated = size;
        } else { // if calloc unsuccessful, free everything and return null
            vect->allocated = 0;
            vect->size = 0;
            vect = NULL;
            return vect;
        }

        uint32_t finalVal = (value == 1) ? GENERATE_FF(IMAGE_LINE_WORD_BITS) : 0; // content of vectors depending on value

        for(int i = 0; i < (newsize/IMAGE_LINE_WORD_BITS); ++i) { // set vector content
            *(vect->content + i) = finalVal;
        }

        if((size % IMAGE_LINE_WORD_BITS) != 0) { // adjust vector content if size is not a multiple of 32
            int shift = (IMAGE_LINE_WORD_BITS - (size % IMAGE_LINE_WORD_BITS));
            *(vect->content + (size/IMAGE_LINE_WORD_BITS)) = (finalVal << shift) >> shift;
        }
    }

    return vect;
}

// See bit_vector.h
bit_vector_t* bit_vector_cpy(const bit_vector_t* pbv)
{

    if(pbv == NULL)
        return NULL;

    bit_vector_t* copy = bit_vector_create(pbv->size, 0); // create blank copy of same size as pbv

    for(int i = 0; i < CLAMP32(pbv->size)/IMAGE_LINE_WORD_BITS; ++i) {
        *(copy->content + i) = *(pbv->content + i); // fill with same content as pbv
    }

    return copy;
}

// See bit_vector.h
bit_t bit_vector_get(const bit_vector_t* pbv, size_t index)
{

    if(pbv == NULL || index >= pbv->size) {
        return 0;
    }

    uint32_t word = *(pbv->content + index/IMAGE_LINE_WORD_BITS); // isolate word of index
    uint8_t byte = (word >> (index/BYTE_SIZE)*BYTE_SIZE) & (GENERATE_FF(BYTE_SIZE)); // isolate byte of index

    return bit_get(byte, ((index % IMAGE_LINE_WORD_BITS) % BYTE_SIZE));
}

// See bit_vector.h
bit_vector_t* bit_vector_not(bit_vector_t* pbv)
{

    if(pbv != NULL) {
        for(int i = 0; i < CLAMP32(pbv->size)/IMAGE_LINE_WORD_BITS; ++i) {
            *(pbv->content + i) = ~(*(pbv->content + i));
        }

        if(pbv->size % IMAGE_LINE_WORD_BITS != 0) { // if size is not a multiple of 32, adjust unconcerned bits
            *(pbv->content + pbv->size/IMAGE_LINE_WORD_BITS) = *(pbv->content + pbv->size/IMAGE_LINE_WORD_BITS) & (GENERATE_FF(IMAGE_LINE_WORD_BITS) >> (IMAGE_LINE_WORD_BITS - (pbv->size % IMAGE_LINE_WORD_BITS)));
        }
    }

    return pbv;
}

// See bit_vector.h
bit_vector_t* bit_vector_and(bit_vector_t* pbv1, const bit_vector_t* pbv2)
{

    if(pbv1 != NULL && pbv2 != NULL) {
        if(pbv1->size != pbv2-> size) {
            return NULL;
        }

        for(int i = 0; i < GENERATE_MAX(pbv1->size); ++i) {
            *(pbv1->content + i) = (*(pbv1->content + i))&(*(pbv2->content + i));
        }
    }
    return pbv1;
}

// See bit_vector.h
bit_vector_t* bit_vector_or(bit_vector_t* pbv1, const bit_vector_t* pbv2)
{

    if(pbv1 != NULL && pbv2 != NULL) {
        if(pbv1->size != pbv2-> size) {
            return NULL;
        }

        for(int i = 0; i < GENERATE_MAX(pbv1->size); ++i) {
            *(pbv1->content + i) = (*(pbv1->content + i))|(*(pbv2->content + i));
        }
    }
    return pbv1;
}

// See bit_vector.h
bit_vector_t* bit_vector_xor(bit_vector_t* pbv1, const bit_vector_t* pbv2)
{

    if(pbv1 != NULL && pbv2 != NULL) {
        if(pbv1->size != pbv2-> size) {
            return NULL;
        }
        for(int i = 0; i < CLAMP32(pbv1->size)/IMAGE_LINE_WORD_BITS; ++i) {
            *(pbv1->content + i) = (*(pbv1->content + i))^(*(pbv2->content + i));
        }

    }
    return pbv1;
}

// See bit_vector.h
bit_vector_t* bit_vector_extract_zero_ext(const bit_vector_t* pbv, int64_t index, size_t size)
{

    return bit_vector_extract(0, pbv, index, size);
}

// See bit_vector.h
bit_vector_t* bit_vector_extract_wrap_ext(const bit_vector_t* pbv, int64_t index, size_t size)
{

    return bit_vector_extract(1, pbv, index, size);
}

// See bit_vector.h
bit_vector_t* bit_vector_shift(const bit_vector_t* pbv, int64_t shift)
{

    if(shift != 0) {
        return bit_vector_extract_zero_ext(pbv, -shift, pbv->size);
    }

    return bit_vector_cpy(pbv);

}

// See bit_vector.h
bit_vector_t* bit_vector_join(const bit_vector_t* pbv1, const bit_vector_t* pbv2, int64_t shift)
{

    if(pbv1 == NULL || pbv2 == NULL || pbv1->size != pbv2->size || !(0 <= shift && shift <= pbv1->size)) {
        return NULL;
    }

    bit_vector_t* res = bit_vector_create(pbv2->size, 0); // create basic vector

    for(int i = 0; i < (shift/IMAGE_LINE_WORD_BITS); ++i) {
        *(res->content + i) = *(pbv1->content + i);		// fill first part with pbv1 content
    }

    if(shift % IMAGE_LINE_WORD_BITS == 0) {

        for(int i = shift/IMAGE_LINE_WORD_BITS; i < (pbv2->size/IMAGE_LINE_WORD_BITS); ++i) {
            *(res->content + i) = *(pbv2->content + i); // if the shift value is a multiple of 32, fill the rest with pbv2 content and return
        }

        return res;
    }

    for(int i = (shift/IMAGE_LINE_WORD_BITS + 1); i < (pbv1->size / IMAGE_LINE_WORD_BITS); ++i) {
        *(res->content + i) = *(pbv2->content + i); // fill other part with pbv2 content
    }

    // extract words in pbv1 and pbv2 that will be merged
    uint32_t merge1 = *(pbv2->content + shift/IMAGE_LINE_WORD_BITS);
    uint32_t merge2 = *(pbv1->content + shift/IMAGE_LINE_WORD_BITS);

    // prepare for merge ; set unconcerned bits to 0
    int shiftMod = shift % IMAGE_LINE_WORD_BITS;
    merge1 = (merge1 >> (IMAGE_LINE_WORD_BITS - shiftMod)) << (IMAGE_LINE_WORD_BITS - shiftMod);
    merge2 = (merge2 << shiftMod) >> shiftMod;

    // merge both words and store in final vector
    *(res->content + shift/IMAGE_LINE_WORD_BITS) = merge1 | merge2;

    return res;

}

// See bit_vector.h
int bit_vector_print(const bit_vector_t* pbv)
{

    int count = 0;
    for(int i = pbv->allocated - 1; i >= 0; i -= 1) {
        count += printf("%u", bit_vector_get(pbv, i));
    }
    return count;
}

// See bit_vector.h
int bit_vector_println(const char* prefix, const bit_vector_t* pbv)
{

    int count = printf("%s", prefix);
    count += bit_vector_print(pbv);
    count +=printf("\n");

    return count;
}

// See bit_vector.h
void bit_vector_free(bit_vector_t** pbv)
{

    free(pbv[0]->content);
    free(pbv[0]);
    pbv = NULL;
}

// See bit_vector.h
bit_vector_t* bit_vector_extract(bit_t type, const bit_vector_t* pbv, int64_t index, size_t size)
{

    if(size == 0 || (type == 1 && pbv == NULL)) {
        return NULL;
    }

    bit_vector_t* res = bit_vector_create(size, 0);

    if(pbv != NULL) {

        uint8_t base = 0;

        // get extract byte, 0 if it is 0-extract and appropriate byte otherwise
        uint8_t alternative = (type == 0) ? 0 : ((*(pbv->content + ((index % pbv->size)/IMAGE_LINE_WORD_BITS)) >> (((index % pbv->size/BYTE_SIZE) * BYTE_SIZE)) & GENERATE_FF(BYTE_SIZE)));

        // get either byte at direct index or appropriate extract byte
        uint8_t other = (index >= 0 && index < pbv->allocated) ? (((*(pbv->content + (index/IMAGE_LINE_WORD_BITS))) >> ((index/BYTE_SIZE)*BYTE_SIZE)) & GENERATE_FF(BYTE_SIZE)) : alternative;


        for (int i = 0; i <= size; ++i) {

            if(i % BYTE_SIZE == 0 || (i == size && size < IMAGE_LINE_WORD_BITS)) { // if base byte is filled

                uint32_t base32 = (size > BYTE_SIZE) ? base << ((i % IMAGE_LINE_WORD_BITS) - BYTE_SIZE) : base; // change to 32-bit word and shift to appropriate location

                int ind = (i % IMAGE_LINE_WORD_BITS == 0 && i != 0) ? (i/IMAGE_LINE_WORD_BITS) - 1 : (i/IMAGE_LINE_WORD_BITS); // get index of word to store
                *(res->content + ind) = *(res->content + ind) | base32; // adjust vector content
                base = 0; // reset base
            }

            int ind = index;

            if(type == 1 && (index < 0 || index >= pbv->size)) {
                ind = index % pbv->size; // in case of extract wrap, get index to extract from
            }


            if((index % BYTE_SIZE == 0 || index % pbv->size == 0) && i < size) { // if the reference byte needs to be changed

                if(type == 0) { // update reference byte if 0-extraction
                    other = (index >= 0 && index < pbv->allocated) ? ((*(pbv->content + (index/IMAGE_LINE_WORD_BITS)) >> (index % IMAGE_LINE_WORD_BITS)) & GENERATE_FF(BYTE_SIZE)) : 0;
                }

                else { // update reference byte if wrap-extraction
                    int div = (pbv->size >= IMAGE_LINE_WORD_BITS) ? IMAGE_LINE_WORD_BITS : pbv->size;
                    other = (index % pbv->size == 0) ? ((*(pbv->content + (ind/div))) & 0x00FF) : ((*(pbv->content + (ind/div)) >> (ind % div)) & GENERATE_FF(BYTE_SIZE));
                }
            }

            if(i < size && other != 0) {
                bit_t at_index = bit_get(other, (ind % BYTE_SIZE)); // get bit at index from reference byte
                if(at_index == 1) {
                    bit_set( &base, (i % BYTE_SIZE));    // set bit in base byte
                }
            }

            ++index; // increment index
        }
    }

    return res;

}


