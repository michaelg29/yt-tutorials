#ifndef UNIFORMMEMORY_HPP
#define UNIFORMMEMORY_HPP

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <vector>
#include <string>

#include "vertexmemory.hpp"
#include "../rendering/shader.h"

#define N 4 // word size

namespace UBO {
    enum class Type : unsigned char {
        SCALAR = 0,
        VEC2,
        VEC3,
        VEC4,
        ARRAY,
        STRUCT,
        INVALID
    };

    // round up val to the next multiple of 2^n
    inline unsigned int roundUpPow2(unsigned int val, unsigned char n) {
        unsigned int pow2n = 0b1 << n; // = 1 * 2^n = 2^n
        unsigned int divisor = pow2n - 1; // = 0b0111...111 (n 1s)

        // last n bits = remainder of val / 2^n
        // add (2^n - rem) to get to the next multiple of 2^n
        unsigned int rem = val & divisor;
        if (rem) {
            val += pow2n - rem;
        }

        return val;
    }

    typedef struct Element {
        Type type;
        unsigned int baseAlign;
        unsigned int length; // length of the array or num elements in structure
        std::vector<Element> list; // for struct (list of sub-elements), or array (1st slot is the type)
    
        std::string typeStr() {
            switch (type) {
            case Type::SCALAR: return "scalar";
            case Type::VEC2: return "vec2";
            case Type::VEC3: return "vec3";
            case Type::VEC4: return "vec4";
            case Type::ARRAY: return "array<" + list[0].typeStr() + ">";
            case Type::STRUCT: return "struct";
            default: return "invalid";
            };
        }

        unsigned int alignPow2() {
            switch (baseAlign) {
            case 2: return 1;
            case 4: return 2;
            case 8: return 3;
            case 16: return 4;
            default: return 0;
            };
        }

        unsigned int calcSize() {
            switch (type) {
            case Type::SCALAR:
                return N;
            case Type::VEC2:
                return 2 * N;
            case Type::VEC3:
                return 3 * N;
            case Type::VEC4:
                return 4 * N;
            case Type::ARRAY:
            case Type::STRUCT:
                return calcPaddedSize();
            default:
                return 0;
            };
        }

        unsigned int calcPaddedSize() {
            unsigned int offset = 0;

            switch (type) {
            case Type::ARRAY:
                return length * roundUpPow2(list[0].calcSize(), alignPow2());
            case Type::STRUCT:
                for (Element e : list) {
                    offset = roundUpPow2(offset, e.alignPow2());
                    offset += e.calcSize();
                }
                return offset;
            case Type::SCALAR:
            case Type::VEC2:
            case Type::VEC3:
            case Type::VEC4:
            default:
                return calcSize();
            };
        }

        Element(Type type = Type::SCALAR)
            : type(type), length(0), list(0) {
            switch (type) {
            case Type::SCALAR:
                baseAlign = N; break;
            case Type::VEC2:
                baseAlign = 2 * N; break;
            case Type::VEC3:
            case Type::VEC4:
                baseAlign = 4 * N; break;
            default:
                baseAlign = 0; break;
            };
        }
    } Element;

    inline Element newScalar() {
        return Element();
    }

    inline Element newVec(unsigned char dim) {
        switch (dim) {
        case 2: return Type::VEC2;
        case 3: return Type::VEC3;
        case 4:
        default:
            return Type::VEC4;
        };
    }

    inline Element newArray(unsigned int length, Element arrElement) {
        Element ret(Type::ARRAY);
        ret.length = length;
        ret.list = { arrElement };
        ret.list.shrink_to_fit();

        ret.baseAlign = arrElement.type == Type::STRUCT ?
            arrElement.baseAlign :
            roundUpPow2(arrElement.baseAlign, 4);

        return ret;
    }

    inline Element newColMat(unsigned char cols, unsigned char rows) {
        return newArray(cols, newVec(rows));
    }

    inline Element newColMatArray(unsigned int noMatrices, unsigned char cols, unsigned char rows) {
        return newArray(noMatrices * cols, newVec(rows));
    }

    inline Element newRowMat(unsigned char rows, unsigned char cols) {
        return newArray(rows, newVec(cols));
    }

    inline Element newRowMatArray(unsigned int noMatrices, unsigned char rows, unsigned char cols) {
        return newArray(noMatrices * rows, newVec(cols));
    }

    inline Element newStruct(std::vector<Element> subelements) {
        Element ret(Type::STRUCT);
        ret.list.insert(ret.list.end(), subelements.begin(), subelements.end());
        ret.length = ret.list.size();

        // base alignment is largest of its subelements
        if (subelements.size()) {
            for (Element e : subelements) {
                if (e.baseAlign > ret.baseAlign) {
                    ret.baseAlign = e.baseAlign;
                }
            }

            ret.baseAlign = roundUpPow2(ret.baseAlign, 4);
        }

        return ret;
    }

    class UBO : public BufferObject {
    public:
        Element block; // root element of the UBO (struct)
        unsigned int calculatedSize;
        GLuint bindingPos;

        UBO(GLuint bindingPos)
            : BufferObject(GL_UNIFORM_BUFFER),
            block(newStruct({})),
            calculatedSize(0),
            bindingPos(bindingPos) {}

        UBO(GLuint bindingPos, std::vector<Element> elements)
            : BufferObject(GL_UNIFORM_BUFFER),
            block(newStruct(elements)),
            calculatedSize(0),
            bindingPos(bindingPos) {}

        void attachToShader(Shader shader, std::string name) {
            GLuint blockIdx = glGetUniformBlockIndex(shader.id, name.c_str());
            glUniformBlockBinding(shader.id, blockIdx, bindingPos);
        }

        void initNullData(GLenum usage) {
            if (!calculatedSize) {
                calculatedSize = calcSize();
            }
            glBufferData(type, calculatedSize, NULL, usage);
        }

        void bindRange(GLuint offset = 0) {
            if (!calculatedSize) {
                calculatedSize = calcSize();
            }
            glBindBufferRange(type, bindingPos, val, offset, calculatedSize);
        }

        unsigned int calcSize() {
            return block.calcPaddedSize();
        }

        void addElement(Element element) {
            block.list.push_back(element);
            if (element.baseAlign > block.baseAlign) {
                block.baseAlign = element.baseAlign;
            }
            block.length++;
        }

        // iteration variables
        GLuint offset;
        GLuint poppedOffset;
        std::vector<std::pair<unsigned int, Element*>> indexStack; // stack to keep track of the nested indices
        int currentDepth; // current size of the stack - 1

        // initialize iterator
        void startWrite() {
            currentDepth = 0;
            offset = 0;
            poppedOffset = 0;
            indexStack.clear();
            indexStack.push_back({ 0, &block });
        }

        // next element in iteration
        Element getNextElement() {
            // highest level struct popped, stack is empty
            if (currentDepth < 0) {
                return Type::INVALID;
            }

            // get current deepest array/struct (last element in the stack)
            Element* currentElement = indexStack[currentDepth].second;

            // get the element at the specified index within that iterable
            if (currentElement->type == Type::STRUCT) {
                currentElement = &currentElement->list[indexStack[currentDepth].first];
            }
            else { // array
                currentElement = &currentElement->list[0];
            }

            // traverse down to deepest array/struct
            while (currentElement->type == Type::STRUCT || currentElement->type == Type::ARRAY) {
                currentDepth++;
                indexStack.push_back({ 0, currentElement });
                currentElement = &currentElement->list[0];
            }

            // now have current element (not an iterable)
            // pop from stack if necessary
            poppedOffset = roundUpPow2(offset, currentElement->alignPow2()) + currentElement->calcSize();
            if (!pop()) {
                // no items popped
                poppedOffset = 0;
            }

            return *currentElement;
        }

        bool pop() {
            bool popped = false;

            for (int i = currentDepth; i >= 0; i--) {
                int advancedIdx = ++indexStack[i].first; // move cursor forward in the iterable
                if (advancedIdx >= indexStack[i].second->length) {
                    // iterated through entire array or struct
                    // pop iterable from the stack
                    poppedOffset = roundUpPow2(poppedOffset, indexStack[i].second->alignPow2());
                    indexStack.erase(indexStack.begin() + i);
                    popped = true;
                    currentDepth--;
                }
                else {
                    break;
                }
            }

            return popped;
        }

        template<typename T>
        void writeElement(T* data) {
            Element element = getNextElement();
            //std::cout << element.typeStr() << "--" << element.baseAlign << "--" << offset << "--";
            offset = roundUpPow2(offset, element.alignPow2());
            //std::cout << offset << std::endl;

            glBufferSubData(GL_UNIFORM_BUFFER, offset, sizeof(T), data);

            if (poppedOffset) {
                offset = poppedOffset;
            }
            else {
                offset += element.calcSize();
            }
        }

        template<typename T>
        void writeArray(T* first, unsigned int n) {
            for (int i = 0; i < n; i++) {
                writeElement<T>(&first[i]);
            }
        }

        template<typename T, typename V>
        void writeArrayContainer(T* container, unsigned int n) {
            for (int i = 0; i < n; i++) {
                writeElement<V>(&container->operator[](i)); // container[i] translates to container + i
            }
        }

        void advanceCursor(unsigned int n) {
            // skip number of elements
            for (int i = 0; i < n; i++) {
                Element element = getNextElement();
                offset = roundUpPow2(offset, element.alignPow2());
                if (poppedOffset) {
                    offset = poppedOffset;
                }
                else {
                    offset += element.calcSize();
                }
            }
        }

        void advanceArray(unsigned int noElements) {
            if (currentDepth < 0) {
                return;
            }

            Element* currentElement = indexStack[currentDepth].second;

            // get the next array
            if (currentElement->type == Type::STRUCT) {
                currentElement = &currentElement->list[indexStack[currentDepth].first];

                unsigned int depthAddition = 0;
                std::vector<std::pair<unsigned int, Element*>> stackAddition;

                // go to next array
                while (currentElement->type == Type::STRUCT) {
                    depthAddition++;
                    stackAddition.push_back({ 0, currentElement });
                    currentElement = &currentElement->list[0];
                }

                if (currentElement->type != Type::ARRAY) {
                    // did not find an array (reached primitive)
                    return;
                }

                // found array, apply changes
                currentDepth += depthAddition + 1; // + 1 for the array
                indexStack.insert(indexStack.end(), stackAddition.begin(), stackAddition.end());
                indexStack.push_back({ 0, currentElement }); // push array to stack
            }

            // at an array, advance number of elements
            unsigned int finalIdx = indexStack[currentDepth].first + noElements;
            unsigned int advanceCount = noElements;
            if (finalIdx >= indexStack[currentDepth].second->length) {
                // advance to the end of array
                advanceCount = indexStack[currentDepth].second->length - indexStack[currentDepth].first;
            }

            // advance offset
            offset += advanceCount * roundUpPow2(currentElement->list[0].calcSize(), currentElement->alignPow2());
            // advance cursor in stack
            indexStack[currentDepth].first += advanceCount;

            // pop from stack
            poppedOffset = offset;
            if (pop()) {
                // item(s) popped
                offset = poppedOffset;
            }
        }
    };
};

#endif