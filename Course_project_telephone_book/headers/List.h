#ifndef LIST_H
#define LIST_H

template <class T>
struct Node // структура элемента списка
{
    T info;
    Node<T>* next = nullptr;
    Node<T>* prev = nullptr;
};

template <class T>
class MyIterator
{
    Node<T>* mPointer;

public:
    MyIterator (Node<T>* pointer);

    Node<T>* getPointer();

    T& operator*() const;

    MyIterator& operator++();

    MyIterator& operator--();

    T& operator[](int step);

    bool operator==(const MyIterator& secondParam) const;

    bool operator!=(const MyIterator& secondParam) const;

    bool operator<(const MyIterator& secondParam) const;

    friend MyIterator<T> operator+(MyIterator<T> iter, int step)
    {
        for (int i = 0; i < step; i++)
            ++iter;
        return iter;
    }
};

template <class T>
class List
{
    Node<T>* head = nullptr;
    Node<T>* tail = nullptr;
public:
    void push(T element);
    void deleteElement(int index);
    MyIterator<T> begin();
    MyIterator<T> end();
    friend class MyIterator<T>;
};

template<class T>
Node<T>* MyIterator<T>::getPointer()
{
    return mPointer;
}

template <class T>
MyIterator<T>::MyIterator(Node<T>* pointer): mPointer(pointer){};

template <class T>
T& MyIterator<T>::operator*() const
{
    return mPointer->info;
}

template <class T>
MyIterator<T>& MyIterator<T>::operator++()
{
    if (!mPointer)
        return *this;
    mPointer=mPointer->next;
    return *this;
}

template <class T>
MyIterator<T>& MyIterator<T>::operator--()
{
    mPointer = mPointer->prev;
    return *this;
}

template<class T>
T &MyIterator<T>::operator[](int step)
{
    Node<T>* pointer = mPointer;
    for (int i = 0; i < step; i++)
        pointer = pointer->next;
    return pointer->info;
}

template <class T>
bool MyIterator<T>::operator==(const MyIterator& secondParam) const
{
    return mPointer == secondParam.mPointer;
}

template <class T>
bool MyIterator<T>::operator!=(const MyIterator& secondParam) const
{
    return mPointer != secondParam.mPointer;
}

template <class T>
bool MyIterator<T>::operator<(const MyIterator& secondParam) const
{
    return mPointer->info < secondParam.mPointer->info;
}


/////////////////////////////////////////////////

template<class T>
void List<T>::push(T element) // добавление элемента в список
{
    Node<T>* elem = new Node<T>;
    elem->info = element;
    if (!head)
    {
        head = tail = elem; // если список пустой, установка головы и хвоста
        head->next = tail->next;
    }
    else
    {
        tail->next = elem; // переопределение хвоста
        elem->prev = tail;
        tail = elem;
    }
}

template<class T>
void List<T>::deleteElement(int index) // удаление элемента из списка
{
    if (!head)
        return;
    Node<T>* temp;
    MyIterator<T> it = head;
    for (int i = 0; i < index - 1; ++it, i++); // поиск эл-та для удаления
    temp = it.getPointer(); // получение указателя с информацией в итераторе
    if (index > 0)// если элемент не первый
    {
        Node<T>* temp2 = temp->next;
        if (temp2 == tail) // если удааляемый элемент последний
            tail = tail->prev; // сдвиг хвоста
        else
            temp2->next->prev = temp; // перепривязка указателей
        temp->next = temp->next->next;
        delete temp2;
    }
    else
    {
        if (head == tail) // если элемент один в списке
            tail = head = nullptr;
        else
        {
            head = head->next; // сдвиг головы
            if (head) // если удаляемый элемент не последний
                head->prev = nullptr;
        }
        delete temp;
    }
}

template<class T>
MyIterator<T> List<T>::begin()
{
    return MyIterator(head);
}

template<class T>
MyIterator<T> List<T>::end()
{
    return MyIterator(tail);
}

//////////////////////////////

namespace algo
{
    template<class T>
    void swap(T& data1, T& data2) // обмен значений
    {
        T data3 = data1;
        data1 = data2;
        data2 = data3;
    }

    template<class T>
    void sort(MyIterator<T> first, MyIterator<T> last, bool (*comparator)(T, T)) // сортировка вставками
    {
        int count = 1;
        for (int i = 0; first[i] != *last; i++)
            count++;
        for (int i = 1; i < count; i++)
            for (int j = i; j > 0 && comparator(first[j - 1], first[j]); j--)
                swap(first[j - 1], first[j]);
    }

    template<class T>
    void editElement(MyIterator<T>& iter, T substitute)
    {
        iter.getPointer()->info = substitute;
    }
}

#endif // LIST_H
