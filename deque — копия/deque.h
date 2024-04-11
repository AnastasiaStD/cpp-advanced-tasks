#pragma once

#include <initializer_list>
#include <algorithm>
#include <array>

const int kBlockSize = 128;

class Deque {
public:
    struct Block {
        std::array<int, kBlockSize> block_{0};
        int end_ = 0;
        int begin_ = 0;
        size_t num_elem_ = 0;

        Block() {
            block_ = std::array<int, kBlockSize>{0};
            end_ = 0;
            begin_ = 0;
            num_elem_ = 0;
        }

        bool IsFull() {
            return (num_elem_ == kBlockSize);
        }

        bool IsEmpty() {
            return (num_elem_ == 0);
        }

        int IndexCheaker(int id) {
            if (id >= 0) {
                return id % kBlockSize;
            } else {
                return (id + kBlockSize) % kBlockSize;
            }
        }

        void PushBackBlock(int value) {
            if (num_elem_ == 0) {
                block_[end_] = value;
                begin_ = kBlockSize;
            } else {
                ++end_;
                end_ = IndexCheaker(end_);
                begin_ = IndexCheaker(begin_);
                block_[end_] = value;
            }
            ++num_elem_;
        }

        void PushFrontBlock(int value) {
            if (num_elem_ == 0) {
                begin_ = kBlockSize - 1;
                block_[begin_] = value;
                end_ = -1;
            } else {
                --begin_;
                end_ = IndexCheaker(end_);
                begin_ = IndexCheaker(begin_);
                block_[begin_] = value;  // присваиваем валью текущему концу блока
            }

            ++num_elem_;
        }

        void PopBackBlock() {
            if (num_elem_ == 1) {
                end_ = 0;
                begin_ = 0;
                block_[end_] = 0;
            } else {
                end_ = IndexCheaker(end_);
                block_[end_] = 0;
                --end_;
                end_ = IndexCheaker(end_);
            }
            --num_elem_;
        };

        void PopFrontBlock() {
            if (num_elem_ == 1) {
                block_[begin_] = 0;
                end_ = 0;
                begin_ = 0;
            } else {
                begin_ = IndexCheaker(begin_);
                block_[begin_] = 0;
                ++begin_;
                begin_ = IndexCheaker(begin_);
            }
            --num_elem_;
        }
    };

    Deque() {
        Block* block = new Block();
        buffer_ = new Block*[1];
        buffer_[0] = block;
        num_of_end_block_ = num_of_begin_block_ = 0;
        begin_ = buffer_[num_of_begin_block_]->begin_;
        end_ = buffer_[num_of_end_block_]->end_;
        numder_of_blocks_ = 1;
        used_blocks_ = 0;
    }

    Deque(const Deque& rhs) {
        buffer_ = new Block*[rhs.numder_of_blocks_];
        for (int i = 0; i < rhs.numder_of_blocks_; ++i) {
            buffer_[i] = new Block(*rhs.buffer_[i]);
        }
        num_of_begin_block_ = rhs.num_of_begin_block_;
        num_of_end_block_ = rhs.num_of_end_block_;
        numder_of_blocks_ = rhs.numder_of_blocks_;
        begin_ = buffer_[num_of_begin_block_]->begin_;
        end_ = buffer_[num_of_end_block_]->end_;
    };
    Deque(Deque&& rhs) {
        buffer_ = rhs.buffer_;
        num_of_begin_block_ = rhs.num_of_begin_block_;
        num_of_end_block_ = rhs.num_of_end_block_;
        numder_of_blocks_ = rhs.numder_of_blocks_;
        begin_ = rhs.begin_;
        end_ = rhs.end_;
        rhs.buffer_ = nullptr;
        rhs.numder_of_blocks_ = 0;
    };

    ~Deque() {
        for (int i = 0; i < numder_of_blocks_; ++i) {
            delete buffer_[i];
        }
        delete[] buffer_;
    }

    explicit Deque(size_t size) : Deque() {
        while (kBlockSize * numder_of_blocks_ <= size) {
            MoreMemory();
        }
        num_of_begin_block_ = num_of_end_block_ = 0;
        used_blocks_ = 0;
        begin_ = buffer_[0]->begin_;
        end_ = buffer_[0]->end_;
        for (size_t i = 0; i < size; ++i) {
            PushBack(0);
        }
        begin_ = (buffer_[num_of_begin_block_]->block_[0]);
        end_ = buffer_[num_of_end_block_]->block_[0] + buffer_[num_of_end_block_]->num_elem_;
    }

    Deque(std::initializer_list<int> list) : Deque() {
        while (kBlockSize * numder_of_blocks_ <= list.size()) {
            MoreMemory();
        }
        num_of_begin_block_ = num_of_end_block_ = 0;
        used_blocks_ = 0;
        begin_ = buffer_[num_of_begin_block_]->begin_;
        end_ = buffer_[num_of_end_block_]->end_;
        for (auto i : list) {
            PushBack(i);
        }
        begin_ = buffer_[num_of_begin_block_]->begin_;
        end_ = buffer_[num_of_end_block_]->end_;
    }

    Deque& operator=(const Deque& rhs) {
        if (this == &rhs) {
            return *this;
        }
        for (int i = 0; i < numder_of_blocks_; ++i) {
            delete buffer_[i];
        }
        delete[] buffer_;
        buffer_ = new Block*[rhs.numder_of_blocks_];
        for (int i = 0; i < rhs.numder_of_blocks_; ++i) {
            buffer_[i] = new Block(*rhs.buffer_[i]);
        }
        num_of_begin_block_ = rhs.num_of_begin_block_;
        num_of_end_block_ = rhs.num_of_end_block_;
        numder_of_blocks_ = rhs.numder_of_blocks_;
        begin_ = buffer_[num_of_begin_block_]->begin_;
        end_ = buffer_[num_of_end_block_]->end_;
        return *this;
    }

    void Swap(Deque& rhs) {
        std::swap(rhs.buffer_, buffer_);
        std::swap(rhs.numder_of_blocks_, numder_of_blocks_);
        std::swap(rhs.num_of_begin_block_, num_of_begin_block_);
        std::swap(rhs.num_of_end_block_, num_of_end_block_);

        rhs.begin_ = rhs.buffer_[num_of_begin_block_]->begin_;
        begin_ = buffer_[num_of_begin_block_]->begin_;

        rhs.end_ = rhs.buffer_[num_of_end_block_]->end_;
        end_ = buffer_[num_of_end_block_]->end_;
    }

    void PushBack(int value) {
        if (!buffer_[CorrectIndex(num_of_end_block_) % numder_of_blocks_]
                 ->IsFull()) {  // если в последнем блоке есть место
            if (used_blocks_ == 0) {  // если пушим впервые, то + 1 к используемым блокам
                used_blocks_ = 1;
            }
            buffer_[CorrectIndex(num_of_end_block_) % numder_of_blocks_]->PushBackBlock(value);
        } else {
            if (used_blocks_ < numder_of_blocks_) {  // если еще есть пустые блоки
                ++num_of_end_block_;
                buffer_[CorrectIndex(num_of_end_block_) % numder_of_blocks_]->PushBackBlock(value);
                ++used_blocks_;
            } else {
                MoreMemory();
                ++num_of_end_block_;
                num_of_end_block_ = CorrectIndex(num_of_end_block_) % numder_of_blocks_;
                buffer_[CorrectIndex(num_of_end_block_) % numder_of_blocks_]->PushBackBlock(value);
                end_ = buffer_[num_of_end_block_]->end_;
                ++used_blocks_;
            }
        }
    }

    int CorrectIndex(int ind) const {
        if (ind >= 0) {
            return ind;
        } else {
            return numder_of_blocks_ + ind;
        }
    }
    void MoreMemory() {
        Block** new_array = new Block*[numder_of_blocks_ * 2];
        if (this->Size() <= kBlockSize) {  // один блок???
            new_array[0] = buffer_[num_of_begin_block_];
        } else {
            int k = 0;
            for (int i = num_of_begin_block_;
                 CorrectIndex(i % numder_of_blocks_) != ((num_of_end_block_) % numder_of_blocks_);
                 ++i) {
                new_array[k] = buffer_[CorrectIndex(i % numder_of_blocks_)];
                k += 1;
            }
            new_array[k] = buffer_[num_of_end_block_];
        }
        for (int j = numder_of_blocks_; j < numder_of_blocks_ * 2; ++j) {
            Block* new_block = new Block();
            new_array[j] = new_block;
        }

        delete[] buffer_;
        num_of_begin_block_ = 0;
        num_of_end_block_ = numder_of_blocks_ - 1;
        numder_of_blocks_ = numder_of_blocks_ * 2;
        buffer_ = new_array;
        begin_ = buffer_[num_of_begin_block_]->begin_;
        end_ = buffer_[num_of_end_block_]->end_;
    }

    void PopBack() {
        buffer_[num_of_end_block_]->PopBackBlock();
        if (buffer_[CorrectIndex(num_of_end_block_)]->num_elem_ == 0) {
            if (used_blocks_ == 1) {
                numder_of_blocks_ = 1;
                used_blocks_ = 0;
                num_of_begin_block_ = 0;
                num_of_end_block_ = 0;
                begin_ = end_ = 0;
            } else {
                Block** new_array = new Block*[CorrectIndex(numder_of_blocks_ - 1)];
                int k = 0;
                for (int i = num_of_begin_block_; (i % numder_of_blocks_) != num_of_end_block_;
                     ++i) {
                    new_array[k] = buffer_[i % numder_of_blocks_];
                    ++k;
                }

                for (int i = (num_of_end_block_ + 1) % numder_of_blocks_;
                     CorrectIndex(i % numder_of_blocks_) != num_of_begin_block_; ++i) {
                    new_array[k] = buffer_[i % numder_of_blocks_];
                    ++k;
                }

                delete buffer_[num_of_end_block_];
                delete[] buffer_;

                --numder_of_blocks_;
                --used_blocks_;
                num_of_end_block_ = used_blocks_ - 1;
                num_of_begin_block_ = 0;
                buffer_ = new_array;
                begin_ = buffer_[num_of_begin_block_]->begin_;
                end_ = buffer_[num_of_end_block_]->end_;
            }
        } else {
            end_ = buffer_[num_of_end_block_]->end_;
        }
    }

    void PushFront(int value) {
        if (!buffer_[CorrectIndex(num_of_begin_block_) % numder_of_blocks_]->IsFull()) {
            if (used_blocks_ == 0) {
                used_blocks_ = 1;
            }
            buffer_[CorrectIndex(num_of_begin_block_) % numder_of_blocks_]->PushFrontBlock(value);
            begin_ = buffer_[CorrectIndex(num_of_begin_block_) % numder_of_blocks_]->begin_;
        } else {
            if (used_blocks_ < numder_of_blocks_) {
                num_of_begin_block_ = CorrectIndex(--num_of_begin_block_) % numder_of_blocks_;
                buffer_[CorrectIndex(num_of_begin_block_) % numder_of_blocks_]->PushFrontBlock(
                    value);
                begin_ = buffer_[CorrectIndex(num_of_begin_block_) % numder_of_blocks_]->begin_;
                ++used_blocks_;
            } else {
                MoreMemory();
                num_of_begin_block_ = CorrectIndex(numder_of_blocks_ - 1) % numder_of_blocks_;
                buffer_[CorrectIndex(num_of_begin_block_) % numder_of_blocks_]->PushFrontBlock(
                    value);
                begin_ = buffer_[num_of_begin_block_]->begin_;
                ++used_blocks_;
            }
        }
    }

    void PopFront() {
        buffer_[CorrectIndex(num_of_begin_block_)]->PopFrontBlock();
        if (buffer_[CorrectIndex(num_of_begin_block_)]->num_elem_ == 0) {
            if (used_blocks_ == 1) {
                numder_of_blocks_ = 1;
                used_blocks_ = 0;
                num_of_begin_block_ = 0;
                num_of_end_block_ = 0;
                begin_ = end_ = 0;
            } else {
                Block** new_array = new Block*[CorrectIndex(numder_of_blocks_ - 1)];
                int k = 0;
                for (int i = CorrectIndex(num_of_begin_block_ + 1);
                     (i % numder_of_blocks_) != (num_of_end_block_); ++i) {
                    new_array[k] = buffer_[CorrectIndex(i % numder_of_blocks_)];
                    ++k;
                }

                for (int i = (num_of_end_block_); (i % numder_of_blocks_) != num_of_begin_block_;
                     ++i) {
                    new_array[k] = buffer_[CorrectIndex(i % numder_of_blocks_)];
                    ++k;
                }

                delete buffer_[num_of_begin_block_];
                delete[] buffer_;

                --numder_of_blocks_;
                --used_blocks_;
                num_of_end_block_ = CorrectIndex(used_blocks_ - 1);
                num_of_begin_block_ = 0;
                buffer_ = new_array;
                end_ = buffer_[num_of_end_block_]->end_;
                begin_ = buffer_[num_of_begin_block_]->begin_;
            }
        } else {
            begin_ = buffer_[num_of_begin_block_]->begin_;
        }
    }

    int& operator[](size_t ind) {
        int n = buffer_[num_of_begin_block_]->num_elem_;
        if (ind < n) {
            auto i = ((buffer_[num_of_begin_block_]->begin_) + ind) % kBlockSize;
            return buffer_[num_of_begin_block_]->block_[i];
        }
        int id = ind - n;
        int num_block = CorrectIndex(id / kBlockSize + num_of_begin_block_ + 1) % numder_of_blocks_;
        int ind_i_block = CorrectIndex(id % kBlockSize);
        auto i = (buffer_[num_block]->begin_ + ind_i_block) % kBlockSize;
        return buffer_[num_block]->block_[i];
    }

    int operator[](size_t ind) const {
        int n = buffer_[num_of_begin_block_]->num_elem_;
        if (ind < n) {
            auto i = ((buffer_[num_of_begin_block_]->begin_) + ind) % kBlockSize;
            return buffer_[num_of_begin_block_]->block_[i];
        }
        int id = ind - n;
        int num_block = CorrectIndex(id / kBlockSize + num_of_begin_block_ + 1) % numder_of_blocks_;
        int ind_i_block = CorrectIndex(id % kBlockSize);
        auto i = (buffer_[num_block]->begin_ + ind_i_block) % kBlockSize;
        return buffer_[num_block]->block_[i];
    }

    size_t Size() const {
        size_t s = 0;
        for (size_t i = 0; i < numder_of_blocks_; ++i) {
            s += buffer_[i]->num_elem_;
        }
        return s;
    }

    void Clear() {
        for (int i = 0; i < numder_of_blocks_; ++i) {
            buffer_[i]->begin_ = 0;
            buffer_[i]->end_ = 0;
            buffer_[i]->num_elem_ = 0;
            buffer_[i]->block_ = std::array<int, kBlockSize>{0};
        }
        num_of_end_block_ = num_of_begin_block_ = 0;
        begin_ = buffer_[num_of_begin_block_]->begin_;
        end_ = buffer_[num_of_end_block_]->end_;
        numder_of_blocks_ = 1;
        used_blocks_ = 1;
    }

private:
    Block** buffer_ = nullptr;  // указатель на массив указателей на блоки
    int num_of_end_block_;         // какой блок сейчас первый
    int num_of_begin_block_;       // какой блок сейчас последний
    int begin_;                    // первая позицию в деке
    int end_;                      // последняя позиция в деке
    size_t numder_of_blocks_ = 1;  // количество блоков
    size_t used_blocks_ = 0;
};
