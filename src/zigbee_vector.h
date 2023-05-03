// include guard for save inclusion
#ifndef ZIGBEE_VECTOR_H
#define ZIGBEE_VECTOR_H

// you will need the uncomment this include if you want to use this library in normal C++ (for using the (u)intX_t
// datatypes)
// #include <cstdint>

// vector class

// template for a variety of datatypes (int, char, String, custom classes)
namespace zigbee_protocol {
template <typename T> class Vector {
public:
    // default constructor
    Vector<T>() { }
    /**
     *@brief constructor with a length and a placeholder
     *@param i_size: this is the length of the vector you want the start with
     *@param i_placeholder: the whole vector is filled up with this value...
     */
    Vector<T>(uint16_t i_size, T i_placeholder)
    {
        // create a temporary heap array with the specified length
        T* temp = new T[i_size];

        // loop through the temporary heap array and set each element equal to the placeholder
        for (uint16_t i = 0; i < i_size; i++) {
            temp[i] = i_placeholder;
        }
        // set the address 'array' is pointing to equal to the address 'temp' is pointing to
        array = temp;

        // set the size (or length) of the vector equal the the constructor size
        _size = i_size;
    }

    // default destructor
    ~Vector<T>() = default;

public:
    /**
     *@brief a function that returns a reference of an element of 'array' at a spesific position (index) of 'array'
     *@param index: this is the index of 'array' you want get access to...
     *@return the function returns a reference of an element of 'array'. that way you can either read out the element or
     *write to it
     */
    T& at(uint16_t index)
    {
        // if the specified index is a valid element of 'array' just return it
        if (index < _size) {
            return array[index];
        }
        // if the specified index to greater than the last index of the last element return the last element of 'array'
        else {
            return array[_size - 1];
        }
    }

    /**
     *@brief this function adds an element at the last position (index) of 'array'
     *@param data: this is the element you want to append the the vector
     */
    void push_back(T data)
    {
        // if there is already one or more elements on 'array'
        if (_size > 0) {
            // create a temporary heap array with one more element than the current 'array'
            T* temp = new T[_size + 1];

            // copy all existing elements from 'array' to 'temp'
            for (uint16_t i = 0; i < _size; i++) {
                temp[i] = array[i];
            }
            // set the last element of 'temp' equal to the value of 'data'
            temp[_size] = data;

            // delete the values of 'array'
            delete[] array;

            // set the address 'array' is pointing to equal the the address 'temp' is pointing to
            array = temp;
        }
        // if 'array' has no elements in it create one
        else {
            // just allocate a one element large array of 'T' and fill it up with the value in 'data'
            array = new T[1] { data };
        }
        // either way increment the size (or length) of the vector
        _size++;
    }

    /**
     *@brief this function inserts an element at a specific index
     *@param index: this is the position the new element will be placed at
     *@param data: this is the element inserted
     */
    void insert(uint16_t index, T data)
    {
        // test if the index is in range of the vector
        if (index <= _size) {
            // create a temporary heap array with one more element than the current 'array'
            T* temp = new T[_size + 1];

            // copy all values the the temporary 'array' to 'temp'. stop at index
            for (uint16_t i = 0; i < index; i++) {
                temp[i] = array[i];
            }
            // insert the new element in 'temp' at desired index
            temp[index] = data;

            // copy the remaining values from 'array' to 'temp'
            for (uint16_t i = index + 1; i < _size + 1; i++) {
                temp[i] = array[i - 1];
            }
            // delete the values of 'array'
            delete[] array;

            // set the address 'array' is pointing to equal the the address 'temp' is pointing to
            array = temp;

            // increment the size of the vector
            _size++;
        }
    }

    /**
     *@brief this function inserts a vector at a specific index
     *@param index: this is the (first) position the new vector will be placed at
     *@param data: this is the vector inserted
     *@Note this function is similar the the function above
     */
    void insert(uint16_t index, Vector<T> data)
    {
        if (index <= _size && data.size() > 0) {
            T* temp = new T[_size + data.size()];

            for (uint16_t i = 0; i < index; i++) {
                temp[i] = array[i];
            }
            for (uint16_t i = index; i < index + data.size(); i++) {
                temp[i] = data.at(i - index);
            }
            for (uint16_t i = index + data.size(); i < _size + data.size(); i++) {
                temp[i] = array[i - data.size()];
            }

            delete[] array;

            array = temp;

            _size += data.size();
        }
    }
    /**
     *@brief this function erases a element at a specific index
     *@param index: this is the position the element will be erased
     */

    void erase(uint16_t index)
    {
        // test if the index is in range of the vector
        if (index < _size) {
            // create a temporary heap array with one more element than the current 'array'
            T* temp = new T[_size - 1];

            // copy all values the the temporary 'array' to 'temp'. stop at index
            for (uint16_t i = 0; i < index; i++) {
                temp[i] = array[i];
            }
            // jump over the value at index

            // copy the remaining values from 'array' to 'temp'
            for (uint16_t i = index + 1; i < _size; i++) {
                temp[i - 1] = array[i];
            }
            // delete the values of 'array'
            delete[] array;

            // set the address 'array' is pointing to equal the the address 'temp' is pointing to
            array = temp;

            // decrement the size of the vector
            _size--;
        }
    }

    /**
     *@brief this function erases a element at a specific index
     *@param min_index: this is the first position the element will be erased
     *@param max_index: this is the last position the element will be erased
     *@Note this function is similar the the function above
     */
    void erase(uint16_t min_index, uint16_t max_index)
    {
        if (max_index < _size && min_index <= max_index) {
            T* temp = new T[_size - 1];

            for (uint16_t i = 0; i < min_index; i++) {
                temp[i] = array[i];
            }
            for (uint16_t i = min_index; i < _size - (max_index - min_index); i++) {
                temp[i] = array[i - min_index + max_index + 1];
            }

            delete[] array;

            array = temp;

            _size -= max_index - min_index + 1;
        }
    }

    // this function will erase the last element of the vector
    void pop_back()
    {
        // test if the vector has at least 1 element
        if (_size > 0) {
            // create a temporary heap array with one more element than the current 'array'
            T* temp = new T[_size - 1];

            // copy all values except the very last the the temporary 'array' to 'temp'
            for (uint16_t i = 0; i < _size - 1; i++) {
                temp[i] = array[i];
            }
            // delete the values of 'array'
            delete[] array;

            // set the address 'array' is pointing to equal the the address 'temp' is pointing to
            array = temp;

            // decrement the size of the vector
            _size--;
        }
    }
    // instead of using the 'at()' function you can also access elements like you would in an array
    T& operator[](uint16_t index) { return at(index); }

    T* data()
    {
        return array;
    }

    // this function erases every element in the array. mainly to free up storage
    void clear()
    {
        if (_size > 0) {
            delete[] array;
            _size = 0;
        }
    }
    // this function returns the current size (or length) of the vector
    uint16_t size() const { return _size; }

private:
    /*
    this the the array of the template datatype 'T'.
    it's the core element of the vector


    how the heap array is working:

      when you create a heap array, the computer first assigns you a free address within the heap.
      if you later want to add elements to the array, you have to "ask" the computer for space in the heap.
      note that you can only allocate space once.
      so if you need more or less space or elements in the array,
      you first have to delete all elements of the array to set a new length.

      that was just a very brief explanation of how this works
    */
    T* array;

    // this is the size (or length) of the vector respectively the array
    uint16_t _size = 0;
};
}

#endif // ZIGBEE_VECTOR_H
